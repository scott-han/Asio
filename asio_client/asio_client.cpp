// asio_client.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <boost/asio.hpp>
#include "socket_session.h"
#include <string>
#include "example_handler.h"
#include <thread>
int main()
{

	boost::asio::io_service io_service;
	ExampleHandler example_handler;
	
	tcp::resolver resolver(io_service);
	tcp::resolver::query query("localhost", "2000");
	tcp::resolver::iterator iterator = resolver.resolve(query);
	std::cout << "main:" << std::this_thread::get_id() << "\n";
	
	socket_session c(io_service, iterator, example_handler, example_handler);
	
	auto t = std::thread([&io_service]() {
		
		std::cout << "io_service run on: " << std::this_thread::get_id() << "\n";
		io_service.run();
	});
	
	//c.sendText("012456789012456789");

	while (true) {
		std::string str;
		std::getline(std::cin, str);
		c.sendText(str);
	}
    return 0;
}

