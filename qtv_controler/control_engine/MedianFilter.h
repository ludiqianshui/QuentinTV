#pragma once
class MedianFilter
{
	double *values;
	int maxNumberOfValues;
	int pointer;

	int addedNewValue;
	double median;
public:
	MedianFilter(void);
	MedianFilter(int maxN);
	~MedianFilter(void);

	void addValue(double f);
	double getMedian();
};

