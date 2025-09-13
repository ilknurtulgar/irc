/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 15:37:30 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

Client::Client(int clientSocketFd, sockaddr_in clientAddr,std::string serverPass)
  : clientSocketFd(clientSocketFd), clientAddr(clientAddr), serverPass(serverPass)
{
    nickName = "";
	userName = "";
	realName = "";
	signPass = false;
    std::cout << "client const: " << this->clientAddr.sin_addr.s_addr << std::endl;
}

Client::~Client()
{
	close(clientSocketFd);
}



void Client::handleCommand(std::string &receiveData){
	
	std::stringstream ss(receiveData);
	std::string newCommand;
	std::vector<std::string> data;
	
	while(ss >> newCommand){
		data.push_back(newCommand);
	}

	if(data.empty())
		return;

	std::cout << "Received from " << clientSocketFd << ": [";
	for(size_t i = 0; i < data.size(); ++i){
		std::cout << data[i];
		if(i < data.size() - 1)
			std::cout << " ";
	}
	std::cout << "]" << std::endl;
	
	if(!invalidCommand(data[0])){
		std::cout << "Unknown command: " << data[0] << std::endl;
		std::string errorMsg = ":irc.server.com 421 * INVALID :Unknown command\r\n";
		send(clientSocketFd, errorMsg.c_str(),errorMsg.length(), 0);	
		return;
	}

	if(!isSignedPassword() && data[0] != "PASS"){
    	std::string errorMsg = ":irc.server.com 464 * :Password required\r\n";
		send(clientSocketFd, errorMsg.c_str(),errorMsg.length(), 0);	
		return;
	}

	std::cout << "receiveData: " << receiveData << std::endl;
	if(data[0] == "PASS")
		handlePass(data);
	else if(data[0] == "USER")
	 	handleUser(data);
	// else if(data[0] == "JOIN")
	// 	handleJoin(data);
    // else if(data[0] == "PRIVMSG")
    //     handlePrivMsg(data);
	// else if(data[0] == "KICK")
	// 	handleKick(data);
	// else if(data[0] == "INVITE")
	// 	handleInvite(data);
	// else if(data[0] == "TOPIC -")
	// 	handleTopic(data);
	// else if(data[0] == "MODE")
	// 	handleMode(data);
}

