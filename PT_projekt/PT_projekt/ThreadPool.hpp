#pragma once


#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <memory>


class ThreadPool
{
public:
	typedef std::function<void()> task_t;

	ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}
	ThreadPool(const unsigned threadCount);
	~ThreadPool();

	void startNew(const task_t &task);
	void joinAll() { std::unique_lock<std::mutex> lock(mutex); cv.wait(lock, [this]{ return workingThreads == 0; }); }

	unsigned size() const { return threads.size(); }

private:
	void threadFunction();

	std::vector<std::thread> threads;
	std::mutex mutex;
	std::condition_variable cv;
	task_t newTask;
	bool closingThreads;
	unsigned workingThreads;

public:
	ThreadPool(const ThreadPool &) = delete;
	ThreadPool& operator=(const ThreadPool &) = delete;
};
