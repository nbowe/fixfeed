#ifndef Subscription_h__
#define Subscription_h__

#include <string>
#include <quickfix/SessionID.h>
#include <boost/operators.hpp>


// represents a Subscription to a particular symbol
struct Subscription : boost::equality_comparable<Subscription> {
	Subscription(const FIX::SessionID& session, const std::string& s, const std::string& r)
		:session_id(session),symbol(s),req_id(r)
	{
	}

	const FIX::SessionID& get_session_id() const {return session_id;}
	const std::string& get_symbol() const {return symbol;}
	const std::string& get_req_id() const {return req_id;}

private:
	FIX::SessionID session_id;
	std::string symbol;
	std::string req_id;
};

inline bool operator==(const Subscription& s1, const Subscription& s2) {
	return s1.get_session_id() == s2.get_session_id() && 
		s1.get_symbol() == s2.get_symbol() && 
		s1.get_req_id() == s2.get_req_id();
}

#endif // Subscription_h__