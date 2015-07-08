#ifndef FIO_FILESPREADSHEET_H
#define FIO_FILESPREADSHEET_H

#include "FileIOConfig.h"

#include <string>

namespace fio
{
	class FileSpreadsheet
	{
	public:
		//constructors and destructor
		FileSpreadsheet() {};
		FileSpreadsheet(const unsigned int nRows, const unsigned int nColumns);

		virtual FileSpreadsheet() {};

		//loading and saving (virtual)
		virtual bool		Save(std::string sFile, const std::string sDelimiter) const = 0;
		virtual bool		Load(std::string sFile, const unsigned int nSkipRows = 0, const unsigned int nSkipColumns = 0, const std::string sDelimiter);

		//retrieving size information (virtual but defined)
		virtual unsigned int GetNumColumns() const;
		virtual unsigned int GetNumRows() const;

		//resizing the contents of the file (virtual but defined)
		virtual void Resize(const unsigned int nColumns, const unsigned int nRows);

		//setting and getting data (virtual but defined)
		virtual void Set(const unsigned int iColumn, const unsigned int iRow, const std::string sValue);
		virtual std::string Get(const unsigned int iColumn, const unsigned int iRow) const;

	protected:
		//size of the spreadsheet
		unsigned int		m_nRows;
		unsigned int		m_nColumns;

		//storage
		std::string			*m_pData;
	};
}