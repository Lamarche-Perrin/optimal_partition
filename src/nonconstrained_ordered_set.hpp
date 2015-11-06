#ifndef INCLUDE_NONCONSTRAINED_ORDERED_SET
#define INCLUDE_NONCONSTRAINED_ORDERED_SET

#include "constrained_set.hpp"
#include "datatree.hpp"


class NonconstrainedOrderedSet: public ConstrainedSet
{
	public:
		int size1;
		int size2;
		OrderedDatatree *dataTree;
		Quality **qualities;	
				
		NonconstrainedOrderedSet (int size1, int size2);
		virtual ~NonconstrainedOrderedSet ();

		void setRandom ();
		void setMeasure (Measure *m);
		void print ();

		void buildDataStructure ();
		void computeQuality ();
		void normalizeQuality ();
		void printQuality ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);

	private:
		OrderedDatatree *buildPartsRec (OrderedDatatree *node);
		void buildPartitionsRec (OrderedDatatree *node);
};

#endif
