#include "FileCSV.h"

#include <COMSTRConversion.h>
#include <COMSTREditing.h>

#include <fstream>
#include <vector>
#include <assert.h>

fio::FileCSV::FileCSV()
{
}

fio::FileCSV::FileCSV(const unsigned int nColumns, const unsigned int nRows)
	: FileSpreadsheet(nColumns, nRows)
{
}

fio::FileCSV::FileCSV(const FileCSV &file)
{
    *this = file;
}

void fio::FileCSV::operator = (const FileCSV &file)
{
    //delete storage if it exists
    if (m_pData) {
        delete [] m_pData;
    }

    //create storage for data
    m_nRows = file.m_nRows;
    m_nColumns = file.m_nColumns;

    const unsigned int nTotal = m_nRows * m_nColumns;
    m_pData = new std::string[nTotal];

    //copy all data from the other storage file
    for (unsigned int i = 0; i < nTotal; i++) {
        m_pData[i] = file.m_pData[i];
    }
}

fio::FileCSV::~FileCSV()
{
    //delete storage if it exists
    if (m_pData) {
        delete [] m_pData;
    }
}

bool fio::FileCSV::Save(std::string sFile, const std::string sDelimiter) const
{
    //check if there is anything to save
    if (m_nRows == 0 || m_nColumns == 0 || !m_pData) {
        //nothing to save
        return false;
    }

    //Create a file to save data to
    std::ofstream oFile(sFile.c_str(), std::ios::out | std::ios::ate);

    if (!oFile.is_open()) {
        //failed to create a file
        return false;
    }

    //loop through all data and save it to the file
    for (unsigned int iRow = 0; iRow < m_nRows; iRow++) {
        const unsigned int nBase = iRow * m_nColumns;
        oFile << m_pData[nBase];

        for (unsigned int iColumn = 1; iColumn < m_nColumns; iColumn++) {
            //write each of the individual values with a delimeter in front
            oFile << sDelimiter << m_pData[nBase + iColumn];
        }

        //done with writing the row, end with a newline
        oFile << std::endl;
    }

    //done writing everything
    oFile.close();
    return true;
}

bool fio::FileCSV::Load(std::string sFile, const unsigned int nSkipRows, const unsigned int nSkipColumns, const std::string sDelimiter)
{
    //remove any data if it exists
    if (m_pData) {
        delete [] m_pData;
        m_pData = NULL;
        m_nRows = 0;
        m_nColumns = 0;
    }

    assert(m_nRows == 0);
    assert(m_nColumns == 0);

    //start loading the file
    std::ifstream iFile(sFile.c_str(), std::ios::in);

    if (!iFile.is_open()) {
        //failed to open the file
        return false;
    }

    //a bit inefficient this, but I suspect I will not use this library very often, If I will:
    //TODO: Optimize the following
    std::vector<std::string> veData;

    std::string sLine;
    std::getline(iFile, sLine);

    while (!iFile.eof()) {
        //sanitize the line, check how many delimiters it contains and store it
        const unsigned int nCurColumns = comstr::CountOccurrence(sLine, sDelimiter) + 1;

        if (nCurColumns > m_nColumns) {
            //number of columns must be increased
            m_nColumns = nCurColumns;
        }

        sLine = comstr::RemoveSystemCharacters(sLine);

        if (sLine.length() != 0) {
            veData.push_back(sLine);
            m_nRows++;
        }

        //get a new line to process
        std::getline(iFile, sLine);
    }

	//all data retrieved, close the file
	iFile.close();

	//make sure everything is ready to skip the required number of rows and columns
	if (m_nColumns <= nSkipColumns || m_nRows <= nSkipRows) {
		//no data will remain if we skip the requested columns
		return true;
	}

	const unsigned int nOldNumColumns = m_nColumns;

	m_nColumns -= nSkipColumns;
	m_nRows -= nSkipRows;

    //done looping through all lines, now store them internally
    m_pData = new std::string[m_nColumns * m_nRows];
    unsigned int iBase = 0;

	if (nSkipRows == 0) {
		//simple optimization, no temporary storage required if no rows are skipped
		for (std::vector<std::string>::const_iterator it = veData.begin() + nSkipRows; it != veData.end(); it++) {
			//this will loop through all rows
			comstr::SeperateString(*it, sDelimiter, m_pData + iBase, m_nColumns);
			iBase += m_nColumns;
		}
	} else {
		//we require temporary storage
		std::string *pTemp = new std::string[nOldNumColumns];

		for (std::vector<std::string>::const_iterator it = veData.begin() + nSkipRows; it != veData.end(); it++) {
			//loop through all rows, disregard the skipped columns
			comstr::SeperateString(*it, sDelimiter, pTemp, m_nColumns);

			for (unsigned int i = 0; i < m_nColumns; i++) {
				m_pData[iBase + i] = pTemp[nSkipRows + i];
			}

			iBase += m_nColumns;
		}

		//delete the temporary storage
		delete [] pTemp;
	}

    //done storing everything
    return true;
}
