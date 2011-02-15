#ifndef MarketData_h__
#define MarketData_h__

#include <string>

namespace fixlib{
class MarketData {
public:
	MarketData():symbol(),bid_price(),bid_quantity(),offer_price(),offer_quantity() {}
	MarketData(const std::string& s):symbol(s),bid_price(),bid_quantity(),offer_price(),offer_quantity() {}
	void set_symbol(const std::string s) {symbol=s;}
	void set_bid_price(double price) {bid_price = price;}
	void set_bid_quantity(int qty) {bid_quantity = qty;}
	void set_offer_price(double price) {offer_price = price;}
	void set_offer_quantity(int qty) {offer_quantity = qty;}
	
	const std::string& get_symbol() const {return symbol;}
	double get_bid_price() const {return bid_price;}
	double get_offer_price() const {return offer_price;}
	int get_bid_quantity() const {return bid_quantity;}
	int get_offer_quantity() const {return offer_quantity;}
private:
	std::string symbol;
	double bid_price;
	int bid_quantity;
	double offer_price;
	int offer_quantity;
};
}
#endif // MarketData_h__
