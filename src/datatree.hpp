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


#ifndef INCLUDE_DATATREE
#define INCLUDE_DATATREE

#include <set>
#include <string>

#include "graph.hpp"
#include "partition.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;
extern bool NORMALIZED_OBJECTIVE;
extern double PARAMETER;

class Graph;
class Datatree;
class Partition;

typedef std::set<int> Vertices;
typedef std::set<Datatree*> TreesSet;
typedef std::list<Datatree*> TreesList;
typedef std::pair<Datatree*,Datatree*> Bipartition;
typedef std::set<Bipartition*> BipartitionsSet;
typedef std::list<Partition*> PartitionList;


class Datatree
{
	public:
		int size;
		int vertex;
		bool wholeSet;
		ObjectiveFunction *objective;
		
		Datatree *parent;
		Datatree *complement;
		TreesList *complementList;
		TreesSet *children;
		ObjectiveValue *value;
		BipartitionsSet *bipartitions;

		double optimalValue;
		Bipartition *optimalBipartition;
		bool optimized;
		
		Datatree (Datatree &tree);
		Datatree (int vertex = -1);
		~Datatree ();

		void setObjectiveFunction (ObjectiveFunction *objective);
		std::string toString ();

		Vertices *getAllVertices ();

		Datatree *addChild (int v, bool print = true);
		Datatree *findChild (int v);
		Datatree *findOrAddChild (int v, bool print = true);
		void addBipartition (Datatree *n1, Datatree *n2);
		
		void computeObjectiveValues ();
		void normalizeObjectiveValues (ObjectiveValue *maxObjectiveValue = 0);
		void printObjectiveValues ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);
		
		void print (bool verbose = false);
		void printVertices (bool endl = true);

		PartSet *getParts ();
		void printParts ();

		PartitionList *getAllPartitions ();
		int printPartitions (bool print = true);
		
	private:
		std::string toStringRec ();
		void printRec (int p, bool verbose);
		void printVerticesRec ();
};



class OrderedDatatree;

typedef std::set<OrderedDatatree*> OrderedTreesSet;
typedef std::list<OrderedDatatree*> OrderedTreesList;
typedef std::pair<OrderedDatatree*,OrderedDatatree*> OrderedBipartition;
typedef std::set<OrderedBipartition*> OrderedBipartitionsSet;
typedef std::list<Partition*> PartitionList;


class OrderedDatatree
{
	public:
		int size1;
		int size2;
		int vertex;
		bool wholeSet;
		ObjectiveFunction *objective;
		
		OrderedDatatree *parent;
		OrderedDatatree *complement;
		OrderedTreesList *complementList;
		OrderedTreesSet *children;
		OrderedBipartitionsSet *bipartitions;
		
		ObjectiveValue **qualities;
		double *optimalValues;
		int *optimalCuts;
		OrderedBipartition **optimalBipartitions;
		bool optimized;
		
		OrderedDatatree (OrderedDatatree &tree);
		OrderedDatatree (int size2, int vertex = -1);
		~OrderedDatatree ();

		void setObjectiveFunction (ObjectiveFunction *objective);

		Vertices *getAllVertices ();
		int getIndex (int i, int j);

		OrderedDatatree *addChild (int v, bool print = true);
		OrderedDatatree *findChild (int v);
		OrderedDatatree *findOrAddChild (int v, bool print = true);
		void addBipartition (OrderedDatatree *n1, OrderedDatatree *n2);
		
		void computeObjectiveValues ();
		void normalizeObjectiveValues (ObjectiveValue *maxObjectiveValue = 0);
		void printObjectiveValues ();

		void buildOptimalPartition (Partition *partition, int pi = 0, int pj = -1);
		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);
		
		void print (bool verbose = false);
		void printVertices (bool endl = true);

	private:
		void printRec (int p, bool verbose);
		void printVerticesRec ();
};

#endif
