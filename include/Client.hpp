/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:18:29 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/07 16:40:17 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>

class Client
{
	private:
		int clientSocketFd;
		struct sockaddr_in clientAddr;
		std::string nickName;

	public:
		Client(int clientSocketFd, sockaddr_in clientAddr);
		~Client();
		void handleCommand(std::string &receiveData);
		void handleNick(std::string &receiveData);
};




#endif