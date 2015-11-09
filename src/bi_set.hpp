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


#ifndef INCLUDE_BI_SET
#define INCLUDE_BI_SET

#include <list>

#include "uni_set.hpp"
#include "abstract_set.hpp"


class BiSubset;
typedef std::list<BiSubset*> BiSubsetSet;
typedef std::list<BiSubsetSet*> BiSubsetSetSet;

class BiSet: public AbstractSet
{
public:
	UniSet *uniSet1;
	UniSet *uniSet2;

	int biSubsetNumber;
	int atomicBiSubsetNumber;
	
	BiSubset *firstBiSubset;
	BiSubset **biSubsetArray;
	
	BiSet (UniSet *uniSet1, UniSet *uniSet2);
	virtual ~BiSet ();

	void initReached ();
	void setRandom ();
	void setObjectiveFunction (ObjectiveFunction *m);
	void print ();

	void buildDataStructure ();
	void computeObjectiveValues ();
	void normalizeObjectiveValues ();
	void printObjectiveValues ();

	void computeOptimalPartition (double parameter);
	void printOptimalPartition (double parameter);
	Partition *getOptimalPartition (double parameter);
};


class BiSubset
{
public:
	UniSubset *uniSubset1;
	UniSubset *uniSubset2;

	int num;
	bool isAtomic;
	bool reached;
	
	BiSubsetSetSet *biSubsetSetSet;
	BiSet *biSet;

	ObjectiveFunction *objective;
	ObjectiveValue *value;
	double optimalValue;
	BiSubsetSet *optimalCut;

	BiSubset (UniSubset *uniSubset1, UniSubset *uniSubset2);
	~BiSubset ();

	void print ();
	void printIndexSet (bool endl = false);
	void addBiSubsetSet (BiSubsetSet *biSubsetSet);
	void setObjectiveFunction (ObjectiveFunction *m);

	void buildDataStructure ();
	void computeObjectiveValues ();
	void normalizeObjectiveValues (ObjectiveValue *maxQual = 0);
	void printObjectiveValues ();

	void computeOptimalPartition (double parameter);
	void printOptimalPartition (double parameter);
	void buildOptimalPartition (Partition *partition);
};

#endif
