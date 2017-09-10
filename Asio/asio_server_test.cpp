// Asio.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <cstdlib>

#include <iostream>
#include <list>
#include <set>

#include <memory>
#include <thread>

#include "msg_with_header.h"
#include <string>
#include "example_handler.h"
#include "server_listerner.h"



typedef std::shared_ptr<SocketListerner> chat_server_ptr;


//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	try
	{
		std::cout <<"main thread on: "<<  std::this_thread::get_id() << " \n";

		boost::asio::io_service io_service;
		
		
		ExampleHandler gui_client;
		tcp::endpoint endpoint(tcp::v4(), 2000);
		
		auto listerner = SocketListerner(io_service, endpoint, gui_client,gui_client);
		listerner.start_accept();
		auto& t = std::thread([&io_service]() {
			std::cout <<  "io_service run on: " << std::this_thread::get_id() <<" \n";
			io_service.run();
			
		});	
		
		t.join();
		
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
