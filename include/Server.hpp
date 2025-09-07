/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:18:31 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/07 14:26:39 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP


#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <map>
#include "Client.hpp"
#include <cstdlib>
#include <cstdio>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

class Client;

class Server
{
	private:
		int port;
		std::string password;
		int serverSocketFd;
		std::map<int, Client*> clients;
		void setupServer();
		void acceptNewClient();
		void recvClientData(int clientSocketFd);
		void setPoll();
	public:
		Server(int port, std::string password);
		~Server();
		void run();
		
};




#endif