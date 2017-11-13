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
#include <fstream>
#include <getopt.h>
#include <algorithm>

#include "geomediatic_aggregation.hpp"
#include "uni_set.hpp"
#include "multi_set.hpp"
#include "information_criterion.hpp"
#include "relative_entropy.hpp"

#include "csv_tools.hpp"

bool VERBOSE = false;
int VERBOSE_TAB = 0;

struct globalArgs_t
{
	std::string cubeFileName;
	std::string outputFileName;
	std::string hierarchyFileName;
	std::string model;
	double parameter;
	double threshold;
	int optimalSubsetNumber;
    int verbosity;
} globalArgs;

//static const char *optString = "d:m:o:s:t:l:v?";
static const char *optString = "d:m:o:h:s:t:l:v?";

static const struct option longOpts[] = {
    {"data", required_argument, NULL, 'd'},
    {"model", required_argument, NULL, 'm'},
    {"output", required_argument, NULL, 'o'},
    {"hierarchy", required_argument, NULL, 'h'},
    {"scale", required_argument, NULL, 's'},
    {"threshold", required_argument, NULL, 't'},
    {"optimal-list", required_argument, NULL, 'l'},
    {"verbose", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, '?'}
};

void usage (void)
{
	std::cout <<
		"Usage: geomediatic_aggregation --data dataFile.csv" << std::endl <<
		"Options:" << std::endl <<
		"-h | --help           Print this help." << std::endl <<
		"-d | --data           File containing the data (observed values). Required." << std::endl <<
		"-m | --model          Data model (expected values) used for the aggregation. If not specified: a uniform model is implied." << std::endl <<
		"-o | --output         File to which the results should be printed. If not specified: results are displayed in the terminal." << std::endl <<
		"-h | --hierarchy      File describing a hierarchy for spatial aggregation. If not specified: no constraint will be assumed on spatial dimensions." << std::endl <<
		"-s | --scale          A float between 0 and 1 describing the aggregation scale. If not specified: multiple scales are computed (see --threshold option)." << std::endl <<
		"-t | --threshold      The minimal distance between two consecutive scales. Not used if a unique scale is specified (see --scale option). If not specified: 0.01." << std::endl <<
		"-l | --optimal-list   Return the list of the N best aggregates instead of the optimal partition, where N is specified after this option." << std::endl;
    exit (EXIT_FAILURE);
}


int main (int argc, char *argv[])
{
    srand(time(NULL));
    int opt = 0;
	int longIndex;
    
    // Initialize globalArgs
    globalArgs.cubeFileName = "";
    globalArgs.model = "";
    globalArgs.outputFileName = "";
    globalArgs.hierarchyFileName = "";
    globalArgs.parameter = -1;
    globalArgs.threshold = 0.01;
    globalArgs.optimalSubsetNumber = 0;
    globalArgs.verbosity = 0;

	// Read program parameters
	opt = getopt_long (argc, argv, optString, longOpts, &longIndex);
    while (opt != -1)
	{
        switch (opt)
		{
		case 'd': globalArgs.cubeFileName = optarg; break;
		case 'm': globalArgs.model = optarg; break;
		case 'o': globalArgs.outputFileName = optarg; break;
		case 'h': globalArgs.hierarchyFileName = optarg; break;
		case 's': globalArgs.parameter = string2double(optarg); break;
		case 't': globalArgs.threshold = string2double(optarg); break;
		case 'l': globalArgs.optimalSubsetNumber = string2int(optarg); break;
		case 'v': globalArgs.verbosity++; break;
		case '?': usage(); break;
		default: break;
        }
        
        opt = getopt_long (argc, argv, optString, longOpts, &longIndex);
    }
	
	// Declare variables
	int dimension;
	int *sizeArray;
	std::string *dimArray;
	std::string *labels;
	std::vector<std::string> models;
	
	UniSet **setArray;

	// Open data file
	CSVLine line;
	std::ifstream cubeFile;
	openInputCSV (cubeFile, globalArgs.cubeFileName);

	// Get number of dimensions
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line);
	
	dimension = string2int(line[0]);

	// Get types of dimensions
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line);

	dimArray = new std::string [dimension];
	for (int d = 0; d < dimension; d++)
	{
		std::string type = line[d];
		type.erase (std::remove_if (type.begin(), type.end(), &isdigit), type.end());
		dimArray[d] = type;
	}

	// Get sizes of dimensions
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line);

	sizeArray = new int [dimension];
	for (int d = 0; d < dimension; d++) { sizeArray[d] = string2int(line[d]); }

	// Build corresponding uni-dimensional sets
	setArray = new UniSet* [dimension];	
	for (int d = 0; d < dimension; d++)
	{
		if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
		getCSVLine (cubeFile, line, sizeArray[d]);

		labels = new std::string [sizeArray[d]];
		for (int l = 0; l < sizeArray[d]; l++) { labels[l] = line[l]; }

		if (dimArray[d] == "media") { setArray[d] = new UnconstrainedUniSet (sizeArray[d], labels); }
		else if (dimArray[d] == "time") { setArray[d] = new OrderedUniSet (sizeArray[d], labels); }
		else if (dimArray[d] == "space")
		{
			if (globalArgs.hierarchyFileName == "") { setArray[d] = new UnconstrainedUniSet (sizeArray[d], labels); }
			else { setArray[d] = new HierarchicalUniSet (globalArgs.hierarchyFileName, sizeArray[d], labels); }
		}

		setArray[d]->buildDataStructure ();
	}

	// Build resulting multi-dimensional set
	MultiSet *multiSet = new MultiSet (setArray, dimension);
	multiSet->buildDataStructure();
	int elementNb = multiSet->atomicMultiSubsetNumber;

	// Print list of dimensions
	for (int d = 0; d < dimension; d++)
	{
		if (d > 0) { std::cout << " x "; }
		std::cout << dimArray[d] << "(" << sizeArray[d] << ")";
	}
	std::cout << " -> " << elementNb << " cells" << std::endl;

	// Get model names
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, models);
	
	// Get values and build objective function
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line, elementNb);

	double sumValues = 0;
	double *values = new double [elementNb];
	for (int i = 0; i < elementNb; i++) { values[i] = string2double(line[i]); sumValues += values[i]; }
	
	double *refValues = 0;
	if (globalArgs.model != "")
	{
		unsigned int modelLineNum = std::find (models.begin(), models.end(), globalArgs.model) - models.begin();
		if (modelLineNum >= models.size() || modelLineNum == 0) { std:: cout << "Warning: model " << globalArgs.model << " not found. Uniform model implied instead." << std::endl; }

		else {
			// Skip some lines
			for (unsigned int i = 1; i < modelLineNum; i++)
			{
				if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
				getCSVLine (cubeFile, line, elementNb);
			}

			// Get refvalues and build objective function
			if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
			getCSVLine (cubeFile, line, elementNb);

			refValues = new double [elementNb];
			for (int i = 0; i < elementNb; i++) { refValues[i] = string2double(line[i]); }
		}
	}

    //InformationCriterion *objective = new InformationCriterion (elementNb, values, refValues);
	RelativeEntropy *objective = new RelativeEntropy (elementNb, values, refValues);

	closeInputCSV (cubeFile);

	// Run algorithm and print results
    multiSet->setObjectiveFunction(objective);
    multiSet->computeObjectiveValues();
    multiSet->normalizeObjectiveValues();
	//multiSet->printObjectiveValues();
	
	if (globalArgs.optimalSubsetNumber > 0)
	{
		MultiSubset **subsetArray = multiSet->getOptimalMultiSubset (globalArgs.parameter, globalArgs.optimalSubsetNumber);
		for (int n = 0; n < globalArgs.optimalSubsetNumber; n++) { subsetArray[n]->print(); }
	}

	else {
		if (globalArgs.parameter >= 0) {
			if (globalArgs.outputFileName == "") { multiSet->getOptimalPartition(globalArgs.parameter)->print(); }

			else {
				Partition *partition = multiSet->getOptimalPartition(globalArgs.parameter);
	
				std::ofstream outputFile;
				openOutputCSV (outputFile, globalArgs.outputFileName, true);

				addCSVField (outputFile, "SCALE");
				for (int d = 0; d < dimension; d++) { addCSVField (outputFile, "DIM_"+int2string(d+1)); }
				addCSVField (outputFile, "DATA");
				addCSVField (outputFile, "MODEL");
				addCSVField (outputFile, "SIZE_REDUCTION");
				addCSVField (outputFile, "INFORMATION_LOSS",false);
				endCSVLine (outputFile);

				for (std::list<Part*>::iterator it1 = partition->parts->begin(); it1 != partition->parts->end(); ++it1)
				{
					MultiPart *multiPart = (MultiPart*) *it1;
					addCSVField (outputFile, partition->parameter);

					for (int d = 0; d < dimension; d++) { addCSVField (outputFile, multiPart->partArray[d]->name); }

					RelativeObjectiveValue *q = (RelativeObjectiveValue*) multiPart->value;
					//CriterionObjectiveValue *q = (CriterionObjectiveValue*) multiPart->value;
					addCSVField (outputFile, q->sumValue);
					addCSVField (outputFile, q->sumRefValue);
					addCSVField (outputFile, q->sizeReduction);
					addCSVField (outputFile, q->divergence, false);

					endCSVLine (outputFile);
				}
		
				closeOutputCSV(outputFile);
	
				delete partition;
			}
		}

		else {
			if (globalArgs.outputFileName == "") { multiSet->printOptimalPartitionList(globalArgs.threshold); }

			else {
				PartitionList *partitionList = multiSet->getOptimalPartitionList(globalArgs.threshold);
	
				std::ofstream outputFile;
				openOutputCSV (outputFile, globalArgs.outputFileName, true);

				addCSVField (outputFile, "SCALE");
				for (int d = 0; d < dimension; d++) { addCSVField (outputFile, "DIM_"+int2string(d+1)); }
				addCSVField (outputFile, "DATA");
				addCSVField (outputFile, "MODEL");
				addCSVField (outputFile, "SIZE_REDUCTION");
				addCSVField (outputFile, "INFORMATION_LOSS",false);
				endCSVLine (outputFile);

				int num = 0;
				for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it)
				{
					Partition *partition = *it;
					
					for (std::list<Part*>::iterator it1 = partition->parts->begin(); it1 != partition->parts->end(); ++it1)
					{
						MultiPart *multiPart = (MultiPart*) *it1;
						addCSVField (outputFile, partition->parameter);

						for (int d = 0; d < dimension; d++) { addCSVField (outputFile, multiPart->partArray[d]->name); }

						RelativeObjectiveValue *q = (RelativeObjectiveValue*) multiPart->value;
						//CriterionObjectiveValue *q = (CriterionObjectiveValue*) multiPart->value;
						addCSVField (outputFile, q->sumValue);
						addCSVField (outputFile, q->sumRefValue);
						addCSVField (outputFile, q->sizeReduction);
						addCSVField (outputFile, q->divergence, false);

						endCSVLine (outputFile);
					}
		
					num++;
				}

				closeOutputCSV (outputFile);
	
				for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it) { delete *it; }
				delete partitionList;
			}
		}
	}
	
	return EXIT_SUCCESS;
}


