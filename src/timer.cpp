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



#include <iostream>
#include <time.h>
#include <cstdio>
#include <stdio.h>

#include "timer.hpp"

int timer_id = 0;


Timer::Timer (char *fName, int dimension, bool append)
{
	id = timer_id++;
	fileName = fName;
	data = new std::list<DataPoint*>();

	time = 0;
	memory = 0;
	
	if (fileName != 0 && !append)
	{
		FILE *file;		
		if ((file = std::fopen(fileName,"w")) == NULL)
		{
			std::cerr << "Unable to open file " << fileName << " for writing." << std::endl;
			return;
		}

		for (int d = 0; d < dimension; d++) { fprintf(file,"PARAM%d,",d); }
		fprintf(file,"TIME,MEMORY\n");				
		fclose(file);
	}
}


Timer::~Timer ()
{
	for (std::list<DataPoint*>::iterator it = data->begin(); it != data->end(); ++it) { delete *it; }
	delete data;
}


void Timer::start (int size, std::string text)
{
	currentDataPoint = new DataPoint();
	currentDataPoint->parameters = std::vector<int>(1,size);
	
	if (text != "") { std::cout << text << ": "; }
	std::cout << "BEGIN TIMER " << id << " (size " << size << ")" << std::endl;
}


void Timer::start (std::vector<int> parameters, std::string text)
{
	currentDataPoint = new DataPoint();
	currentDataPoint->parameters = parameters;
	
	if (text != "") { std::cout << text << ": "; }
	std::cout << "BEGIN TIMER " << id << " (";
	for (unsigned int p = 0; p < parameters.size(); p++)
	{
		std::cout << "param" << p << " = " << parameters[p];
		if (p+1 < parameters.size()) { std::cout << ", "; }
	}
	std::cout << ")" << std::endl;
}


void Timer::startTime () { sTime = clock(); iTime = sTime; }
void Timer::startMemory () { sMemory = getMemory(); iMemory = sMemory; }


void Timer::stop (std::string text)
{
	clock_t clickNb = eTime - sTime;
	time = ((float)clickNb)/CLOCKS_PER_SEC;
	memory = eMemory - sMemory;

	if (text != "") { std::cout << text << ": "; }
	std::cout << "END TIMER " << id << " (" << time << " sec, " << memory << " bytes)" << std::endl;

	currentDataPoint->time = time;
	currentDataPoint->memory = memory;
	data->push_back(currentDataPoint);
	
	if (fileName != 0)
	{
		FILE *file;
		if ((file = std::fopen(fileName,"a")) == NULL)
		{
			std::cerr << "Unable to open file " << fileName << " for writing." << std::endl;
			return;
		}

		for (unsigned int p = 0; p < currentDataPoint->parameters.size(); p++) { fprintf(file,"%d,",currentDataPoint->parameters[p]);	}
		fprintf(file,"%f,%d\n",currentDataPoint->time,currentDataPoint->memory);	
		fclose(file);
	}
}


void Timer::stopTime () { eTime = clock(); }
void Timer::stopMemory () { eMemory = getMemory(); }

void Timer::step (std::string text)
{
	stopTime();
	stopMemory();

	clock_t clickNb = eTime - iTime;
	time = ((float)clickNb)/CLOCKS_PER_SEC;
	memory = eMemory - iMemory;

	if (text != "") { std::cout << text << ": "; }
	std::cout << "STEP TIMER " << id << " (" << time << " sec, " << memory << " bytes)" << std::endl;
	
	iTime = clock();
	iMemory = getMemory();
}

int getMemory ()
{
    /*
	PROCESS_MEMORY_COUNTERS memCounter;
	bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
	//std::cout << memCounter.WorkingSetSize << std::endl;
	return memCounter.WorkingSetSize;
    */
    return 0;
}

