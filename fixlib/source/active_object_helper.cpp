#include <active_object_helper.h>
#include <boost/bind.hpp>

namespace fixlib {


	active_object_helper::active_object_helper() :m_exit(false)
	{
		m_thread.reset( new boost::thread( boost::bind(&active_object_helper::run, this) ) );
	}

	active_object_helper::~active_object_helper()
	{
		send( boost::bind(&active_object_helper::exit, this) );
		// wait for queue to drain and thread to exit
		m_thread->join();
	}

	void active_object_helper::send( const boost::function0<void>& f )
	{
		m_queue.push(f);
	}

	void active_object_helper::run()
	{
		boost::function0<void> f;
		while (true){
			m_queue.pop(f);
			f();
			f.clear();
			if (m_exit)
				return;
		}
	}

	void active_object_helper::exit()
	{
		m_exit = true;
	}
}