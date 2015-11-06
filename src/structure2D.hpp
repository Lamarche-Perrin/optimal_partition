#ifndef INCLUDE_STRUCTURE2D
#define INCLUDE_STRUCTURE2D

#include <list>

#include "structure.hpp"
#include "constrained_set.hpp"


class Aggregate2D;
typedef std::list<Aggregate2D*> Aggregate2DSet;
typedef std::list<Aggregate2DSet*> Aggregate2DSetSet;

class Structure2D: public ConstrainedSet
{
public:
	Structure *structure1;
	Structure *structure2;

	int aggregateNumber;
	int atomicAggregateNumber;
	
	Aggregate2D *firstAggregate;
	Aggregate2D **aggregateArray;
	
	Structure2D (Structure *structure1, Structure *structure2);
	virtual ~Structure2D ();

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


class Aggregate2D
{
public:
	Aggregate *aggregate1;
	Aggregate *aggregate2;

	int num;
	bool isAtomic;
	bool reached;
	
	Aggregate2DSetSet *aggregateSetSet;
	Structure2D *structure;

	Measure *measure;
	Quality *quality;
	double optimalValue;
	Aggregate2DSet *optimalCut;

	Aggregate2D (Aggregate *aggregate1, Aggregate *aggregate2);
	~Aggregate2D ();

	void print ();
	void printIndexSet (bool endl = false);
	void addAggregateSet (Aggregate2DSet *aggregateSet);
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
