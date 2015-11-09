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


#ifndef INCLUDE_TIMER
#define INCLUDE_TIMER

#include <time.h>
#include <list>
#include <vector>
#include <string>


typedef struct DataPointStruct
{
	std::vector<int> parameters;
	float time;
	int memory;
} DataPoint;


class Timer
{
public:
	float time;
	int memory;

	Timer (char *file = 0, int dimension = 1, bool append = false);
	~Timer ();

	void start (int size, std::string text = "");
	void start (std::vector<int> parameters, std::string text = "");
	void startTime ();
	void startMemory ();
		
	void stop (std::string text = "");
	void stopTime ();
	void stopMemory ();

	void step (std::string text = "");
	void print (char *fName);

private:
	int id;
	char *fileName;
	clock_t sTime;
	clock_t eTime;
	clock_t iTime;
		
	int sMemory;
	int eMemory;
	int iMemory;
	
	std::list<DataPoint*> *data;
	DataPoint *currentDataPoint;
};

int getMemory ();

#endif
