/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 12:59:26 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 17:43:05 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Client::handlePass(std::vector<std::string> data){
	
	if(data.size() != 2){
		std::string errorMsg= ":irc.server.com 461 * PASS :Invalid number of parameters\r\n";
		send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
		return;
	}
	
	if(data[1] != serverPass){
		std::string errorMsg = ":irc.server.com 464 * :Password incorrect\r\n";
		send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
		return;
	}
	signPass = true;
	
}	

bool isValidNickname(const std::string& nickName) {
    if (nickName.empty() || !std::isalpha(nickName[0]))
        return false;

    for (size_t i = 0; i < nickName.size(); i++) {
        char c = nickName[i];
        if (!std::isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' &&
            c != '{' && c != '}' && c != '\\' && c != '`' && c != '^') {
            return false;
        }
    }
    return true;
}

void Client::handleNick(std::vector<std::string> data){
    if (data.size() < 2){
        std::string errorMsg = ":irc.server.com 431 * :No nickname given\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);	
        return;
    }
    if (!isValidNickname(data[1])) {
        std::string errorMsg = ":irc.server.com 432 * " + data[1] + " :Erroneous nickname\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    nickName = data[1];
    std::cout << "Nickname set to: " << nickName << std::endl;
}

// USER <username> <unused> <unused> :zerrin ayaz //burada real nama arasında boslık olbialir

void Client::handleUser(std::vector<std::string> data){
	
	std::cout << "size: " << data.size() << std::endl;

	if (data.size() < 5 || data[4].size() < 2 || data[4][0] != ':') {
			std::string errorMsg = ":irc.server.com 461 * USER :Not enough parameters\r\n";
			send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
			return;
	}
	userName = data[1];
    hostName = data[2];
    serverName = data[3];
	
	if(data.size()> 5)
	{
		realName = data[4].substr(1);
		for (size_t i = 5; i < data.size(); ++i) {
			realName += " " + data[i];
		}
	} else
		realName = data[4].substr(1);
	
	std::cout << "User info set - username: " << userName << ", realname: " << realName << std::endl;
	std::string welcomeMsg = ":irc.server.com 001 " + nickName + " :Welcome to the IRC server\r\n";
    send(clientSocketFd, welcomeMsg.c_str(), welcomeMsg.length(), 0);
}


