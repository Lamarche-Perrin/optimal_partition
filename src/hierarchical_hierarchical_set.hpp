#ifndef INCLUDE_HIERARCHICAL_HIERARCHICAL_SET
#define INCLUDE_HIERARCHICAL_HIERARCHICAL_SET

#include "constrained_set.hpp"
#include "hierarchical_set.hpp"

class HHNode;
typedef std::set<HHNode*> HHNodeSet;

class HierarchicalHierarchicalSet: public ConstrainedSet
{
	public:
		HNode *hierarchy1;
		HNode *hierarchy2;
		HHNode *hyperarchy;
		
		HierarchicalHierarchicalSet (HNode *hierarchy1, HNode *hierarchy2);
		virtual ~HierarchicalHierarchicalSet ();
		
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


class HHNode
{
	public:
		HNode *node1;
		HNode *node2;

		HHNodeSet *children1;
		HHNodeSet *children2;
		
		Measure *measure;
		Quality *quality;

		double optimalValue;
		int optimalCut;

		HHNode (HNode *node1, HNode *node2);
		virtual ~HHNode ();

		void addChild1 (HHNode *node);
		void addChild2 (HHNode *node);
		void setMeasure (Measure *m);
		void print ();
		void printIndices (bool endl = false);
		
		void buildDataStructure (); // SUPPRESS ?
		void computeQuality ();
		void normalizeQuality (Quality *maxQual = 0);
		void printQuality ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);		
		void buildOptimalPartition (Partition *partition);
};

#endif
