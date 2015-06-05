/*******************************************************************
* Data File Reader and Training Set Creator
* ------------------------------------------------------------------
* Vaclav Divis - 2015
********************************************************************/

#ifndef _DATAREADER
#define _DATAREADER

//include standard header files
#include<iostream>
#include<vector>
#include<cstring>
#include<fstream>

class HistogramOfGradients;
class SlidingWindow;
/*******************************************************************
* stores a data item
********************************************************************/
class dataEntry
{
public:

	//public members
	//----------------------------------------------------------------------------------------------------------------
	std::vector<std::vector <unsigned char> > pattern; // patterns - has to have same number as input neurons
	std::vector<double> target;						// target - has to have same number as output neurons

public:

	//public methods
	//----------------------------------------------------------------------------------------------------------------

	//constructor
	dataEntry(std::vector<std::vector <unsigned char> >p, std::vector<double>t) : pattern(p), target(t) {}

};

/*******************************************************************
* Training Sets Storage - stores shortcuts to data items
********************************************************************/
class trainingDataSet
{
public:

	std::vector<dataEntry*> trainingSet;
	std::vector<dataEntry*> generalizationSet;
	std::vector<dataEntry*> validationSet;
	std::vector<std::string> picNames;
	std::vector<bool> pos;

	trainingDataSet(){}
	
	void clear()
	{
		trainingSet.clear();
		generalizationSet.clear();
		validationSet.clear();
		picNames.clear();
	}
};

//dataset retrieval approach enum
enum { NONE, STATIC, GROWING, WINDOWING };

//data reader class
class dataReader
{
	
//private members
//----------------------------------------------------------------------------------------------------------------
private:

	//data storage
	std::vector<dataEntry*> dataPos;
	std::vector<dataEntry*> dataNeg;
	int nInputs;
	int nTargets;

	//current data set
	trainingDataSet tSet;

	//data set creation approach and total number of dataSets
	int creationApproach;
	int numTrainingSets;
	int trainingDataEndIndex;
	int trainingDataNegEndIndex;

	//creation approach variables
	double growingStepSize;			//step size - percentage of total set
	int growingLastDataIndex;		//last index added to current dataSet
	int windowingSetSize;			//initial size of set
	int windowingStepSize;			//how many entries to move window by
	int windowingStartIndex;		//window start index	
	
//public methods
//----------------------------------------------------------------------------------------------------------------
public:

	dataReader(): creationApproach(NONE), numTrainingSets(-1) {}
	~dataReader();
	
	bool loadDataFile(const char* filenameNeg, const char* filenamePos, int nI, int nT, unsigned int limitPos, unsigned int limitNeg);
	void setCreationApproach( int approach, double param1 = -1, double param2 = -1 );
	int getNumTrainingSets();	
	
	trainingDataSet* getTrainingDataSet();
	std::vector<dataEntry*>& getAllDataEntries();

//private methods
//----------------------------------------------------------------------------------------------------------------
private:
	
	void createStaticDataSet();
	void createGrowingDataSet();
	void createWindowingDataSet();	
	void processLine( std::string &line );
};

#endif
