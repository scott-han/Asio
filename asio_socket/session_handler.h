#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
class socket_session;
class ISessionHandler {
public:
	virtual void HandleNewSessionEvt(std::shared_ptr<socket_session> &session, const std::string& info) {};
	virtual void HandleBrokenSessionEvt(int session_id, const boost::system::error_code& err) = 0;
	virtual void HandleSessionConnected(int session_id) {};
};