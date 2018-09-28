#include "Convolution.h"
#include "pgmIO.h"

#pragma warning(disable:4700);

int main(const int argc, char *argv[]) {
	if (argc != 3)
	{
		printf("Wrong number of arguments");
		printf("Format: .exe inFile outFile");
		exit(EXIT_FAILURE);
	}


#define X(u,v) x[(u)*pgmFile.GetHeader().height+(v)]

	pgm_file_header header;
	int nSamples;
	PgmIO pgmFile(argv[1], argv[2]);
	float* x = (float*) calloc(sizeof(float), pgmFile.dataSize);
	pgmFile.GetData(x);
	pgmFile.WriteData(x);

	return 0;
}
