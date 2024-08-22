#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <ctime>

#pragma warning(disable : 4996)

using boost::asio::ip::tcp;

std::string make_daytime_string1() {
	std::time_t now = time(0);
	return ctime(&now);
}

int test() {
	std::cout << "Server Load ...... " << std::endl;
	try {
		boost::asio::io_context io;
		tcp::acceptor accptor(io, tcp::endpoint(tcp::v4(), 13));

		std::cout << "Server Start " << std::endl;
		while (true) {
			tcp::socket socket(io);
			accptor.accept(socket);

			std::string message = make_daytime_string1();

			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}