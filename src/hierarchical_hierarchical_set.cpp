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


void HierarchicalHierarchicalSet::setMeasure (Measure *m)
{
	measure = m;
	hyperarchy->setMeasure(m);
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
void HierarchicalHierarchicalSet::computeQuality () { measure->computeQuality(); hyperarchy->computeQuality(); }
void HierarchicalHierarchicalSet::normalizeQuality () { hyperarchy->normalizeQuality(); }
void HierarchicalHierarchicalSet::printQuality () { hyperarchy->printQuality(); }
void HierarchicalHierarchicalSet::computeOptimalPartition (double parameter) { hyperarchy->computeOptimalPartition(parameter); }
void HierarchicalHierarchicalSet::printOptimalPartition (double parameter) { hyperarchy->printOptimalPartition(parameter); }


Partition *HierarchicalHierarchicalSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	Partition *partition = new Partition(measure,parameter);
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
	
	if (quality != 0) { std::cout << " -> "; quality->print(false); } else { std::cout << std::endl; }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->print(); }

	if (node1->level == 0) { std::cout << std::endl; }
}


void HHNode::printIndices (bool endl)
{
	node1->printIndices();
	std::cout << " x ";
	node2->printIndices(endl);
}


void HHNode::setMeasure (Measure *m)
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->setMeasure(m); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->setMeasure(m); }

	measure = m;
	if (node1->index == -1 || node2->index == -1) { quality = m->newQuality(); }
	else { quality = m->newQuality(node1->index + node2->index * node1->root->width); }
}


void HHNode::computeQuality ()
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->computeQuality(); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->computeQuality(); }

	if (node1->index != -1) {
		if (node2->index != -1) { quality->compute(); }
		else {
			QualitySet *qSet = new QualitySet();
			for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { qSet->insert((*it)->quality); }
			quality->compute(qSet);
			delete qSet;
		}
	} else {
		QualitySet *qSet = new QualitySet();
		for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { qSet->insert((*it)->quality); }
		quality->compute(qSet);
		delete qSet;
	}
}


void HHNode::normalizeQuality (Quality *maxQual)
{
	if (maxQual == 0) { maxQual = quality; }

	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->normalizeQuality(maxQual); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->normalizeQuality(maxQual); }

	quality->normalize(maxQual);
}


void HHNode::printQuality ()
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->printQuality(); }

	for (int i = 0; i < node1->level; i++) { std::cout << "..."; }
	if (node1->level > 0) { std::cout << " "; }

	printIndices();
	std::cout << " -> ";
	quality->print(true);
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->printQuality(); }

	if (node1->level == 0) { std::cout << std::endl; }
}


void HHNode::computeOptimalPartition (double parameter)
{
	if (node1->level == 0)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->computeOptimalPartition(parameter); }
	
	for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->computeOptimalPartition(parameter); }

	optimalValue = quality->getValue(parameter);
	optimalCut = 0;
	
	if (!children1->empty())
	{
		double value1 = 0;
		for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { value1 += (*it)->optimalValue; }
		if ((measure->maximize && value1 > optimalValue) || (!measure->maximize && value1 < optimalValue))
		{
			optimalValue = value1;
			optimalCut = 1;
		}
	}

	if (!children2->empty())
	{
		double value2 = 0;
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { value2 += (*it)->optimalValue; }	
		if ((measure->maximize && value2 > optimalValue) || (!measure->maximize && value2 < optimalValue))
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
		BiPart *part = new BiPart(p1,p2,quality);

		for (std::set<int>::iterator it = node1->indices->begin(); it != node1->indices->end(); ++it) { p1->addIndividual(*it); }
		for (std::set<int>::iterator it = node2->indices->begin(); it != node2->indices->end(); ++it) { p2->addIndividual(*it); }

		partition->addPart(part,true);
	}
	
	if (optimalCut == 1)
		for (HHNodeSet::iterator it = children1->begin(); it != children1->end(); ++it) { (*it)->buildOptimalPartition(partition); }
		
	if (optimalCut == 2)
		for (HHNodeSet::iterator it = children2->begin(); it != children2->end(); ++it) { (*it)->buildOptimalPartition(partition); }
}

