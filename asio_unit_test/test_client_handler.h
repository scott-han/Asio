#pragma once
#pragma once
#include "msg_handler.h"
#include "session_handler.h"
#include <thread>
#include <string>
#include <iostream>
#include "socket_session.h"
#include <string>

#include <memory>
class TestClientHandler : public IMsgHandler, public ISessionHandler {
public:
	std::vector<std::string> msg_to_send;
	std::vector<std::string> msg_received;
	int test_rep = 1000;
	void StartTest(boost::asio::io_service &io_service) {
		
		
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("localhost", "2000");
		tcp::resolver::iterator iterator = resolver.resolve(query);
		session = std::make_unique<socket_session>(io_service, iterator, *this, *this);
		for (int i = 0; i < test_rep; ++i) {
			std::stringstream os;
			os << "this is test message " << i << " from session: " << session->getSessionId();
			msg_to_send.push_back(os.str());
		}
		
	}
	void HandleMessage(std::unique_ptr<MessageWithHeader> msg, int session_id) override {
		    msg_received.push_back(std::string(msg->getBody()));
			if (send_msg_indx == msg_to_send.size())
			{
				std::cout << std::this_thread::get_id() << " shutDown session id: " << session_id << " \n";
				session->shutDown();
			}
			else{
				session->sendText(msg_to_send[send_msg_indx++]);
			}
	}
	void HandleSessionConnected(int session_id) override {
		session->sendText(msg_to_send[send_msg_indx++]);
		
	}
	void HandleNewSessionEvt(std::shared_ptr<socket_session> &session_, const std::string& info) override {
		std::cout << std::this_thread::get_id() << " HandleNewSessionEvt " << info << ". session id: " << session_->getSessionId() << " \n";
	}
	virtual void HandleBrokenSessionEvt(int session_id, const boost::system::error_code& err) override {
		std::cout << std::this_thread::get_id() << " HandleBrokenSessionEvt " << err.message() << ". session id: " << session_id << " \n";
	}

private:
	std::shared_ptr<socket_session> session;
	int send_msg_indx = 0;
};