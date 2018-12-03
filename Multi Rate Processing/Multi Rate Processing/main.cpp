#include <cstdio>
#include "fileRead.h"
#include "PolyPhase Filter.h"
#include <chrono>
#include "Timer.h"
#include <iostream>

typedef std::chrono::high_resolution_clock Clock;



int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	PolyPhaseFilter filter(argv[1], argv[2], 3, 4);
	if (argc == 4 && !strcmp(argv[3], "poly"))
	{
		Timer::Start();
		filter.PolyFilter();
		Timer::Stop();

		printf("Polyphase duration: %0.3f seconds\n", Timer::GetDuration());
	}
	else
	{

		auto start = Clock::now();
		filter.Filter();
		auto end = Clock::now();
		printf("Filter duration: %lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(end - start).count());
	}
}

