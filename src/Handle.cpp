/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 12:59:26 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 20:36:31 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Client::handlePass(std::vector<std::string> data)
{

    if (data.size() != 2)
    {
        std::string errorMsg = "461 * PASS :Invalid number of parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    if (data[1] != serverPass)
    {
        std::string errorMsg = "464 * :Password incorrect\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    signPass = true;
    isRegistered[0] = true;
    std::cout << "PASS received: " << data[1] << std::endl;
}

bool isValidNickname(const std::string &nickName)
{
    if (nickName.empty() || !std::isalpha(nickName[0]))
        return false;

    for (size_t i = 0; i < nickName.size(); i++)
    {
        char c = nickName[i];
        if (!std::isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' &&
            c != '{' && c != '}' && c != '\\' && c != '`' && c != '^')
        {
            return false;
        }
    }
    return true;
}

void Client::handleNick(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string errorMsg = "431 * :No nickname given\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (!isValidNickname(data[1]))
    {
        std::string errorMsg = "432 * " + data[1] + " :Erroneous nickname\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    nickName = data[1];
    std::cout << "Nickname set to: " << nickName << std::endl;
    isRegistered[1] = true;
}

void Client::handleUser(std::vector<std::string> data)
{
    if (data.size() < 5 || data[4].size() < 2 || data[4][0] != ':')
    {
        std::string errorMsg = "461 * USER :Not enough parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    userName = data[1];
    hostName = data[2];
    serverName = data[3];

    if (data.size() > 5)
    {
        realName = data[4].substr(1);
        for (size_t i = 5; i < data.size(); ++i)
        {
            realName += " " + data[i];
        }
    }
    else
        realName = data[4].substr(1);
    isRegistered[2] = true;
    std::cout << "Username: " << data[1] << ", Realname: " << data[4] << std::endl;
}

void Client::handlePing(std::vector<std::string> data)
{

    std::string serverNames = "irc.localhost";
    if (data.size() > 1)
    {
        serverNames = data[1];
    }
    else
    {
        std::cout << "PING: Missing parameter." << std::endl;
        return;
    }
    std::string response = "PONG " + serverNames + " :" + serverNames + "\r\n";
    send(clientSocketFd, response.c_str(), response.length(), 0);

    std::cout << "Sent PONG to " << nickName << ": " << response;
}

void Client::handleJoin(std::vector<std::string> data)
{
    //join de şifreli deneme kısmına bak

    if (data.size() < 2)
    {
        std::string errorMsg = "461 * USER :Not enough parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

        if (data[1][0] != '#')
        {
            std::string errorMsg = "403 ERR_NOSUCHCHANNEL" + nickName + " " + data[1] + " :No such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        server->checkChannel(this,data[1]);
}

void Client::handlePrivMsg(std::vector<std::string> data){

    if(data.size() < 3)
    {
        std::string errorMsg = "PRIVMSG: Not enough parameters.\r\n";
        send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
        return ;
    }

    if(data[2][0] != ':'){
            std::string errorMsg = ":412 " + nickName + ":No text to send\r\n";
            send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
            return;
    }

    if(data[1][0] == '#'){

        if(!server->isChannel(data[1])){
            std::string errorMsg = ":403 " + nickName + " " + data[1] + " :No such channel\r\n"; 
            send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
            return;
        }

        Channel* channel = server->getChannel(data[1]);
        std::string msg = ":" + nickName + data[1] + data[2] + "\r\n";
        channel->broadcast(msg,this);
        return;
        
    } else { 
        Client* nickClinet = server->getClientNick(data[1]);
        std::cout << nickClinet << std::endl;
        
        if(nickClinet == nullptr){
            std::string errorMsg = "401 " + nickName + " " + data[1] + " :No such nick/channel\r\n";
            send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
            return;
        }
        
        std::string msg = ":" + nickName + " PRIVMSG " + data[1] + " :" + data[2] + "\r\n";
         send(nickClinet->getFd(),msg.c_str(),msg.length(),0);
         return;
    }
}

void Client::handleNames(std::vector<std::string> data){
    if(data.size() == 1){
        server->singleNames(this);
    }
}