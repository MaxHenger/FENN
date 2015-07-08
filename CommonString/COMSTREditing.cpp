#include "COMSTREditing.h"

bool comstr::SeperateString(const std::string sLine, const std::string sDelimeter, std::vector<std::string> &veRes)
{
	size_t nOld = 0;
	size_t nNew = sLine.find(sDelimeter);

	while (nNew != sLine.npos) {
		//retrieve new substring
		const size_t nLength = nNew - nOld;

		if (nLength != 0) {
			veRes.push_back(sLine.substr(nOld, nLength));
		}

		//set iterators for a new line retrieval
		nOld = nNew + sDelimeter.length();
		nNew = sLine.find(sDelimeter, nOld);
	}

	//add next substring, if it still exists
	const size_t nLength = sLine.length() - nOld;

	if (nLength != 0) {
		veRes.push_back(sLine.substr(nOld));
	}

	return true;
}

unsigned int comstr::SeperateString(const std::string sLine, const std::string sDelimeter, std::string *pRes, unsigned int nMax)
{
	if (nMax == 0) {
		//nothing to retrieve, but the rest of this function assumes that nMax != 0,
		//because at least one value will be stored
		return true;
	}

	size_t nOld = 0;
	size_t nNew = sLine.find(sDelimeter);
	unsigned int iCounter = 0;

	while (nNew != sLine.npos) {
		//retrieve new substring
		const size_t nLength = nNew - nOld;

		if (nLength != 0) {
			pRes[iCounter++] = sLine.substr(nOld, nLength);

			//return if the maximum number of strings are retrieved
			if (iCounter == nMax) {
				return nMax;
			}
		}

		//set iterators for a new line retrieval
		nOld = nNew + sDelimeter.length();
		nNew = sLine.find(sDelimeter, nOld);
	}

	//add new substring, if it still exists
	const size_t nLength = sLine.length() - nOld;

	if (nLength != 0) {
		pRes[iCounter++] = sLine.substr(nOld);
	}

	return iCounter;
}

unsigned int comstr::CountOccurrence(const std::string sLine, const char chToFind)
{
	unsigned int nCounter = 0;

	for (size_t i = 0; i < sLine.length(); i++) {
		if (sLine[i] == chToFind) {
			nCounter++;
		}
	}

	return nCounter;
}

unsigned int comstr::CountOccurrence(const std::string sLine, const std::string sToFind)
{
	unsigned int nCounter = 0;

	size_t nPos = sLine.find(sToFind, 0);

	while (nPos != sLine.npos) {
		//found the occurring character sequence
		nCounter++;

		nPos = sLine.find(sToFind, nPos + sToFind.length());
	}

	return nCounter;
}

std::string comstr::RemoveLeadingWhitespace(const std::string sLine)
{
	size_t nPos = 0;
	const size_t nLength = sLine.length();
	while (nPos < nLength && sLine[nPos] == ' ') {
		nPos++;
	}

	if (nPos < nLength) {
		return sLine.substr(nPos);
	}

	return "";
}

std::string comstr::RemoveTrailingWhitespace(const std::string sLine)
{
	const size_t nLength = sLine.length();
	int nPos = nLength - 1;
	while (nPos >= 0 && sLine[nPos] == ' ') {
		nPos--;
	}

	if (nPos >= 0) {
		return sLine.substr(0, nPos + 1);
	}

	return "";
}

std::string comstr::RemoveSystemCharacters(std::string sLine)
{
	std::string sRes;
	for (size_t i = 0; i < sLine.length(); i++) {
		if (sLine[i] >= ' ' || sLine[i] == '\t') {
			sRes += sLine[i];
		}
	}

	return sRes;
}