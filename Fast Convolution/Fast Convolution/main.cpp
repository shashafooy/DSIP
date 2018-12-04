#include "fileRead.h"
#include "Convolution.h"
#include "Timer.h"


int main(const int argc, char *argv[])
{
	//Print out the impuse response
	if (argc < 3)
	{
		printf("Wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	Convolution filter(argv[1], argv[2]);

	Timer::Start();
	filter.Filter();
	Timer::Stop();
	printf("Polyphase duration: %f", Timer::GetDuration());

}

