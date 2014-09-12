#include "StdAfx.h"
#include "GestureRecognizer.h"
#include <stdio.h>
#include <fstream>
#include <time.h>

using namespace std;

GestureRecognizer::GestureRecognizer(void)
{
	// init the changeable system paramters with default values
	TriggerThresholdHigh = 120;
	TriggerThresholdLow  = 20;
	MinNumberOfSamples = 8;
	MedianFilterWidth = 9;
	TimeoutAfterGesture = 1500;
	rejectThreshold = 1.0;

	// init the control flags and parameters
	strcpy(outText,"");
	recordingFeatures = 0; // false
	startFlag = (1==1);
	trainFlag = (0==1);
	numberOfPatterns = 0;
	windowHandle = NULL;
	lastRecognitionResult = NULL;
	
	oldFeatures= new double[maxFeatures];
	for (int i=0; i<maxFeatures; i++)
		oldFeatures[i] = 0.0;
	integralOfEnergy = 0.0;
	

	// init the different operators and Objects
	recPattern = new GesturePattern();

	medFilterEnergy = new MedianFilter(MedianFilterWidth);
	medFilterDX = new MedianFilter(MedianFilterWidth);
	medFilterDY = new MedianFilter(MedianFilterWidth);
	medFilterDZ = new MedianFilter(MedianFilterWidth);
	(*recPattern).setPatternName("");
	(*recPattern).setNumberOfFeatureVectors(120);
	(*recPattern).setNumberOfFeatures(8);
	rejectThreshold = 1.0;
	gestureTimeStart = clock();
}


GestureRecognizer::~GestureRecognizer(void)
{
}

int GestureRecognizer::deltaEnergy(int x1, int y1, int z1, int x2, int y2, int z2)
{
	return ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
}

void GestureRecognizer::setOutputWindow(HWND whdl, int wForm)
{
	 windowHandle = whdl;
	 windowForm = wForm;
}

void GestureRecognizer::inputFeatures(int x, int y, int z, int t)
{
	ofstream myFeatStream;
	int deltaE = 0;

	(*medFilterDX).addValue(oldFeatures[1] - x);
	(*medFilterDY).addValue(oldFeatures[2] - y);
	(*medFilterDZ).addValue(oldFeatures[3] - z);

	if (!startFlag)
		deltaE = deltaEnergy(oldFeatures[1],oldFeatures[2],oldFeatures[3],x,y,z);
	else
		deltaE = 0;
	(*medFilterEnergy).addValue(deltaE);

	
	oldFeatures[4] = (*medFilterDX).getMedian(); //oldFeatures[1] - x;
	oldFeatures[5] = (*medFilterDY).getMedian(); //oldFeatures[2] - y;
	oldFeatures[6] = (*medFilterDZ).getMedian(); //oldFeatures[3] - z;
	oldFeatures[0] = (double)t;
	oldFeatures[1] = (double)x;
	oldFeatures[2] = (double)y;
	oldFeatures[3] = (double)z;
	oldFeatures[7] = (double)(*medFilterEnergy).getMedian();
	
	//
	// if in debug mode output the features to the file "debug-featurestream.txt"
	//
	if (outputDebugInformation==TRUE) {
		myFeatStream.open("debug-featurestream.txt", ios_base::app);
	
		for (int i=0; i<maxFeatures; i++)
			myFeatStream<<oldFeatures[i]<<"\t";
		myFeatStream<<"\n";
		myFeatStream.close();
	}
	
	if (clock()>gestureTimeStart) {
		//SetDlgItemInt( windowHandle, windowForm, clock(),FALSE);
		if (recordingFeatures == 0) { // no recording at the moment
			(*recPattern).resetNumOfFeatureVectors();
			if (((*medFilterEnergy).getMedian()>TriggerThresholdHigh)&&(!startFlag)) {
				recordingFeatures = 1;
				(*recPattern).resetNumOfFeatureVectors();
				(*recPattern).addFeatureVector(oldFeatures, maxFeatures);
				startFlag = (0==1);
			} else {
				recordingFeatures = 0;
				startFlag = (0==1);
			}
		} else
		if (recordingFeatures == 1) { // recording runs
			//(*recPattern).addFeatureVector(maxFeatures, oldFeatures);
			if ((*recPattern).numberOfFeatureVectors<maxFeaturesVectors) {
				(*recPattern).addFeatureVector(oldFeatures, maxFeatures);
			}

			if ((*medFilterEnergy).getMedian()>TriggerThresholdLow) {
				recordingFeatures = 1;
			} else { // start processing
				if ((*recPattern).getNumberOfFeatureVectors()>MinNumberOfSamples) {		
									//
				// do recognition
				//
				
				// output recorded features for debugging and training
					strcpy(outText,"");
					outputFeatures();
							//strcpy(outText,(*recPattern).getPatternString());
				// here the processing should take place
				//strcat(outText, "starting recognizing the gesture\r\n");
				
					char *tmpName = NULL;
					if (trainFlag){
						saveTrainingPattern(recPattern);
						trainFlag = (0==1);
					} else {
						//if (clock()>gestureTimeStart+thresholdAfterGesture) {
						recognize(recPattern);
						gestureTimeStart = clock()+TimeoutAfterGesture;
						//}
					}
				
									// output the recognition result
				
				//if (getRecognitionResult() != NULL)
				//	strcat(outText,strcat("recognized gesture: ", getRecognitionResult() ));
				//strcat(outText, "finishing recognizing the gesture\r\n");
					recordingFeatures = 0;
					if (windowHandle != NULL) {
						SetDlgItemText( windowHandle, windowForm, convertCharToLongchar(outText));
					}
				}

			}
		} else {
			SetDlgItemText( windowHandle, windowForm, L"Waiting for TimeOut, No Features to Input");
		}
	}
	
}

void GestureRecognizer::recognize(GesturePattern *gpToComp) 
{
	double minDist = 0.0;
	int    minIndex = 0;
	double dd=999999.9;
	

	strcat(outText, "starting recognize() - RECOGNIZER: ");
	strcat(outText, int2char(recognizerID));
	strcat(outText, "\r\n");
	
	if (windowHandle != NULL) {
		SetDlgItemText( windowHandle, windowForm, convertCharToLongchar(outText));
	}

	strcat(outText, "Number of Patterns: ");
	strcat(outText, int2char(numberOfPatterns));
	strcat(outText, "\r\n");
	
	if (numberOfPatterns>0) {
		minDist = (gesturePatterns[0].compare(gpToComp));
		minIndex = 0;

		strcat(outText, "Pattern: 0 ");
		strcat(outText, gesturePatterns[0].getPatternName() );
		strcat(outText, " := ");
		strcat(outText, double2char(minDist));
		strcat(outText, "\r\n");
	
		for (int i=1; i<numberOfPatterns; i++) {
			dd = gesturePatterns[i].compare(gpToComp);
			strcat(outText, "Pattern ");
			strcat(outText, int2char(i));
			strcat(outText, " ");
			strcat(outText, gesturePatterns[i].getPatternName() );
			strcat(outText, " := ");
			strcat(outText, double2char(dd));
			strcat(outText, "\r\n");
			if (minDist> (dd)) {
				minDist = dd; 
				minIndex = i; 
			}
		}
	}	
	ofstream myFile;

	if (outputDebugInformation == TRUE) {
		myFile.open("debug-endpointedFeatureSequences.txt", ios_base::app);
		myFile<<"Minimal Distance:"<<minDist<<"\n";
		myFile<<"Minimal Index:"<<minIndex<<"\n";
		myFile<<"Result:"<<gesturePatterns[minIndex].getPatternName()<<"\n\n";
		myFile.close();
	}

	strcat(outText, "Minimal Distance: ");
	strcat(outText, double2char(minDist));
	strcat(outText, "\r\n");
	strcat(outText, "Minimal Index: ");
	strcat(outText, int2char(minIndex));
	strcat(outText, "\r\n");
	strcat(outText, "Result:");
	strcat(outText, gesturePatterns[minIndex].getPatternName());
	strcat(outText, "\r\n");


	
	if (lastRecognitionResult!=NULL)
		delete [] lastRecognitionResult;

	if (minDist<rejectThreshold) {
		lastRecognitionResult = new char[255];
		strcpy(lastRecognitionResult, gesturePatterns[minIndex].getPatternName() ); 
	} else {
		lastRecognitionResult = NULL;
	}

	if (lastRecognitionResult!=NULL) {
		strcat(outText, "Recognition Result:");
		strcat(outText, gesturePatterns[minIndex].getPatternName());
		strcat(outText, "\r\n");
	} else {
		strcat(outText, "Result Rejected!!!! ");
	}
	strcat(outText, "finishing recognize()\r\n");

	
}


void GestureRecognizer::saveTrainingPattern(GesturePattern *gp)
{
	strcat(outText, "Saving the recorded pattern\r\n");
	strcat(outText, "Filename: ");
	strcat(outText, trainingFilename);
	strcat(outText, "\r\n");
	strcat(outText, "Training Pattern: ");
	strcat(outText, trainingPatternName);
	strcat(outText, "\r\n");
	strcat(outText, "Number Of Feature Vectors: ");
	strcat(outText, int2char((*gp).getNumberOfFeatureVectors()));
	strcat(outText, "\r\n");
	strcat(outText, "Number Of Features: ");
	strcat(outText, int2char((*gp).getNumberOfFeatures()));
	strcat(outText, "\r\n");
	ofstream myFile;
	myFile.open(trainingFilename, ios_base::app);
	myFile<<"\n";
	myFile<<trainingPatternName<<"\n";
	myFile<<(*gp).getNumberOfFeatureVectors()<<" "<<(*gp).getNumberOfFeatures()<<"\n";
	for (int i=0; i<(*gp).getNumberOfFeatureVectors(); i++) {
		for (int j=0; j<(*gp).getNumberOfFeatures(); j++) {
			myFile<<(int)(*gp).getFeature(i,j)<<" ";
		}
		myFile<<"\n";
	}
	myFile<<"\n";
	myFile.close();
}
void GestureRecognizer::train(char *gestureName, char *patternFile)
{
	trainingFilename = new char[strlen(patternFile)+1];
	strcpy(trainingFilename, patternFile);
	trainingPatternName = new char[strlen(gestureName)+1];
	strcpy(trainingPatternName, gestureName);
	if (trainFlag)
		trainFlag = (0==1);
	else
		trainFlag = (1==1);
}

void GestureRecognizer::resetRecognitionResult()
{
	if (lastRecognitionResult!=NULL)
		delete [] lastRecognitionResult;
	lastRecognitionResult= NULL;
}

void GestureRecognizer::outputFeatures()
{
	
	int dx = 0;
	int dy = 0;
	int dz = 0;
	int e = 0;
	ofstream myFile;
	if (outputDebugInformation == TRUE) {
		myFile.open("debug-endpointedFeatureSequences.txt", ios_base::app);
		myFile<<"NumberOfFeatureVectors: "<<(*recPattern).getNumberOfFeatureVectors()<<"\n";
		myFile<<"NumberOfFeatures: "<<(*recPattern).getNumberOfFeatures()<<"\n";

		for (int i=0; i<(*recPattern).getNumberOfFeatureVectors(); i++) {
			myFile<<i<<": ";
			for (int j=0; j<maxFeatures; j++)
				myFile<<(*recPattern).features[i][j]<<"\t";
			myFile<<"\n";
		}
		myFile.close();	
	}
}

char *GestureRecognizer::getRecognitionResult()
{
	char *result = NULL;
	result = lastRecognitionResult;
	return result;
}


WCHAR *GestureRecognizer::convertCharToLongchar(char *text)
{
	int textlen = strlen(text);
	WCHAR *tmpstr = new WCHAR[textlen];
	MultiByteToWideChar(CP_ACP,0, text, -1, tmpstr, textlen);
	return tmpstr;
	//SetDlgItemText( pthis->m_hWnd, IDC_DEBUG, tmpstr);
}


char *GestureRecognizer::int2char(int i)
{
	char *res;
	char tmp[256];
	itoa(i,tmp,10);
	res = new char[strlen(tmp)+1];
	strcpy(res,tmp);
	return res;
}

char *GestureRecognizer::double2char(double d)
{
	char *res;
	char tmp[256];
	sprintf(tmp,"%f",d);
	res = new char[strlen(tmp)+1];
	strcpy(res,tmp);
	return res;
}


void GestureRecognizer::loadPatterns(char *filename)
{
	LPCWSTR outputText = L"Reading Pattern File!";

	if (windowHandle != NULL) {
		SetDlgItemText( windowHandle, windowForm, outputText);
	}

	ifstream myInputFile;
	myInputFile.open(filename);

	if (myInputFile) {

		//
		// format for the patterns: name
		//							number of features vectore
		//							feature vector, numbered from 1 to n 
		//
		char tmpName[256];
		int  tmpNumOfVectors;
		int  tmpNumOfFeatures;
		//double *tmpFeatureVector = new double[maxFeatures];
		double tmpFeatureVector[maxFeatures];
		//char outText[6000];
		strcpy(outText,"");

		myInputFile>>numberOfPatterns;
		gesturePatterns = new GesturePattern[numberOfPatterns+1];

		strcat(outText,"Number of patterns: ");
		strcat(outText, int2char(numberOfPatterns));
		strcat(outText,"\r\n");
		if (windowHandle != NULL) {
			SetDlgItemInt( windowHandle, windowForm, numberOfPatterns, false);
		}

		for (int i=0; i<numberOfPatterns; i++){
			myInputFile>>tmpName;
			strcat(outText,tmpName); strcat(outText,"\r\n");
			myInputFile>>tmpNumOfVectors;
			strcat(outText, int2char(tmpNumOfVectors)); strcat(outText," ");
			myInputFile>>tmpNumOfFeatures;
			strcat(outText, int2char(tmpNumOfFeatures)); strcat(outText,"\r\n");
		
			//gesturePatterns[i].init(tmpName, tmpNumOfVectors, tmpNumOfFeatures);
			gesturePatterns[i].setPatternName(tmpName);
			gesturePatterns[i].setNumberOfFeatures(tmpNumOfFeatures);
			gesturePatterns[i].setNumberOfFeatureVectors(tmpNumOfVectors);
			gesturePatterns[i].resetNumOfFeatureVectors();

		
			for (int j=0; j<tmpNumOfVectors; j++) {
				for (int k=0; k<tmpNumOfFeatures; k++) {				
					int tmpI;
					myInputFile>>tmpI;
					tmpFeatureVector[k] = tmpI;
					//myOutFile<<"tmpI["<<j<<","<<k<<"] = "<<tmpFeatureVector[k]<<"  "; myOutFile.flush();
					strcat(outText, int2char(tmpI)); strcat(outText," ");
	//				(gesturePatterns[i]).features[j][k]=tmpI;
				}
				strcat(outText, "\r\n");
				(gesturePatterns[i]).addFeatureVector( tmpFeatureVector, maxFeatures);
			}
			if (windowHandle != NULL) {
				SetDlgItemText( windowHandle, windowForm, convertCharToLongchar(outText));
			}		
		}
		myInputFile.close();
	} else {
		if (windowHandle != NULL) {
			SetDlgItemText( windowHandle, windowForm, L"ERROR: Pattern File Not Found!!");
		}		
	}
	 
	
	//myOutFile.close();

}

