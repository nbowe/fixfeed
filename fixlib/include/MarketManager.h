#ifndef MarketManager_h__
#define MarketManager_h__

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <ThreadHelpers.h>

namespace fixlib{
	class MarketData;

	// manages the list of symbols and current market data for those symbols
	class MarketManager {
	public:
		MarketManager();
		~MarketManager();

		void setMarketData(const MarketData& md);
		bool symbolExists(const std::string& symbol) const;
		// returns a copy of the data for thread safety
		MarketData getMarketDataForSymbol(const std::string& symbol);

		MarketData getMarketDataByIndex(size_t index) const;

		bool empty() const;
		size_t size() const;
	private:
		typedef boost::shared_ptr<MarketData> MarketData_ptr;
		typedef std::map<std::string, MarketData_ptr> SymbolMarketDataMap;
		typedef std::vector<MarketData_ptr> MarketDataVec;

		FIXLIB_PROTECTED_BEGIN_GROUP(checked_data);
			FIXLIB_PROTECTED_WITH(boost::mutex);
			FIXLIB_PROTECTED_MEMBER(SymbolMarketDataMap, symbol_to_market_data);
			FIXLIB_PROTECTED_MEMBER(MarketDataVec, markets);
		FIXLIB_PROTECTED_END_GROUP(checked_data);
	};

}
#endif // MarketManager_h__
