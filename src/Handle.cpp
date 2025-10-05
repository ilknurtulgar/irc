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
    if (data.size() < 2)
    {
        std::string errorMsg = "461 * JOIN :Not enough parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::stringstream commands(data[1]);
    std::string channelName;
    while (std::getline(commands, channelName, ','))
    {
        if (channelName.empty() || channelName[0] != '#')
        {
            std::string errorMsg = "403 " + nickName + " " + channelName + " :No such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            continue;
        }

        server->checkChannel(this, channelName);
    }
}

void Client::handlePrivMsg(std::vector<std::string> data)
{

    if (data.size() < 3)
    {
        std::string errorMsg = "PRIVMSG: Not enough parameters.\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    if (data[2][0] != ':')
    {
        std::string errorMsg = ":412 " + nickName + ":No text to send\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    if (data[1][0] == '#')
    {

        if (!server->isChannel(data[1]))
        {
            std::string errorMsg = ":403 " + nickName + " " + data[1] + " :No such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        Channel *channel = server->getChannel(data[1]);
        std::string msg = ":" + nickName + data[1] + data[2] + "\r\n";
        channel->broadcast(msg, this);
        return;
    }
    else
    {
        Client *nickClinet = server->getClientNick(data[1]);
        std::cout << nickClinet << std::endl;

        if (nickClinet == nullptr)
        {
            std::string errorMsg = "401 " + nickName + " " + data[1] + " :No such nick/channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        std::string msg = ":" + nickName + " PRIVMSG " + data[1] + " :" + data[2] + "\r\n";
        send(nickClinet->getFd(), msg.c_str(), msg.length(), 0);
        return;
    }
}

void Client::handleNames(std::vector<std::string> data)
{
    if (data.size() == 1)
    {
        server->singleNames(this);
    }
    else if (data.size() != 2)
    {
        std::string errorMsg = "421 * INVALID :Unknown command\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::stringstream commands(data[1]);
    std::string channelName;
    while (std::getline(commands, channelName, ','))
    {
        if (channelName.empty() || channelName[0] != '#' || !server->isChannel(channelName))
        {
            std::string errorMsg = ":403 " + nickName + " " + channelName + " :No such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            continue;
        }
        Channel *channel = server->getChannel(channelName);
        std::string nickList = channel->getNickList();
        std::string errorMsg1 = ":353 " + nickName + " = " + channelName + " :" + nickList + "\r\n";
        send(clientSocketFd, errorMsg1.c_str(), errorMsg1.length(), 0);

        std::string errorMsg2 = ":366 " + nickName + " " + channelName + " :End of /NAMES list\r\n";
        send(clientSocketFd, errorMsg2.c_str(), errorMsg2.length(), 0);
    }
}

// komutta kanal adı var mı BAK
// kanal var mı
// kullanıcı kanalda var mı
// mapten sil
// bir kullanıcı varsa kanalı da sil
void Client::handlePart(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string errorMsg = "461 " + nickName + " PART :Not enough parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string message = "";
    if (data.size() > 2)
    {
        message = data[2];
        if (!message.empty() && message[0] == ':')
            message = message.substr(1);
        for (size_t i = 3; i < data.size(); ++i)
            message += " " + data[i];
    }

    std::stringstream ss(data[1]);
    std::string channelName;
    while (std::getline(ss, channelName, ','))
    {
        Channel *channel = server->getChannel(channelName);
        if (!channel)
        {
            std::string errMsg = "403 " + nickName + " " + channelName + " :No such channel\r\n";
            send(clientSocketFd, errMsg.c_str(), errMsg.length(), 0);
            continue;
        }

        if (!channel->findUser(this))
        {
            std::string errMsg = "442 " + nickName + " " + channelName + " :You're not on that channel\r\n";
            send(clientSocketFd, errMsg.c_str(), errMsg.length(), 0);
            continue;
        }

        std::string partMsg = ":" + nickName + "!" + userName + "@localhost PART " + channelName;
        if (!message.empty())
            partMsg += " :" + message;
        partMsg += "\r\n";

        channel->broadcast(partMsg, this);
        send(clientSocketFd, partMsg.c_str(), partMsg.length(), 0);

        channel->removeUser(this);
        if (channel->getUsers().empty())
        {
            server->removeChannel(channelName);
            std::cout << "INFO: Channel " << channelName << " deleted" << std::endl;
        }
    }
}

// QUIT iteratorle kanalı sil çıktı= :nick!user@host QUIT :Client quit

// QUIT :mesaj varsa mesajı da göster ekranda çıktısı= :nick!user@host QUIT :mdsaj
// kullanıcı tüm kanallardan çıkacak
// socket kapat!!!!!!!!!!!!
//  client sil!!!!!!!!!!

void Client::handleQuit(std::vector<std::string> data)
{
    std::string message = "Client Quit";
    if (data.size() > 1)
    {
        message = data[1];
        if (!message.empty() && message[0] == ':')
            message = message.substr(1);
        for (size_t i = 2; i < data.size(); ++i)
            message += " " + data[i];
    }
    std::string host = hostName.empty() ? "localhost" : hostName;
    std::string errMsg = ":" + nickName + "!" + userName + "@" + host + " QUIT";
    if (!message.empty())
        errMsg += " :" + message;
    errMsg += "\r\n";
    send(clientSocketFd, errMsg.c_str(), errMsg.length(), 0);
    server->removeClient(clientSocketFd, errMsg);
    close(clientSocketFd);
}

void Client::handleWho(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string err = "461 " + nickName + " WHO :Not enough parameters\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    else if (data.size() > 2)
    {
        std::string err = "461 " + nickName + " WHO :Too many parameters\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }

    if (!server->isChannel(data[1]))
    {
        std::string errorMsg = "403 " + nickName + " " + data[1] + " :No such channel\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    Channel *channel = server->getChannel(data[1]);

    std::map<int, Client *> members = channel->getUsers();

    for (std::map<int, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        Client *member = it->second;

        std::string flag = "H";
        if (channel->isOperator(member))
            flag += "@";
        std::string msg = ":irc.localhost 352 " + member->getNickName() + " " + data[1] + " " + member->getUserName() + " " + member->getHostName() + " irc.localhost " + member->getNickName() + " " + flag + " :0 " + member->getRealName() + "\r\n";
        send(clientSocketFd, msg.c_str(), msg.length(), 0);
    }

    std::string endMsg = ":315 " + nickName + " " + data[1] + " :End of WHO list\r\n";
    send(clientSocketFd, endMsg.c_str(), endMsg.length(), 0);
}

void Client::handleKick(std::vector<std::string> data){
    if(data.size() < 3){
        std::string err = "461 " + nickName + " KICK :Not enough parameters\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    if(!server->isChannel(data[1])){

        std::string err = "403 " + nickName + " " + data[1] + " :No such channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }

    Channel *channel = server->getChannel(data[1]);

    
    if(!channel->findUser(this)){
        std::string err = "442 " + nickName + " " + data[1] + " :You're not on that channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    
    
    if(!channel->isOperator(this)){
    
        std::string err = "482 " + nickName + " " + data[1] + " :You're not channel operator\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    Client *userClient = server->getClientNick(data[2]);
    if(userClient == nullptr)
    {
        std::string err = "401 " + nickName + " " + data[2] + " :No such nick\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    if(!channel->findUser(userClient))
    {
        std::string err = "401 " + nickName + " " + data[2] + " :They aren't on that channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    std::string msg;
    if(data.size() >= 4){   
        for (size_t i = 3; i < data.size(); i++){
            if(i > 3)
                msg += " ";
            msg += data[i];
        }   
    }else
        msg = nickName;
        std::string clientMsg = ":" + nickName + "!" + userName + "@" + hostName + 
             " KICK " + data[1] + " " + data[2] + " :" + msg + "\r\n";
        channel->broadcast(clientMsg, nullptr);
        channel->removeUser(userClient);
}