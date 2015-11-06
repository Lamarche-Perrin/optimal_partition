#ifndef INCLUDE_RELATIVE_ENTROPY
#define INCLUDE_RELATIVE_ENTROPY


#include "measure.hpp"

class RelativeEntropy: public Measure
{
public:
	int size;
	double *values;
	double *refValues;
		
	RelativeEntropy (int size, double *values = 0, double *refValues = 0);
	~RelativeEntropy ();
		
	void setRandom ();
	Quality *newQuality (int index = -1);
	void computeQuality ();
	void printQuality (bool verbose = true);

	double getParameter (double unit);
	double getUnitDistance (double uMin, double uMax);
	double getIntermediaryUnit (double uMin, double uMax);
};


class RelativeQuality: public Quality
{
public:
	int index;
	double sumValue;
	double sumRefValue;
	double microInfo;
	double divergence;
	double sizeReduction;
		
	RelativeQuality (RelativeEntropy *measure, int index = -1);
	~RelativeQuality ();
		
	void add (Quality *quality);
	void compute ();
	void compute (Quality *quality1, Quality *quality2);
	void compute (QualitySet *qualityset);
	void normalize (Quality *q);
	void print (bool value = true);
	double getValue (double param);
};


#endif
