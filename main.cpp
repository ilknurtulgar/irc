/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itulgar <itulgar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:48:05 by itulgar           #+#    #+#             */
/*   Updated: 2025/08/23 18:05:04 by itulgar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <iostream>

int main(int argc, char** arv){
	
	(void)arv;
	if(argc != 3){
	 	std::cout << "invalid argument honey" << std::endl;
		return 0;
	}else {
		std::cout << "ak git kanka" << std::endl;
	}

}