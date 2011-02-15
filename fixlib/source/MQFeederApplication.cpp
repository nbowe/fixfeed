#include "MQFeederApplication.h"

#include <quickfix/fix44/MarketDataRequest.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix44/Logon.h>
#include <boost/tokenizer.hpp>
#include "MarketData.h"

namespace fixlib {

	MQFeederApplication::MQFeederApplication(const FIX::SessionSettings& s, boost::shared_ptr<tick_publisher> publisher ) 
		:settings(s), m_publisher(publisher)
	{
	}

	MQFeederApplication::~MQFeederApplication()
	{
	}

	void MQFeederApplication::onCreate( const FIX::SessionID& )
	{
	}

	void MQFeederApplication::onLogon( const FIX::SessionID& sessionID )
	{
		std::cout << std::endl << "Logon - " << sessionID << std::endl;
		// send Market Data Request to subscribe to updates

		const FIX::Dictionary& session_settings = settings.get(sessionID);
		std::string instruments_untokenized = session_settings.getString("Instruments");

		sendMarketDataRequest(sessionID);
	}

	void MQFeederApplication::onLogout( const FIX::SessionID& sessionID )
	{
		std::cout << std::endl << "Logout - " << sessionID << std::endl;
	}

	void MQFeederApplication::toAdmin( FIX::Message& message, const FIX::SessionID& sessionID)
	{
		// when we send Logon make sure we add Username and Password
		// I would have like to use a message cracker for this, but theres a bug in quickfix 1.13.3
		// wrt cracking non const messages
		if (isLogonMessage(message)) {
			const FIX::Dictionary& session_settings = settings.get(sessionID);
			addUsernameAndPasswordToLogon(message, session_settings);
		}
	}

	void MQFeederApplication::toApp( FIX::Message& message, const FIX::SessionID& ) 
		throw( FIX::DoNotSend )
	{
	}

	void MQFeederApplication::fromAdmin( const FIX::Message& message, const FIX::SessionID& ) 
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
	{
	}

	void MQFeederApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID ) 
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
	{
		crack( message, sessionID );
	}



	// sends a MarketDataRequest to counterparty defined by sessionID.
	// 
	void MQFeederApplication::sendMarketDataRequest(const FIX::SessionID& sessionID)
	{
		std::string instruments = settings.get(sessionID).getString("Instruments");
		FIX44::MarketDataRequest m;
		buildMarketDataRequest(m, instruments);
		FIX::Session::sendToTarget( m, sessionID );
	}

	void MQFeederApplication::onMessage( const FIX44::MarketDataSnapshotFullRefresh& m, const FIX::SessionID& sessionID)
	{
		MarketData md = marketDataSnapshotToMarketData(m);
		m_publisher->tick(md);
	}


	bool isLogonMessage(const FIX::Message& message) {
		if (FIX::MsgType_Logon == FIELD_GET_REF( message.getHeader(), MsgType))
			return true;
		return false;
	}

	void addUsernameAndPasswordToLogon(FIX::Message& message, const FIX::Dictionary& session_settings){
		assert(isLogonMessage(message));

		// NOTE: annoyingly RTTI doesnt track this as a Logon, just a Message
		FIX44::Logon& logon_message = (FIX44::Logon&)message;
		if (session_settings.has("Username")) {
			FIX::Username username = session_settings.getString("Username");
			logon_message.set( username );
		}
		if (session_settings.has("Password")) {
			FIX::Password password = session_settings.getString("Password");
			logon_message.set( password );
		}
	}


	MarketData marketDataSnapshotToMarketData(const FIX44::MarketDataSnapshotFullRefresh& m) {
		FIX::Symbol s;
		FIX::NoMDEntries NoMDEntriesGroup;

		m.get(s);
		m.get(NoMDEntriesGroup);
		assert(NoMDEntriesGroup == 2);

		MarketData md(s);
		for (int i=1 ; i<=NoMDEntriesGroup; ++i) {
			FIX44::MarketDataSnapshotFullRefresh::NoMDEntries entries_group;
			m.getGroup(i, entries_group);

			FIX::MDEntryType entry_type;
			FIX::MDEntryPx entry_px;
			FIX::MDEntrySize entry_size;

			entries_group.get(entry_type);
			entries_group.get(entry_px);
			entries_group.get(entry_size);
			switch (entry_type)
			{
			case FIX::MDEntryType_BID:
				md.set_bid_price(entry_px);
				md.set_bid_quantity( static_cast<int>(entry_size) );
				break;
			case FIX::MDEntryType_OFFER:
				md.set_offer_price(entry_px);
				md.set_offer_quantity( static_cast<int>(entry_size) );
				break;
			default:
				throw std::logic_error("unsupported MDEntryType received");
			}
		}
		return md;
	}

	void buildMarketDataRequest(FIX44::MarketDataRequest& m, const std::string& instruments){
		// all market data snapshot messages that are a result of this will carry this field.
		// must be unique for the session
		FIX::MDReqID mdReqID("Req01");
		// keep us updated
		FIX::SubscriptionRequestType subType( FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES );
		// just top of book
		FIX::MarketDepth marketDepth(1);

		m.set(mdReqID);
		m.set(subType);
		m.set(marketDepth);
		// each update should be full refresh. (dont want to mess with incremental for simple sample)
		m.set( FIX::MDUpdateType(FIX::MDUpdateType_FULL_REFRESH) );

		// subscribe to bids and offers
		FIX44::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
		marketDataEntryGroup.set( FIX::MDEntryType(FIX::MDEntryType_BID) );
		m.addGroup(marketDataEntryGroup);
		marketDataEntryGroup.set( FIX::MDEntryType(FIX::MDEntryType_OFFER) );
		m.addGroup(marketDataEntryGroup);

		FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
		boost::tokenizer<> tok(instruments);
		for (boost::tokenizer<>::iterator it = tok.begin(); it!=tok.end();++it) {
			symbolGroup.set( FIX::Symbol(*it) );
			m.addGroup(symbolGroup);
		}

	}
}
