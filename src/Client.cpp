/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/10/26 12:17:32 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

Client::Client(int clientSocketFd, std::string serverPass, Server* srv)
	: clientSocketFd(clientSocketFd), serverPass(serverPass), server(srv)
{
	nickName = "";
	userName = "";
	hostName = "";
	serverName = "";
	recvBuffer = "";
	realName = "";
	signPass = false;
	hasWelcomed = false;
	for (size_t i = 0; i < 3; i++)
		isRegistered[i] = 0;

}

Client::~Client()
{
	close(clientSocketFd);
}

Server* Client::getServer()const{
	return server;
}

bool Client::handleCommand(std::string &receiveData)
{
	std::stringstream ss(receiveData);
	std::string newCommand;
	std::vector<std::string> data;

	while (ss >> newCommand)
	{
		data.push_back(newCommand);
	}

	if (data.empty())
		return true;


	if (!invalidCommand(data[0]))
	{
		std::string errorMsg = ":server 421 " + nickName + " " + data[0] + " :Unknown command\r\n";
		send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
		return true;
	}

	if (!isSignedPassword() && data[0] != "PASS" && data[0] != "QUIT" && data[0] != "PING")
	{
		std::string errorMsg = ":server 464 * :Password required\r\n";
		send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
		return true;
	}

	if (data[0] == "QUIT") 
		return handleQuit(data),true;
	else if (data[0] == "PASS")
		handlePass(data);
	else if (data[0] == "USER")
		handleUser(data);
	else if (data[0] == "NICK")
		handleNick(data);	
	else if (data[0] == "PING") 
			handlePing(data);
	else
	{
		if(!isRegister())
		{
			std::string errorMsg = ":server 451 * :You have not registered\r\n"; 
			std::cout << " data[0] rejected: Not fully registered." << std::endl;
			send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
			return true;
		}	
		if (data[0] == "JOIN") 	
			handleJoin(data);
		else if (data[0] == "PRIVMSG") 
			handlePrivMsg(data);
		else if (data[0] == "NAMES") 
			handleNames(data);
		else if (data[0] == "WHO") 
			handleWho(data);
		if (data[0] == "PART") 
			handlePart(data);
		else if (data[0] == "KICK") 
			handleKick(data);
		else if (data[0] == "TOPIC") 
			handleTopic(data);
    	else if (data[0] == "NOTICE") 
			handleNotice(data);
    	else if (data[0] == "MODE") 
			handleMode(data);
     	else if (data[0] == "INVITE")
			handleInvite(data);
   	 	else if (data[0] == "LIST") 
			handleList(data);

	}
	if (isRegister() && !hasWelcomed)
	{
		std::string msg001 = ":server 001 " + nickName + " :Welcome to the IRC server, " + nickName + "\r\n";
		std::string msg002 = ":server 002 " + nickName + " :Your host is irc.localhost, running version 0.1\r\n";
		std::string msg003 = ":server 003 " + nickName + " :This server was created just now\r\n";

		send(clientSocketFd, msg001.c_str(), msg001.length(), 0);
		send(clientSocketFd, msg002.c_str(), msg002.length(), 0);
		send(clientSocketFd, msg003.c_str(), msg003.length(), 0);

		hasWelcomed = true;
	}
	return false;
}

int Client::getFd()const{
	return clientSocketFd;
}

std::string Client::getHostName()const{
	return hostName;
}

std::string Client::getNickName()const{
	return nickName;
}

std::string Client::getRealName()const{
	return realName;
}

std::string Client::getUserName()const{
	return userName;
}

std::string& Client::getRecvBuffer(){
	return recvBuffer;
}