#pragma once
#include <boost/asio.hpp>
#include "socket_session.h"
#include <memory>
#include "msg_handler.h"
#include "session_handler.h"
using boost::asio::ip::tcp;

class SocketListerner
{
	
public:
	SocketListerner(boost::asio::io_service& io_service,
		const tcp::endpoint& endpoint, IMsgHandler &gui_client, ISessionHandler& session_handler)
		: io_service_(io_service),
		acceptor_(io_service, endpoint),
		m_gui_client(gui_client), m_session_handler(session_handler)
	{
	}

	void start_accept()
	{
		std::shared_ptr<socket_session> new_session(new socket_session(io_service_, m_gui_client, m_session_handler));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&SocketListerner::handle_accept, this,new_session,
				boost::asio::placeholders::error));
	}
	void shut_down() {
		acceptor_.get_io_service().post([=]() {acceptor_.close(); });
	}
	void handle_accept(std::shared_ptr<socket_session> session,
		const boost::system::error_code& error)
	{
		std::cout << "acceptted:" << std::this_thread::get_id() << "\n";
		std::stringstream os;
		if (!error)
		{
			os << "socket connection: " << session->socket().remote_endpoint().address().to_string() << ":"
				<< session->socket().remote_endpoint().port() << " connected.\n";
			session->start();
			m_session_handler.HandleNewSessionEvt(session,os.str());
			start_accept();
		}
		else {
			os << "socket connection accept error:" << error.message() << "\n";
		}
		
	}

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	ISessionHandler& m_session_handler;
	IMsgHandler& m_gui_client;
};