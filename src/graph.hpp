#ifndef INCLUDE_GRAPH
#define INCLUDE_GRAPH

#include <map>
#include <vector>

#include "constrained_set.hpp"
#include "partition.hpp"
#include "datatree.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;
extern bool NORMALIZED_MEASURE;
extern double PARAMETER;

typedef std::set<int> Vertices;
typedef std::vector<int> VVertices;


class Datatree;
class Partition;

class GraphComponent;
typedef std::set<GraphComponent*> GraphComponentSet;


class Graph: public ConstrainedSet
{
public:
    int size;
    Vertices **adjacencySets;
    GraphComponent **graphComponents;
    GraphComponentSet *graphComponentSet;

    bool *reachedVertices;
    Quality *quality;
	
    Graph (int size);
    ~Graph ();
	
    void setRandom ();
    void setMeasure (Measure *m);
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
    void computeQuality ();
    void normalizeQuality ();
    void printQuality ();

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
    void computeQuality ();
    void normalizeQuality ();
    void printQuality ();
				
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
