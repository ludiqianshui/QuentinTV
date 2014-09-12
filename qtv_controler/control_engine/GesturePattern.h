#pragma once
class GesturePattern
{
private: 	
	static const bool outputDebugInformation = FALSE;
	static const int maxFeaturesVectors=120;
	static const int maxFeatures=8;
	double deletionPenalty; // = 1.0;
	double insertionPenalty; // = 1.0;
	double substitutionPenalty; // = 1.0;
	

	char *patternName;
	int numberOfFeatures;

	char *int2char(int i);
	char *double2char(double d);
	double computeVectorDistance(int i, int j, GesturePattern *gp);

public:
	double **features;
	int numberOfFeatureVectors;

	GesturePattern();
	//GesturePattern(char *name, int numfeaturevectors, int numfeatures, double **values);(
	~GesturePattern(void);

	void   init(char *name, int numOfFeatVects, int numOfFeats);
	void   setPatternName(char *name);
	void   setNumberOfFeatureVectors(int n);
	void   setNumberOfFeatures(int n);
	void   resetNumOfFeatureVectors() { numberOfFeatureVectors = 0; }

	 //void   addFeatureVector(double a,double b,double c,double d,double e,double f,double g,double h,  int numOfFeat);
	void   addFeatureVector(double *feat, int numOfFeat);
	void   addFeature(int vector, int featurNr, double f);
	double getFeature(int vector, int featureNr);
	
	int    getNumberOfFeatureVectors() { return numberOfFeatureVectors; }
	int    getNumberOfFeatures() { return numberOfFeatures; }

	void    incNumberOfFeatureVectors();
	
	//void   setPattern(char *name, int numOfVects, int numOfFeat, double **feats);
	double compare (GesturePattern *gp);
	char *getPatternName();
	double *getFeatureVector(int n);
	char *getPatternString();
};

