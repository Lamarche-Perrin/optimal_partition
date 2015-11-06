#ifndef INCLUDE_NHO_SET
#define INCLUDE_NHO_SET

#include "abstract_set.hpp"

class NHONode;
typedef std::set<NHONode*> NHONodeSet;

class NHOSet: public AbstractSet
{
public:
	int NSize;
	HNode *HHierarchy;
	int OSize;

	NHODatatree *NHODatatree;
		
	NHOSet (int NSize, HNode *HHierarchy, int OSize);
	virtual ~NHOSet ();
		
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


class NHONode
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
