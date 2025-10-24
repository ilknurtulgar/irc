/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:59:03 by zayaz             #+#    #+#             */
/*   Updated: 2025/10/24 20:35:34 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Client::handlePass(std::vector<std::string> data)
{

    if (data.size() != 2)
    {
        std::string errorMsg = "PASS requires more parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    if (data[1] != serverPass)
    {
        std::string errorMsg = "Password incorrect\r\n";
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
        return;
    
    if (!isValidNickname(data[1]))
    {
        std::string errorMsg = ":server 432 * " + data[1] + " :Erroneous nickname\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (server->isNickInUse(data[1]))
    {
        std::string errorMsg = ":server 433 * " + data[1] + " :Nickname is already in use\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string oldNick = nickName;
    std::string newNick = data[1];

    nickName = newNick;
    std::cout << "Nickname set to: " << nickName << std::endl;
    isRegistered[1] = true;

    if (!oldNick.empty() && oldNick != newNick)
    {
        std::string host = hostName.empty() ? "localhost" : hostName;
        std::string nickChangeMsg = ":" + oldNick + "!" + userName + "@" + host + " NICK " + ":" + newNick + "\r\n";
        
        std::map<std::string, Channel*>& channels = server->getChannels();
        for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            Channel* ch = it->second;
            if (ch->findUser(this))
            {
                ch->broadcast(nickChangeMsg, this);
            }
        }

        send(clientSocketFd, nickChangeMsg.c_str(), nickChangeMsg.length(), 0);
    }
}

void Client::handleUser(std::vector<std::string> data)
{
    if (data.size() < 5 || data[4].size() < 2 || data[4][0] != ':')
    {
        std::string errorMsg = "USER requires more parameters\r\n";
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
        return;
    
    std::string response = "PONG " + serverNames + " :" + serverNames + "\r\n";
    send(clientSocketFd, response.c_str(), response.length(), 0);

    std::cout << "Sent PONG to " << nickName << ": " << response;
}

void Client::handleJoin(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string errorMsg = "JOIN requires more parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
        return;
    }

    std::stringstream channelsStream(data[1]);
    std::stringstream keysStream;
    if (data.size() >= 3)
        keysStream.str(data[2]);

    std::string channelName;
    std::string key;

    while (std::getline(channelsStream, channelName, ','))
    {
        if (channelName.empty() || channelName[0] != '#')
        {
            std::string errorMsg = "No such channel: no such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
            continue;
        }

        Channel *channel = server->getChannel(channelName);
        if (channel && channel->isOpenLimit())
        {
            if (channel->getUserCount() >= channel->getUserLimit()) 
            {
                std::string errorMsg = "Cannot join channel (+l): cannot join channel (+l)\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
                continue;
            }
        }

        if (data.size() >= 3)
            std::getline(keysStream, key, ',');
        else
            key.clear();
        if (channel && channel->hasKey())
        {
            if (key.empty() || channel->getKey() != key)
            {
                std::string errorMsg = "Cannot join channel (+k): cannot join channel (+k)\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
                continue;
            }
        }
        server->checkChannel(this, channelName);
    }
}


void Client::handlePrivMsg(std::vector<std::string> data)
{
    if (data.size() < 3)
    {
        std::string errorMsg = "Not requires more parameters.\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
        return;
    }

  
    if (data[2][0] != ':')
    {
        std::string errorMsg = ":server 412 " + nickName + ":No text to send\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
        return;
    }

    std::string msg = data[2].substr(1);  
    for (size_t i = 3; i < data.size(); ++i) {
        msg += " " + data[i]; 
    }
	if(msg.empty() || (msg == ":" && msg.size() == 1))
		return;
  
    if (data[1][0] == '#')  
    {
        if (!server->isChannel(data[1]))  
        {
            std::string errorMsg = "No such channel: no such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
            return;
        }

        Channel *channel = server->getChannel(data[1]);
        std::string broadcastMsg = ":" + nickName + "!~" + getUserName() + "@localhost PRIVMSG " + data[1] + " :" + msg + "\r\n";        
        channel->broadcast(broadcastMsg, this); 
        return;
    }
    else 
    {
        Client *nickClient = server->getClientNick(data[1]);

        if (nickClient == NULL)
        {
            std::string errorMsg = data[1] + ": No such nick/channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), MSG_NOSIGNAL);
            return;
        }

    std::string sendMsg = ":" + nickName + "!~" + getUserName() + "@localhost PRIVMSG " + data[1] + " :" + msg + "\r\n";
    send(nickClient->getFd(), sendMsg.c_str(), sendMsg.length(), MSG_NOSIGNAL);
        return;
    }
}



void Client::handleNames(std::vector<std::string> data)
{
    if (data.size() == 1)
    {
        server->singleNames(this);
		return;
		
    }
    else if (data.size() != 2)
    {
        std::string errorMsg = ":421 * INVALID :Unknown command\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    std::stringstream commands(data[1]);
    std::string channelName;
    while (std::getline(commands, channelName, ','))
    {
        if (channelName.empty() || channelName[0] != '#' || !server->isChannel(channelName))
        {
            std::string errorMsg = "No such channel: no such channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            continue;
        }
        Channel *channel = server->getChannel(channelName);
        std::string nickList = channel->getNickList();
        std::string errorMsg1 = ":353 " + nickName + " = " + channelName + " :" + nickList + "\r\n";
        send(clientSocketFd, errorMsg1.c_str(), errorMsg1.length(), 0);

        std::string errorMsg2 = ":localhost 366 " + nickName + " " + channelName + " :End of /NAMES for *\r\n";
        send(clientSocketFd, errorMsg2.c_str(), errorMsg2.length(), 0);
    }
}

void Client::handlePart(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string errorMsg = "PART requires more parameters\r\n";
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
            std::string errMsg = "No such channel: no such channel\r\n";
            send(clientSocketFd, errMsg.c_str(), errMsg.length(), 0);
            continue;
        }

        if (!channel->findUser(this))
        {
            std::string errMsg = ":server 442 " + channelName + " :You're not on that channel\r\n";
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
    std::string errMsg = nickName + " [" + userName + "@" + host + "] has quit IRC";
    if (!message.empty())
        errMsg += ": " + message;
    errMsg += "\r\n";
	std::string closeMsg = "Server ERROR::Closing Link: " + nickName + " (irc.localhost) [Client Quit]\r\n";
	send(clientSocketFd, closeMsg.c_str(), closeMsg.length(), 0);
    server->removeClient(clientSocketFd, errMsg);
}
void Client::handleWho(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string err = "WHO requires more paramters\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    else if (data.size() > 2)
    {
        std::string err = "WHO requires more paramters\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }

    if (!server->isChannel(data[1]))
    {
        std::string errorMsg = "No such channel: no such channel\r\n";
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
        std::string msg = ":352 " + member->getNickName() + " " + data[1] + " " + member->getUserName() + " " + member->getHostName() + " irc.localhost " + member->getNickName() + " " + flag + " :0 " + member->getRealName() + "\r\n";
        send(clientSocketFd, msg.c_str(), msg.length(), 0);
    }

    std::string endMsg = ":315 " + nickName + " " + data[1] + " :End of WHO list\r\n";
    send(clientSocketFd, endMsg.c_str(), endMsg.length(), 0);
}

void Client::handleKick(std::vector<std::string> data)
{
    if (data.size() < 3)
    {
        std::string err = "KICK requires more parameters\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    if (!server->isChannel(data[1]))
    {

        std::string err = "No such channel: no such channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }

    Channel *channel = server->getChannel(data[1]);

    if (!channel->findUser(this))
    {
        std::string err = ":server 441 " + nickName + data[1] + " :You're not on that channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }

    if (!channel->isOperator(this))
    {

        std::string err = "Cannot kick the last operator from the channel: cannot kick the last operator from the channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    Client *userClient = server->getClientNick(data[2]);
    if (userClient == NULL)
    {
        std::string err =  data[2] + ": No such nick\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->findUser(userClient))
    {
        std::string err = ":server 401 " + nickName + " " + data[2] + " :They aren't on that channel\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    std::string msg;
    if (data.size() >= 4)
    {
        for (size_t i = 3; i < data.size(); i++)
        {
            if (i > 3)
                msg += " ";
            msg += data[i];
        }
    }
    else
        msg = nickName;
    std::string clientMsg = ":" + nickName + "!" + userName + "@" + hostName +
                            " KICK " + data[1] + " " + data[2] + " :" + msg + "\r\n";
    channel->broadcast(clientMsg, NULL);
    channel->removeUser(userClient);
}

void Client::handleInvite(std::vector<std::string> data)
{
    if (data.size() < 3)
    {
        std::string errorMsg = "INVITE requires more parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    std::string inviteNick = data[1];
    std::string channelName = data[2];
    if (!server->isChannel(channelName))
    {
        std::string errorMsg =  "No such channel: no such channel\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    Channel *channel = server->getChannel(channelName);

    if (!channel->isOperator(this))
    {
        std::string errorMsg =  "You're not channel operator: You're not channel operator\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (!channel->findUser(this))
    {
        std::string errorMsg = ":server 442 " + channelName + " :You're not on that channel\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    Client *inviteClient = server->getClientNick(inviteNick);
    if (!inviteClient)
    {
        std::string errorMsg = ":401 " + nickName + " " + inviteNick + " :No such nick/channel\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    std::string inviteMsg = ":" + nickName + " INVITE " + inviteNick + " :" + channelName + "\r\n";
    send(inviteClient->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);

    std::string message = ":" + nickName + " INVITE " + inviteNick + " :" + channelName + "\r\n";
    send(clientSocketFd, message.c_str(), message.length(), 0);
    channel->addInvite(inviteClient);
}

void Client::handleNotice(std::vector<std::string> data)
{
    if (data.size() < 3)
        return;
    std::string user = data[1];
    std::string message;
    for (size_t i = 2; i < data.size(); ++i)
    {
        message += data[i];
        if (i + 1 < data.size())
            message += " ";
    }
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);
    std::string prefix = ":" + nickName + "!~" + getUserName() + "@localhost";
    std::string errormsg = prefix + " NOTICE " + user + " :" + message + "\r\n";
    Client *userClient = server->getClientNick(user);
    if (userClient)
    {
        send(userClient->getFd(), errormsg.c_str(), errormsg.length(), 0);
        return;
    }
    if (user[0] == '#')
    {
        if (!server->isChannel(user))
            return;
        Channel *channel = server->getChannel(user);
        if (!channel)
            return;
        channel->broadcast(errormsg, this);
    }
}

void Client::handleTopic(std::vector<std::string> data)
{
    if (data.size() < 2)
    {
        std::string errorMsg = "TOPIC requires more parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (!server->isChannel(data[1]))
    {

        std::string errorMsg = "No such channel : no such channel\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    Channel *channel = server->getChannel(data[1]);
    std::string topic = channel->getTopic();
    std::string msg;

    if (!channel->findUser(this))
    {
        msg = ":server 442 " + data[1] + " :You're not on that channel\r\n";
        send(clientSocketFd, msg.c_str(), msg.length(), 0);
        return;
    }
    if (channel->isAuthTopic() && !channel->isOperator(this))
    {
        msg =  "You're not channel operator: You're not channel operator\r\n";
        send(clientSocketFd, msg.c_str(), msg.length(), 0);
        return;
    }

    if (data.size() == 2)
    {

        if (topic.empty())
            msg = ":localhost 331 " + nickName + " " + data[1] + " :No topic is set\r\n";
        else
            msg = ":localhost 332 " + nickName + " " + data[1] + " :" + topic + "\r\n";

        send(clientSocketFd, msg.c_str(), msg.length(), 0);
        return;
    }
    else
    {
        for (size_t i = 2; i < data.size(); i++)
        {
            if (i > 2)
                msg += " ";
            msg += data[i];
        }
        channel->setTopic(msg);

        msg = ":" + nickName + "!" + userName + "@" + hostName +
              " TOPIC " + data[1] + " :" + channel->getTopic() + "\r\n";
    }
    channel->broadcast(msg, this);
    send(clientSocketFd, msg.c_str(), msg.length(), 0);
}

void Client::handleMode(std::vector<std::string> data)
{
    std::string msg;

    if (data.size() < 3 || ( data.size() <= 3 && data[2][1] == 'o'))
    {
        std::string errorMsg = "MODE requires more parameters\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (!server->isChannel(data[1]))
    {
        std::string errorMsg = "No such channel: no such channel\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    char sign = data[2][0];
    char mode = data[2][1];
    Channel *channel = server->getChannel(data[1]);
    if((sign != '+' && sign != '-') ||
        (mode != 'i' && mode != 't' && mode != 'l' && mode != 'k' && mode != 'o'))
    {
        std::string err = data[2] + ": is unknown mode char to me\r\n";
        send(clientSocketFd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->isOperator(this))
    {
        std::string errorMsg = data[1] + ": You're not channel operator\r\n";
        send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (mode == 'i')
    {
        if (sign == '+')
        {
            channel->setInviteOnly(true);
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " +i\r\n";
        }
        else if (sign == '-')
        {
            channel->setInviteOnly(false);
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " -i\r\n";
        }
        channel->broadcast(msg, NULL);
    }
    else if (mode == 't')
    {
        if (sign == '+')
        {
            channel->setAuthTopic(true);
             msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " " + data[2] + "\r\n";
        }
        else if (sign == '-')
        {
            channel->setAuthTopic(false);
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " " + data[2] + "\r\n";
        }
        channel->broadcast(msg, NULL);
    }
    else if (mode == 'l')
    {
        if (sign == '+')
        {
            if (data.size() < 4)
            {
                std::string errorMsg = "MODE requires more parameters\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
                return;
            }

            size_t limit = atoi(data[3].c_str());
            if (limit == 0)
            {
                std::string errorMsg = data[3] + " :Invalid limit number\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
                return;
            }

            channel->setUserLimit(limit);
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " +l " + data[3] + "\r\n";
        }
        else if (sign == '-')
        {
            channel->closeUserLimit();
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " -l\r\n";
        }

        channel->broadcast(msg, NULL);
    }
    else if (mode == 'o')
    {
        if (data.size() < 4)
        {
            std::string errorMsg = "MODE requires more parameters\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        Client *targetClient = server->getClientNick(data[3]);
        if (!targetClient)
        {
            std::string errorMsg = ":401 " + data[3] + " :No such nick/channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }
        if (!channel->findUser(targetClient))
        {
            std::string errorMsg = ":server 441 " + nickName + " " + data[3] + " " + data[1] + " :They aren't on that channel\r\n";
            send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }
        if (sign == '+')
            channel->addOperator(targetClient);
        else
            channel->removeOperator(targetClient);

        msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " " + data[2] + " " + data[3] + "\r\n";
        channel->broadcast(msg, NULL);
    }
    else if (mode == 'k')
    {
        if (sign == '+')
        {
            if (data.size() < 4)
            {
                std::string errorMsg ="MODE requires more parameters\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
                return;
            }

            if (channel->hasKey())
            {
                std::string errorMsg = ":467 " + nickName + " " + data[1] + " :Channel key already set\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
                return;
            }

            channel->setKey(data[3]);
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " +k " + data[3] + "\r\n";
        }
        else if (sign == '-')
        {
            if (!channel->hasKey())
            {
                std::string errorMsg = ":467 " + nickName + " " + data[1] + " :Channel key not set\r\n";
                send(clientSocketFd, errorMsg.c_str(), errorMsg.length(), 0);
                return;
            }

            channel->removeKey();
            msg = ":" + nickName + "!" + userName + "@localhost MODE " + data[1] + " -k\r\n";
        }

        channel->broadcast(msg, NULL);
    }
}

void Client::handleList(std::vector<std::string> data)
{
    std::string message = ":321 " + nickName + " Channel :Users  Name\r\n";
    send(clientSocketFd, message.c_str(), message.length(), 0);

    std::map<std::string, Channel *> &allChannels = server->getChannels();

    if (data.size() == 1)
    {
        for (std::map<std::string, Channel *>::iterator it = allChannels.begin();
             it != allChannels.end(); ++it)
        {
            Channel *channel = it->second;
			std::ostringstream oss; 
			oss << channel->getUsers().size();
			std::string countUser = oss.str();
            std::string msg = ":322 " + nickName + " " + channel->getChannelName() +
                              " " + countUser +
                              " :" + channel->getTopic() + "\r\n";
            send(clientSocketFd, msg.c_str(), msg.length(), 0);
        }
    }
    else
    {
        std::stringstream commands(data[1]);
        std::string channelName;
        while (std::getline(commands, channelName, ','))
        {
            if (allChannels.find(channelName) == allChannels.end())
                continue;
            Channel *channel = allChannels[channelName];
				std::ostringstream oss; 
			oss << channel->getUsers().size();
			std::string countUser = oss.str();
            std::string msg = ":322 " + nickName + " " + channel->getChannelName() +
                              " " + countUser +
                              " :" + channel->getTopic() + "\r\n";
            send(clientSocketFd, msg.c_str(), msg.length(), 0);
        }
    }

    std::string msgEnd = ":323 " + nickName + " :End of /LIST\r\n";
    send(clientSocketFd, msgEnd.c_str(), msgEnd.length(), 0);
}
