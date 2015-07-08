#include "FileSpreadsheet.h"

#include <cassert>

fio::FileSpreadsheet::FileSpreadsheet()
	: m_nRows(0)
	, m_nColumns(0)
	, m_pData(NULL)
{
}

fio::FileSpreadsheet::FileSpreadsheet(const unsigned int nRows, const unsigned int nColumns)
	: m_nRows(0)
	, m_nColumns(0)
	, m_pData(NULL)
{
	Resize(m_nRows, m_nColumns);
}

unsigned int fio::FileSpreadsheet::GetNumColumns() const
{
	return m_nColumns;
}

unsigned int fio::FileSpreadsheet::GetNumRows() const
{
	return m_nRows;
}

void fio::FileSpreadsheet::Resize(const unsigned int nColumns, const unsigned int nRows)
{
	//resize the data array, but keep old data wherever possible
	std::string *pNew = new std::string[nColumns * nRows];

	if (m_pData) {
		const unsigned int nProcessRows = nRows > m_nRows ? m_nRows : nRows;
		const unsigned int nProcessColumns = nColumns > m_nColumns ? m_nColumns : nColumns;

		for (unsigned int iRow = 0; iRow < nProcessRows; iRow++) {
			for (unsigned int iColumn = 0; iColumn < nProcessColumns; iColumn++) {
				pNew[iRow * nColumns + iColumn] = m_pData[iRow * m_nColumns + iColumn];
			}
		}

		delete[] m_pData;
	}

	//assign values to the new rows and columns
	m_nColumns = nColumns;
	m_nRows = nRows;
	m_pData = pNew;
}

void fio::FileSpreadsheet::Set(const unsigned int iColumn, const unsigned int iRow, const std::string sValue)
{
	assert(iColumn < m_nColumns);
	assert(iRow < m_nRows);

	m_pData[iRow * m_nColumns + iColumn] = sValue;
}

std::string fio::FileSpreadsheet::Get(const unsigned int iColumn, const unsigned int iRow) const
{
	assert(iColumn < m_nColumns);
	assert(iRow < m_nRows);

	return m_pData[iRow * m_nColumns + iColumn];
}