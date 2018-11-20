#pragma once
#include <stdio.h>
#include<string>
#include "dspFileHeader.h"
#include <fstream>
using namespace std;

class FileRead
{
public:
	FileRead(string inFile, string outFile) : eof(false)
	{
		in.open(inFile, ios::binary);
		out.open(outFile, ios::binary);
		if (!in.is_open())
		{
			printf("Error opening input file\n");
			exit(EXIT_FAILURE);
		}

		if (!out.is_open())
		{
			printf("error opening output file\n");
			exit(EXIT_FAILURE);
		}

		in.read(reinterpret_cast<char*>(&header), sizeof(dsp_file_header));
		dataIndex = in.tellg();
		//write out initial header, will be updated in the destructor
		out.write(reinterpret_cast<char*>(&header), sizeof(dsp_file_header));
	}

	~FileRead()
	{
		//go to beginning and update header
		out.seekp(0);
		out.write(reinterpret_cast<char*>(&header), sizeof(dsp_file_header));

		in.close();
		out.close();
	}

	float GetValue()
	{
		float retVal;
		if (eof) return 0;
		in.read(reinterpret_cast<char*>(&retVal), sizeof(float));
		eof = in.eof();
		return retVal;
	}

	void WriteValue(float val)
	{
		out.write(reinterpret_cast<char*>(&val), sizeof(float));
	}


	//float* inputFile(const char * inFile, dsp_file_header & h, int & nsamples, int read_offset = 0)
	//{
	//	FILE *in;
	//	float *x;
	//	fopen_s(&in, inFile, "rb");
	//	fread(&h, sizeof(dsp_file_header), 1, in);
	//	//fread_s(&h, sizeof(dsp_file_header), sizeof(dsp_file_header), 1, in);
	//	if (h.ndim == 1) nsamples = h.nchan*h.dim0;
	//	else if (h.ndim == 2) nsamples = h.nchan*h.dim0*h.dim1;
	//	else if (h.ndim == 3) nsamples = h.nchan*h.dim0*h.dim1*h.dim2;
	//	x = (float*) calloc(sizeof(float), nsamples);
	//	fread(x, sizeof(float), nsamples, in);
	//	fclose(in);
	//	return x;
	//}
	//void outFile(const char * outFile, dsp_file_header & h, float * x, int nsamples)
	//{
	//	FILE *out;
	//	fopen_s(&out, outFile, "wb");
	//	fwrite(&h, sizeof(dsp_file_header), 1, out);
	//	fwrite(x, sizeof(float), nsamples, out);
	//	fclose(out);
	//	free(x);
	//}


	dsp_file_header header;
	bool eof;

private:
	ifstream in;
	ofstream out;
	int dataIndex {};
};