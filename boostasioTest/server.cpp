#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>

class printer {
private:
	boost::asio::steady_timer t1_;
	boost::asio::steady_timer t2_;
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
	int count_;
public:
	printer(boost::asio::io_context& io)
		:strand_(boost::asio::make_strand(io)),
		t1_(io, boost::asio::chrono::seconds(1)),
		t2_(io, boost::asio::chrono::seconds(1)),
		count_(0)
	{
		t1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print1, this)));
		t2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print2, this)));
	}

	void print1() {
		if (count_ < 10) {
			count_++;
			std::cout << "print1 :  " << count_ << std::endl;
			t1_.expires_at(t1_.expiry() + boost::asio::chrono::seconds(1));
			t1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print1, this)));
		}
	}

	void print2() {
		if (count_ < 10) {
			count_++;
			std::cout << "print2 : " <<  count_ << std::endl;
			t2_.expires_at(t2_.expiry() + boost::asio::chrono::seconds(1));
			t2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print2, this)));
		}
	}
};

int main() {
	boost::asio::io_context io;
	printer p(io);
	io.run();
	return 0;
}