#ifndef FIO_FILECSV_H
#define FIO_FILECSV_H

#include "FileSpreadsheet.h"

#include <string>

namespace fio
{
    class FileCSV : public FileSpreadsheet
    {
    public:
        //(copy) constructors and destructor
        FileCSV();
		FileCSV(const unsigned int nColumns, const unsigned int nRows);
        FileCSV(const FileCSV &file);
        void operator = (const FileCSV &file);
        ~FileCSV();

        //loading and saving
        bool            Save(std::string sFile, const std::string sDelimeter=",") const;
        bool            Load(std::string sFile, const unsigned int nSkipRows = 0, const unsigned int nSkipColumns = 0, const std::string sDelimeter=",");
    };
}

#endif
