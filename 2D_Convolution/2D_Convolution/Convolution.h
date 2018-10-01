#pragma once
#include <string>
#include "pgmIO.h"
#include "Convolution.h"

using namespace std;

#define X(u,v) x[(u)*colX+(v)]
#define S1(u,v) s1[(u)*colH+(v)]
#define S2(u,v) s2[(u)*colH+(v)]
#define H(u,v) h[(u)*colH+(v)]
#define Y(u,v) y[(u)*colY+(v)]

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
	static void Conv2D(char* inFile, char* outFile, const double * h, const int hM, const int hN)
	{
		PgmIO pgmFile(inFile, outFile);


		// ReSharper disable CppInconsistentNaming
		const auto rowX = pgmFile.GetHeader().height,
			colX = pgmFile.GetHeader().width,
			rowH = hM,
			colH = hN,
			rowY = rowX,
			colY = colX,
			rowZ = rowX + 2 * (rowH - 1),
			colZ = colX + 2 * (colH - 1);
		// ReSharper restore CppInconsistentNaming


		//Allocate and get data
		const auto y = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));

		pgmFile.GetData(x, rowH - 1, colH - 1);


		//Convolve
		auto result = 0.0f;
		for (auto k = 0; k < rowY; k++)
		{ //Y-row
			for (auto l = 0; l < colY; l++)
			{ //Y-col
				for (int i = 0; i < rowH; i++)
				{ //H-row
					for (int j = 0; j < colH; j++)
					{ //H-col
						result += H(i, j)*X(k + i, l + j);
					}
				}
				Y(k, l) = result;
				result = 0.0f;
			}
		}

		//Output to file and cleanup
		pgmFile.WriteData(y, pgmFile.GetHeader());

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

		//Allocate and get data for x
		const auto y = static_cast<double*>(calloc(sizeof(double), rowY * colY));
		const auto x = static_cast<double*>(calloc(sizeof(double), rowZ * colZ));

		//zero pad the bounds of x
		pgmFile.GetData(x, rowH-1, colH-1);

		double s1[] = {
			1,0,-1,
			2,0,-2,
			1,0,-1 };
		double s2[] = {
			-1,-2,-1,
			0,0,0,
			1,2,1 };
		
		//convolve with S1 S2 and combine the result
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
};


