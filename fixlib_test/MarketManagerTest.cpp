#include <gtest/gtest.h>
#include <MarketManager.h>
#include <MarketData.h>

using namespace fixlib;


TEST(MarketManagerTest, setMarketData){
	MarketManager mm;
	MarketData md("test");

	mm.setMarketData(md);
	ASSERT_EQ(1, mm.size());
}

TEST(MarketManagerTest, symbolExists){
	MarketManager mm;
	std::string symbol("test");
	MarketData md(symbol);

	mm.setMarketData(md);
	ASSERT_EQ(true, mm.symbolExists(symbol));
}

TEST(MarketManagerTest, getMarketDataForSymbol){
	MarketManager mm;
	std::string symbol("test");
	MarketData md1(symbol);
	md1.set_bid_price(1.0);
	mm.setMarketData(md1);

	MarketData md2 = mm.getMarketDataForSymbol(symbol);
	ASSERT_EQ(md1.get_bid_price(), md2.get_bid_price());
}
