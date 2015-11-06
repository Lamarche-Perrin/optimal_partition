#include <iostream>

#include "hierarchical_ordered_set.hpp"


HierarchicalOrderedSet::HierarchicalOrderedSet (HONode *h, int s)
{
	size = s;
	hierarchy = h;
}

void HierarchicalOrderedSet::setRandom () {}

HierarchicalOrderedSet::~HierarchicalOrderedSet ()
{
	delete hierarchy;
}


void HierarchicalOrderedSet::setMeasure (Measure *m)
{
	measure = m;
	hierarchy->setMeasure(m);
}


void HierarchicalOrderedSet::buildDataStructure () { hierarchy->buildDataStructure(); }
void HierarchicalOrderedSet::print () { hierarchy->print(); }
void HierarchicalOrderedSet::computeQuality () { measure->computeQuality(); hierarchy->computeQuality(); }
void HierarchicalOrderedSet::normalizeQuality () { hierarchy->normalizeQuality(); }
void HierarchicalOrderedSet::printQuality () { hierarchy->printQuality(); }
void HierarchicalOrderedSet::computeOptimalPartition (double parameter) { hierarchy->computeOptimalPartition(parameter); }
void HierarchicalOrderedSet::printOptimalPartition (double parameter) { hierarchy->printOptimalPartition(parameter); }


Partition *HierarchicalOrderedSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(measure,parameter);
	hierarchy->buildOptimalPartition(partition);	
	return partition;
}





HONode::HONode (int s, int i)
{
	index = i;
	level = -1;
	indices = new std::set<int>();

	children = new HONodeSet();

	size = s;
	int s2 = (s+1)*s/2;
	
	qualities = new Quality* [s2];
	optimalValues = new double [s2];
	optimalCuts = new int [s2];
}


HONode::~HONode ()
{
	delete indices;
	delete children;

	delete[] qualities;
	delete[] optimalValues;
	delete[] optimalCuts;
}

void HONode::addChild (HONode *node) { children->insert(node); }

int HONode::getIndex (int i, int j) { return j*size-j*(j-1)/2+i; }


void HONode::buildDataStructure (int l)
{
	level = l;
	if (index != -1) { indices->insert(index); }
	
	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		HONode *child = *it;
		child->buildDataStructure(l+1);

		for (std::set<int>::iterator it = child->indices->begin(); it != child->indices->end(); ++it) { indices->insert(*it); }
	}
}


void HONode::print ()
{
	for (int i = 0; i < level; i++) { std::cout << "..."; }
	if (level > 0) { std::cout << " "; }

	std::cout << "{"; bool first = true;
	for (std::set<int>::iterator it = indices->begin(); it != indices->end(); ++it)
	{
		if (!first) { std::cout << ","; }
		std::cout << *it;
		first = false;
	}
	std::cout << "}" << std::endl;

	if (index != -1) {
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < level; j++) { std::cout << "..."; }
			if (level > 0) { std::cout << " "; }
			std::cout << " [" << i << "] -> ";
			qualities[getIndex(i,0)]->print(false);
		}
	}
	
	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->print(); }
}


void HONode::setMeasure (Measure *m)
{
	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->setMeasure(m); }

	measure = m;
	if (index == -1)
	{
		for (int j = 0; j < size; j++)
			for (int i = 0; i < size-j; i++)
				qualities[getIndex(i,j)] = measure->newQuality();
	}
	
	else {
		for (int i = 0; i < size; i++)
			qualities[getIndex(i,0)] = measure->newQuality(index*size+i);
	
		for (int j = 1; j < size; j++)
			for (int i = 0; i < size-j; i++)
				qualities[getIndex(i,j)] = measure->newQuality();
	}
}


void HONode::computeQuality ()
{
	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->computeQuality(); }

	if (index == -1)
	{
		for (int i = 0; i < size; i++)
		{
			QualitySet *qSet = new QualitySet();
			for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { qSet->insert((*it)->qualities[getIndex(i,0)]); }
			qualities[getIndex(i,0)]->compute(qSet);		
			delete qSet;
		}
	}
	
	else {
		for (int i = 0; i < size; i++)
			qualities[getIndex(i,0)]->compute();		
	}
		
	for (int j = 1; j < size; j++)
		for (int i = 0; i < size-j; i++) {
			qualities[getIndex(i,j)]->compute(qualities[getIndex(i,j-1)],qualities[getIndex(i+j,0)]);
			//qualities[getIndex(i,j)]->print();
		}
}


void HONode::normalizeQuality (Quality *maxQual)
{
	if (maxQual == 0) { maxQual = qualities[getIndex(0,size-1)]; }

	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->normalizeQuality(maxQual); }

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
			qualities[getIndex(i,j)]->normalize(maxQual);
}


void HONode::printQuality ()
{
	for (int i = 0; i < level; i++) { std::cout << "..."; }
	if (level > 0) { std::cout << " "; }

	std::cout << "{"; bool first = true;
	for (std::set<int>::iterator it = indices->begin(); it != indices->end(); ++it)
	{
		if (!first) { std::cout << ","; }
		std::cout << *it;
		first = false;
	}
	std::cout << "}" << std::endl;

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
		{
			for (int l = 0; l < level; l++) { std::cout << "..."; }
			if (level > 0) { std::cout << " "; }
			std::cout << " [" << i << "," << j << "] -> ";
			qualities[getIndex(i,j)]->print(true);
		}
	
	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->printQuality(); }
}


void HONode::computeOptimalPartition (double parameter)
{
	for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->computeOptimalPartition(parameter); }

	for (int i = size-1; i >= 0; i--)
	{
		for (int j = 0; j < size-i; j++)
		{
			optimalCuts[getIndex(i,j)] = j;
//			std::cout << "(" << i << "," << j << ") " << getIndex(i,j) << " -> " << parameter << std::endl;
//			std::cout << qualities[getIndex(i,j)]->getValue(parameter) << std::endl;
			optimalValues[getIndex(i,j)] = qualities[getIndex(i,j)]->getValue(parameter);
			
			if (index == -1)
			{
				double value = 0;
				for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { value += (*it)->optimalValues[getIndex(i,j)]; }
				
				if ((measure->maximize && value > optimalValues[getIndex(i,j)]) || (!measure->maximize && value < optimalValues[getIndex(i,j)]))
				{
					optimalValues[getIndex(i,j)] = value;
					optimalCuts[getIndex(i,j)] = -1;
				}				
			}
			
			for (int cut = 0; cut < j; cut++)
			{
				//double value = qualities[getIndex(i,cut)]->getValue(parameter) + qualities[getIndex(i+cut+1,j-cut-1)]->getValue(parameter);
				double value = optimalValues[getIndex(i,cut)] + optimalValues[getIndex(i+cut+1,j-cut-1)];
				if ((measure->maximize && value > optimalValues[getIndex(i,j)]) || (!measure->maximize && value < optimalValues[getIndex(i,j)]))
				{
					optimalValues[getIndex(i,j)] = value;
					optimalCuts[getIndex(i,j)] = cut;
				}
			}
		}
	}
}


void HONode::printOptimalPartition (double parameter) {}


void HONode::buildOptimalPartition (Partition *partition, int i, int j)
{
	if (j == -1) { j = size-1; }
	
	if (optimalCuts[getIndex(i,j)] == -1)
	{
		for (HONodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->buildOptimalPartition(partition,i,j); }
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

		for (std::set<int>::iterator it = indices->begin(); it != indices->end(); ++it) { p1->addIndividual(*it); }
		for (int k = i; k <= i+j; k++) { p2->addIndividual(k); }
		partition->addPart(part,true);
	}
}


