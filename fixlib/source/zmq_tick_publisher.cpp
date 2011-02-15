#include <zmq_tick_publisher.h>
#include <boost/lexical_cast.hpp>

// move to common
#include "MarketData.h"

zmq_tick_publisher::zmq_tick_publisher( zmq::context_t& ctx, const std::string& bind_address )
{
	m_active_object.send( boost::bind(&zmq_tick_publisher::init, this, boost::ref(ctx), bind_address) );
}

zmq_tick_publisher::~zmq_tick_publisher()
{
	m_active_object.send( boost::bind(&zmq_tick_publisher::deinit, this) );
}

void zmq_tick_publisher::tick( const MarketData& md )
{
	m_active_object.send( boost::bind(&zmq_tick_publisher::tick_, this, md) );
}

void zmq_tick_publisher::init( zmq::context_t& ctx, const std::string& bind_address )
{
	// setup socket. NOTE: all socket interaction must be done in the thread that created the socket
	m_socket.reset( new zmq::socket_t(ctx, ZMQ_PUB) );
	m_socket->bind(bind_address.c_str());
}

void zmq_tick_publisher::deinit()
{
	// teardown socket
	m_socket.reset();
}

// REFACTOR: make its own class (message_encoder) that we inject into zmq_tick_publisher

#include <bert.hpp>
static void encode(const MarketData& md, zmq::message_t& msg){
	std::vector<byte> storage;
	// first add Erlang External Term Format version field 
	// see http://erlang.org/doc/apps/erts/erl_ext_dist.html
	storage.push_back(131);	
	std::back_insert_iterator< std::vector<byte> > it = std::back_inserter(storage);
	it = bert::format_small_tuple_size(5,it);
	
	it = bert::format_string(md.get_symbol(), it);
	it = bert::format(md.get_bid_price(), it);
	it = bert::format(md.get_bid_quantity(), it);
	it = bert::format(md.get_offer_price(), it);
	it = bert::format(md.get_offer_quantity(), it);

	msg.rebuild( storage.size() );
	memcpy( msg.data(), &storage.front(), storage.size());
}

void zmq_tick_publisher::tick_( const MarketData& md )
{
	// encode and broadcast on socket
	zmq::message_t msg;
	encode(md, msg);
	bool success = m_socket->send(msg);
	assert(success);
}

