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
#include <iomanip>
#include <algorithm>

#include "partition.hpp"
#include "datatree.hpp"


Part::Part (ObjectiveValue *q)
{
    id = -1;
    size = 0;
    num = -2;
	individuals = new std::list<int>();
	value = q;
	name = "";
}


Part::Part (Datatree *node, ObjectiveValue *q)
{
	individuals = new std::list<int>();
	while (node->vertex != -1)
	{
		addIndividual (node->vertex,true);
		node = node->parent;
	}
	value = q;
	name = "";
}


Part::Part (Part *p)
{
	value = p->value;
	name = p->name;

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


void Part::addIndividual (int i, bool front, int v)
{
    size++;
    if (v != -1)
    {
		if (num == -2) { num = v; }
		else if (num != v) { num = -1; }
    }
	
	if (front) { individuals->push_front(i); }
	else { individuals->push_back(i); }
}


bool Part::contains (int i)
{
    for (std::list<int>::iterator it = individuals->begin(); it != individuals->end(); ++it)
		if (*it == i) return true;
    return false;
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
	if (name != "") { std::cout << name; }

	else {
		std::cout << "{";
		bool first = true;
		for (std::list<int>::iterator it = individuals->begin(); it != individuals->end(); ++it)
		{
			if (first) { first = false; } else { std::cout << ","; }
			std::cout << (*it);
		}
		std::cout << "}";
	}
	
	if (endl) { std::cout << std::endl; }
}

int Part::printSize () { return 2*individuals->size()+1; }


BiPart::BiPart (Part *p1, Part *p2, ObjectiveValue *q)
{
	firstPart = p1;
	secondPart = p2;
	individuals = 0;
	value = q;
}


BiPart::BiPart (BiPart *bp)
{
	firstPart = bp->firstPart;
	secondPart = bp->secondPart;
	individuals = 0;
	value = bp->value;
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


MultiPart::MultiPart (Part **pArray, int dim, ObjectiveValue *q) : Part(q)
{
	partArray = pArray;
	dimension = dim;
}


MultiPart::MultiPart (MultiPart *hp) : Part(hp->value)
{
	partArray = hp->partArray;
	dimension = hp->dimension;
}


MultiPart::~MultiPart ()
{
	for (int d = 0; d < dimension; d++) { delete partArray[d]; }
	delete [] partArray;
}


bool MultiPart::equal (Part *p)
{
	MultiPart *hp = (MultiPart *) p;
	if (hp->dimension != dimension) { return false; }
	for (int d = 0; d < dimension; d++)
		if (!hp->partArray[d]->equal(partArray[d])) { return false; }
	return true;
}


void MultiPart::print (bool endl)
{
	std::cout << "{";
	for (int d = 0; d < dimension; d++)
	{
		partArray[d]->print();
		if (d+1 < dimension) { std::cout << " x "; }
	}
	
	std::cout << "}";
	if (endl) { std::cout << std::endl; }
}


int MultiPart::printSize ()
{
	int size = 1;
	for (int d = 0; d < dimension; d++) { size += partArray[d]->printSize(); }
	return size;
}


Partition::Partition (ObjectiveFunction *m, double param)
{
	size = 0;
	parameter = param;
	parts = new std::list<Part*>();
	if (m != 0) { value = m->newObjectiveValue(); } else { value = 0; }
}


Partition::Partition (Partition *p)
{
	size = p->size;
	parameter = p->parameter;
	value = p->value;

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
	if (value != 0) { delete value; }
}


void Partition::addPart (Part *p, bool front)
{
	size++;
	if (front) { parts->push_front(p); } else { parts->push_back(p); }
	if (p->value != 0) { value->add(p->value); }
}


Part *Partition::findPart (int i)
{
    for (std::list<Part*>::iterator it = parts->begin(); it != parts->end(); ++it)
    {
		Part *p = *it;
		if (p->contains(i)) { return p; }
    }
    return 0;
}


Part *Partition::getPartFromValue (int v)
{
    Part *rp = 0;
    for (std::list<Part*>::iterator it = parts->begin(); it != parts->end(); ++it)
    {
		Part *p = *it;
		if (p->num == v)
		{
			if (rp != 0)
			{
				std::cout << "ERROR: several parts with the same value!" << std::endl;
				return 0;
			}
			rp = p;
		}
    }
    return rp;
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
	if (value != 0) { std::cout << std::setw(6) << std::setprecision(3) << parameter << " -> "; }
	for (std::list<Part*>::iterator it = parts->begin(); it != parts->end(); ++it)
	{
		Part *part = *it;
		part->print();
		std::cout << " ";
		printSize += part->printSize() + 1;
	}
	//std::cout << std::setw(partition->printSize() - printSize) << " ";
	if (value != 0) { value->print(); }
	if (endl) { std::cout << std::endl; }
}


void Partition::printAsOrderedBiSet (std::string *labelMatrix)
{
	int size1 = 0; int size2 = 0;
	for (std::list<Part*>::iterator it = parts->begin(); it != parts->end(); ++it)
	{
		MultiPart *part = (MultiPart*) *it;
		if (part->dimension != 2) { std::cout << "ERROR: Call of printAsOrderedBiSet only allowed on bi-dimensional partitions!" << std::endl; }

		Part *part1 = part->partArray[0];
		Part *part2 = part->partArray[1];
		int tmax1 = *std::max_element (part1->individuals->begin(), part1->individuals->end());
		int tmax2 = *std::max_element (part2->individuals->begin(), part2->individuals->end());
		size1 = std::max(size1,tmax1);
		size2 = std::max(size2,tmax2);
	}
	size1++; size2++;

	bool bottom [size1*size2]; bool right [size1*size2];
	for (int s = 0; s < size1*size2; s++) { bottom[s] = false; right[s] = false; }

	for (std::list<Part*>::iterator it = parts->begin(); it != parts->end(); ++it)
	{
		MultiPart *part = (MultiPart*) *it;
		Part *part1 = part->partArray[0];
		Part *part2 = part->partArray[1];

		int tmin1 = *std::min_element (part1->individuals->begin(), part1->individuals->end());
		int tmax1 = *std::max_element (part1->individuals->begin(), part1->individuals->end());

		int tmin2 = *std::min_element (part2->individuals->begin(), part2->individuals->end());
		int tmax2 = *std::max_element (part2->individuals->begin(), part2->individuals->end());

		for (int i1 = tmin1; i1 <= tmax1; i1++)
		{
			if (tmin2-1 >= 0) { bottom[i1 + (tmin2-1)*size1] = true; }
			bottom[i1 + tmax2*size1] = true;
		}
		
		for (int i2 = tmin2; i2 <= tmax2; i2++)
		{
			if (tmin1-1 >= 0) { right[(tmin1-1) + i2*size1] = true; }
			right[tmax1 + i2*size1] = true;
		}
	}

	for (int i2 = 0; i2 < size2; i2++)
	{
		for (int i1 = 0; i1 < size1; i1++)
		{
			std::cout << "+";
			if (i2 == 0 || bottom[i1+(i2-1)*size1]) { std::cout << "---"; } else { std::cout << "   "; }
		}
		std::cout << "+" << std::endl;

		for (int l = 0; l < 1; l++)
		{
			for (int i1 = 0; i1 < size1; i1++)
			{
				if (i1 == 0 || right[(i1-1)+i2*size1]) { std::cout << "|"; } else { std::cout << " "; }
				std::cout << " ";
				if (labelMatrix != 0) { std::cout << labelMatrix[i1+i2*size1]; } else { std::cout << " "; }
				std::cout << " ";
			}
			std::cout << "|" << std::endl;
		}
	}

	std::cout << "+";
	for (int i1 = 0; i1 < size1; i1++) { std::cout << "---+"; }
	std::cout << std::endl;
}
