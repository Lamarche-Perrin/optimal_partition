#include <iostream>
#include <cstdlib>
#include <utility>
#include <list>
#include <math.h>

#include "ring.hpp"


Ring::Ring (int s)
{
	size = s;
	int s2 = (s-1)*s+1;
	
	values = new double [s];

	sumValues = new double [s2];
	microInfos = new double [s2];
	sizeReductions = new double [s2];
	divergences = new double [s2];

	optimalQualities = new double [s2];
	optimalCuts = new int [s2-1];
}


void Ring::setObjectiveFunction (ObjectiveFunction *m) {}
void Ring::setRandom () {}


AbstractSet *Ring::getRandomSet (int s)
{
	Ring *set = new Ring(s);
	for (int v = 0; v < set->size; v++) { set->values[v] = rand() % 1024; }
	return set;
}


Ring::~Ring ()
{
	delete[] values;
	
	delete[] sumValues;
	delete[] microInfos;
	delete[] sizeReductions;
	delete[] divergences;
	
	delete[] optimalQualities;
	delete[] optimalCuts;
}


int Ring::getIndex (int i, int j) { return i%size+((j-i+size)%size)*size; }


void Ring::print ()
{
	if (size == 0) return;
	std::cout << values[0];
	for (int i = 1; i < size; i++) { std::cout << ", " << values[i]; }
	std::cout << std::endl;
}


void Ring::buildDataStructure () {}


void Ring::computeObjectiveValues ()
{
	for (int i = 0; i < size; i++)
	{
		int newIndex = getIndex(i,i);
					
		sumValues[newIndex] = values[i];
		if (values[i] > 0) { microInfos[newIndex] = - values[i] * log2(values[i]); }
		else { microInfos[newIndex] = 0; }
		sizeReductions[newIndex] = 0;
		divergences[newIndex] = 0;
	}

	for (int s = 1; s < size; s++)
	{
		for (int i = 0; i < size; i++)
		{
			if (s == size-1 && i > 0) break;
			
			int j = i+s;
			int newIndex = getIndex(i,j);
			int index1 = getIndex(i,j-1);
			int index2 = getIndex(j,j);
			
			//std::cout << "[" << i << "-" << (j%size) << "] -> " << newIndex << " = " << index1 << " + " << index2 << std::endl;
			
			sumValues[newIndex] = sumValues[index1] + sumValues[index2];
			microInfos[newIndex] = microInfos[index1] + microInfos[index2];
			sizeReductions[newIndex] = s;
			if (sumValues[newIndex] > 0) { divergences[newIndex] = - microInfos[newIndex] - sumValues[newIndex] * log2(sumValues[newIndex] / (s+1)); }
			else { divergences[newIndex] = 0; }
		}
	}
}


void Ring::normalizeObjectiveValues ()
{
	double maxSizeReduction = sizeReductions[getIndex(0,size-1)];
	double maxDivergence = divergences[getIndex(0,size-1)];

	for (int s = 1; s < size; s++)
	{
		for (int i = 0; i < size; i++)
		{
			if (s == size-1 && i > 0) break;
			
			int j = i+s;
			int newIndex = getIndex(i,j);

			sizeReductions[newIndex] /= maxSizeReduction;
			divergences[newIndex] /= maxDivergence;
		}
	}
}


void Ring::printObjectiveValues ()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = i; j < i+size-1; j++)
		{
			if (i==j) { std::cout << "[" << i << "]" << std::endl;; }
			else { std::cout << "[" << i << "-" << (j%size) << "]" << std::endl;; }
			std::cout << " -> values = " << sumValues[getIndex(i,j)] << std::endl;
			std::cout << " -> infos  = " << microInfos[getIndex(i,j)] << std::endl;
			std::cout << " -> gain   = " << sizeReductions[getIndex(i,j)] << std::endl;
			std::cout << " -> loss   = " << divergences[getIndex(i,j)] << std::endl;
		}
	}
	
	int i = 0;
	int j = size-1;
	std::cout << "[" << i << "-" << (j%size) << "]";
	std::cout << " -> values = " << sumValues[getIndex(i,j)] << std::endl;
	std::cout << " -> infos  = " << microInfos[getIndex(i,j)] << std::endl;
	std::cout << " -> gain   = " << sizeReductions[getIndex(i,j)] << std::endl;
	std::cout << " -> loss   = " << divergences[getIndex(i,j)] << std::endl;
}


void Ring::computeOptimalPartition (double parameter)
{
	
	for (int i = 0; i < size-1; i++)
	{
		for (int j = i; j < size-1; j++)
		{
			optimalQualities[getIndex(i,j)] = parameter * sizeReductions[getIndex(i,j)] - (1-parameter) * divergences[getIndex(i,j)];
			optimalCuts[getIndex(i,j)] = i;
			
			for (int cut = i+1; cut <= j; cut++)
			{
			    double value = optimalQualities[getIndex(i,cut-1)] + parameter * sizeReductions[getIndex(cut,j)] - (1-parameter) * divergences[getIndex(cut,j)];
				if (value > optimalQualities[getIndex(i,j)])
				{
					optimalQualities[getIndex(i,j)] = value;
					optimalCuts[getIndex(i,j)] = cut;			
				}
			}
		} 
	}
		
	firstOptimalCut = 0;
	lastOptimalCut = size-1;
	optimalQualities[getIndex(0,size-1)] = parameter * sizeReductions[getIndex(0,size-1)] - (1-parameter) * divergences[getIndex(0,size-1)];

	for (int i = 0; i < size-1; i++)
	{
		for (int j = i; j < size-1; j++)
		{
			double value = optimalQualities[getIndex(i,j)] + parameter * sizeReductions[getIndex(j+1,i-1)] - (1-parameter) * divergences[getIndex(j+1,i-1)];
			if (value > optimalQualities[getIndex(0,size-1)])
			{	
				firstOptimalCut = i;
				lastOptimalCut = j;
				optimalQualities[getIndex(0,size-1)] = value;
			}
		}
	}
}


void Ring::printOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	std::list< std::pair<int,int> > cutList = std::list< std::pair<int,int> >();

	if (firstOptimalCut == 0 && lastOptimalCut == size-1) { cutList.push_front(std::make_pair(0,size-1)); }
	else {
			
		cutList.push_front(std::make_pair(lastOptimalCut+1,(firstOptimalCut-1+size)%size));

		int first = firstOptimalCut;
		int last = lastOptimalCut;
		int cut = optimalCuts[getIndex(first,last)];

		while (cut > firstOptimalCut)
		{
			cutList.push_front(std::make_pair(cut,last));
			last = cut-1;
			cut = optimalCuts[getIndex(first,last)];
		}

		cutList.push_front(std::make_pair(cut,last));
	}

	while (!cutList.empty())
	{
		std::pair<int,int> cut = cutList.front();
		if (cut.first != cut.second) { std::cout << "[" << cut.first << "-" << cut.second << "] "; }
		else { std::cout << "[" << cut.first << "] "; }
		cutList.pop_front();
	}
	std::cout << std::endl;
}


Partition *Ring::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	return 0;
}


