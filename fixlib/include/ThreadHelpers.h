#ifndef ThreadHelpers_h__
#define ThreadHelpers_h__

#include <boost/thread.hpp>

// Helpers for threading
// Threading remains an error prone area of software development
// 

namespace fixlib {

	// LockingPtr
	// see http://www.drdobbs.com/184403766
	// Its similar to a lock_guard, but combines it with a const_cast to remove the volatile qualifier.
	// by tagging our shared resources as volatile we can have the compiler check that we are protecting them correctly
	// as we wont be able to call non volatile member functions until we get a non volatile reference using this class.
	template <typename T, typename Mutex>
	class LockingPtr {
	public:
		// Constructors/destructors
		LockingPtr(volatile T& obj, Mutex& mtx)
			: pObj_(const_cast<T*>(&obj)),
			pMtx_(&mtx)
		{    mtx.lock();    }
		~LockingPtr()
		{    pMtx_->unlock();    }
		// Pointer behavior
		T& operator*()
		{    return *pObj_;    }
		T* operator->()
		{   return pObj_;   }
	private:
		T* pObj_;
		Mutex* pMtx_;
		LockingPtr(const LockingPtr&);
		LockingPtr& operator=(const LockingPtr&);
	};


	// Ensure access to data is protected by appropriate mutex
	// by making it impossible to access the data except through a routine which checks the mutex is held
	// see http://www.drdobbs.com/224701827
	// You can combine it with LockingPtr. They have different purposes
	// REVIEW: some of these macros wont work for external locking. 

	template<typename Mutex>
	class TestableMutex {
	public:
		void lock()     { m.lock(); id = boost::this_thread::get_id(); }
		void unlock()   { id = boost::thread::id(); m.unlock(); }
		bool try_lock() {
			bool b = m.try_lock();
			if (b)
				id = boost::this_thread::get_id();
			return b;  
		}
		bool is_held()  { return id == boost::this_thread::get_id(); }
	private:
		Mutex m;
		// should be atomic<boost::thread::id>
		// but boost doesnt have atomic.
		// in practice its ok though as we assert that its held.
		// so its protected by the mutex
		boost::thread::id id;
	};

#define FIXLIB_PROTECTED_WITH(MutType)	\
	 public:    void    lock() const     {  mut_.lock();  } \
	 public:    bool    try_lock() const {  return mut_.try_lock();  } \
	 public:    void    unlock() const   {  mut_.unlock();  } \
	 private:    mutable TestableMutex<MutType> mut_; \
     public:    typedef TestableMutex<MutType> mutex_t;
#define FIXLIB_PROTECTED_MEMBER(Type,name) \
	 public:    Type&    name()     { assert(mut_.is_held()); return name##_; } \
				const Type& name() const { assert(mut_.is_held()); return name##_; } \
	 typedef Type name##_t;	\
	 private:    Type    name##_;

#define FIXLIB_PROTECTED_CHECK(group, name) \
	group##_t::name##_t& name = group.name();

#define FIXLIB_PROTECTED_CHECK_CONST(group, name) \
	const group##_t::name##_t& name = group.name();

#define FIXLIB_PROTECTED_BEGIN_GROUP(group) \
	struct group##_t {  \
	typedef group##_t my_type

#define FIXLIB_PROTECTED_END_GROUP(group) \
	}; group##_t group

#define FIXLIB_PROTECTED_SCOPED_LOCK(group) \
	boost::unique_lock<const group##_t> l(group);
}

#endif // ThreadHelpers_h__