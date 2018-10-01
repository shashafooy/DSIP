#include "Convolution.h"
#include "pgmIO.h"

#pragma warning(disable:4700);


void MultMatrixScalar(const double scalar, double *h, const int m, const int n)
{
	for(auto i=0; i<m; i++)
	{
		for(auto j=0; j<n; j++)
		{
			h[i*m+j] = scalar * h[i*m+j];
		}
	}
}


int main(const int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Wrong number of arguments");
		printf("Format: .exe inFile outFile");
		exit(EXIT_FAILURE);
	}


#define X(u,v) x[(u)*pgmFile.GetHeader().height+(v)]


	Convolution conv {};

	double h[] = {
		1,2,3,2,1,
		2,4,6,4,2,
		3,6,9,6,3,
		2,4,6,4,2,
		1,2,3,2,1
	};
	MultMatrixScalar(1 / 81.0f, h, 5, 5);


	Convolution::Conv2D(argv[1], argv[2], h, 5, 5);
	//Convolution::Sobel(argv[1], argv[2]);



	return 0;
}
