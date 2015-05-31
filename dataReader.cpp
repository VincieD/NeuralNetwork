//include definition file
#include "dataReader.h"
#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <windows.h>
#include <limits>
#include <cstdlib>
#include <iomanip>      // std::setw
#include "CImg/CImg.h"
#include "Sliding/slidingWindow.hpp"
#include "Histogram/histOfGrad.hpp"

using namespace std;
using namespace cimg_library;

size_t const MAX_CHAR = 100; // maximum number of chars
char cstr[MAX_CHAR] = {0};

SlidingWindow slWin;
HistogramOfGradients HoG;

/*******************************************************************
* Destructor
********************************************************************/
dataReader::~dataReader()
{
	//clear data Neg
	for (int i=0; i < (int) dataNeg.size(); i++ ) delete dataNeg[i];		
	dataNeg.clear();
	//clear data Pos
	for (int i = 0; i < (int)dataPos.size(); i++) delete dataPos[i];
	dataPos.clear();
}
/*******************************************************************
* Loads file of input data
********************************************************************/
bool dataReader::loadDataFile(const char* filenameNeg, const char* filenamePos, int nI, int nT, unsigned int limitPos, unsigned int limitNeg)
{
	//clear any previous data
	for (int i = 0; i < (int)dataPos.size(); i++) delete dataPos[i];
	for (int i = 0; i < (int)dataNeg.size(); i++) delete dataNeg[i];

	dataNeg.clear();
	dataPos.clear();
	tSet.clear();

	unsigned int inputNeuronsSize = 0;
	unsigned int posSamplesNum = 0;
	unsigned int negSamplesNum = 0;

	//set number of inputs and outputs
	nInputs = nI;
	nTargets = nT;

	// maximum value of unsigned long is returned in case of line.find(".bmp") is not fond 
	unsigned long MAX = std::string::npos;
	//size_t position = numeric_limits<unsigned long>::max();
	size_t position = std::string::npos;
	string line;
	string lineNeg;

	// training set
	//--------------------------------------------------------------------------------------------------------
	// following steps:
	// 1) select path where will be stored the training set and create a vector of strings
	// will create a text file where will be listed every file
	system("dir Data\\70X134H96\\Test\\pos\\bmp\\ /B > pic_pos.txt");

	// name of this text file needs to be same as the above one
	ifstream myfile("pic_pos.txt");

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			// position of ".bmp" in str
			position = line.find(".bmp");

			// maximum value of unsigned long is returned in case of line.find(".bmp") is not fond 
			if (position != MAX)
			{
				//cout << line << '\n';
				//copy(line.begin(), (line.size() >= MAX_CHAR ? line.begin() + MAX_CHAR : line.end()), cstr);
				tSet.picNames.push_back(line);
				tSet.pos.push_back(true);
				posSamplesNum++;
				//tSet.picNames.insert(tSet.picNames.end(), cstr);
				//cout << cstr << '\n';
			}
		}
		myfile.close();
	}

	else
	{
		cout << "Unable to open file";
	}

	// negative dataset
	system("dir Data\\70X134H96\\Test\\neg\\bmp\\ /B > pic_neg.txt");

	// name of this text file needs to be same as the above one
	ifstream myfileNeg("pic_neg.txt");

	if (myfileNeg.is_open())
	{
		while (getline(myfileNeg, lineNeg))
		{
			// position of ".bmp" in str
			position = lineNeg.find(".bmp");

			// maximum value of unsigned long is returned in case of line.find(".bmp") is not fond 
			if (position != MAX)
			{
				//cout << line << '\n';
				//copy(line.begin(), (line.size() >= MAX_CHAR ? line.begin() + MAX_CHAR : line.end()), cstr);
				tSet.picNames.push_back(lineNeg);
				tSet.pos.push_back(false);
				negSamplesNum++;
				//tSet.picNames.insert(tSet.picNames.end(), cstr);
				//cout << cstr << '\n';
			}
		}
		myfile.close();
	}

	else
	{
		cout << "Unable to open file";
	}

	/*for (int a = 0; a < tSet.picNames.size(); a++)
	{
		cout << tSet.picNames[a] << endl;
	}

	system("PAUSE");*/
	// 2) load a picture in grey scale vector
	//cout << tSet.picNames.size() << '\n';
	// picture number limitation
	unsigned int limitation = limitPos + limitNeg;
	if ((limitPos > 0) && (limitNeg > 0)) limitation = limitPos + limitNeg;
	else limitation = tSet.picNames.size();

	int posPosition = 0;
	int indexForCout = 0;
	// main loop of picture loading
	for (int i = 0; i < (int)limitation; i++)
	{
		// START - load an image using the CImg 

		//cout << filename << tSet.picNames[i];
		//cout << filename << "\\" << tSet.picNames[i] << '\n';
		size_t const MAX_CHAR = 100; // maximum number of chars
		char cstr[MAX_CHAR] = { 0 };
		char buffer[MAX_CHAR] = { 0 }; // <- danger, only storage for 256 characters.
		if (i > (limitPos-1))
		{
			copy(tSet.picNames[posPosition + posSamplesNum].begin(), (tSet.picNames[posPosition + posSamplesNum].size() >= MAX_CHAR ? tSet.picNames[posPosition + posSamplesNum].begin() + MAX_CHAR : tSet.picNames[posPosition + posSamplesNum].end()), cstr);
			posPosition++;
			strncpy(buffer, filenameNeg, sizeof(buffer));
			strncat(buffer, cstr, sizeof(buffer));
			indexForCout = posPosition + posSamplesNum;
		}
		else
		{
			copy(tSet.picNames[i].begin(), (tSet.picNames[i].size() >= MAX_CHAR ? tSet.picNames[i].begin() + MAX_CHAR : tSet.picNames[i].end()), cstr);
			strncpy(buffer, filenamePos, sizeof(buffer));
			strncat(buffer, cstr, sizeof(buffer));
			indexForCout = i;
		}
		//copy(tSet.picNames[0].begin(), (tSet.picNames[0].size() >= MAX_CHAR ? tSet.picNames[0].begin() + MAX_CHAR : tSet.picNames[0].end()), cstr);

		//cout << buffer << endl;
		CImg<unsigned char> image(buffer);


		int width = image.width();
		int height = image.height();
		int depth = image.depth();
		inputNeuronsSize = width * height;

		//cout << (int)width << "px - Picture width" << endl;
		//cout << (int)height << "px - Picture height" << endl;
		unsigned char* ptr = image.data(0, 0); // get pointer to pixel @ 0,0
		unsigned char pixel = *ptr;

		//New grayscale images.
		CImg<unsigned char> gray(width, height, depth, 1);
		vector<vector<unsigned char> > testArray(height, vector<unsigned char>(width));

		unsigned char r, g, b;
		unsigned char gr1 = 0;

		// Convert RGB image to grayscale image
		for (int indexC = 0; indexC < width; indexC++)
		{
			for (int indexD = 0; indexD < height; indexD++)
			{

				//Return a pointer to a located pixel value. 
				r = image(indexC, indexD, 0, 0); // First channel RED
				g = image(indexC, indexD, 0, 1); // Second channel GREEN
				b = image(indexC, indexD, 0, 2); // Third channel BLUE


				//PAL and NTSC
				//Y = 0.299*R + 0.587*G + 0.114*B
				gr1 = (unsigned char)round(0.299*((double)r) + 0.587*((double)g) + 0.114*((double)b));
				gray(indexC, indexD, 0, 0) = gr1;
			}
		}

		for (int indexA = 0; indexA < height; indexA += 1)
		{

			for (int indexB = 0; indexB < width; indexB += 1)
			{
				ptr = gray.data(indexB, indexA); // get pointer to pixel @ 10,10
				pixel = *ptr;
				testArray[indexA][indexB] = pixel;
				//std::cout << setw(3) << int(pixel) << " ";
			}
		}

		// 3) HOG transformation
		HoG.setPicXsize(width);
		HoG.setPicYsize(height);

		vector<vector<unsigned char> > hogMatrixMag(((height / HoG.pixelStep) - HoG.getWinYsize()), vector<unsigned char>((width / HoG.pixelStep) - HoG.getWinXsize()));
		vector<vector<int> > hogMatrixAng(((height / HoG.pixelStep) - HoG.getWinYsize()), vector<int>((width / HoG.pixelStep) - HoG.getWinXsize()));

		// calculating the HOG
		HoG.getSlidingWindows(testArray, hogMatrixMag, hogMatrixAng);

		// distinguish between positive and negative
		vector<double> v_target(nTargets);
		if (tSet.pos[i] == true)
		{
			v_target[0] = 1.0;
			v_target[1] = 1.0;
			v_target[2] = 1.0;
			// push back the new POSITIVE dataEntry - shall be thinkable to somehow straighten the vectors
			dataPos.push_back(new dataEntry(hogMatrixAng, v_target));
		}
		else
		{
			v_target[0] = 0.0;
			v_target[1] = 0.0;
			v_target[2] = 0.0;
			// push back the new NEGATIVE dataEntry - shall be thinkable to somehow straighten the vectors
			dataNeg.push_back(new dataEntry(hogMatrixAng, v_target));
		}

		cout << i << ": Loading of file " << tSet.picNames[indexForCout] << " is done." << '\n';
	}

	//--------------------------------------------------------------------------------------------------------
	// Spliting the data set - 60% TrainigSet, 20% GeneralizationSet, 20% TestingSet
	//--------------------------------------------------------------------------------------------------------
	cout << "Complately data size is: " << dataPos.size() << " of pictures." << endl;
	trainingDataEndIndex = (int)(0.6 * dataPos.size());
	cout << "Training Data set " << trainingDataEndIndex << endl;

	int gSize = (int)(ceil(0.2 * dataPos.size()));
	int vSize = (int)(dataPos.size() - trainingDataEndIndex - gSize);

	//--------------------------------------------------------------------------------------------------------
	// generalization set
	//--------------------------------------------------------------------------------------------------------

	for (int i = trainingDataEndIndex; i < trainingDataEndIndex + gSize; i++)
	{
		tSet.generalizationSet.push_back(dataPos[i]);
	}
	for (int i = 0; i < (int)(dataNeg.size()); i++)
	{
		tSet.generalizationSet.push_back(dataNeg[i]);
	}

	// trying to push back the pictures which i allready used to lear my NN
	/*for (int i = 0; i < gSize; i++)
	{
		tSet.validationSet.push_back(data[i]);
	}*/

	//--------------------------------------------------------------------------------------------------------
	// validation set
	//--------------------------------------------------------------------------------------------------------
	for (int i = trainingDataEndIndex + gSize; i < (int)dataPos.size(); i++)
	{
		tSet.validationSet.push_back(dataPos[i]);
	}
	for (int i = 0; i < (int)(dataNeg.size()); i++)
	{
		tSet.validationSet.push_back(dataNeg[i]);
	}

	// trying to push back the pictures which i allready used to lear my NN
	/*for (int i = 0; i < gSize; i++)
	{
		tSet.validationSet.push_back(data[i]);
	}*/

	//slWin.matrixPrintOut(tSet.validationSet(pattern), 1);

	//cout << "Input neurons has to be: " << inputNeuronsSize << '\n';

	return true;
}

/*******************************************************************
* Selects the data set creation approach
********************************************************************/
void dataReader::setCreationApproach( int approach, double param1, double param2 )
{
	//static
	if ( approach == STATIC )
	{
		creationApproach = STATIC;
		
		//only 1 data set
		numTrainingSets = 1;
	}

	//growing
	else if ( approach == GROWING )
	{			
		if ( param1 <= 100.0 && param1 > 0)
		{
			creationApproach = GROWING;
		
			//step size
			growingStepSize = param1 / 100;
			growingLastDataIndex = 0;

			//number of sets
			numTrainingSets = (int) ceil( 1 / growingStepSize );				
		}
	}

	//windowing
	else if ( approach == WINDOWING )
	{
		//if initial size smaller than total entries and step size smaller than set size
		if ( param1 < dataPos.size() && param2 <= param1)
		{
			creationApproach = WINDOWING;
			
			//params
			windowingSetSize = (int) param1;
			windowingStepSize = (int) param2;
			windowingStartIndex = 0;			

			//number of sets
			numTrainingSets = (int) ceil( (double) ( trainingDataEndIndex - windowingSetSize ) / windowingStepSize ) + 1;
		}			
	}

}

/*******************************************************************
* Returns number of data sets created by creation approach
********************************************************************/
int dataReader::getNumTrainingSets()
{
	return numTrainingSets;
}
/*******************************************************************
* Get data set created by creation approach
********************************************************************/
trainingDataSet* dataReader::getTrainingDataSet()
{		
	switch ( creationApproach )
	{	
		case STATIC : createStaticDataSet(); break;
		case GROWING : createGrowingDataSet(); break;
		case WINDOWING : createWindowingDataSet(); break;
	}
	
	return &tSet;
}
/*******************************************************************
* Get all data entries loaded
********************************************************************/
vector<dataEntry*>& dataReader::getAllDataEntries()
{
	return dataPos;
}

/*******************************************************************
* Create a static data set (all the entries)
********************************************************************/
void dataReader::createStaticDataSet()
{
	//cout << trainingDataEndIndex << endl;
	//training set
	for (int i = 0; i < trainingDataEndIndex; i++)
	{
		tSet.trainingSet.push_back(dataPos[i]);
		
		// printing right actualy stored pattern
		//cout << endl << "Hog matrix angles" << endl;
		//slWin.matrixPrintOut(tSet.trainingSet[i]->pattern, 8);
	}
}
/*******************************************************************
* Create a growing data set (contains only a percentage of entries
* and slowly grows till it contains all entries)
********************************************************************/
void dataReader::createGrowingDataSet()
{
	//increase data set by step percentage
	growingLastDataIndex += (int) ceil( growingStepSize * trainingDataEndIndex );		
	if ( growingLastDataIndex > (int) trainingDataEndIndex ) growingLastDataIndex = trainingDataEndIndex;

	//clear sets
	tSet.trainingSet.clear();
	
	//training set
	for ( int i = 0; i < growingLastDataIndex; i++ ) tSet.trainingSet.push_back( dataPos[i] );			
}
/*******************************************************************
* Create a windowed data set ( creates a window over a part of the data
* set and moves it along until it reaches the end of the date set )
********************************************************************/
void dataReader::createWindowingDataSet()
{
	//create end point
	int endIndex = windowingStartIndex + windowingSetSize;
	if ( endIndex > trainingDataEndIndex ) endIndex = trainingDataEndIndex;		

	//clear sets
	tSet.trainingSet.clear();
					
	//training set
	for ( int i = windowingStartIndex; i < endIndex; i++ ) tSet.trainingSet.push_back( dataPos[i] );
			
	//increase start index
	windowingStartIndex += windowingStepSize;
}

