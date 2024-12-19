#include "Program.h"

Program::Program(unsigned int type)
{
	this->type = (programTypes_e)type;
	this->fileName = "";
}

Program::Program(std::string fileName, unsigned int type)
{
	this->type = (programTypes_e)type;
	this->fileName = fileName;
}
