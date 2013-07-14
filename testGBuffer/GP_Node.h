#ifndef _GP_NODE_H_
#define _GP_NODE_H_

#include "GP_CommonType.h"
#include "pico/picostorage.hpp"
#include "pico/os.hpp"
#include "pico/offsetfile.hpp"

class GP_Node{
public:
	GP_Node();
	~GP_Node();
	
private:
	GP_Node* pNodeList;
	
private:
	std::string nodepath;
	std::string nodename;
	GP_Node* pFather;
	int nodetype;
};

#endif