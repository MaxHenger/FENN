#ifndef FIO_FILEINI_H
#define FIO_FILEINI_H

#include "FileSettings.h"

#include <string>
#include <vector>

namespace fio
{
    struct FileINIEntry
    {
        FileINIEntry();
        FileINIEntry(std::string name, std::string value);
        FileINIEntry(const FileINIEntry &entry);
        void operator = (const FileINIEntry &entry);

        std::string sName;
        std::string sValue;
    };

    class FileINI : public FileSettings
    {
    public:
        //(copy) constructors and destructor
        FileINI();
        FileINI(const FileINI &file);
        void operator = (const FileINI &file);
        ~FileINI();

        //saving and loading
        bool Save(const std::string sFile);
        bool Load(const std::string sFile);

        //adding data
        void Add(const FileINIEntry entry);
        void Add(const std::string sName, const std::string sValue);
        void Add(const std::string sHeader, const FileINIEntry entry);
        void Add(const std::string sHeader, const std::string sName, const std::string sValue);

        //retrieving data
        // - retrieving headers
        unsigned int    GetNumHeaders() const;
        std::string     GetHeader(const unsigned int index) const;

        // - retrieving entries
        unsigned int    GetNumEntries() const;
        unsigned int    GetNumEntries(const unsigned int iHeader) const;
        bool            GetNumEntries(const std::string sHeader, unsigned int &entries) const;

        FileINIEntry    GetEntry(const unsigned int index) const;
        FileINIEntry    GetEntry(const unsigned int iHeader, const unsigned int iEntry) const;
        bool            GetEntry(const std::string sHeader, const unsigned int index, FileINIEntry &res) const;

        bool            GetEntry(const std::string sName, FileINIEntry &res) const;
        bool            GetEntry(const unsigned int iHeader, const std::string sName, FileINIEntry &res) const;
        bool            GetEntry(const std::string sHeader, const std::string sName, FileINIEntry &res) const;

    protected:
        struct FileINIHeader
        {
            FileINIHeader();
            FileINIHeader(const FileINIHeader &header);
            void operator = (const FileINIHeader &header);

            std::string sHeader;
            std::vector<FileINIEntry> veEntries;
        };

        std::vector<FileINIHeader> m_veHeaders;

    private:
        void Sanitize(std::string &s) const;
        std::vector<FileINIHeader>::const_iterator FindHeader(const std::string sHeader) const;
        std::vector<FileINIHeader>::iterator FindHeader(const std::string sHeader);
        std::vector<FileINIHeader>::iterator CreateHeader(const std::string sHeader);
        std::vector<FileINIEntry>::const_iterator FindEntry(std::vector<FileINIHeader>::const_iterator it, const std::string sName) const;
        std::vector<FileINIEntry>::iterator FindEntry(std::vector<FileINIHeader>::iterator it, const std::string sName);
    };
}

#endif
