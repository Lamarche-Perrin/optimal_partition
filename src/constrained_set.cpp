
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "orderedset.hpp"
#include "constrained_set.hpp"
#include "timer.hpp"
#include "csv_tools.hpp"
#include "relative_entropy.hpp"


ConstrainedSet::~ConstrainedSet() {}


void ConstrainedSet::printOptimalPartitionList (double threshold)
{
	PartitionList *partitionList = getOptimalPartitionList(threshold);
	
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it)
	{
		Partition *p = *it;
		p->print();
	}
	
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it) { delete *it; }
	delete partitionList;
}


void ConstrainedSet::printOptimalPartitionListInCSV (double threshold, Dataset *data, int dim, std::string fileName)
{
	PartitionList *partitionList = getOptimalPartitionList(threshold);
	
	std::ofstream file;
	openOutputCSV(file,fileName,true);

	addCSVField(file,"PARTITION");
	addCSVField(file,"PARAMETER");

	if (dim == 0) { addCSVField(file,"PART_1"); addCSVField(file,"PART_2"); } else { addCSVField(file,"PART"); }
	
	addCSVField(file,"NB_ITEM_TAG");
	addCSVField(file,"NB_ITEM");
	addCSVField(file,"COMPLEXITY_REDUCTION");
	addCSVField(file,"INFORMATION_LOSS",false);
	endCSVLine(file);

	int num = 1;
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it)
	{
		Partition *p = *it;
		
		for (std::list<Part*>::iterator it1 = p->parts->begin(); it1 != p->parts->end(); ++it1)
		{
			Part *part = *it1;
			RelativeQuality *q = (RelativeQuality*) part->quality;
	
			addCSVField(file,num);
			addCSVField(file,p->parameter);
			
			if (dim == 0)
			{
				BiPart *bpart = (BiPart*) part;
				std::string partTxt1 = "";	bool first1 = true;		
				for (std::list<int>::iterator it2 = bpart->firstPart->individuals->begin(); it2 != bpart->firstPart->individuals->end(); ++it2)
				{
					if (!first1) { partTxt1 += ","; }
					partTxt1 += data->getLabel1(*it2);
					first1 = false;
				}
				addCSVField(file,partTxt1);
	
				std::string partTxt2 = ""; bool first2 = true;
				for (std::list<int>::iterator it2 = bpart->secondPart->individuals->begin(); it2 != bpart->secondPart->individuals->end(); ++it2)
				{
					if (!first2) { partTxt2 += ","; }
					partTxt2 += data->getLabel2(*it2);
					first2 = false;
				}
				addCSVField(file,partTxt2);
			}

			else {
				std::string partTxt = "";	bool first = true;		
				for (std::list<int>::iterator it2 = part->individuals->begin(); it2 != part->individuals->end(); ++it2)
				{
					if (!first) { partTxt += ","; }
					if (dim == 1) { partTxt += data->getLabel1(*it2); }
					if (dim == 2) { partTxt += data->getLabel2(*it2); }
					first = false;
				}
				addCSVField(file,partTxt);
			}
	
			addCSVField(file,(int) q->sumValue);
			addCSVField(file,(int) q->sumRefValue);
			addCSVField(file,q->sizeReduction);
			addCSVField(file,q->divergence,false);
	
			endCSVLine(file);
		}
		
		num++;
	}

	closeOutputCSV(file);
	
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); ++it) { delete *it; }
	delete partitionList;

}


PartitionList *ConstrainedSet::getOptimalPartitionList (double threshold)
{
	PartitionList *partitionList = new PartitionList();

	Partition *minPartition = getOptimalPartition(measure->getParameter(0));
	Partition *maxPartition = getOptimalPartition(measure->getParameter(1));

	partitionList->push_back(minPartition);
	addOptimalPartitionList(partitionList,minPartition,maxPartition,0,1,threshold);
	partitionList->push_back(maxPartition);
	
	Partition *currentPartition;
	bool first = true;
	
	
	for (PartitionList::iterator it = partitionList->begin(); it != partitionList->end(); )
	{
		Partition *newPartition = *it;
		if (first)
		{
			currentPartition = newPartition;
			first = false;
			++it;
		}
		else
		{
			if (!newPartition->equal(currentPartition))
			{
				currentPartition = newPartition;
				++it;
			}
			else
			{
				partitionList->erase(it++);
				delete newPartition;
			}
		}
	}
	
	return partitionList;
}


void ConstrainedSet::addOptimalPartitionList (PartitionList *partitionList, Partition *minPartition, Partition *maxPartition,
	double minUnit, double maxUnit, double threshold)
{
	if (minPartition->equal(maxPartition) || (measure->getUnitDistance(minUnit,maxUnit) <= threshold)) return;

	double newUnit = measure->getIntermediaryUnit(minUnit,maxUnit);
	Partition *newPartition = getOptimalPartition(measure->getParameter(newUnit));
	
	addOptimalPartitionList(partitionList,minPartition,newPartition,minUnit,newUnit,threshold);
	partitionList->push_back(newPartition);
	addOptimalPartitionList(partitionList,newPartition,maxPartition,newUnit,maxUnit,threshold);
}


void ConstrainedSet::testAggregationAlgorithm (bool verbose, int param)
{
/*	ConstrainedSet *set;
	set = getRandomSet(5);
	set->print();

	set->computeQuality();
	set->normalizeQuality();
	if (verbose) set->printQuality();

	if (param == -1) { set->computeOptimalPartition(rand()/(double)RAND_MAX); }
	else { set->computeOptimalPartition(param); }
	set->printOptimalPartition();

	delete set;*/
}

void ConstrainedSet::complexityAnalysis (char *fName, int start, int stop, int step, int number)
{
/*	Timer t = Timer(fName);

	for (int s = start; s <= stop; s += step)
	{
		for (int i = 0; i < number; i++)
		{
			t.start(s);
	
			t.startMemory();
			ConstrainedSet *set;
			set = this->getRandomSet(s);
	
			t.startTime();
			set->computeQuality();
			set->normalizeQuality();
			set->computeOptimalPartition(rand()/(double)RAND_MAX);
			t.stopTime();
	
			t.stopMemory();
			t.stop();

			delete set;
		}
	}*/
}

