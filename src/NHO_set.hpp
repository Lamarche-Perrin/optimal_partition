#ifndef INCLUDE_NHO_SET
#define INCLUDE_NHO_SET

#include "constrained_set.hpp"

class NHONode;
typedef std::set<NHONode*> NHONodeSet;

class NHOSet: public ConstrainedSet
{
public:
	int NSize;
	HNode *HHierarchy;
	int OSize;

	NHODatatree *NHODatatree;
		
	NHOSet (int NSize, HNode *HHierarchy, int OSize);
	virtual ~NHOSet ();
		
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


class NHONode
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
