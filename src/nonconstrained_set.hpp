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


#ifndef INCLUDE_NONCONSTRAINED_SET
#define INCLUDE_NONCONSTRAINED_SET

#include "abstract_set.hpp"
#include "datatree.hpp"
#include "partition.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;
extern bool NORMALIZED_OBJECTIVE;
extern double PARAMETER;

class Datatree;
class Partition;

class NonconstrainedSet: public AbstractSet
{
	public:
		int size;
		Datatree *dataTree;
		ObjectiveValue **qualities;	
		
		NonconstrainedSet (int size);
		~NonconstrainedSet ();

		void setRandom ();
		void setObjectiveFunction (ObjectiveFunction *m);
		void print ();
	
		void printDataTree (bool verbose = true);
		void printParts ();
		int printPartitions (bool print = true);

		void buildDataStructure ();
		void computeObjectiveValues ();
		void normalizeObjectiveValues ();
		void printObjectiveValues ();
				
		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);		
		Partition *getOptimalPartition (double parameter);

	private:
		Datatree *buildPartsRec (Datatree *node);
		void buildPartitionsRec (Datatree *node);
};



#endif
