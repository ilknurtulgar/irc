#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <map> 
#include <string> 
#include<set>
#include "Client.hpp"

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


};

#endif
