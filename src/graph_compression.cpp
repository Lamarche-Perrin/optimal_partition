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
 * Copyright © 2017 Robin Lamarche-Perrin
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
#include <string>
#include <getopt.h>

#include "graph_compression.hpp"
#include "csv_tools.hpp"
#include "timer.hpp"

#include "uni_set.hpp"
#include "multi_set.hpp"
#include "relative_entropy.hpp"

bool VERBOSE = false;
int VERBOSE_TAB = 0;

struct globalArgs_t
{
	int size;
	std::string inputFilename;
	std::string outputFilename;
	double parameter;
	double threshold;
    int verbosity;
} globalArgs;

static const char *optString = "h:s:i:o:t:v";

static const struct option longOpts[] = {
    {"help", no_argument, NULL, 'h'},
    {"size", required_argument, NULL, 's'},
    {"input", required_argument, NULL, 'i'},
    {"output", required_argument, NULL, 'o'},
    //{"rate", required_argument, NULL, 'r'},
    {"threshold", required_argument, NULL, 't'},
    {"verbose", no_argument, NULL, 'v'}
};

void usage (void)
{
	std::cout <<
		"Usage: graph_compression --input data.csv" << std::endl <<
		"Options:" << std::endl <<
		"-h | --help           Print this help." << std::endl <<
		"-s | --size           Size of the input graph." << std::endl <<
		"-i | --input          File containing the graph data. Required." << std::endl <<
		"-o | --output         File to which the results should be printed. If not specified: results are displayed in the terminal." << std::endl <<
		//"-r | --rate           A float between 0 and 1 describing the compression rate. If not specified: multiple compression rates are used (see --threshold option)." << std::endl <<
		"-t | --threshold      The minimal distance between two consecutive compression rates. If not specified: 0.01." << std::endl << //Not used if a unique compression rate is specified (see --rate option). 
		"-v | --verbose        Print some information regarding the script's execution." << std::endl;
    exit (EXIT_FAILURE);
}


int main (int argc, char *argv[])
{
    srand(time(NULL));
    int opt = 0;
	int longIndex;
    
    // Initialize globalArgs
	globalArgs.size = 0;
    globalArgs.inputFilename = "";
    globalArgs.outputFilename = "";
    //globalArgs.parameter = -1;
    globalArgs.threshold = 0.01;
    globalArgs.verbosity = 0;

	// Read program parameters
	opt = getopt_long (argc, argv, optString, longOpts, &longIndex);
    while (opt != -1)
	{
        switch (opt)
		{
		case 'h': usage(); break;
		case 's': globalArgs.size = string2int(optarg); break;
		case 'i': globalArgs.inputFilename = optarg; break;
		case 'o': globalArgs.outputFilename = optarg; break;
			//case 'r': globalArgs.parameter = string2double(optarg); break;
		case 't': globalArgs.threshold = string2double(optarg); break;
		case 'v': globalArgs.verbosity++; break;
		default: break;
        }
        
        opt = getopt_long (argc, argv, optString, longOpts, &longIndex);
    }

	Timer timer;
	if (globalArgs.verbosity)
	{
		timer.start(0,"START");
		timer.startTime();
	}

	UnconstrainedUniSet *startVertices = new UnconstrainedUniSet (globalArgs.size);
	startVertices->buildDataStructure();

	UnconstrainedUniSet *endVertices = new UnconstrainedUniSet (globalArgs.size);
	endVertices->buildDataStructure();

	std::vector<UniSet*> *setVector = new std::vector<UniSet*>();
	setVector->push_back(startVertices);
	setVector->push_back(endVertices);

	MultiSet *multiSet = new MultiSet (setVector);
	multiSet->buildDataStructure();
	
	if (globalArgs.verbosity) { timer.step("BUILD STRUCTURE"); }


	std::ifstream inputFile (globalArgs.inputFilename.c_str());
	if (!inputFile) { std::cout << "Cannot open " << globalArgs.inputFilename << std::endl; }

	std::string line;
	std::string field;
	double values [globalArgs.size*globalArgs.size];

	for (int i = 0; i < globalArgs.size; i++)
	{
		getline (inputFile, line, '\n');
		std::istringstream inputLine (line);	

		for (int j = 0; j < globalArgs.size; j++)
		{
			inputLine >> field;
			double value = atof (field.c_str());
			values[j+i*globalArgs.size] = value;
		}
	}

	double *refValues = NULL;
	// double *refValues = new double [globalArgs.size*globalArgs.size];
	// for (int i = 0; i < globalArgs.size; i++)
	// 	for (int j = 0; j < globalArgs.size; j++)
	// 		if (i == j) { refValues[i+j*globalArgs.size] = 0; }
	// 		else { refValues[i+j*globalArgs.size] = 1; }
	 
    RelativeEntropy *m = new RelativeEntropy (globalArgs.size*globalArgs.size, values, refValues);
 
    multiSet->setObjectiveFunction(m);
    multiSet->computeObjectiveValues();
    multiSet->normalizeObjectiveValues();

	if (globalArgs.verbosity) { timer.step("BUILD MEASURE"); }

	
	PartitionList *partitionList = multiSet->getOptimalPartitionList(globalArgs.threshold);
	
	std::ofstream outputFile;
	openOutputCSV (outputFile, globalArgs.outputFilename, true);

	addCSVField (outputFile, "RATE");
	addCSVField (outputFile, "AGG1");
	addCSVField (outputFile, "AGG2");
	addCSVField (outputFile, "VALUE");
	addCSVField (outputFile, "INFORMATION_LOSS");
	addCSVField (outputFile, "SIZE_REDUCTION",false);
	endCSVLine (outputFile);

	int num = 0;
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it)
	{
		Partition *partition = *it;

		for (std::list<Part*>::iterator it1 = partition->parts->begin(); it1 != partition->parts->end(); ++it1)
		{
			MultiPart *multiPart = (MultiPart*) *it1;
			addCSVField (outputFile, partition->parameter);
			
			std::string part1 = ""; bool first1 = true;
			for (std::list<int>::iterator it2 = multiPart->partArray[0]->individuals->begin(); it2 != multiPart->partArray[0]->individuals->end(); ++it2)
			{
				if (!first1) { part1 = "," + part1; } else { first1 = false; }
				part1 = int2string(*it2) + part1;
			}
			addCSVField (outputFile, part1);

			std::string part2 = ""; bool first2 = true;
			for (std::list<int>::iterator it2 = multiPart->partArray[1]->individuals->begin(); it2 != multiPart->partArray[1]->individuals->end(); ++it2)
			{
				if (!first2) { part2 = "," + part2; } else { first2 = false; }
				part2 = int2string(*it2) + part2;
			}
			addCSVField (outputFile, part2);

			RelativeObjectiveValue *q = (RelativeObjectiveValue*) multiPart->value;
			addCSVField (outputFile, q->sumValue);
			//addCSVField (outputFile, q->sumRefValue);
			addCSVField (outputFile, q->divergence);
			addCSVField (outputFile, q->sizeReduction, false);

			endCSVLine (outputFile);
		}
		
		num++;
	}

	closeOutputCSV(outputFile);
	
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it) { delete *it; }
	delete partitionList;
	
	if (globalArgs.verbosity) { timer.step("COMPUTE OPTIMAL PARTITION"); }

	if (globalArgs.verbosity)
	{
		timer.stopTime();
		timer.stop();
	}

	return EXIT_SUCCESS;
}


