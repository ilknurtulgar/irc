/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:18:31 by itulgar           #+#    #+#             */
/*   Updated: 2025/10/11 19:34:23 by itulgar          ###   ########.fr       */
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
#include "Channel.hpp"
#include <cstdlib>
#include <cstdio>
#include <csignal>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

extern volatile sig_atomic_t g_run;


class Client;
class Channel;

class Server
{
	private:
		int port;
		std::string password;
		int serverSocketFd;
		std::map<int, Client*> clients;
		std::map<std::string, Channel*> channels;
		
		void setupServer();
		void acceptNewClient();
		void recvClientData(int clientSocketFd);
		void setPoll();
	public:
		Server(int port, std::string password);
		~Server();
		void run();
		bool isChannel(const std::string& name);
		void checkChannel(Client* client, const std::string& channelName);
		Client* getClientNick(std::string& nick);
		Channel* getChannel(std::string& channel);
		void singleNames(Client *client);
		void removeChannel(const std::string& channelName);
		void removeClient(int clientSocketFd, const std::string& message);
		 std::map<std::string, Channel*>& getChannels();

		
		
};




#endif