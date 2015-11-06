#ifndef INCLUDE_NONCONSTRAINED_SET
#define INCLUDE_NONCONSTRAINED_SET

#include "abstract_set.hpp"
#include "datatree.hpp"
#include "partition.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;
extern bool NORMALIZED_OBJECTIVE;
extern double PARAMETER;

class Datatree;
class Partition;

class NonconstrainedSet: public AbstractSet
{
	public:
		int size;
		Datatree *dataTree;
		ObjectiveValue **qualities;	
		
		NonconstrainedSet (int size);
		~NonconstrainedSet ();

		void setRandom ();
		void setObjectiveFunction (ObjectiveFunction *m);
		void print ();
	
		void printDataTree (bool verbose = true);
		void printParts ();
		int printPartitions (bool print = true);

		void buildDataStructure ();
		void computeObjectiveValues ();
		void normalizeObjectiveValues ();
		void printObjectiveValues ();
				
		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);		
		Partition *getOptimalPartition (double parameter);

	private:
		Datatree *buildPartsRec (Datatree *node);
		void buildPartitionsRec (Datatree *node);
};



#endif
