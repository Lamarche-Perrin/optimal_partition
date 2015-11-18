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

#include "geomediatic_aggregation.hpp"
#include "uni_set.hpp"
#include "multi_set.hpp"
#include "relative_entropy.hpp"

#include "csv_tools.hpp"

bool VERBOSE = false;
int VERBOSE_TAB = 0;

int main (int argc, char *argv[])
{
    srand(time(NULL));

	// Declare and instantiate data files
	std::string cubeFileName = "input/GEOMEDIA/smallCube.csv";
	std::string hierarchyFileName = "input/GEOMEDIA/WUTS.csv";
	std::string modelFileName = "input/GEOMEDIA/smallModel_ST.csv";
	
	// Declare variables
	int dimension;
	int *sizeArray;
	std::string *dimArray;
	std::string *labels;
	
	UniSet **setArray;

	// Open data file
	CSVLine line;
	std::ifstream cubeFile;
	openInputCSV (cubeFile, cubeFileName);

	// Get number of dimensions
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line);
	
	dimension = string2int(line[0]);

	// Get types of dimensions
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line);

	dimArray = new std::string [dimension];
	for (int d = 0; d < dimension; d++) { dimArray[d] = line[d]; }

	// Get sizes of dimensions
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line);

	sizeArray = new int [dimension];
	for (int d = 0; d < dimension; d++) { sizeArray[d] = string2int(line[d]); }

	// Print list of dimensions
	for (int d = 0; d < dimension; d++)
	{
		if (d > 0) { std::cout << " x "; }
		std::cout << dimArray[d] << "(" << sizeArray[d] << ")";
	}
	std::cout << std::endl;

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
		else if (dimArray[d] == "space") { setArray[d] = new HierarchicalUniSet (hierarchyFileName, sizeArray[d], labels); }
		
		setArray[d]->buildDataStructure ();
	}

	// Build resulting multi-dimensional set
	MultiSet *multiSet = new MultiSet (setArray, dimension);
	multiSet->buildDataStructure();
	int elementNb = multiSet->atomicMultiSubsetNumber;
	//multiSet->print();
	//std::cout << multiSet->atomicMultiSubsetNumber << std::endl;

	// Get values and build objective function
	if (!hasCSVLine (cubeFile)) { closeInputCSV (cubeFile); return EXIT_FAILURE; }
	getCSVLine (cubeFile, line, elementNb);

	double *values = new double [elementNb];
	for (int i = 0; i < elementNb; i++) { values[i] = string2int(line[i]); }

	double *refValues = 0;
	if (modelFileName != "")
	{
		std::ifstream modelFile;
		openInputCSV (modelFile, modelFileName);

		for (int i = 0; i < 3+dimension; i++)
		{
			if (!hasCSVLine (modelFile)) { closeInputCSV (modelFile); return EXIT_FAILURE; }
			nextCSVLine (modelFile);
		}

		if (!hasCSVLine (modelFile)) { closeInputCSV (modelFile); return EXIT_FAILURE; }
		getCSVLine (modelFile, line, elementNb);

		refValues = new double [elementNb];
		for (int i = 0; i < elementNb; i++) { refValues[i] = string2int(line[i]); }
		
		closeInputCSV (modelFile);
	}

    RelativeEntropy *objective = new RelativeEntropy (elementNb, values, refValues);

	// Run algorithm
    multiSet->setObjectiveFunction(objective);
    multiSet->computeObjectiveValues();
    multiSet->normalizeObjectiveValues();
	//multiSet->getOptimalPartition(0.6)->print();
	
	int subsetNumber = 10;
	MultiSubset **subsetArray = multiSet->getOptimalMultiSubset (0.5, subsetNumber);
	for (int n = 0; n < subsetNumber; n++) { subsetArray[n]->print(); }
	
	closeInputCSV (cubeFile);
	return EXIT_SUCCESS;
}


