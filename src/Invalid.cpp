/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invalid.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 13:41:21 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 13:46:10 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

bool Client::isSignedPassword()
{
	return signPass;
}

bool Client::invalidCommand(const std::string& command){
	return (command == "PASS" || command == "NICK" || 
            command == "USER" || command == "JOIN" || command == "PING" || command == "NAMES" || command == "WHO" ||
            command == "PRIVMSG" || command == "KICK" ||
            command == "INVITE" || command == "TOPIC" ||
            command == "MODE" || command == "QUIT" || command == "PART");
	
}

bool Client::isRegister() {
    for(int i = 0; i < 3; i++)
    {
        if(isRegistered[i]!=true)
            return false;
    }

    if(!userName.empty() && !realName.empty() && !hostName.empty() && !serverName.empty())
        return true;
    return false;

}
