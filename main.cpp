/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:48:05 by itulgar           #+#    #+#             */
/*   Updated: 2025/10/09 17:13:58 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/Server.hpp"
#include "./include/Client.hpp"

volatile sig_atomic_t g_run = 1;

void signal_handler(int signum){
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	g_run = 0;
}

int main(int argc, char** argv){
	
	std::signal(SIGINT,signal_handler);
	std::signal(SIGTERM,signal_handler);
	
	if(argc != 3){
	 	std::cout << "ERROR: Invalid argument honey" << std::endl;
		return 0;
	}else {
		
		std::string password= argv[2];
		for (size_t i = 0; i < password.size(); ++i)
		{
			char character = password[i];
			if(character < 32 || character > 126){
				std::cout << "ERROR: Invalid password honey" << std::endl;
				return 0;
		}
			
		}
		
		if(password.empty() || password[0] == ':' || password.find(' ') !=std::string::npos)
		{
			std::cout << "ERROR: Invalid password honey" << std::endl;
			return 0;
		}
		int port = atoi(argv[1]);
		if(port < 0 || port > 65535)
		{
			std::cout << "ERROR: Invalid port honey" << std::endl;
			return 0;
		}
		Server server(port,password);
		server.run();
	}
}

