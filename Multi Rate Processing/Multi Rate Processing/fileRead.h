#pragma once
#include <cstdio>
#include<string>
#include "dspFileHeader.h"
#include <fstream>
using namespace std;

class FileRead
{
public:
	FileRead(const string& inFile, const string& outFile) : eof(false)
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

		in.read(reinterpret_cast<char*>(&inFileHeader), sizeof(dsp_file_header));
		outFileHeader = inFileHeader;
		outFileHeader.dim0 = 0;
		dataIndex = in.tellg();
		//write out initial header, will be updated in the destructor
		out.write(reinterpret_cast<char*>(&outFileHeader), sizeof(dsp_file_header));
	}

	~FileRead()
	{
		//go to beginning and update header
		out.seekp(0);
		out.write(reinterpret_cast<char*>(&outFileHeader), sizeof(dsp_file_header));

		in.close();
		out.close();
	}

	float GetValue()
	{
		float retVal;
		if (eof) return 0.0f;
		in.read(reinterpret_cast<char*>(&retVal), sizeof(float));
		eof = in.eof();
		if (eof) retVal = 0.0f;
		return retVal;
	}

	void WriteValue(float val)
	{
		out.write(reinterpret_cast<char*>(&val), sizeof(float));
		outFileHeader.dim0++;
	}

	void goToStartOfData()
	{
		in.seekg(dataIndex);
	}


	dsp_file_header inFileHeader {};
	dsp_file_header outFileHeader {};
	bool eof;

private:
	ifstream in;
	ofstream out;
	fstream::pos_type dataIndex {};
};