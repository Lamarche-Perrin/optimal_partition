#ifndef INCLUDE_BIDIMENSIONAL_RELATIVE_ENTROPY
#define INCLUDE_BIDIMENSIONAL_RELATIVE_ENTROPY

/*
#include "objective_function.hpp"

class BidimensionalRelativeEntropy: public ObjectiveFunction
{
	public:
		int size1;
		int size2;
		double *values;
		double *refValues1;
		double *refValues2;
		
		BidimensionalRelativeEntropy (int size1, int size2, double *values = 0, double *refValues1 = 0, double *refValues2 = 0);
		~BidimensionalRelativeEntropy ();
		
		void setRandom ();
		ObjectiveValue *newObjectiveValue (int index = -1);

		double getParameter (double unit);
		double getUnitDistance (double uMin, double uMax);
		double getIntermediaryUnit (double uMin, double uMax);
};


class BidimensionalRelativeObjectiveValue: public ObjectiveValue
{
	public:
		int index;
		
		double sumValue;
		double sumRefValue;
		double microInfo;
		double divergence;
		double sizeReduction;
		
		BidimensionalRelativeObjectiveValue (BidimensionalRelativeEntropy *objective, int index = -1);
		~BidimensionalRelativeObjectiveValue ();
		
		void add (ObjectiveValue *value);
		void compute ();
		void compute (ObjectiveValue *value1, ObjectiveValue *value2);
		void compute (ObjectiveValueSet *valueset);
		void normalize (ObjectiveValue *q);
		void print (bool verbose = true);
		double getValue (double param);
};

*/
#endif
