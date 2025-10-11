/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 16:19:21 by itulgar           #+#    #+#             */
/*   Updated: 2025/10/09 17:30:03 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

Server::Server(int port, std::string password) : port(port), password(password){}

Server::~Server()
{
	std::map<int, Client*>::iterator it;
	for(it = clients.begin(); it != clients.end(); ++it)
		delete it->second;
	clients.clear();
	if (serverSocketFd >= 0)
		close(serverSocketFd);
}


void Server::run()
{
	setupServer();
	 while(g_run)
	 {
		setPoll();
	}
	std::cout << "durdum" << std::endl;
	exit(0);
}

void Server::setupServer()
{
	serverSocketFd= socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	setsockopt(serverSocketFd,SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(serverSocketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if(listen(serverSocketFd, SOMAXCONN) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	
	int flag = fcntl(serverSocketFd, F_GETFL, 0);
	if(flag < 0)
	{
		perror("fcntl F_getfl");
		exit(EXIT_FAILURE);
	}
	flag |= O_NONBLOCK;
	if(fcntl(serverSocketFd, F_SETFL, flag) < 0)
	{
		perror("fcntl F_setfl");
		exit(EXIT_FAILURE);
	}	
}


void Server::setPoll()
{
	struct pollfd fds[MAX_CLIENTS];
	int nfds = 0;
	fds[nfds].fd =serverSocketFd;
	fds[nfds].events = POLLIN;
	nfds++;

	std::map<int, Client*>::iterator it;
	for(it = clients.begin(); it !=clients.end(); ++it)
	{
		if(nfds >= MAX_CLIENTS)
			break;
		fds[nfds].fd = it->first;
		fds[nfds].events = POLLIN;
		nfds++;
	}

	int ret = poll(fds, nfds, -1);
	if(ret < 0 && g_run) {
		perror("Poll failed");
        exit(EXIT_FAILURE);
	}
	
	for( int i = 0; i < nfds; i++)
	{
		if(fds[i].revents & POLLIN)
		{
			if(fds[i].fd== serverSocketFd)
				acceptNewClient();
			else
				recvClientData(fds[i].fd); 
		}
	}
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddrr;
	socklen_t clientAdrrLen = sizeof(clientAddrr);
	int newClientSocketFd = accept(serverSocketFd,(struct sockaddr*) &clientAddrr, &clientAdrrLen);
	if (newClientSocketFd < 0)
    {
        perror("accept failed");
        return;
    }
		
	int flag = fcntl(newClientSocketFd, F_GETFL, 0);
	if(flag < 0)
	{
		perror("fcntl F_getfl");
		
		exit(EXIT_FAILURE);
	}
	flag |= O_NONBLOCK;
	if(fcntl(newClientSocketFd, F_SETFL, flag) < 0)
	{
		perror("fcntl F_setfl");
		exit(EXIT_FAILURE);
	}
	
	Client* newClient= new Client(newClientSocketFd, password,this);
	clients[newClientSocketFd] = newClient;
	
	std::cout << "IRC Server listening on port " << port << std::endl;
	std::cout << "New client connected: " << newClientSocketFd << std::endl;
	
}

void Server::recvClientData(int clientSocketFd)
{
	char buffer[BUFFER_SIZE];
	size_t byteRead = recv(clientSocketFd, buffer, BUFFER_SIZE - 1, 0);
	if(byteRead < 0)
	{
		perror("recv failed");
		return;
	}
	else if (byteRead == 0)
	{
		std::cout << "Client dissconnect: " << clientSocketFd << std::endl;
		return;
	}

	if(byteRead > 510){
		std::string errorMsg = "ERROR :Line too long. Max 512 bytes allowed per message.\r\n";
		send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
		std::cout << "Client " << clientSocketFd << " sent oversized message (" << byteRead << " bytes). Disconnecting." << std::endl;
		close(clientSocketFd);
        delete clients[clientSocketFd];
        clients.erase(clientSocketFd);
		return;
	}
	
	buffer[byteRead] = '\0';
	std::string receiveData(buffer);
	
	clients[clientSocketFd]-> handleCommand(receiveData);
}

bool Server::isChannel(const std::string &name){
	if(channels.find(name) == channels.end())
		return false;
	return true;
}

void Server::checkChannel(Client *client,const std::string& channelName){
	Channel *channel = NULL;

	if(isChannel(channelName))
		channel = channels[channelName];
	else{
		channel = new Channel(channelName);
		channels[channelName] = channel;
	}

	if(channel->isInviteOnly()  && !channel->isInvited(client)){
		std::string errorMsg = "473 " + client->getNickName() + " " + channelName + " :Cannot join channel (+i)\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.length(), 0);
        return;
	}
	channel->addUser(client);
	channel->removeInvite(client);

	std::string joinMsg = ":" + client->getNickName() + " JOIN " + channelName + "\r\n";
	send(client->getFd(),joinMsg.c_str(),joinMsg.length(),0);
	channel->broadcast(joinMsg, client);
}

Client* Server::getClientNick(std::string& nick){

    for (std::map<int,Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
		if(it->second->getNickName() == nick)
			return it->second;
	}
	return NULL;
}

Channel* Server::getChannel(std::string& channel){

	std::map<std::string, Channel*>::iterator it = channels.find(channel);
	if(it != channels.end())
		return it->second;
	return NULL;
	
}

void Server::singleNames(Client *client){
	for(std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it){
		if(it->second->whereNames(client)){
			
		std::string msg = ":353 " +client->getNickName() + " = " 
                        + it->first + " :" + it->second->getNickList() + "\r\n";
        send(client->getFd(), msg.c_str(), msg.length(), 0);

        std::string endMsg = ":366 " + client->getNickName() + " " 
                           + it->first + " :End of /NAMES list\r\n";
        send(client->getFd(), endMsg.c_str(), endMsg.length(), 0);
		}
	}
}

void Server::removeChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        delete it->second;
        channels.erase(it);
    }
}

void Server::removeClient(int clientSocketFd, const std::string& message)
{
    std::map<int, Client*>::iterator it = clients.find(clientSocketFd);
    if (it == clients.end())
        return;
    Client* client = it->second;
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); )
    {
        Channel* channel = it->second;
        if (channel->findUser(client)){
            channel->broadcast(message, client);
            channel->removeUser(client);
            if (channel->getUsers().empty())
            {
                std::cout << "INFO: Channel " << it->first << " deleted (empty) after QUIT" << std::endl;
                delete channel;
                std::map<std::string, Channel*>::iterator toErase = it++;
				channels.erase(toErase);
                continue;
            }
        }
        ++it;
    }
    clients.erase(it);
    std::cout << "INFO: Client fd=" << clientSocketFd << " (" << client->getNickName() << ") removed from server map." << std::endl;

}

std::map<std::string, Channel*>& Server::getChannels() {
    return channels;
}
