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


#include "prediction_programs.hpp"

#include "uni_set.hpp"
#include "multi_set.hpp"
#include "prediction_dataset.hpp"
#include "logarithmic_score.hpp"

#include "timer.hpp"
#include "voter_graph.hpp"

#include <stdlib.h>
#include <cmath>


/*
 * Test the algorithm for optimal binning of uni-dimensional pre-measurements. We use
 * the logarithmic score function. The microscopic binning of both the pre-measurement
 * and the post-measurement contains 5 values.
 */
void testLogarithmicScore ()
{
	/*
	 * Build the pre-measurement observation space, that is a set of 5 ordered elements
	 * such that the feasible subsets (aggregates) are all the possible intervals of
	 * elements. This is first represented by the OrderedUniSet class, representing a
	 * particular kind of uni-dimensional set (UniSet), then this class is encapsulated
	 * as a multi-dimensional set (MultiSet, even though it contains one dimension)
	 */
	UniSet *preUniSet = new OrderedUniSet (5);
	preUniSet->buildDataStructure(); // Important: this method should always be called after construction

	MultiSet *preMultiSet = new MultiSet (preUniSet);
	preMultiSet->buildDataStructure(); // Important: this method should always be called after construction

	/*
	 * Idem for the post-measurement.
	 */
	UniSet *postUniSet = new OrderedUniSet (5);
	postUniSet->buildDataStructure(); // Important: this method should always be called after construction

	MultiSet *postMultiSet = new MultiSet (postUniSet);
	postMultiSet->buildDataStructure(); // Important: this method should always be called after construction

	/*
	 * Create a new data set that will be use for prediction of the post-measurement
	 * given the pre-measurement (train and test).
	 */
	PredictionDataset *data = new PredictionDataset (preMultiSet, postMultiSet);

	/*
	 * Feed train values to this data set.
	 */
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
	
	/*
	 * Feed test values to this data set.
	 */
	data->addTestValue (0,1,1);
	data->addTestValue (2,2,2);
	data->addTestValue (2,3,4);
	data->addTestValue (3,0,3);

	/*
	 * Set and compute the objective function to be optimised. We choose the logarithmic
	 * score with a prior of 1 observation for all couple of pre-observation and post-observation.
	 */
	LogarithmicScore *score = new LogarithmicScore (data, 1); // Create the objective from the data.
	preMultiSet->setObjectiveFunction (score); // Associate the objective to the pre-measurement structure.
	preMultiSet->computeObjectiveValues (); // Compute the objective for all feasible subsets.

	/*
	 * Compute the partition (binning) that optimises the objective. Here, because we use the
	 * logarithmic score, that is a non-parametrised objective, the parameter (0) does not matter.
	 */
	Partition *optimalPartition = preMultiSet->getOptimalPartition (0);
	
	/*
	 * Print the results.
	 */
	//preMultiSet->print();
	//postMultiSet->print();
	//preMultiSet->printObjectiveValues();
	optimalPartition->print();

	/*
	 * Free memory.
	 */
	delete preUniSet;
	delete preMultiSet;
	delete postUniSet;
	delete postMultiSet;
	delete score;
	delete optimalPartition;
	delete data;
}


/*
 * Run the algorithm for optimal binning on data sets with various sizes and dimensions in order
 * to get the average computation time. The data sets are randomly generated and the result is
 * recorded in an external file (see data directory).
 */
void getBinningComputationTime ()
{
	/*
	 * Hard-coded parameters of the program.
	 */
		
	int dimensionMin = 2; // Number of dimensions to begin with
	int dimensionMax = 7; // Maximal number of dimensions

	int sizeMin = 1; // Number of bins to begin with
	int sizeMax = 20; // Maximal number of bins

	/*
	 * Create a timer and a file to record the results.
	 */
	char *fileName = (char *) "../data/computation_time.csv";
	Timer timer = Timer (fileName, 2, true);
	std::vector<int> param = std::vector<int>(2,0);

	/*
	 * Run the algorithm for each dimension and size.
	 */
	for (int dimension = dimensionMin; dimension <= dimensionMax; dimension++)
	{
		param[0] = dimension;
		int size = sizeMin;
		
		while ((size == sizeMin || timer.time < 60) && size <= sizeMax)
		{
			/*
			 * Start timer.
			 */
			param[1] = size;
			timer.start(param);
			timer.startTime();
			timer.startMemory();

			/*
			 * Build the pre-measurement as the Cartesian product of several uni-dimensional ordered sets (OrderedUniSet).
			 */
			UniSet *uniSet = new OrderedUniSet (size);
			uniSet->buildDataStructure();

			UniSet **uniSetArray = new UniSet* [dimension];
			for (int d = 0; d < dimension; d++) { uniSetArray[d] = uniSet; }

			MultiSet *multiSet = new MultiSet (uniSetArray, dimension);
			multiSet->buildDataStructure();

			/*
			 * Build the post-measurement as a uni-dimensional ordered sets (OrderedUniSet) with 10 bins
			 */
			UniSet *postSet = new OrderedUniSet (10);
			postSet->buildDataStructure();
			
			MultiSet *postMultiSet = new MultiSet (postSet);
			postMultiSet->buildDataStructure();

			/*
			 * Create the objective function (logarithmic score) from a random data set and with a prior of 1.
			 */
			PredictionDataset *dataset = new PredictionDataset (multiSet, postMultiSet);
			LogarithmicScore *objective = new LogarithmicScore (dataset, 1);
			objective->setRandom();	
			multiSet->setObjectiveFunction(objective);

			/*
			 * Compute the values of the objective for each possible multi-dimensional bin of the pre-measurement.
			 */
			multiSet->computeObjectiveValues();

			/*
			 * Compute the binning that optimises the logarithmic score.
			 */
			Partition *partition = multiSet->getOptimalPartition (0);
			//partition->print();

			/*
			 * Free memory.
			 */
			delete partition;
			delete objective;
			delete multiSet;
			delete uniSet;	
			delete [] uniSetArray;

			/*
			 * Stop timer.
			 */
			timer.stopTime();
			timer.stopMemory();
			timer.stop();

			size++;
		}
	}
}


void optimalBinningOfVoterModel ()
{
	/*
	 * Build voter graph with two communities.
	 */
	int size = 6;
	double intraRate = 1.;
	double interRate = 1./5;
	double contrarian = 0; //1./(size+1);

	//VoterGraph *VG = new TwoCommunitiesVoterGraph (size,size, intraRate,intraRate, interRate,interRate, contrarian,contrarian);
	VoterGraph *VG = new TwoCommunitiesVoterGraph (size,0, intraRate,0, 0,0, contrarian,0);

	/*
	 * Build uni-dimensional measurements of the graph.
	 */
	VoterMeasurement *microM = getMeasurement (VG, M_MICRO, MACRO_STATE);
	VoterMeasurement *agentM = getMeasurement (VG, M_AGENT1, MACRO_STATE);
	VoterMeasurement *mesoM = getMeasurement (VG, M_MESO1, MACRO_STATE);
	VoterMeasurement *macroM = getMeasurement (VG, M_MACRO, MACRO_STATE);

	VoterMeasurement *postM = getMeasurement (VG, M_MACRO, MACRO_STATE);

	/*
	 * Get corresponding uni-dimensional ordered set representing the observation space.
	 */
	std::vector<OrderedUniSet*> *microSetVector = microM->getOrderedUniSetVector();

	for (unsigned int p = 0; p < microSetVector->size(); p++)
		microSetVector->at(p)->buildDataStructure();

	OrderedUniSet *agentSet = agentM->getOrderedUniSet();
	OrderedUniSet *mesoSet = mesoM->getOrderedUniSet();
	OrderedUniSet *macroSet = macroM->getOrderedUniSet();
	OrderedUniSet *postSet = postM->getOrderedUniSet();

	agentSet->buildDataStructure();
	mesoSet->buildDataStructure();
	macroSet->buildDataStructure();
	postSet->buildDataStructure();

	/*
	 * Build the pre-measurement (multi-dimensional) and the post-measurement (one-dimensional).
	 */
	std::vector<UniSet*> *setVector = new std::vector<UniSet*>();

	for (unsigned int p = 0; p < microSetVector->size(); p++)
		setVector->push_back(microSetVector->at(p));

	//setArray[dimension++] = agentSet;
	//setArray[dimension++] = mesoSet;
	setVector->push_back(macroSet);

	MultiSet *preMultiSet = new MultiSet (setVector);
	MultiSet *postMultiSet = new MultiSet (postSet);

	preMultiSet->buildDataStructure();
	postMultiSet->buildDataStructure();
	
	/*
	 * Compute a data set of trajectories.
	 */
	int time = 0;
	int delay = 1;
	int trainSize = 100000;
	int testSize = 100;
	int trainLength = 1;
	int testLength = 0;
	
	VoterDataSet *DS = new VoterDataSet (VG, time, delay, trainSize, testSize, trainLength, testLength);
	//DS->print();
	
	/*
	 * Get the corresponding prediction data set.
	 */
	PredictionDataset *PDS = DS->getPredictionDataset (preMultiSet, postMultiSet);
	//PDS->print();
	
	/*
	 * Build the logarithmic score function associated to the prediction data set.
	 */
	int prior = 0;
	
	LogarithmicScore *score = new LogarithmicScore (PDS, prior);
	preMultiSet->setObjectiveFunction(score);

	/*
	 * Compute the values of the objective for each possible multi-dimensional bin of the pre-measurement.
	 */
	preMultiSet->computeObjectiveValues();
	//preMultiSet->printObjectiveValues();

	/*
	 * Compute the binning that optimises the logarithmic score.
	 */
	Partition *partition = preMultiSet->getOptimalPartition (0);
	partition->print();

	/*
	 * Compare to former algorithm.
	 */
	/*
	VoterBinning *binning = DS->getOptimalBinning (macroM, macroM, prior);
	binning->print(true);
	*/
	
	/*
	 * Free memory.
	 */
}

