


Example code. 
Inspired by the example at http://blog.springsource.com/2010/08/19/building-rabbitmq-apps-using-python/

keywords: FIX, protocol buffers, ZeroMQ, Boost



The simulator listens for Market Data Request messages for top of book, snapshots.


In theory it would have been possible to do a transport for quickfix that uses zeromq.
But I wanted this example to be simple, and I think in practice that most people would talk a custom procotol
inside their network.



Layout
------
 simulator: Simulates upstream FIX server sending MarketDataSnapshot
 mqfeeder: Connects to FIX server, logs in, subscribes to market data, broadcasts received data over zeromq
 pymarket: example consumer of market data. just logs it.



Buidling
--------
First build QuickFix.




Running
-------
 


Some intresting bits of code
----------------------------
Simulator uses LockingPtr (of http://www.drdobbs.com/184403766 fame) to make the compiler check that we are
protecting access to resources that we share between threads (well, the volatile ones).
mqfeeder uses zeromq inproc endpoints to send messages between threads.
These are 2 different approaches to multithreaded communication/synchronization.

