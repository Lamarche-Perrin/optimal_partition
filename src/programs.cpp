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

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>

#include "programs.hpp"
#include "timer.hpp"

#include "graph.hpp"
#include "check_graph_datatree.hpp"
#include "csv_tools.hpp"
#include "dataset.hpp"

#include "hierarchical_set.hpp"
#include "orderedset.hpp"
#include "hierarchical_ordered_set.hpp"
#include "hierarchical_hierarchical_set.hpp"
#include "ring.hpp"
#include "nonconstrained_set.hpp"
#include "nonconstrained_ordered_set.hpp"

#include "relative_entropy.hpp"
#include "logarithmic_score.hpp"
#include "prediction_dataset.hpp"

#include "uni_set.hpp"
#include "bi_set.hpp"
#include "multi_set.hpp"


void EbolaAggregation ()
{

    Timer timer;
    timer.start(0,"DOWNLOAD DATA");
    timer.startTime();
    timer.startMemory();
	
    std::string valuesFileName = "data/GEOMEDIA/Ebola/data_weeks.csv";
    std::string labelsFileName1 = "data/GEOMEDIA/Ebola/flows.csv";
    std::string labelsFileName2 = "data/GEOMEDIA/Ebola/weeks.csv";
    std::string outputFileName = "data/GEOMEDIA/Ebola/agg_marge_flows_weeks.csv";

    Dataset *data = getDatasetFromCSV (valuesFileName,labelsFileName1,labelsFileName2,true,true);
    data->print();

/*
// TIME
for (int i = 0; i < data->size1; i++)
{
timer.step("BUILD STRUCTURE");

OrderedSet *set = new OrderedSet(data->size2);	
set->buildDataStructure();

timer.step("FILL STRUCTURE");

std::string label = data->getLabel1(i);
RelativeEntropy *m = new RelativeEntropy(data->size2,data->getValues2(label),data->getRefValues2(label));

set->setObjectiveFunction(m);
set->computeObjectiveValues();
set->normalizeObjectiveValues();
	
timer.step("COMPUTE PARTITIONS");
	
outputFileName = "data/GEOMEDIA/Ebola/time_days/" + label + ".csv";
set->printOptimalPartitionListInCSV(0.1,data,2,outputFileName);
	
delete m;
delete set;
}
*/

    // MEDIA x TIME
    timer.step("BUILD STRUCTURE");

    NonconstrainedOrderedSet *set = new NonconstrainedOrderedSet(data->size1,data->size2);	
    set->buildDataStructure();

    timer.step("FILL STRUCTURE");

    RelativeEntropy *m = new RelativeEntropy(data->size1*data->size2,data->getValues(false),data->getRefValues(false));

    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    timer.step("COMPUTE PARTITIONS");

    //set->print();
    //set->printOptimalPartitionList(0.1);
    set->printOptimalPartitionListInCSV(0.1,data,0,outputFileName);

    delete m;
    delete set;

	

    delete data;
	
    timer.stopTime();
    timer.stopMemory();
    timer.stop();
}



void testHierarchicalSet ()
{
    int size = 5;
    double values [5] = {24,30,0,4,34};
    double refValues [5] = {100,110,10,20,50};

    HNode *n11 = new HNode(0);
    HNode *n12 = new HNode(1);
    HNode *n13 = new HNode(2);
    HNode *n21 = new HNode(3);
    HNode *n22 = new HNode(4);
    HNode *n1 = new HNode();
    HNode *n2 = new HNode();
    HNode *n = new HNode();

    n1->addChild(n11);
    n1->addChild(n12);
    n1->addChild(n13);
    n2->addChild(n21);
    n2->addChild(n22);
    n->addChild(n1);
    n->addChild(n2);
	
    HierarchicalSet *set = new HierarchicalSet(n);
    set->buildDataStructure();

    RelativeEntropy *m = new RelativeEntropy(size,values,refValues);
	
    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    set->print();
    set->printObjectiveValues();

    set->printOptimalPartitionList(0.001);

    delete m;
    delete set;
}



void testOrderedSet ()
{
    int size = 5;
    double values [5] = {24,30,0,4,34};
    double refValues [5] = {100,110,10,20,50};

    OrderedSet *set = new OrderedSet(size);	
    RelativeEntropy *m = new RelativeEntropy(size,values,refValues);
	
    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    set->print();
    set->printObjectiveValues();

    set->printOptimalPartitionList(0.001);

    delete m;
    delete set;
}


void testNonconstrainedSet ()
{
    int size = 7;
    double values [7] = {100,1001,1102,103,104,1005,1106};
    double *refValues = 0;

/*
  int size = 10;
  double values [10];
  double refValues [10];
	
  for (int i = 0; i < size; i++)
  {
  values[i] = rand();
  refValues[i] = values[i] + rand();
  }
*/

    NonconstrainedSet *set = new NonconstrainedSet(size);	
    set->buildDataStructure();

    RelativeEntropy *m = new RelativeEntropy(size,values,refValues);
	
    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    set->print();
    set->printObjectiveValues();

    set->printOptimalPartitionList(0.001);

    delete m;
    delete set;
}


void testNonconstrainedOrderedSet ()
{
    int size1 = 4;
    int size2 = 5;

    double values [20] = {	10.5,9,	10,	16,	34.1,
							9.5,10,	11,	15,	36.1,
							62.4, 57.6,	57.4,	60,	35.9,
							62.6, 60,	65.4,	65.6,	33.9};

    double *refValues = 0;

    NonconstrainedOrderedSet *set = new NonconstrainedOrderedSet(size1,size2);	
    set->buildDataStructure();

    RelativeEntropy *m = new RelativeEntropy(size1*size2,values,refValues);
	
    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    set->print();
    set->printObjectiveValues();
    set->printOptimalPartitionList(0.001);

    delete m;
    delete set;
}



void testHierarchicalOrderedSet ()
{
    int size = 3;
    double values [3*2]		= {	54,	30,	4,
								54,	58,	64};
	
    double refValues [3*2]	= {	9504,	7744,	5984,
								19008,	15488,	11968};

    HONode *n11 = new HONode(size,0);
    HONode *n12 = new HONode(size,1);
    HONode *n1 = new HONode(size);

    n1->addChild(n11);
    n1->addChild(n12);
	
    HierarchicalOrderedSet *set = new HierarchicalOrderedSet(n1,size);
    set->buildDataStructure();

    RelativeEntropy *m = new RelativeEntropy(size*2,values,refValues);
	
    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    set->print();
    set->printObjectiveValues();
    set->printOptimalPartitionList(0.001);

    delete m;
    delete set;	
}



void testHierarchicalHierarchicalSet ()
{
    int size = 9;

    double values [3*3]		= {	54,	30,	4,
								54,	58,	64,
								50, 18, 2 };
	
    double refValues [3*3]	= {	13904,	9328,	6160,
								27808,	18656,	12320,
								11060,	7420,	4900};

    /*
      double values [3*3]		= {	10,	11,	99,
      17,	18,	100,
      10, 18, 101};
      double *refValues = 0;
    */

    HNode *n0 = new HNode(0);
    HNode *n1 = new HNode(1);
    HNode *n2 = new HNode(2);
    HNode *n01 = new HNode();
    HNode *n = new HNode();

    n01->addChild(n0);
    n01->addChild(n1);
    n->addChild(n01);
    n->addChild(n2);

    HNode *h0 = new HNode(0);
    HNode *h1 = new HNode(1);
    HNode *h2 = new HNode(2);
    HNode *h01 = new HNode();
    HNode *h = new HNode();

    h01->addChild(h0);
    h01->addChild(h1);
    h->addChild(h01);
    h->addChild(h2);
	
    HierarchicalHierarchicalSet *set = new HierarchicalHierarchicalSet(n,h);
    set->buildDataStructure();

    RelativeEntropy *m = new RelativeEntropy(size,values,refValues);
	
    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    set->print();
    set->printObjectiveValues();
    set->printOptimalPartitionList(0.01);

    delete m;
    delete set;
}



void testGraph ()
{
    int graphNb = 10000;
    int size = 6;
    int edgeNb = size*(size-1)/2;
	
    Graph *graph;

    bool error = false;
    int first = 0; int last = std::pow(2,edgeNb); VERBOSE = false;
    if (graphNb >= 0) { first = graphNb; last = graphNb+1; VERBOSE = true; }
	
    for (int index = first; index < last && !error; index++)
    {
		std::cout << "GRAPH " << index << std::endl;
		graph = new Graph (size);
		graph->buildFromBinary(index);
		
		if (graphNb >= 0) { graph->print(); }
		
		graph->buildDataStructure();

		if (graphNb >= 0) { graph->printDataStructure(); }

		bool errorParts = checkParts(graph);
		error = error || errorParts;

		delete graph;
    }
	
    if (error) { std::cout << "ERROR: there has been an error!" << std::endl; }
    else { std::cout << "-> CHECK SUCCESSFUL!" << std::endl; }
}


void testGraphWithSlyce ()
{
	Graph *graph = new Graph(5);
    graph->addEdge(0,4);
    graph->addEdge(1,4);
    graph->addEdge(1,2);
    graph->addEdge(1,3);
    graph->addEdge(3,2);
    
    graph->print();
    graph->buildDataStructureWithSlyce();

    delete graph;
}


void testBiSet ()
{
	UniSubset *h00 = new UniSubset (0);
	UniSubset *h01 = new UniSubset (1);
	UniSubset *h10 = new UniSubset (2);
	UniSubset *h11 = new UniSubset (3);

	UniSubset *h0 = new UniSubset();
	UniSubsetSet *h0Subset = new UniSubsetSet();
	h0Subset->push_back(h00);
	h0Subset->push_back(h01);
	h0->addUniSubsetSet(h0Subset);
	
	UniSubset *h1 = new UniSubset();
	UniSubsetSet *h1Subset = new UniSubsetSet();
	h1Subset->push_back(h10);
	h1Subset->push_back(h11);
	h1->addUniSubsetSet(h1Subset);

	UniSubset *h = new UniSubset();
	UniSubsetSet *hSubset = new UniSubsetSet();
	hSubset->push_back(h0);
	hSubset->push_back(h1);
	h->addUniSubsetSet(hSubset);

	UniSet *hSet = new UniSet (h);
	hSet->buildDataStructure();
	//hSet->print();

	
	UniSubset *o0 = new UniSubset (0);
	UniSubset *o1 = new UniSubset (1);
	UniSubset *o2 = new UniSubset (2);
	UniSubset *o3 = new UniSubset (3);

	UniSubset *o01 = new UniSubset();
	UniSubsetSet *o0c1Set = new UniSubsetSet();
	o0c1Set->push_back(o0);
	o0c1Set->push_back(o1);
	o01->addUniSubsetSet(o0c1Set);

	UniSubset *o12 = new UniSubset();
	UniSubsetSet *o1c2Set = new UniSubsetSet();
	o1c2Set->push_back(o1);
	o1c2Set->push_back(o2);
	o12->addUniSubsetSet(o1c2Set);

	UniSubset *o23 = new UniSubset();
	UniSubsetSet *o2c3Set = new UniSubsetSet();
	o2c3Set->push_back(o2);
	o2c3Set->push_back(o3);
	o23->addUniSubsetSet(o2c3Set);

	UniSubset *o012 = new UniSubset();

	UniSubsetSet *o0c12Set = new UniSubsetSet();
	o0c12Set->push_back(o0);
	o0c12Set->push_back(o12);
	o012->addUniSubsetSet(o0c12Set);

	UniSubsetSet *o01c2Set = new UniSubsetSet();
	o01c2Set->push_back(o01);
	o01c2Set->push_back(o2);
	o012->addUniSubsetSet(o01c2Set);

	UniSubset *o123 = new UniSubset();

	UniSubsetSet *o1c23Set = new UniSubsetSet();
	o1c23Set->push_back(o1);
	o1c23Set->push_back(o23);
	o123->addUniSubsetSet(o1c23Set);

	UniSubsetSet *o12c3Set = new UniSubsetSet();
	o12c3Set->push_back(o12);
	o12c3Set->push_back(o3);
	o123->addUniSubsetSet(o12c3Set);

	UniSubset *o0123 = new UniSubset();

	UniSubsetSet *o0c123Set = new UniSubsetSet();
	o0c123Set->push_back(o0);
	o0c123Set->push_back(o123);
	o0123->addUniSubsetSet(o0c123Set);

	UniSubsetSet *o01c23Set = new UniSubsetSet();
	o01c23Set->push_back(o01);
	o01c23Set->push_back(o23);
	o0123->addUniSubsetSet(o01c23Set);

	UniSubsetSet *o012c3Set = new UniSubsetSet();
	o012c3Set->push_back(o012);
	o012c3Set->push_back(o3);
	o0123->addUniSubsetSet(o012c3Set);

	UniSet *oSet = new UniSet (o0123);
	oSet->buildDataStructure();
	//oSet->print();

	
	BiSet *hoSet = new BiSet (oSet,hSet);
	hoSet->buildDataStructure();
	//hoSet->print();

	double values [16]		= {	54,	30,	4, 5,
								54,	58,	64, 22,
								50, 18, 2, 12,
								45, 10, 3, 5 };
	
    RelativeEntropy *m = new RelativeEntropy(16,values);
	
    hoSet->setObjectiveFunction(m);
    hoSet->computeObjectiveValues();
    hoSet->normalizeObjectiveValues();
    //hoSet->printObjectiveValues();
	
    hoSet->printOptimalPartitionList(0.001);


	int size = 4;
	
    HONode *n00 = new HONode(size,0);
    HONode *n01 = new HONode(size,1);
    HONode *n10 = new HONode(size,2);
    HONode *n11 = new HONode(size,3);
    HONode *n0 = new HONode(size);
    HONode *n1 = new HONode(size);
    HONode *n = new HONode(size);

    n0->addChild(n00);
    n0->addChild(n01);
    n1->addChild(n10);
    n1->addChild(n11);
    n->addChild(n0);
    n->addChild(n1);
	
    HierarchicalOrderedSet *set = new HierarchicalOrderedSet(n,size);
    set->buildDataStructure();

    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

    //set->print();
    //set->printObjectiveValues();
    set->printOptimalPartitionList(0.001);
}



void testMultiSet ()
{
	UniSubset *h00 = new UniSubset (0);
	UniSubset *h01 = new UniSubset (1);
	UniSubset *h10 = new UniSubset (2);
	UniSubset *h11 = new UniSubset (3);

	UniSubset *h0 = new UniSubset();
	UniSubsetSet *h0Subset = new UniSubsetSet();
	h0Subset->push_back(h00);
	h0Subset->push_back(h01);
	h0->addUniSubsetSet(h0Subset);
	
	UniSubset *h1 = new UniSubset();
	UniSubsetSet *h1Subset = new UniSubsetSet();
	h1Subset->push_back(h10);
	h1Subset->push_back(h11);
	h1->addUniSubsetSet(h1Subset);

	UniSubset *h = new UniSubset();
	UniSubsetSet *hSubset = new UniSubsetSet();
	hSubset->push_back(h0);
	hSubset->push_back(h1);
	h->addUniSubsetSet(hSubset);

	UniSet *hSet = new UniSet (h);
	hSet->buildDataStructure();

	
	UniSet *oSet = new OrderedUniSet (4);
	oSet->buildDataStructure();

	
	int dimension = 2;
	UniSet **uniSetArray = new UniSet* [dimension];
	uniSetArray[0] = hSet;
	uniSetArray[1] = oSet;
		
	MultiSet *multiSet = new MultiSet (uniSetArray, dimension);
	multiSet->buildDataStructure();

	double values [16]		= {	54,	30,	4, 5,
								54,	58,	64, 22,
								50, 18, 2, 12,
								45, 10, 3, 5 };
	
    RelativeEntropy *m = new RelativeEntropy(16,values);
	
    multiSet->setObjectiveFunction(m);
    multiSet->computeObjectiveValues();
    multiSet->normalizeObjectiveValues();


	int size = 4;
	
    HONode *n00 = new HONode(size,0);
    HONode *n01 = new HONode(size,1);
    HONode *n10 = new HONode(size,2);
    HONode *n11 = new HONode(size,3);
    HONode *n0 = new HONode(size);
    HONode *n1 = new HONode(size);
    HONode *n = new HONode(size);

    n0->addChild(n00);
    n0->addChild(n01);
    n1->addChild(n10);
    n1->addChild(n11);
    n->addChild(n0);
    n->addChild(n1);
	
    HierarchicalOrderedSet *set = new HierarchicalOrderedSet(n,size);
    set->buildDataStructure();

    set->setObjectiveFunction(m);
    set->computeObjectiveValues();
    set->normalizeObjectiveValues();

	//hSet->print();
	oSet->print();
	//multiSet->print();
    //set->print();
	
	//multiSet->printObjectiveValues();
    //set->printObjectiveValues();

	for (double p = 0; p <= 1; p += 0.01)
	{		
		multiSet->getOptimalPartition(p)->print();
		set->getOptimalPartition(p)->print();
		std::cout << std::endl;
	}

    //multiSet->printOptimalPartitionList(0.001);
    //set->printOptimalPartitionList(0.001);

}
