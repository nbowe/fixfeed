#include <gtest/gtest.h>
#include <zmq_tick_publisher.h>
#include <MarketData.h>


using namespace fixlib;

TEST(zmq_tick_publisher_test, tick){
	bool received = false;
	MarketData md("test_symbol");
	const char* endpoint = "inproc://test";
	zmq::context_t ctx(1);
	zmq_tick_publisher pub(ctx, endpoint);

	// zmq needs some time to start the socket before we can connect to it	
	boost::this_thread::sleep(boost::posix_time::millisec(10));
	zmq::socket_t sub( ctx, ZMQ_SUB);
	sub.connect(endpoint);
	sub.setsockopt(ZMQ_SUBSCRIBE, "", 0);

	zmq::message_t msg;
	received = sub.recv(&msg, ZMQ_NOBLOCK);
	ASSERT_EQ(false, received);
	pub.tick(md);
	// needs a little time for the active object thread to process it and for zmq io thread to process it
	boost::this_thread::sleep(boost::posix_time::millisec(30));
	received = sub.recv(&msg, ZMQ_NOBLOCK);
	ASSERT_EQ(true, received);
}
