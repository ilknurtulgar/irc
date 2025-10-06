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
#include<set>
#include "Client.hpp"

class Client;

class Channel
{
    private:
        std::string channelName;
        std::string topic;
        std::map<int, Client*> users;
        std::set<Client*> operators; //@kullanıcı
        std::set<Client*> invited;
        bool inviteOnly;
        bool authTopic;
    public:
        Channel(const std::string &channelName);
        ~Channel();

        void addUser(Client* client);
        void broadcast(const std::string& msg, Client* client);
        bool whereNames(Client *client);
        std::map<int, Client*>& getUsers() { return users; }
        void removeUser(Client* client);
        bool findUser(Client* client) const;
        bool isOperator(Client *client)const;
        
        std::string getChannelName() const;
        std::string getNickList()const;
        std::string getTopic()const;
        bool setInviteOnly(bool inviteOnly);
        void setTopic(std::string &topic);
        void setAuthTopic(bool authTopic);
        bool isAuthTopic();
        bool isInviteOnly();
        bool isInvited(Client *client);
        void removeInvite(Client *client);
        void addInvite(Client *client);
};

#endif
