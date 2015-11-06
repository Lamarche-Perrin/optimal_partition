#ifndef INCLUDE_NONCONSTRAINED_SET
#define INCLUDE_NONCONSTRAINED_SET

#include "constrained_set.hpp"
#include "datatree.hpp"
#include "partition.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;
extern bool NORMALIZED_MEASURE;
extern double PARAMETER;

class Datatree;
class Partition;

class NonconstrainedSet: public ConstrainedSet
{
	public:
		int size;
		Datatree *dataTree;
		Quality **qualities;	
		
		NonconstrainedSet (int size);
		~NonconstrainedSet ();

		void setRandom ();
		void setMeasure (Measure *m);
		void print ();
	
		void printDataTree (bool verbose = true);
		void printParts ();
		int printPartitions (bool print = true);

		void buildDataStructure ();
		void computeQuality ();
		void normalizeQuality ();
		void printQuality ();
				
		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);		
		Partition *getOptimalPartition (double parameter);

	private:
		Datatree *buildPartsRec (Datatree *node);
		void buildPartitionsRec (Datatree *node);
};



#endif
