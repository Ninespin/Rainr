#pragma once
#include <chrono>

class Timer
{
public:
	typedef unsigned int handle_t;
private:
	static handle_t current_available_handle_;
public:
	Timer();
	~Timer();
	void restart();
	std::chrono::nanoseconds stop() const;
private:
	std::chrono::steady_clock::time_point begin_time_;
};

