
#include <iostream>
#include <list>

#include "structure.hpp"


Structure::Structure (Aggregate *firstAgg)
{
	aggregateNumber = 0;
	atomicAggregateNumber = 0;
	firstAggregate = firstAgg;
	aggregateArray = 0;
	atomicAggregateArray = 0;
}


Structure::~Structure ()
{
	for (int num = 0; num < aggregateNumber; num++) { delete aggregateArray[num]; }
	delete [] aggregateArray;
	delete [] atomicAggregateArray;
}


void Structure::buildDataStructure()
{
	aggregateNumber = 0;
	atomicAggregateNumber = 0;

	AggregateSet set;
	std::list<Aggregate*> list;

	firstAggregate->structure = this;
	list.push_back(firstAggregate);
	while (!list.empty())
	{
		Aggregate *currentAggregate = list.front();
		list.pop_front();

		currentAggregate->num = aggregateNumber++;
		set.push_back(currentAggregate);
		if (currentAggregate->isAtomic) { currentAggregate->atomicNum = atomicAggregateNumber++; }

		for (AggregateSetSet::iterator it1 = currentAggregate->aggregateSetSet->begin(); it1 != currentAggregate->aggregateSetSet->end(); ++it1)
		{
			AggregateSet *aggregateSet = *it1;
			for (AggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
			{
				Aggregate *aggregate = *it2;
				if (aggregate->structure == 0) {
					aggregate->structure = this;
					list.push_back(aggregate);
				}
			}
		}
	}

	aggregateArray = new Aggregate*[aggregateNumber];
	atomicAggregateArray = new Aggregate*[atomicAggregateNumber];
	for (AggregateSet::iterator it = set.begin(); it != set.end(); it++)
	{
		Aggregate *agg = *it;
		aggregateArray[agg->num] = agg;
		if (agg->isAtomic) { atomicAggregateArray[agg->atomicNum] = agg; }
	}

	for (AggregateSet::iterator it = set.begin(); it != set.end(); it++) { aggregateArray[(*it)->num] = *it; }

	initReached();
	firstAggregate->buildDataStructure();
}


void Structure::initReached ()
{
	for (int num = 0; num < aggregateNumber; num++) { aggregateArray[num]->reached = false; }
}


void Structure::print ()
{
	initReached();
	firstAggregate->print();
	std::cout << std::endl;
}


OrderedStructure::OrderedStructure (int s) : Structure (0)
{
	size = s;
	
	int size2 = size*(size+1)/2;
	Aggregate **aggArray = new Aggregate* [size2];
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
		{
			int index = -1;
			if (j == 0) { index = i; }
			aggArray[getIndex(i,j)] = new Aggregate (index);
		}

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
			for (int c = 0; c < j; c++)
			{
				AggregateSet *aggSet = new AggregateSet ();
				aggSet->push_back(aggArray[getIndex(i,c)]);
				aggSet->push_back(aggArray[getIndex(i+c+1,j-c-1)]);
				aggArray[getIndex(i,j)]->addAggregateSet(aggSet);
			}

	firstAggregate = aggArray[getIndex(0,size-1)];
	delete [] aggArray;
}


int OrderedStructure::getIndex (int i, int j) { return j*size-j*(j-1)/2+i; }


HierarchicalStructure::HierarchicalStructure (int d) : Structure (0)
{
	depth = d;
	if (d == 0) { firstAggregate = new Aggregate (0); }
	else {
		firstAggregate = new Aggregate ();
		buildHierarchy(firstAggregate,d-1,0);
	}
}


int HierarchicalStructure::buildHierarchy (Aggregate *agg, int d, int i)
{
	Aggregate *agg1;
	Aggregate *agg2;

	if (d == 0)
	{
		agg1 = new Aggregate (i++);
		agg2 = new Aggregate (i++);		
	}

	else {
		agg1 = new Aggregate ();
		agg2 = new Aggregate ();	
	}

	AggregateSet *aggSet = new AggregateSet ();
	aggSet->push_back(agg1);
	aggSet->push_back(agg2);
	agg->addAggregateSet(aggSet);

	if (d > 0)
	{
		i = buildHierarchy(agg1,d-1,i);
		i = buildHierarchy(agg2,d-1,i);
	}
	
	return i;
}


Aggregate::Aggregate (int index)
{
	structure = 0;
	count = 0;

	num = -1;
	atomicNum = -1;
	isAtomic = false;
	reached = false;

	aggregateSetSet = new AggregateSetSet();
	indexSet = new IndexSet();
	
	if (index != -1) { indexSet->push_back(index); }
	isAtomic = (index != -1);
}


Aggregate::~Aggregate ()
{
	for (AggregateSetSet::iterator it = aggregateSetSet->begin(); it != aggregateSetSet->end(); ++it) { delete *it; }
	delete aggregateSetSet;
	delete indexSet;
}


void Aggregate::print ()
{
	printIndexSet(true);
	for (AggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		AggregateSet *aggregateSet = *it1;

		bool first = true;
		std::cout << " -> ";

		for (AggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate *aggregate = *it2;
			if (!first) { std::cout << " "; } else { first = false; }
			aggregate->printIndexSet();
		}
		std::cout << std::endl;
	}
	
	for (AggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		AggregateSet *aggregateSet = *it1;
		for (AggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->print();
			}
		}
	}	
}


void Aggregate::printIndexSet (bool endl)
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


void Aggregate::addAggregateSet (AggregateSet *aggregateSet)
{
	aggregateSetSet->push_back(aggregateSet);
}


void Aggregate::buildDataStructure ()
{
	for (AggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		AggregateSet *aggregateSet = *it1;
		for (AggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->buildDataStructure();
			}
		}
	}

	if (!isAtomic)
	{
		AggregateSet *set = *aggregateSetSet->begin();
		for (AggregateSet::iterator it1 = set->begin(); it1 != set->end(); ++it1)
		{
			Aggregate *aggregate = *it1;
			for (IndexSet::iterator it2 = aggregate->indexSet->begin(); it2 != aggregate->indexSet->end(); ++it2)
			{
				indexSet->push_back(*it2);
			}
		}
	}
}
