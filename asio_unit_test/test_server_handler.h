#pragma once
#pragma once
#pragma once
#include "msg_handler.h"
#include "session_handler.h"
#include <thread>
#include <string>
#include <iostream>
#include <set>
#include "socket_session.h"
#include <string>
#include "server_listerner.h"
#include <memory>
class TestServerHandler : public IMsgHandler, public ISessionHandler {
public:
	
	
	void startListern() {
		listerner = std::make_unique<SocketListerner>(io_service, tcp::endpoint(tcp::v4(), 2000), *this, *this);
		
		auto t = std::thread([=]() {
			listerner->start_accept();
			std::cout << "io_service run:" << std::this_thread::get_id() << "\n";
			io_service.run();
		});
		t.join();
		std::cout<< std::this_thread::get_id() << " TestServerHandler join:"  << "\n";
	}
	void HandleMessage(std::unique_ptr<MessageWithHeader> msg, int session_id) override {
		//just echo back
		session->sendText(msg->getBody());
	}

	void HandleNewSessionEvt(std::shared_ptr<socket_session> &session_, const std::string& info) override {
		session = session_;
	}
	virtual void HandleBrokenSessionEvt(int session_id, const boost::system::error_code& err) override {
		//shut down the listerner
		listerner->shut_down();
		
	}

private:
	boost::asio::io_service io_service;
	std::shared_ptr<socket_session> session;
	std::unique_ptr<SocketListerner> listerner;
};