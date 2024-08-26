#pragma once
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

class Member {
public:
	Member();
	virtual ~Member() {};
	virtual void Send(boost::asio::mutable_buffer& buffer);
};

typedef boost::shared_ptr<Member> MemberPtr;
