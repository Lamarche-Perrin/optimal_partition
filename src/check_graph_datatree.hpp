#ifndef INCLUDE_CHECK_GRAPH_DATATREE
#define INCLUDE_CHECK_GRAPH_DATATREE

#include <set>
#include "graph.hpp"

typedef std::set<int> Vertices;
typedef std::set<Vertices*> VerticesSet;
typedef std::set<VerticesSet*> VerticesSetsSet;

class Graph;

bool checkOrder (Graph *graph);
bool checkParts (Graph *graph);
void checkPartitions (Graph *graph);

void addRecVertex(PartSet *set, Part *part, int vMin, int vMax);
VerticesSet *getPartition(int *s, int n);
int next(int *s, int *m, int n);

#endif
