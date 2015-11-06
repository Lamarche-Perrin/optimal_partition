
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
