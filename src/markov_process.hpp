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


#ifndef INCLUDE_MARKOV_PROCESS
#define INCLUDE_MARKOV_PROCESS

/*!
 * \file markov_process.hpp
 * \brief Class to build a finite Markov chain
 * \author Robin Lamarche-Perrin
 * \date 22/01/2015
 */

#include <vector>

#include "partition.hpp"

long unsigned int nChoosek (int n, int k);

class MarkovTrajectory;

/*!
 * \class MarkovProcess
 * \brief A finite Markov chain described by a discrete state space, an initial distribution, and a transition kernel
 */
class MarkovProcess
{
public:
    int size;						     	/*!< The size of the Markov chain state space*/

    double *distribution;					/*!< The initial probability distribution of the system state (time 0)*/
    std::vector<double*> *distributions;	/*!< A vector of probability distributions through time (from 0 to lastTime)*/
    int lastTime;							/*!< The time of the furthest computed probability distribution in the distributions vector*/

    double *transition;						/*!< The transition kernel of the Markov chain (1 step)*/
    std::vector<double*> *transitions;		/*!< A vector of transition kernels for several steps (from 1 to lastDelay)*/
    int lastDelay;							/*!< The delay of the furthest computed transition kernel in the transitions vector*/
				
    /*!
     * \brief Constructor
     * \param size : The size of the Markov chain state space
     */
    MarkovProcess (int size);

    /*!
     * \brief Destructor
     */
    ~MarkovProcess ();

    /*!
     * \brief Print the Markov chain structure and details
     */
    void print();

    /*!
     * \brief Set the initial distribution
     * \param array : An array of probabilities (summing to 1) with the size of the Markov chain state space
     */
    void setDistribution (double *array);

    /*!
     * \brief Set the transition kernel
     * \param array : A 2D-array of probabilities (summing to 1 within each row) with the square of the size of the Markov chain state space
     */
    void setTransition (double *array);

    /*!
     * \brief Set one line of the transition kernel
     * \param j : The index of the row to be set
     * \param array : An array of probabilities (summing to 1) with the size of the Markov chain state space
     */
    void setTransition (int i, double *array);

    /*!
     * \brief Get the state distribution at a given time (-1 for the stationary distribution)
     */
    double *getDistribution (int time);

    /*!
     * \brief Get the transition kernel for a given number of simulation steps (delay)
     */
    double *getTransition (int delay);

    /*!
     * \brief Compute the stationary distribution of the Markov chain by iterating the transition kernel
     * \param threshold : Determines stationarity by giving the minimal difference between two probability values in two different but consecutive distributions
     * \warning Will endlessly loop for periodic Markov chains
     */
    void computeStationaryDistribution (double threshold);

    /*!
     * \brief Compute a possible trajectory of the Markov chain
     * \param length : Length of the trajectory
     */
    MarkovTrajectory *computeTrajectory (int time, int length);

    /*!
     * \brief Get the probability to be in a given state at a given time (-1 for the stationary distribution)
     */
    double getProbability (int individual, int currentTime);

    /*!
     * \brief Get the probability to be in a given subset of states at a given time (-1 for the stationary distribution)
     */
    double getProbability (Part *part, int currentTime);

    /*!
     * \brief Get the probability to be in a given state after a given delay knowing the current state
     */
    double getNextProbability (int nextIndividual, int currentIndividual, int delay);

    /*!
     * \brief Get the probability to be in a given subset of states after a given delay knowing the current state
     */
    double getNextProbability (Part *nextPart, int currentIndividual, int delay);

    /*!
     * \brief Get the probability to be in a given subset of states after a given delay knowing the current subset of states at a given time (-1 for the stationary distribution)
     */
    double getNextProbability (Part *nextPart, Part *currentPart, int delay, int time);

    /*!
     * \brief Get the Shannon entropy of the state distribution at a given time (-1 for the stationary distribution) when lumped according to a given partition of the state space
     */
    double getEntropy (Partition *partition, int currentTime);

    /*!
     * \brief Get the mutual information between the state distribution at a given time (-1 for the stationary distribution) and the state distribution after a given delay, when both are lumped according to a given partition of the state space
     */
    double getMutualInformation (Partition *nextPartition, Partition *currentPartition, int delay, int time);

    double getPartMutualInformation (Partition *nextPartition, Part *currentPart, int delay, int time);

    /*!
     * \brief Get the Shannon entropy of the next state distribution knowing the current state distribution at a given time (-1 for the stationary distribution), when the next distribution is lumped according to a given partition of the state space, and when the current partition is also lumped by the same partition (when micro is false)
     */
	double getNextEntropy (Partition *partition, bool micro, int delay, int time);

    /*!
     * \brief Get the information flow at a given time (-1 for the stationary distribution) between the Markov chain lumped according to a given partition of the state space and the microscopic Markov chain
     */
    double getInformationFlow (Partition *partition, int delay, int time);

    int *getOptimalCut (int microSize, double *macroEntropy, double *macroInformation, double beta);
    std::set<OrderedPartition*> *getOptimalOrderedPartition (Partition *nextPartition, Partition *currentPartition, int delay, int time, double threshold);
};


class MarkovTrajectory
{
public:
    MarkovProcess *process;
    int time;
    int length;
    int *states;

    MarkovTrajectory (MarkovProcess *process, int time, int length);
    ~MarkovTrajectory ();
    
    void print (int binary = 0);
};


class MarkovDataSet
{
public:
    MarkovProcess *process;

    int size;
    int time;
    int length;
    MarkovTrajectory **trajectories;

    MarkovDataSet (MarkovProcess *process, int size, int time, int length);
    ~MarkovDataSet ();

    double computeScore (Partition *preP, Partition *postP, int delay, int trainingLength);
};


#endif
