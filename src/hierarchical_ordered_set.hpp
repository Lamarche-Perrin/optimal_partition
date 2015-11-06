#ifndef INCLUDE_HIERARCHICAL_ORDERED_SET
#define INCLUDE_HIERARCHICAL_ORDERED_SET

#include "constrained_set.hpp"

class HONode;
typedef std::set<HONode*> HONodeSet;

class HierarchicalOrderedSet: public ConstrainedSet
{
	public:
		int size;
		HONode *hierarchy;
		
		HierarchicalOrderedSet (HONode *hierarchy, int size);
		virtual ~HierarchicalOrderedSet ();
		
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
};


class HONode
{
	public:
		int index;
		int level;
		std::set<int> *indices;

		HONodeSet *children;
		Measure *measure;

		int size;
		Quality **qualities;
		double *optimalValues;
		int *optimalCuts;
	
		HONode (int size, int index = -1);
		virtual ~HONode ();

		int getIndex (int i, int j);

		void addChild (HONode *node);
		void setMeasure (Measure *m);
		void print ();
		
		void buildDataStructure (int level = 0);
		void computeQuality ();
		void normalizeQuality (Quality *maxQual = 0);
		void printQuality ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);
		
		void buildOptimalPartition (Partition *partition, int pi = 0, int pj = -1);
};

#endif
