#ifndef INCLUDE_LOGARITHMIC_SCORE
#define INCLUDE_LOGARITHMIC_SCORE


#include "measure.hpp"
#include "prediction_dataset.hpp"

class LogarithmicScore: public Measure
{
public:
	PredictionDataset *dataset;

	int preSize, postSize;
	int *trainCountArray;
	int trainCountTotal;
	int prior;
	
	LogarithmicScore (PredictionDataset *dataset, int prior = 0);
	~LogarithmicScore ();
	
	void setRandom ();
	Quality *newQuality (int index = -1);
	void computeQuality();
	void printQuality (bool verbose = true);

	double getParameter (double unit);
	double getUnitDistance (double uMin, double uMax);
	double getIntermediaryUnit (double uMin, double uMax);
};


class LogarithmicQuality: public Quality
{
public:
	int preSize, postSize;
	int *trainCountArray;
	int *testCountArray;
	int trainCountTotal;
	int testCountTotal;
	double score;
	
	LogarithmicQuality (LogarithmicScore *measure);
	~LogarithmicQuality ();
		
	void add (Quality *quality);
	void compute ();
	void compute (Quality *quality1, Quality *quality2);
	void compute (QualitySet *qualityset);
	void normalize (Quality *q);
	void print (bool verbosex = true);
	double getValue (double param);
};


#endif
