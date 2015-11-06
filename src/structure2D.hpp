#ifndef INCLUDE_STRUCTURE2D
#define INCLUDE_STRUCTURE2D

#include <list>

#include "structure.hpp"
#include "abstract_set.hpp"


class BiSubset;
typedef std::list<BiSubset*> BiSubsetSet;
typedef std::list<BiSubsetSet*> BiSubsetSetSet;

class BiSet: public AbstractSet
{
public:
	UniSet *uniSet1;
	UniSet *uniSet2;

	int biSubsetNumber;
	int atomicBiSubsetNumber;
	
	BiSubset *firstBiSubset;
	BiSubset **biSubsetArray;
	
	BiSet (UniSet *uniSet1, UniSet *uniSet2);
	virtual ~BiSet ();

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


class BiSubset
{
public:
	UniSubset *uniSubset1;
	UniSubset *uniSubset2;

	int num;
	bool isAtomic;
	bool reached;
	
	BiSubsetSetSet *biSubsetSetSet;
	BiSet *biSet;

	ObjectiveFunction *objective;
	ObjectiveValue *value;
	double optimalValue;
	BiSubsetSet *optimalCut;

	BiSubset (UniSubset *uniSubset1, UniSubset *uniSubset2);
	~BiSubset ();

	void print ();
	void printIndexSet (bool endl = false);
	void addBiSubsetSet (BiSubsetSet *biSubsetSet);
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
