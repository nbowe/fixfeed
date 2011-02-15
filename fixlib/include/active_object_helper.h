#ifndef active_object_helper_h__
#define active_object_helper_h__

#include <concurrent_queue.h>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>


// helper for the Active Object pattern
// see Sutters article at http://www.drdobbs.com/high-performance-computing/225700095
class active_object_helper {
public:
	active_object_helper();

	~active_object_helper();

	void send(const boost::function0<void>& f);
private:

	// gets run on the launched thread
	void run();

	// a message we use to exit the thread
	void exit();

	concurrent_queue< boost::function0<void> > m_queue;
	boost::scoped_ptr<boost::thread> m_thread;
	bool m_exit;
};


#endif // libnb_active_object_helper_h__