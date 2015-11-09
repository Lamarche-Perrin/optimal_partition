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



#include <math.h>
#include <list>
#include <iostream>

#include "nonconstrained_set.hpp"


NonconstrainedSet::NonconstrainedSet (int s)
{
	size = s;
	dataTree = new Datatree();
}



NonconstrainedSet::~NonconstrainedSet ()
{
	delete dataTree;
}


void NonconstrainedSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	dataTree->setObjectiveFunction(m);
}


void NonconstrainedSet::setRandom () {}


void NonconstrainedSet::buildDataStructure ()
{
	if (VERBOSE)
	{
		for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
		std::cout << "(" << VERBOSE_TAB << ") BUILD DATA TREE:" << std::endl;
		VERBOSE_TAB++;
	}

	Datatree *lastNode = buildPartsRec(dataTree);
	
	if (size > 1)
	{
		Datatree *node = dataTree->findChild(0);
		for (int vertex = 1; vertex < size; vertex++)
		{
			node = node->findChild(vertex);
			node->addBipartition(node->parent,dataTree->findChild(node->vertex));
		}

		node = dataTree->findChild(0);
		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
			buildPartitionsRec(*it);
	}

	dataTree->parent = lastNode;
	dataTree->addBipartition(lastNode,0);
	
	if (VERBOSE)
	{
		VERBOSE_TAB--;
		std::cout << std::endl;
	}
}


Datatree *NonconstrainedSet::buildPartsRec (Datatree *node)
{
	Datatree *lastNode = node;
	for (int vertex = node->vertex + 1; vertex < size; vertex++)
	{
		Datatree *n = buildPartsRec(node->addChild(vertex));
		if (lastNode == node) lastNode = n;
	}
	return lastNode;
}


void NonconstrainedSet::buildPartitionsRec (Datatree *node)
{
	for (BipartitionsSet::iterator it = node->parent->bipartitions->begin(); it != node->parent->bipartitions->end(); ++it)
	{
		Bipartition *p = *it;
		Datatree *n1 = p->first;
		Datatree *n2 = p->second;
		Datatree *n1p = n1->findChild(node->vertex);
		Datatree *n2p = n2->findChild(node->vertex);
		node->addBipartition(n1,n2p);
		node->addBipartition(n1p,n2);
	}

	for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
		buildPartitionsRec(*it);
}




void NonconstrainedSet::print ()
{
	for (TreesSet::iterator it = dataTree->children->begin(); it != dataTree->children->end(); ++it)
	{
		Datatree *node = *it;
		node->printVertices(false);
		std::cout << " -> ";
		node->value->print(false);
	}
}


void NonconstrainedSet::computeObjectiveValues () { objective->computeObjectiveValues(); dataTree->computeObjectiveValues(); }
void NonconstrainedSet::normalizeObjectiveValues () { dataTree->normalizeObjectiveValues(); }
void NonconstrainedSet::printObjectiveValues () { dataTree->printObjectiveValues(); }
void NonconstrainedSet::computeOptimalPartition (double parameter) { dataTree->computeOptimalPartition(parameter); }
void NonconstrainedSet::printOptimalPartition (double parameter) { dataTree->printOptimalPartition(parameter); }
Partition *NonconstrainedSet::getOptimalPartition (double parameter) { return dataTree->getOptimalPartition(parameter); }


void NonconstrainedSet::printDataTree (bool verbose) { dataTree->print(verbose); }
void NonconstrainedSet::printParts () { dataTree->printParts(); }
int NonconstrainedSet::printPartitions (bool print) { return dataTree->printPartitions(print); }
