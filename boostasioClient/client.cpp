#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

int main() {
	boost::asio::io_context io;

	tcp::resolver resolver(io);
	tcp::resolver::results_type endpoint = resolver.resolve("127.0.0.1", "daytime");

	tcp::socket socket(io);
	boost::asio::connect(socket, endpoint);

	while (true) {
		std::array<char, 128> buf;
		boost::system::error_code error;

		size_t len = socket.read_some(boost::asio::buffer(buf), error);

		if (error == boost::asio::error::eof)
			break;
		else if (error)
			throw boost::system::system_error(error);

		std::cout.write(buf.data(), len);
	}
}