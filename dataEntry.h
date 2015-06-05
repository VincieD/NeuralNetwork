#ifndef _DATAENTRY
#define _DATAENTRY

//standard libraries
#include <iostream>
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>

using namespace std;

class dataEntry
{
public:
	
	//public members
	//----------------------------------------------------------------------------------------------------------------
	std::vector<std::vector <unsigned char> > pattern; // patterns - has to have same number as input neurons
	double* target;						// target - has to have same number as output neurons

public:

	//public methods
	//----------------------------------------------------------------------------------------------------------------

	//constructor
	dataEntry(std::vector<std::vector <unsigned char> >p, double* t) : pattern(p), target(t) {}

	~dataEntry()
	{
		pattern.clear(); // delete all entries within the vector and leave it with size 0
		delete[] target;
	}

};

#endif
