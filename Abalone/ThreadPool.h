#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>


/// \brief Use this class to run tasks in parallel.
class ThreadPool{
public:
	ThreadPool();
	ThreadPool(size_t threads);
	~ThreadPool();

	/// \brief Initialize the ThreadPool with a number of threads.
	/// This method does nothing if the thread pool is already running,
	/// i.e. ThreadPool( size_t ) was called.
	void initializeWithThreads(size_t threads);

	/// \brief Schedule a task to be executed by a thread immediately.
	void schedule(const std::function<void()>&);

	/// \brief a blocking function that waits until the threads have processed all the tasks in the queue.
	void wait() const;

private:
	std::vector<std::thread>            _workers;
	std::queue<std::function<void()>>   _taskQueue;
	std::atomic_uint                    _taskCount;
	std::mutex                          _mutex;
	std::condition_variable             _condition;
	std::atomic_bool                    _stop;
};
