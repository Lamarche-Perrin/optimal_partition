#ifndef INCLUDE_LOGARITHMIC_SCORE
#define INCLUDE_LOGARITHMIC_SCORE


#include "objective_function.hpp"
#include "prediction_dataset.hpp"

class LogarithmicScore: public ObjectiveFunction
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
	ObjectiveValue *newObjectiveValue (int index = -1);
	void computeObjectiveValues();
	void printObjectiveValues (bool verbose = true);

	double getParameter (double unit);
	double getUnitDistance (double uMin, double uMax);
	double getIntermediaryUnit (double uMin, double uMax);
};


class LogarithmicObjectiveValue: public ObjectiveValue
{
public:
	int preSize, postSize;
	int *trainCountArray;
	int *testCountArray;
	int trainCountTotal;
	int testCountTotal;
	double score;
	
	LogarithmicObjectiveValue (LogarithmicScore *objective);
	~LogarithmicObjectiveValue ();
		
	void add (ObjectiveValue *value);
	void compute ();
	void compute (ObjectiveValue *value1, ObjectiveValue *value2);
	void compute (ObjectiveValueSet *valueset);
	void normalize (ObjectiveValue *q);
	void print (bool verbosex = true);
	double getValue (double param);
};


#endif
