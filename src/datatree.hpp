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
