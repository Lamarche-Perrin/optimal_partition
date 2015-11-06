
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>

#include "main.hpp"
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

#include "structure.hpp"
#include "structure2D.hpp"
#include "hyper_structure.hpp"


bool VERBOSE = false;
int VERBOSE_TAB = 0;

bool NORMALIZED_MEASURE = true;


int main (int argc, char *argv[]) {
    srand(time(NULL));

	testLogarithmicScore ();

	/*
	char *fileName = (char *) "data/timer.csv";
	Timer timer = Timer (fileName,2,true);
	std::vector<int> param = std::vector<int>(2,0);

	for (int dimension = 9; dimension <= 9; dimension++)
	{
		param[0] = dimension;
		int size = 3;
		
		while (size == 3)
			//while (size == 1 || timer.time < 60)
		{
			param[1] = size;
			timer.start(param);
			timer.startTime();
			timer.startMemory();
			
			int depth = 3;
			bool hierarchical = false;
			double parameter = 0.5;

			Structure *structure;
			if (hierarchical) { structure = new HierarchicalStructure (depth); }
			else { structure = new OrderedStructure (size); }
			structure->buildDataStructure();

			Structure **structureArray = new Structure* [dimension];
			for (int d = 0; d < dimension; d++) { structureArray[d] = structure; }

			HyperStructure *hyperStruct = new HyperStructure (structureArray, dimension);
			hyperStruct->buildDataStructure();

			int valueNb;
			if (hierarchical) { valueNb = pow(depth,2*dimension); }
			else { valueNb = pow(size,dimension); }
	
			double values [valueNb];
			for (int v = 0; v < valueNb; v++) { values[v] = rand(); }

			
			RelativeEntropy *measure = new RelativeEntropy (valueNb,values);
	
			hyperStruct->setMeasure(measure);
			hyperStruct->computeQuality();
			hyperStruct->normalizeQuality();

			Partition *partition = hyperStruct->getOptimalPartition(parameter);
			//partition->print();
			//hyperStruct->printOptimalPartitionList(0.1);

			delete partition;
			delete measure;
			delete hyperStruct;
			delete structure;	
			delete [] structureArray;

			timer.stopTime();
			timer.stopMemory();
			timer.stop();
			size++;
		}
		size = 1;
	}
	*/
		
	//testHyperStructure();


	return EXIT_SUCCESS;
}


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

set->setMeasure(m);
set->computeQuality();
set->normalizeQuality();
	
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

    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

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
	
    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    set->print();
    set->printQuality();

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
	
    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    set->print();
    set->printQuality();

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
	
    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    set->print();
    set->printQuality();

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
	
    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    set->print();
    set->printQuality();
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
	
    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    set->print();
    set->printQuality();
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
	
    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    set->print();
    set->printQuality();
    set->printOptimalPartitionList(0.01);

    delete m;
    delete set;
}



void testGraph ()
{
    int graphNb = -1;
    int size = 7;
    int edgeNb = size*(size-1)/2;
	
    Graph *graph;

    bool error = false;
    int first = 0; int last = std::pow(2,edgeNb); VERBOSE = false;
    if (graphNb >= 0) { first = graphNb; last = graphNb+1; VERBOSE = true; }
	
    first = 120000;
    for (int index = first; index < last && !error; index++)
    {
	std::cout << "GRAPH " << index << std::endl;
	graph = new Graph(size);
	graph->buildFromBinary(index);
		
	if (graphNb >= 0) { graph->print(); }
		
	graph->buildDataStructure();

	if (graphNb >= 0) { graph->printDataStructure(); }

	bool errorParts = checkParts(graph);
	error = error || errorParts;

	delete graph;
    }
	
    if (error) { std::cout << "ERROR: there has been an error!"; }
    else { std::cout << "-> CHECK SUCCESSFUL!"; }
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


void testStructure2D ()
{
	Aggregate *h00 = new Aggregate (0);
	Aggregate *h01 = new Aggregate (1);
	Aggregate *h10 = new Aggregate (2);
	Aggregate *h11 = new Aggregate (3);

	Aggregate *h0 = new Aggregate();
	AggregateSet *h0Set = new AggregateSet();
	h0Set->push_back(h00);
	h0Set->push_back(h01);
	h0->addAggregateSet(h0Set);
	
	Aggregate *h1 = new Aggregate();
	AggregateSet *h1Set = new AggregateSet();
	h1Set->push_back(h10);
	h1Set->push_back(h11);
	h1->addAggregateSet(h1Set);

	Aggregate *h = new Aggregate();
	AggregateSet *hSet = new AggregateSet();
	hSet->push_back(h0);
	hSet->push_back(h1);
	h->addAggregateSet(hSet);

	Structure *hStruct = new Structure (h);
	hStruct->buildDataStructure();
	//hStruct->print();

	
	Aggregate *o0 = new Aggregate (0);
	Aggregate *o1 = new Aggregate (1);
	Aggregate *o2 = new Aggregate (2);
	Aggregate *o3 = new Aggregate (3);

	Aggregate *o01 = new Aggregate();
	AggregateSet *o0c1Set = new AggregateSet();
	o0c1Set->push_back(o0);
	o0c1Set->push_back(o1);
	o01->addAggregateSet(o0c1Set);

	Aggregate *o12 = new Aggregate();
	AggregateSet *o1c2Set = new AggregateSet();
	o1c2Set->push_back(o1);
	o1c2Set->push_back(o2);
	o12->addAggregateSet(o1c2Set);

	Aggregate *o23 = new Aggregate();
	AggregateSet *o2c3Set = new AggregateSet();
	o2c3Set->push_back(o2);
	o2c3Set->push_back(o3);
	o23->addAggregateSet(o2c3Set);

	Aggregate *o012 = new Aggregate();

	AggregateSet *o0c12Set = new AggregateSet();
	o0c12Set->push_back(o0);
	o0c12Set->push_back(o12);
	o012->addAggregateSet(o0c12Set);

	AggregateSet *o01c2Set = new AggregateSet();
	o01c2Set->push_back(o01);
	o01c2Set->push_back(o2);
	o012->addAggregateSet(o01c2Set);

	Aggregate *o123 = new Aggregate();

	AggregateSet *o1c23Set = new AggregateSet();
	o1c23Set->push_back(o1);
	o1c23Set->push_back(o23);
	o123->addAggregateSet(o1c23Set);

	AggregateSet *o12c3Set = new AggregateSet();
	o12c3Set->push_back(o12);
	o12c3Set->push_back(o3);
	o123->addAggregateSet(o12c3Set);

	Aggregate *o0123 = new Aggregate();

	AggregateSet *o0c123Set = new AggregateSet();
	o0c123Set->push_back(o0);
	o0c123Set->push_back(o123);
	o0123->addAggregateSet(o0c123Set);

	AggregateSet *o01c23Set = new AggregateSet();
	o01c23Set->push_back(o01);
	o01c23Set->push_back(o23);
	o0123->addAggregateSet(o01c23Set);

	AggregateSet *o012c3Set = new AggregateSet();
	o012c3Set->push_back(o012);
	o012c3Set->push_back(o3);
	o0123->addAggregateSet(o012c3Set);

	Structure *oStruct = new Structure (o0123);
	oStruct->buildDataStructure();
	//oStruct->print();

	
	Structure2D *hoStruct = new Structure2D (oStruct,hStruct);
	hoStruct->buildDataStructure();
	//hoStruct->print();

	double values [16]		= {	54,	30,	4, 5,
								54,	58,	64, 22,
								50, 18, 2, 12,
								45, 10, 3, 5 };
	
    RelativeEntropy *m = new RelativeEntropy(16,values);
	
    hoStruct->setMeasure(m);
    hoStruct->computeQuality();
    hoStruct->normalizeQuality();
    //hoStruct->printQuality();
	
    hoStruct->printOptimalPartitionList(0.001);


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

    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

    //set->print();
    //set->printQuality();
    set->printOptimalPartitionList(0.001);
}



void testHyperStructure ()
{
	Aggregate *h00 = new Aggregate (0);
	Aggregate *h01 = new Aggregate (1);
	Aggregate *h10 = new Aggregate (2);
	Aggregate *h11 = new Aggregate (3);

	Aggregate *h0 = new Aggregate();
	AggregateSet *h0Set = new AggregateSet();
	h0Set->push_back(h00);
	h0Set->push_back(h01);
	h0->addAggregateSet(h0Set);
	
	Aggregate *h1 = new Aggregate();
	AggregateSet *h1Set = new AggregateSet();
	h1Set->push_back(h10);
	h1Set->push_back(h11);
	h1->addAggregateSet(h1Set);

	Aggregate *h = new Aggregate();
	AggregateSet *hSet = new AggregateSet();
	hSet->push_back(h0);
	hSet->push_back(h1);
	h->addAggregateSet(hSet);

	Structure *hStruct = new Structure (h);
	hStruct->buildDataStructure();

	
	Structure *oStruct = new OrderedStructure (4);
	oStruct->buildDataStructure();

	
	int dimension = 2;
	Structure **structureArray = new Structure* [dimension];
	structureArray[0] = hStruct;
	structureArray[1] = oStruct;
		
	HyperStructure *hyperStruct = new HyperStructure (structureArray, dimension);
	hyperStruct->buildDataStructure();

	double values [16]		= {	54,	30,	4, 5,
								54,	58,	64, 22,
								50, 18, 2, 12,
								45, 10, 3, 5 };
	
    RelativeEntropy *m = new RelativeEntropy(16,values);
	
    hyperStruct->setMeasure(m);
    hyperStruct->computeQuality();
    hyperStruct->normalizeQuality();


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

    set->setMeasure(m);
    set->computeQuality();
    set->normalizeQuality();

	//hStruct->print();
	oStruct->print();
	//hyperStruct->print();
    //set->print();
	
	//hyperStruct->printQuality();
    //set->printQuality();

	for (double p = 0; p <= 1; p += 0.01)
	{		
		hyperStruct->getOptimalPartition(p)->print();
		set->getOptimalPartition(p)->print();
		std::cout << std::endl;
	}

    //hyperStruct->printOptimalPartitionList(0.001);
    //set->printOptimalPartitionList(0.001);

}


void testLogarithmicScore ()
{
	Structure *preStruct = new OrderedStructure (5);
	preStruct->buildDataStructure();
	HyperStructure *preStructure = new HyperStructure(preStruct);
	preStructure->buildDataStructure();
	
	Structure *postStruct = new OrderedStructure (5);
	postStruct->buildDataStructure();
	HyperStructure *postStructure = new HyperStructure(postStruct);
	postStructure->buildDataStructure();
	
	PredictionDataset *data = new PredictionDataset (preStructure, postStructure);

	data->addTrainValue (0,0,2);
	data->addTrainValue (0,1,2);
	data->addTrainValue (0,2,1);
	data->addTrainValue (0,3,1);
	data->addTrainValue (0,4,1);
	data->addTrainValue (1,1,2);
	data->addTrainValue (2,1,3);
	data->addTrainValue (2,2,1);
	data->addTrainValue (4,0,1);
	data->addTrainValue (4,1,1);
	data->addTrainValue (4,2,1);
	data->addTrainValue (4,3,1);
	
	data->addTestValue (0,1,1);
	data->addTestValue (2,2,2);
	data->addTestValue (2,3,4);
	data->addTestValue (3,0,3);

	
	LogarithmicScore *score = new LogarithmicScore (data, 1);
	preStructure->setMeasure(score);
	preStructure->computeQuality();

	//preStructure->print();
	//postStructure->print();
	preStructure->printQuality();

	preStructure->getOptimalPartition(0)->print();
	
	delete data;
}
