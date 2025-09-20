/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:18:29 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/13 20:16:22 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>

class Client
{
	private:
		int clientSocketFd;
		struct sockaddr_in clientAddr;
		std::string nickName;
		std::string userName;
		std::string hostName;
		std::string serverName;
		std::string realName;
		std::string serverPass;
		bool signPass;
		bool isRegistered[3];
		bool hasWelcomed;


	public:
		Client(int clientSocketFd, sockaddr_in clientAddr,std::string serverPass);
		~Client();
		
		void handleCommand(std::string &receiveData);
		void handleNick(std::vector<std::string> data);
		bool isSignedPassword();
		bool invalidCommand(const std::string& command);
		void handlePass(std::vector<std::string> data);
		void handleUser(std::vector<std::string> data);
		bool isRegister();
		void handleJoin(std::vector<std::string> data);
};




#endif