#include <iostream>

#include "nonconstrained_ordered_set.hpp"


NonconstrainedOrderedSet::NonconstrainedOrderedSet (int s1, int s2)
{
	size1 = s1;
	size2 = s2;

	dataTree = new OrderedDatatree(size2);
}

void NonconstrainedOrderedSet::setRandom () {}

NonconstrainedOrderedSet::~NonconstrainedOrderedSet ()
{
	delete dataTree;
}


void NonconstrainedOrderedSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	dataTree->setObjectiveFunction(m);
}


void NonconstrainedOrderedSet::buildDataStructure ()
{
	if (VERBOSE)
	{
		for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
		std::cout << "(" << VERBOSE_TAB << ") BUILD DATA TREE:" << std::endl;
		VERBOSE_TAB++;
	}

	OrderedDatatree *lastNode = buildPartsRec(dataTree);
	
	if (size1 > 1)
	{
		OrderedDatatree *node = dataTree->findChild(0);
		for (int vertex = 1; vertex < size1; vertex++)
		{
			node = node->findChild(vertex);
			node->addBipartition(node->parent,dataTree->findChild(node->vertex));
		}

		node = dataTree->findChild(0);
		for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
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


OrderedDatatree *NonconstrainedOrderedSet::buildPartsRec (OrderedDatatree *node)
{
	OrderedDatatree *lastNode = node;
	for (int vertex = node->vertex + 1; vertex < size1; vertex++)
	{
		OrderedDatatree *n = buildPartsRec(node->addChild(vertex));
		if (lastNode == node) lastNode = n;
	}
	return lastNode;
}


void NonconstrainedOrderedSet::buildPartitionsRec (OrderedDatatree *node)
{
	for (OrderedBipartitionsSet::iterator it = node->parent->bipartitions->begin(); it != node->parent->bipartitions->end(); ++it)
	{
		OrderedBipartition *p = *it;
		OrderedDatatree *n1 = p->first;
		OrderedDatatree *n2 = p->second;
		OrderedDatatree *n1p = n1->findChild(node->vertex);
		OrderedDatatree *n2p = n2->findChild(node->vertex);
		node->addBipartition(n1,n2p);
		node->addBipartition(n1p,n2);
	}

	for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
		buildPartitionsRec(*it);
}



void NonconstrainedOrderedSet::print ()
{
	for (OrderedTreesSet::iterator it = dataTree->children->begin(); it != dataTree->children->end(); ++it)
	{
		OrderedDatatree *node = *it;

		for (int i = 0; i < size2; i++)
		{
			for (int j = 0; j < size2-i; j++)
			{
				node->printVertices(false);
				//if (j == 0) { std::cout << " x [" << i << "]"; }
				std::cout << " x [" << i << "," << (i+j) << "]";
				
				if (node->objective != 0)
				{
					std::cout << " -> ";
					node->qualities[node->getIndex(i,j)]->print(false);
				} else { std::cout << std::endl; }
			}
		}
	}
}


void NonconstrainedOrderedSet::computeObjectiveValues () { objective->computeObjectiveValues(); dataTree->computeObjectiveValues(); }
void NonconstrainedOrderedSet::normalizeObjectiveValues () { dataTree->normalizeObjectiveValues(); }
void NonconstrainedOrderedSet::printObjectiveValues () { dataTree->printObjectiveValues(); }
void NonconstrainedOrderedSet::computeOptimalPartition (double parameter) { dataTree->computeOptimalPartition(parameter); }
void NonconstrainedOrderedSet::printOptimalPartition (double parameter) { dataTree->printOptimalPartition(parameter); }

Partition *NonconstrainedOrderedSet::getOptimalPartition (double parameter)
{
	std::cout << parameter << std::endl;
	return dataTree->getOptimalPartition(parameter);
}

/*
Partition *NonconstrainedOrderedSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	dataTree->buildOptimalPartition(partition);	
	return partition;
}

*/

