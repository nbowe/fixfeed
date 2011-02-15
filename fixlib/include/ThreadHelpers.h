#ifndef ThreadHelpers_h__
#define ThreadHelpers_h__

#include <boost/thread.hpp>


// Helpers for threading
// Threading remains an error prone area of software development
// 

namespace thread_helpers {

// LockingPtr
// see http://www.drdobbs.com/184403766
// Its similar to a lock_guard, but combines it with a const_cast to remove the volatile qualifier.
// by tagging our shared resources as volatile we can have the compiler check that we are protecting them correctly
// as we wont be able to call non volatile member functions until we get a non volatile reference using this class.


// mutex
// basic mutex wrapper. Is a model of TimedLockable, so can be used with all the boost::thread functions.
// supports a number of features useful for debugging and profiling.
//  * supports specifying an id. You can only lock mutexes in increasing order.
//  * supports deadlock detection.
//  * supports naming mutexes.
//  * supports querying the thread that holds the lock.
//  * supports hooks for lock/unlock.


// message_queue
//  receive
//  selective_receive
//  push
//  pop


// active_object
// active object is a pattern where an object runs in its own thread
// 	class active_object{
// 		virtual 
//		message_queue
// 		boost::thread thread;
// 	};
}


#endif // ThreadHelpers_h__