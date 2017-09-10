#pragma once
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <deque>
#include "msg_with_header.h"
#include "msg_handler.h"
#include "session_handler.h"
using boost::asio::ip::tcp;
static std::atomic<int> session_id_count{1};
class socket_session
	//: public std::enable_shared_from_this<socket_session>
{
public:
	socket_session(boost::asio::io_service& io_service, IMsgHandler &msg_handler, ISessionHandler& session_handler)
		: socket_(io_service), m_msg_handler(msg_handler), m_session_handler(session_handler), m_session_id(++session_id_count)
	{
	}
	socket_session(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator, IMsgHandler &msg_handler, ISessionHandler& session_handler)
		: socket_(io_service), m_msg_handler(msg_handler), m_session_handler(session_handler), m_session_id(++session_id_count)
	{
		std::cout<< std::this_thread::get_id() << " socket_session: "  << m_session_id << "\n";
		boost::asio::async_connect(socket_, endpoint_iterator,
			boost::bind(&socket_session::handle_connect, this,
				boost::asio::placeholders::error));
	}
	void handle_connect(const boost::system::error_code& error)
	{
		std::cout << "handle_connect:" << std::this_thread::get_id() << "\n";
		if (!error)
		{
			asyncReadHeader();
			m_session_handler.HandleSessionConnected(m_session_id);
		}
		else {
			handleSessionBroke(error, __func__);
		}
	}
	tcp::socket& socket()
	{
		return socket_;
	}
	void asyncReadHeader() {
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.getBodayLenthAddress(), MessageWithHeader::header_length),
			boost::bind(
				&socket_session::handle_read_header, this,
				boost::asio::placeholders::error));
	}

	void start() {
		std::cout << std::this_thread::get_id() << " start read on session :" << m_session_id << "\n";
		asyncReadHeader();
	}
	void shutDown() {
		socket_.get_io_service().post([=]() {socket_.close(); });
	}
	void sendText(const std::string &txt) {
		socket_.get_io_service().post([=]() {send(MessageWithHeader(txt)); });
	}

	void send(const MessageWithHeader& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(socket_,
				boost::asio::buffer(write_msgs_.front().getMsg(),
					write_msgs_.front().getMsgLength()),
				boost::bind(&socket_session::handle_write, this,
					boost::asio::placeholders::error));

		}

	}

	void handle_read_header(const boost::system::error_code& error)
	{
		if (!error)
		{
			read_msg_.allocateMemory();
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
				boost::bind(&socket_session::handle_read_body, this,
					boost::asio::placeholders::error));
		}
		else {
			handleSessionBroke(error, __func__);
		}

	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if (!error)
		{
			m_msg_handler.HandleMessage(std::unique_ptr<MessageWithHeader>(new MessageWithHeader(std::move(read_msg_))), m_session_id);
			asyncReadHeader();
		}
		else {
			handleSessionBroke(error, __func__);
		}

	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				boost::asio::async_write(socket_,
					boost::asio::buffer(write_msgs_.front().getMsg(),
						write_msgs_.front().getMsgLength()),
					boost::bind(&socket_session::handle_write, this,
						boost::asio::placeholders::error));
			}
		}
		else {
			handleSessionBroke(error, __func__);
		}

	}
	int getSessionId()const { return m_session_id; }
private:
	void handleSessionBroke(const boost::system::error_code& err, const std::string& func_name) {
		std::cout << std::this_thread::get_id() << " " <<
			func_name << " error:" << err.message() << " on session id:" << m_session_id << "\n";
		m_session_handler.HandleBrokenSessionEvt(m_session_id, err);
	}
	tcp::socket socket_;
	MessageWithHeader read_msg_;
	using message_queue = std::deque<MessageWithHeader>;
	IMsgHandler& m_msg_handler;
	message_queue write_msgs_;
	ISessionHandler& m_session_handler;
	int m_session_id;
};