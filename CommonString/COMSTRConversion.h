#ifndef COMSTR_COMSTRCONVERSION_H
#define COMSTR_COMSTRCONVERSION_H

#include "COMSTRConfig.h"

#include <string>
#include <sstream>

namespace comstr
{
	template <typename T>
	bool ToString(const T &t, std::string &s)
	{
		std::stringstream ss;
		ss << t;
		return !(ss >> s).fail();
	}

	template <typename T>
	bool ToString(const T &t, std::string &s, const unsigned int nPrecision)
	{
		std::stringstream ss;
		ss.precision(nPrecision);
		ss << t;
		return !(ss >> s).fail();
	}

	template <typename T>
	bool FromString(const std::string s, T &t)
	{
		std::stringstream ss;
		ss << s;
		return !(ss >> t).fail();
	}
}

#endif