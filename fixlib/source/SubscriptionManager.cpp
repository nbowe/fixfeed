#include "SubscriptionManager.h"

#include <list>
#include <boost/foreach.hpp>

using namespace boost::multi_index;


SubscriptionManager::SubscriptionManager(){
}

SubscriptionManager::~SubscriptionManager(){
}

void SubscriptionManager::addSubscription(const FIX::SessionID& session, const std::string& symbol, const std::string& req_id ){
	FIX::Locker l(subscriptions_mutex);
	Subscription subscription(session, symbol, req_id);
	subscriptions.insert(subscription);
}

void SubscriptionManager::deleteSubscription(const FIX::SessionID& session, const std::string& symbol, const std::string& req_id ){
	FIX::Locker l(subscriptions_mutex);
	subscription_index_type& index = subscriptions.get<subscription_tag>();
	// look for exact match
	subscription_index_type::iterator it = index.find( boost::make_tuple(session, symbol, req_id) );
	if (it != index.end())
		index.erase(it);
}

void SubscriptionManager::deleteAllSubscriptions(const FIX::SessionID& session){
	FIX::Locker l(subscriptions_mutex);
	// search by session
	session_index_type& index = subscriptions.get<session_id_tag>();
	index.erase(session);
}


void SubscriptionManager::notifySubscribers( const std::string& symbol, const notify_func& f )
{
	typedef symbol_index_type::iterator iterator;
	std::list<Subscription> targets;
	{
		FIX::Locker l(subscriptions_mutex);
		symbol_index_type& index = subscriptions.get<symbol_tag>();
		
		for (iterator it = index.find(symbol);
			it != index.end() && it->get_symbol() == symbol;
			++it)
		{
			targets.push_back(*it);
		}
	}

	BOOST_FOREACH(const Subscription& s, targets){
		f(s);
	}
}

size_t SubscriptionManager::debug_size() const
{
	FIX::Locker l(subscriptions_mutex);
	return subscriptions.size();
}