/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 13:49:45 by zayaz             #+#    #+#             */
/*   Updated: 2025/10/19 19:15:23 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel(const std::string &channelName) : channelName(channelName), topic(""), inviteOnly(false), authTopic(false), userLimit(0), openLimit(false) {}

Channel::~Channel() {}

void Channel::addUser(Client *client)
{
    users[client->getFd()] = client;
    if (users.size() == 1)
        operators.insert(client);
}

void Channel::broadcast(const std::string &msg, Client *client)
{
    for (std::map<int, Client *>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it->second != client)
            send(it->second->getFd(), msg.c_str(), msg.length(), MSG_NOSIGNAL);
    }
}

bool Channel::whereNames(Client *client)
{

    return (users.find(client->getFd()) != users.end());
}

void Channel::removeUser(Client *client)
{
    // Remove from user map
    users.erase(client->getFd());
    // If the user was an operator, remove them from operators set
    operators.erase(client);
    // Also remove any outstanding invite for this client
    invited.erase(client);

    // If no operators remain but users exist, promote the first user to operator
    if (operators.empty() && !users.empty())
    {
        Client *newOp = users.begin()->second;
        operators.insert(newOp);
        // Broadcast MODE +o to inform clients about the new operator
        std::string modeMsg = ":" + newOp->getNickName() + "!" + newOp->getUserName() + "@localhost MODE " + channelName + " +o " + newOp->getNickName() + "\r\n";
        broadcast(modeMsg, NULL);
    }
}

bool Channel::findUser(Client *client) const
{
    return users.find(client->getFd()) != users.end();
}

std::string Channel::getChannelName() const
{
    return channelName;
}

std::string Channel::getNickList() const
{
    std::string nick;
    for (std::map<int, Client *>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        Client *client = it->second;

        if (!nick.empty())
            nick += " ";

        if (operators.find(client) != operators.end())
            nick += "@" + client->getNickName();
        else
            nick += client->getNickName();
    }
    return nick;
}

bool Channel::isOperator(Client *client) const
{
    return operators.find(client) != operators.end();
}

std::string Channel::getTopic() const
{
    return topic;
}

void Channel::setTopic(std::string &topic)
{
    this->topic = topic;
}

bool Channel::isInviteOnly()
{
    return this->inviteOnly;
}

bool Channel::setInviteOnly(bool inviteOnly)
{
    return this->inviteOnly = inviteOnly;
}

bool Channel::isInvited(Client *client)
{
    return (invited.count(client) > 0);
}

void Channel::removeInvite(Client *client)
{
    invited.erase(client);
}

void Channel::addInvite(Client *client)
{
    invited.insert(client);
}

void Channel::setAuthTopic(bool authTopic)
{
    this->authTopic = authTopic;
}

bool Channel::isAuthTopic()
{
    return authTopic;
}

void Channel::setUserLimit(size_t limit)
{
    userLimit = limit;
    openLimit = true;
}

void Channel::closeUserLimit()
{
    openLimit = false;
}

bool Channel::isOpenLimit() const
{
    return openLimit;
}

size_t Channel::getUserLimit() const
{
    return userLimit;
}

void Channel::addOperator(Client *client) 
{ 
    operators.insert(client);
}
void Channel::removeOperator(Client *client) 
{ 
    operators.erase(client); 
}

void Channel::setKey(const std::string& k) {
    key = k;
}

void Channel::removeKey() {
    key.clear();
}

bool Channel::hasKey() const {
    return !key.empty();
}

const std::string& Channel::getKey() const {
    return key;
}

size_t Channel::getUserCount() const
{
    return users.size();
}

std::map<int, Client*> &Channel::getUsers(){
     return users; 
}