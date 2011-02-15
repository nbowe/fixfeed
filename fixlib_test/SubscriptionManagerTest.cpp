#include <gtest/gtest.h>
#include <SubscriptionManager.h>

using namespace fixlib;

struct test_notifier{
	test_notifier(): notified(0){};
	int notified;
	void operator()(const Subscription& sub){++notified;}
};

TEST(SubscriptionManagerTest, addSubscription){
	SubscriptionManager sm;
 
	FIX::SessionID session;
	session.fromString("FIX4.4:sender1->target");
	std::string instrument("Instrument1");
	std::string req_id("req01");

	sm.addSubscription(session, instrument, req_id);
	ASSERT_EQ(1, sm.debug_size());
}

TEST(SubscriptionManagerTest, notify_none){
	test_notifier n;
	SubscriptionManager sm;

	sm.notifySubscribers("Instrument1", boost::ref(n) );
	ASSERT_EQ(0, n.notified);
}

TEST(SubscriptionManagerTest, notify_all){
	test_notifier n;
	SubscriptionManager sm;

	std::string instrument("Instrument1");
	FIX::SessionID session1, session2;
	session1.fromString("FIX4.4:sender1->target");
	session2.fromString("FIX4.4:sender2->target");
	std::string req_id1("req01");
	std::string req_id2("req02");

	sm.addSubscription(session1, instrument, req_id1);
	sm.addSubscription(session2, instrument, req_id2);

	sm.notifySubscribers(instrument, boost::ref(n) );
	ASSERT_EQ(2, n.notified);
}

// check we can remove a subscription
TEST(SubscriptionManagerTest, deleteSubscription){
	SubscriptionManager sm;

	FIX::SessionID session;
	session.fromString("FIX4.4:sender1->target");
	std::string instrument("Instrument1");
	std::string req_id("req01");
	sm.addSubscription(session, instrument, req_id);

	sm.deleteSubscription(session, instrument, req_id);
	ASSERT_EQ(0, sm.debug_size());
}

// check we can remove all for a particular session
TEST(SubscriptionManagerTest, deleteAllSubscriptions){
	test_notifier n;
	SubscriptionManager sm;

	std::string instrument1("Instrument1");
	std::string instrument2("Instrument2");
	FIX::SessionID session1, session2;
	session1.fromString("FIX4.4:sender1->target");
	session2.fromString("FIX4.4:sender2->target");
	std::string req_id1("req01");
	std::string req_id2("req02");

	sm.addSubscription(session1, instrument1, req_id1);
	sm.addSubscription(session1, instrument2, req_id1);
	sm.addSubscription(session2, instrument1, req_id2);

	// delete all for session 1, leaving session2s subscriptions alone
	sm.deleteAllSubscriptions(session1);
	ASSERT_EQ(1, sm.debug_size());
}
