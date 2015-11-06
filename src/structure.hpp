#ifndef INCLUDE_STRUCTURE
#define INCLUDE_STRUCTURE

#include <list>

class Aggregate;
typedef std::list<Aggregate*> AggregateSet;
typedef std::list<AggregateSet*> AggregateSetSet;
typedef std::list<int> IndexSet;


class Structure
{
public:
	int aggregateNumber;
	int atomicAggregateNumber;

	Aggregate *firstAggregate;
	Aggregate **aggregateArray;
	Aggregate **atomicAggregateArray;

	Structure (Aggregate *firstAggregate);
	~Structure ();

	void buildDataStructure ();
	void initReached ();
	void print ();
};


class OrderedStructure: public Structure
{
public:
	int size;
	OrderedStructure (int size);
	int getIndex (int i, int j);
};


class HierarchicalStructure: public Structure
{
public:
	int depth;
	HierarchicalStructure (int depth);
	int buildHierarchy (Aggregate *aggregate, int depth, int index);
};


class Aggregate
{
public:
	int num;
	int atomicNum;
	bool isAtomic;
	bool reached;
	int count;

	IndexSet *indexSet;
	AggregateSetSet *aggregateSetSet;
	Structure *structure;

	Aggregate (int index = -1);
	~Aggregate ();

	void print ();
	void printIndexSet (bool endl = false);
	void addAggregateSet (AggregateSet *aggregateSet);

	void buildDataStructure ();
};


#endif
