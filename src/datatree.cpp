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
#include <iomanip>
#include <sstream>

#include "datatree.hpp"



Datatree::Datatree (int v)
{
	vertex = v;	
	objective = 0;

	parent = 0;
	complement = 0;
	complementList = new TreesList();
	children = new TreesSet();
	bipartitions = new BipartitionsSet();
	value = 0;

	optimized = false;
	wholeSet = false;
}


Datatree::Datatree (Datatree &tree)
{
	vertex = tree.vertex;
	objective = tree.objective;

	parent = 0;
	complement = 0;
	complementList = new TreesList();
	children = new TreesSet();
	bipartitions = new BipartitionsSet();
	value = 0;

	optimized = false;
	wholeSet = false;
	
	for (TreesSet::iterator it = tree.children->begin(); it != tree.children->end(); ++it)
	{
		Datatree *child = new Datatree (**it);
		children->insert(children->end(),child);
		child->parent = this;
	}
}

Datatree::~Datatree()
{
	if (optimized) delete optimalBipartition;
	
	for (TreesSet::iterator it = children->begin(); it != children->end(); ++it) { delete *it; }
	delete children;
	delete complementList;

	for (BipartitionsSet::iterator it = bipartitions->begin(); it != bipartitions->end(); ++it) { delete *it; }
	delete bipartitions;
}


Vertices *Datatree::getAllVertices ()
{
	Vertices *v = new Vertices();
	Datatree *currentNode = this;
	while (currentNode->vertex != -1)
	{
		v->insert(currentNode->vertex);
		currentNode = currentNode->parent;
	}
	return v;
}



void Datatree::addBipartition (Datatree *n1, Datatree *n2)
{
	if (VERBOSE)
	{
		for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
		std::cout << "(" << VERBOSE_TAB << ") PARTITION ";
		if (n1 != 0) n1->printVertices(false);
		if (n2 != 0) n2->printVertices(false);
		std::cout << " ADDED" << std::endl;
	}

	bipartitions->insert(new Bipartition(n1,n2));
}


Datatree *Datatree::addChild (int v, bool print)
{
	Datatree *child = new Datatree(v);
	children->insert(children->end(),child);
	child->parent = this;

	if (VERBOSE && print)
	{
		for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
		std::cout << "(" << VERBOSE_TAB << ") PART ";
		child->printVertices(false);
		std::cout << " ADDED" << std::endl;
	}

	return child;
}



Datatree *Datatree::findChild (int v)
{
	for (TreesSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		if ((*it)->vertex == v) return *it;
	}
	
	std::cout << "WARNING: node {" << v << "} not found after node ";
	printVertices(false);
	std::cout << "!" << std::endl;
	return 0;
}


Datatree *Datatree::findOrAddChild (int v, bool print)
{
	for (TreesSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		if ((*it)->vertex == v) return *it;
	} // REPLACE BY findChild(v)
		
	return addChild(v,print);
}


/*
Datatree *Datatree::findNode (Vertices *v)
{
	Datatree *currentNode = this;
	
	Datatree *nextNode = 0; int minOrder;
	
	for (TreesSet::iterator it = currentNode->children->begin(); it != currentNode->children->end(); it++)
	{
		Datatree *child = *it;
		if (std::include(v->begin(),v->end(),child->vertices->begin()),child->vertices->end())
		{
			if (nextNode == 0) { nextNode = child; }
			else {
				if (order->
			}
		}
	}
}
*/



/*
Datatree *Datatree::findChild (Vertices *V)
{
	Datatree *currentNode = this;
	Datatree *nextNode;

	for (Vertices::iterator it = V->begin(); it != V->end(); ++it)
	{
		int nextVertex = *it;
		nextNode = currentNode->findChild(nextVertex);
		if (nextNode != 0) { currentNode = nextNode; } else { return 0; }
	}
	return currentNode;
}
*/

/*
Datatree *Datatree::findOrBuildChild (int v, bool connected)
{
	for (TreesSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		if ((*it)->vertex == v) return *it;
	}
	return addChild(v,connected);
}


Datatree *Datatree::findOrBuildChild (Vertices *V)
{
	Datatree *currentNode = this;
	for (Vertices::iterator it = V->begin(); it != V->end(); ++it)
	{
		int nextVertex = *it;
		currentNode = currentNode->findOrBuildChild(nextVertex,false);
	}
	currentNode->connected = true;
	return currentNode;
}


Datatree *Datatree::findOrBuildChild (Datatree *n)
{
	Datatree *currentNode = this;
	while (n->vertex != -1)
	{
		int nextVertex = n->vertex;
		currentNode = currentNode->findOrBuildChild(nextVertex,false);
		n = n->parent;
	}
	currentNode->connected = true;
	return currentNode;
}
*/

void Datatree::setObjectiveFunction (ObjectiveFunction *m)
{
	std::list<Datatree*> *list = new std::list<Datatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		Datatree *node = list->front();
		list->pop_front();

		if (node->vertex == -1) { node->value = m->newObjectiveValue(); }
		else if (node->parent->vertex == -1) { node->value = m->newObjectiveValue(node->vertex); }
		else { node->value = m->newObjectiveValue(); }
		
		node->objective = m;

		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void Datatree::computeObjectiveValues ()
{
	std::list<Datatree*> *list = new std::list<Datatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		Datatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			if (node->parent->vertex == -1)
				node->value->compute();
			
			else if (node->vertex != -1)
			{
				ObjectiveValue *q1 = findChild(node->vertex)->value;
				ObjectiveValue *q2 = node->parent->value;
				
				node->value->compute(q1,q2);
			}
		}
		
		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void Datatree::normalizeObjectiveValues (ObjectiveValue *maxObjectiveValue)
{
	ObjectiveValue *maxQual = maxObjectiveValue; 
	if (maxQual == 0) { maxQual = parent->value; }

	std::list<Datatree*> *list = new std::list<Datatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		Datatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1) { node->value->normalize(maxQual); }

		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}



void Datatree::printObjectiveValues ()
{
	std::list<Datatree*> *list = new std::list<Datatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		Datatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			node->printVertices(false);
			std::cout << " -> ";
			node->value->print(true);
		}

		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void Datatree::computeOptimalPartition (double parameter)
{
	if (VERBOSE) { std::cout << "parameter = " << std::setprecision(10) << parameter << std::endl; }

	std::list<Datatree*> *list = new std::list<Datatree*>();
	list->push_back(findChild(0)->findChild(1));
	
	while (!list->empty())
	{
		Datatree *node = list->front();
		list->pop_front();

		node->optimalBipartition = new Bipartition();
		node->optimalBipartition->first = this;
		node->optimalBipartition->second = node;
		
		node->optimalValue = node->value->getValue(parameter);

		for (BipartitionsSet::iterator it = node->bipartitions->begin(); it != node->bipartitions->end(); ++it)
		{
			Bipartition *p = *it;
			Datatree *n1 = p->first;
			Datatree *n2 = p->second;
			
			double value = 0;

			if (n1->optimized) { value += n1->optimalValue; }
			else { value += n1->value->getValue(parameter); }

			if (n2->optimized) { value += n2->optimalValue; }
			else { value += n2->value->getValue(parameter); }
			
			if ((objective->maximize && value > node->optimalValue) || (!objective->maximize && value < node->optimalValue))
			{
				node->optimalBipartition->first = n1;
				node->optimalBipartition->second = n2;
				node->optimalValue = value;
			}
		}

		node->optimized = true;
		
		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void Datatree::printOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);

	std::list<Datatree*> *optimalPartition = new std::list<Datatree*>();

	Datatree *node = parent;
	
	while (node != 0)
	{
		Bipartition *p = node->optimalBipartition;
		Datatree *n1 = p->first;
		Datatree *n2 = p->second;
		
		Datatree *nextNode = 0;

		if (n1->optimized && n1 != node) { nextNode = n1; }
		else if (n1->vertex != -1) { optimalPartition->push_front(n1); }
		
		if (n2->optimized && n2 != node) { nextNode = n2; }
		else if (n2->vertex != -1) { optimalPartition->push_front(n2); }

		node = nextNode;
	}
	
	for (std::list<Datatree*>::iterator it = optimalPartition->begin(); it != optimalPartition->end(); ++it)
	{
		(*it)->printVertices(false);
	}
	
	std::cout << std::endl;
}


Partition *Datatree::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);

	Partition *partition = new Partition(objective,parameter);
	
	Datatree *node = parent;
	
	while (node != 0)
	{
		Bipartition *p = node->optimalBipartition;
		Datatree *n1 = p->first;
		Datatree *n2 = p->second;
		
		Datatree *nextNode = 0;

		if (n2->optimized && n2 != node) { nextNode = n2; }
		else if (n2->vertex != -1)
		{
			Part *part = new Part(n2->value);
			Datatree *n = n2;
			while (n->vertex != -1)
			{
				part->addIndividual(n->vertex,true);
				n = n->parent;
			}
			partition->addPart(part,true);
		}

		if (n1->optimized && n1 != node) { nextNode = n1; }
		else if (n1->vertex != -1)
		{
			Part *part = new Part(n1->value);
			Datatree *n = n1;
			while (n->vertex != -1)
			{
				part->addIndividual(n->vertex,true);
				n = n->parent;
			}
			partition->addPart(part,true);
		}
		
		node = nextNode;
	}
	
	return partition;
}



void Datatree::print (bool verbose)
{
	std::cout << "PRINTING DATA TREE:" << std::endl;
	printRec(0,verbose);
	std::cout << std::endl;
}


void Datatree::printRec (int p, bool verbose)
{
	for (int i = 0; i < p; i++) { std::cout << "---"; }
		
	if (vertex == -1) { std::cout << " ROOT "; }
	else { std::cout << " PART "; printVertices(false); }
		
	if (complement != 0)
	{
		std::cout << " comp = ";
		complement->printVertices(false);
	}
	else { std::cout << " no complement"; }
		
		
	if (optimized)
	{
		std::cout << " opt = ";
		optimalBipartition->first->printVertices();
		optimalBipartition->second->printVertices();
	}
	std::cout << std::endl;
		
	if (verbose)
	{
		for (BipartitionsSet::iterator it = bipartitions->begin(); it != bipartitions->end(); ++it)
		{
			for (int i = 0; i < p; i++) { std::cout << "---"; }
			std::cout << " PARTITION ";
			Bipartition *bipartition = *it;
			bipartition->first->printVertices(false);
			if (bipartition->second != 0) { bipartition->second->printVertices(false); }
			std::cout << std::endl;
		}
	}
		
	for (TreesSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->printRec(p+1,verbose); }
}



void Datatree::printParts ()
{
	std::cout << "COUNTING PARTS:" << std::endl;

	PartSet *set = getParts();
	for (PartSet::iterator it = set->begin(); it != set->end(); ++it) { (*it)->print(); std::cout << std::endl; }

	std::cout << "-> " << set->size() << " PARTS\n" << std::endl;
}



PartSet *Datatree::getParts ()
{
	PartSet *partSet = new PartSet();
	
	std::list< std::pair<Datatree*,Part*> > *list = new std::list< std::pair<Datatree*,Part*> >();
	list->push_back(std::make_pair(this,new Part()));
	
	while (!list->empty())
	{
		Datatree *node = list->front().first;
		Part *part = list->front().second;
		list->pop_front();
		
		if (node->vertex != -1)
		{
			part->addIndividual(node->vertex);
			partSet->insert(part);
		}
		
		for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
		{ list->push_back(std::make_pair(*it,new Part(part))); }
	}
	
	delete list;

	return partSet;
}



PartitionList *Datatree::getAllPartitions ()
{
	PartitionList *partitionList = new PartitionList();
	
	std::list< std::pair<Datatree*,Partition*> > *list = new std::list< std::pair<Datatree*,Partition*> >();
	list->push_back(std::make_pair(this->parent,new Partition()));
	
	while (!list->empty())
	{
		Datatree *node = list->front().first;
		Partition *partition = list->front().second;
		list->pop_front();
		
		node->printVertices(false);
		partition->print();
		std::cout << std::endl;
		
		for (BipartitionsSet::iterator it = node->bipartitions->begin(); it != node->bipartitions->end(); ++it)
		{
			Bipartition *p = *it;
			Datatree *n1 = p->first;
			Datatree *n2 = p->second;
			
			if (!n1->bipartitions->empty())
			{
				Partition *newPartition = new Partition (partition);
				Part *newPart = new Part(n2);
				newPartition->addPart(newPart,true);
				list->push_back(std::make_pair(n1,newPartition));
			}
			
			else {
				Partition *newPartition = new Partition (partition);
				Part *newPart = new Part(n1);
				newPartition->addPart(newPart,true);
				list->push_back(std::make_pair(n2,newPartition));
			}
			
			if (!n1->bipartitions->empty() && !n2->bipartitions->empty())
			{ std::cout << "WARNING: bipartition recursively defined on both subpartitions!" << std::endl; }

		}

		Part *part = new Part(node);
		partition->addPart(part,true);
		partitionList->push_back(partition);
	}
	
	delete list;
	
	std::cout << std::endl;
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it) { (*it)->print(); std::cout << std::endl; }
	
	return partitionList;
}


int Datatree::printPartitions (bool print)
{
	if (print) { std::cout << "COUNTING PARTITIONS:" << std::endl; }
	int partitionsNb = 0;
	
	std::list<Datatree*> *treeList = new std::list<Datatree*>();
	std::list<std::string> *strList = new std::list<std::string>();

	treeList->push_back(this);
	strList->push_back(std::string(""));
	
	while (!treeList->empty())
	{
		Datatree *node = treeList->front();
		treeList->pop_front();

		std::string str = strList->front();
		strList->pop_front();

		for (BipartitionsSet::iterator it = node->bipartitions->begin(); it != node->bipartitions->end(); ++it)
		{
			Bipartition *p = *it;
			std::string nextStr1 = str;
			std::string nextStr2 = str;
			
			if (print)
			{
				std::cout << "{";
				p->first->printVertices(false);
				if (p->second != 0)
				{
					std::cout << ",";
					p->second->printVertices(false);
					nextStr1 = std::string(",") + p->second->toString() + str;
				}
				std::cout << str << "}" << std::endl;
			}
			partitionsNb++;

			treeList->push_back(p->first);
			strList->push_back(nextStr1);
			
			if (p->second != 0)
			{
				nextStr2 = std::string(",") + p->first->toString() + str;
				treeList->push_back(p->second);
				strList->push_back(nextStr2);
			}
		}
	}

	delete treeList;
	delete strList;
	if (print) { std::cout << "-> " << partitionsNb << " PARTITIONS\n" << std::endl; }

	return partitionsNb;
}


std::string Datatree::toString ()
{
	if (vertex == -1) { return std::string("{}"); }
	else {
		std::string str = toStringRec();
		return std::string("{") + str + std::string("}");
	}
}


std::string Datatree::toStringRec ()
{
	std::string str;
	if (parent->vertex != -1)
	{
		str = parent->toStringRec() + std::string(",");
	}
	
	std::ostringstream ss;
	ss << vertex;
	str += ss.str();
	
	return str;
}


void Datatree::printVertices (bool endl)
{
	if (vertex == -1) { std::cout << "{}"; }
	else {
		std::cout << "{";
		printVerticesRec();
		std::cout << "}";
	}
	if (endl) std::cout << std::endl;
}


void Datatree::printVerticesRec ()
{
	if (parent != 0 && parent->vertex != -1)
	{
		parent->printVerticesRec();
		std::cout << ",";
	}
	std::cout << vertex;
}









OrderedDatatree::OrderedDatatree (int s, int v)
{
	vertex = v;	
	objective = 0;

	parent = 0;
	complement = 0;
	complementList = new OrderedTreesList();
	children = new OrderedTreesSet();
	bipartitions = new OrderedBipartitionsSet();

	size1 = 0;
	size2 = s;
	int s2 = (s+1)*s/2;
	
	qualities = new ObjectiveValue* [s2];
	optimalValues = new double [s2];
	optimalCuts = new int [s2];
	optimalBipartitions = new OrderedBipartition* [s2];

	for (int i = 0; i < s2; i++)
	{
		qualities[i] = 0;
		optimalBipartitions[i] = 0;
	}

	optimized = false;
	wholeSet = false;
}


OrderedDatatree::OrderedDatatree (OrderedDatatree &tree)
{
	vertex = tree.vertex;
	objective = tree.objective;

	parent = 0;
	complement = 0;
	complementList = new OrderedTreesList();
	children = new OrderedTreesSet();
	bipartitions = new OrderedBipartitionsSet();

	size1 = tree.size1;
	size2 = tree.size2;
	int s2 = (size2+1)*size2/2;
	
	qualities = new ObjectiveValue* [s2];
	optimalValues = new double [s2];
	optimalCuts = new int [s2];
	optimalBipartitions = new OrderedBipartition* [s2];

	optimized = false;
	wholeSet = false;
	
	for (OrderedTreesSet::iterator it = tree.children->begin(); it != tree.children->end(); ++it)
	{
		OrderedDatatree *child = new OrderedDatatree (**it);
		children->insert(children->end(),child);
		child->parent = this;
	}
}



OrderedDatatree::~OrderedDatatree()
{
	for (OrderedTreesSet::iterator it = children->begin(); it != children->end(); ++it) { delete *it; }
	for (OrderedBipartitionsSet::iterator it = bipartitions->begin(); it != bipartitions->end(); it++) { delete *it; }

	delete children;
	delete complementList;
	delete bipartitions;
	
	if (vertex != -1)
	{
		for (int i = 0; i < (size2+1)*size2/2; i++)
		{
			if (qualities[i] != 0) { delete qualities[i]; }
			if (optimalBipartitions[i] != 0) { delete optimalBipartitions[i]; }
		}
	}

	delete [] qualities;
	delete [] optimalValues;
	delete [] optimalCuts;
	delete [] optimalBipartitions;
}


Vertices *OrderedDatatree::getAllVertices ()
{
	Vertices *v = new Vertices();
	OrderedDatatree *currentNode = this;
	while (currentNode->vertex != -1)
	{
		v->insert(currentNode->vertex);
		currentNode = currentNode->parent;
	}
	return v;
}

int OrderedDatatree::getIndex (int i, int j) { return j*size2-j*(j-1)/2+i; }


void OrderedDatatree::addBipartition (OrderedDatatree *n1, OrderedDatatree *n2)
{
	if (VERBOSE)
	{
		for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
		std::cout << "(" << VERBOSE_TAB << ") PARTITION ";
		if (n1 != 0) n1->printVertices(false);
		if (n2 != 0) n2->printVertices(false);
		std::cout << " ADDED" << std::endl;
	}

	bipartitions->insert(new OrderedBipartition(n1,n2));
}


OrderedDatatree *OrderedDatatree::addChild (int v, bool print)
{
	size1++;
	OrderedDatatree *child = new OrderedDatatree(size2,v);
	children->insert(children->end(),child);
	child->parent = this;

	if (VERBOSE && print)
	{
		for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
		std::cout << "(" << VERBOSE_TAB << ") PART ";
		child->printVertices(false);
		std::cout << " ADDED" << std::endl;
	}

	return child;
}



OrderedDatatree *OrderedDatatree::findChild (int v)
{
	for (OrderedTreesSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		if ((*it)->vertex == v) return *it;
	}
	
	std::cout << "WARNING: node {" << v << "} not found after node ";
	printVertices(false);
	std::cout << "!" << std::endl;
	return 0;
}


OrderedDatatree *OrderedDatatree::findOrAddChild (int v, bool print)
{
	for (OrderedTreesSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		if ((*it)->vertex == v) return *it;
	} // REPLACE BY findChild(v)
		
	return addChild(v,print);
}


void OrderedDatatree::setObjectiveFunction (ObjectiveFunction *m)
{
	std::list<OrderedDatatree*> *list = new std::list<OrderedDatatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		OrderedDatatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			if (node->parent->vertex == -1)
			{
				for (int i = 0; i < size2; i++)
					node->qualities[getIndex(i,0)] = m->newObjectiveValue(node->vertex * node->parent->size2 + i);
			
				for (int j = 1; j < size2; j++)
					for (int i = 0; i < size2-j; i++)
						node->qualities[getIndex(i,j)] = m->newObjectiveValue();
			}
	
			else {
				for (int j = 0; j < size2; j++)
					for (int i = 0; i < size2-j; i++)
						node->qualities[getIndex(i,j)] = m->newObjectiveValue();
			}
		}
		
		node->objective = m;

		for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}



void OrderedDatatree::computeObjectiveValues ()
{
	std::list<OrderedDatatree*> *list = new std::list<OrderedDatatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		OrderedDatatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			if (node->parent->vertex == -1)
				for (int i = 0; i < size2; i++)
					node->qualities[getIndex(i,0)]->compute();		
			
			else if (node->vertex != -1)
			{
				for (int i = 0; i < size2; i++)
				{
					ObjectiveValue *q1 = findChild(node->vertex)->qualities[getIndex(i,0)];
					ObjectiveValue *q2 = node->parent->qualities[getIndex(i,0)];
					
					node->qualities[getIndex(i,0)]->compute(q1,q2);
				}
			}

			for (int j = 1; j < size2; j++)
				for (int i = 0; i < size2-j; i++)
					node->qualities[getIndex(i,j)]->compute(node->qualities[getIndex(i,j-1)],node->qualities[getIndex(i+j,0)]);
		}
		
		for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void OrderedDatatree::normalizeObjectiveValues (ObjectiveValue *maxObjectiveValue)
{
	ObjectiveValue *maxQual = maxObjectiveValue; 
	if (maxQual == 0) { maxQual = parent->qualities[getIndex(0,size2-1)]; }

	std::list<OrderedDatatree*> *list = new std::list<OrderedDatatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		OrderedDatatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			for (int j = 0; j < size2; j++)
				for (int i = 0; i < size2-j; i++)
					node->qualities[getIndex(i,j)]->normalize(maxQual);
		}

		for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}



void OrderedDatatree::printObjectiveValues ()
{
	std::list<OrderedDatatree*> *list = new std::list<OrderedDatatree*>();
	list->push_back(this);
	
	while (!list->empty())
	{
		OrderedDatatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			for (int j = 0; j < size2; j++)
				for (int i = 0; i < size2-j; i++)
				{
					node->printVertices(false);
					std::cout << " x ";		
					std::cout << "[" << i << "," << j << "] -> ";
					node->qualities[getIndex(i,j)]->print(true);
				}

			/*
			for (OrderedBipartitionsSet::iterator it = node->bipartitions->begin(); it != node->bipartitions->end(); ++it)
			{
				OrderedBipartition *p = *it;
				OrderedDatatree *n1 = p->first;
				OrderedDatatree *n2 = p->second;
				n1->printVertices(false);
				n2->printVertices(true);
			}
			*/
		}

		for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void OrderedDatatree::computeOptimalPartition (double parameter)
{
	if (VERBOSE) { std::cout << "parameter = " << std::setprecision(10) << parameter << std::endl; }

	std::list<OrderedDatatree*> *list = new std::list<OrderedDatatree*>();
	list->push_back(this);
//	list->push_back(findChild(0)->findChild(1));
	
	while (!list->empty())
	{
		OrderedDatatree *node = list->front();
		list->pop_front();

		if (node->vertex != -1)
		{
			for (int i = size2-1; i >= 0; i--)
			{
				for (int j = 0; j < size2-i; j++)
				{
					if (VERBOSE)
					{
						node->printVertices(false);
						if (j == 0) { std::cout << " x [" << i << "]"; }
						else { std::cout << " x [" << i << "," << (i+j) << "]"; }
					}

					node->optimalBipartitions[getIndex(i,j)] = new OrderedBipartition();
					node->optimalValues[getIndex(i,j)] = node->qualities[getIndex(i,j)]->getValue(parameter);
					node->optimalCuts[getIndex(i,j)] = j;
				
					for (OrderedBipartitionsSet::iterator it = node->bipartitions->begin(); it != node->bipartitions->end(); ++it)
					{
						OrderedBipartition *p = *it;
						OrderedDatatree *n1 = p->first;
						OrderedDatatree *n2 = p->second;
							
						double value = 0;
				
						if (n1->optimized) { value += n1->optimalValues[getIndex(i,j)]; }
						else { value += n1->qualities[getIndex(i,j)]->getValue(parameter); }
				
						if (n2->optimized) { value += n2->optimalValues[getIndex(i,j)]; }
						else { value += n2->qualities[getIndex(i,j)]->getValue(parameter); }
							
						if ((objective->maximize && value > node->optimalValues[getIndex(i,j)])
						|| (!objective->maximize && value < node->optimalValues[getIndex(i,j)]))
						{
							node->optimalBipartitions[getIndex(i,j)]->first = n1;
							node->optimalBipartitions[getIndex(i,j)]->second = n2;
							node->optimalValues[getIndex(i,j)] = value;
							node->optimalCuts[getIndex(i,j)] = -1;
						}
					}
					
					for (int cut = 0; cut < j; cut++)
					{
						double value = node->optimalValues[getIndex(i,cut)] + node->optimalValues[getIndex(i+cut+1,j-cut-1)];
						if ((objective->maximize && value > node->optimalValues[getIndex(i,j)])
						|| (!objective->maximize && value < node->optimalValues[getIndex(i,j)]))
						{
							node->optimalValues[getIndex(i,j)] = value;
							node->optimalCuts[getIndex(i,j)] = cut;
						}
					}

					if (node->optimalCuts[getIndex(i,j)] != -1)
					{
						delete node->optimalBipartitions[getIndex(i,j)];
						node->optimalBipartitions[getIndex(i,j)] = 0;
					}
					
					if (VERBOSE)
					{
						if (node->optimalCuts[getIndex(i,j)] == -1)
						{
							std::cout << " -> spacial cut = ";
							node->optimalBipartitions[getIndex(i,j)]->first->printVertices(false);
							node->optimalBipartitions[getIndex(i,j)]->second->printVertices(false);
						}
						
						else if (node->optimalCuts[getIndex(i,j)] < j)
						{
							std::cout << " -> temporal cut = ";
							if (node->optimalCuts[getIndex(i,j)] == 0) { std::cout << "[" << i << "]"; }
							else { std::cout << "[" << i << "," << (i+node->optimalCuts[getIndex(i,j)]) << "]"; }
							
							if (1+node->optimalCuts[getIndex(i,j)] == j) { std::cout << "[" << (i+1+node->optimalCuts[getIndex(i,j)]) << "]"; }
							else { std::cout << "[" << (i+1+node->optimalCuts[getIndex(i,j)]) << "," << (i+j) << "]"; }
						}
						
						else { std::cout << " -> no cut"; }
						
						std::cout << " (" << node->optimalValues[getIndex(i,j)] << ")" << std::endl;
					}
				}
			}
		}

		node->optimized = true;
		
		for (OrderedTreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
	}
}


void OrderedDatatree::printOptimalPartition (double parameter) {}

/*
Partition *OrderedDatatree::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);

	Partition *partition = new Partition(objective,parameter);
	
	OrderedDatatree *node = parent;
	
	while (node != 0)
	{
		OrderedBipartition *p = node->optimalBipartition;
		OrderedDatatree *n1 = p->first;
		OrderedDatatree *n2 = p->second;
		
		OrderedDatatree *nextNode = 0;

		if (n2->optimized && n2 != node) { nextNode = n2; }
		else if (n2->vertex != -1)
		{
			Part *part = new Part(n2->value);
			OrderedDatatree *n = n2;
			while (n->vertex != -1)
			{
				part->addIndividual(n->vertex,true);
				n = n->parent;
			}
			partition->addPart(part,true);
		}

		if (n1->optimized && n1 != node) { nextNode = n1; }
		else if (n1->vertex != -1)
		{
			Part *part = new Part(n1->value);
			OrderedDatatree *n = n1;
			while (n->vertex != -1)
			{
				part->addIndividual(n->vertex,true);
				n = n->parent;
			}
			partition->addPart(part,true);
		}
		
		node = nextNode;
	}
	
	return partition;
}
*/


Partition *OrderedDatatree::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(objective,parameter);
	parent->buildOptimalPartition(partition);
	return partition;
}


void OrderedDatatree::buildOptimalPartition (Partition *partition, int i, int j)
{
	if (vertex == -1) return;
	if (j == -1) { j = size2-1; }
	
	if (optimalCuts[getIndex(i,j)] == -1)
	{
		OrderedBipartition *p = optimalBipartitions[getIndex(i,j)];
		p->first->buildOptimalPartition(partition,i,j);
		p->second->buildOptimalPartition(partition,i,j);
	}
	
	else if (optimalCuts[getIndex(i,j)] < j) {
		int cut = optimalCuts[getIndex(i,j)];
		buildOptimalPartition(partition,i,cut);
		buildOptimalPartition(partition,i+cut+1,j-cut-1);
	}
	
	else {
		Part *p1 = new Part();
		Part *p2 = new Part();
		BiPart *part = new BiPart(p1,p2,qualities[getIndex(i,j)]);

		OrderedDatatree *node = this;
		while (node->vertex != -1)
		{
			p1->addIndividual(node->vertex);
			node = node->parent;
		}

		for (int k = i; k <= i+j; k++) { p2->addIndividual(k); }
		partition->addPart(part,true);
	}
}


void OrderedDatatree::print (bool verbose)
{
	std::cout << "PRINTING DATA TREE:" << std::endl;
	printRec(0,verbose);
	std::cout << std::endl;
}


void OrderedDatatree::printRec (int p, bool verbose)
{
	for (int i = 0; i < p; i++) { std::cout << "---"; }
		
	if (vertex == -1) { std::cout << " ROOT "; }
	else { std::cout << " PART "; printVertices(false); }
		
	if (complement != 0)
	{
		std::cout << " comp = ";
		complement->printVertices(false);
	}
	else { std::cout << " no complement"; }
	
	/*	
	if (optimized)
	{
		std::cout << " opt = ";
		optimalBipartitions->first->printVertices();
		optimalBipartitions->second->printVertices();
	}
	*/

	std::cout << std::endl;
		
	if (verbose)
	{
		for (OrderedBipartitionsSet::iterator it = bipartitions->begin(); it != bipartitions->end(); ++it)
		{
			for (int i = 0; i < p; i++) { std::cout << "---"; }
			std::cout << " PARTITION ";
			OrderedBipartition *bipartition = *it;
			bipartition->first->printVertices(false);
			if (bipartition->second != 0) { bipartition->second->printVertices(false); }
			std::cout << std::endl;
		}
	}
		
	for (OrderedTreesSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->printRec(p+1,verbose); }
}


void OrderedDatatree::printVertices (bool endl)
{
	if (vertex == -1) { std::cout << "{}"; }
	else {
		std::cout << "{";
		printVerticesRec();
		std::cout << "}";
	}
	if (endl) std::cout << std::endl;
}


void OrderedDatatree::printVerticesRec ()
{
	if (parent != 0 && parent->vertex != -1)
	{
		parent->printVerticesRec();
		std::cout << ",";
	}
	std::cout << vertex;
}


