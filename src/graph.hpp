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


#ifndef INCLUDE_GRAPH
#define INCLUDE_GRAPH

#include <map>
#include <vector>

#include "abstract_set.hpp"
#include "partition.hpp"
#include "datatree.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;
extern bool NORMALIZED_OBJECTIVE;
extern double PARAMETER;

typedef std::set<int> Vertices;
typedef std::vector<int> VVertices;


class Datatree;
class Partition;

class GraphComponent;
typedef std::set<GraphComponent*> GraphComponentSet;


class Graph: public AbstractSet
{
public:
    int size;
    Vertices **adjacencySets;
    GraphComponent **graphComponents;
    GraphComponentSet *graphComponentSet;

    bool *reachedVertices;
    ObjectiveValue *value;
	
    Graph (int size);
    ~Graph ();
	
    void setRandom ();
    void setObjectiveFunction (ObjectiveFunction *m);
    void print ();

    void addEdge (int v1, int v2);
    void buildFromBinary (int index);

    bool areAdjacent(int v1, int v2);
    bool areAdjacent(int v1, Vertices *v2);
    bool areAdjacent(Vertices *v1, int v2);
    bool areAdjacent(Vertices *v1, Vertices *v2);
    bool areAdjacent(int v1, VVertices *v2);
    bool areAdjacent(VVertices *v1, int v2);
    bool areAdjacent(VVertices *v1, VVertices *v2);

    Vertices *getAdjacentVertices (int v, int vMax = -1);
    void printVertices (Vertices *V);

    bool isConnected ();
    bool isConnected (Vertices *V);
		
    void printDataStructure (bool verbose = true);

    PartSet *getParts ();
    void printParts ();
    int printPartitions (bool print = true);

    void buildDataStructure ();
    void computeObjectiveValues ();
    void normalizeObjectiveValues ();
    void printObjectiveValues ();

    void buildDataStructureWithSlyce ();
    void slyce (VVertices *R, VVertices *F, int m);
    void enumerateSubsets (VVertices *R, VVertices *F, int m, int n, VVertices *T, int q, int r);
    
    void computeOptimalPartition (double parameter);
    void printOptimalPartition (double parameter);		
    Partition *getOptimalPartition (double parameter);
};


class GraphComponent
{
public:
    int size;
    int *vertices;
    std::map<int,int> *order;
		
    Graph *graph;
    Datatree *datatree;
	
    GraphComponent (Graph *graph);
    ~GraphComponent ();
	
    void setVertices (std::list<int> *vertexList);
		
    void printDataStructure (bool verbose = true);
		
    void buildDataStructure ();
    void computeObjectiveValues ();
    void normalizeObjectiveValues ();
    void printObjectiveValues ();
				
    void computeOptimalPartition (double parameter);
    void printOptimalPartition (double parameter);		
    Partition *getOptimalPartition (double parameter);
};


class RandomGraph : public Graph
{
public:
    RandomGraph (int vNum, int eNum);
};


class FiliformGraph : public Graph
{
public:
    FiliformGraph (int vNum);
};


class RingGraph : public Graph
{
public:
    RingGraph (int vNum);
};


class CompleteGraph : public Graph
{
public:
    CompleteGraph (int vNum);
};


#endif
