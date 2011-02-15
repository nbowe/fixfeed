#ifndef SimulatorApplication_h__
#define SimulatorApplication_h__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>

#include <MarketData.h>


namespace fixlib{

class SubscriptionManager;
class MarketManager;

// Simple Simulator application

class SimulatorApplication
	: public FIX::Application,
	private FIX::MessageCracker
{
public:
	explicit SimulatorApplication(const FIX::SessionSettings&);
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

	void validateMarketDataRequest( const FIX44::MarketDataRequest &m );
	void getSymbolsFromMarketDataRequest( const FIX44::MarketDataRequest &m, std::vector<std::string> &symbols );


	void initMarketData();

	const FIX::SessionSettings& settings;
	boost::scoped_ptr<MarketManager> market_manager;
	boost::scoped_ptr<SubscriptionManager> subscription_manager;
};


}
#endif // SimulatorApplication_h__
