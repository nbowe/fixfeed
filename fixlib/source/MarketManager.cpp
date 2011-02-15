#include <MarketManager.h>
#include <MarketData.h>


fixlib::MarketManager::MarketManager()
{
}

fixlib::MarketManager::~MarketManager()
{
}

void fixlib::MarketManager::setMarketData( const MarketData& md )
{
	FIXLIB_PROTECTED_SCOPED_LOCK(checked_data);
	FIXLIB_PROTECTED_CHECK(checked_data, symbol_to_market_data);

	std::string symbol = md.get_symbol();
	SymbolMarketDataMap::iterator it = symbol_to_market_data.find(symbol);
	if (it == symbol_to_market_data.end()) {
		MarketData_ptr p(new MarketData(md));
		symbol_to_market_data.insert( std::make_pair(symbol, p) );
		FIXLIB_PROTECTED_CHECK(checked_data, markets);
		markets.push_back(p);
	}
	else{
		*it->second = md;
	}
}

bool fixlib::MarketManager::symbolExists( const std::string& symbol ) const
{
	FIXLIB_PROTECTED_SCOPED_LOCK(checked_data);
	FIXLIB_PROTECTED_CHECK_CONST(checked_data, symbol_to_market_data);

	SymbolMarketDataMap::const_iterator it = symbol_to_market_data.find(symbol);
	return it != symbol_to_market_data.end();
}

fixlib::MarketData fixlib::MarketManager::getMarketDataForSymbol( const std::string& symbol )
{
	FIXLIB_PROTECTED_SCOPED_LOCK(checked_data);
	FIXLIB_PROTECTED_CHECK(checked_data, symbol_to_market_data);

	SymbolMarketDataMap::const_iterator it = symbol_to_market_data.find(symbol);
	if (it == symbol_to_market_data.end())
		throw std::invalid_argument("symbol not found");
	return *it->second;
}

bool fixlib::MarketManager::empty() const{
	FIXLIB_PROTECTED_SCOPED_LOCK(checked_data);
	FIXLIB_PROTECTED_CHECK_CONST(checked_data, symbol_to_market_data);
	return symbol_to_market_data.empty();
}
size_t fixlib::MarketManager::size() const{
	FIXLIB_PROTECTED_SCOPED_LOCK(checked_data);
	FIXLIB_PROTECTED_CHECK_CONST(checked_data, symbol_to_market_data);
	return symbol_to_market_data.size();
}

fixlib::MarketData fixlib::MarketManager::getMarketDataByIndex( size_t index ) const
{
	FIXLIB_PROTECTED_SCOPED_LOCK(checked_data);
	FIXLIB_PROTECTED_CHECK_CONST(checked_data, markets);
	return *markets.at(index);
}
