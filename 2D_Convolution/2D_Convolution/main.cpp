#include "Convolution.h"
#include "pgmIO.h"


/**
 * \brief Multiplies the given matrix with a scalar
 * \param scalar 
 * \param h matrix to multiply
 * \param m number of rows in matrix
 * \param n number of column in matrix
 */
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
	if (argc < 3)
	{
		printf("Wrong number of arguments");
		printf("Format: .exe inFile outFile");
		exit(EXIT_FAILURE);
	}


	double h[] = {
		1,2,3,2,1,
		2,4,6,4,2,
		3,6,9,6,3,
		2,4,6,4,2,
		1,2,3,2,1
	};
	MultMatrixScalar(1 / 81.0f, h, 5, 5);


	//Convolution::Conv2D(argv[1], argv[2], h, 5, 5);
	//Convolution::Sobel(argv[1], argv[2]);
	Convolution::Correlate(argv[1], argv[2], argv[3]);


	return 0;
}
