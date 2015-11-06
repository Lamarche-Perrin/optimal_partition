#ifndef INCLUDE_HIERARCHICAL_SET
#define INCLUDE_HIERARCHICAL_SET

#include "constrained_set.hpp"

class HNode;
typedef std::set<HNode*> HNodeSet;

class HierarchicalSet: public ConstrainedSet
{
	public:
		HNode *hierarchy;
			
		HierarchicalSet (HNode *hierarchy);
		virtual ~HierarchicalSet ();
		
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


class HNode
{
	public:
		int index;
		std::set<int> *indices;

		int level;
		int size;
		int width;
		int num;
		
		HNode *root;
		HNodeSet *children;
		Measure *measure;
		Quality *quality;

		double optimalValue;
		bool optimalCut;
	
		HNode (int index = -1);
		virtual ~HNode ();

		void addChild (HNode *node);
		void setMeasure (Measure *m);
		void print ();
		void printIndices (bool endl = false);
		
		void buildDataStructure (HNode *root = 0, int level = 0, int num = 0);
		void computeQuality ();
		void normalizeQuality (Quality *maxQual = 0);
		void printQuality ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);
		
		void buildOptimalPartition (Partition *partition);
};

#endif
