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
	/**
	 * \brief Attempts to open the given .pgm files and parses the header.
	 * \param inFile Input .pgm
	 * \param outFile Output .pgm
	 */
	PgmIO(const string& inFile, const string& outFile = "noFile")
	{
		in.open(inFile, ios::in | ios::binary);
		if(outFile != "noFile")out.open(outFile, ios::binary | ios::out);

		if (!in.is_open())
		{
			printf("Error opening input file\n");
			exit(EXIT_FAILURE);
		}

		if (outFile != "noFile" && !out.is_open())
		{
			printf("error opening output file\n");
			exit(EXIT_FAILURE);
		}
		HeaderParser();


	}

	~PgmIO()
	{
		in.close();
		out.close();
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


	/**
	 * \brief Parse the data of the input file into a vector. Must allocate space for x before hand
	 * \param x vector to store data into.
	 * \param xOffSet offset for where to read data into x
	 * \param yOffSet offset for where to read data into y
	 */
	void GetData(double * x, const int xOffSet = 0, const int yOffSet = 0)
	{
		string ss;
		const auto temp = new unsigned char[dataSize];
		in.seekg(dataIndex, ios::beg);
		in.read(reinterpret_cast<char*>(temp), dataSize);
		//put image into padded x
		for (auto i = 0; i < header.height; i++)
		{
			for (auto j = 0; j < header.width; j++)
			{
				x[(i+xOffSet)*header.width + yOffSet+j] = temp[i*header.width+j];
			}
		}
		delete[] temp;
	}

	/**
	 * \brief Gets the header of the input file
	 * \return header
	 */
	pgm_file_header GetHeader() const
	{
		return header;
	}

	/**
	 * \brief Writes data to output file .pgm
	 * \param y Data vector to output
	 * \param yHeader Header to use for .pgm
	 * \param mOffset y row offset due to H
	 * \param nOffset y col offset due to H
	 */
	void WriteData(double *y, const pgm_file_header yHeader, const int mOffset = 0, const int nOffset = 0)
	{
		string dim;
		
		dim.assign(header.magicNumber).append("\n").append(header.comments).append("\n");		
		out.write(dim.c_str(), dim.length());
		dim.assign(std::to_string(header.width)).append(" ").append(std::to_string(header.height)).append("\n");
		out.write(dim.c_str(), dim.length());
		dim.assign(std::to_string(header.maxVal)).append("\n");
		out.write(dim.c_str(), dim.length());

		for (auto i = 0; i < yHeader.width*yHeader.height; i++)
		{
			if (y[i] < 0) y[i] = 0;
			if (y[i] > header.maxVal) y[i] = header.maxVal;
		}

		auto* temp = new unsigned char[dataSize];

		for (auto i = 0; i < header.height; i++)
		{
			for(auto j=0; j < header.width; j++)
			{
				temp[i*header.width+j] = static_cast<int>(floor(y[(i+mOffset)*yHeader.width+j+nOffset]));

			}
		}

		out.write(reinterpret_cast<char*>(temp), dataSize);
		

		out.close();
		delete[] temp;
	}


	int dataSize{};
private:
	ifstream in;
	ofstream out;
	pgm_file_header header;
	int dataIndex{};


	/**
	 * \brief Parse the .pgm header and stores the data into the header var
	 */
	void HeaderParser()
	{
		auto buffer = ReadLine();
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
		auto size = 0;
		header.width = stoi(Split(buffer, size)[0]);
		if(size>1)	header.height = stoi(Split(buffer, size)[1]);
		else
		{
			header.height = stoi(ReadLine());
		}
		header.maxVal = stoi(ReadLine());
		dataIndex = in.tellg();
		dataSize = header.width*header.height;
	}


	/**
	 * \brief Split the given string into an array of strings
	 * \param str string to parse
	 * \param size size of the output array
	 * \param delim delimiter to separate strings
	 * \return array of strings. max size 10.
	 */
	string* Split(const std::string& str, int &size, const char delim = ' ') const
	{
		auto* cString = new string[10];
		stringstream ss(str);
		string token;
		auto i = 0;
		size = 0;
		while (getline(ss, token, delim))
		{
			cString[i++] = token;
			size++;
		}
		return cString;
	}

	/**
	 * \brief Read in n bytes (unsigned char)
	 * \param numBytes number of bytes to read in, default 1
	 * \return c string with the given byte. Terminated in NULL
	 */
	char* ReadByte(const int numBytes = 1)
	{
		const auto buffer = new char[numBytes + 1];
		in.read(buffer, numBytes);

		//string retVal(buffer, numBytes);
		buffer[numBytes] = NULL;
		return buffer;
	}


	/**
	 * \brief Read in the line up until the delimiter
	 * \param delim delimiter for when to stop looking. Default new line
	 * \return The parsed line NOT including the delimiter 
	 */
	string ReadLine(const char delim = '\n')
	{
		string retVal, buffer;
		do
		{
			retVal.append(buffer);
			buffer.assign(ReadByte());
		} while (buffer.find(delim));
		return retVal;
	}

	/**
	 * \brief Normalizes the upper and lower limits of the signal.
	 * Anything lower than 0 becomes 0. 
	 * Anything higher than the header maxVal becomes maxVal
	 * \param p Data to normalize
	 */
	void NormalizeBounds(double* p) const
	{
		for (auto i = 0; i < dataSize; i++)
		{
			if (p[i] < 0) p[i] = 0;
			if (p[i] > header.maxVal) p[i] = header.maxVal;
		}
	}
};


