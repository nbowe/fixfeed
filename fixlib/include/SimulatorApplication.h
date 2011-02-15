#ifndef SimulatorApplication_h__
#define SimulatorApplication_h__

#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <map>

#include "MarketData.h"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

class SubscriptionManager;


// Simple Simulator application

class SimulatorApplication
	: public FIX::Application,
	private FIX::MessageCracker
{
public:
	SimulatorApplication(const FIX::SessionSettings&);
	virtual ~SimulatorApplication();

	// 
	void simulateMarketEvent();
private:
	virtual void onCreate( const FIX::SessionID& );
	virtual void onLogon( const FIX::SessionID& sessionID );
	virtual void onLogout( const FIX::SessionID& sessionID );
	virtual void toAdmin( FIX::Message&, const FIX::SessionID& );
	virtual void toApp( FIX::Message&, const FIX::SessionID& )
		throw( FIX::DoNotSend );
	virtual void fromAdmin( const FIX::Message&, const FIX::SessionID& )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
	virtual void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );


	virtual void onMessage( const FIX44::MarketDataRequest&, const FIX::SessionID& );



	typedef boost::shared_ptr<MarketData> MarketData_ptr;
	typedef std::vector<MarketData_ptr> MarketDataVec;
	typedef std::map<std::string, MarketData_ptr> SymbolMarketDataMap;
	MarketDataVec markets;
	SymbolMarketDataMap symbol_to_market_data;
	FIX::Mutex market_mutex;
	void initMarketData();
	void simulateMarketEvent(MarketData_ptr);
	// get a snapshot of MarketData for a symbol at a point in time.
	MarketData getMarketDataForSymbol(const std::string& symbol);
	bool symbolExists(const std::string& symbol) const;


	const FIX::SessionSettings& settings;
	boost::scoped_ptr<SubscriptionManager> subscription_manager;
};



#endif // SimulatorApplication_h__
