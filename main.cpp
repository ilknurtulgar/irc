/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zayaz <zayaz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:48:05 by itulgar           #+#    #+#             */
/*   Updated: 2025/09/07 13:18:38 by zayaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/Server.hpp"
#include "./include/Client.hpp"


int main(int argc, char** argv){
	
	if(argc != 3){
	 	std::cout << "invalid argument honey" << std::endl;
		return 0;
	}else {
		std::string password= argv[2];
		if(password.empty())
		{
			std::cout << "hatalı password" << std::endl;
			return 0;
		}
		int port = atoi(argv[1]);
		if(port < 0 || port > 65535)
		{
			std::cout << "hatalı port" << std::endl;
			return 0;
		}
		Server server(port,password);

		server.run();
		
	}

}

