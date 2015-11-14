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

#include "geomediatic_aggregation.hpp"
#include "uni_set.hpp"
#include "multi_set.hpp"
#include "relative_entropy.hpp"

bool VERBOSE = false;
int VERBOSE_TAB = 0;


int main (int argc, char *argv[])
{
    srand(time(NULL));

	UniSet *hierarchy = new HierarchicalUniSet ("../input/GEOMEDIA/smallWUTS.csv");
	hierarchy->buildDataStructure();
	//hierarchy->print();
	//std::cout << hierarchy->atomicUniSubsetNumber << std::endl;

	UniSet *dates = new OrderedUniSet (10);
	dates->buildDataStructure();
	//dates->print();
	//std::cout << dates->atomicUniSubsetNumber << std::endl;

	UniSet *media = new UnconstrainedUniSet (5);
	media->buildDataStructure();
	//media->print();
	//std::cout << media->atomicUniSubsetNumber << std::endl;

	UniSet *setArray [3] = {hierarchy, dates, media};
	MultiSet *multiSet = new MultiSet (setArray,3);
	multiSet->buildDataStructure();
	//multiSet->print();
	//std::cout << multiSet->atomicMultiSubsetNumber << std::endl;
	
    RelativeEntropy *objective = new RelativeEntropy (multiSet->atomicMultiSubsetNumber);
	objective->setRandom();
	
    multiSet->setObjectiveFunction(objective);
    multiSet->computeObjectiveValues();
    multiSet->normalizeObjectiveValues();

	//multiSet->getOptimalPartition(0.6)->print();

	//multiSet->print();
	MultiSubset **subsetArray = multiSet->getOptimalMultiSubset (1, 10);
	for (int n = 0; n < 10; n++) { subsetArray[n]->print(); }

	return EXIT_SUCCESS;
}


