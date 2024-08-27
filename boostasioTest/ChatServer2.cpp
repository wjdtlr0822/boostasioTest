#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#pragma warning(disable : 4996)

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

std::string make_daytime_string() {
	std::time_t now = time(0);
	return ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
	typedef boost::shared_ptr<tcp_connection> p;

	static p create(boost::asio::io_context& io) {
		return p(new tcp_connection(io));
	}

	tcp::socket& socket() {
		return m_socket;
	}

	void start() {
		m_message = make_daytime_string();
		boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
			boost::bind(&tcp_connection::handler_write, shared_from_this()));
	}

private:
	tcp_connection(boost::asio::io_context& io) : m_socket(io) {}
	void handler_write() {}

	tcp::socket m_socket;
	std::string m_message;
};

class tcp_server {
public:
	tcp_server(boost::asio::io_context& io) : io(io), m_acceptor(io, tcp::endpoint(tcp::v4(), 13)) {
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::p new_connection = tcp_connection::create(io);

		m_acceptor.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::p new_connection,
		const boost::system::error_code& err) {
		if (!err) {
			new_connection->start();
		}
		start_accept();
	}

	boost::asio::io_context& io;
	tcp::acceptor m_acceptor;
};

class udp_server {
public:
	udp_server(boost::asio::io_context& io) : m_socket(io, udp::endpoint(udp::v4(), 13)) {
		start_receive();
	};
private:
	void start_receive() {
		m_socket.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint,
			boost::bind(&udp_server::handle_receive, this,
				boost::asio::placeholders::error));
	}

	void handle_receive(const boost::system::error_code& err) {
		if (!err) {
			boost::shared_ptr<std::string> message(new std::string(make_daytime_string()));
			m_socket.async_send_to(boost::asio::buffer(*message), remote_endpoint,
				boost::bind(&udp_server::handle_send, this, message));

			start_receive();
		}
	}

	void handle_send(boost::shared_ptr<std::string> message) {}

	udp::socket m_socket;
	udp::endpoint remote_endpoint;
	boost::array<char, 1> recv_buffer_;
};

int main() {
	boost::asio::io_context io;
	tcp_server server1(io);
	udp_server server2(io);
	
	io.run();
}