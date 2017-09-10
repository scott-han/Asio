#pragma once
#include "msg_handler.h"
#include "session_handler.h"
#include <thread>
#include <string>
#include <iostream>
#include <set>
#include "socket_session.h"
#include "active.h"
class ExampleHandler : public IMsgHandler , public ISessionHandler {
public:
	ExampleHandler(): active(Active::createActive(__func__)) {}
	void HandleMessage(std::unique_ptr<MessageWithHeader> msg,int session_id) override{
		std::shared_ptr<MessageWithHeader> msg_shared = std::move(msg);
		auto task = std::bind([=] {
				std::cout << std::this_thread::get_id() << " HandleMessage from session:" << session_id << " with length: " << msg_shared->getMsgLength() << " with content:\n"
					<< msg_shared->getBody() << "\n";
				std::ostringstream stringStream;
				stringStream << "from " << session_id << " say:" << msg_shared->getBody();
				std::string copyOfStr = stringStream.str();
				for (auto& s : this->session_list) {
					if (s->getSessionId() != session_id) {
						s->sendText(copyOfStr);
					}
				}
			}
		);
		active->send(task);
		
	}
	void HandleSessionConnected(int session_id) override {
		Task task = std::bind([session_id]() {
			std::cout << std::this_thread::get_id() << " session id: " << session_id << " connected\n";
			}
		);
		active->send(task);
	}
	void HandleNewSessionEvt(std::shared_ptr<socket_session> &session, const std::string& info) override {
		
		Task task = std::bind([=, &session]() {
			session_list.insert(session);
			std::cout << std::this_thread::get_id() << " session id: " << session->getSessionId() <<" "<< info << "\n";
		}
		);
		active->send(task);
		
	}
	void HandleBrokenSessionEvt(int session_id, const boost::system::error_code& err) override {
	
		Task task = std::bind([=]() {
			auto del_session = std::find_if(session_list.begin(), session_list.end(),
                                    [&session_id](auto& s) { return s->getSessionId() == session_id; });
			if (del_session != session_list.end()) {
				session_list.erase(del_session);
			}
			std::cout << std::this_thread::get_id() << " HandleBrokenSessionEvt " << err.message() << ". session number left: " << session_list.size() << " \n";
		}
		);
		active->send(task);
	}

private:
	std::set<std::shared_ptr<socket_session>> session_list;
	std::unique_ptr<Active> active;
};