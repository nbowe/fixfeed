
#include <gtest/gtest.h>
#include <MarketData.h>

TEST(MarketDataTest, default_ctor ){
	MarketData md;
	EXPECT_EQ("", md.get_symbol());
	EXPECT_EQ(0.0, md.get_bid_price());
	EXPECT_EQ(0, md.get_bid_quantity());
}

TEST(MarketDataTest, setters_getters ){
	MarketData md("test");
	md.set_bid_price(1.0);
	md.set_bid_quantity(2);
	EXPECT_EQ("test", md.get_symbol());
	EXPECT_EQ(1.0, md.get_bid_price());
	EXPECT_EQ(2, md.get_bid_quantity());
}


