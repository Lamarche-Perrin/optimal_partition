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

#include "hierarchical_set.hpp"


HierarchicalSet::HierarchicalSet (HNode *h)
{
	hierarchy = h;
}

void HierarchicalSet::setRandom () {}

HierarchicalSet::~HierarchicalSet ()
{
}


void HierarchicalSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	hierarchy->setObjectiveFunction(m);
}


void HierarchicalSet::buildDataStructure () { hierarchy->buildDataStructure(); }
void HierarchicalSet::print () { hierarchy->print(); }
void HierarchicalSet::computeObjectiveValues () { objective->computeObjectiveValues(); hierarchy->computeObjectiveValues(); }
void HierarchicalSet::normalizeObjectiveValues () { hierarchy->normalizeObjectiveValues(); }
void HierarchicalSet::printObjectiveValues () { hierarchy->printObjectiveValues(); }
void HierarchicalSet::computeOptimalPartition (double parameter) { hierarchy->computeOptimalPartition(parameter); }
void HierarchicalSet::printOptimalPartition (double parameter) { hierarchy->printOptimalPartition(parameter); }


Partition *HierarchicalSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	hierarchy->buildOptimalPartition(partition);	
	return partition;
}





HNode::HNode (int i)
{
	index = i;
	level = -1;
	indices = new std::set<int>();

	children = new HNodeSet();
}


HNode::~HNode ()
{
	delete indices;
	delete children;
}


void HNode::addChild (HNode *node) { children->insert(node); }


void HNode::buildDataStructure (HNode *r, int l, int n)
{
	if (r == 0) { root = this; } else { root = r; }
	level = l;
	size = 1;
	width = 0;
	
	if (index != -1) { indices->insert(index); width = 1; }
	
	for (HNodeSet::iterator it1 = children->begin(); it1 != children->end(); ++it1)
	{
		HNode *child = *it1;
		child->buildDataStructure(root,l+1,n);
		n = child->num+1;
		size += child->size;
		width += child->width;

		for (std::set<int>::iterator it2 = child->indices->begin(); it2 != child->indices->end(); ++it2) { indices->insert(*it2); }
	}
	
	num = n;
}


void HNode::print ()
{
	for (int i = 0; i < level; i++) { std::cout << "..."; }
	if (level > 0) { std::cout << " "; }

	printIndices();
	std::cout << " -> ";
	
	value->print(false);
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->print(); }
}


void HNode::printIndices (bool endl)
{
	std::cout << "{"; bool first = true;
	for (std::set<int>::iterator it = indices->begin(); it != indices->end(); ++it)
	{
		if (!first) { std::cout << ","; }
		std::cout << *it;
		first = false;
	}
	std::cout << "}";
	if (endl) { std::cout << std::endl; }
}


void HNode::setObjectiveFunction (ObjectiveFunction *m)
{
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->setObjectiveFunction(m); }

	objective = m;
	if (index == -1) { value = m->newObjectiveValue(); }
	else { value = m->newObjectiveValue(index); }
}


void HNode::computeObjectiveValues ()
{
	ObjectiveValueSet *qSet = new ObjectiveValueSet();
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		HNode *node = *it;
		node->computeObjectiveValues();
		qSet->insert(node->value);
	}

	if (index == -1) { value->compute(qSet); } else { value->compute(); }
	delete qSet;
}


void HNode::normalizeObjectiveValues (ObjectiveValue *maxQual)
{
	if (maxQual == 0) { maxQual = value; }

	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->normalizeObjectiveValues(maxQual); }
	value->normalize(maxQual);
}


void HNode::printObjectiveValues ()
{
	printIndices();
	std::cout << " -> ";
		
	value->print(true);
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->printObjectiveValues(); }
}


void HNode::computeOptimalPartition (double parameter)
{
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->computeOptimalPartition(parameter); }

	optimalValue = value->getValue(parameter);
	optimalCut = true;
	
	double value = 0;
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { value += (*it)->optimalValue; }
	
	if ((objective->maximize && value > optimalValue) || (!objective->maximize && value < optimalValue))
	{
		optimalValue = value;
		optimalCut = false;
	}
}


void HNode::printOptimalPartition (double parameter) {}


void HNode::buildOptimalPartition (Partition *partition)
{
	if (optimalCut)
	{
		Part *part = new Part(value);
		for (std::set<int>::iterator it = indices->begin(); it != indices->end(); ++it) { part->addIndividual(*it); }
		partition->addPart(part);
	}
	else { for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->buildOptimalPartition(partition); } }
}

