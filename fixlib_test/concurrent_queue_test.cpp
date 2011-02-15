#include <gtest/gtest.h>
#include <concurrent_queue.h>


TEST(concurrent_queue_test, ctor_post_conditions) {
	concurrent_queue<int> q;
	ASSERT_EQ(true, q.empty());
	ASSERT_EQ(0, q.size());
}

TEST(concurrent_queue_test, push) {
	concurrent_queue<int> q;
	q.push(1);
	ASSERT_EQ(false, q.empty());
	ASSERT_EQ(1, q.size());
}

TEST(concurrent_queue_test, push_pop) {
	concurrent_queue<int> q;
	q.push(1);
	int v;
	q.pop(v);
	ASSERT_EQ(1, v);
}

// test popping waits
TEST(concurrent_queue_test, empty_pop_waits) {
	typedef concurrent_queue<int> queue;
	queue q;

	struct consumer {
		queue& q;
		consumer(queue& aq):q(aq) {}
		void operator()(){
			int v;
			q.pop(v);
		}
	};

	bool joined = false;
	consumer c(q);
	boost::thread t1(c);
	// we wont be able to join, as the consumer is blocked in pop
	joined = t1.timed_join( boost::posix_time::milliseconds(10) );
	ASSERT_EQ(false, joined);
	// pushing a value will let the consumer proceed
	q.push(1);
	joined = t1.timed_join( boost::posix_time::milliseconds(10) );
	ASSERT_EQ(true, joined);
}

// test pushing at max elements waits
TEST(concurrent_queue_test, full_push_waits) {
	typedef concurrent_queue<int> queue;
	queue q(1);

	struct producer {
		queue& q;
		producer(queue& aq):q(aq) {}
		void operator()(){
			q.push(1);
		}
	};
	// fill the queue
	q.push(1);

	bool joined = false;
	producer c(q);
	boost::thread t1(c);
	// we wont be able to join, as the producer is blocked in push
	joined = t1.timed_join( boost::posix_time::milliseconds(10) );
	ASSERT_EQ(false, joined);
	// popping  a value will let the producer proceed
	int v; q.pop(v);
	joined = t1.timed_join( boost::posix_time::milliseconds(10) );
	ASSERT_EQ(true, joined);
}

// TODO: test multiple threads writing unique values
