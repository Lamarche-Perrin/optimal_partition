/*
 * This file is part of Optimal Partition.
 *
 * Optimal Partition is a toolbox to solve special versions of the Set
 * Partitioning Problem, that is the combinatorial optimisation of a
 * decomposable objective over a set of feasible partitions (defined
 * according to specific algebraic structures: e.g., hierachies, sets of
 * intervals, graphs). The objectives are mainly based on information theory,
 * in the perspective of multilevel analysis of large-scale datasets, and the
 * algorithms are based on dynamic programming. For details regarding the
 * formal grounds of this work, please refer to:
 * 
 * Robin Lamarche-Perrin, Yves Demazeau and Jean-Marc Vincent. A Generic Set
 * Partitioning Algorithm with Applications to Hierarchical and Ordered Sets.
 * Technical Report 105/2014, Max-Planck-Institute for Mathematics in the
 * Sciences, Leipzig, Germany, May 2014.
 * 
 * <http://www.mis.mpg.de/publications/preprints/2014/prepr2014-105.html>
 * 
 * Copyright © 2015 Robin Lamarche-Perrin
 * (<Robin.Lamarche-Perrin@lip6.fr>)
 * 
 * Optimal Partition is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * Optimal Partition is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */



#include <iostream>

#include "bi_set.hpp"

		
BiSet::BiSet (UniSet *uniSet1, UniSet *uniSet2)
{
	this->uniSet1 = uniSet1;
	this->uniSet2 = uniSet2;

	biSubsetNumber = 0;
	atomicBiSubsetNumber = 0;
	
	firstBiSubset = 0;
	biSubsetArray = 0;
}


BiSet::~BiSet ()
{
	delete firstBiSubset;
	delete [] biSubsetArray;
}


void BiSet::initReached()
{
	for (int num = 0; num < biSubsetNumber; num++) { biSubsetArray[num]->reached = false; }
}


void BiSet::setRandom () {}


void BiSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	initReached();
	firstBiSubset->setObjectiveFunction(m);
}


void BiSet::print ()
{
	initReached();
	firstBiSubset->print();
	std::cout << std::endl;
}


void BiSet::buildDataStructure ()
{
	biSubsetNumber = uniSet1->uniSubsetNumber * uniSet2->uniSubsetNumber;
	atomicBiSubsetNumber = uniSet1->atomicUniSubsetNumber * uniSet2->atomicUniSubsetNumber;
	biSubsetArray = new BiSubset *[biSubsetNumber];
	
	for (int num1 = 0; num1 < uniSet1->uniSubsetNumber; num1++)
	{
		UniSubset *uniSubset1 = uniSet1->uniSubsetArray[num1];
		
		for (int num2 = 0; num2 < uniSet2->uniSubsetNumber; num2++)
		{
			UniSubset *uniSubset2 = uniSet2->uniSubsetArray[num2];

			BiSubset *biSubset = new BiSubset (uniSubset1,uniSubset2);
			biSubset->biSet = this;

			biSubset->isAtomic = uniSubset1->isAtomic && uniSubset2->isAtomic;
			biSubset->num = num1 + num2 * uniSet1->uniSubsetNumber;
			biSubsetArray[biSubset->num] = biSubset;
		}
	}

	initReached();
	firstBiSubset = biSubsetArray[uniSet1->firstUniSubset->num + uniSet2->firstUniSubset->num * uniSet1->uniSubsetNumber];
	firstBiSubset->buildDataStructure();
}


void BiSet::computeObjectiveValues ()
{
	objective->computeObjectiveValues();
	initReached();
	firstBiSubset->computeObjectiveValues();
}


void BiSet::normalizeObjectiveValues ()
{
	initReached();
	firstBiSubset->normalizeObjectiveValues();
}


void BiSet::printObjectiveValues ()
{
	initReached();
	firstBiSubset->printObjectiveValues();
}


void BiSet::computeOptimalPartition (double parameter)
{
	initReached();
	firstBiSubset->computeOptimalPartition(parameter);
}


void BiSet::printOptimalPartition (double parameter)
{
	firstBiSubset->printOptimalPartition(parameter);
}


Partition *BiSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	firstBiSubset->buildOptimalPartition(partition);	
	return partition;
}



BiSubset::BiSubset (UniSubset *subset1, UniSubset *subset2)
{
	biSet = 0;
	
	num = -1;
	isAtomic = false;
	reached = false;
	
	uniSubset1 = subset1;
	uniSubset2 = subset2;
	biSubsetSetSet = new BiSubsetSetSet();

	value = 0;
	optimalCut = 0;
}


BiSubset::~BiSubset ()
{
	for (BiSubsetSetSet::iterator it = biSubsetSetSet->begin(); it != biSubsetSetSet->end(); ++it) { delete *it; }
	delete biSubsetSetSet;
	delete value;
}



void BiSubset::addBiSubsetSet (BiSubsetSet *biSubsetSet)
{
	biSubsetSetSet->push_back(biSubsetSet);
}


void BiSubset::setObjectiveFunction (ObjectiveFunction *m)
{
	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->setObjectiveFunction(m);
			}
		}
	}
	
	objective = m;
	if (isAtomic)
	{
		int index1 = *uniSubset1->indexSet->begin();
		int index2 = *uniSubset2->indexSet->begin();
		value = m->newObjectiveValue(index1 + index2 * uniSubset1->uniSet->atomicUniSubsetNumber);
	}
	else { value = m->newObjectiveValue(); }
}


void BiSubset::print ()
{
	printIndexSet(true);
	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;

		bool first = true;
		std::cout << " -> ";
		
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!first) { std::cout << " "; } else { first = false; }
			biSubset->printIndexSet();
		}
		std::cout << std::endl;
	}

	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->print();
			}
		}
	}
}


void BiSubset::printIndexSet (bool endl)
{
	std::cout << "(";
	uniSubset1->printIndexSet();
	std::cout << ",";
	uniSubset2->printIndexSet();
	std::cout << ")";
	if (endl) { std::cout << std::endl; }
}

		
void BiSubset::buildDataStructure ()
{
	for (UniSubsetSetSet::iterator it1 = uniSubset1->uniSubsetSetSet->begin(); it1 != uniSubset1->uniSubsetSetSet->end(); it1++)
	{
		UniSubsetSet *currentSet = *it1;
		BiSubsetSet *newSet = new BiSubsetSet();

		for (UniSubsetSet::iterator it2 = currentSet->begin(); it2 != currentSet->end(); it2++)
		{
			UniSubset *subSubset1 = *it2;
			newSet->push_back(biSet->biSubsetArray[subSubset1->num + uniSubset2->num * subSubset1->uniSet->uniSubsetNumber]);
		}

		addBiSubsetSet(newSet);
	}

	for (UniSubsetSetSet::iterator it1 = uniSubset2->uniSubsetSetSet->begin(); it1 != uniSubset2->uniSubsetSetSet->end(); it1++)
	{
		UniSubsetSet *currentSet = *it1;
		BiSubsetSet *newSet = new BiSubsetSet();

		for (UniSubsetSet::iterator it2 = currentSet->begin(); it2 != currentSet->end(); it2++)
		{
			UniSubset *subSubset2 = *it2;
			newSet->push_back(biSet->biSubsetArray[uniSubset1->num + subSubset2->num * uniSubset1->uniSet->uniSubsetNumber]);
		}

		addBiSubsetSet(newSet);
	}

	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->buildDataStructure();
			}
		}
	}
}


void BiSubset::computeObjectiveValues ()
{
	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->computeObjectiveValues();
			}
		}
	}

	if (isAtomic) { value->compute(); }
	else {
		ObjectiveValueSet *qSet = new ObjectiveValueSet();
		BiSubsetSet *biSubsetSet = *biSubsetSetSet->begin();
		for (BiSubsetSet::iterator it = biSubsetSet->begin(); it != biSubsetSet->end(); ++it) { qSet->insert((*it)->value); }
		value->compute(qSet);
		delete qSet;
	}
}


void BiSubset::normalizeObjectiveValues (ObjectiveValue *maxQual)
{
	if (maxQual == 0) { maxQual = value; }

	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->normalizeObjectiveValues(maxQual);
			}
		}
	}

	value->normalize(maxQual);
}


void BiSubset::printObjectiveValues ()
{
	printIndexSet();
	std::cout << " -> ";
	value->print(true);
	
	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->printObjectiveValues();
			}
		}
	}
}


void BiSubset::computeOptimalPartition (double parameter)
{
	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2)
		{
			BiSubset *biSubset = *it2;
			if (!biSubset->reached)
			{
				biSubset->reached = true;
				biSubset->computeOptimalPartition(parameter);
			}
		}
	}

	optimalValue = value->getValue(parameter);
	optimalCut = 0;

	for (BiSubsetSetSet::iterator it1 = biSubsetSetSet->begin(); it1 != biSubsetSetSet->end(); ++it1)
	{
		BiSubsetSet *biSubsetSet = *it1;

		double value = 0;
		for (BiSubsetSet::iterator it2 = biSubsetSet->begin(); it2 != biSubsetSet->end(); ++it2) { value += (*it2)->optimalValue; }

		if ((objective->maximize && value > optimalValue) || (!objective->maximize && value < optimalValue))
		{
			optimalValue = value;
			optimalCut = biSubsetSet;
		}
	}
}


void BiSubset::printOptimalPartition (double parameter) {}


void BiSubset::buildOptimalPartition (Partition *partition)
{
	if (optimalCut == 0)
	{
		Part *p1 = new Part();
		Part *p2 = new Part();
		BiPart *part = new BiPart(p1,p2,value);

		for (IndexSet::iterator it = uniSubset1->indexSet->begin(); it != uniSubset1->indexSet->end(); ++it) { p1->addIndividual(*it); }
		for (IndexSet::iterator it = uniSubset2->indexSet->begin(); it != uniSubset2->indexSet->end(); ++it) { p2->addIndividual(*it); }

		partition->addPart(part,true);
	}
	else { for (BiSubsetSet::iterator it = optimalCut->begin(); it != optimalCut->end(); it++) { (*it)->buildOptimalPartition(partition); } }
}
