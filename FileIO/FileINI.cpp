#include "FileINI.h"
#include "FileIOUtility.h"

#include <fstream>
#include <assert.h>

fio::FileINIEntry::FileINIEntry()
{
}

fio::FileINIEntry::FileINIEntry(std::string name, std::string value)
    : sName(name)
    , sValue(value)
{
}

fio::FileINIEntry::FileINIEntry(const FileINIEntry &entry)
{
    *this = entry;
}

void fio::FileINIEntry::operator = (const FileINIEntry &entry)
{
    sName    = entry.sName;
    sValue   = entry.sValue;
}

fio::FileINI::FileINIHeader::FileINIHeader()
{
}

fio::FileINI::FileINIHeader::FileINIHeader(const FileINIHeader &header)
{
    *this = header;
}

void fio::FileINI::FileINIHeader::operator = (const FileINIHeader &header)
{
    sHeader     = header.sHeader;

    for (std::vector<FileINIEntry>::const_iterator it = header.veEntries.begin(); it != header.veEntries.end(); it++) {
        veEntries.push_back(*it);
    }
}

//continue here!
fio::FileINI::FileINI()
{
    m_veHeaders.push_back(FileINIHeader());
}

fio::FileINI::FileINI(const FileINI &file)
{
    *this = file;
}

void fio::FileINI::operator = (const FileINI &file)
{
    //loop through every header in the other file
    for (std::vector<fio::FileINI::FileINIHeader>::const_iterator itHeader = file.m_veHeaders.begin(); itHeader != file.m_veHeaders.end(); itHeader++) {
        //create a new header and set the header name
        FileINIHeader newHeader;
        newHeader.sHeader = itHeader->sHeader;

        m_veHeaders.push_back(newHeader);
        std::vector<fio::FileINI::FileINIHeader>::iterator itNewHeader = m_veHeaders.end() - 1;

        //loop through all entries in the current header
        for (std::vector<fio::FileINIEntry>::const_iterator itEntry = itHeader->veEntries.begin(); itEntry != itHeader->veEntries.end(); itEntry++) {
            itNewHeader->veEntries.push_back(fio::FileINIEntry(itEntry->sName, itEntry->sValue));
        }
    }
}

fio::FileINI::~FileINI()
{
}

bool fio::FileINI::Save(std::string sFile)
{
    //open a file to write everything to
    std::ofstream oFile(sFile.c_str(), std::ios::out | std::ios::ate);

    if (!oFile.is_open()) {
        //could not open/create file
        return false;
    }

    //loop through all headers
    for (std::vector<fio::FileINI::FileINIHeader>::const_iterator itHeader = m_veHeaders.begin(); itHeader != m_veHeaders.end(); itHeader++) {
        if (itHeader->sHeader.length() != 0) {
            //write the header name
            oFile << "[" + itHeader->sHeader + "]\n";
        }

        //loop through all entries in the current header
        for (std::vector<fio::FileINIEntry>::const_iterator itEntry = itHeader->veEntries.begin(); itEntry != itHeader->veEntries.end(); itEntry++) {
            //store the current value
            oFile << itEntry->sName << " = " << itEntry->sValue << "\n";
        }
    }

    //everything is written to the file
    oFile.close();
    return true;
}

bool fio::FileINI::Load(std::string sFile)
{
    //clear all data
    m_veHeaders.clear();

    //even when the function fails, the default non-named header will
    //be there
    m_veHeaders.push_back(FileINIHeader());
    std::vector<FileINIHeader>::iterator itHeader = m_veHeaders.begin();

    //open a file to read data from
    std::ifstream iFile(sFile.c_str(), std::ios::in);

    if (!iFile.is_open()) {
        //could not open file
        return false;
    }

    std::string sLine;
    std::getline(iFile, sLine);

    while (!iFile.eof()) {
        //have not yet reached the end of the file, remove anything
        //that resembles a comment from the string
        const size_t nComment = sLine.find('#');

        if (nComment != sLine.npos) {
            //remove the comment from the line
            sLine.substr(0, nComment);
        }

        Sanitize(sLine);

        if (sLine.length() != 0) {
            //line still contains information
            if (sLine[0] == '[') {
                //dealing with a header, create a new header
                FileINIHeader newHeader;
                const size_t nHeaderEnd = sLine.find(']');

                if (nHeaderEnd == sLine.npos) {
                    //expected to find a closing bracket as well
                    iFile.close();
                    return false;
                }

                newHeader.sHeader = sLine.substr(1, nHeaderEnd - 1);

                //sanitize header name
                newHeader.sHeader = fio::RemoveLeadingWhitespace(newHeader.sHeader);
                newHeader.sHeader = fio::RemoveTrailingWhitespace(newHeader.sHeader);

                //add the the vector and update the iterator
                m_veHeaders.push_back(newHeader);
                itHeader = m_veHeaders.end() - 1;
            } else {
                //line contains data and is not a header, then it must contain
                //a variable and its associated value
                const size_t nEqual = sLine.find('=');

                if (nEqual == sLine.npos) {
                    //expected to find an equal sign
                    iFile.close();
                    return false;
                }

                FileINIEntry newEntry;
                newEntry.sName = fio::RemoveTrailingWhitespace(sLine.substr(0, nEqual));
                newEntry.sValue = fio::RemoveLeadingWhitespace(sLine.substr(nEqual + 1));

                //add entry to the current header
                itHeader->veEntries.push_back(newEntry);
            }
        }

        //retrieve a new line
        std::getline(iFile, sLine);
    }

    //if this code is reached, the file is completely loaded
    iFile.close();
    return true;
}

void fio::FileINI::Add(const FileINIEntry entry)
{
    m_veHeaders[0].veEntries.push_back(entry);
}

void fio::FileINI::Add(const std::string sName, const std::string sValue)
{
    m_veHeaders[0].veEntries.push_back(FileINIEntry(sName, sValue));
}

void fio::FileINI::Add(const std::string sHeader, const FileINIEntry entry)
{
    std::vector<FileINIHeader>::iterator it = FindHeader(sHeader);
    if (it == m_veHeaders.end()) {
        it = CreateHeader(sHeader);
    }

    it->veEntries.push_back(entry);
}

void fio::FileINI::Add(const std::string sHeader, const std::string sName, const std::string sValue)
{
    std::vector<FileINIHeader>::iterator it = FindHeader(sHeader);
    if (it == m_veHeaders.end()) {
        it = CreateHeader(sHeader);
    }

    it->veEntries.push_back(FileINIEntry(sName, sValue));
}

unsigned int fio::FileINI::GetNumHeaders() const
{
    return m_veHeaders.size() - 1;
}

std::string fio::FileINI::GetHeader(const unsigned int index) const
{
    assert(index < m_veHeaders.size() - 1);
    return m_veHeaders[index + 1].sHeader;
}

unsigned int fio::FileINI::GetNumEntries() const
{
    return m_veHeaders[0].veEntries.size();
}

unsigned int fio::FileINI::GetNumEntries(const unsigned int index) const
{
    assert(index < m_veHeaders.size() - 1);
    return m_veHeaders[index + 1].veEntries.size();
}

bool fio::FileINI::GetNumEntries(const std::string sHeader, unsigned int &res) const
{
    std::vector<FileINIHeader>::const_iterator it = FindHeader(sHeader);

    if (it == m_veHeaders.end()) {
        res = 0;
        return false;
    }

    res = it->veEntries.size();
    return true;
}

fio::FileINIEntry fio::FileINI::GetEntry(const unsigned int index) const
{
    assert(index < m_veHeaders[0].veEntries.size());
    return m_veHeaders[0].veEntries[index];
}

fio::FileINIEntry fio::FileINI::GetEntry(const unsigned int iHeader, const unsigned int iEntry) const
{
    assert(iHeader < m_veHeaders.size() - 1);
    assert(iEntry < m_veHeaders[iHeader + 1].veEntries.size());

    return m_veHeaders[iHeader + 1].veEntries[iEntry];
}

bool fio::FileINI::GetEntry(const std::string sHeader, const unsigned int iEntry, FileINIEntry &entry) const
{
    std::vector<FileINIHeader>::const_iterator it = FindHeader(sHeader);

    if (it == m_veHeaders.end()) {
        //failed to find the header
        entry.sName.clear();
        entry.sValue.clear();
        return false;
    }

    entry.sName = it->veEntries[iEntry].sName;
    entry.sValue = it->veEntries[iEntry].sValue;
    return true;
}

bool fio::FileINI::GetEntry(const std::string sName, FileINIEntry &entry) const
{
    std::vector<FileINIEntry>::const_iterator it = FindEntry(m_veHeaders.begin(), sName);

    if (it == m_veHeaders[0].veEntries.end()) {
        entry.sName.clear();
        entry.sValue.clear();
        return false;
    }

    entry = *it;
    return true;
}

bool fio::FileINI::GetEntry(const unsigned int iHeader, const std::string sName, FileINIEntry &entry) const
{
    assert(iHeader < m_veHeaders.size() - 1);
    std::vector<FileINIHeader>::const_iterator itHeader = m_veHeaders.begin() + (iHeader + 1);
    std::vector<FileINIEntry>::const_iterator itEntry = FindEntry(itHeader, sName);

    if (itEntry == itHeader->veEntries.end()) {
        entry.sName.clear();
        entry.sValue.clear();
        return false;
    }

    entry = *itEntry;
    return true;
}

bool fio::FileINI::GetEntry(const std::string sHeader, const std::string sName, FileINIEntry &entry) const
{
    std::vector<FileINIHeader>::const_iterator itHeader = FindHeader(sHeader);

    if (itHeader == m_veHeaders.end()) {
        //could not find the header
        entry.sName.clear();
        entry.sValue.clear();
        return false;
    }

    std::vector<FileINIEntry>::const_iterator itEntry = FindEntry(itHeader, sName);

    if (itEntry == itHeader->veEntries.end()) {
        //failed to find the entry
        entry.sName.clear();
        entry.sValue.clear();
        return false;
    }

    entry = *itEntry;
    return true;
}

void fio::FileINI::Sanitize(std::string &s) const
{
    s = fio::RemoveSystemCharacters(s);
    s = fio::RemoveLeadingWhitespace(s);
    s = fio::RemoveTrailingWhitespace(s);
}

std::vector<fio::FileINI::FileINIHeader>::const_iterator fio::FileINI::FindHeader(const std::string sHeader) const
{
    for (std::vector<FileINIHeader>::const_iterator it = m_veHeaders.begin(); it != m_veHeaders.end(); it++) {
        if (it->sHeader == sHeader) {
            //found the header
            return it;
        }
    }

    return m_veHeaders.end();
}

std::vector<fio::FileINI::FileINIHeader>::iterator fio::FileINI::FindHeader(const std::string sHeader)
{
    for (std::vector<FileINIHeader>::iterator it = m_veHeaders.begin(); it != m_veHeaders.end(); it++) {
        if (it->sHeader == sHeader) {
            //found the header
            return it;
        }
    }

    return m_veHeaders.end();
}

std::vector<fio::FileINI::FileINIHeader>::iterator fio::FileINI::CreateHeader(const std::string sHeader)
{
    //create a new entry
    FileINIHeader newHeader;
    newHeader.sHeader = sHeader;
    m_veHeaders.push_back(newHeader);
    return m_veHeaders.end() - 1;
}

std::vector<fio::FileINIEntry>::const_iterator fio::FileINI::FindEntry(std::vector<FileINIHeader>::const_iterator it, const std::string sName) const
{
    for (std::vector<FileINIEntry>::const_iterator itEntry = it->veEntries.begin(); itEntry != it->veEntries.end(); itEntry++) {
        if (itEntry->sName == sName) {
            //found the entry
            return itEntry;
        }
    }

    //could not find the entry
    return it->veEntries.end();
}

std::vector<fio::FileINIEntry>::iterator fio::FileINI::FindEntry(std::vector<FileINIHeader>::iterator it, const std::string sName)
{
    for (std::vector<FileINIEntry>::iterator itEntry = it->veEntries.begin(); itEntry != it->veEntries.end(); itEntry++) {
        if (itEntry->sName == sName) {
            return itEntry;
        }
    }

    return it->veEntries.end();
}
