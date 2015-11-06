#ifndef INCLUDE_BIDIMENSIONAL_RELATIVE_ENTROPY
#define INCLUDE_BIDIMENSIONAL_RELATIVE_ENTROPY

/*
#include "measure.hpp"

class BidimensionalRelativeEntropy: public Measure
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
		Quality *newQuality (int index = -1);

		double getParameter (double unit);
		double getUnitDistance (double uMin, double uMax);
		double getIntermediaryUnit (double uMin, double uMax);
};


class BidimensionalRelativeQuality: public Quality
{
	public:
		int index;
		
		double sumValue;
		double sumRefValue;
		double microInfo;
		double divergence;
		double sizeReduction;
		
		BidimensionalRelativeQuality (BidimensionalRelativeEntropy *measure, int index = -1);
		~BidimensionalRelativeQuality ();
		
		void add (Quality *quality);
		void compute ();
		void compute (Quality *quality1, Quality *quality2);
		void compute (QualitySet *qualityset);
		void normalize (Quality *q);
		void print (bool value = true);
		double getValue (double param);
};

*/
#endif
