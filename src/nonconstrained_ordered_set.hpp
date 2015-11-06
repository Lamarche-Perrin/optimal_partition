#ifndef INCLUDE_NONCONSTRAINED_ORDERED_SET
#define INCLUDE_NONCONSTRAINED_ORDERED_SET

#include "abstract_set.hpp"
#include "datatree.hpp"


class NonconstrainedOrderedSet: public AbstractSet
{
	public:
		int size1;
		int size2;
		OrderedDatatree *dataTree;
		ObjectiveValue **qualities;	
				
		NonconstrainedOrderedSet (int size1, int size2);
		virtual ~NonconstrainedOrderedSet ();

		void setRandom ();
		void setObjectiveFunction (ObjectiveFunction *m);
		void print ();

		void buildDataStructure ();
		void computeObjectiveValues ();
		void normalizeObjectiveValues ();
		void printObjectiveValues ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);

	private:
		OrderedDatatree *buildPartsRec (OrderedDatatree *node);
		void buildPartitionsRec (OrderedDatatree *node);
};

#endif
