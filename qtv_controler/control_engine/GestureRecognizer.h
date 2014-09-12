
#pragma once
#include "GesturePattern.h"
#include "MedianFilter.h"
class GestureRecognizer
{
private:
	int recognizerID;
	static const bool outputDebugInformation = TRUE;
	static const int MaxGesturePatterns = 20;
	static const int maxFeatures = 8;
	static const int maxFeaturesVectors = 120;
	int TriggerThresholdHigh; // default = 100;  // to be adaptable!!
	int TriggerThresholdLow; // default  = 20;   // to be adaptable!!
	int MinNumberOfSamples; // default = 8;	  // to be adaptable!!
	int MedianFilterWidth; // default = 9;        // to be adaptable!!
	int TimeoutAfterGesture; // default = 1000; // to be adaptable!!
	double rejectThreshold;		// deault = 1.0;				// confidence threshold to be adaptable!!
	
	int startFlag;
	int trainFlag;
	MedianFilter   *medFilterEnergy;
	MedianFilter   *medFilterDX;
	MedianFilter   *medFilterDY;
	MedianFilter   *medFilterDZ;

	GesturePattern *recPattern;
	GesturePattern *gesturePatterns;
	char *lastRecognitionResult;
	int numberOfPatterns;
	double *oldFeatures;
	double integralOfEnergy;
	int    gestureTimeStart;

	char *trainingFilename;
	char *trainingPatternName;

	int recordingFeatures; // 0 = no recording, 1 = recording
	WCHAR *convertCharToLongchar(char *text);

	int deltaEnergy(int x1, int y1, int z1, int x2, int y2, int z2);
	
	HWND windowHandle; 
	int windowForm;
	char outText[10000];

public:
	GestureRecognizer(void);
	~GestureRecognizer(void);

	void setOutputWindow(HWND whdl, int wForm);
	void inputFeatures(int x, int y, int z, int t);
	void outputFeatures();
	void recognize(GesturePattern *gp);
	void train(char *gestureName, char *patternFile);
	void saveTrainingPattern(GesturePattern *gp);
	char *getRecognitionResult();
	void resetRecognitionResult();

	void    setRecognizerID(int id) { recognizerID = id; }
	int     getTriggerThresholdHigh() { return TriggerThresholdHigh; }
	void    setTriggerThresholdHigh(int t) { TriggerThresholdHigh = t; }
	int     getTriggerThresholdLow() { return TriggerThresholdLow; }
	void    setTriggerThresholdLow(int t) { TriggerThresholdLow = t; }
	int     getMinNumberOfSamples() { return MinNumberOfSamples; }
	void    setMinNumberOfSamples(int t) { MinNumberOfSamples = t; }
	int     getMedianFilterWidth() { return MedianFilterWidth; }
	void    setMedianFilterWidth(int t) { MedianFilterWidth = t; }
	int     getTimeoutAfterGesture() { return TimeoutAfterGesture; }
	void    setTimeoutAfterGesture(int t) { TimeoutAfterGesture = t; }
	double  getRejectThreshold() { return rejectThreshold; }
	void    setRejectThreshold(double t) { rejectThreshold = t; }

	void loadPatterns(char *filename);
	char *int2char(int i);
	char *double2char(double d);
};

