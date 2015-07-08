#ifndef COMSTR_COMSTREDITING_H
#define COMSTR_COMSTREDITING_H

#include "COMSTRConfig.h"

#include <string>
#include <vector>

namespace comstr
{
	bool SeperateString(const std::string sLine, const std::string sDelimeter, std::vector<std::string> &veRes);
	unsigned int SeperateString(const std::string sLine, const std::string sDelimeter, std::string *pRes, unsigned int nMax);
	unsigned int CountOccurrence(const std::string sLine, const char chToFind);
	unsigned int CountOccurrence(const std::string sLine, const std::string sToFind);
	std::string RemoveLeadingWhitespace(const std::string sLine);
	std::string RemoveTrailingWhitespace(const std::string sLine);
	std::string RemoveSystemCharacters(std::string sLine);
}

#endif