#ifndef _GP_FILE_H_
#define _GP_FILE_H_

#include "GP_CommonType.h"
#include "pico/picostorage.hpp"
#include "pico/os.hpp"
#include "pico/offsetfile.hpp"

class GP_File{
public:
	GP_File();
	~GP_File();
	
private:
	
private:
	std::string path;
	GP_Dir* pFather;
	std::string filename;
};

#endif