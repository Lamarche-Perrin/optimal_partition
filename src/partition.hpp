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


#ifndef INCLUDE_PARTITION
#define INCLUDE_PARTITION

#include <list>

#include "objective_function.hpp"

/*!
 * \file parition.hpp
 * \brief Classes to represent partitions of sets (individuals/elements, parts/subsets, and partitions)
 * \author Robin Lamarche-Perrin
 * \date 06/11/2015
 */


class Datatree;
typedef std::set<int> Vertices;

class Part;
class Partition;

typedef std::set<Part*> PartSet;
typedef std::list<Partition*> PartitionList;


/*!
 * \class Part
 * \brief A part is a subset of a set of elements (individuals) represented by integers
 */
class Part
{
public:
	std::list<int> *individuals;
	ObjectiveValue *value;
	
	Part (ObjectiveValue *value = 0);
	Part (Part *part);
	Part (Datatree *node, ObjectiveValue *value = 0);
	virtual ~Part ();
	
	void addIndividual (int i, bool front = false);
	Vertices *getVertices ();
	virtual bool equal (Part *p);
		
	virtual void print (bool endl = false);
	virtual int printSize ();
};


/*!
 * \class BiPart
 * \brief A bi-part is a subset of a bi-dimensional set of elements (individuals)
 */
class BiPart: public Part
{
public:
	Part *firstPart;
	Part *secondPart;
	
	BiPart (Part *part1, Part *part2, ObjectiveValue *value = 0);
	BiPart (BiPart *biPart);
	~BiPart ();

	bool equal (Part *p);
	void print (bool endl = false);
	int printSize ();
};


/*!
 * \class MultiPart
 * \brief A multi-part is a subset of a multi-dimensional set of elements (individuals)
 */
class MultiPart: public Part
{
public:
	int dimension;
	Part **partArray;
	
	MultiPart (Part **partArray, int dimension, ObjectiveValue *value = 0);
	MultiPart (MultiPart *multiPart);
	~MultiPart ();

	bool equal (Part *p);
	void print (bool endl = false);
	int printSize ();
};


/*!
 * \class Partition
 * \brief A partition is a collection of pairwise-disjoint and covering subsets (parts) of a set of elements
 */
class Partition
{
public:
	double parameter;
	std::list<Part*> *parts;
	ObjectiveValue *value;
		
	Partition (ObjectiveFunction *objective = 0, double parameter = 0);
	Partition (Partition *partition);
	~Partition ();
		
	void addPart (Part *p, bool front = false);
	bool equal (Partition *p);
	void print (bool endl = false);
};


#endif
