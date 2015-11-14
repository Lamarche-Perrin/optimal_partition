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
#include <list>
#include <cmath>

#include "uni_set.hpp"
#include "csv_tools.hpp"


UniSet::UniSet (UniSubset *firstSubset)
{
	uniSubsetNumber = 0;
	atomicUniSubsetNumber = 0;
	firstUniSubset = firstSubset;
	uniSubsetArray = 0;
	atomicUniSubsetArray = 0;

	voterMeasurement = 0;
	voterProbe = 0;
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
		UniSubset *subset = *it;
		uniSubsetArray[subset->num] = subset;
		if (subset->isAtomic) { atomicUniSubsetArray[subset->atomicNum] = subset; }
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


UnconstrainedUniSet::UnconstrainedUniSet (int s) : UniSet (0)
{
	size = s;
	long unsigned int subsetNb = std::pow(2,size);
	UniSubset **subsetArray = new UniSubset* [subsetNb];

	int index = 0;
	for (long unsigned int i = 0; i < subsetNb; i++)
	{
		int elementNb = 0;
		bool boolArray [size];

		long unsigned int j = i;
		for (int s = 0; s < size; s++)
		{
			boolArray[s] = (j % 2 == 1);
			if (boolArray[s]) { elementNb++; }
			j = j >> 1;
		}

		if (elementNb == 1) { subsetArray[i] = new UniSubset (index++); }
		else { subsetArray[i] = new UniSubset (); }
	}

	for (long unsigned int i = 1; i < subsetNb; i++)
	{
		long unsigned int elementNb = 0;
		std::vector<int> elementPos;

		long unsigned int j = i;
		for (int s = 0; s < size; s++)
		{
			if (j % 2 == 1) { elementPos.push_back(s); elementNb++; }
			j = j >> 1;
		}

		//std::cout << "subset " << i << " (" << elementNb << ")";
		//if (subsetArray[i]->isAtomic) { std::cout << " atomic"; }
		//std::cout << std::endl;

		for (long unsigned int k = 1; k < pow(2,elementNb-1); k++)
		{
			long unsigned int k1 = 1 << elementPos[elementNb-1];
			long unsigned int k2 = 0;

			long unsigned int l = k;
			for (unsigned long int s = 0; s < elementNb-1; s++)
			{
				if (l % 2 == 0) { k1 += 1 << elementPos[s]; } else { k2 += 1 << elementPos[s]; }
				l = l >> 1;
			}

			UniSubsetSet *subsetSet = new UniSubsetSet ();
			subsetSet->push_back(subsetArray[k1]);
			subsetSet->push_back(subsetArray[k2]);
			subsetArray[i]->addUniSubsetSet(subsetSet);			
			//std::cout << "-> " << k1 << " " << k2 << std::endl;
		}
	}

	firstUniSubset = subsetArray[subsetNb-1];
	//std::cout << "first subset " << (subsetNb-1) << std::endl;
	delete [] subsetArray;
}


OrderedUniSet::OrderedUniSet (int s) : UniSet (0)
{
	size = s;
	int size2 = size*(size+1)/2;

	UniSubset **subsetArray = new UniSubset* [size2];
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
		{
			int index = -1;
			if (j == 0) { index = i; }
			subsetArray[getCell(i,j)] = new UniSubset (index);

			if (j == 0) { subsetArray[getCell(i,j)]->name = "[" + int2string(i) + "]"; }
			else { subsetArray[getCell(i,j)]->name = "[" + int2string(i) + "," + int2string(i+j) + "]"; }
		}

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
			for (int c = 0; c < j; c++)
			{
				UniSubsetSet *subsetSet = new UniSubsetSet ();
				subsetSet->push_back(subsetArray[getCell(i,c)]);
				subsetSet->push_back(subsetArray[getCell(i+c+1,j-c-1)]);
				subsetArray[getCell(i,j)]->addUniSubsetSet(subsetSet);
			}

	firstUniSubset = subsetArray[getCell(0,size-1)];
	delete [] subsetArray;
}


int OrderedUniSet::getCell (int i, int j) { return j*size-j*(j-1)/2+i; }


HierarchicalUniSet::HierarchicalUniSet (std::string fileName) : UniSet (0)
{
	int index = 0;
	std::map<std::string,std::string> parentMap;
	std::map<std::string,UniSubset*> subsetMap;

	std::ifstream file;
	openInputCSV (file, fileName);

	CSVLine line;
	while (hasCSVLine (file))
	{
		getCSVLine (file, line);
		parentMap.insert(std::make_pair(line[0],line[2]));
	}

	for (std::map<std::string,std::string>::iterator it1 = parentMap.begin(); it1 != parentMap.end(); it1++)
	{
		std::string name = it1->first;
		std::string parent = it1->second;
		
		bool atomic = true;
		for (std::map<std::string,std::string>::iterator it2 = parentMap.begin(); it2 != parentMap.end() && atomic; it2++)
			atomic = (it2->second != name);

		UniSubset *subset;
		if (atomic) { subset = new UniSubset(index++); }
		else { subset = new UniSubset(); }

		subset->name = name;
		subsetMap.insert (std::make_pair(name,subset));
		if (parent == "NULL") { firstUniSubset = subset; }
	}

	for (std::map<std::string,UniSubset*>::iterator it1 = subsetMap.begin(); it1 != subsetMap.end(); it1++)
	{
		std::string name = it1->first;
		UniSubset *subset = it1->second;

		if (subset->isAtomic) { continue; }

		UniSubsetSet *subsetSet = new UniSubsetSet ();
		for (std::map<std::string,std::string>::iterator it2 = parentMap.begin(); it2 != parentMap.end(); it2++)
		{
			if (it2->second == name) { subsetSet->push_back(subsetMap.at(it2->first)); }
		}
		subset->addUniSubsetSet(subsetSet);
	}	
}


BinaryTreeUniSet::BinaryTreeUniSet (int d) : UniSet (0)
{
	depth = d;
	if (d == 0) { firstUniSubset = new UniSubset (0); }
	else {
		firstUniSubset = new UniSubset ();
		buildTree (firstUniSubset,d-1,0);
	}
}


int BinaryTreeUniSet::buildTree (UniSubset *subset, int d, int i)
{
	UniSubset *subset1;
	UniSubset *subset2;

	if (d == 0)
	{
		subset1 = new UniSubset (i++);
		subset2 = new UniSubset (i++);
	}

	else {
		subset1 = new UniSubset ();
		subset2 = new UniSubset ();	
	}

	UniSubsetSet *subsetSet = new UniSubsetSet ();
	subsetSet->push_back(subset1);
	subsetSet->push_back(subset2);
	subset->addUniSubsetSet(subsetSet);

	if (d > 0)
	{
		i = buildTree(subset1,d-1,i);
		i = buildTree(subset2,d-1,i);
	}
	
	return i;
}


GraphBasedUniSet::GraphBasedUniSet (Graph *g) : UniSet (0)
{
	graph = g;	
}


UniSubset::UniSubset (int index)
{
	uniSet = 0;

	name = "";
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
	if (name != "") { std::cout << "[" << name << "]"; } else { printIndexSet(); }
	
	for (UniSubsetSetSet::iterator it1 = uniSubsetSetSet->begin(); it1 != uniSubsetSetSet->end(); ++it1)
	{
		UniSubsetSet *uniSubsetSet = *it1;

		bool first = true;
		std::cout << " -> ";

		for (UniSubsetSet::iterator it2 = uniSubsetSet->begin(); it2 != uniSubsetSet->end(); ++it2)
		{
			UniSubset *uniSubset = *it2;
			if (!first) { std::cout << " "; } else { first = false; }
			if (uniSubset->name != "") { std::cout << "[" << uniSubset->name << "]"; } else { uniSubset->printIndexSet(); }
		}
	}
	std::cout << std::endl;
	
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
