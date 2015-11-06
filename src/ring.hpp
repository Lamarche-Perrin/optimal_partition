#ifndef INCLUDE_RING
#define INCLUDE_RING

#include "abstract_set.hpp"

class Ring: public AbstractSet
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

		void setObjectiveFunction (ObjectiveFunction *m);
		void setRandom ();
		void print ();

		void buildDataStructure ();
		void computeObjectiveValues ();
		void normalizeObjectiveValues ();
		void printObjectiveValues ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);

		AbstractSet *getRandomSet (int size);
};

#endif
