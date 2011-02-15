#ifndef MQFeederApplication_h__
#define MQFeederApplication_h__

#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <tick_publisher.h>
#include <boost/shared_ptr.hpp>

// Simple Simulator application
// 
// accepts logon from the client, and 

class MQFeederApplication
	: public FIX::Application,
	private FIX::MessageCracker

{
public:
	MQFeederApplication(const FIX::SessionSettings&, boost::shared_ptr<tick_publisher> publisher);
	virtual ~MQFeederApplication();

private:
	void onCreate( const FIX::SessionID& );
	void onLogon( const FIX::SessionID& sessionID );
	void onLogout( const FIX::SessionID& sessionID );
	void toAdmin( FIX::Message&, const FIX::SessionID& );
	void toApp( FIX::Message&, const FIX::SessionID& )
		throw( FIX::DoNotSend );
	void fromAdmin( const FIX::Message&, const FIX::SessionID& )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
	void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

	void onMessage( const FIX44::MarketDataSnapshotFullRefresh&, const FIX::SessionID& );

	void sendMarketDataRequest(const FIX::SessionID& sessionID);

	const FIX::SessionSettings& settings;
	boost::shared_ptr<tick_publisher> m_publisher;
};


#endif // SimulatorApplication_h__
