
#include <iostream>
#include <list>

#include "structure.hpp"


UniSet::UniSet (UniSubset *firstAgg)
{
	uniSubsetNumber = 0;
	atomicUniSubsetNumber = 0;
	firstUniSubset = firstAgg;
	uniSubsetArray = 0;
	atomicUniSubsetArray = 0;
}


UniSet::~UniSet ()
{
	for (int num = 0; num < uniSubsetNumber; num++) { delete uniSubsetArray[num]; }
	delete [] uniSubsetArray;
	delete [] atomicUniSubsetArray;
}


void UniSet::buildDataStructure()
{
	uniSubsetNumber = 0;
	atomicUniSubsetNumber = 0;

	UniSubsetSet set;
	std::list<UniSubset*> list;

	firstUniSubset->uniSet = this;
	list.push_back(firstUniSubset);
	while (!list.empty())
	{
		UniSubset *currentUniSubset = list.front();
		list.pop_front();

		currentUniSubset->num = uniSubsetNumber++;
		set.push_back(currentUniSubset);
		if (currentUniSubset->isAtomic) { currentUniSubset->atomicNum = atomicUniSubsetNumber++; }

		for (UniSubsetSetSet::iterator it1 = currentUniSubset->uniSubsetSetSet->begin(); it1 != currentUniSubset->uniSubsetSetSet->end(); ++it1)
		{
			UniSubsetSet *uniSubsetSet = *it1;
			for (UniSubsetSet::iterator it2 = uniSubsetSet->begin(); it2 != uniSubsetSet->end(); ++it2)
			{
				UniSubset *uniSubset = *it2;
				if (uniSubset->uniSet == 0) {
					uniSubset->uniSet = this;
					list.push_back(uniSubset);
				}
			}
		}
	}

	uniSubsetArray = new UniSubset*[uniSubsetNumber];
	atomicUniSubsetArray = new UniSubset*[atomicUniSubsetNumber];
	for (UniSubsetSet::iterator it = set.begin(); it != set.end(); it++)
	{
		UniSubset *agg = *it;
		uniSubsetArray[agg->num] = agg;
		if (agg->isAtomic) { atomicUniSubsetArray[agg->atomicNum] = agg; }
	}

	for (UniSubsetSet::iterator it = set.begin(); it != set.end(); it++) { uniSubsetArray[(*it)->num] = *it; }

	initReached();
	firstUniSubset->buildDataStructure();
}


void UniSet::initReached ()
{
	for (int num = 0; num < uniSubsetNumber; num++) { uniSubsetArray[num]->reached = false; }
}


void UniSet::print ()
{
	initReached();
	firstUniSubset->print();
	std::cout << std::endl;
}


OrderedUniSet::OrderedUniSet (int s) : UniSet (0)
{
	size = s;
	
	int size2 = size*(size+1)/2;
	UniSubset **aggArray = new UniSubset* [size2];
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
		{
			int index = -1;
			if (j == 0) { index = i; }
			aggArray[getIndex(i,j)] = new UniSubset (index);
		}

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
			for (int c = 0; c < j; c++)
			{
				UniSubsetSet *aggSet = new UniSubsetSet ();
				aggSet->push_back(aggArray[getIndex(i,c)]);
				aggSet->push_back(aggArray[getIndex(i+c+1,j-c-1)]);
				aggArray[getIndex(i,j)]->addUniSubsetSet(aggSet);
			}

	firstUniSubset = aggArray[getIndex(0,size-1)];
	delete [] aggArray;
}


int OrderedUniSet::getIndex (int i, int j) { return j*size-j*(j-1)/2+i; }


HierarchicalUniSet::HierarchicalUniSet (int d) : UniSet (0)
{
	depth = d;
	if (d == 0) { firstUniSubset = new UniSubset (0); }
	else {
		firstUniSubset = new UniSubset ();
		buildHierarchy(firstUniSubset,d-1,0);
	}
}


int HierarchicalUniSet::buildHierarchy (UniSubset *agg, int d, int i)
{
	UniSubset *agg1;
	UniSubset *agg2;

	if (d == 0)
	{
		agg1 = new UniSubset (i++);
		agg2 = new UniSubset (i++);		
	}

	else {
		agg1 = new UniSubset ();
		agg2 = new UniSubset ();	
	}

	UniSubsetSet *aggSet = new UniSubsetSet ();
	aggSet->push_back(agg1);
	aggSet->push_back(agg2);
	agg->addUniSubsetSet(aggSet);

	if (d > 0)
	{
		i = buildHierarchy(agg1,d-1,i);
		i = buildHierarchy(agg2,d-1,i);
	}
	
	return i;
}


UniSubset::UniSubset (int index)
{
	uniSet = 0;
	count = 0;

	num = -1;
	atomicNum = -1;
	isAtomic = false;
	reached = false;

	uniSubsetSetSet = new UniSubsetSetSet();
	indexSet = new IndexSet();
	
	if (index != -1) { indexSet->push_back(index); }
	isAtomic = (index != -1);
}


UniSubset::~UniSubset ()
{
	for (UniSubsetSetSet::iterator it = uniSubsetSetSet->begin(); it != uniSubsetSetSet->end(); ++it) { delete *it; }
	delete uniSubsetSetSet;
	delete indexSet;
}


void UniSubset::print ()
{
	printIndexSet(true);
	for (UniSubsetSetSet::iterator it1 = uniSubsetSetSet->begin(); it1 != uniSubsetSetSet->end(); ++it1)
	{
		UniSubsetSet *uniSubsetSet = *it1;

		bool first = true;
		std::cout << " -> ";

		for (UniSubsetSet::iterator it2 = uniSubsetSet->begin(); it2 != uniSubsetSet->end(); ++it2)
		{
			UniSubset *uniSubset = *it2;
			if (!first) { std::cout << " "; } else { first = false; }
			uniSubset->printIndexSet();
		}
		std::cout << std::endl;
	}
	
	for (UniSubsetSetSet::iterator it1 = uniSubsetSetSet->begin(); it1 != uniSubsetSetSet->end(); ++it1)
	{
		UniSubsetSet *uniSubsetSet = *it1;
		for (UniSubsetSet::iterator it2 = uniSubsetSet->begin(); it2 != uniSubsetSet->end(); ++it2)
		{
			UniSubset *uniSubset = *it2;
			if (!uniSubset->reached)
			{
				uniSubset->reached = true;
				uniSubset->print();
			}
		}
	}	
}


void UniSubset::printIndexSet (bool endl)
{
	bool first = true;
	std::cout << "{";
	for (IndexSet::iterator it = indexSet->begin(); it != indexSet->end(); it++)
	{
		if (!first) { std::cout << ","; } else { first = false; }
		std::cout << (*it);
	}
	std::cout << "}";	
	if (endl) { std::cout << std::endl; }
}


void UniSubset::addUniSubsetSet (UniSubsetSet *uniSubsetSet)
{
	uniSubsetSetSet->push_back(uniSubsetSet);
}


void UniSubset::buildDataStructure ()
{
	for (UniSubsetSetSet::iterator it1 = uniSubsetSetSet->begin(); it1 != uniSubsetSetSet->end(); ++it1)
	{
		UniSubsetSet *uniSubsetSet = *it1;
		for (UniSubsetSet::iterator it2 = uniSubsetSet->begin(); it2 != uniSubsetSet->end(); ++it2)
		{
			UniSubset *uniSubset = *it2;
			if (!uniSubset->reached)
			{
				uniSubset->reached = true;
				uniSubset->buildDataStructure();
			}
		}
	}

	if (!isAtomic)
	{
		UniSubsetSet *set = *uniSubsetSetSet->begin();
		for (UniSubsetSet::iterator it1 = set->begin(); it1 != set->end(); ++it1)
		{
			UniSubset *uniSubset = *it1;
			for (IndexSet::iterator it2 = uniSubset->indexSet->begin(); it2 != uniSubset->indexSet->end(); ++it2)
			{
				indexSet->push_back(*it2);
			}
		}
	}
}
