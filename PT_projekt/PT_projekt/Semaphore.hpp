#pragma once


#include <condition_variable>
#include <mutex>


class Semaphore {
public:
	using native_handle_type = std::condition_variable::native_handle_type;

	explicit Semaphore(const unsigned n = 0) : count(n) {}

	void release(const unsigned n = 1);
	void wait(const unsigned n = 1);
	bool try_wait(const unsigned n = 1);
	template<class Rep, class Period>
	bool wait_for(const std::chrono::duration<Rep, Period>& duration, const unsigned n = 1);
	template<class Clock, class Duration>
	bool wait_until(const std::chrono::time_point<Clock, Duration>& timePoint, const unsigned n = 1);

	native_handle_type native_handle() { return cv.native_handle(); }

private:
	unsigned count;
	std::mutex mutex;
	std::condition_variable cv;

public:
	Semaphore(const Semaphore &) = delete;
	Semaphore& operator=(const Semaphore &) = delete;
};

inline void Semaphore::release(const unsigned n) {
	std::lock_guard<std::mutex> lock(mutex);
	count += n;
	cv.notify_one();
}

inline void Semaphore::wait(const unsigned n) {
	std::unique_lock<std::mutex> lock(mutex);
	cv.wait(lock, [=]{ return count >= n; });
	count -= n;
}

inline bool Semaphore::try_wait(const unsigned n) {
	std::lock_guard<std::mutex> lock(mutex);
	if (count >= n) {
		count -= n;
		return true;
	}else
		return false;
}

template<class Rep, class Period>
bool Semaphore::wait_for(const std::chrono::duration<Rep, Period>& duration, const unsigned n) {
	std::unique_lock<std::mutex> lock(mutex);
	auto finished = cv.wait_for(lock, duration, [=]{ return count >= n; });
	if (finished)
		count -= n;
	return finished;
}

template<class Clock, class Duration>
bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& timePoint, const unsigned n) {
	std::unique_lock<std::mutex> lock(mutex);
	auto finished = cv.wait_until(lock, timePoint, [=]{ return count >= n; });
	if (finished)
		count -= n;
	return finished;
}
