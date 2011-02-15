#include <stdlib.h>
#include <iostream>

#include <SimulatorApplication.h>
#include <quickfix/SocketAcceptor.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/FileStore.h>


int main() {
	try{
		FIX::SessionSettings settings( "../simulator.cfg" );
		SimulatorApplication app(settings);
		// FIX::FileStoreFactory storeFactory(settings);
		FIX::MemoryStoreFactory storeFactory;
		FIX::ScreenLogFactory logFactory( settings );
		FIX::SocketAcceptor acceptor( app, storeFactory, settings, logFactory );

		acceptor.start();
		std::cout << "Running Server simulator (Ctrl-C to quit)" << std::endl;
		while(true)
		{
			FIX::process_sleep(0.1);
			app.simulateMarketEvent();
		}
		acceptor.stop();
		return EXIT_SUCCESS;
	}
	catch ( std::exception & e )
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}


