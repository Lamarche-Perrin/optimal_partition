#ifndef INCLUDE_STRUCTURE2D
#define INCLUDE_STRUCTURE2D

#include <list>

#include "structure.hpp"
#include "abstract_set.hpp"


class Aggregate2D;
typedef std::list<Aggregate2D*> Aggregate2DSet;
typedef std::list<Aggregate2DSet*> Aggregate2DSetSet;

class Structure2D: public AbstractSet
{
public:
	UniSet *uniSet1;
	UniSet *uniSet2;

	int aggregateNumber;
	int atomicAggregateNumber;
	
	Aggregate2D *firstAggregate;
	Aggregate2D **aggregateArray;
	
	Structure2D (UniSet *uniSet1, UniSet *uniSet2);
	virtual ~Structure2D ();

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


class Aggregate2D
{
public:
	UniSubset *uniSubset1;
	UniSubset *uniSubset2;

	int num;
	bool isAtomic;
	bool reached;
	
	Aggregate2DSetSet *aggregateSetSet;
	Structure2D *structure;

	ObjectiveFunction *objective;
	ObjectiveValue *value;
	double optimalValue;
	Aggregate2DSet *optimalCut;

	Aggregate2D (UniSubset *uniSubset1, UniSubset *uniSubset2);
	~Aggregate2D ();

	void print ();
	void printIndexSet (bool endl = false);
	void addAggregateSet (Aggregate2DSet *aggregateSet);
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
