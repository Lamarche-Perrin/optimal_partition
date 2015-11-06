#ifndef INCLUDE_HIERARCHICAL_HIERARCHICAL_SET
#define INCLUDE_HIERARCHICAL_HIERARCHICAL_SET

#include "abstract_set.hpp"
#include "hierarchical_set.hpp"

class HHNode;
typedef std::set<HHNode*> HHNodeSet;

class HierarchicalHierarchicalSet: public AbstractSet
{
	public:
		HNode *hierarchy1;
		HNode *hierarchy2;
		HHNode *hyperarchy;
		
		HierarchicalHierarchicalSet (HNode *hierarchy1, HNode *hierarchy2);
		virtual ~HierarchicalHierarchicalSet ();
		
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


class HHNode
{
	public:
		HNode *node1;
		HNode *node2;

		HHNodeSet *children1;
		HHNodeSet *children2;
		
		ObjectiveFunction *objective;
		ObjectiveValue *value;

		double optimalValue;
		int optimalCut;

		HHNode (HNode *node1, HNode *node2);
		virtual ~HHNode ();

		void addChild1 (HHNode *node);
		void addChild2 (HHNode *node);
		void setObjectiveFunction (ObjectiveFunction *m);
		void print ();
		void printIndices (bool endl = false);
		
		void buildDataStructure (); // SUPPRESS ?
		void computeObjectiveValues ();
		void normalizeObjectiveValues (ObjectiveValue *maxQual = 0);
		void printObjectiveValues ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);		
		void buildOptimalPartition (Partition *partition);
};

#endif
