#include "StdAfx.h"
#include "MedianFilter.h"

#include <stdio.h>
#include <fstream>

using namespace std;

MedianFilter::MedianFilter(void)
{
	values = NULL;
	maxNumberOfValues=0;
	pointer = 0;
	addedNewValue = (0==1);
	double median = 0.0;
}


MedianFilter::MedianFilter(int maxN)
{
	values = new double[maxN];
	for (int i=0; i<maxN; i++)
		values[i] = 0.0;
	maxNumberOfValues=maxN;
	pointer = 0;
	addedNewValue = (0==1);
	double median = 0.0;
}

MedianFilter::~MedianFilter(void)
{
	if (values!=NULL)
		delete [] values;
	maxNumberOfValues = 0;
	pointer = 0;
}

void MedianFilter::addValue(double f)
{
	values[pointer++] = f;
	if (pointer>=maxNumberOfValues)
		pointer = 0;
	addedNewValue = (0==0);
}

double MedianFilter::getMedian()
{
	/*
	if(addedNewValue) {
		double sum=0.0;
		for (int i=0; i<maxNumberOfValues; i++)
			sum+= values[i];
		median = sum/(double)maxNumberOfValues;
	}*/
	if (addedNewValue) {
		double *tmpd = new double[maxNumberOfValues];
		for (int i=0; i<maxNumberOfValues; i++) {
			tmpd[i]=values[i];
		}

		for (int i=0; i<maxNumberOfValues; i++)
			for (int j=1; j<maxNumberOfValues-i; j++)
				if (tmpd[j-1]>tmpd[j]) {
					double tmpdd = tmpd[j];
					tmpd[j] = tmpd[j-1];
					tmpd[j-1] = tmpdd;
				}
		median = tmpd[maxNumberOfValues/2];
		delete [] tmpd;
	}
	return median;
}
