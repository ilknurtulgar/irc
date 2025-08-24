/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/08/24 19:25:08 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/Server.hpp"
#include "./include/Client.hpp"




Client::Client(int clientSocketFd, sockaddr_in clientAddr) : clientSocketFd(clientSocketFd) , clientAddr(clientAddr)
{}

Client::~Client()
{
	close(clientSocketFd);
}

void Client::handleCommand(std::string &receiveData){
	
}