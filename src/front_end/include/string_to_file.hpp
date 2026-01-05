#ifndef C4C_STRING_TO_FILE_H
#define C4C_STRING_TO_FILE_H

#include "../../utils/include/utils.hpp"

class StringToFile
{
	std::stringstream buf;

public:
	StringToFile(std::string file_name,std::string buf)
	{
		std::ofstream file(file_name);

		if (file)
		{
			file << buf;
		}
		else
		{
			DEBUG_PRINT("Error : could not open file  =>  ",file_name);
		}


	}

};

#endif
