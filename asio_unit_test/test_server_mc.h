#pragma once
#include "msg_handler.h"
#include "session_handler.h"
#include <thread>
#include <string>
#include <iostream>
#include <set>
#include "socket_session.h"
#include <memory>
#include "server_listerner.h"
class TestSrvMultipleClient : public IMsgHandler , public ISessionHandler {
public:
	void startListern() {
		listerner = std::make_unique<SocketListerner>(io_service, tcp::endpoint(tcp::v4(), 2000), *this, *this);
		listerner->start_accept();
		io_service.run();
	}
	void HandleMessage(std::unique_ptr<MessageWithHeader> msg,int session_id) override{
		for (auto& s : session_list) {
				if (s->getSessionId() == session_id) {
					s->sendText(msg->getBody());
					break;
				}
		}
	}

	void HandleNewSessionEvt(std::shared_ptr<socket_session> &session, const std::string& info) override {
		session_list.insert(session);
	}
	void HandleBrokenSessionEvt(int session_id, const boost::system::error_code& err) override {
		auto& del_session = std::find_if(session_list.begin(), session_list.end(), [&session_id](auto& s) { return s->getSessionId() == session_id; });
		if (del_session != session_list.end()) {
			session_list.erase(del_session);
		}
		if (session_list.empty())
			listerner->shut_down();
	}

private:
	boost::asio::io_service io_service;
	std::set<std::shared_ptr<socket_session>> session_list;
	std::unique_ptr<SocketListerner> listerner;
};