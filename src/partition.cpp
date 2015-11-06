
#include <iostream>
#include <iomanip>

#include "partition.hpp"
#include "datatree.hpp"


Part::Part (Quality *q)
{
	individuals = new std::list<int>();
	quality = q;
}


Part::Part (Datatree *node, Quality *q)
{
	individuals = new std::list<int>();
	while (node->vertex != -1)
	{
		addIndividual (node->vertex,true);
		node = node->parent;
	}
	quality = q;
}


Part::Part (Part *p)
{
	quality = p->quality;

	individuals = new std::list<int>();
	for (std::list<int>::iterator it = p->individuals->begin(); it != p->individuals->end(); ++it)
	{
		addIndividual(*it);
	}
}


Part::~Part ()
{
	delete individuals;
}


void Part::addIndividual (int i, bool front)
{
	if (front) { individuals->push_front(i); }
	else { individuals->push_back(i); }
}


Vertices *Part::getVertices ()
{
	Vertices *v = new Vertices ();
	for (std::list<int>::iterator it = individuals->begin(); it != individuals->end(); ++it) { v->insert(*it); }
	return v;
}


bool Part::equal (Part *p)
{
	if (individuals->size() != p->individuals->size()) return false;
	
	for (std::list<int>::iterator it1 = individuals->begin(); it1 != individuals->end(); ++it1)
	{
		int i1 = *it1;
		
		bool found = false;
		for (std::list<int>::iterator it2 = p->individuals->begin(); it2 != p->individuals->end(); ++it2)
		{
			int i2 = *it2;
			if (i1 == i2) { found = true; break; }
		}
		
		if (!found) return false;
	}
	return true;
}



void Part::print (bool endl)
{
	std::cout << "{";
	bool first = true;
	for (std::list<int>::iterator it = individuals->begin(); it != individuals->end(); ++it)
	{
		if (first) { first = false; } else { std::cout << ","; }
		std::cout << (*it);
	}
	std::cout << "}";
	if (endl) { std::cout << std::endl; }
}

int Part::printSize () { return 2*individuals->size()+1; }


BiPart::BiPart (Part *p1, Part *p2, Quality *q)
{
	firstPart = p1;
	secondPart = p2;
	individuals = 0;
	quality = q;
}


BiPart::BiPart (BiPart *bp)
{
	firstPart = bp->firstPart;
	secondPart = bp->secondPart;
	individuals = 0;
	quality = bp->quality;
}


BiPart::~BiPart ()
{
	delete firstPart;
	delete secondPart;
}


bool BiPart::equal (Part *p)
{
	BiPart *bp = (BiPart *) p;
	return bp->firstPart->equal(firstPart) && bp->secondPart->equal(secondPart);
}


void BiPart::print (bool endl)
{
	firstPart->print();
	std::cout << "x";
	secondPart->print();
	if (endl) { std::cout << std::endl; }
}


int BiPart::printSize () { return firstPart->printSize() + secondPart->printSize() + 1; }


HyperPart::HyperPart (Part **pArray, int dim, Quality *q) : Part(q)
{
	partArray = pArray;
	dimension = dim;
}


HyperPart::HyperPart (HyperPart *hp) : Part(hp->quality)
{
	partArray = hp->partArray;
	dimension = hp->dimension;
}


HyperPart::~HyperPart ()
{
	for (int d = 0; d < dimension; d++) { delete partArray[d]; }
	delete [] partArray;
}


bool HyperPart::equal (Part *p)
{
	HyperPart *hp = (HyperPart *) p;
	if (hp->dimension != dimension) { return false; }
	for (int d = 0; d < dimension; d++)
		if (!hp->partArray[d]->equal(partArray[d])) { return false; }
	return true;
}


void HyperPart::print (bool endl)
{
	for (int d = 0; d < dimension; d++)
	{
		partArray[d]->print();
		if (d+1 < dimension) { std::cout << "x"; }
	}
	if (endl) { std::cout << std::endl; }
}


int HyperPart::printSize ()
{
	int size = 1;
	for (int d = 0; d < dimension; d++) { size += partArray[d]->printSize(); }
	return size;
}


Partition::Partition (Measure *m, double param)
{
	parameter = param;
	parts = new std::list<Part*>();
	if (m != 0) { quality = m->newQuality(); } else { quality = 0; }
}


Partition::Partition (Partition *p)
{
	parameter = p->parameter;
	quality = p->quality;

	parts = new std::list<Part*>();
	for (std::list<Part*>::iterator it = p->parts->begin(); it != p->parts->end(); ++it)
	{
		Part *part = new Part(*it);
		addPart(part);
	}
}


Partition::~Partition ()
{
	for(std::list<Part*>::iterator it = parts->begin(); it != parts->end(); it++) { delete *it; }
	delete parts;
	if (quality != 0) { delete quality; }
}


void Partition::addPart (Part *p, bool front)
{
	if (front) { parts->push_front(p); } else { parts->push_back(p); }
	if (p->quality != 0) { quality->add(p->quality); }
}


bool Partition::equal (Partition *p)
{
	if (parts->size() != p->parts->size()) return false;
	
	for (std::list<Part*>::iterator it1 = parts->begin(); it1 != parts->end(); ++it1)
	{
		Part *p1 = *it1;
		
		bool found = false;
		for (std::list<Part*>::iterator it2 = p->parts->begin(); it2 != p->parts->end(); ++it2)
		{
			Part *p2 = *it2;
			if (p1->equal(p2)) { found = true; break; }
		}
		
		if (!found) return false;
	}
	return true;
}


void Partition::print (bool endl)
{
	int printSize = 1;
	if (quality != 0) { std::cout << std::setw(6) << std::setprecision(3) << parameter << " -> "; }
	for (std::list<Part*>::iterator it = parts->begin(); it != parts->end(); ++it)
	{
		Part *part = *it;
		part->print();
		std::cout << " ";
		printSize += part->printSize() + 1;
	}
	//std::cout << std::setw(partition->printSize() - printSize) << " ";
	if (quality != 0) { quality->print(); }
	if (endl) { std::cout << std::endl; }
}


