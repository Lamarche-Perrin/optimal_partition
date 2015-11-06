
#include <iostream>

#include "structure2D.hpp"

		
Structure2D::Structure2D (Structure *structure1, Structure *structure2)
{
	this->structure1 = structure1;
	this->structure2 = structure2;

	aggregateNumber = 0;
	atomicAggregateNumber = 0;
	
	firstAggregate = 0;
	aggregateArray = 0;
}


Structure2D::~Structure2D ()
{
	delete firstAggregate;
	delete [] aggregateArray;
}


void Structure2D::initReached()
{
	for (int num = 0; num < aggregateNumber; num++) { aggregateArray[num]->reached = false; }
}


void Structure2D::setRandom () {}


void Structure2D::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	initReached();
	firstAggregate->setObjectiveFunction(m);
}


void Structure2D::print ()
{
	initReached();
	firstAggregate->print();
	std::cout << std::endl;
}


void Structure2D::buildDataStructure ()
{
	aggregateNumber = structure1->aggregateNumber * structure2->aggregateNumber;
	atomicAggregateNumber = structure1->atomicAggregateNumber * structure2->atomicAggregateNumber;
	aggregateArray = new Aggregate2D *[aggregateNumber];
	
	for (int num1 = 0; num1 < structure1->aggregateNumber; num1++)
	{
		Aggregate *aggregate1 = structure1->aggregateArray[num1];
		
		for (int num2 = 0; num2 < structure2->aggregateNumber; num2++)
		{
			Aggregate *aggregate2 = structure2->aggregateArray[num2];

			Aggregate2D *aggregate = new Aggregate2D (aggregate1,aggregate2);
			aggregate->structure = this;

			aggregate->isAtomic = aggregate1->isAtomic && aggregate2->isAtomic;
			aggregate->num = num1 + num2 * structure1->aggregateNumber;
			aggregateArray[aggregate->num] = aggregate;
		}
	}

	initReached();
	firstAggregate = aggregateArray[structure1->firstAggregate->num + structure2->firstAggregate->num * structure1->aggregateNumber];
	firstAggregate->buildDataStructure();
}


void Structure2D::computeObjectiveValues ()
{
	objective->computeObjectiveValues();
	initReached();
	firstAggregate->computeObjectiveValues();
}


void Structure2D::normalizeObjectiveValues ()
{
	initReached();
	firstAggregate->normalizeObjectiveValues();
}


void Structure2D::printObjectiveValues ()
{
	initReached();
	firstAggregate->printObjectiveValues();
}


void Structure2D::computeOptimalPartition (double parameter)
{
	initReached();
	firstAggregate->computeOptimalPartition(parameter);
}


void Structure2D::printOptimalPartition (double parameter)
{
	firstAggregate->printOptimalPartition(parameter);
}


Partition *Structure2D::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	firstAggregate->buildOptimalPartition(partition);	
	return partition;
}



Aggregate2D::Aggregate2D (Aggregate *agg1, Aggregate *agg2)
{
	structure = 0;
	
	num = -1;
	isAtomic = false;
	reached = false;
	
	aggregate1 = agg1;
	aggregate2 = agg2;
	aggregateSetSet = new Aggregate2DSetSet();

	value = 0;
	optimalCut = 0;
}


Aggregate2D::~Aggregate2D ()
{
	for (Aggregate2DSetSet::iterator it = aggregateSetSet->begin(); it != aggregateSetSet->end(); ++it) { delete *it; }
	delete aggregateSetSet;
	delete value;
}



void Aggregate2D::addAggregateSet (Aggregate2DSet *aggregateSet)
{
	aggregateSetSet->push_back(aggregateSet);
}


void Aggregate2D::setObjectiveFunction (ObjectiveFunction *m)
{
	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
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
		int index1 = *aggregate1->indexSet->begin();
		int index2 = *aggregate2->indexSet->begin();
		value = m->newObjectiveValue(index1 + index2 * aggregate1->structure->atomicAggregateNumber);
	}
	else { value = m->newObjectiveValue(); }
}


void Aggregate2D::print ()
{
	printIndexSet(true);
	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;

		bool first = true;
		std::cout << " -> ";
		
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
			if (!first) { std::cout << " "; } else { first = false; }
			aggregate->printIndexSet();
		}
		std::cout << std::endl;
	}

	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->print();
			}
		}
	}
}


void Aggregate2D::printIndexSet (bool endl)
{
	std::cout << "(";
	aggregate1->printIndexSet();
	std::cout << ",";
	aggregate2->printIndexSet();
	std::cout << ")";
	if (endl) { std::cout << std::endl; }
}

		
void Aggregate2D::buildDataStructure ()
{
	for (AggregateSetSet::iterator it1 = aggregate1->aggregateSetSet->begin(); it1 != aggregate1->aggregateSetSet->end(); it1++)
	{
		AggregateSet *currentSet = *it1;
		Aggregate2DSet *newSet = new Aggregate2DSet();

		for (AggregateSet::iterator it2 = currentSet->begin(); it2 != currentSet->end(); it2++)
		{
			Aggregate *subAggregate1 = *it2;
			newSet->push_back(structure->aggregateArray[subAggregate1->num + aggregate2->num * subAggregate1->structure->aggregateNumber]);
		}

		addAggregateSet(newSet);
	}

	for (AggregateSetSet::iterator it1 = aggregate2->aggregateSetSet->begin(); it1 != aggregate2->aggregateSetSet->end(); it1++)
	{
		AggregateSet *currentSet = *it1;
		Aggregate2DSet *newSet = new Aggregate2DSet();

		for (AggregateSet::iterator it2 = currentSet->begin(); it2 != currentSet->end(); it2++)
		{
			Aggregate *subAggregate2 = *it2;
			newSet->push_back(structure->aggregateArray[aggregate1->num + subAggregate2->num * aggregate1->structure->aggregateNumber]);
		}

		addAggregateSet(newSet);
	}

	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->buildDataStructure();
			}
		}
	}
}


void Aggregate2D::computeObjectiveValues ()
{
	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
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
		Aggregate2DSet *aggregateSet = *aggregateSetSet->begin();
		for (Aggregate2DSet::iterator it = aggregateSet->begin(); it != aggregateSet->end(); ++it) { qSet->insert((*it)->value); }
		value->compute(qSet);
		delete qSet;
	}
}


void Aggregate2D::normalizeObjectiveValues (ObjectiveValue *maxQual)
{
	if (maxQual == 0) { maxQual = value; }

	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->normalizeObjectiveValues(maxQual);
			}
		}
	}

	value->normalize(maxQual);
}


void Aggregate2D::printObjectiveValues ()
{
	printIndexSet();
	std::cout << " -> ";
	value->print(true);
	
	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->printObjectiveValues();
			}
		}
	}
}


void Aggregate2D::computeOptimalPartition (double parameter)
{
	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2)
		{
			Aggregate2D *aggregate = *it2;
			if (!aggregate->reached)
			{
				aggregate->reached = true;
				aggregate->computeOptimalPartition(parameter);
			}
		}
	}

	optimalValue = value->getValue(parameter);
	optimalCut = 0;

	for (Aggregate2DSetSet::iterator it1 = aggregateSetSet->begin(); it1 != aggregateSetSet->end(); ++it1)
	{
		Aggregate2DSet *aggregateSet = *it1;

		double value = 0;
		for (Aggregate2DSet::iterator it2 = aggregateSet->begin(); it2 != aggregateSet->end(); ++it2) { value += (*it2)->optimalValue; }

		if ((objective->maximize && value > optimalValue) || (!objective->maximize && value < optimalValue))
		{
			optimalValue = value;
			optimalCut = aggregateSet;
		}
	}
}


void Aggregate2D::printOptimalPartition (double parameter) {}


void Aggregate2D::buildOptimalPartition (Partition *partition)
{
	if (optimalCut == 0)
	{
		Part *p1 = new Part();
		Part *p2 = new Part();
		BiPart *part = new BiPart(p1,p2,value);

		for (IndexSet::iterator it = aggregate1->indexSet->begin(); it != aggregate1->indexSet->end(); ++it) { p1->addIndividual(*it); }
		for (IndexSet::iterator it = aggregate2->indexSet->begin(); it != aggregate2->indexSet->end(); ++it) { p2->addIndividual(*it); }

		partition->addPart(part,true);
	}
	else { for (Aggregate2DSet::iterator it = optimalCut->begin(); it != optimalCut->end(); it++) { (*it)->buildOptimalPartition(partition); } }
}
