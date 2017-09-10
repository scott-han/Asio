#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include "msg_with_header.h"
class IMsgHandler {
public:
	virtual void HandleMessage(std::unique_ptr<MessageWithHeader> msg, int session_id) = 0;
};