#pragma once
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include "dspFileHeader.h"
#include "pgmIO.h"
#include "Convolution.h"
#include "Convolution.h"

using namespace std;

#define M 10
#define X(u,v) x[(u)*colX+(v)]
#define S1(u,v) s1[(u)*colH+(v)]
#define S2(u,v) s2[(u)*colH+(v)]
#define H(u,v) h[(u)*colH+(v)]
#define Y(u,v) y[(u)*colY+(v)]

class Convolution
{
public:
	Convolution() = default;

	static void Conv2D(char* inFile, char* outFile, const double * h, const int hM, const int hN)
	{
		PgmIO pgmFile(inFile, outFile);


		// ReSharper disable CppInconsistentNaming
		const auto rowX = pgmFile.GetHeader().height,
			colX = pgmFile.GetHeader().width,
			rowH = hM,
			colH = hN,
			rowY = rowX,// +(rowH - 1),
			colY = colX,// +colH - 1,
			rowZ = rowX + 2 * (rowH - 1),
			colZ = colX + 2 * (colH - 1);
		// ReSharper restore CppInconsistentNaming

		const auto y = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));

		pgmFile.GetData(x);

		double hx[] = {
			-1,0,1,
			-2,0,2,
			-1,0,1 };
		double hy[] = {
			-1,-2,-1,
			0,0,0,
			1,2,1 };
		for (auto i = 0; i < 10; i++)
		{
			for (auto j = 0; j < 10; j++)
			{
				cout << X(i, j) << " ";
			}
			cout << endl;
		}

		auto tempX = 0.0f, tempY = 0.0f;
		for (auto k = 0; k < rowY; k++)
		{ //Y-row
			for (auto l = 0; l < colY; l++)
			{ //Y-col
				for (int i = 0; i < rowH; i++)
				{ //H-row
					for (int j = 0; j < colH; j++)
					{ //H-col
						tempX += H(i, j)*X(k + i, l + j);
						//tmpy += Hy(i, j)*X(k + i, l + j);
					}
				}
				//auto res = (int) sqrt(pow(tmpx, 2) + pow(tmpy, 2));
				//if (k<rowH - 1 || k>rowY - rowH || l<colH - 1 || l>colY - rowH) res = 0;
				Y(k, l) = tempX;
				tempX = 0.0f, tempY = 0.0f;
			}
		}
		pgmFile.WriteData(y, pgmFile.GetHeader());

		free(y);
		free(x);
	}

	static void Sobel(char* inFile, char* outFile)
	{
		PgmIO pgmFile(inFile, outFile);


		// ReSharper disable CppInconsistentNaming
		const auto rowX = pgmFile.GetHeader().height,
			colX = pgmFile.GetHeader().width,
			rowH = 3,
			colH = 3,
			rowY = rowX,// +(rowH - 1),
			colY = colX,// +colH - 1,
			rowZ = rowX + 2 * (rowH - 1),
			colZ = colX + 2 * (colH - 1);
		// ReSharper restore CppInconsistentNaming

		const auto y = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));

		pgmFile.GetData(x, rowH-1, colH-1);

		double s1[] = {
			1,0,-1,
			2,0,-2,
			1,0,-1 };
		double s2[] = {
			-1,-2,-1,
			0,0,0,
			1,2,1 };
		

		auto tempX = 0.0f, tempY = 0.0f;
		for (auto k = 0; k < rowY; k++)
		{ //Y-row
			for (auto l = 0; l < colY; l++)
			{ //Y-col
				for (int i = 0; i < rowH; i++)
				{ //H-row
					for (int j = 0; j < colH; j++)
					{ //H-col
						tempX += S1(i, j)*X(k + i, l + j);
						tempY += S2(i, j)*X(k + i, l + j);
					}
				}
				//auto res = static_cast<int>(abs(tempX) + abs(tempY));
				auto res = floor(sqrt(pow(tempX, 2) + pow(tempY, 2)));
				//if (k<rowH - 1 || k>rowY - rowH || l<colH - 1 || l>colY - rowH) res = 0;
				Y(k, l) = res;
				tempX = 0.0f, tempY = 0.0f;
			}
		}
		pgmFile.WriteData(y, pgmFile.GetHeader());

		free(y);
		free(x);
	}


private:


};




int getNumSamples(dsp_file_header h)
{
	if (h.ndim == 1) return h.nchan*h.dim0;
	else if (h.ndim == 2) return h.nchan*h.dim0*h.dim1;
	else if (h.ndim == 3) return h.nchan*h.dim0*h.dim1*h.dim2;
	return -1;
}

void Conv(char* impulseFile, char* inFile, char* outFile)
{
	FILE *fx, *fy, *fh;
	dsp_file_header x_header, h_header, y_header;

	fopen_s(&fx, inFile, "rb");
	fopen_s(&fy, outFile, "wb");
	fopen_s(&fh, impulseFile, "rb");
	fread(&x_header, sizeof(dsp_file_header), 1, fx);
	fread(&h_header, sizeof(dsp_file_header), 1, fh);
	// ReSharper disable once CppInconsistentNaming
	const auto Lh = getNumSamples(h_header);
	// ReSharper disable once CppInconsistentNaming
	const auto Lx = getNumSamples(x_header);

	// ReSharper disable once CppInconsistentNaming
	const auto Ly = Lx + Lh - 1;
	// ReSharper disable once CppInconsistentNaming
	const auto Lz = Lx + 2 * (Lh - 1);

	auto*x = static_cast<double*>(calloc(sizeof(double), Lz));
	auto*y = static_cast<double*>(calloc(sizeof(double), Ly));
	auto*h = static_cast<double*>(calloc(sizeof(double), Lh));

	fread(x + Lh - 1, sizeof(double), Lx, fx);
	fread(h, sizeof(double), Lh, fh);
	for (auto i = 0; i < Ly; i++)
	{
		for (auto j = 0; j < Lh; j++)
		{
			y[i] += h[j] * x[i + j];
		}
	}
	y_header = x_header;
	y_header.dim0 = Ly;
	fwrite(&y_header, sizeof(dsp_file_header), 1, fy);
	fwrite(y, sizeof(double), Ly, fy);
	fclose(fx);
	fclose(fy);
	fclose(fh);
}

inline void CircularConv(char* impulseFile, char* inFile, char* outFile)
{
	FILE *fx, *fh, *fy;
	dsp_file_header xHeader, hHeader, yHeader;
	int k;
	double y;


	fopen_s(&fx, inFile, "rb");
	fopen_s(&fy, outFile, "wb");
	fopen_s(&fh, impulseFile, "rb");
	fread(&xHeader, sizeof(dsp_file_header), 1, fx);
	fread(&hHeader, sizeof(dsp_file_header), 1, fh);
	// ReSharper disable once CppInconsistentNaming
	const auto Lh = getNumSamples(hHeader);
	// ReSharper disable once CppInconsistentNaming
	const auto Lx = getNumSamples(xHeader);
	// ReSharper disable once CppInconsistentNaming
	const auto Ly = Lx + Lh - 1;
	// ReSharper restore CppInconsistentNaming

	auto*x = static_cast<double*>(calloc(sizeof(double), Lh));
	auto*h = static_cast<double*>(calloc(sizeof(double), Lh));

	for (int i = 0; i < Lh; i++) x[i] = 0;
	yHeader = xHeader;
	yHeader.dim0 = Ly;
	fwrite(&yHeader, sizeof(dsp_file_header), 1, fy);

	int i = Lh - 1;
	fread(h, sizeof(double), Lh, fh);
	fread(x + i, sizeof(double), 1, fx);
	while (!feof(fx))
	{
		for (k = 0, y = 0.0f; k < Lh; k++)
		{
			y += h[k] * x[(k + i) % Lh];
		}
		i += Lh - 1; i %= Lh;
		fwrite(&y, sizeof(double), 1, fy);
		fread(x + i, sizeof(double), 1, fx);
	}
	x[i] = 0;
	for (int j = 1; j < Lh; j++)
	{ //get tail end of convolution
		for (k = 0, y = 0.0f; k < Lh; k++)
		{
			y += h[k] * x[(k + i) % Lh];
		}
		i += Lh - 1; i %= Lh;
		fwrite(&y, sizeof(double), 1, fy);
		x[i] = 0;
	}
	fclose(fx);
	fclose(fy);
	fclose(fh);

}

inline void Conv2D(char* inFile, char* outFile)
{
#define X(u,v) x[(u)*Cz+(v)]
#define S1(u,v) hx[(u)*Ch+(v)]
#define S2(u,v) hy[(u)*Ch+(v)]
#define Y(u,v) y[(u)*Cy+(v)]
	FILE *fx, *fy;
	dsp_file_header xHeader, yHeader;
	fopen_s(&fx, inFile, "rb");
	fopen_s(&fy, outFile, "wb");

	fread(&xHeader, sizeof(dsp_file_header), 1, fx);
	auto hxHeader = xHeader;
	hxHeader.dim0 = 3;
	hxHeader.dim1 = 3;
	auto hyHeader = hxHeader;

	// ReSharper disable CppInconsistentNaming
	const auto Rx = xHeader.dim0,
		Cx = xHeader.dim1,
		Rh = hxHeader.dim0,
		Ch = hxHeader.dim1, Ry = Rx + (Rh - 1),
		Cy = Cx + Ch - 1,
		Rz = Rx + 2 * (Rh - 1),
		Cz = Cx + 2 * (Ch - 1);
	// ReSharper restore CppInconsistentNaming

	auto* x = static_cast<double*>(calloc(sizeof(double), Rz * Cz));
	auto* y = static_cast<double*>(calloc(sizeof(double), Ry * Cy));
	for (auto i = Rh - 1; i < Rz - Rh + 1; i++)
	{
		fread(x + i * Cz + (Ch - 1), sizeof(double), Cx, fx);
	}

	double hx[] = {
		-1,0,1,
		-2,0,2,
		-1,0,1 };
	double hy[] = {
		-1,-2,-1,
		0,0,0,
		1,2,1 };
	for (auto i = 0; i < 10; i++)
	{
		for (auto j = 0; j < 10; j++)
		{
			cout << X(i, j) << " ";
		}
		cout << endl;
	}

	auto tmpx = 0.0f, tmpy = 0.0f;
	for (auto k = 0; k < Ry; k++)
	{ //Y-row
		for (auto l = 0; l < Cy; l++)
		{ //Y-col
			for (int i = 0; i < Rh; i++)
			{ //H-row
//tmpx = 0.0f; tmpy = 0.0f;
				for (int j = 0; j < Ch; j++)
				{ //H-col
					tmpx += S1(i, j)*X(k + i, l + j);
					tmpy += S2(i, j)*X(k + i, l + j);
				}
			}
			auto res = (int) sqrt(pow(tmpx, 2) + pow(tmpy, 2));
			if (k<Rh - 1 || k>Ry - Rh || l<Ch - 1 || l>Cy - Rh) res = 0;
			Y(k, l) = res;
			tmpx = 0.0f, tmpy = 0.0f;
		}
	}


	yHeader = xHeader;
	yHeader.dim0 = Ry;
	yHeader.dim1 = Cy;
	fwrite(&yHeader, sizeof(dsp_file_header), 1, fy);
	fwrite(y, sizeof(double), Cy*Ry, fy);

	fclose(fx);
	fclose(fy);

}




