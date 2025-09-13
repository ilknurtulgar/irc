/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 12:59:26 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 15:15:31 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

bool isValidNickname(const std::string& nickName) {
    if (nickName.empty() || !std::isalpha(nickName[0]))
        return false;

    for (int i = 0; i < nickName.size(); i++) {
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