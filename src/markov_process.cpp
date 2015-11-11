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


/*!
 * \author Robin Lamarche-Perrin
 * \date 22/01/2015
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <map>
#include <limits>

#include "markov_process.hpp"
#include "csv_tools.hpp"




MarkovProcess::MarkovProcess (int s)
{
    size = s;

    distribution = new double [size];
    distributions = new std::vector<double*>(1);
    distributions->at(0) = distribution;
    lastTime = 0;

    transition = new double [size*size];
    transitions = new std::vector<double*>(2);
    transitions->at(1) = transition;
    lastDelay = 1;
	
    double *trivialTransition = new double [size*size];
    for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			if (i == j) { trivialTransition[j*size+i] = 1; } else { trivialTransition[j*size+i] = 0; }
    transitions->at(0) = trivialTransition;
}


MarkovProcess::~MarkovProcess ()
{
    //delete[] distribution;
	for (std::vector<double*>::iterator it = distributions->begin(); it != distributions->end(); ++it) { delete [] (*it); }
    distributions->clear();
    delete distributions;

    //delete[] transition;
	for (std::vector<double*>::iterator it = transitions->begin(); it != transitions->end(); ++it) { delete [] (*it); }
    transitions->clear();
    delete transitions;	
}


void MarkovProcess::setDistribution (double *array)
{
    for (int i = 0; i < size; i++) { distribution[i] = array[i]; }
}


void MarkovProcess::setTransition (double *array)
{
    for (int j = 0; j < size; j++)
    { 
		for (int i = 0; i < size; i++) { transition[j*size+i] = array[j*size+i]; }
    }
}


void MarkovProcess::setTransition (int i, double *array)
{
    for (int j = 0; j < size; j++) { transition[j*size+i] = array[j]; }
}


double *MarkovProcess::getDistribution (int time)
{
    if (time == -1) { return distributions->at(lastTime); }
	
    if (lastTime < time)
    {
		distributions->resize(time+1);
		
		double *currentDist = getDistribution(lastTime);
		for (int t = lastTime + 1; t <= time; t++)
		{
			if (VERBOSE) { std::cout << "Distribution " << t << std::endl; }
			double *newDist = new double [size];
			for (int i = 0; i < size; i++)
			{
				newDist[i] = 0;
				for (int j = 0; j < size; j++) { newDist[i] += currentDist[j] * getNextProbability(i,j,1); }
			}
			distributions->at(t) = newDist;
			currentDist = newDist;
		}
		lastTime = time;
    }
	
    return distributions->at(time);
}


void MarkovProcess::computeStationaryDistribution (double threshold)
{
    if (lastTime == 0) { getDistribution(1); }
	
    double *dist1 = getDistribution(lastTime-1);
    double *dist2 = getDistribution(lastTime);

    bool stationary = true;
    for (int i = 0; i < size && stationary; i++) { stationary = std::abs(dist1[i] - dist2[i]) < threshold; }
	
    while (!stationary)
    {
		dist1 = dist2;
		dist2 = getDistribution(lastTime+1);
		
		stationary = true;
		for (int i = 0; i < size && stationary; i++) { stationary = std::abs(dist1[i] - dist2[i]) < threshold; }
    }
}


MarkovTrajectory *MarkovProcess::computeTrajectory (int time, int length)
{
    MarkovTrajectory *traj = new MarkovTrajectory(this,time,length);

    if (length > 0)
    {
		int state = 0;
		double *dist = getDistribution(time);
		double currentProb = dist[state];
		double r = ((double) rand() / (RAND_MAX));
		while (r > currentProb) {
			r -= currentProb;
			state++;
			currentProb = dist[state];
		}
		traj->states[0] = state;

		for (int l = 1; l < length; l++)
		{
			int nextState = 0;
			currentProb = transitions->at(1)[nextState*size+state];
			r = ((double) rand() / (RAND_MAX));
			while (r > currentProb) {
				r -= currentProb;
				nextState++;
				currentProb = transitions->at(1)[nextState*size+state];
			}
			state = nextState;
			traj->states[l] = state;
		}
    }

    return traj;
}


double *MarkovProcess::getTransition (int delay)
{
    if (lastDelay < delay)
    {
		transitions->resize(delay+1);
		
		double *currentTrans = getTransition(lastDelay);
		for (int d = lastDelay + 1; d <= delay; d++)
		{
			if (VERBOSE) { std::cout << "Transition " << d << " (size " << size << ")" << std::endl; }
			double *newTrans = new double [size*size];

			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					newTrans[j*size+i] = 0;
					for (int k = 0; k < size; k++)
						newTrans[j*size+i] += transition[j*size+k] * currentTrans[k*size+i];
				}
			}

			transitions->at(d) = newTrans;
			currentTrans = newTrans;

		}
		lastDelay = delay;
    }
	
    return transitions->at(delay);
}


double MarkovProcess::getProbability (int individual, int time)
{
    double *dist = getDistribution(time);
    return dist[individual];
}


double MarkovProcess::getProbability (Part *part, int time)
{
    double probability = 0;
    for (std::list<int>::iterator it = part->individuals->begin(); it != part->individuals->end(); ++it)
    {
		int individual = *it;
		probability += getProbability(individual,time);
    }
    return probability;
}


double MarkovProcess::getNextProbability (int nextIndividual, int currentIndividual, int delay)
{
    double *trans = getTransition(delay);
    return trans[nextIndividual*size+currentIndividual];
}


double MarkovProcess::getNextProbability (Part *nextPart, int currentIndividual, int delay)
{
    double probability = 0;
    for (std::list<int>::iterator it = nextPart->individuals->begin(); it != nextPart->individuals->end(); ++it)
    {
		int nextIndividual = *it;
		probability += getNextProbability(nextIndividual,currentIndividual,delay);
    }
    return probability;
}


double MarkovProcess::getNextProbability (Part *nextPart, Part *currentPart, int delay, int time)
{	
    double probability = 0;
    double condProbability = 0;
	
    for (std::list<int>::iterator currentIt = currentPart->individuals->begin(); currentIt != currentPart->individuals->end(); ++currentIt)
		condProbability += getProbability(*currentIt,time);

    for (std::list<int>::iterator currentIt = currentPart->individuals->begin(); currentIt != currentPart->individuals->end(); ++currentIt)
    {
		int currentIndividual = *currentIt;
		for (std::list<int>::iterator nextIt = nextPart->individuals->begin(); nextIt != nextPart->individuals->end(); ++nextIt)
		{
			int nextIndividual = *nextIt;
			probability += getProbability(currentIndividual,time) / condProbability * getNextProbability(nextIndividual,currentIndividual,delay);
		}
    }
    return probability;
}


double MarkovProcess::getEntropy (Partition *partition, int time)
{
    double entropy = 0;
	
    for (std::list<Part*>::iterator it1 = partition->parts->begin(); it1 != partition->parts->end(); ++it1)
    {
		double prob = getProbability(*it1,time);
		if (prob > 0) { entropy += - prob * log2(prob); }
    }
	
    return entropy;
}


double MarkovProcess::getMutualInformation (Partition *nextPartition, Partition *currentPartition, int delay, int time)
{
    int nextTime = time + delay;
    if (time == -1) { nextTime = -1; }
	
    double information = 0;
	
    for (std::list<Part*>::iterator it1 = currentPartition->parts->begin(); it1 != currentPartition->parts->end(); ++it1)
    {
		Part *currentPart = *it1;
		double probCurrentPart = getProbability(currentPart,time);
		
		for (std::list<Part*>::iterator it2 = nextPartition->parts->begin(); it2 != nextPartition->parts->end(); ++it2)
		{
			Part *nextPart = *it2;
			double probNextPart = getProbability(nextPart,nextTime);
			
			double condProb = getNextProbability(nextPart,currentPart,delay,time);
			if (condProb > 0 && probNextPart > 0) { information += probCurrentPart * condProb * log2(condProb / probNextPart); }
		}
    }
	
    return information;
}


double MarkovProcess::getPartMutualInformation (Partition *nextPartition, Part *currentPart, int delay, int time)
{
    int nextTime = time + delay;
    if (time == -1) { nextTime = -1; }
	
    double information = 0;
	
    for (std::list<Part*>::iterator it2 = nextPartition->parts->begin(); it2 != nextPartition->parts->end(); ++it2)
    {
		Part *nextPart = *it2;
		double probNextPart = getProbability(nextPart,nextTime);
			
		double condProb = getNextProbability(nextPart,currentPart,delay,time);
		if (condProb > 0 && probNextPart > 0) { information += condProb * log2(condProb / probNextPart); }
    }
	
    return information;
}


double MarkovProcess::getNextEntropy (Partition *partition, bool micro, int delay, int time)
{
    double entropy = 0;
	
    if (micro)
    {
		for (int currentIndividual = 0; currentIndividual < size; currentIndividual++)
		{
			for (std::list<Part*>::iterator nextIt = partition->parts->begin(); nextIt != partition->parts->end(); ++nextIt)
			{
				Part *nextPart = *nextIt;
				double nextProbability = getNextProbability(nextPart,currentIndividual,delay);
				if (nextProbability > 0)
					entropy += - getProbability(currentIndividual,time) * nextProbability * log2(nextProbability);
			}
		}		
    }
	
    else {
		for (std::list<Part*>::iterator currentIt = partition->parts->begin(); currentIt != partition->parts->end(); ++currentIt)
		{
			Part *currentPart = *currentIt;
			for (std::list<Part*>::iterator nextIt = partition->parts->begin(); nextIt != partition->parts->end(); ++nextIt)
			{
				Part *nextPart = *nextIt;
				double nextProbability = getNextProbability(nextPart,currentPart,delay,time);
				if (nextProbability > 0)
					entropy += - getProbability(currentPart,time) * nextProbability * log2(nextProbability);
			}
		}
    }
	
    return entropy;
}



std::set<OrderedPartition*> *MarkovProcess::getOptimalOrderedPartition (Partition *nextPartition, Partition *currentPartition, int delay, int time, double threshold)
{
    int nextTime = time + delay;
    if (time == -1) { nextTime = -1; }
	
    int microSize = currentPartition->size;
    int macroSize = microSize * microSize;
    int nextSize = nextPartition->size;

    // Compute entropy of pre-measurement
    double microProb [microSize];	
    for (int i = 0; i < microSize; i++)
		microProb[i] = getProbability(currentPartition->getPartFromValue(i),time);
	
    double macroProb [macroSize];
    for (int i = 0; i < microSize; i++)
		macroProb[i] = microProb[i];
    for (int j = 1; j < microSize; j++)
		for (int i = 0; i < microSize - j; i++)
			macroProb[i+j*microSize] = macroProb[i+(j-1)*microSize] + macroProb[i+j];
	
    double macroEntropy [macroSize];
    for (int j = 0; j < microSize; j++)
		for (int i = 0; i < microSize - j; i++)
		{
			if (macroProb[i+j*microSize] > 0) { macroEntropy[i+j*microSize] = - macroProb[i+j*microSize] * log2(macroProb[i+j*microSize]); }
			else { macroEntropy[i+j*microSize] = 0; }
		}

    // Compute mutual information between pre-measurement and post-measurement
    double microCondProb [microSize*nextSize];
    for (int i = 0; i < microSize; i++)
		for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
			microCondProb[i+(*it)->id*microSize] = getNextProbability(*it,currentPartition->getPartFromValue(i),delay,time);

    double macroCondProb [macroSize*nextSize];
    for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
		for (int i = 0; i < microSize; i++)
			macroCondProb[i+(*it)->id*macroSize] = microCondProb[i+(*it)->id*microSize] * microProb[i];			
    for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
		for (int j = 1; j < microSize; j++)
			for (int i = 0; i < microSize - j; i++)
				macroCondProb[i+j*microSize+(*it)->id*macroSize] = macroCondProb[i+(j-1)*microSize+(*it)->id*macroSize] + macroCondProb[i+j+(*it)->id*macroSize];
    for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
		for (int j = 0; j < microSize; j++)
			for (int i = 0; i < microSize - j; i++)
				macroCondProb[i+j*microSize+(*it)->id*macroSize] /= macroProb[i+j*microSize];

    double nextProb [nextSize];
    for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
		nextProb[(*it)->id] = getProbability(*it,nextTime);
	
    double macroInformation [macroSize];
    for (int j = 0; j < microSize; j++)
		for (int i = 0; i < microSize - j; i++)
		{
			macroInformation[i+j*microSize] = 0;
			for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
				if (macroCondProb[i+j*microSize+(*it)->id*macroSize] > 0)
					macroInformation[i+j*microSize] += macroCondProb[i+j*microSize+(*it)->id*macroSize]
						* log2(macroCondProb[i+j*microSize+(*it)->id*macroSize] / nextProb[(*it)->id]);
			macroInformation[i+j*microSize] = macroInformation[i+j*microSize] * macroProb[i+j*microSize];
		}

/*
// PRINT RESULTS
std::cout << "microProb" << std::endl;
for (int i = 0; i < microSize; i++) std::cout << microProb[i] << "\t";
std::cout << std::endl;
	
std::cout << "macroProb" << std::endl;
for (int j = 0; j < microSize; j++)
{
for (int i = 0; i < microSize - j; i++)
std::cout << macroProb[i+j*microSize] << "\t";
std::cout << std::endl;
}
std::cout << std::endl;
	
std::cout << "macroEntropy" << std::endl;
for (int j = 0; j < microSize; j++)
{
for (int i = 0; i < microSize - j; i++) std::cout << macroEntropy[i+j*microSize] << "\t";
std::cout << std::endl;
}
std::cout << std::endl;

std::cout << "microCondProb" << std::endl;
for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
{
std::cout << "v = " << (*it)->id << " -> ";
for (int i = 0; i < microSize; i++) std::cout << microCondProb[i+(*it)->id*microSize] << "\t";
std::cout << std::endl;
}
std::cout << std::endl;

std::cout << "macroCondProb" << std::endl;
for (std::list<Part*>::iterator it = nextPartition->parts->begin(); it != nextPartition->parts->end(); ++it)
{
std::cout << "v = " << (*it)->id << " -> ";
for (int j = 0; j < microSize; j++)
{
for (int i = 0; i < microSize - j; i++) std::cout << macroCondProb[i+j*microSize+(*it)->id*macroSize] << "\t";
std::cout << std::endl;
}
std::cout << std::endl;
}
std::cout << std::endl;

std::cout << "macroInformation" << std::endl;
for (int j = 0; j < microSize; j++)
{
for (int i = 0; i < microSize - j; i++) std::cout << macroInformation[i+j*microSize] << "\t";
std::cout << std::endl;
}
std::cout << std::endl;
*/

    // Ordered partitioning algorithm
    std::map<double,OrderedPartition*> pMap;
	
    OrderedPartition *pInf = new OrderedPartition(microSize,0);
    pInf->optimalCut = getOptimalCut(microSize,macroEntropy,macroInformation,pInf->beta);
    pMap[pInf->param] = pInf;

    OrderedPartition *pSup = new OrderedPartition(microSize,1);
    pSup->optimalCut = getOptimalCut(microSize,macroEntropy,macroInformation,pSup->beta);
    pMap[pSup->param] = pSup;

    std::list< std::pair<OrderedPartition*,OrderedPartition*> > pList;	
    pList.push_back(std::make_pair(pInf,pSup));

    while (!pList.empty())
    {
		pInf = pList.front().first;
		pSup = pList.front().second;
		pList.pop_front();

		int i = microSize;
		while (i > 0 && pInf->optimalCut[i-1] == pSup->optimalCut[i-1]) { i = pInf->optimalCut[i-1]; }
		
		if (i > 0 && pSup->beta - pInf->beta > threshold)
		{
			OrderedPartition *pNew = new OrderedPartition(microSize,(pSup->param+pInf->param)/2);
			pNew->optimalCut = getOptimalCut(microSize,macroEntropy,macroInformation,pNew->beta);
			pMap[pNew->param] = pNew;
			
			pList.push_back(std::make_pair(pInf,pNew));
			pList.push_back(std::make_pair(pNew,pSup));
		}
    }

    /*
    // PRINT
    OrderedPartition *previous = 0;
    for (std::map<double,OrderedPartition*>::iterator it = pMap.begin(); it != pMap.end(); it++)
    {
    OrderedPartition *next = it->second;

    int i = 1;
    if (previous != 0)
    {
    i = microSize;
    while (i > 0 && previous->optimalCut[i-1] == next->optimalCut[i-1]) { i = previous->optimalCut[i-1]; }
    }
    if (i > 0)
    {
    next->print();
    previous = next;
    }
    }
    */

    // Get results
    std::set<OrderedPartition*> *pSet = new std::set<OrderedPartition*>();

    OrderedPartition *previous = 0;
    for (std::map<double,OrderedPartition*>::iterator it = pMap.begin(); it != pMap.end(); it++)
    {
		OrderedPartition *current = it->second;

		int i = 1;
		if (previous != 0)
		{
			i = microSize;
			while (i > 0 && previous->optimalCut[i-1] == current->optimalCut[i-1]) { i = previous->optimalCut[i-1]; }
		}
		
		if (i > 0)
		{
			current->string = "|";
			current->entropy = 0;
			current->information = 0;

			i = microSize;
			while (i > 0)
			{
				int cut = current->optimalCut[i-1];

				if (cut == i-1) { current->string = "|" + int2string(cut) + current->string; }
				else { current->string = "|" + int2string(cut) + "-" + int2string(i-1) + current->string; }
				current->entropy += macroEntropy[cut+(i-1-cut)*microSize];
				current->information += macroInformation[cut+(i-1-cut)*microSize];

				i = cut;
			}
			
			pSet->insert(current);
			previous = current;
		}
    }
    return pSet;
}


int *MarkovProcess::getOptimalCut (int microSize, double *macroEntropy, double *macroInformation, double beta)
{
    int *optimalCut = new int [microSize];
    double optimalValue [microSize];
	
    for (int j = 0; j < microSize; j++)
    {
		optimalCut[j] = 0;
		if (beta == std::numeric_limits<double>::infinity()) { optimalValue[j] = - macroInformation[j*microSize]; }
		else { optimalValue[j] = macroEntropy[j*microSize] - beta * macroInformation[j*microSize]; }

		for (int cut = 1; cut <= j; cut++)
		{
			double value = optimalValue[cut-1];
			if (beta == std::numeric_limits<double>::infinity()) { value += - macroInformation[cut+(j-cut)*microSize]; }
			else { value += macroEntropy[cut+(j-cut)*microSize] - beta * macroInformation[cut+(j-cut)*microSize]; }
		 
			if (value < optimalValue[j])
			{
				optimalCut[j] = cut;
				optimalValue[j] = value;
			}
		}		
    }

    return optimalCut;
}	


double MarkovProcess::getInformationFlow (Partition *partition, int delay, int time)
{
    return getNextEntropy(partition,false,delay,time) - getNextEntropy(partition,true,delay,time);
}


void MarkovProcess::print ()
{
    int width = 6;
    int prec = 3;
	
    for (int t = 0; t <= lastTime; t++)
    {
		double *dist = getDistribution(t);

		std::cout << "p(X" << t << ") = {";
		if (size > 0) { std::cout << std::setw(width) << std::setprecision(prec) << dist[0]; }
		if (size > 1)
		{
			for (int i = 1; i < size; i++) { std::cout << ", " << std::setw(width) << std::setprecision(prec) << dist[i]; }
		}
		std::cout << "}" << std::endl << std::endl;
    }

    for (int t = 0; t <= lastDelay; t++)
    {
		double *trans = getTransition(t);

		for (int i = 0; i < size; i++)
		{
			std::cout << "p(X" << (t+1) << "|x" << i << ") = {";
			if (size > 0) { std::cout << std::setw(width) << std::setprecision(prec) << trans[0*size+i]; }
			if (size > 1)
			{
				for (int j = 1; j < size; j++) { std::cout << ", " << std::setw(width) << std::setprecision(prec) << trans[j*size+i]; }
			}
			std::cout << "}" << std::endl;
		}
		std::cout << std::endl;
    }
}



MarkovTrajectory::MarkovTrajectory (MarkovProcess *pr, int ti, int le)
{
    process = pr;
    time = ti;
    length = le;
    states = new int [length];
}


MarkovTrajectory::~MarkovTrajectory ()
{
    delete[] states;
}


void MarkovTrajectory::print (const int binary)
{
    if (binary > 0)
    {
		if (length > 0) {
			std::cout << "START:  ";
			for (int i = binary-1; i >= 0; i--) { std::cout << ((states[0] >> i) & 1); }
		}
	
		for (int l = 1; l < length; l++)
		{
			std::cout << "  ->  ";
			for (int i = binary-1; i >= 0; i--) { std::cout << ((states[l] >> i) & 1); }
		}
    }

    else {
		if (length > 0) { std::cout << "START:  " << states[0]; }
		for (int l = 1; l < length; l++) { std::cout << "  ->  " << states[l]; }
    }

    std::cout << std::endl;
}



MarkovDataSet::MarkovDataSet (MarkovProcess *pr, int si, int ti, int le)
{
    process = pr;
    size = si;
    time = ti;
    length = le;

    trajectories = new MarkovTrajectory *[size];
    for (int t = 0; t < size; t++) { trajectories[t] = process->computeTrajectory(time,length); }
}


MarkovDataSet::~MarkovDataSet ()
{
    for (int t = 0; t < size; t++) { delete trajectories[t]; }
    delete[] trajectories;
}


double MarkovDataSet::computeScore (Partition *preP, Partition *postP, int delay, int trainingLength)
{
    if (trainingLength + delay > length) { std::cout << "ERROR: the size of the training set is inconsistent!" << std::endl; return -1; }


    // Estimate the transition matrix by the empirical distribution on the training set

    // initialise
    double *trainDist = new double [preP->size * postP->size];
    for (int i = 0; i < preP->size; i++)
		for (int j = 0; j < postP->size; j++)
			trainDist[j * preP->size + i] = 0;

    // count occurrences
    for (int t = 0; t < size; t++)
    {
		MarkovTrajectory *traj = trajectories[t];
		for (int time = 0; time < trainingLength; time++)
		{
			Part *prePart = preP->findPart(traj->states[time]);
			Part *postPart = postP->findPart(traj->states[time+delay]);
			trainDist[postPart->id * preP->size + prePart->id]++;
		}
    }

    // normalise
    for (int i = 0; i < preP->size; i++)
    {
		double sum = 0;
		for (int j = 0; j < postP->size; j++)
			sum += trainDist[j * preP->size + i];
		if (sum > 0)
			for (int j = 0; j < postP->size; j++)
				trainDist[j * preP->size + i] /= sum;
    }

    // print
    /*
	  int width = 10; int prec = 4;

	  std::cout << "        ";
	  for (int j = 0; j < postP->size; j++) { std::cout << std::setw(width+2) << j; }
	  std::cout << std::endl;

	  for (int i = 0; i < preP->size; i++)
	  {
	  std::cout << "p(.|" << i << ") = {";
	  if (postP->size > 0) { std::cout << std::setw(width) << std::setprecision(prec) << trainDist[0 * preP->size + i]; }
	  if (postP->size > 1)
	  {
	  for (int j = 1; j < postP->size; j++) { std::cout << ", " << std::setw(width) << std::setprecision(prec) << trainDist[j * preP->size + i]; }
	  }
	  std::cout << "}" << std::endl;
	  }
	  std::cout << std::endl;
    */

    // compute score
    int count = 0;
    double score = 0;
    for (int t = 0; t < size; t++)
    {
		MarkovTrajectory *traj = trajectories[t];
		for (int time = trainingLength; time < length - delay; time++)
		{
			Part *prePart = preP->findPart(traj->states[time]);
			Part *postPart = postP->findPart(traj->states[time+delay]);
			double prob = trainDist[postPart->id * preP->size + prePart->id];
			if (prob == 0) { return -1; }

			score += -log(prob);
			count++;
		}
    }
    if (count > 0) { return score/count; }
    else { return 0; }
}


long unsigned int nChoosek (int n, int k)
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;
	
    long unsigned int result = n;
    for( int i = 2; i <= k; ++i ) {
		result *= (n-i+1);
		result /= i;
    }
    return result;
}
