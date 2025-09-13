/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 12:59:26 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 14:42:35 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Client::handlePass(std::vector<std::string> data){
	
	if(data.size() != 2){
		std::string errorMsg= ":irc.server.com 461 * PASS :Invalid number of parameters\r\n";
		send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
		return;
	}
	
	if(data[1] != serverPass){
		std::string errorMsg = ":irc.server.com 464 * :Password incorrect\r\n";
		send(clientSocketFd,errorMsg.c_str(),errorMsg.length(),0);
		return;
	}
	signPass = true;
	
}	

// void Client::handleUser(std::vector<std::string> data){
	

// }