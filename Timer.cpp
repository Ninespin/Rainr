#include "Timer.h"

Timer::handle_t Timer::current_available_handle_ = 0;

Timer::Timer()
{
}


Timer::~Timer()
{
}


void Timer::restart()
{
	begin_time_ = std::chrono::high_resolution_clock::now();
}


std::chrono::nanoseconds Timer::stop() const
{
	const auto end_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - begin_time_);
}
