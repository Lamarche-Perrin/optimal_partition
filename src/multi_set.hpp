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


#ifndef INCLUDE_MULTI_SET
#define INCLUDE_MULTI_SET

/*!
 * \file multi_set.hpp
 * \brief Classes to represent multi-dimensional sets of elements and their algebraic structure (feasible subsets and feasible refinements)
 * \author Robin Lamarche-Perrin
 * \date 06/11/2015
 */


#include <list>

#include "uni_set.hpp"
#include "abstract_set.hpp"

class UniSet;
class UniSubset;

class MultiSubset;
typedef std::list<MultiSubset*> MultiSubsetSet;
typedef std::list<MultiSubsetSet*> MultiSubsetSetSet;

/*!
 * \class MultiSet
 * \brief A multi-dimensional set of elements based on the Cartesian product of several uni-dimensional sets (UniSet) and their algebraic structures (feasible subsets and feasible refinements)
 */
class MultiSet: public AbstractSet
{
	friend MultiSubset;
public:
	int dimension; /*!< Number of dimensions */
	int atomicMultiSubsetNumber; /*!< Number of elements (e.g., atomic feasible subsets) */
	int multiSubsetNumber; /*!< Number of feasible subsets */

	/*!
	 * \brief Constructor for a one-dimensional set
	 * \param uniSet : Pointer to one uni-dimensional set (UniSet)
	 */
	MultiSet (UniSet *uniSet);
	
	/*!
	 * \brief Constructor
	 * \param uniSetArray : Array of pointers to uni-dimensional sets from which the Cartesian product is computed
	 * \param dimension : Number of uni-dimensional sets
	 */
	MultiSet (UniSet **uniSetArray, int dimension);

	/*!
	 * Destructor
	 */
	virtual ~MultiSet ();

	/*!
	 * Access to an element (e.g., atomic feasible subset) from its index
	 * /param The index of the element to access
	 * /return A pointer to the unique atomic feasible subset that contains the element
	 */
	MultiSubset *getAtomicMultiSubset (int index);
	
	void setRandom ();
	void setObjectiveFunction (ObjectiveFunction *objective);
	void print ();

	void buildDataStructure ();
	void computeObjectiveValues ();
	void normalizeObjectiveValues ();
	void printObjectiveValues ();

	void computeOptimalPartition (double parameter);
	void printOptimalPartition (double parameter);
	Partition *getOptimalPartition (double parameter);

protected:
	UniSet **uniSetArray;

	MultiSubset *firstMultiSubset;
	MultiSubset **multiSubsetArray;
	MultiSubset **atomicMultiSubsetArray;

	int getNum (int *multiNum);
	int *getMultiNum (int num);
	void initReached ();
};



class MultiSubset
{
public:
	int dimension;
	UniSubset **uniSubsetArray;

	int num;
	int atomicNum;
	
	bool isAtomic;
	bool reached;
	
	MultiSubsetSetSet *multiSubsetSetSet;
	MultiSet *multiSet;

	ObjectiveFunction *objective;
	ObjectiveValue *value;
	double optimalValue;
	MultiSubsetSet *optimalCut;

	MultiSubset (UniSubset **uniSubsetArray, int dimension);
	~MultiSubset ();

	void print ();
	void printIndexSet (bool endl = false);
	void addMultiSubsetSet (MultiSubsetSet *multiSubsetSet);
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
