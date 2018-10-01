#pragma once
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>

#pragma warning(disable:4996)

using namespace std;

// ReSharper disable CppInconsistentNaming
class PgmIO
	// ReSharper restore CppInconsistentNaming
{
public:
	PgmIO(const string& inFile, const string& outFile)
	{
		in.open(inFile, ios::in | ios::binary);
		out.open(outFile, ios::binary | ios::out);

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
		HeaderParser();


	}

	typedef struct
	{
		//2 char number to identify file type, pgm is P5
		string magicNumber;

		//comments starting each line with #
		string comments;

		// width of the image
		int width{};

		//height of the image
		int height{};

		//max gray value
		int maxVal{};

	} pgm_file_header;


	void GetData(double * x, int xOffSet = 0, int yOffSet = 0)
	{
		string ss;
		const auto temp = new unsigned char[dataSize];
		in.seekg(dataIndex, ios::beg);
		in.read(reinterpret_cast<char*>(temp), dataSize);
		for (auto i = 0; i < header.height; i++)
		{
			for (auto j = 0; j < header.width; j++)
			{
				x[(i+xOffSet)*header.width + yOffSet+j] = temp[i*header.width+j];
			}
		}

	}

	pgm_file_header GetHeader() const
	{
		return header;
	}

	void WriteData(double *y, pgm_file_header yHeader)
	{
		string dim;
		
		dim.assign(yHeader.magicNumber).append("\n").append(yHeader.comments).append("\n");		
		out.write(dim.c_str(), dim.length());
		dim.assign(std::to_string(yHeader.width)).append(" ").append(std::to_string(yHeader.height)).append("\n");
		out.write(dim.c_str(), dim.length());
		dim.assign(std::to_string(yHeader.maxVal)).append("\n");
		out.write(dim.c_str(), dim.length());

		NormalizeBounds(y);

		auto* temp = new unsigned char[dataSize];
		for (auto i = 0; i < dataSize; i++)
		{
			temp[i] = static_cast<int>(floor(y[i]));
		}

		out.write(reinterpret_cast<char*>(temp), dataSize);
		out.close();
		delete[] temp;
		//free(y);


	}


	int dataSize{};
private:
	ifstream in;
	ofstream out;
	pgm_file_header header;
	int dataIndex{};


	void HeaderParser()
	{
		string buffer = ReadLine();
		if (buffer != "P5")
		{
			printf("Invalid file format");
			exit(EXIT_FAILURE);
		}
		header.magicNumber = buffer;
		do
		{
			buffer = ReadLine();

			if (!buffer.find('#')) header.comments.append(buffer).append("\n");
		} while (!buffer.find('#'));
		header.width = stoi(Split(buffer)[0]);
		header.height = stoi(Split(buffer)[1]);
		header.maxVal = stoi(ReadLine());
		dataIndex = in.tellg();
		dataSize = header.width*header.height;
	}


	string* Split(const std::string& str, const char delim = ' ') const
	{
		auto* cString = new string[10];
		stringstream ss(str);
		string token;
		auto i = 0;
		while (getline(ss, token, delim))
		{
			cString[i++] = token;
		}
		return cString;
	}

	char* ReadByte(const int numBytes = 1)
	{
		const auto buffer = new char[numBytes + 1];
		in.read(buffer, numBytes);

		//string retVal(buffer, numBytes);
		buffer[numBytes] = NULL;
		return buffer;
	}

	string ReadLine()
	{
		string retVal, buffer;
		do
		{
			retVal.append(buffer);
			buffer.assign(ReadByte());
		} while (buffer.find('\n'));
		return retVal;
	}

	void NormalizeBounds(double* p) const
	{
		for (auto i = 0; i < dataSize; i++)
		{
			if (p[i] < 0) p[i] = 0;
			if (p[i] > header.maxVal) p[i] = header.maxVal;
		}
	}
};


