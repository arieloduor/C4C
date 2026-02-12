#ifndef C4C_FILE_TO_STRING_H
#define C4C_FILE_TO_STRING_H

#include "../../utils/include/utils.hpp"

class FileToString
{
	std::stringstream buf;

public:
	FileToString(std::string file_name)
	{
		std::ifstream file(file_name);

		if (file)
		{
			this->buf << file.rdbuf();
			file.close();
		}
		else
		{
			DEBUG_PRINT("Error : could not open file  =>  ",file_name);
		}


	}

	std::string read()
	{
		return this->buf.str();
	}

};

#endif
