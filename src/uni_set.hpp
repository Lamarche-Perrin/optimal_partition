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


#ifndef INCLUDE_UNI_SET
#define INCLUDE_UNI_SET

#include <list>

class UniSubset;
typedef std::list<UniSubset*> UniSubsetSet;
typedef std::list<UniSubsetSet*> UniSubsetSetSet;
typedef std::list<int> IndexSet;


class UniSet
{
public:
	int uniSubsetNumber;
	int atomicUniSubsetNumber;

	UniSubset *firstUniSubset;
	UniSubset **uniSubsetArray;
	UniSubset **atomicUniSubsetArray;

	UniSet (UniSubset *firstUniSubset);
	~UniSet ();

	void buildDataStructure ();
	void initReached ();
	void print ();
};


class OrderedUniSet: public UniSet
{
public:
	int size;
	OrderedUniSet (int size);
	int getIndex (int i, int j);
};


class HierarchicalUniSet: public UniSet
{
public:
	int depth;
	HierarchicalUniSet (int depth);
	int buildHierarchy (UniSubset *uniSubset, int depth, int index);
};


class UniSubset
{
public:
	int num;
	int atomicNum;
	bool isAtomic;
	bool reached;
	int count;

	IndexSet *indexSet;
	UniSubsetSetSet *uniSubsetSetSet;
	UniSet *uniSet;

	UniSubset (int index = -1);
	~UniSubset ();

	void print ();
	void printIndexSet (bool endl = false);
	void addUniSubsetSet (UniSubsetSet *uniSubsetSet);

	void buildDataStructure ();
};


#endif
