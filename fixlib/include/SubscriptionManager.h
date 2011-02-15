#ifndef SubscriptionManager_h__
#define SubscriptionManager_h__

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/function.hpp>

#include "Subscription.h"
#include <quickfix/Mutex.h>


// SubscriptionManager
// Purpose: manages which sessions are subscribed to which symbols. (Also tracks req_id as needed by FIX)
// 
// Acting on subscribers is done through notifySubscribers(). This keeps the work done in notifying them decoupled
// from the process of managing the subscriptions. Eg you could use it for more than market data subscriptions (news feeds?).

class SubscriptionManager {
public:
	SubscriptionManager();
	~SubscriptionManager();
	typedef boost::function1<void, const Subscription&> notify_func;

	void addSubscription(const FIX::SessionID&, const std::string& symbol, const std::string& req_id );
	void deleteSubscription(const FIX::SessionID&, const std::string& symbol, const std::string& req_id );
	void deleteAllSubscriptions(const FIX::SessionID&);
	void notifySubscribers(const std::string& symbol, const notify_func& f);

	// the following funcs are used for testing and debugging
	// they should not be used in production code as there is no guarantee that the value will be accurate on return
	// due to the fact that a subscription manager can be accessed from multiple threads
	size_t debug_size() const;
private:

	// tags for our multi index container
	struct session_id_tag{};
	struct subscription_tag{};
	struct symbol_tag{};

	// composite key for all fields of a Subscription
	struct subscription_key : boost::multi_index::composite_key<
		Subscription,
		BOOST_MULTI_INDEX_CONST_MEM_FUN(Subscription, const FIX::SessionID&, get_session_id ),
		BOOST_MULTI_INDEX_CONST_MEM_FUN(Subscription, const std::string&, get_symbol ),
		BOOST_MULTI_INDEX_CONST_MEM_FUN(Subscription, const std::string&, get_req_id ) 
	>{};

	struct subscriptions_container_indices : 
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique<boost::multi_index::tag<subscription_tag>, subscription_key>,
			boost::multi_index::ordered_non_unique< 
				boost::multi_index::tag<session_id_tag>,
				BOOST_MULTI_INDEX_CONST_MEM_FUN(Subscription, const FIX::SessionID&, get_session_id ) 
			>,
			boost::multi_index::ordered_non_unique< 
				boost::multi_index::tag<symbol_tag>,
				BOOST_MULTI_INDEX_CONST_MEM_FUN(Subscription, const std::string&, get_symbol ) 
			>
		>
	{};

	// Container of Subscriptions, that we can index using session,symbol, or req_id
	typedef boost::multi_index_container<Subscription, subscriptions_container_indices> subscriptions_container;
	// indexes into subscriptions_container
	typedef subscriptions_container::index<session_id_tag>::type session_index_type;
	typedef subscriptions_container::index<subscription_tag>::type subscription_index_type;
	typedef subscriptions_container::index<symbol_tag>::type symbol_index_type;

	subscriptions_container subscriptions;
	mutable FIX::Mutex subscriptions_mutex;
};


#endif // SubscriptionManager_h__