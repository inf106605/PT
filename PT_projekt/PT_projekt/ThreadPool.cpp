#include "ThreadPool.hpp"


ThreadPool::ThreadPool(const unsigned threadCount) :
	threads(),
	mutex(),
	cv(),
	newTask(),
	closingThreads(false),
	workingThreads(0)
{
	threads.reserve(threadCount);
	for (unsigned i = 0; i != threadCount; ++i)
		threads.push_back(std::thread([this]{ threadFunction(); }));
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(mutex);
		newTask = task_t([]{});
		closingThreads = true;
	}
	cv.notify_all();
	for (std::thread &thread : threads)
		thread.join();
}

void ThreadPool::startNew(const task_t &task)
{
	std::unique_lock<std::mutex> lock(mutex);
	cv.wait(lock, [this]{ return !bool(newTask); });
	newTask = task;
	lock.unlock();
	cv.notify_all();
	lock.lock();
	cv.wait(lock, [this]{ return !bool(newTask); });
}

void ThreadPool::threadFunction()
{
	for (;;)
	{
		task_t task;
		{
			std::unique_lock<std::mutex> lock(mutex);
			cv.wait(lock, [this]{ return bool(newTask); });
			if (closingThreads)
				break;
			++workingThreads;
			task = newTask;
			newTask = task_t();
		}
		task();
		{
			std::lock_guard<std::mutex> lock(mutex);
			--workingThreads;
		}
		cv.notify_all();
	}
}
