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
        std::set<Client*> operators;
    public:
        Channel(const std::string &channelName);
        ~Channel();

        void addUser(Client* client);
        void broadcast(const std::string& msg, Client* client);
        bool whereNames(Client *client);
        std::map<int, Client*>& getUsers() { return users; }
        void removeUser(Client* client);
        bool findUser(Client* client) const;
        //std::string getChannelName() const;

};

#endif
