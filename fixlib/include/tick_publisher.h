#ifndef tick_publisher_h__
#define tick_publisher_h__

class MarketData;

class tick_publisher{
	public:
		virtual ~tick_publisher() {}
		virtual void tick(const MarketData& md) = 0;
};

#endif // tick_publisher_h__