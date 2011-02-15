fixfeed
=======
Warning: this is **example code**. 
It may make a useful basis for experiments or building on top of, but it is not in production use.

This projects goal is to help me to get more familiar with FIX, QuickFix, ZeroMQ.
Its also meant to be a usable basis for other work.
Its inspired by the example at http://blog.springsource.com/2010/08/19/building-rabbitmq-apps-using-python/
Except I deliberately dont try to track price trends, and instead just focus on the communication side of things.
I wanted to use technologies that might/could be used in the wild.

It consists of
 * a simulator that speaks the FIX protocol.
   The simulator listens for Market Data Request messages, sends MarketDataSnaphots, 
   and fluctuates the price for various instruments.
 * the MQ feeder
   The feeder connects to a FIX server, subscribes to a configurable set of instruments' market data,
   and publishes market data over ZeroMQ.
 * a python subscriber
   The python subscriber connects to the MQ feeder, and logs market data to screen.
   It would be easy to extend to graph it in realtime, but thats work for another day.
 * tests

Getting it
----------
> git clone --recursive git://github.com/nbowe/fixfeed.git 

This will clone the repo and init its submodules.

Layout
------
 deps: contains libs that we depend on (except boost, which I assume everyone has)
 fixlib: The code library.
 fixlib_test: Unit tests for various functionality in fixlib.
 simulator: Simulates upstream FIX server.
 mqfeeder: Connects to FIX server, logs in, subscribes to market data (instruments configurable in settings file),
           broadcasts received data over zeromq.
 pymqsubscriber: Example consumer of market data from zeromq. just logs it.


Building
--------
You must have boost installed and paths configured. 
I dont include boost in the repo because I figure everyone has it.
To build you will need to build the deps first. This only needs to be done once.

### Build QuickFix.
Open quickfix_vs9.sln.
Build quickfix_vs9 in debug and release.

### Build PyZMQ
This is only if you havent already got a version of pyzmq.
See the docs at https://github.com/zeromq/pyzmq

pyzmq uses cython so you will need to `easy_install cython` if you dont have it.
Then you need to edit setup.cfg to look like

    [build_ext]
    library_dirs = ../zeromq2/lib
    include_dirs = ../zeromq2/include

Then its just `python setup.py install`

You will probably have to copy the (release) zmq.dll into your python directory.

you can then test it with `python -c "import zmq"`
If that doesnt print an ImportError or some other issue then you are good to go.

### Build the Solution
Now we have built our deps, we can build the project itself.
Open fixfeed.sln and build the solution.


Running
-------
You can run the test, simulator and mqfeeder projects from within visual studio.
The expect to be run from the directory that the exe is located in (debug or release), rather than the project root.
This was done to make it easy to run from visual studio (I hate it that 'working directory' is not stored in the vcproj).

First run the simulator. Then run mqfeeder. you should see both windows start to spew FIX messages.

Then run pymqsubscriber with `python pymqsubscriber/pymqsubscriber.py`.
It will connect to mqfeeder and start spewing market data.


Some intresting bits of code
----------------------------
These are 2 different approaches to multithreaded communication/synchronization.
 * message passing, as used by active_object_helper and zmq_tick_publisher
 * manual locking.
Even with the help of LockingPtr and FIXLIB_PROTECTED macros it was much easier using active objects.
A lot less code at the call site.

files: concurrent_queue, active_object_helper, zmq_tick_publisher

The FIXLIB_PROTECTED macros are interesting, in that they allow traditional style locking, but enforce (at runtime) that 
locks are acquired. You simply cant get at your members unless you have a lock.



TODO
----
I really should setup SCons for this so I dont have to deal with braindead msvc build system.

More tests.
In particular I dont test the Application subclasses. This suggests that they could do with a refactor.

Clean up the FIX::Application subclasses. I dont like that they inherit from message cracker, even though it
looks like its the standard way people do things with QuickFix.
(see Item 34 C++ coding standards)

For some reason I cant ctrl-c pymqsubscriber until it returns from zmq's connect or recv functions.
I need to work out how to fix that.

The sequence numbers should reset on logout. The server does this ok, but the client doesnt. 

Support proto buf encoding

Measure latency

Allow the python subscriber to choose the symbols to subscribe to.

Python subscriber for graphing market data in real time.
