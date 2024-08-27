#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
	Session(tcp::socket socket) : _socket(std::move(socket)), 
		_strand(boost::asio::make_strand(socket.get_executor())) {};

	void start() {
		DoRead();
	}
private:
	void DoRead() {
		auto self(shared_from_this());
		boost::asio::async_read_until(_socket, boost::asio::dynamic_buffer(_data), '\n',
			boost::asio::bind_executor(_strand,
				[this, self](boost::system::error_code ec, std::size_t length)
				{
					if (!ec) {
						std::cout << "Received : " << _data.substr(0, length);
						DoWrite(length);
					}
				}));
	}

	void DoWrite(std::size_t length) {
		auto self(shared_from_this());
		boost::asio::async_write(_socket,
			boost::asio::buffer(_data.substr(0, length)),
			boost::asio::bind_executor(_strand,
				[this, self](boost::system::error_code ec, std::size_t length) {
					if (!ec) {
						_data.erase(0, length);
						DoRead();
					}
				}));
	}

	tcp::socket _socket;
	boost::asio::strand<boost::asio::io_context::executor_type> _strand;
	std::string _data;
};

class Server {
public:
	Server(boost::asio::io_context& io, int port) : _acceptor(io, tcp::endpoint(tcp::v4(), port)) {
		DoAccept();
	}

private:
	void DoAccept() {
		_acceptor.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket) {
				if (!ec) {
					std::make_shared<Session>(std::move(socket))->start();
				}
			    DoAccept();
			});
	}

	tcp::acceptor _acceptor;
};

int q() {
	boost::asio::io_context io;
	Server server(io, 8888);
	io.run();

	return 0;
}