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


#ifndef INCLUDE_PREDICTION_DATASET
#define INCLUDE_PREDICTION_DATASET

#include "multi_set.hpp"

/*!
 * \file prediction_dataset.hpp
 * \brief Class to represent a data set for the prediction of a post-measurement from the knowledge of a pre-measurement (composed of a train set and a test set)
 * \author Robin Lamarche-Perrin
 * \date 06/11/2015
 */

class MultiSet;
class MultiSubset;

/*!
 * \class PredictionDataset
 * \brief Class to represent a data set for the prediction of a post-measurement from the knowledge of a pre-measurement (composed of a train set and a test set)
 */
class PredictionDataset {

public:
	
	MultiSet *preMultiSet; /** \brief The pre-measurement modelled as a structured multi-dimensional set of elements (the possible observation values) */
	MultiSet *postMultiSet; /** \brief The post-measurement modelled as a structured multi-dimensional set of elements (the possible observation values) */

	std::vector<MultiSubset*> *trainPreValues; /** \brief A vector of pre-observations that are used to train the predictor */
	std::vector<MultiSubset*> *trainPostValues; /** \brief A vector of post-observations that are used to train the predictor */
	std::vector<int> *trainCountValues;

	std::vector<MultiSubset*> *testPreValues; /** \brief A vector of pre-observations that are used to test the predictor */
	std::vector<MultiSubset*> *testPostValues; /** \brief A vector of post-observations that are used to test the predictor */
	std::vector<int> *testCountValues;

	/*!
	 * \brief Constructor
	 * \param preMeasurement : The pre-measurement modelled as a structured multi-dimensional set of elements (the possible observation values)
	 * \param postMeasurement : The post-measurement modelled as a structured multi-dimensional set of elements (the possible observation values)
	 */
	PredictionDataset (MultiSet *preMeasurement, MultiSet *postMeasurement);

	
	/*!
	 * \brief Destructor
	 */
	~PredictionDataset ();

	
	/*!
	 * \brief Add a couple of (pre and post) observations to the train set
	 * \param preValue : A pointer to the feasible subset that have been pre-observed in the multi-dimensional set representing the pre-measurement. It should always be an element of the set, that is an atomic feasible subset
	 * \param preValue : A pointer to the feasible subset that have been post-observed in the multi-dimensional set representing the pre-measurement. It should always be an element of the set, that is an atomic feasible subset
	 * \param count : The number of times the couple has been observed
	 */
	void addTrainValue (MultiSubset *preValue, MultiSubset *postValue, int count = 1);

	
	/*!
	 * \brief Add a couple of (pre and post) observations to the test set
	 * \param preValue : A pointer to the feasible subset that has been pre-observed in the multi-dimensional set representing the pre-measurement. It should always be an element of the set, that is an atomic feasible subset
	 * \param preValue : A pointer to the feasible subset that has been post-observed in the multi-dimensional set representing the pre-measurement. It should always be an element of the set, that is an atomic feasible subset
	 * \param count : The number of times the couple has been observed
	 */
	void addTestValue (MultiSubset *preValue, MultiSubset *postValue, int count = 1);

	
	/*!
	 * \brief Add a couple of (pre and post) observations to the train set <b>in the case of one-dimensional measurements</b>
	 * \param preIndex : The index of the element that has been pre-observed in the one-dimensional set representing the pre-measurement
	 * \param postIndex : The index of the element that has been post-observed in the one-dimensional set representing the pre-measurement
	 * \param count : The number of times the couple has been observed
	 */
	void addTrainValue (int preIndex, int postIndex, int count = 1);

	
	/*!
	 * \brief Add a couple of (pre and post) observations to the test set <b>in the case of one-dimensional measurements</b>
	 * \param preIndex : The index of the element that has been pre-observed in the one-dimensional set representing the pre-measurement
	 * \param postIndex : The index of the element that has been post-observed in the one-dimensional set representing the pre-measurement
	 * \param count : The number of times the couple has been observed
	 */
	void addTestValue (int preIndex, int postIndex, int count = 1);

};

#endif
