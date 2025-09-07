/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 20:12:54 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/07 16:15:18 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

Client::Client(int clientSocketFd, sockaddr_in clientAddr)
  : clientSocketFd(clientSocketFd), clientAddr(clientAddr)
{
    // Parametreyi değil, class üyesini kullanalım
    std::cout << "client const: " << this->clientAddr.sin_addr.s_addr << std::endl;
}

Client::~Client()
{
	close(clientSocketFd);
}

void Client::handleCommand(std::string &receiveData){
	
	std::cout <<"burasıyom : " << receiveData << std::endl;
}