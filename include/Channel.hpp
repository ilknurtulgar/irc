/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 13:50:12 by zayaz             #+#    #+#             */
/*   Updated: 2025/10/19 19:13:54 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <map>
#include <string>
#include <set>
#include "Client.hpp"

class Client;

class Channel
{
private:
    std::string channelName;
    std::string topic;
    std::map<int, Client *> users;
    std::set<Client *> operators;
    std::set<Client *> invited;
    bool inviteOnly;
    bool authTopic;
    size_t userLimit;
    bool openLimit;
    std::string key;

public:
    Channel(const std::string &channelName);
    ~Channel();

    void addUser(Client *client);
    bool whereNames(Client *client);
    std::map<int, Client *> &getUsers();
    void removeUser(Client *client);
    bool findUser(Client *client) const;

    std::string getChannelName() const;
    std::string getNickList() const;
    std::string getTopic() const;
    void setTopic(std::string &topic);
    void setAuthTopic(bool authTopic);
    bool isAuthTopic();

    bool setInviteOnly(bool inviteOnly);
    bool isInviteOnly();
    bool isInvited(Client *client);
    void removeInvite(Client *client);
    void addInvite(Client *client);

    void setUserLimit(size_t limit);
    void closeUserLimit();
    bool isOpenLimit() const;
    size_t getUserLimit() const;
    size_t getUserCount() const;

    bool isOperator(Client *client) const;
    void addOperator(Client *client);
    void removeOperator(Client *client);

    void setKey(const std::string& key);
    void removeKey();
    bool hasKey() const;
    const std::string& getKey() const;

    void broadcast(const std::string &msg, Client *client);
};

#endif
