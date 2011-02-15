#include "SimulatorApplication.h"

#include <quickfix/SessionID.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix44/MarketDataRequest.h>
#include <quickfix/fix44/MarketDataRequestReject.h>
#include <boost/foreach.hpp>

#include <SubscriptionManager.h>
#include <MarketManager.h>

namespace fixlib {

	void sendMarketDataRequestReject(const std::string& req_id, const std::string& error_msg, const FIX::SessionID& session_id);
	void sendMarketDataSnapshot(const MarketData&, const FIX::SessionID& session_id);

	SimulatorApplication::SimulatorApplication(const FIX::SessionSettings& s) 
		: settings(s), subscription_manager( new SubscriptionManager ),market_manager( new MarketManager )
	{
		initMarketData();
	}

	SimulatorApplication::~SimulatorApplication()
	{
	}

	void SimulatorApplication::onCreate( const FIX::SessionID& session )
	{
		FIX::Session* psession = FIX::Session::lookupSession(session);
		if (psession)
			psession->reset();
	}

	void SimulatorApplication::onLogon( const FIX::SessionID& sessionID )
	{
	}

	void SimulatorApplication::onLogout( const FIX::SessionID& sessionID )
	{
		subscription_manager->deleteAllSubscriptions(sessionID);
	}

	void SimulatorApplication::toAdmin( FIX::Message&, const FIX::SessionID& )
	{
	}

	void SimulatorApplication::toApp( FIX::Message&, const FIX::SessionID& ) 
		throw( FIX::DoNotSend )
	{
	}

	void SimulatorApplication::fromAdmin( const FIX::Message&, const FIX::SessionID& ) 
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
	{

	}

	void SimulatorApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID ) 
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
	{
		crack( message, sessionID );
	}


	void SimulatorApplication::onMessage( const FIX44::MarketDataRequest& m, const FIX::SessionID& session)
	{
		FIX::MDReqID req_id;
		m.get(req_id);
		try{
			validateMarketDataRequest(m);

			// validate and build a list of symbols.
			// we do this for strong exception safety.
			// if we throw and/or reject this market data request
			// the system will be in essentially the same state (no subscriptions lying around that shouldn't be)
			std::vector<std::string> symbols;
			getSymbolsFromMarketDataRequest(m, symbols);

			FIX::SubscriptionRequestType subscription_type;
			m.get(subscription_type);
			
			BOOST_FOREACH(const std::string& symbol, symbols)
			{
				if (FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES == subscription_type)
				{
					MarketData md = market_manager->getMarketDataForSymbol(symbol);
					sendMarketDataSnapshot(md, session);
					subscription_manager->addSubscription(session, symbol, req_id.getValue());
				}
				else if (FIX::SubscriptionRequestType_SNAPSHOT == subscription_type)
				{
					MarketData md = market_manager->getMarketDataForSymbol(symbol);
					sendMarketDataSnapshot(md, session);
				}
				else if(FIX::SubscriptionRequestType_UNSUBSCRIBE == subscription_type)
				{
					subscription_manager->deleteSubscription(session, symbol, req_id);
				}
			}
		}
		catch (std::exception& e){
			sendMarketDataRequestReject(req_id, e.what(), session);
		}
	}


	// just fill the sim with some random instruments and prices.
	void SimulatorApplication::initMarketData(){
		assert(market_manager->empty());
		std::string instruments[] = {"Instrument1","Instrument2","Instrument3","Instrument4"};
		BOOST_FOREACH(const std::string& instrument, instruments)
		{
			MarketData md(instrument);
			// generate initial bid and offer
			double bid = double(rand()%10 + 1) *1000.0;
			md.set_bid_quantity(1000);
			md.set_offer_quantity(1000);
			md.set_bid_price(bid);
			md.set_offer_price(bid*1.1);
			market_manager->setMarketData(md);
			// symbol_to_market_data.insert( make_pair(instrument,p) );
		}
	}

	// This simulates an event which alters the bid or ask for an instrument.
	// In the real world this could be because of a trade or from an external source
	void SimulatorApplication::simulateMarketEvent()
	{
		// TODO: external locking needed on market_manager
		// except we know that size doesnt shrink.
 		size_t i = rand()%market_manager->size();
		MarketData md = market_manager->getMarketDataByIndex(i);
		double r = double(rand())/double(RAND_MAX);	// 0.0 - 1.0
		double bid =  md.get_bid_price() * (0.9+r*0.2);
		md.set_bid_price(bid);
		md.set_offer_price(bid*1.1);
		market_manager->setMarketData(md);

		// notify all subscribers by sending a MarketDataSnapshot
		struct notify_functor 
		{
			notify_functor(const MarketData& md) : market_data(md) {}
			void operator()(const Subscription& subscriber) 
			{
				sendMarketDataSnapshot(market_data, subscriber.get_session_id());
			}
			const MarketData& market_data;
		};
		subscription_manager->notifySubscribers(md.get_symbol(), notify_functor(md) );
	}

	// validate the requirements on MarketDataRequests that we handle
	// thows a logic_error if any arent met.
	// requirements:
	//  * only top of book
	//  * only full refresh
	void SimulatorApplication::validateMarketDataRequest( const FIX44::MarketDataRequest &m )
	{
		FIX::SubscriptionRequestType subscription_type;
		m.get(subscription_type);
		FIX::MarketDepth depth;
		m.get(depth);
		if (depth != 1)
			throw std::logic_error("unsupported market depth. only top-of-book (depth==1) supported");
		if (FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES == subscription_type)
		{
			FIX::MDUpdateType update_type;
			m.get(update_type);
			if (update_type != FIX::MDUpdateType_FULL_REFRESH)
				throw std::logic_error("unsupported update type. only full refresh supported");
		}
		// TODO: we don't validate uniqueness of req_id.
		// TODO: validate its for OFFER and BID
		// FIX::NoMDEntryTypes num_entry_types;
		// m.get(num_entry_types);
	}

	void SimulatorApplication::getSymbolsFromMarketDataRequest( const FIX44::MarketDataRequest &m, std::vector<std::string> &symbols ) {
		FIX::NoRelatedSym num_symbols;
		m.get(num_symbols);
		symbols.reserve(num_symbols);
		for ( int i = 1; i <= num_symbols; ++i )
		{
			FIX44::MarketDataRequest::NoRelatedSym symbol_group;
			m.getGroup( i, symbol_group );
			FIX::Symbol symbol;
			symbol_group.get( symbol );
			if (!market_manager->symbolExists(symbol))
				throw std::logic_error("unknown symbol "+symbol.getValue());
			symbols.push_back(symbol);
		}
	}

	void sendMarketDataRequestReject(const std::string& req_id, const std::string& error_msg, const FIX::SessionID& session) {
		FIX44::MarketDataRequestReject message;
		message.set( FIX::MDReqID(req_id) );
		// more robust error reporting would use MDReqRejReason field when appropriate
		message.set( FIX::Text(error_msg) );
		FIX::Session::sendToTarget(message, session);
	}


	void sendMarketDataSnapshot(const MarketData& md, const FIX::SessionID& session_id)
	{
		// set the subscription request id
		// msg.set subscription id, 
		// top of market (MarketDepth(264)=1, AggregatedBook(266)=Y)
		FIX44::MarketDataSnapshotFullRefresh message;
		message.set( FIX::Symbol( md.get_symbol() ) );

		FIX44::MarketDataSnapshotFullRefresh::NoMDEntries group;
		group.set(FIX::MDEntryType(FIX::MDEntryType_BID));
		group.set(FIX::MDEntryPx( md.get_bid_price() ));	// best bid price
		group.set(FIX::MDEntrySize( md.get_bid_quantity() ));	// best bid quantity
		message.addGroup(group);

		group.set(FIX::MDEntryType(FIX::MDEntryType_OFFER));
		group.set(FIX::MDEntryPx( md.get_offer_price() ));	// best offer price
		group.set(FIX::MDEntrySize( md.get_offer_quantity() ));	// best offer quantity
		message.addGroup(group);

		FIX::Session::sendToTarget(message, session_id);

	}
}
