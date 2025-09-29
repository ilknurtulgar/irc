#include "../include/Channel.hpp"

Channel::Channel(const std::string &channelName) : channelName(channelName), topic(""){} 

Channel::~Channel(){}

void Channel::addUser(Client* client){
    users[client->getFd()] = client;
}

void Channel::broadcast(const std::string& msg, Client* client){
    for (std::map<int, Client*>::iterator it = users.begin() ; it != users.end() ;++it){
        if(it->second != client)
            send(it->second->getFd(),msg.c_str(),msg.length(),0);
    }
}

bool Channel::whereNames(Client *client){
    
    return (users.find(client->getFd()) != users.end());
}