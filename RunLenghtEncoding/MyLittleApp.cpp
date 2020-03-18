#include <iostream>
#include <limits>
#include <vector>

#include "MyLittleApp.h"

void MyLittleApp::run()
{
	m_isRunning = true;
	std::cout << "This is an app developed as a part of my engineering test.\nPlease keep in mind that this algorithm is convenient only for files with many repetitive characters.\n";
	while (m_isRunning)
	{
		handleInputs();
	}
}

void MyLittleApp::handleInputs()
{
	m_fileName = "";
	std::cout << "Enter name of file that has to be compressed: ";
	std::getline(std::cin, m_fileName);

	while (validateFile() == false) 
	{
		m_fileName = "";
		std::cin.clear();	
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	
		std::cout << "\nCan't open the file, try again: ";
		std::getline(std::cin, m_fileName);
		if (validateFile() == true)
		{
			break;
		}
	}

	operateOnFile();
}

void MyLittleApp::operateOnFile()
{
	m_hasKey = false;
	m_operatingString = ""; 
	m_keyString = "";
	m_textFile.open(m_fileName, std::fstream::in || std::fstream::out);
	while (m_textFile.good())
	{ 
		std::string textToAppend;
		m_textFile >> textToAppend;
		m_operatingString += textToAppend;
	}
	m_textFile.close();

	compress();
}

void MyLittleApp::compress()
{
	m_compressedVersion = "";
	int symbolRepetition = 1, beginningLocation = 0; // The second variable is for decompressing
	size_t timesIterated = 0;
	bool executeOncePerNumber = false, executeOncePerRepetition = false; // The first one is to decide whether we need to make decipher file and the second one is for tracing beginning location of each number
	while (timesIterated < m_operatingString.size()) // Basically, while we iterated less times than the size of our file
	{
		if (executeOncePerRepetition == false)
		{
			beginningLocation = timesIterated;
			executeOncePerRepetition = true;
		}

		if (m_operatingString[timesIterated] == m_operatingString[timesIterated + 1])
		{
			++symbolRepetition; // I am using pre-increment operators a lot because they prevent the computer from spending memory on creating a copy of our variable
			if (symbolRepetition >= 10)
			{
				executeOncePerNumber = true;
				m_hasKey = true;
			}
		}
		else
		{
			if (executeOncePerNumber == true)
			{
				//m_keyString += (m_operatingString[timesIterated] + "(" + std::to_string(symbolRepetition) + ")," + std::to_string(beginningLocation) + "," + std::to_string(timesIterated));
				m_keyString += "(";
				m_keyString += std::to_string(beginningLocation);
				m_keyString += "){";
				m_keyString += m_operatingString[timesIterated];
				m_keyString += "}[";
				m_keyString += std::to_string(symbolRepetition);
				m_keyString += "];";
			}
			executeOncePerNumber = false;
			executeOncePerRepetition = false;
			m_compressedVersion += std::to_string(symbolRepetition) + m_operatingString[timesIterated];
			symbolRepetition = 1;
		}
		++timesIterated;
	}
	std::cout << "\nThe compressed version of your file's contents is: " << m_compressedVersion << "\nCompression ratio is: " << static_cast<float>(m_operatingString.size()) / static_cast<float>(m_compressedVersion.size());
	m_operatingString = "";
	whatToDoNext();
}

void MyLittleApp::decompress()
{
	size_t numbersIterated = 0;
	int timesRepeated; // resultingFileSize variable has to be the same as original (before it was compressed)
	m_decompressedVersion = "";
	std::cout << "Decompressed version: ";
	if (m_hasKey == true)
	{
		std::vector <int> keyLocations;
		std::string currentlyIteratedChar = "";
		size_t numbersIterated2 = 0; // For iterating through cipher string

		while (numbersIterated2 < m_keyString.size())
		{
			if (m_keyString[numbersIterated2] == '(')
			{
				++numbersIterated2;
				while (m_keyString[numbersIterated2] != ')')
				{
					currentlyIteratedChar += m_keyString[numbersIterated2]; // Fill in string with key location

					++numbersIterated2;
				}
				keyLocations.push_back(std::stoi(currentlyIteratedChar)); // And pass it to vector
				currentlyIteratedChar = "";
			}

			++numbersIterated2;
		}

		currentlyIteratedChar = ""; // Used to store to-be-output
		numbersIterated2 = 0; //This variable basically represent the length of the string holding decompressed output
		while (numbersIterated < m_compressedVersion.size())
		{
			if (std::find(keyLocations.begin(), keyLocations.end(), numbersIterated2) != keyLocations.end()) // If current length of the decompressed string is a number at where
			{                                                                                              // a particular character starts repeating more than 10 times (inclusive)
				for (int temp = 0; temp < m_keyString.size(); ++temp)
				{
					if (m_keyString[temp] == '{')
					{
						++temp;
						while (m_keyString[temp] != '}')
						{
							currentlyIteratedChar += m_keyString[temp]; // Stores what character is repeated, usually it is only 1 character
							++temp;
						}
					}
					if (m_keyString[temp] == '[')
					{
						std::string tempBuffer; // Temporary buffer for holding amount of time the character is repeated, which is retrieved from our key
						++temp;
						while (m_keyString[temp] != ']')
						{
							tempBuffer += m_keyString[temp];
							++temp;
							++numbersIterated;
						}
						timesRepeated = std::stoi(tempBuffer);
						m_keyString.erase(m_keyString.begin(), m_keyString.begin() + temp + 2);
						break;
					}
				}
				for (int temp = 0; temp < timesRepeated; ++temp)
				{
					m_decompressedVersion += currentlyIteratedChar;
					++numbersIterated2;
				}
				currentlyIteratedChar = "";
			}

			else // Use simple decompress algorithm if a character does not repeat more than 10 times
			{
				if (asciiCheckIfNumber(m_compressedVersion[numbersIterated]) == true) 
				{
						m_operatingString += m_compressedVersion[numbersIterated];
						++numbersIterated;
				}
				timesRepeated = std::stoi(m_operatingString);
				m_operatingString = "";
				for (int temp = 0; temp < timesRepeated; ++temp)
				{
					m_decompressedVersion += m_compressedVersion[numbersIterated];
					++numbersIterated2;
				}
			}
			++numbersIterated; 
		}
	}

	else // Uses simple decompressing method if no character repeats more than 10 times in the whole string
	{
		while (numbersIterated < m_compressedVersion.size())
		{
			if (asciiCheckIfNumber(m_compressedVersion[numbersIterated]) == true)
			{
				m_operatingString += m_compressedVersion[numbersIterated];
				++numbersIterated;
			}
			timesRepeated = std::stoi(m_operatingString);
			m_operatingString = "";
			for (int temp = 0; temp < timesRepeated; ++temp)
			{
				m_decompressedVersion += m_compressedVersion[numbersIterated];
			}
			++numbersIterated;
		}
	}
	std::cout << m_decompressedVersion;
}

void MyLittleApp::whatToDoNext()
{
	char myAnswer;
	std::cout << "\nWould like me to decompress that file back?\nEnter Y to continue or N to exit the program now.\n";
	std::cin >> myAnswer;
	while ((myAnswer != 'Y' && myAnswer != 'y') && (myAnswer != 'N' && myAnswer != 'n')) // Those kinds of things check for correct input. 
	{                                                                                   // I don't expect you to make an incorrect input but we need to handle this just in case
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "\nSomething went wrong with your input, you should either enter Y or N. Please try again.\n";
		std::cin >> myAnswer;
	}
	if (myAnswer == 'N' || myAnswer == 'n')
	{
		m_isRunning = false;
	}
	else
	{
		decompress();
	}

	std::cout << "\n";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // We are basically cleaning the input buffer from the trash, left after our previous inputs

	std::cout << "\nDo you want to exit the program, or compress another file? Press Y to compress another file or N to exit.\n";
	std::cin >> myAnswer;
	while ((myAnswer != 'Y' && myAnswer != 'y') && (myAnswer != 'N' && myAnswer != 'n'))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "\nSomething went wrong with your input, you should either enter Y or N. Please try again.\n";
		std::cin >> myAnswer;
	}
	if (myAnswer == 'N' || myAnswer == 'n')
	{
		m_isRunning = false;
	}
}

bool MyLittleApp::validateFile()
{
	std::fstream fileToValidate;
	fileToValidate.open(m_fileName);
	if (fileToValidate.is_open())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool MyLittleApp::asciiCheckIfNumber(char &p_charToCheck)
{
	if (p_charToCheck >= 48 && p_charToCheck <= 57)
	{
		return true;
	}
	else
	{
		return false;
	}
}