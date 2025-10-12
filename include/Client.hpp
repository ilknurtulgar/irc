/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:18:29 by itulgar           #+#    #+#             */
/*   Updated: 2025/10/12 19:46:40 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>
#include "Channel.hpp"

class Server;
class Channel;

class Client
{
	private:

	int clientSocketFd;
	std::string nickName;
	std::string userName;
	std::string hostName;
	std::string serverName;
	std::string realName;
	std::string serverPass;
	Server *server;
	bool signPass;
	bool isRegistered[3];
	bool hasWelcomed;

	// Buffer to accumulate incoming bytes until a full line (CRLF or LF) is received
	std::string inputBuffer;


	public:
		Client(int clientSocketFd,std::string serverPass, Server* srv);
		~Client();

		Server* getServer() const;
		
		void handleCommand(std::string &receiveData);
		void handleNick(std::vector<std::string> data);
		void handlePing(std::vector<std::string> data);
		void handleJoin(std::vector<std::string> data);
		void handleUser(std::vector<std::string> data);
		void handlePrivMsg(std::vector<std::string> data);
		void handleNames(std::vector<std::string> data);
		void handlePart(std::vector<std::string> data);
		void handleQuit(std::vector<std::string> data);
		void handleWho(std::vector<std::string> data);
		void handlePass(std::vector<std::string> data);
		void handleKick(std::vector<std::string> data);
		void handleTopic(std::vector<std::string> data);
		void handleInvite(std::vector<std::string> data);
		void handleNotice(std::vector<std::string> data);
		void handleMode(std::vector<std::string> data);
		void handleList(std::vector<std::string> data);

		bool isRegister();
		int getFd()const;
		bool isSignedPassword();
		bool invalidCommand(const std::string& command);

		// Append raw incoming bytes and process complete lines (CRLF or LF terminated)
		void handleIncoming(const char* data, ssize_t len);
		
		std::string getHostName()const;
		std::string getNickName()const;
		std::string getRealName()const;
		std::string getUserName()const;
};




#endif