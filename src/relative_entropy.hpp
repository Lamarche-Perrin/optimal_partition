#ifndef INCLUDE_RELATIVE_ENTROPY
#define INCLUDE_RELATIVE_ENTROPY


#include "objective_function.hpp"

class RelativeEntropy: public ObjectiveFunction
{
public:
	int size;
	double *values;
	double *refValues;
		
	RelativeEntropy (int size, double *values = 0, double *refValues = 0);
	~RelativeEntropy ();
		
	void setRandom ();
	ObjectiveValue *newObjectiveValue (int index = -1);
	void computeObjectiveValues ();
	void printObjectiveValues (bool verbose = true);

	double getParameter (double unit);
	double getUnitDistance (double uMin, double uMax);
	double getIntermediaryUnit (double uMin, double uMax);
};


class RelativeObjectiveValue: public ObjectiveValue
{
public:
	int index;
	double sumValue;
	double sumRefValue;
	double microInfo;
	double divergence;
	double sizeReduction;
		
	RelativeObjectiveValue (RelativeEntropy *objective, int index = -1);
	~RelativeObjectiveValue ();
		
	void add (ObjectiveValue *value);
	void compute ();
	void compute (ObjectiveValue *value1, ObjectiveValue *value2);
	void compute (ObjectiveValueSet *valueset);
	void normalize (ObjectiveValue *q);
	void print (bool verbose = true);
	double getValue (double param);
};


#endif
