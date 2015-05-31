//standard includes
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include "stdafx.h"

//include definition file
#include "neuralNetwork.h"

using namespace std;

/*******************************************************************
* Constructor
********************************************************************/
neuralNetwork::neuralNetwork(int nI, int nH, int nO) : nInput(nI), nHidden(nH), nOutput(nO)
{				
	//create neuron lists
	//--------------------------------------------------------------------------------------------------------
	inputNeurons = new( double[nInput + 1] );
	for ( int i=0; i < nInput; i++ ) inputNeurons[i] = 0;

	//create input bias neuron
	inputNeurons[nInput] = -1;

	hiddenNeurons = new( double[nHidden + 1] );
	for ( int i=0; i < nHidden; i++ ) hiddenNeurons[i] = 0;

	//create hidden bias neuron
	hiddenNeurons[nHidden] = -1;

	outputNeurons = new( double[nOutput] );
	for ( int i=0; i < nOutput; i++ ) outputNeurons[i] = 0;

	//create weight lists (include bias neuron weights)
	//--------------------------------------------------------------------------------------------------------
	wInputHidden = new( double*[nInput + 1] );
	for ( int i=0; i <= nInput; i++ ) 
	{
		wInputHidden[i] = new (double[nHidden]);
		for ( int j=0; j < nHidden; j++ ) wInputHidden[i][j] = 0;		
	}

	wHiddenOutput = new( double*[nHidden + 1] );
	for ( int i=0; i <= nHidden; i++ ) 
	{
		wHiddenOutput[i] = new (double[nOutput]);			
		for ( int j=0; j < nOutput; j++ ) wHiddenOutput[i][j] = 0;		
	}	
	
	//initialize weights
	//--------------------------------------------------------------------------------------------------------
	initializeWeights();			
}

/*******************************************************************
* Destructor
********************************************************************/
neuralNetwork::~neuralNetwork()
{
	//delete neurons
	delete[] inputNeurons;
	delete[] hiddenNeurons;
	delete[] outputNeurons;

	//delete weight storage
	for (int i=0; i <= nInput; i++) delete[] wInputHidden[i];
	delete[] wInputHidden;

	for (int j=0; j <= nHidden; j++) delete[] wHiddenOutput[j];
	delete[] wHiddenOutput;	
}
/*******************************************************************
* Feed pattern through network and return results
* Return - pointer to array of int (zeros and ones - based on clampOutput function)
********************************************************************/
int* neuralNetwork::feedForwardPattern(std::vector<std::vector<int> >& pattern)
{
	feedForward(pattern);

	//create copy of output results
	int* results = new int[nOutput];
	for (int i=0; i < nOutput; i++ ) results[i] = clampOutput(outputNeurons[i]);

	return results;
}
/*******************************************************************
* Return the NN accuracy on the set
********************************************************************/
double neuralNetwork::getSetAccuracy(std::vector<dataEntry*>& set)
{
	double incorrectResults = 0;
		
	//for every training input array
	for ( int tp = 0; tp < (int) set.size(); tp++)
	{						
		//feed inputs through network and backpropagate errors
		feedForward( set[tp]->pattern );
		
		//correct pattern flag
		bool correctResult = true;

		//check all outputs against desired output values
		for ( int k = 0; k < nOutput; k++ )
		{					
			//set flag to false if desired and output differ
			if (clampOutput(outputNeurons[k]) != set[tp]->target[k])
			{
				correctResult = false;
			}
				
		}
		
		//inc training error for a incorrect result
		if ( !correctResult ) incorrectResults++;	
		
	}//end for
	
	//cout << "ClampOut Neurons: " << clampOutput(outputNeurons[0]) << " " << clampOutput(outputNeurons[1]) << " " << clampOutput(outputNeurons[2]) << " ";
	//calculate error and return as percentage
	return 100 - (incorrectResults/set.size() * 100);
}
/*******************************************************************
* Return the NN mean squared error on the set
********************************************************************/
double neuralNetwork::getSetMSE( std::vector<dataEntry*>& set )
{
	double mse = 0;
		
	//for every training input array
	for ( int tp = 0; tp < (int) set.size(); tp++)
	{						
		//feed inputs through network and backpropagate errors
		feedForward( set[tp]->pattern );
		
		//check all outputs against desired output values
		for ( int k = 0; k < nOutput; k++ )
		{					
			//sum all the MSEs together
			mse += pow((outputNeurons[k] - set[tp]->target[k]), 2);
		}		
		
	}//end for
	
	//calculate error and return as percentage
	return mse/(nOutput * set.size());
}

////////////////////////// Private Methods /////////////////////////////

/*******************************************************************
* Initialize Neuron Weights
********************************************************************/
void neuralNetwork::initializeWeights()
{
	//set range
	double rH = 1/sqrt( (double) nInput);
	double rO = 1/sqrt( (double) nHidden);
	
	//set weights between input and hidden 		
	//--------------------------------------------------------------------------------------------------------
	for(int i = 0; i <= nInput; i++)
	{		
		for(int j = 0; j < nHidden; j++) 
		{
			//set weights to random values
			wInputHidden[i][j] = ( ( (double)(rand()%100)+1)/100  * 2 * rH ) - rH;			
		}
	}
	
	//set weights between input and hidden
	//--------------------------------------------------------------------------------------------------------
	for(int i = 0; i <= nHidden; i++)
	{		
		for(int j = 0; j < nOutput; j++) 
		{
			//set weights to random values
			wHiddenOutput[i][j] = ( ( (double)(rand()%100)+1)/100 * 2 * rO ) - rO;
		}
	}
}
/*******************************************************************
* Activation Function
********************************************************************/
inline double neuralNetwork::activationFunction( double x )
{
	//sigmoid function
	return 1/(1+exp(-x));
}	
/*******************************************************************
* Output Clamping
********************************************************************/
inline int neuralNetwork::clampOutput( double x )
{
	if ( x < 0.1 ) return 0;
	else if ( x > 0.9 ) return 1;
	else return -1;
}
/*******************************************************************
* Feed Forward Operation
********************************************************************/
void neuralNetwork::feedForward(std::vector<std::vector<int> >& window)
{
	//set input neurons to input values
	int index = 0;
	double value = (double)(window[0][0]/255.0);
	//std::copy(window.begin(), window.end(), inputNeurons);
	for (unsigned int ii = 0; ii<window.size(); ii++)
	{
		for (unsigned int jj = 0; jj < window[0].size(); jj++)
		{
			value = (double)(window[ii].at(jj) / 255.0);
			inputNeurons[index] = value;
			index++;
		}
	}
	
	//Calculate Hidden Layer values - include bias neuron
	//--------------------------------------------------------------------------------------------------------
	for(int j=0; j < nHidden; j++)
	{
		//clear value
		hiddenNeurons[j] = 0;				
		
		//get weighted sum of pattern and bias neuron
		for( int i=0; i <= nInput; i++ ) hiddenNeurons[j] += (inputNeurons[i] * wInputHidden[i][j]);
		
		//set to result of sigmoid
		hiddenNeurons[j] = activationFunction( hiddenNeurons[j] );			
	}
	
	//Calculating Output Layer values - include bias neuron
	//--------------------------------------------------------------------------------------------------------
	for(int k=0; k < nOutput; k++)
	{
		//clear value
		outputNeurons[k] = 0;				
		
		//get weighted sum of pattern and bias neuron
		for (int j = 0; j <= nHidden; j++)
		{
			outputNeurons[k] += (hiddenNeurons[j] * wHiddenOutput[j][k]);
			// controled to here - neurons are changing it selfs
			//cout << outputNeurons[k] << " ";
		}
		
		//set to result of sigmoid
		outputNeurons[k] = activationFunction( outputNeurons[k] );
	}
}


