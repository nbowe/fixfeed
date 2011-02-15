import zmq
import bert
import bert.codec

# TODO: ctrl-c doesnt work when its in socket.recv
# TODO: allow specifying messages to accept
	
ctx = zmq.Context()
subscriber = ctx.socket(zmq.SUB)
subscriber.connect("tcp://localhost:5000")
# accept all messages
subscriber.setsockopt (zmq.SUBSCRIBE, "")

def decode(message_data):
	t = bert.decode(message_data)
	(instrument, bid_price, bid_quantity, offer_price, offer_quantity) = t
	return {
		'instrument':bert.codec.list_to_str(instrument),
		'bid_price':bid_price,
		'bid_quantity':bid_quantity,
		'offer_price':offer_price,
		'offer_quantity':offer_quantity,
		}

while True:
	message = decode(subscriber.recv())
	print message

# Subscriber
# takes an address to subscribe to

# Decoder


# MarketData
# object represents the data received over the connection



