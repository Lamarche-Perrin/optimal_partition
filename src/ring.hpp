#ifndef INCLUDE_RING
#define INCLUDE_RING

#include "constrained_set.hpp"

class Ring: public ConstrainedSet
{
	public:
		int size;
	
		double *values;
		
		double *sumValues;
		double *microInfos;
		double *sizeReductions;
		double *divergences;
	
		double *optimalQualities;
		int *optimalCuts;
		int firstOptimalCut;
		int lastOptimalCut;
	
		Ring (int s);
		virtual ~Ring ();

		int getIndex (int i, int j);

		void setMeasure (Measure *m);
		void setRandom ();
		void print ();

		void buildDataStructure ();
		void computeQuality ();
		void normalizeQuality ();
		void printQuality ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);

		ConstrainedSet *getRandomSet (int size);
};

#endif
