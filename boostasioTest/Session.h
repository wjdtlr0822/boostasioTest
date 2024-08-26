#pragma once
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/buffer.hpp>
#include "Member.h"

using boost::asio::ip::tcp;

class Session :public Member, public boost::enable_shared_from_this<Session> {
public:
	Session(boost::asio::io_context& io) :
		_socket(io)
		, _strand(boost::asio::make_strand(io)) 
	{
		memset(_recvBuffer, 0, RecvBufferSize);
		memset(_sendBuffer, 0, SendBufferSize);
	}

	void start() {

	}

	tcp::socket& GetSocket() {
		return _socket;
	}

	void Send(boost::asio::mutable_buffer& buffer) {

	}

protected:
	void AsyncRead()
	{
		memset(_recvBuffer, 0, RecvBufferSize);
		_socket.async_read_some(boost::asio::buffer(_recvBuffer, RecvBufferSize),
			boost::asio::bind_executor(_strand,
				boost::bind(&Session::onRead,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
			)
		);
	}

	void onRead(const boost::system::error_code& err, size_t size)
	{

	}

	void AsyncWrite(const char* message, size_t size)
	{
		memcpy(_sendBuffer, message, size);
		boost::asio::async_write(_socket,
			boost::asio::buffer(_sendBuffer, size),
			boost::asio::bind_executor(_strand,
				boost::bind(&Session::OnWrite,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
			)
		);
	}

	void OnWrite(const boost::system::error_code& err, size_t size)
	{

	}

private:
	tcp::socket _socket;
	const static int RecvBufferSize = 1024;
	char _recvBuffer[RecvBufferSize];
	const static int SendBufferSize = 1024;
	char _sendBuffer[SendBufferSize];

	boost::asio::strand<boost::asio::io_context::executor_type> _strand;
};