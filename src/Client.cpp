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
	std::cout << "client const: " << this->clientAddr.sin_addr.s_addr << std::endl;
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

	std::cout << "receiveData: " << receiveData << std::endl;

	if (data[0] == "PASS")
		handlePass(data);
	else if (data[0] == "USER")
		handleUser(data);
	else if (data[0] == "NICK")
		handleNick(data);

	// Kayıt durumu kontrol et ve güncelle
	else if (data[0] == "JOIN")
	{
		if (isRegister())
		{
			if (!userName.empty() && !realName.empty() && !hostName.empty() && !serverName.empty())
			{
				std::string welcomeMsg = ":irc.server.com 001 " + nickName + " :Welcome to the IRC server\r\n";
				send(clientSocketFd, welcomeMsg.c_str(), welcomeMsg.length(), 0);
			}

			// Kullanıcı henüz kayıtlı değilse ve komut PASS/USER/NICK değilse, izin verme
		}
		else
		{
			std::string errorMsg = ":irc.server.com 451 * :You have not registered\r\n";
			send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
			return;
		}
	}
}
