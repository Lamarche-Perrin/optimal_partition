#ifndef INCLUDE_HYPER_STRUCTURE
#define INCLUDE_HYPER_STRUCTURE

#include <list>

#include "structure.hpp"
#include "constrained_set.hpp"


class HyperAggregate;
typedef std::list<HyperAggregate*> HyperAggregateSet;
typedef std::list<HyperAggregateSet*> HyperAggregateSetSet;

class HyperStructure: public ConstrainedSet
{
public:
	int dimension;
	Structure **structureArray;

	int aggregateNumber;
	int atomicAggregateNumber;
	
	HyperAggregate *firstAggregate;
	HyperAggregate **aggregateArray;
	HyperAggregate **atomicAggregateArray;
	
	HyperStructure (Structure *structure);
	HyperStructure (Structure **structureArray, int dimension);
	virtual ~HyperStructure ();

	int getNum (int *hyperNum);
	int *getHyperNum (int num);
	HyperAggregate *getAtomicAggregate (int index);
	
	void initReached ();
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


class HyperAggregate
{
public:
	int dimension;
	Aggregate **aggregateArray;

	int num;
	int atomicNum;
	
	bool isAtomic;
	bool reached;
	
	HyperAggregateSetSet *aggregateSetSet;
	HyperStructure *structure;

	Measure *measure;
	Quality *quality;
	double optimalValue;
	HyperAggregateSet *optimalCut;

	HyperAggregate (Aggregate **aggregateArray, int dimension);
	~HyperAggregate ();

	void print ();
	void printIndexSet (bool endl = false);
	void addAggregateSet (HyperAggregateSet *aggregateSet);
	void setMeasure (Measure *m);

	void buildDataStructure ();
	void computeQuality ();
	void normalizeQuality (Quality *maxQual = 0);
	void printQuality ();

	void computeOptimalPartition (double parameter);
	void printOptimalPartition (double parameter);
	void buildOptimalPartition (Partition *partition);
};

#endif
