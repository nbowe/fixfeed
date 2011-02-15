#ifndef zmq_tick_publisher_h__
#define zmq_tick_publisher_h__

#include <string>
#include <boost/scoped_ptr.hpp>
#include <zmq.hpp>
#include <active_object_helper.h>
#include <tick_publisher.h>


namespace fixlib{

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
	zmq::socket_t* m_socket;
};
}


#endif // zmq_tick_publisher_h__
