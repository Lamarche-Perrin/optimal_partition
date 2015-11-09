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

#include "hierarchical_hierarchical_set.hpp"


HierarchicalHierarchicalSet::HierarchicalHierarchicalSet (HNode *h1, HNode *h2)
{
	hierarchy1 = h1;
	hierarchy2 = h2;
}


void HierarchicalHierarchicalSet::setRandom () {}


HierarchicalHierarchicalSet::~HierarchicalHierarchicalSet ()
{
	if (hyperarchy != 0) { delete hyperarchy; }
}


void HierarchicalHierarchicalSet::setObjectiveFunction (ObjectiveFunction *m)
{
	objective = m;
	hyperarchy->setObjectiveFunction(m);
}


void HierarchicalHierarchicalSet::buildDataStructure ()
{
	hierarchy1->buildDataStructure();
	hierarchy2->buildDataStructure();
	HHNode *nodeArray [hierarchy1->size * hierarchy2->size];
	int mult = hierarchy1->size;

	std::list<HNode*> nodeList1;
	nodeList1.push_back(hierarchy1);
	
	while (!nodeList1.empty())
	{
		HNode *node1 = nodeList1.front();
		nodeList1.pop_front();
		
		std::list<HNode*> nodeList2;
		nodeList2.push_back(hierarchy2);

		while (!nodeList2.empty())
		{
			HNode *node2 = nodeList2.front();
			nodeList2.pop_front();
			
			nodeArray[node1->num + node2->num * mult] = new HHNode(node1,node2);
			
			/*
			std::cout << node1->num << "/" << node2->num << " -> ";
			nodeArray[node1->num + node2->num * mult]->printIndices();
			std::cout << std::endl;
			*/
			
			for (HNodeSet::iterator it = node2->children->begin(); it != node2->children->end(); ++it) { nodeList2.push_back(*it); }
		}

		for (HNodeSet::iterator it = node1->children->begin(); it != node1->children->end(); ++it) { nodeList1.push_back(*it); }
	}

	for (int i = 0; i < hierarchy1->size; i++)
	{
		for (int j = 0; j < hierarchy2->size; j++)
		{
			HHNode *hypernode = nodeArray[i+j*mult];
//			hypernode->printIndices();
			
//			std::cout << std::endl << "Children 1:" << std::endl;
			for (HNodeSet::iterator it = hypernode->node1->children->begin(); it != hypernode->node1->children->end(); ++it)
			{
				HHNode *child = nodeArray[(*it)->num + hypernode->node2->num * mult];
				hypernode->addChild1(child);
//				child->printIndices();
//				std::cout << std::endl;
			}
			
//			std::cout << std::endl << "Children 2:" << std::endl;
			for (HNodeSet::iterator it = hypernode->node2->children->begin(); it != hypernode->node2->children->end(); ++it)
			{
				HHNode *child = nodeArray[hypernode->node1->num + (*it)->num * mult];
				hypernode->addChild2(child);
//				child->printIndices();
//				std::cout << std::endl;
			}
		}
	}

	hyperarchy = nodeArray[hierarchy1->num + hierarchy2->num * mult];
	hyperarchy->buildDataStructure();
}


void HierarchicalHierarchicalSet::print () { hyperarchy->print(); }
void HierarchicalHierarchicalSet::computeObjectiveValues () { objective->computeObjectiveValues(); hyperarchy->computeObjectiveValues(); }
void HierarchicalHierarchicalSet::normalizeObjectiveValues () { hyperarchy->normalizeObjectiveValues(); }
void HierarchicalHierarchicalSet::printObjectiveValues () { hyperarchy->printObjectiveValues(); }
void HierarchicalHierarchicalSet::computeOptimalPartition (double parameter) { hyperarchy->computeOptimalPartition(parameter); }
void HierarchicalHierarchicalSet::printOptimalPartition (double parameter) { hyperarchy->printOptimalPartition(parameter); }


Partition *HierarchicalHierarchicalSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	hyperarchy->buildOptimalPartition(partition);	
	return partition;
}





HHNode::HHNode (HNode *n1, HNode *n2)
{
	node1 = n1;
	node2 = n2;

	children1 = new HHNodeSet();
	children2 = new HHNodeSet();
}


HHNode::~HHNode ()
{
	delete children1;
	delete children2;
}


void HHNode::addChild1 (HHNode *node) { children1->insert(node); }
void HHNode::addChild2 (HHNode *node) { children2->insert(node); }


void HHNode::buildDataStructure () {}


void HHNode::print ()
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->print(); }

	for (int i = 0; i < node1->level; i++) { std::cout << "..."; }
	if (node1->level > 0) { std::cout << " "; }

	printIndices();
	
	if (value != 0) { std::cout << " -> "; value->print(false); } else { std::cout << std::endl; }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->print(); }

	if (node1->level == 0) { std::cout << std::endl; }
}


void HHNode::printIndices (bool endl)
{
	node1->printIndices();
	std::cout << " x ";
	node2->printIndices(endl);
}


void HHNode::setObjectiveFunction (ObjectiveFunction *m)
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->setObjectiveFunction(m); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->setObjectiveFunction(m); }

	objective = m;
	if (node1->index == -1 || node2->index == -1) { value = m->newObjectiveValue(); }
	else { value = m->newObjectiveValue(node1->index + node2->index * node1->root->width); }
}


void HHNode::computeObjectiveValues ()
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->computeObjectiveValues(); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->computeObjectiveValues(); }

	if (node1->index != -1) {
		if (node2->index != -1) { value->compute(); }
		else {
			ObjectiveValueSet *qSet = new ObjectiveValueSet();
			for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { qSet->insert((*it)->value); }
			value->compute(qSet);
			delete qSet;
		}
	} else {
		ObjectiveValueSet *qSet = new ObjectiveValueSet();
		for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { qSet->insert((*it)->value); }
		value->compute(qSet);
		delete qSet;
	}
}


void HHNode::normalizeObjectiveValues (ObjectiveValue *maxQual)
{
	if (maxQual == 0) { maxQual = value; }

	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->normalizeObjectiveValues(maxQual); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->normalizeObjectiveValues(maxQual); }

	value->normalize(maxQual);
}


void HHNode::printObjectiveValues ()
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->printObjectiveValues(); }

	for (int i = 0; i < node1->level; i++) { std::cout << "..."; }
	if (node1->level > 0) { std::cout << " "; }

	printIndices();
	std::cout << " -> ";
	value->print(true);
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->printObjectiveValues(); }

	if (node1->level == 0) { std::cout << std::endl; }
}


void HHNode::computeOptimalPartition (double parameter)
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->computeOptimalPartition(parameter); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->computeOptimalPartition(parameter); }

	optimalValue = value->getValue(parameter);
	optimalCut = 0;
	
	if (!children1->empty())
	{
		double value1 = 0;
		for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { value1 += (*it)->optimalValue; }
		if ((objective->maximize && value1 > optimalValue) || (!objective->maximize && value1 < optimalValue))
		{
			optimalValue = value1;
			optimalCut = 1;
		}
	}

	if (!children2->empty())
	{
		double value2 = 0;
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { value2 += (*it)->optimalValue; }	
		if ((objective->maximize && value2 > optimalValue) || (!objective->maximize && value2 < optimalValue))
		{
			optimalValue = value2;
			optimalCut = 2;
		}
	}
}


void HHNode::printOptimalPartition (double parameter) {}


void HHNode::buildOptimalPartition (Partition *partition)
{
	if (optimalCut == 0)
	{
		Part *p1 = new Part();
		Part *p2 = new Part();
		BiPart *part = new BiPart(p1,p2,value);

		for (std::set<int>::iterator it = node1->indices->begin(); it != node1->indices->end(); ++it) { p1->addIndividual(*it); }
		for (std::set<int>::iterator it = node2->indices->begin(); it != node2->indices->end(); ++it) { p2->addIndividual(*it); }

		partition->addPart(part,true);
	}
	
	if (optimalCut == 1)
		for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->buildOptimalPartition(partition); }
		
	if (optimalCut == 2)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->buildOptimalPartition(partition); }
}

