#pragma once
#include <vector>
#include <memory>

class MessageWithHeader
{
public:
	static constexpr int header_length = sizeof(size_t);
	MessageWithHeader(const MessageWithHeader& other) = default;
	MessageWithHeader& operator=(const MessageWithHeader& other) = default;
	MessageWithHeader(MessageWithHeader&& other)
		: m_data(std::move(other.m_data))
		, m_body_length(other.m_body_length)
	{
		other.m_body_length = 0;
	}

	// Move assignment operator.  
	MessageWithHeader& operator=(MessageWithHeader&& other)
	{
		m_data = std::move(other.m_data);
		m_body_length = other.m_body_length;
		other.m_body_length = 0;
		return *this;
	}
	MessageWithHeader(const std::string & msg)
		: m_body_length(msg.length() + 1),	 m_data(getMsgLength(),0)
	{
		memcpy(m_data.data(), &m_body_length, header_length);
		memcpy(m_data.data() + header_length, msg.c_str(), msg.length());
	}

	MessageWithHeader()
		: m_body_length(0)
	{
	}

	char* getBodayLenthAddress() {
		return reinterpret_cast<char*>(&m_body_length);
	}
	char* getMsg() {
		return m_data.data();
	}
	size_t getBodyLength() const
	{
		return m_body_length;
	}
	size_t getMsgLength() const {
		return m_body_length + header_length;
	}
	void allocateMemory() {
		m_data.swap(std::vector<char>(getMsgLength(),0));
		memcpy(m_data.data(), &m_body_length, header_length);
	}
	
	char* getBody() {
		return m_data.data() + header_length;
	}
	
private:
	size_t m_body_length;
	std::vector<char> m_data;
	
};
