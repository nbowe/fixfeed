


Example code. 
Inspired by the example at http://blog.springsource.com/2010/08/19/building-rabbitmq-apps-using-python/

keywords: FIX, protocol buffers, ZeroMQ, Boost



The simulator listens for Market Data Request messages for top of book, snapshots.



In theory it would have been possible to do a transport for quickfix that uses zeromq.
But I wanted this example to be simple, and I think in practice that most people would talk a custom procotol
inside their network.


TODO:
	zmq mq feeder
		PUB thread (boost).
	volatile the shared resources
	break out market sim from simapp
	


Layout
------
 simulator: Simulates upstream FIX server sending MarketDataSnapshot
 mqfeeder: Connects to FIX server, logs in, subscribes to market data, broadcasts received data over zeromq
 proto: contains proto files specifying messages that are sent over zeromq.
 pymarket: example consumer of market data. just logs it.


Running
-------
 
 


Some intresting bits of code
----------------------------
Simulator uses LockingPtr (of http://www.drdobbs.com/184403766 fame) to make the compiler check that we are
protecting access to resources that we share between threads (well, the volatile ones).
mqfeeder uses zeromq inproc endpoints to send messages between threads.
These are 2 different approaches to multithreaded communication/synchronization.








http://www.validfix.com/fix-analyzer.html








this is meant to show that I can work infrastructure


hence i need to be able to 
	proxy FIX -> protobuff + zeromq
	external process for monitoring and displaying market data
	broadcast info for purchases. log


custom settings in quickfix
	eg serialization form for MQ. fix or protofix?



Minimal example of using quickfix with custom tags.
[SESSION] 
BeginString=FIX.4.2 
SenderCompID=BKK 
TargetCompID=AX1 
DataDictionary=../spec/FIX44_ASX.xml 
 





simulater along the lines of
http://blog.springsource.com/2010/08/19/building-rabbitmq-apps-using-python/

but 
 * talks FIX
 * uses zeromq instead of rabbit










gripes with fix
----------------
message checksum algorithm suffers from poor diffusion



TinyFIX
=======
Small library for parsing and formating FIX messages.

Supports custom tags via 
	Message::get_tag<TagType>(int)

Supports custom message types

can register handlers for specific message types


http://www.codeproject.com/KB/cs/fixprotocol.aspx



