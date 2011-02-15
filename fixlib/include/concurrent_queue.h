#ifndef concurrent_queue_h__
#define concurrent_queue_h__

#include <boost/thread.hpp>
#include <queue>

// multiple writer, multiple consumer
// based on Anthony Williams implementation (with added support for bounded size)
// Anthony Williams is the current maintainer of boost::thread
// http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
template<typename T>
class concurrent_queue {
public:

	concurrent_queue():max_elements(0) {}
	concurrent_queue(size_t max):max_elements(max) {}

	// pushes an entry onto the queue.
	// if the queue is at maximum, the current thread waits
	// this helps us avoid producers outpacing the consumer(s) and causing OOM
	void push(const T& v) {
		boost::mutex::scoped_lock l(m_mutex);
		while(max_elements!=0 && m_queue.size() >= max_elements )
		{
			m_cond.wait(l);
		}
		m_queue.push(v);
		l.unlock();
		m_cond.notify_one();
	}

	// pops an element off the queue and returns it
	// if there are no elements in the queue the current thread waits
	void pop(T& v) {
		boost::mutex::scoped_lock l(m_mutex);
		while(m_queue.empty())
		{
			m_cond.wait(l);
		}
		// we cant return by value and maintain strong exception safety because copy ctors can throw
		// if it throws on the return we would have already done the pop. 
		// see http://www.gotw.ca/gotw/008.htm
		v = m_queue.front();
		m_queue.pop();
		m_cond.notify_one();
	}

	// no guarantee that this is accurate as soon as its returned.
	// but may be useful for diagnostics
	bool empty() const {
		boost::mutex::scoped_lock l(m_mutex);
		return m_queue.empty();
	}

	// no guarantee that this is accurate as soon as its returned.
	// but may be useful for diagnostics
	size_t size() const{
		boost::mutex::scoped_lock l(m_mutex);
		return m_queue.size();
	}

	size_t max_size () const {
		boost::mutex::scoped_lock l(m_mutex);
		return max_elements;
	}

private:
	mutable boost::mutex m_mutex;
	std::queue<T> m_queue;
	size_t max_elements;
	boost::condition_variable m_cond;
};


#endif // concurrent_queue_h__