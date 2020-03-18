#ifndef MLA_H
#define MLA_H

#include <string>
#include <fstream>

struct MyLittleApp // There is no need for making it a class in this example
{
	void run(); // This function is created to make it easier for us to return back after we read one file
	void handleInputs();
	void operateOnFile();
	void compress();
	void decompress();
	void whatToDoNext();

	bool validateFile();
	bool asciiCheckIfNumber(char &p_charToCheck); // Checks for ASCII-encoded numbers only

	bool m_isRunning, m_hasKey;
	std::string m_fileName, m_operatingString, m_compressedVersion, m_decompressedVersion, m_keyString;
	std::fstream m_textFile;
};
#endif