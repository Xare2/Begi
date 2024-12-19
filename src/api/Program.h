#pragma once
#include "common.h"

typedef enum programTypes_e
{
	vertex = 0,
	fragment = 1
} programTypes_e;

class Program
{
protected:
	programTypes_e type;
	std::string fileName;
	unsigned int idProgram = -1;

public:
	Program(unsigned int type);
	Program(std::string fileName, unsigned int type);
	virtual void compile() = 0;
	virtual void readFile() = 0;
	virtual void checkErrors() = 0;
};

