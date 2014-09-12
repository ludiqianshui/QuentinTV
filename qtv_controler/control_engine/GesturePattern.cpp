#include "StdAfx.h"
#include "GesturePattern.h"
#include <stdio.h>
#include <fstream>
#include <math.h>

using namespace std;

GesturePattern::GesturePattern()
{
	deletionPenalty = 2.0;
	insertionPenalty = 2.0;
	substitutionPenalty = 0.0;

	numberOfFeatureVectors = 0;
	numberOfFeatures = 0;
	patternName = NULL;
	features = NULL;

	features = new double*[maxFeaturesVectors];
	for (int i=0; i<maxFeaturesVectors; i++) {
		features[i] = new double[maxFeatures];
		for (int j=0; j<maxFeatures; j++)
			features[i][j] = 0.0;
	}
}

GesturePattern::~GesturePattern(void)
{
/*	if (features != NULL) {
		for (int i=0; i<numberOfFeatureVectors; i++)
			delete features[i];
		delete features;
		features = NULL;
	}
	*/
	if (patternName!=NULL)
		delete patternName;

	numberOfFeatures = 0;
	numberOfFeatureVectors = 0;
}

void GesturePattern::setNumberOfFeatureVectors(int n){
/*	if (features!=NULL) {
		for (int i=0; i<numberOfFeatureVectors; i++)
			if (features[i] != NULL)
				delete [] features[i];
		delete [] features;
	}*/
	numberOfFeatureVectors = n;
	/*
	features = new double*[maxFeaturesVectors];
	for (int i=0; i<maxFeaturesVectors; i++) {
		features[i] = new double[maxFeatures];
		for (int j=0; j<numberOfFeatures; j++)
			features[i][j] = 0.0;
	}*/
}
void GesturePattern::setNumberOfFeatures(int n){
	/*if (features!=NULL) {
		for (int i=0; i<numberOfFeatureVectors; i++)
			if (features[i] != NULL)
				delete [] features[i];
	}*/
	numberOfFeatures = n;
	/*if (features != NULL)
		for (int i=0; i<maxFeaturesVectors; i++) {
			features[i] = new double[maxFeatures];
			for (int j=0; j<maxFeatures; j++)
				features[i][j] = 0.0;
		}*/

}


void GesturePattern::incNumberOfFeatureVectors()
{
	if (numberOfFeatureVectors<maxFeaturesVectors)
		numberOfFeatureVectors++;
}


void GesturePattern::addFeature(int vector, int featureNr, double f)
{
	if ((vector<numberOfFeatureVectors)&&(featureNr<numberOfFeatures))
		features[vector][featureNr] = f;
}

double GesturePattern::getFeature(int vector, int featureNr)
{
	if ((vector<maxFeaturesVectors)&&(featureNr<numberOfFeatures))
		return features[vector][featureNr];
	else
		return 0.0;
}


void GesturePattern::addFeatureVector( double *feat, int numOfFeat)
{
	if (numOfFeat == numberOfFeatures) {
		if (numberOfFeatureVectors<maxFeaturesVectors) {
			for (int i=0; i<numOfFeat; i++) {
				double d = feat[i] ;
				features[numberOfFeatureVectors][i] = (double)d;
			}
			numberOfFeatureVectors++;
		}
	}
}

void  GesturePattern::setPatternName(char *name)
{
	if (patternName != NULL) delete [] patternName;
	patternName = new char[strlen(name)+1];
	strcpy(patternName, name);
}

char *GesturePattern::getPatternName()
{
	return patternName;
}

double *GesturePattern::getFeatureVector(int n)
{

	double *res = new double[numberOfFeatures];
	for (int i=0; i<numberOfFeatures; i++) {
		double d = features[n][i];
		res[i] = d;
	}
	return res;
}

double GesturePattern::computeVectorDistance( int i, int j, GesturePattern *gp)
{
	double result = 0.0;
	double distance = 0.0;

	double l1 = 1.0;
	double l2 = 1.0;

	// compute cos distance
	if (numberOfFeatures == (*gp).getNumberOfFeatures()) {
		/* use only the delta coefficients (4-6)
		   feature 0  ~ t
		   feature 1  ~ x
		   feature 2  ~ y
		   feature 3  ~ z
		   feature 4  ~ dx
		   feature 5  ~ dy
		   feature 6  ~ dz
		   feature 7  ~ E

		*/
		for (int k=4; k<7; k++) {
			l1 += features[i][k]*features[i][k];
			l2 += (*gp).features[j][k]*(*gp).features[j][k];
			distance += features[i][k]*(*gp).features[j][k];
		}
	} else {
		distance = 1.0;
	}
	if (l1*l2 != 0.0)
		result = 1.0 - distance /sqrt(l1*l2);
		//result = distance /sqrt(l1*l2);
	else
		result = 2.0;
	return result;
}

double GesturePattern::compare(GesturePattern *gp) 
{
	double outputResult = 0.0;

	// create the result field for dynamic programming
	double **result;
	result = new double*[numberOfFeatureVectors+1];
	for (int i=0; i<numberOfFeatureVectors+1; i++)
	{
		result[i] = new double[(*gp).numberOfFeatureVectors+1];
		for (int j=0; j<(*gp).numberOfFeatureVectors+1; j++)
			result[i][j] = 0.0;
	}
	
	// init the result field
	for (int i=1; i<numberOfFeatureVectors+1; i++)
		result[i][0] = result[i-1][0] +  deletionPenalty;
	for (int j=1; j<(*gp).numberOfFeatureVectors+1; j++)
		result[0][j] = result[0][j-1] + insertionPenalty;
	
	// do the dynamic programming, fill the matrix with the minimal distances
	for (int i=1; i<numberOfFeatureVectors+1; i++) {
		for (int j=1; j<(*gp).numberOfFeatureVectors+1; j++) {
			double tmp0 = result[i-1][j] + deletionPenalty;
			double tmp1 = result[i][j-1] + insertionPenalty;
			double tmp2 = result[i-1][j-1] + computeVectorDistance(i-1, j-1, gp) ;// + substitutionPenalty;
			result[i][j] = min(tmp0,min(tmp1,tmp2));
		}
	}
	outputResult = result[numberOfFeatureVectors][(*gp).numberOfFeatureVectors];
	
	// remove the result field
	for (int i=0; i<numberOfFeatureVectors+1; i++)
		delete [] result[i];
	delete [] result;
	
	return outputResult/(double)numberOfFeatureVectors;
}


char *GesturePattern::getPatternString() {
	char *output= NULL;
	output = new char[1000];
	strcpy(output,"<GesturePattern>\r\n");
	strcat(output, "\t<Name>");	strcat(output, patternName); strcat(output, "</Name>\r\n");
	strcat(output, "\t<Features n="); strcat(output, int2char(numberOfFeatureVectors)); strcat(output, ">\r\n");
	for (int i=0; i<numberOfFeatureVectors; i++) {
		strcat(output, "\t\t<Vector n="); strcat(output, int2char(numberOfFeatures)); strcat(output, ">");
		for (int j=0; j<numberOfFeatures; j++) {
			if ((features!=NULL)&&(features[i]!=NULL)) {
				double f=features[i][j];
				strcat(output, double2char(f)); strcat(output," ");
			}
		}
		strcat(output, "</Vector>\r\n");
	}
	strcat(output, "\t</Features>\r\n");
	strcat(output,"</GesturePattern>\r\n");
	return output;
}

char *GesturePattern::int2char(int i)
{
	char *res;
	char tmp[256];
	itoa(i,tmp,10);
	res = new char[strlen(tmp)+1];
	strcpy(res,tmp);
	return res;
}

char *GesturePattern::double2char(double d)
{
	char *res;
	char tmp[256];
	sprintf(tmp,"%f",d);
	res = new char[strlen(tmp)+1];
	strcpy(res,tmp);
	return res;
}

void   GesturePattern::init(char *name, int numOfFeatVects, int numOfFeats)
{
	patternName = new char[strlen(name)+1];
	strcpy(patternName, name);

	numberOfFeatures = numOfFeats;
	numberOfFeatureVectors = numOfFeatVects;
	features= new double*[numberOfFeatureVectors];
	for (int i=0; i<numberOfFeatureVectors; i++) {
		features[i] = new double[numberOfFeatures];
		for (int j=0; j<numberOfFeatures; j++)
			features[i][j]=0.0;
	}
}