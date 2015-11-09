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



#include "prediction_dataset.hpp"


PredictionDataset::PredictionDataset (MultiSet *preM, MultiSet *postM)
{
	preMultiSet = preM;
	postMultiSet = postM;

	trainPreValues = new std::vector<MultiSubset*> ();
	trainPostValues = new std::vector<MultiSubset*> ();
	trainCountValues = new std::vector<int> ();

	testPreValues = new std::vector<MultiSubset*> ();
	testPostValues = new std::vector<MultiSubset*> ();
	testCountValues = new std::vector<int> ();
}


PredictionDataset::~PredictionDataset ()
{
	delete trainPreValues;
	delete trainPostValues;
	delete trainCountValues;

	delete testPreValues;
	delete testPostValues;
	delete testCountValues;
}


void PredictionDataset::addTrainValue (MultiSubset *preValue, MultiSubset *postValue, int count)
{
	trainPreValues->push_back(preValue);
	trainPostValues->push_back(postValue);
	trainCountValues->push_back(count);
}


void PredictionDataset::addTestValue (MultiSubset *preValue, MultiSubset *postValue, int count)
{
	testPreValues->push_back(preValue);
	testPostValues->push_back(postValue);
	testCountValues->push_back(count);
}


void PredictionDataset::addTrainValue (int preIndex, int postIndex, int count)
{
	addTrainValue(preMultiSet->getAtomicMultiSubset(preIndex), preMultiSet->getAtomicMultiSubset(postIndex), count);
}


void PredictionDataset::addTestValue (int preIndex, int postIndex, int count)
{
	addTestValue(preMultiSet->getAtomicMultiSubset(preIndex), preMultiSet->getAtomicMultiSubset(postIndex), count);
}



