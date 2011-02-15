#ifndef zmq_tick_publisher_h__
#define zmq_tick_publisher_h__

#include <tick_publisher.h>
#include <zmq.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>
#include <active_object_helper.h>

class zmq_tick_publisher : public tick_publisher {
public:
	zmq_tick_publisher(zmq::context_t& ctx, const std::string& bind_address);
	virtual ~zmq_tick_publisher();
	virtual void tick(const MarketData& md);

private:
	void init(zmq::context_t& ctx, const std::string& bind_address);
	void deinit();
	void tick_(const MarketData& md);

	active_object_helper m_active_object;
	boost::scoped_ptr<zmq::socket_t> m_socket;
};


#endif // zmq_tick_publisher_h__