#include <stdio.h>
#include "fileRead.h"
#include "PolyPhase Filter.h"

//int Nb = 8; //Length of b array and xbuff array
//double b[] = {
//	0.7179399593918316391238931828411296010017,
//	-5.7179464628775047430053746211342513561249,
//	19.9491802301737202185449859825894236564636,
//	-39.8222408594441787954565370455384254455566,
//	49.7461342665276831098708498757332563400269,
//	-39.8222408594441859008838946465402841567993,
//	19.9491802301737237712586647830903530120850,
//	-5.7179464628775047430053746211342513561249,
//	0.7179399593918316391238931828411296010017
//};
//double xbuff[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
//
//int Na = 8; //Length of a array and ybuff array
//double a[] = {
//	1.0000000000000000000000000000000000000000,
//	-7.3448685676708285186009561584796756505966,
//	23.6656157657255263870865746866911649703979,
//	-43.6881216686887228206614963710308074951172,
//	50.5394019724190428632937255315482616424561,
//	-37.5165701715804402738285716623067855834961,
//	17.4527294007001820830282667884603142738342,
//	-4.6523481930426022756819293135777115821838,
//	0.5441624775502006405858423931931611150503
//};
//double ybuff[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
//
//void shiftRight(double *x, int size)
//{
//	for (int i = size; i > 0; i--)
//	{
//		x[i] = x[i - 1];
//	}
//	x[0] = 0.0;
//}
//
//float filter(float x)
//{
//	double y = 0.0;
//	// Step 1. Shift xbuff and put x into xbuff[0]
//	shiftRight(xbuff, Nb);
//	xbuff[0] = x;
//	// Step 2. Shift ybuff
//	shiftRight(ybuff, Na);
//	// Step 3. Accumulate filter output into y
//	for (int i = 0; i <= Nb; i++)
//	{
//		y += b[i] * xbuff[i];
//	}
//	for (int i = 1; i <= Na; i++)
//	{
//		y -= a[i] * ybuff[i];
//	}
//	// Step 4. Put y into ybuff[0]
//	ybuff[0] = y;
//
//	return static_cast<float>(y);
//}

int main(int argc, char *argv[])
{
	//Print out the impuse response
	if (argc != 3) {
		printf("Wrong number of arguments");
		exit(EXIT_FAILURE);
	}
	//dsp_file_header h;
	//float *x;
	//int insamples;
	//x = inputFile(argv[1], h, insamples);
	//float *y = (float*)calloc(sizeof(float), insamples);
	//

	////impuse response
	///*
	//int N = 20;
	//for (int n = 0; n<N; n++) {
	//	if (n == 0) { printf("h[%4d] = %30.20f\n", n, filter(1.0)); }
	//	else { printf("h[%4d] = %30.20f\n", n, filter(0.0)); }
	//}
	//*/
	//


	//for (int n = 0; n < insamples; n++) {
	//	y[n] = filter(x[n]);
	//}
	//outFile(argv[2], h, y, insamples);
	PolyPhaseFilter filter(argv[1], argv[2], 3, 4);
	filter.Filter();
}

