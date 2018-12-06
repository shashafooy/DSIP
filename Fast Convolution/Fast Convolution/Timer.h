#pragma once

#include <chrono>


class Timer
{
public:

	static void Start()
	{
		begin = Clock::now();
	}

	static void Stop()
	{
		end = Clock::now();
	}

	enum Time { seconds, milliseconds };

	static double GetDuration(const Time baseTime = seconds)
	{
		double time;
		if (baseTime == seconds)
		{
			time = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count());
			time += static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) / 1000.0 - time;
		}
		else
		{
			time = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
		}
		return time;
	}


private:
	Timer() = default;

	typedef std::chrono::high_resolution_clock Clock;

	std::chrono::time_point<std::chrono::steady_clock> static begin;
	std::chrono::time_point<std::chrono::steady_clock> static end;
};

std::chrono::time_point<std::chrono::steady_clock> Timer::begin;
std::chrono::time_point<std::chrono::steady_clock> Timer::end;
