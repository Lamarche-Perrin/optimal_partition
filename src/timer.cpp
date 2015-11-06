
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

