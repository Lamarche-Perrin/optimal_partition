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
#include <utility>
#include <list>
#include <math.h>

#include "orderedset.hpp"


OrderedSet::OrderedSet (int s)
{
	size = s;
	int s2 = (s+1)*s/2;
	
	qualities = new ObjectiveValue* [s2];
	optimalValues = new double [s];
	optimalCuts = new int [s];
}


void OrderedSet::setRandom () {}

void OrderedSet::buildDataStructure () {}

OrderedSet::~OrderedSet ()
{
	delete[] qualities;
	delete[] optimalValues;
	delete[] optimalCuts;
}


void OrderedSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	
	for (int i = 0; i < size; i++) { qualities[getIndex(i,0)] = objective->newObjectiveValue(i); }

	for (int j = 1; j < size; j++)
	{
		for (int i = 0; i < size-j; i++) { qualities[getIndex(i,j)] = objective->newObjectiveValue(); }
	}
}


int OrderedSet::getIndex (int i, int j) { return j*size-j*(j-1)/2+i; }


void OrderedSet::print ()
{
	for (int i = 0; i < size; i++)
	{
		std::cout << "[" << i << "] -> ";
		qualities[getIndex(i,0)]->print(false);
	}
}


void OrderedSet::computeObjectiveValues ()
{
	for (int i = 0; i < size; i++)
		qualities[getIndex(i,0)]->compute();
		
	for (int j = 1; j < size; j++)
		for (int i = 0; i < size-j; i++)
			qualities[getIndex(i,j)]->compute(qualities[getIndex(i,j-1)],qualities[getIndex(i+j,0)]);
}


void OrderedSet::normalizeObjectiveValues ()
{
	ObjectiveValue *maxQual = qualities[getIndex(0,size-1)];

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
			qualities[getIndex(i,j)]->normalize(maxQual);
}


void OrderedSet::printObjectiveValues ()
{
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
		{
			std::cout << "[" << i << "," << j << "] -> ";
			qualities[getIndex(i,j)]->print(true);
		}
}


void OrderedSet::computeOptimalPartition (double parameter)
{
	optimalValues[0] = qualities[getIndex(0,0)]->getValue(parameter);
	optimalCuts[0] = 0;
	
	for (int j = 1; j < size; j++)
	{
		int currentCut = 0;
		float currentValue = qualities[getIndex(0,j)]->getValue(parameter);
		
		for (int cut = 1; cut < j+1; cut++)
		{
			float value = optimalValues[cut-1] + qualities[getIndex(cut,j-cut)]->getValue(parameter);
			if ((objective->maximize && value > currentValue) || (!objective->maximize && value < currentValue))
			{
				currentValue = value;
				currentCut = cut;
			}
		}
		
		optimalValues[j] = currentValue;
		optimalCuts[j] = currentCut;
	}
}


void OrderedSet::printOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	
	std::list< std::pair<int,int> > cutList = std::list< std::pair<int,int> >();

	int last;
	int first = size;
	while (first > 0)
	{
		last = first-1;
		first = optimalCuts[last];
		cutList.push_front(std::make_pair(first,last));
	}
	
	while (!cutList.empty())
	{
		std::pair<int,int> cut = cutList.front();
		if (cut.first != cut.second) { std::cout << "[" << cut.first << "-" << cut.second << "] "; }
		else { std::cout << "[" << cut.first << "] "; }
		cutList.pop_front();
	}
	std::cout << std::endl;
}


Partition *OrderedSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);

	Partition *partition = new Partition(objective,parameter);
//	std::list<Part*> *partList = new std::list<Part*>();
	
	int i;
	int k = size-1;
	
	while (k >= 0)
	{
		i = optimalCuts[k];
		int j = k-i;

		Part *part = new Part(qualities[getIndex(i,j)]);
//		part->value->print();
		for (int l = i; l <= k; l++) { part->addIndividual(l); }
		partition->addPart(part,true);
//		partList->push_front(part);
		
		k = i - 1;
	}

//	for (std::list<Part*>::iterator it = partList->begin(); it != partList->end(); ++it) { partition->addPart(*it); }
	
	return partition;
}



/*

void OrderedSet::printPartitions (vector<part> partitions) {
	for (vector<part>::iterator it = partitions.begin(); it != partitions.end(); ++it) {
		printPartition(*it);
		cout << endl;
	}
}


void OrderedSet::writeAllPartitionsToCSV (vector<part> &partitions, string fileName) {
	ofstream file;
	openOutputCSV(file,fileName+"-PARTITIONS.csv");

	vector<string> headLine;
	headLine.push_back("ID");
	headLine.push_back("COEFF");
	headLine.push_back("SIZE");
	headLine.push_back("NORM_SIZE_REDUCTION");
	headLine.push_back("NORM_KL_DIVERGENCE");
	headLine.push_back("MAX_SIZE_REDUCTION");
	headLine.push_back("MAX_KL_DIVERGENCE");
	addCSVLine(file,headLine);

	int id = 0;	
	for (vector<part>::iterator it = partitions.begin(); it != partitions.end(); ++it) {
		part *p = &(*it);

		vector<string> line;
		line.push_back(int2string(id));
		line.push_back(float2string(p->coefficient));
		line.push_back(int2string(p->size));
		line.push_back(float2string(p->normSizeReduction));
		line.push_back(float2string(p->normDivergence));
		line.push_back(float2string(p->maxSizeReduction));
		line.push_back(float2string(p->maxDivergence));
		addCSVLine(file,line);

		id++;		
	}
	closeOutputCSV(file);
}


void OrderedSet::writeEachPartitionToCSV (vector<part> &partitions, string fileName) {

	vector<string> headLine;
	headLine.push_back("BEGIN_LABEL");
	headLine.push_back("END_LABEL");
	headLine.push_back("SIZE");
	headLine.push_back("VALUE");
	headLine.push_back("REF_VALUE");
	headLine.push_back("NORM_SIZE_REDUCTION");
	headLine.push_back("NORM_KL_DIVERGENCE");

	int id = 0;	
	for (vector<part>::iterator it = partitions.begin(); it != partitions.end(); ++it) {
		part *p = &(*it);
				
		stringstream ss; ss << id;
		string idStr = ss.str();

		ofstream file;
		openOutputCSV(file,fileName+"-PARTITION-"+idStr+".csv");
		addCSVLine(file,headLine);

		for (int i = 0; i < p->size; i++) {
			vector<string> line;
			line.push_back(p->beginLabels[i]);
			line.push_back(p->endLabels[i]);
			line.push_back(int2string(p->sizes[i]));
			line.push_back(float2string(p->values[i]));
			line.push_back(float2string(p->refValues[i]));
			line.push_back(float2string(p->normSizeReductions[i]));
			line.push_back(float2string(p->normDivergences[i]));
			addCSVLine(file,line);
		}
		closeOutputCSV(file);

		id++;		
	}
}

*/
