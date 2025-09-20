/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 20:43:13 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

Client::Client(int clientSocketFd, sockaddr_in clientAddr, std::string serverPass)
	: clientSocketFd(clientSocketFd), clientAddr(clientAddr), serverPass(serverPass)
{
	nickName = "";
	userName = "";
	hostName = "";
	serverName = "";
	realName = "";
	signPass = false;
	hasWelcomed = false;
}

Client::~Client()
{
	close(clientSocketFd);
}

void Client::handleCommand(std::string &receiveData)
{

	std::stringstream ss(receiveData);
	std::string newCommand;
	std::vector<std::string> data;

	while (ss >> newCommand)
	{
		data.push_back(newCommand);
	}

	if (data.empty())
		return;

	std::cout << "Received from " << clientSocketFd << ": [";
	for (size_t i = 0; i < data.size(); ++i)
	{
		std::cout << data[i];
		if (i < data.size() - 1)
			std::cout << " ";
	}
	std::cout << "]" << std::endl;

	if (!invalidCommand(data[0]))
	{
		std::cout << "Unknown command: " << data[0] << std::endl;
		std::string errorMsg = ":irc.server.com 421 * INVALID :Unknown command\r\n";
		send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
		return;
	}

	if (!isSignedPassword() && data[0] != "PASS")
	{
		std::string errorMsg = ":irc.server.com 464 * :Password required\r\n";
		send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
		return;
	}

	if (data[0] == "PASS")
		handlePass(data);
	else if (data[0] == "USER")
		handleUser(data);
	else if (data[0] == "NICK")
		handleNick(data);
	else
	{
		if(!isRegister())
		{
			std::string errorMsg = ":irc.server.com 451 * :You have not registered\r\n";
			send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
			return;
		}
	
	if (data[0] == "JOIN") 
		std::cout << "join" << std::endl;	
	//handleJoin(data);
	
    // else if (data[0] == "PART") handlePart(data);
    // else if (data[0] == "PRIVMSG") handlePrivMsg(data);
    // else if (data[0] == "NOTICE") handleNotice(data);
    // else if (data[0] == "TOPIC") handleTopic(data);
    // else if (data[0] == "KICK") handleKick(data);
    // else if (data[0] == "MODE") handleMode(data);
    // else if (data[0] == "INVITE") handleInvite(data);
    // else if (data[0] == "WHO") handleWho(data);
    // else if (data[0] == "NAMES") handleNames(data);
    // else if (data[0] == "LIST") handleList(data);
    // else if (data[0] == "PING") handlePing(data);
	}
	
	if (isRegister() && !hasWelcomed)
	{
		std::string msg001 = ":irc.localhost 001 " + nickName + " :Welcome to the IRC server, " + nickName + "!\r\n";
		std::string msg002 = ":irc.localhost 002 " + nickName + " :Your host is irc.localhost, running version 0.1\r\n";
		std::string msg003 = ":irc.localhost 003 " + nickName + " :This server was created just now\r\n";
		std::string msg004 = ":irc.localhost 004 " + nickName + " irc.localhost 0.1 iowghraAsORTVSxNCWqBzvdHtGp\r\n";

		send(clientSocketFd, msg001.c_str(), msg001.length(), 0);
		send(clientSocketFd, msg002.c_str(), msg002.length(), 0);
		send(clientSocketFd, msg003.c_str(), msg003.length(), 0);
		send(clientSocketFd, msg004.c_str(), msg004.length(), 0);

		hasWelcomed = true;
	}
}
