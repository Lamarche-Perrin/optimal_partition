#ifndef INCLUDE_HIERARCHICAL_ORDERED_SET
#define INCLUDE_HIERARCHICAL_ORDERED_SET

#include "abstract_set.hpp"

class HONode;
typedef std::set<HONode*> HONodeSet;

class HierarchicalOrderedSet: public AbstractSet
{
	public:
		int size;
		HONode *hierarchy;
		
		HierarchicalOrderedSet (HONode *hierarchy, int size);
		virtual ~HierarchicalOrderedSet ();
		
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
};


class HONode
{
	public:
		int index;
		int level;
		std::set<int> *indices;

		HONodeSet *children;
		ObjectiveFunction *objective;

		int size;
		ObjectiveValue **qualities;
		double *optimalValues;
		int *optimalCuts;
	
		HONode (int size, int index = -1);
		virtual ~HONode ();

		int getIndex (int i, int j);

		void addChild (HONode *node);
		void setObjectiveFunction (ObjectiveFunction *m);
		void print ();
		
		void buildDataStructure (int level = 0);
		void computeObjectiveValues ();
		void normalizeObjectiveValues (ObjectiveValue *maxQual = 0);
		void printObjectiveValues ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);
		
		void buildOptimalPartition (Partition *partition, int pi = 0, int pj = -1);
};

#endif
