
//standard libraries
#include <iostream>
#include <ctime>
#include "stdafx.h"
#include <limits>
#include <cstdlib>
#include <iomanip>      // std::setw


//custom includes
#include "CImg/CImg.h"
#include "Sliding/slidingWindow.hpp"
#include "Histogram/histOfGrad.hpp"
#include "neuralNetwork.h"
#include "neuralNetworkTrainer.h"

//use standard namespace
using namespace std;
using namespace cimg_library;


void main()
{		
	//seed random number generator
	srand((unsigned int)time(0));

	//create data set reader and load data file
	dataReader d;
	d.loadDataFile("Data\\70X134H96\\Test\\neg\\bmp\\", "Data\\70X134H96\\Test\\pos\\bmp\\", 16, 3, 20, 400);
	d.setCreationApproach(STATIC, 10);

	//system("PAUSE");
	//create neural network
	// input neurons needs to be 70x134 = 9380 , 130x67 = 8710
	// hidden neurons shall be less than input at least twice = 4690 , 4355
	// output neurons shall be as number of possibilities = in our case 1
	neuralNetwork neuralNet(8710, 4355, 3);

	//create neural network trainer
	neuralNetworkTrainer nT(&neuralNet);
	nT.setTrainingParameters(0.001, 0.9, false);
	nT.setStoppingConditions(3, 90);
	//nT.enableLogging("log.csv", 5);
	

	//train neural network on data sets
	//cout << d.getNumTrainingSets();
	for (int i=0; i < d.getNumTrainingSets(); i++ )
	{
		nT.trainNetwork( d.getTrainingDataSet() );
	}

	//save the weights
	//nn.saveWeights("weights.csv");			
	system("PAUSE");
}
