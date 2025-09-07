/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/07 19:09:54 by zayaz            ###   ########.fr       */
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
	std::vector<std::string> data;
	
	while(ss >> newCommand){
		data.push_back(newCommand);
		//std::cout << "nrCommand vector: " << newCommand << std::endl;
	}

	if(data.empty())
		return;
	
	if(data[0] == "NICK")
		handleNick(data);
	// else if(data[0] == "USER")
	//  	handleUser(data);
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



void Client::handleNick(std::vector<std::string> data)
{
	//""
	if(data.size() != 2 || data[1].empty()){
		perror("hata salak");
		exit(EXIT_FAILURE);
	}
	
	//nickName = data[1];
	//std::cout << "nickteyim: " << nickName << std::endl;
}

// void Client::handleUser(std::vector<std::string> data)
// {
// 	//""
// 	if(data[1].empty()){
// 		perror("hata salak");
// 		exit(EXIT_FAILURE);
// 	}
	
// 	user = data[1];
// 	std::cout << "nickteyim: " << nickName << std::endl;
// }