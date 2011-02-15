#ifndef MQFeederApplication_h__
#define MQFeederApplication_h__

#include <boost/shared_ptr.hpp>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <tick_publisher.h>

namespace fixlib{

class MQFeederApplication
	: public FIX::Application,
	private FIX::MessageCracker

{
public:
	MQFeederApplication(const FIX::SessionSettings&, boost::shared_ptr<tick_publisher> publisher);
	virtual ~MQFeederApplication();

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

	virtual void onMessage( const FIX44::MarketDataSnapshotFullRefresh&, const FIX::SessionID& );

	void sendMarketDataRequest(const FIX::SessionID& sessionID);

	const FIX::SessionSettings& settings;
	boost::shared_ptr<tick_publisher> m_publisher;
};

// functionality used by MQFeederApplication in nice testable chunks
MarketData marketDataSnapshotToMarketData(const FIX44::MarketDataSnapshotFullRefresh& m);
bool isLogonMessage(const FIX::Message& message);
void addUsernameAndPasswordToLogon(FIX::Message& message, const FIX::Dictionary& session_settings);
void buildMarketDataRequest(FIX44::MarketDataRequest& m, const std::string& instruments);

}

#endif // SimulatorApplication_h__

