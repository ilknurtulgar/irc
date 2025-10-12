/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 16:19:21 by itulgar           #+#    #+#             */
/*   Updated: 2025/10/12 19:46:41 by zayaz            ###   ########.fr       */
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

	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    	delete it->second;

	channels.clear();

}


void Server::run()
{
	setupServer();
	 while(g_run)
	 {
		setPoll();
	}
	return;
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
	memset(fds,0,sizeof(fds));
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
	ssize_t byteRead = recv(clientSocketFd, buffer, BUFFER_SIZE - 1, 0);
	if(byteRead < 0)
	{
		perror("recv failed");
		return;
	}
	else if (byteRead == 0)
	{
		std::cout << "Client disconnect: " << clientSocketFd << std::endl;

		std::map<int, Client*>::iterator it = clients.find(clientSocketFd);

		if (it != clients.end())
		{
			Client* clientToDestroy = it->second;
			// Build a QUIT message so other users get notified and channel lists are updated
			std::string host = clientToDestroy->getHostName().empty() ? "localhost" : clientToDestroy->getHostName();
			std::string quitMsg = ":" + clientToDestroy->getNickName() + "!" + clientToDestroy->getUserName() + "@" + host + " QUIT :Client disconnected\r\n";

			// Remove client from channels (this broadcasts the quit/part as needed)
			removeClient(clientSocketFd, quitMsg);

			// removeClient will now delete and erase the client from the clients map,
			// so do not delete/erase here to avoid double-free.
		}
		if (clientSocketFd >= 0)
			close(clientSocketFd);

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
	
	// Pass raw bytes to client's incoming handler which will accumulate and
	// process only complete CRLF/LF terminated lines.
	clients[clientSocketFd]->handleIncoming(buffer, byteRead);
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
		std::string errorMsg = ":server 473 " + channelName + " :Cannot join channel (+i)\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.length(), 0);
        return;
	}
	channel->addUser(client);
	channel->removeInvite(client);

	std::string joinMsg = ":" + client->getNickName() + " JOIN " + channelName + "\r\n";
	send(client->getFd(),joinMsg.c_str(),joinMsg.length(),0);
	channel->broadcast(joinMsg, client);

	// Send existing users' JOIN and MODE messages to the joining client so clients
	// that rely on JOIN events (e.g., KVIrc) will populate their user list.
	std::map<int, Client *> &users = channel->getUsers();
	for (std::map<int, Client *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		Client *other = it->second;
		if (other == client)
			continue;
		std::string otherJoin = ":" + other->getNickName() + "!~" + other->getUserName() + "@localhost JOIN " + channelName + "\r\n";
		send(client->getFd(), otherJoin.c_str(), otherJoin.length(), 0);
		if (channel->isOperator(other))
		{
			std::string modeMsg = "MODE " + channelName + " +o " + other->getNickName() + "\r\n";
			send(client->getFd(), modeMsg.c_str(), modeMsg.length(), 0);
		}
	}

	// If the joining client is an operator (e.g., they created the channel),
	// inform them with a MODE +o message so clients like KVIrc display operator status.
	if (channel->isOperator(client))
	{
		std::string selfMode = "MODE " + channelName + " +o " + client->getNickName() + "\r\n";
		send(client->getFd(), selfMode.c_str(), selfMode.length(), 0);
	}

	// After that, send the channel topic (if any) and NAMES list to the joining client
	std::string topic = channel->getTopic();
	std::string topicMsg;
	if (topic.empty())
		topicMsg = ":localhost 331 " + client->getNickName() + " " + channelName + " :No topic is set\r\n";
	else
		topicMsg = ":localhost 332 " + client->getNickName() + " " + channelName + " :" + topic + "\r\n";
	send(client->getFd(), topicMsg.c_str(), topicMsg.length(), 0);

	std::string nickList = channel->getNickList();
	std::string namesMsg = ":353 " + client->getNickName() + " = " + channelName + " :" + nickList + "\r\n";
	send(client->getFd(), namesMsg.c_str(), namesMsg.length(), 0);

	std::string endMsg = ":localhost 366 " + client->getNickName() + " " + channelName + " :End of /NAMES list\r\n";
	send(client->getFd(), endMsg.c_str(), endMsg.length(), 0);
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

        std::string endMsg = ":localhost 366  " + client->getNickName() + " " 
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
	// For each channel the client is in, send a PART message so clients like KVIrc
	// update their channel user lists, then remove the user from the channel.
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); )
	{
		Channel* channel = it->second;
		if (channel->findUser(client)){
			// Build PART message: :nick!user@host PART #channel :reason\r\n
			std::string host = client->getHostName().empty() ? "localhost" : client->getHostName();
			// Extract a short reason from the provided message if possible
			std::string reason = "";
			std::string msg = message;
			// message may be like ":nick!user@host QUIT :reason\r\n"
			size_t pos = msg.find(" QUIT :");
			if (pos != std::string::npos)
			{
				reason = msg.substr(pos + 7);
				// strip trailing CRLF if present
				if (!reason.empty() && reason.size() >= 2 && reason.substr(reason.size() - 2) == "\r\n")
					reason = reason.substr(0, reason.size() - 2);
			}

			std::string partMsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + host + " PART " + it->first;
			if (!reason.empty())
				partMsg += " :" + reason;
			partMsg += "\r\n";

			channel->broadcast(partMsg, client);
			// Also, send PART to the client itself if still connected
			send(client->getFd(), partMsg.c_str(), partMsg.length(), 0);

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

	// After per-channel PARTs, broadcast a global QUIT to all remaining clients so
	// they see the user's disconnection as well.
	for (std::map<int, Client*>::iterator cit = clients.begin(); cit != clients.end(); ++cit)
	{
		if (cit->first == clientSocketFd) continue;
		send(cit->first, message.c_str(), message.length(), 0);
	}

	std::cout << "INFO: Client fd=" << clientSocketFd << " (" << client->getNickName() << ") removed from server map." << std::endl;
	
}

std::map<std::string, Channel*>& Server::getChannels() {
    return channels;
}

bool Server::isNickInUse(const std::string& nick) {
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second && it->second->getNickName() == nick)
			return true;
	}
	return false;
}