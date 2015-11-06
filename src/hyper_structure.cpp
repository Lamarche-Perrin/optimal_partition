
#include <iostream>

#include "hyper_structure.hpp"


HyperStructure::HyperStructure (Structure *str)
{
	dimension = 1;
	structureArray = new Structure* [1];
	structureArray[0] = str;

	aggregateNumber = 0;
	atomicAggregateNumber = 0;
	
	firstAggregate = 0;
	aggregateArray = 0;
	atomicAggregateArray = 0;
}


HyperStructure::HyperStructure (Structure **strArray, int dim)
{
	dimension = dim;
	structureArray = strArray;

	aggregateNumber = 0;
	atomicAggregateNumber = 0;
	
	firstAggregate = 0;
	aggregateArray = 0;
	atomicAggregateArray = 0;
}


HyperStructure::~HyperStructure ()
{
	for (int num = 0; num < aggregateNumber; num++) { delete aggregateArray[num]; }
	delete [] aggregateArray;
	delete [] atomicAggregateArray;
}


int HyperStructure::getNum (int *hyperNum)
{
	int num = 0;
	for (int d = dimension-1; d >= 0; d--)
	{
		if (d < dimension-1) { num *= structureArray[d]->aggregateNumber; }
		num += hyperNum[d];
	}
	return num;
}


int *HyperStructure::getHyperNum (int num)
{
	int *hyperNum = new int [dimension];
	for (int d = 0; d < dimension; d++)
	{
		hyperNum[d] = num % structureArray[d]->aggregateNumber;
		num -= hyperNum[d];
		num /= structureArray[d]->aggregateNumber;
	}
	return hyperNum;
}


HyperAggregate *HyperStructure::getAtomicAggregate (int index)
{
	if (dimension > 1)
	{
		std::cout << "ERROR: use of getAtomicAggregate on a multidimensional structure!" << std::endl;
		return 0;
	}
	
	for (int num = 0; num < atomicAggregateNumber; num++)
	{
		Aggregate *agg = atomicAggregateArray[num]->aggregateArray[0];
		if (agg->isAtomic && index == agg->indexSet->front()) { return atomicAggregateArray[num]; }
	}
	return 0;
}


void HyperStructure::initReached()
{
	for (int num = 0; num < aggregateNumber; num++) { aggregateArray[num]->reached = false; }
}


void HyperStructure::setRandom () {}


void HyperStructure::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	initReached();
	firstAggregate->setObjectiveFunction(m);
}


void HyperStructure::print ()
{
	initReached();
	firstAggregate->print();
	std::cout << std::endl;
}


void HyperStructure::buildDataStructure ()
{
	firstAggregate = 0;
	aggregateNumber = 1;
	atomicAggregateNumber = 1;

	for (int d = 0; d < dimension; d++)
	{
		aggregateNumber *= structureArray[d]->aggregateNumber;
		atomicAggregateNumber *= structureArray[d]->atomicAggregateNumber;
	}
	
	aggregateArray = new HyperAggregate* [aggregateNumber];
	atomicAggregateArray = new HyperAggregate* [atomicAggregateNumber];
	
	int atomicNum = 0;
	for (int num = 0; num < aggregateNumber; num++)
	{
		int *hyperNum = getHyperNum(num);
		Aggregate **aggArray = new Aggregate* [dimension];
		for (int d = 0; d < dimension; d++) { aggArray[d] = structureArray[d]->aggregateArray[hyperNum[d]]; }

		HyperAggregate *aggregate = new HyperAggregate (aggArray,dimension);
		aggregateArray[num] = aggregate;

		aggregate->structure = this;
		aggregate->num = num;
		aggregate->isAtomic = true;
		for (int d = 0; d < dimension && aggregate->isAtomic; d++) { aggregate->isAtomic = structureArray[d]->aggregateArray[hyperNum[d]]->isAtomic; }
		if (aggregate->isAtomic)
		{
			aggregate->atomicNum = atomicNum++;
			atomicAggregateArray[aggregate->atomicNum] = aggregate;
		}

		if (firstAggregate == 0)
		{
			bool isFirstAggregate = true;
			for (int d = 0; d < dimension && isFirstAggregate; d++) { isFirstAggregate = (aggArray[d] == structureArray[d]->firstAggregate); }
			if (isFirstAggregate) { firstAggregate = aggregate; }
		}

		delete [] hyperNum;
	}

	initReached();
	firstAggregate->buildDataStructure();
}


void HyperStructure::computeObjectiveValues ()
{
	initReached();
	objective->computeObjectiveValues();
	firstAggregate->computeObjectiveValues();
}


void HyperStructure::normalizeObjectiveValues ()
{
	initReached();
	firstAggregate->normalizeObjectiveValues();
}


void HyperStructure::printObjectiveValues ()
{
	objective->printObjectiveValues();
	initReached();
	firstAggregate->printObjectiveValues();
}


void HyperStructure::computeOptimalPartition (double parameter)
{
	initReached();
	firstAggregate->computeOptimalPartition(parameter);
}


void HyperStructure::printOptimalPartition (double parameter)
{
	firstAggregate->printOptimalPartition(parameter);
}


Partition *HyperStructure::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	firstAggregate->buildOptimalPartition(partition);	
	return partition;
}



HyperAggregate::HyperAggregate (Aggregate **aggArray, int dim)
{
	structure = 0;
	
	num = -1;
	atomicNum = -1;
	isAtomic = false;
	reached = false;

	dimension = dim;
	aggregateArray = aggArray;
	aggregateSetSet = new HyperAggregateSetSet();

	value = 0;
	optimalCut = 0;
}


HyperAggregate::~HyperAggregate ()
{
	delete [] aggregateArray;
	for (HyperAggregateSetSet::iterator it = aggregateSetSet->begin(); it != aggregateSetSet->end(); ++it) { delete *it; }
	delete aggregateSetSet;
	delete value;
}



void HyperAggregate::addAggregateSet (HyperAggregateSet *aggregateSet)
{
	aggregateSetSet->push_back(aggregateSet);
}


void HyperAggregate::setObjectiveFunction (ObjectiveFunction *m)
{
	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->setObjectiveFunction(m);
			}
		}
	}

	objective = m;
	if (isAtomic)
	{
		int index = 0;
		for (int d = 0; d < dimension; d++)
		{
			index += *aggregateArray[d]->indexSet->begin();
			if (d+1 < dimension) { index *= aggregateArray[d]->structure->atomicAggregateNumber; }
		}
		value = m->newObjectiveValue(index);
	}
	else { value = m->newObjectiveValue(); }
}


void HyperAggregate::print ()
{
	std::cout << "[" << num << "] ";
	printIndexSet(true);
	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;

		bool first = true;
		std::cout << " -> ";
		
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!first) { std::cout << " "; } else { first = false; }
			aggregate->printIndexSet();
		}
		std::cout << std::endl;
	}

	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->print();
			}
		}
	}
}


void HyperAggregate::printIndexSet (bool endl)
{
	std::cout << "(";
	for (int d = 0; d < dimension; d++)
	{
		aggregateArray[d]->printIndexSet();
		if (d+1 < dimension) { std::cout << ","; }
	}
	std::cout << ")";
	if (endl) { std::cout << std::endl; }
}

		
void HyperAggregate::buildDataStructure ()
{
	int *hyperNum = new int [dimension];
	for (int d = 0; d < dimension; d++) { hyperNum[d] = aggregateArray[d]->num; }

	for (int d = 0; d < dimension; d++)
	{
		for (AggregateSetSet::iterator it1 = aggregateArray[d]->aggregateSetSet->begin(); it1 != aggregateArray[d]->aggregateSetSet->end(); it1++)
		{
			AggregateSet *currentSet = *it1;
			HyperAggregateSet *newSet = new HyperAggregateSet();

			for (AggregateSet::iterator it2 = currentSet->begin(); it2 != currentSet->end(); it2++)
			{
				Aggregate *subAggregate = *it2;
				hyperNum[d] = subAggregate->num;
				newSet->push_back(structure->aggregateArray[structure->getNum(hyperNum)]);
			}

			addAggregateSet(newSet);
		}
		hyperNum[d] = aggregateArray[d]->num;
	}
	delete [] hyperNum;

	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->buildDataStructure();
			}
		}
	}
}


void HyperAggregate::computeObjectiveValues ()
{
	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->computeObjectiveValues();
			}
		}
	}

	if (isAtomic) { value->compute(); }
	else {
		ObjectiveValueSet *qSet = new ObjectiveValueSet();
		HyperAggregateSet *aggregateSet = *aggregateSetSet->begin();
		for (HyperAggregateSet::iterator it = aggregateSet->begin(); it != aggregateSet->end(); ++it) { qSet->insert((*it)->value); }
		value->compute(qSet);
		delete qSet;
	}
}


void HyperAggregate::normalizeObjectiveValues (ObjectiveValue *maxQual)
{
	if (maxQual == 0) { maxQual = value; }

	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->normalizeObjectiveValues(maxQual);
			}
		}
	}

	value->normalize(maxQual);
}


void HyperAggregate::printObjectiveValues ()
{
	printIndexSet();
	std::cout << " -> ";
	value->print(true);
	
	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->printObjectiveValues();
			}
		}
	}
}


void HyperAggregate::computeOptimalPartition (double parameter)
{
	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			HyperAggregate *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->computeOptimalPartition(parameter);
			}
		}
	}

	optimalValue = value->getValue(parameter);
	optimalCut = 0;

	for (HyperAggregateSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		HyperAggregateSet *aggregateSet = *it1;

		double value = 0;
		for (HyperAggregateSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2) { value += (*it2)->optimalValue; }

		if ((objective->maximize && value > optimalValue) || (!objective->maximize && value < optimalValue))
		{
			optimalValue = value;
			optimalCut = aggregateSet;
		}
	}
}


void HyperAggregate::printOptimalPartition (double parameter) {}


void HyperAggregate::buildOptimalPartition (Partition *partition)
{
	if (optimalCut == 0)
	{
		Part **partArray = new Part* [dimension];
		for (int d = 0; d < dimension; d++)
		{
			Part *p = new Part();
			for (IndexSet::iterator it = aggregateArray[d]->indexSet->begin(); it != aggregateArray[d]->indexSet->end(); ++it) { p->addIndividual(*it); }
			partArray[d] = p;
		}

		HyperPart *part = new HyperPart (partArray,dimension,value);
		partition->addPart(part,true);
	}
	else { for (HyperAggregateSet::iterator it = optimalCut->begin(); it != optimalCut->end(); it++) { (*it)->buildOptimalPartition(partition); } }
}
