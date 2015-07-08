#ifndef FIO_FILESETTINGS_H
#define FIO_FILESETTINGS_H

#include "FileIOConfig.h"

#include <string>

namespace fio
{
	//This class is not entirely necessary at the moment, but would make it easier to extend to a morge
	//generalized type of settings file in the future.
	class FileSettings
	{
	public:
		FileSettings() {};
		virtual ~FileSettings() {};

		//virtual saving and loading functions
		virtual bool Save(const std::string sFilename) const = 0;
		virtual bool Load(const std::string sFilename) = 0;
	};
}

#endif