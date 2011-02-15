#include <stdlib.h>
#include <iostream>

#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/FileStore.h>

#include <zmq.hpp>

#include <zmq_tick_publisher.h>
#include <MQFeederApplication.h>



int main() {
	try{
		FIX::SessionSettings settings( "../mqfeed.cfg" );
		zmq::context_t zmq_ctx(1);
		std::string bind_address = settings.get().getString("BindAddress");
		boost::shared_ptr<tick_publisher> publisher(new zmq_tick_publisher(zmq_ctx, bind_address));
		boost::shared_ptr<MQFeederApplication> app(new MQFeederApplication(settings, publisher));

		// FIX::FileStoreFactory storeFactory(settings);
		FIX::MemoryStoreFactory storeFactory;
		FIX::ScreenLogFactory logFactory( settings );
		FIX::SocketInitiator initiator( *app, storeFactory, settings, logFactory );

		initiator.start();
		std::cout << "Running FIX MQ feeder (Ctrl-C to quit)" << std::endl;
		while(true)
			FIX::process_sleep(1);
		initiator.stop();
		return EXIT_SUCCESS;
	}
	catch ( std::exception & e )
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}


