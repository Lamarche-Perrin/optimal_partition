#ifndef INCLUDE_STRUCTURE
#define INCLUDE_STRUCTURE

#include <list>

class UniSubset;
typedef std::list<UniSubset*> UniSubsetSet;
typedef std::list<UniSubsetSet*> UniSubsetSetSet;
typedef std::list<int> IndexSet;


class UniSet
{
public:
	int uniSubsetNumber;
	int atomicUniSubsetNumber;

	UniSubset *firstUniSubset;
	UniSubset **uniSubsetArray;
	UniSubset **atomicUniSubsetArray;

	UniSet (UniSubset *firstUniSubset);
	~UniSet ();

	void buildDataStructure ();
	void initReached ();
	void print ();
};


class OrderedUniSet: public UniSet
{
public:
	int size;
	OrderedUniSet (int size);
	int getIndex (int i, int j);
};


class HierarchicalUniSet: public UniSet
{
public:
	int depth;
	HierarchicalUniSet (int depth);
	int buildHierarchy (UniSubset *uniSubset, int depth, int index);
};


class UniSubset
{
public:
	int num;
	int atomicNum;
	bool isAtomic;
	bool reached;
	int count;

	IndexSet *indexSet;
	UniSubsetSetSet *uniSubsetSetSet;
	UniSet *uniSet;

	UniSubset (int index = -1);
	~UniSubset ();

	void print ();
	void printIndexSet (bool endl = false);
	void addUniSubsetSet (UniSubsetSet *uniSubsetSet);

	void buildDataStructure ();
};


#endif
