/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:13:56 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 15:11:31 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Server.hpp"
#include "../include/Client.hpp"

Server::Server(int port, std::string password) : port(port), password(password)
{
}

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
	 while(1)
	 {
		setPoll();
	}

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
	if(ret < 0) {
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
	
	Client* newClient= new Client(newClientSocketFd, clientAddrr,password);
	clients[newClientSocketFd] = newClient;
	
	std::cout << "New client connected, socket fd: " << newClientSocketFd << std::endl;
	
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