#pragma once
#include <string>
#include "pgmIO.h"
#include "Convolution.h"
#include <algorithm>
#include <iostream>

using namespace std;


class Convolution
{
public:
	Convolution() = default;

	/**
	 * \brief 2D Convolve the given signal with a given h matrix. Writes out the result to a file
	 * \param inFile Input .pgm file
	 * \param outFile Output .pgm file
	 * \param h h matrix set up as a vector (h[x*numCol+y])
	 * \param hM number of rows in h
	 * \param hN number of columns in h
	 */
	static void Conv2D(char* inFile, char* outFile, double* h, const int hM, const int hN)
	{
		PgmIO pgmFile(inFile, outFile);


		// ReSharper disable CppInconsistentNaming
		const auto rowX = pgmFile.GetHeader().height,
		           colX = pgmFile.GetHeader().width,
		           rowZ = rowX + 2 * (hM - 1),
		           colZ = colX + 2 * (hN - 1),
		           rowY = rowX + hM - 1,
		           colY = colX + hN - 1;
		// ReSharper restore CppInconsistentNaming


		//Allocate and get data
		const auto y = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));

		pgmFile.GetData(x, hM - 1, hN - 1);


		Convolve2D(y, rowY, colY, x, rowX, colX, h, hM, hN);
		auto yHeader = pgmFile.GetHeader();
		yHeader.width = colY;
		yHeader.height = rowY;

		//Output to file and cleanup
		pgmFile.WriteData(y, yHeader, hM - 2, hN - 2);

		free(y);
		free(x);
	}

	/**
	 * \brief 2D Convolve the given signal with two matrices S1 and S2. The output is
	 * edge detection of the given image. Writes out the result to a file
	 * \param inFile Input .pgm file
	 * \param outFile Output .pgm file
	 */
	static void Sobel(char* inFile, char* outFile)
	{
#define X(u,v) x[(u)*colX+(v)]
#define S1(u,v) s1[(u)*colH+(v)]
#define S2(u,v) s2[(u)*colH+(v)]
#define Y(u,v) y[(u)*colY+(v)]


		PgmIO pgmFile(inFile, outFile);


		// ReSharper disable CppInconsistentNaming
		const auto rowX = pgmFile.GetHeader().height,
		           colX = pgmFile.GetHeader().width,
		           rowH = 3,
		           colH = 3,
		           rowY = rowX, // +(rowH - 1),
		           colY = colX, // +colH - 1,
		           rowZ = rowX + 2 * (rowH - 1),
		           colZ = colX + 2 * (colH - 1);
		// ReSharper restore CppInconsistentNaming

		//Allocate and get data for x
		const auto y = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));

		//zero pad the bounds of x
		pgmFile.GetData(x, rowH - 1, colH - 1);

		double s1[] = {
			1, 0, -1,
			2, 0, -2,
			1, 0, -1
		};
		double s2[] = {
			-1, -2, -1,
			0, 0, 0,
			1, 2, 1
		};

		//convolve with S1 S2 and combine the result
		auto tempX = 0.0f, tempY = 0.0f;
		for (auto k = 0; k < rowY; k++)
		{
			//Y-row
			for (auto l = 0; l < colY; l++)
			{
				//Y-col
				for (auto i = 0; i < rowH; i++)
				{
					//H-row
					for (auto j = 0; j < colH; j++)
					{
						//H-col
						tempX += S1(i, j) * X(k + i, l + j);
						tempY += S2(i, j) * X(k + i, l + j);
					}
				}
				const auto res = static_cast<int>(abs(tempX) + abs(tempY));
				Y(k, l) = res;
				tempX = 0.0f, tempY = 0.0f;
			}
		}

		//write data out to file and cleanup
		pgmFile.WriteData(y, pgmFile.GetHeader());

		free(y);
		free(x);
	}

	static void Correlate(char* inFile, char* outFile, char* hFile)
	{
		PgmIO xPgm(inFile, outFile);
		PgmIO hPgm(hFile);

		auto yHeader = xPgm.GetHeader();
		const auto rowX = xPgm.GetHeader().height,
		           colX = xPgm.GetHeader().width,
		           rowH = hPgm.GetHeader().height,
		           colH = hPgm.GetHeader().width,
		           rowZ = rowX + 2 * (rowH - 1),
		           colZ = colX + 2 * (colH - 1),
		           rowY = rowX + (rowH - 1),
		           colY = colX + (colH - 1);

		yHeader.width = colY;
		yHeader.height = rowY;

		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));
		const auto h = static_cast<double*>(calloc(sizeof(double), rowH * colH));
		const auto x2 = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));
		const auto h2 = static_cast<double*>(calloc(sizeof(double), rowH * colH));
		const auto y1 = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto y2 = static_cast<double*>(calloc(sizeof(double), rowY * colY));


		xPgm.GetData(x, rowH - 1, colH - 1);
		hPgm.GetData(h);

		for (auto i = 0; i < rowZ * colZ; i++)
		{
			x2[i] = pow(x[i], 2);
		}
		for (auto i = 0; i < rowH * colH; i++)
		{
			h2[i] = 1;
		}
		Convolve2D(y2, rowY, colY, x2, rowX, colX, h2, rowH, colH);
		Convolve2D(y1, rowY, colY, x, rowX, colX, h, rowH, colH);
		for (auto i = 0; i < rowY * colY; i++)
		{
			y1[i] /= y2[i];
		}
		ScaleValues(y1, rowY * colY, 255);

		for (auto i = 350; i < 375; i++)
		{
			for (auto j = 25; j < 50; j++)
			{
				cout << static_cast<int>(floor(y1[i * rowY + j])) << " ";
			}
			cout << endl;
		}

		xPgm.WriteData(y1, yHeader, hPgm.GetHeader().height - 1, hPgm.GetHeader().width - 1);
		free(x);
		free(h);
		free(x2);
		free(h2);
		free(y1);
		free(y2);
	}

private:
	static void Convolve2D(double* y, const int yM, const int yN, const double* x, const int xM, const int xN,
	                       const double* h, const int hM, const int hN)
	{
#define Y(u,v) y[(u)*yN+(v)]
#define X(u,v) x[(u)*xN+(v)]
#define H(u,v) h[(u)*hN+(v)]


		//Convolve
		auto result = 0.0f;
		for (auto k = 0; k < yM; k++)
		{
			//Y-row
			for (auto l = 0; l < yN; l++)
			{
				//Y-col
				for (auto i = 0; i < hM; i++)
				{
					//H-row
					for (auto j = 0; j < hN; j++)
					{
						//H-col
						result += H(i, j) * X(k + i, l + j);
					}
				}
				Y(k, l) = result;
				result = 0.0f;
			}
		}
	}

	static void ScaleValues(double* y, const int n, const double maxOut)
	{
		const auto maxVal = *max_element(y, y + n);
		for (auto i = 0; i < n; i++)
		{
			y[i] = y[i] / maxVal * maxOut;
		}
	}
};
