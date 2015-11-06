#ifndef INCLUDE_MULTI_SET
#define INCLUDE_MULTI_SET

#include <list>

#include "uni_set.hpp"
#include "abstract_set.hpp"


class MultiSubset;
typedef std::list<MultiSubset*> MultiSubsetSet;
typedef std::list<MultiSubsetSet*> MultiSubsetSetSet;

class MultiSet: public AbstractSet
{
public:
	int dimension;
	UniSet **uniSetArray;

	int multiSubsetNumber;
	int atomicMultiSubsetNumber;
	
	MultiSubset *firstMultiSubset;
	MultiSubset **multiSubsetArray;
	MultiSubset **atomicMultiSubsetArray;
	
	MultiSet (UniSet *uniSet);
	MultiSet (UniSet **uniSetArray, int dimension);
	virtual ~MultiSet ();

	int getNum (int *multiNum);
	int *getMultiNum (int num);
	MultiSubset *getAtomicMultiSubset (int index);
	
	void initReached ();
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


class MultiSubset
{
public:
	int dimension;
	UniSubset **uniSubsetArray;

	int num;
	int atomicNum;
	
	bool isAtomic;
	bool reached;
	
	MultiSubsetSetSet *multiSubsetSetSet;
	MultiSet *multiSet;

	ObjectiveFunction *objective;
	ObjectiveValue *value;
	double optimalValue;
	MultiSubsetSet *optimalCut;

	MultiSubset (UniSubset **uniSubsetArray, int dimension);
	~MultiSubset ();

	void print ();
	void printIndexSet (bool endl = false);
	void addMultiSubsetSet (MultiSubsetSet *multiSubsetSet);
	void setObjectiveFunction (ObjectiveFunction *m);

	void buildDataStructure ();
	void computeObjectiveValues ();
	void normalizeObjectiveValues (ObjectiveValue *maxQual = 0);
	void printObjectiveValues ();

	void computeOptimalPartition (double parameter);
	void printOptimalPartition (double parameter);
	void buildOptimalPartition (Partition *partition);
};

#endif
