#ifndef INCLUDE_HYPER_STRUCTURE
#define INCLUDE_HYPER_STRUCTURE

#include <list>

#include "structure.hpp"
#include "abstract_set.hpp"


class HyperAggregate;
typedef std::list<HyperAggregate*> HyperAggregateSet;
typedef std::list<HyperAggregateSet*> HyperAggregateSetSet;

class HyperStructure: public AbstractSet
{
public:
	int dimension;
	UniSet **structureArray;

	int aggregateNumber;
	int atomicAggregateNumber;
	
	HyperAggregate *firstAggregate;
	HyperAggregate **aggregateArray;
	HyperAggregate **atomicAggregateArray;
	
	HyperStructure (UniSet *structure);
	HyperStructure (UniSet **structureArray, int dimension);
	virtual ~HyperStructure ();

	int getNum (int *hyperNum);
	int *getHyperNum (int num);
	HyperAggregate *getAtomicAggregate (int index);
	
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


class HyperAggregate
{
public:
	int dimension;
	UniSubset **aggregateArray;

	int num;
	int atomicNum;
	
	bool isAtomic;
	bool reached;
	
	HyperAggregateSetSet *aggregateSetSet;
	HyperStructure *structure;

	ObjectiveFunction *objective;
	ObjectiveValue *value;
	double optimalValue;
	HyperAggregateSet *optimalCut;

	HyperAggregate (UniSubset **aggregateArray, int dimension);
	~HyperAggregate ();

	void print ();
	void printIndexSet (bool endl = false);
	void addAggregateSet (HyperAggregateSet *aggregateSet);
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
