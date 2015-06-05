/*******************************************************************
* Basic Feed Forward Neural Network Class
* ------------------------------------------------------------------
* Vaclav Divis - 2015
********************************************************************/

#ifndef NNetwork
#define NNetwork

#include "dataReader.h"

class neuralNetworkTrainer;

class neuralNetwork
{
	//class members
	//--------------------------------------------------------------------------------------------
private:

	//number of neurons
	int nInput, nHidden, nOutput;
	
	//neurons
	double* inputNeurons;
	double* hiddenNeurons;
	double* outputNeurons;

	//weights
	double** wInputHidden;
	double** wHiddenOutput;
		
	//Friends
	//--------------------------------------------------------------------------------------------
	friend neuralNetworkTrainer;
	
	//public methods
	//--------------------------------------------------------------------------------------------

public:

	//constructor & destructor
	neuralNetwork(int numInput, int numHidden, int numOutput);
	~neuralNetwork();

	//weight operations
	//bool loadWeights(char* inputFilename);
	//bool saveWeights(char* outputFilename);
	int* feedForwardPattern(std::vector<std::vector<unsigned char> >& pattern);
	double* feedForwardPattern_double(std::vector<std::vector<unsigned char> >& pattern);

	double getSetAccuracy( std::vector<dataEntry*>& set );
	double getSetMSE( std::vector<dataEntry*>& set );
	bool saveWeights(char* filename);
	bool loadWeights(char* filename);

	int getNumberOfOutputNeurons();
	double* getResultFromOutputNeuros();


	//private methods
	//--------------------------------------------------------------------------------------------

private: 

	void initializeWeights();
	inline double activationFunction( double x );
	inline int clampOutput( double x );
	void feedForward(std::vector<std::vector<unsigned char> >& window);

	
};

#endif
