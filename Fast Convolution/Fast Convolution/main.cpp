#include "Convolution.h"
#include "Timer.h"


int main(const int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	Convolution filter(argv[1], argv[2]);

#ifdef _FFT
	Timer::Start();
	filter.FastConvolution();
	Timer::Stop();
	printf("Fast Convolution using overlap-save duration: %0.3f seconds\n", Timer::GetDuration());
#else
	Timer::Start();
	filter.Filter();
	Timer::Stop();
	printf("Filter duration: %0.3f seconds\n", Timer::GetDuration());
#endif

	system("pause");
	return 0;
}

