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


void HierarchicalSet::setMeasure (Measure *m)
{
	measure = m;
	hierarchy->setMeasure(m);
}


void HierarchicalSet::buildDataStructure () { hierarchy->buildDataStructure(); }
void HierarchicalSet::print () { hierarchy->print(); }
void HierarchicalSet::computeQuality () { measure->computeQuality(); hierarchy->computeQuality(); }
void HierarchicalSet::normalizeQuality () { hierarchy->normalizeQuality(); }
void HierarchicalSet::printQuality () { hierarchy->printQuality(); }
void HierarchicalSet::computeOptimalPartition (double parameter) { hierarchy->computeOptimalPartition(parameter); }
void HierarchicalSet::printOptimalPartition (double parameter) { hierarchy->printOptimalPartition(parameter); }


Partition *HierarchicalSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(measure,parameter);
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
	
	quality->print(false);
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


void HNode::setMeasure (Measure *m)
{
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->setMeasure(m); }

	measure = m;
	if (index == -1) { quality = m->newQuality(); }
	else { quality = m->newQuality(index); }
}


void HNode::computeQuality ()
{
	QualitySet *qSet = new QualitySet();
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it)
	{
		HNode *node = *it;
		node->computeQuality();
		qSet->insert(node->quality);
	}

	if (index == -1) { quality->compute(qSet); } else { quality->compute(); }
	delete qSet;
}


void HNode::normalizeQuality (Quality *maxQual)
{
	if (maxQual == 0) { maxQual = quality; }

	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->normalizeQuality(maxQual); }
	quality->normalize(maxQual);
}


void HNode::printQuality ()
{
	printIndices();
	std::cout << " -> ";
		
	quality->print(true);
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->printQuality(); }
}


void HNode::computeOptimalPartition (double parameter)
{
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->computeOptimalPartition(parameter); }

	optimalValue = quality->getValue(parameter);
	optimalCut = true;
	
	double value = 0;
	for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { value += (*it)->optimalValue; }
	
	if ((measure->maximize && value > optimalValue) || (!measure->maximize && value < optimalValue))
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
		Part *part = new Part(quality);
		for (std::set<int>::iterator it = indices->begin(); it != indices->end(); ++it) { part->addIndividual(*it); }
		partition->addPart(part);
	}
	else { for (HNodeSet::iterator it = children->begin(); it != children->end(); ++it) { (*it)->buildOptimalPartition(partition); } }
}

