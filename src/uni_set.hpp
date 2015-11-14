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
#include <fstream>

#include "bi_set.hpp"
#include "multi_set.hpp"

#include "graph.hpp"
#include "voter_graph.hpp"

/*!
 * \file uni_set.hpp
 * \brief Some classes to represent uni-dimensional sets of elements and their algebraic structure (feasible subsets and feasible refinements)
 * \author Robin Lamarche-Perrin
 * \date 06/11/2015
 */

class BiSet;
class BiSubset;
class MultiSet;
class MultiSubset;

class UniSubset;
typedef std::list<UniSubset*> UniSubsetSet;
typedef std::list<UniSubsetSet*> UniSubsetSetSet;
typedef std::list<int> IndexSet;

/*!
 * \class UniSet
 * \brief A uni-dimensional set of elements and its algebraic structure (feasible subsets and feasible refinements)
 */
class UniSet
{
	friend BiSet;
	friend BiSubset;
	friend MultiSet;
	friend MultiSubset;
	friend VoterMeasurement;
	friend VoterDataSet;

public:
	int atomicUniSubsetNumber; /** \brief Number of elements (i.e., atomic feasible subsets)*/
	int uniSubsetNumber; /** \brief Number of feasible subsets */
	
	/*!
     * \brief Constructor
	 * \param firstUniSubset : Top subset in the lattice of feasible subsets
     */
	UniSet (UniSubset *firstUniSubset);

	/*!
     * \brief Destructor
     */
	~UniSet ();

	/*!
     * \brief Build a proper data structure to represent the uni-dimensional set of elements and its algebraic structure (warning: this method should be called after construction, and before actually using the set)
     */
	void buildDataStructure ();

	/*!
     * \brief Print the current state of the set and its algebraic structure
     */
	void print ();

protected:
	UniSubset **atomicUniSubsetArray; /** \brief Array of pointers to all elements (i.e., atomic feasible subsets) */
	UniSubset **uniSubsetArray; /** \brief Array of pointers to all feasible subsets */
	UniSubset *firstUniSubset; /** \brief Top subset in the lattice of feasible subsets (assumed to be unique and to include all feasible subsets) */

	VoterMeasurement *voterMeasurement; /** \brief (Optional) A probe measurement of a voter model that has been used to build this uni-dimensional set */
	VoterProbe *voterProbe; /** \brief (Optional) A probe of a voter model that has been used to build this uni-dimensional set */
	
	/*!
	 * \brief Initialise the `reached` field of all feasible subsets to `false` (used by other methods to run through the algebraic structure in a recursive fashion without considering twice the same subset) 
     */
	void initReached ();
};


/*!
 * \class UnconstrainedUniSet
 * \brief A uni-dimensional set of elements with no particular structure, such that all subsets are feasible
 */
class UnconstrainedUniSet: public UniSet
{
public:
	int size; /** \brief Number of elements */

	/*!
	 * \brief Constructor
	 * \param size : Number of elements
	 */
	UnconstrainedUniSet (int size);
};


/*!
 * \class OrderedUniSet
 * \brief A uni-dimensional set of elements with a total order, and such that the feasible subsets are all the intervals induced by this order
 */
class OrderedUniSet: public UniSet
{
public:
	int size; /** \brief Number of ordered elements */

	/*!
	 * \brief Constructor
	 * \param size : Number of ordered elements
	 */
	OrderedUniSet (int size);

private:
	int getCell (int i, int j);
};


/*!
 * \class HierarchicalUniSet
 * \brief A uni-dimensional set of elements structured according to a hierarchy (tree), and such that the feasible subsets are all the nodes of the hierarchy (tree)
 */
class HierarchicalUniSet: public UniSet
{
public:
	/*!
	 * \brief Constructor
	 * \param fileName : The location of the CSV file from which the hierarchy should be build: first column should indicate nodes labels, and third column should indicate the corresponding parent (or NULL if top element)
	 */
	HierarchicalUniSet (std::string fileName);
};


/*!
 * \class BinaryTreeUniSet
 * \brief A uni-dimensional set of elements structured according to a complete binary tree, and such that the feasible subsets are all the nodes of the tree
 */
class BinaryTreeUniSet: public UniSet
{
public:
	int depth; /** \brief Depth of the complete binary tree */

	/*!
	 * \brief Constructor
	 * \param size : Depth of the complete binary tree
	 */
	BinaryTreeUniSet (int depth);

private:
	int buildTree (UniSubset *uniSubset, int depth, int index);
};


class GraphBasedUniSet: public UniSet
{
public:
	Graph *graph;
	GraphBasedUniSet (Graph *graph);
};


/*!
 * \class UniSubset
 * \brief A feasible subset associated to a uni-dimensional set of elements (UniSet)
 */
class UniSubset
{
	friend UniSet;
public:
	UniSet *uniSet; /** \brief Pointer to the set of elements to which this subset is associated */
	bool isAtomic; /** \brief `true` if and only if this subset is actually an element of the associated set (i.e., an atomic feasible subset) */
	int atomicNum; /** \brief If this subset is an element (i.e., an atomic feasible subset), identifier of this subset among all the elements of the associated set; if not, always equal to `-1` */
	int num; /** \brief Identifier of this subset among all the feasible subsets of the associated set */

	std::string name; /** \brief Name of this subset */
	IndexSet *indexSet; /** \brief Indexes of all the elements in this subset (only one index / one element in the case of an atomic subset) */
	UniSubsetSetSet *uniSubsetSetSet; /** \brief Set of the refinements of this subset, that is the set of all partitions of this subset that are made of other feasible subsets; this hence properly defines the algebraic structure */

	/*!
	 * \brief Constructor
	 * \param index : The index of the unique element in the case of an atomic subset
	 */
	UniSubset (int index = -1);

	/*!
	 * \brief Destructor
	 */
	~UniSubset ();

	/*!
	 * \brief Print the actual state of this subset
	 */
	void print ();
	
	/*!
	 * \brief Print indexes of the elements in this subset
	 */
	void printIndexSet (bool endl = false);

	/*!
	 * \brief Add a refinement to this subset, that is a partition of this subset that is made of other feasible subsets
	 */
	void addUniSubsetSet (UniSubsetSet *uniSubsetSet);

private:
	bool reached; /** \brief Boolean value used by external methods to run through the algebraic structure in a recursive fashion, without considering twice the same subset */
	void buildDataStructure ();
};


#endif
