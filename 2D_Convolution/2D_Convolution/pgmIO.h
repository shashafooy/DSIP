#pragma once
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>

#pragma warning(disable:4996)

using namespace std;

class PgmIO
{
public:
	PgmIO(string inFile, string outFile)
	{
		in.open(inFile, ios::in | ios::binary);
		out.open(outFile, ios::binary | ios::out);

		//fopen_s(&in, inFile, "rb");
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
		int width;

		//height of the image
		int height;

		//max gray value
		int maxVal;

	} pgm_file_header;


	void GetData(float * x)
	{
		string ss;
		auto temp = new unsigned char[dataSize];
		//*x = (float*) calloc(sizeof(float), dataSize);
		in.seekg(dataIndex, ios::beg);
		in.read((char*) temp, dataSize);
		for (int i = 0; i < dataSize; i++)
		{
			x[i] = temp[i];
			//cout << x[i] << " ";
		}

	}

	pgm_file_header GetHeader() const
	{
		return header;
	}

	void WriteData(float *x)
	{
		string dim;
		const char* t;
		int n;
		
		dim.assign(header.magicNumber).append("\n").append(header.comments).append("\n");		
		out.write(dim.c_str(), dim.length());
		dim.assign(std::to_string(header.width)).append(" ").append(std::to_string(header.height)).append("\n");
		out.write(dim.c_str(), dim.length());
		dim.assign(std::to_string(header.maxVal)).append("\n");
		out.write(dim.c_str(), dim.length());

		NormalizeBounds(x);

		unsigned char* temp = new unsigned char[dataSize];
		for (int i = 0; i < dataSize; i++)
		{
			temp[i] = x[i];
		}

		out.write((char*)temp, dataSize);
		out.close();
		delete[] temp;
		free(x);


	}


	int dataSize;
private:
	ifstream in;
	ofstream out;
	pgm_file_header header;
	int dataIndex;


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


	string* Split(const std::string& str, char delim = ' ')
	{
		string* cstring = new string[10];
		stringstream ss(str);
		string token;
		int i = 0;
		while (getline(ss, token, delim))
		{
			cstring[i++] = token;
		}
		return cstring;
	}

	char* ReadByte(const int numBytes = 1)
	{
		auto buffer = new char[numBytes + 1];
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

	void NormalizeBounds(float* p)
	{
		for (auto i = 0; i < dataSize; i++)
		{
			if (p[i] < 0) p[i] = 0;
			if (p[i] > header.maxVal) p[i] = header.maxVal;
		}
	}
};
typedef struct
{
	//2 char number to identify file type, pgm is P5
	string magicNumber;

	//whitespace
	char _blank1;

	// width of the image
	int width;

	//whitespace
	char _blank2;

	//height of the image
	char height;

	//whitespace
	char _blank3;

	//max gray value
	char maxVal;

	//whitespace
	char _blank4;

} pgm_file_header;

//inline string ReadByte(ifstream fin, const int numBytes = 1)
//{
//	cout << "pos before: " << fin.tellg();
//	auto buffer = new char [numBytes];
//	fin.read(buffer, numBytes);
//
//	string retVal(buffer,numBytes);
//	cout << retVal << "pos after: " << fin.tellg() << endl;
//
//	//const auto buffer = new char[numBytes];
//	/*char* buffer;
//	string buf;
//	fread(buffer, sizeof(char), numBytes, fin);
//	getline();
//	string retVal(buffer);
//	delete[] buffer;*/
//	return retVal;
//}
//
//inline pgm_file_header HeaderParser(ifstream fin)
//{
//	pgm_file_header h;
//	const string magicNum = ReadByte(std::move(fin), 2);
//	h.magicNumber = magicNum;
//	ReadByte(std::move(fin));
//	ReadByte(std::move(fin));
//	ReadByte(std::move(fin));
//	ReadByte(std::move(fin));
//	ReadByte(std::move(fin));
//	ReadByte(std::move(fin));
//	fin.seekg(sizeof(char), ios::cur);
//	ReadByte(std::move(fin));
//	return h;
//}



inline float* InputFile(string inFile, pgm_file_header & h, int & nsamples)
{
	ifstream in;

	in.open(inFile, ios::in | ios::binary);

	//fopen_s(&in, inFile, "rb");
	if (!in.is_open())
	{
		printf("Error opening input file");
		exit(EXIT_FAILURE);
	}
	//HeaderParser(std::move(in));
	//fread(&h, sizeof(pgm_file_header), 1, in);
	//fread_s(&h, sizeof(dsp_file_header), sizeof(dsp_file_header), 1, in);
	nsamples = h.width*h.height;
	auto* x = static_cast<float*>(calloc(sizeof(float), nsamples));
	//fread(x, sizeof(unsigned char), nsamples, in);
	//fclose(in);
	return x;
}

/**
 * \brief Normalizes the bounds of the input digital signal to be within 0-255
 * \param p given array to check and modify
 * \param nsamples number of samples in p
 */
inline void NormalizeBounds(float* p, const int nsamples)
{
	for (auto i = 0; i < nsamples; i++)
	{
		if (p[i] < 0) p[i] = 0;
		if (p[i] > 255) p[i] = 255;
	}
}

inline void OutFile(char const* outFile, pgm_file_header& h, float* x, const int nsamples)
{
	FILE *out;
	fopen_s(&out, outFile, "wb");
	if (out == nullptr)
	{
		printf("Error opening output file");
		exit(EXIT_FAILURE);
	}
	NormalizeBounds(x, nsamples);
	fwrite(&h, sizeof(pgm_file_header), 1, out);
	fwrite(x, sizeof(unsigned char), nsamples, out);
	fclose(out);
	free(x);
}
