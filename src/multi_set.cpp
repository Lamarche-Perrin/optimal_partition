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
#include <cstdlib>
#include <algorithm>

#include "multi_set.hpp"


MultiSet::MultiSet (UniSet *str)
{
	dimension = 1;
	uniSetArray = new UniSet* [1];
	uniSetArray[0] = str;

	multiSubsetNumber = 0;
	atomicMultiSubsetNumber = 0;
	
	firstMultiSubset = 0;
	multiSubsetArray = 0;
	atomicMultiSubsetArray = 0;
}


MultiSet::MultiSet (UniSet **strArray, int dim)
{
	dimension = dim;
	uniSetArray = strArray;

	multiSubsetNumber = 0;
	atomicMultiSubsetNumber = 0;
	
	firstMultiSubset = 0;
	multiSubsetArray = 0;
	atomicMultiSubsetArray = 0;
}


MultiSet::MultiSet (std::vector<UniSet*> *strVector)
{
	dimension = strVector->size();
	uniSetArray = new UniSet* [dimension];
	
	for (int d = 0; d < dimension; d++)
		uniSetArray[d] = strVector->at(d);

	multiSubsetNumber = 0;
	atomicMultiSubsetNumber = 0;
	
	firstMultiSubset = 0;
	multiSubsetArray = 0;
	atomicMultiSubsetArray = 0;
}


MultiSet::~MultiSet ()
{
	for (int num = 0; num < multiSubsetNumber; num++) { delete multiSubsetArray[num]; }
	delete [] multiSubsetArray;
	delete [] atomicMultiSubsetArray;
}


int MultiSet::getNum (int *multiNum)
{
	int num = 0;
	for (int d = dimension-1; d >= 0; d--)
	{
		if (d < dimension-1) { num *= uniSetArray[d]->uniSubsetNumber; }
		num += multiNum[d];
	}
	return num;
}


int *MultiSet::getMultiNum (int num)
{
	int *multiNum = new int [dimension];
	for (int d = 0; d < dimension; d++)
	{
		multiNum[d] = num % uniSetArray[d]->uniSubsetNumber;
		num -= multiNum[d];
		num /= uniSetArray[d]->uniSubsetNumber;
	}
	return multiNum;
}


int MultiSet::getAtomicNum (int *atomicMultiNum)
{
	int atomicNum = 0;
	for (int d = dimension-1; d >= 0; d--)
	{
		if (d < dimension-1) { atomicNum *= uniSetArray[d]->atomicUniSubsetNumber; }
		atomicNum += atomicMultiNum[d];
	}
	return atomicNum;
}


int *MultiSet::getAtomicMultiNum (int atomicNum)
{
	int *atomicMultiNum = new int [dimension];
	for (int d = 0; d < dimension; d++)
	{
		atomicMultiNum[d] = atomicNum % uniSetArray[d]->atomicUniSubsetNumber;
		atomicNum -= atomicMultiNum[d];
		atomicNum /= uniSetArray[d]->atomicUniSubsetNumber;
	}
	return atomicMultiNum;
}


MultiSubset *MultiSet::getAtomicMultiSubset (int index)
{
	if (dimension > 1)
	{
		std::cout << "ERROR: use of getAtomicMultiSubset on a multidimensional structure!" << std::endl;
		return 0;
	}
	
	for (int num = 0; num < atomicMultiSubsetNumber; num++)
	{
		UniSubset *subset = atomicMultiSubsetArray[num]->uniSubsetArray[0];
		if (subset->isAtomic && index == subset->indexSet->front()) { return atomicMultiSubsetArray[num]; }
	}

	std::cout << "ERROR: atomic subset not found!" << std::endl;
	return 0;
}


MultiSubset *MultiSet::getAtomicMultiSubset (int *indices)
{
	for (int num = 0; num < atomicMultiSubsetNumber; num++)
	{
		MultiSubset *multiSubset = atomicMultiSubsetArray[num];
		bool equal = true;
		for (int d = 0; d < dimension && equal; d++)
		{
			UniSubset *uniSubset = multiSubset->uniSubsetArray[d];
			equal = (indices[d] == uniSubset->indexSet->front());
		}		
		if (equal) { return multiSubset; }
	}

	std::cout << "ERROR: atomic subset not found!" << std::endl;
	return 0;
}


MultiSubset *MultiSet::getAtomicMultiSubset (double *values)
{
	for (int num = 0; num < atomicMultiSubsetNumber; num++)
	{
		MultiSubset *multiSubset = atomicMultiSubsetArray[num];
		bool within = true;
		for (int d = 0; d < dimension && within; d++)
		{
			UniSubset *uniSubset = multiSubset->uniSubsetArray[d];
			within = (values[d] >= uniSubset->start && values[d] < uniSubset->end);
		}
		if (within) { return multiSubset; }
	}

	std::cout << "ERROR: atomic subset not found!" << std::endl;
	return 0;	
}


MultiSubset *MultiSet::getRandomAtomicMultiSubset () { return atomicMultiSubsetArray[rand() % atomicMultiSubsetNumber]; }


void MultiSet::initReached()
{
	for (int num = 0; num < multiSubsetNumber; num++) { multiSubsetArray[num]->reached = false; }
}


void MultiSet::initAtomicReached()
{
	for (int atomicNum = 0; atomicNum < atomicMultiSubsetNumber; atomicNum++) { atomicMultiSubsetArray[atomicNum]->reached = false; }
}


MultiSubset **MultiSet::getOptimalMultiSubset (double parameter, int number)
{
	// TODO: THIS METHOD CAN BE OPTIMISED!
	MultiSubset **subsetArray = new MultiSubset* [number];

	double optimalValue = multiSubsetArray[0]->value->getValue(parameter);
	subsetArray[0] = multiSubsetArray[0];

	for (int num = 1; num < number; num++)
	{
		subsetArray[num] = multiSubsetArray[num];

		double value = multiSubsetArray[num]->value->getValue(parameter);
		if ((objective->maximize && value < optimalValue) || (!objective->maximize && value > optimalValue)) { optimalValue = value; }
	}
	
	for (int num = number; num < multiSubsetNumber; num++)
	{
		double value = multiSubsetArray[num]->value->getValue(parameter);
		if ((objective->maximize && value > optimalValue) || (!objective->maximize && value < optimalValue))
		{
			int nWorst = 0;
			double worst = subsetArray[nWorst]->value->getValue(parameter);
			for (int n = 1; n < number; n++)
			{
				double v = subsetArray[n]->value->getValue(parameter);
				if ((objective->maximize && v < worst) || (!objective->maximize && v > worst)) { nWorst = n; worst = v; }				
			}
			subsetArray[nWorst] = multiSubsetArray[num];

			optimalValue = subsetArray[0]->value->getValue(parameter);
			for (int num = 1; num < number; num++)
			{
				double v = subsetArray[num]->value->getValue(parameter);
				if ((objective->maximize && v < optimalValue) || (!objective->maximize && v > optimalValue)) { optimalValue = v; }
			}
		}
	}

	return subsetArray;
}


void MultiSet::setRandom () {}


void MultiSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	initReached();
	firstMultiSubset->setObjectiveFunction(m);
}


void MultiSet::print ()
{
	initReached();
	firstMultiSubset->print();
	std::cout << std::endl;
}


void MultiSet::buildDataStructure ()
{
	firstMultiSubset = 0;
	multiSubsetNumber = 1;
	atomicMultiSubsetNumber = 1;

	for (int d = 0; d < dimension; d++)
	{
		multiSubsetNumber *= uniSetArray[d]->uniSubsetNumber;
		atomicMultiSubsetNumber *= uniSetArray[d]->atomicUniSubsetNumber;
	}
	
	multiSubsetArray = new MultiSubset* [multiSubsetNumber];
	atomicMultiSubsetArray = new MultiSubset* [atomicMultiSubsetNumber];
	
	int atomicNum = 0;
	for (int num = 0; num < multiSubsetNumber; num++)
	{
		int *multiNum = getMultiNum(num);
		UniSubset **subsetArray = new UniSubset* [dimension];
		for (int d = 0; d < dimension; d++) { subsetArray[d] = uniSetArray[d]->uniSubsetArray[multiNum[d]]; }

		MultiSubset *multiSubset = new MultiSubset (subsetArray,dimension);
		multiSubsetArray[num] = multiSubset;

		multiSubset->multiSet = this;
		multiSubset->num = num;
		multiSubset->isAtomic = true;
		for (int d = 0; d < dimension && multiSubset->isAtomic; d++) { multiSubset->isAtomic = uniSetArray[d]->uniSubsetArray[multiNum[d]]->isAtomic; }
		if (multiSubset->isAtomic)
		{
			multiSubset->atomicNum = atomicNum++;
			atomicMultiSubsetArray[multiSubset->atomicNum] = multiSubset;
		}

		if (firstMultiSubset == 0)
		{
			bool isFirstMultiSubset = true;
			for (int d = 0; d < dimension && isFirstMultiSubset; d++) { isFirstMultiSubset = (subsetArray[d] == uniSetArray[d]->firstUniSubset); }
			if (isFirstMultiSubset) { firstMultiSubset = multiSubset; }
		}

		delete [] multiNum;
	}

	initReached();
	firstMultiSubset->buildDataStructure();
}


void MultiSet::computeObjectiveValues ()
{
	initReached();
	objective->computeObjectiveValues();
	firstMultiSubset->computeObjectiveValues();
}


void MultiSet::normalizeObjectiveValues ()
{
	initReached();
	firstMultiSubset->normalizeObjectiveValues();
}


void MultiSet::printObjectiveValues ()
{
	objective->printObjectiveValues();
	initReached();
	firstMultiSubset->printObjectiveValues();
}


void MultiSet::computeOptimalPartition (double parameter)
{
	initReached();
	firstMultiSubset->computeOptimalPartition(parameter);
}


void MultiSet::printOptimalPartition (double parameter)
{
	firstMultiSubset->printOptimalPartition(parameter);
}


Partition *MultiSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	firstMultiSubset->buildOptimalPartition(partition);	
	return partition;
}


void MultiSet::approximateOptimalPartition (ObjectiveFunction *m, double parameter)
{
	// BUILD ARRAY OF ATOMIC MULTISUBSETS
 	int atomicNumber = 1;
	for (int d = 0; d < dimension; d++) { atomicNumber *= uniSetArray[d]->atomicUniSubsetNumber; }

	MultiSubset **atomicArray = new MultiSubset* [atomicNumber];		
	
	for (int atomicNum = 0; atomicNum < atomicNumber; atomicNum++)
	{
		int *atomicMultiNum = getAtomicMultiNum(atomicNum);
		UniSubset **subsetArray = new UniSubset* [dimension];
		for (int d = 0; d < dimension; d++) { subsetArray[d] = uniSetArray[d]->atomicUniSubsetArray[atomicMultiNum[d]]; }

		MultiSubset *atomic = new MultiSubset (subsetArray, dimension);
		atomicMultiSubsetArray[atomicNum] = atomic;
		atomicArray[atomicNum] = atomic;

		atomic->multiSet = this;
		atomic->atomicNum = atomicNum;
		atomic->isAtomic = true;
		atomic->objective = m;
		atomic->value = m->newObjectiveValue(atomicNum);
			
		delete [] atomicMultiNum;
	}

	initAtomicReached();
	std::random_shuffle (&atomicArray[0], &atomicArray[atomicNumber]);
	int unreachedIndex = 0;
	
	// SELECT AN UNCOVERED ATOMIC MULTISUBSET
	while (unreachedIndex < atomicNumber)
	{
		while (atomicArray[unreachedIndex]->reached && unreachedIndex < atomicNumber) { unreachedIndex++; }

		if (unreachedIndex < atomicNumber)
		{		
			// CREATE LIST OF NEXT MULTISUBSETS
			MultiSubset *atomic = atomicArray[unreachedIndex];

			MultiSubsetSet nextSubsetSet;
			for (int d = 0; d < dimension; d++)
			{	
				for (UniSubsetPairSet::iterator it1 = atomic->uniSubsetArray[d]->parentSet->begin(); it1 != atomic->uniSubsetArray[d]->parentSet->end(); ++it1)
				{
					UniSubset *parent = (*it1)->first;
					UniSubsetSet *addedUniSubsets = (*it1)->second;

					// CREATE NEXT MULTISUBSET
					UniSubset **subsetArray = new UniSubset* [dimension];
					for (int dp = 0; dp < dimension; dp++) { subsetArray[dp] = atomic->uniSubsetArray[dp]; }
					subsetArray[d] = parent;
					
					MultiSubset *subset = new MultiSubset (subsetArray, dimension);
					subset->multiSet = this;

					
					// COMPUTE NEW OBJECTIVE VALUE
					subset->objective = m;
					subset->value = m->newObjectiveValue();

					ObjectiveValueSet *valueSet = new ObjectiveValueSet ();
					valueSet->insert(atomic->value);

					int *multiNum = new int [dimension];
					for (int dp = 0; dp < dimension; dp++) { multiNum[dp] = atomic->uniSubsetArray[dp]->atomicNum; }
					for (UniSubsetSet::iterator it2 = addedUniSubsets->begin(); it2 != addedUniSubsets->end(); ++it2)
					{
						multiNum[d] = (*it2)->atomicNum;
						valueSet->insert(atomicArray[getAtomicNum(multiNum)]->value);
					}
					delete multiNum;
					
					subset->value->compute(valueSet);
					delete valueSet;

					nextSubsetSet.push_back(subset);
				}
			}
			
			// SELECT BEST NEXT MULTISUBSET

			// UPDATE LIST OF NEXT MULTISUBSETS

		
		}
	}

	for (int atomicNum = 0; atomicNum < atomicNumber; atomicNum++) { delete atomicArray[atomicNum]; }
	delete [] atomicArray;
}


MultiSubset::MultiSubset (UniSubset **subsetArray, int dim)
{
	multiSet = 0;
	
	num = -1;
	atomicNum = -1;
	isAtomic = false;
	reached = false;

	dimension = dim;
	uniSubsetArray = subsetArray;
	multiSubsetSetSet = new MultiSubsetSetSet();

	value = 0;
	optimalCut = 0;
}


MultiSubset::~MultiSubset ()
{
	delete [] uniSubsetArray;
	for (MultiSubsetSetSet::iterator it = multiSubsetSetSet->begin(); it != multiSubsetSetSet->end(); ++it) { delete *it; }
	delete multiSubsetSetSet;
	delete value;
}



void MultiSubset::addMultiSubsetSet (MultiSubsetSet *multiSubsetSet)
{
	multiSubsetSetSet->push_back(multiSubsetSet);
}


void MultiSubset::setObjectiveFunction (ObjectiveFunction *m)
{
	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->setObjectiveFunction(m);
			}
		}
	}

	objective = m;
	if (isAtomic)
	{
		int index = 0;
		for (int d = dimension-1; d >= 0; d--)
		{
			index += *uniSubsetArray[d]->indexSet->begin();
			if (d > 0) { index *= uniSubsetArray[d-1]->uniSet->atomicUniSubsetNumber; }
		}
		value = m->newObjectiveValue(index);
	}
	else { value = m->newObjectiveValue(); }
}

/*
std::string MultiSubset::getName ()
{
	bool hasName = true;
	std::string name = "";
	for (int d = 0; d < dimension && hasName; d++)
	{
		hasName = (uniSubsetArray[d]->name != "");
		if (d > 0) { name += " x "; }
		name += uniSubsetArray[d]->name;
	}

	if (hasName) { return name; } else { return ""; }
}
*/

void MultiSubset::print ()
{
	//std::cout << "[" << num << "] ";
	printIndexSet();
	if (isAtomic) { std::cout << " atomic"; }
	if (value != 0) { std::cout << "   ->   "; value->print(true); }
	else { std::cout << std::endl; }

	if (value == 0)
	{
		for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
		{
			MultiSubsetSet *multiSubsetSet = *it1;

			bool first = true;
			std::cout << " -> ";
		
			for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
			{
				MultiSubset *multiSubset = *it2;
				if (!first) { std::cout << " "; } else { first = false; }
				multiSubset->printIndexSet();
			}
			std::cout << std::endl;
		}
	}

	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->print();
			}
		}
	}
}


void MultiSubset::printIndexSet (bool endl)
{
	std::cout << "(";
	for (int d = 0; d < dimension; d++)
	{
		if (uniSubsetArray[d]->name != "") { std::cout << uniSubsetArray[d]->name; } else { uniSubsetArray[d]->printIndexSet(); }
		if (d+1 < dimension) { std::cout << ", "; }
	}
	std::cout << ")";
	if (endl) { std::cout << std::endl; }
}

		
void MultiSubset::buildDataStructure ()
{
	int *multiNum = new int [dimension];
	for (int d = 0; d < dimension; d++) { multiNum[d] = uniSubsetArray[d]->num; }

	for (int d = 0; d < dimension; d++)
	{
		for (UniSubsetSetSet::iterator it1 = uniSubsetArray[d]->uniSubsetSetSet->begin(); it1 != uniSubsetArray[d]->uniSubsetSetSet->end(); it1++)
		{
			UniSubsetSet *currentSet = *it1;
			MultiSubsetSet *newSet = new MultiSubsetSet();

			for (UniSubsetSet::iterator it2 = currentSet->begin(); it2 != currentSet->end(); it2++)
			{
				UniSubset *subSubset = *it2;
				multiNum[d] = subSubset->num;
				newSet->push_back(multiSet->multiSubsetArray[multiSet->getNum(multiNum)]);
			}

			addMultiSubsetSet(newSet);
		}
		multiNum[d] = uniSubsetArray[d]->num;
	}
	delete [] multiNum;

	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->buildDataStructure();
			}
		}
	}
}


void MultiSubset::computeObjectiveValues ()
{
	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->computeObjectiveValues();
			}
		}
	}

	if (isAtomic) { value->compute(); }
	else {
		ObjectiveValueSet *qSet = new ObjectiveValueSet ();
		MultiSubsetSet *multiSubsetSet = *multiSubsetSetSet->begin();
		for (MultiSubsetSet::iterator it = multiSubsetSet->begin(); it != multiSubsetSet->end(); ++it) { qSet->insert((*it)->value); }
		value->compute(qSet);
		delete qSet;
	}
}


void MultiSubset::normalizeObjectiveValues (ObjectiveValue *maxQual)
{
	if (maxQual == 0) { maxQual = value; }

	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->normalizeObjectiveValues(maxQual);
			}
		}
	}

	value->normalize(maxQual);
}


void MultiSubset::printObjectiveValues ()
{
	printIndexSet();
	std::cout << " -> ";
	value->print(true);
	
	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->printObjectiveValues();
			}
		}
	}
}


void MultiSubset::computeOptimalPartition (double parameter)
{
	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2)
		{
			MultiSubset *multiSubset = *it2;
			if (!multiSubset->reached)
			{
				multiSubset->reached = true;
				multiSubset->computeOptimalPartition(parameter);
			}
		}
	}

	optimalValue = value->getValue(parameter);
	optimalCut = 0;

	for (MultiSubsetSetSet::iterator it1 = multiSubsetSetSet->begin(); it1 != multiSubsetSetSet->end(); ++it1)
	{
		MultiSubsetSet *multiSubsetSet = *it1;

		double value = 0;
		for (MultiSubsetSet::iterator it2 = multiSubsetSet->begin(); it2 != multiSubsetSet->end(); ++it2) { value += (*it2)->optimalValue; }

		if ((objective->maximize && value >= optimalValue) || (!objective->maximize && value <= optimalValue))
		{
			optimalValue = value;
			optimalCut = multiSubsetSet;
		}
	}
}


void MultiSubset::printOptimalPartition (double parameter) {}


void MultiSubset::buildOptimalPartition (Partition *partition)
{
	if (optimalCut == 0)
	{
		Part **partArray = new Part* [dimension];
		for (int d = 0; d < dimension; d++)
		{
			Part *p = new Part();
			for (IndexSet::iterator it = uniSubsetArray[d]->indexSet->begin(); it != uniSubsetArray[d]->indexSet->end(); ++it) { p->addIndividual(*it); }
			partArray[d] = p;
			p->name = uniSubsetArray[d]->name;
		}

		MultiPart *part = new MultiPart (partArray,dimension,value);
		partition->addPart(part,true);
	}
	else { for (MultiSubsetSet::iterator it = optimalCut->begin(); it != optimalCut->end(); it++) { (*it)->buildOptimalPartition(partition); } }
}
