/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/07 16:46:19 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

Client::Client(int clientSocketFd, sockaddr_in clientAddr)
  : clientSocketFd(clientSocketFd), clientAddr(clientAddr)
{
    // Parametreyi değil, class üyesini kullanalım
    std::cout << "client const: " << this->clientAddr.sin_addr.s_addr << std::endl;
}

Client::~Client()
{
	close(clientSocketFd);
}

void Client::handleCommand(std::string &receiveData){
	std::stringstream ss(receiveData);
	std::string newCommand;
	std::getline(ss,newCommand,' ');
	ss >> newNick;
	if(newCommand == "NICK")
		handleNick(receiveData);
	// else if(newCommand == "USER")
	// 	handleUser(receiveData);
	// else if(newCommand == "JOIN")
	// 	handleJoin(receiveData);
    // else if(newCommand == "PRIVMSG")
    //     handlePrivMsg(receiveData);
	// else if(newCommand == "KICK")
	// 	handleKick(receiveData);
	// else if(newCommand == "INVITE")
	// 	handleInvite(receiveData);
	// else if(newCommand == "TOPIC -")
	// 	handleTopic(receiveData);
	// else if(newCommand == "MODE")
	// 	handleMode(receiveData);
	
	printf("command: %s\n", newCommand.c_str());
	
	
	std::cout <<"burasıyom : " << receiveData << std::endl;
}

void Client::handleNick(std::string &receiveData)
{
	std::stringstream ss(receiveData);
	std::getline(ss,newCommand,' ');
	std::string newNick;
	ss >> newNick;
	std::cout << "new: " << newNick << std::endl;
	//std::getline(ss, newNick,);
}