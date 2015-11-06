#include <iostream>
#include <utility>
#include <list>
#include <math.h>

#include "orderedset.hpp"


OrderedSet::OrderedSet (int s)
{
	size = s;
	int s2 = (s+1)*s/2;
	
	qualities = new Quality* [s2];
	optimalValues = new double [s];
	optimalCuts = new int [s];
}


void OrderedSet::setRandom () {}

void OrderedSet::buildDataStructure () {}

OrderedSet::~OrderedSet ()
{
	delete[] qualities;
	delete[] optimalValues;
	delete[] optimalCuts;
}


void OrderedSet::setMeasure (Measure *m)
{
	measure = m;
	
	for (int i = 0; i < size; i++) { qualities[getIndex(i,0)] = measure->newQuality(i); }

	for (int j = 1; j < size; j++)
	{
		for (int i = 0; i < size-j; i++) { qualities[getIndex(i,j)] = measure->newQuality(); }
	}
}


int OrderedSet::getIndex (int i, int j) { return j*size-j*(j-1)/2+i; }


void OrderedSet::print ()
{
	for (int i = 0; i < size; i++)
	{
		std::cout << "[" << i << "] -> ";
		qualities[getIndex(i,0)]->print(false);
	}
}


void OrderedSet::computeQuality ()
{
	for (int i = 0; i < size; i++)
		qualities[getIndex(i,0)]->compute();
		
	for (int j = 1; j < size; j++)
		for (int i = 0; i < size-j; i++)
			qualities[getIndex(i,j)]->compute(qualities[getIndex(i,j-1)],qualities[getIndex(i+j,0)]);
}


void OrderedSet::normalizeQuality ()
{
	Quality *maxQual = qualities[getIndex(0,size-1)];

	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
			qualities[getIndex(i,j)]->normalize(maxQual);
}


void OrderedSet::printQuality ()
{
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size-j; i++)
		{
			std::cout << "[" << i << "," << j << "] -> ";
			qualities[getIndex(i,j)]->print(true);
		}
}


void OrderedSet::computeOptimalPartition (double parameter)
{
	optimalValues[0] = qualities[getIndex(0,0)]->getValue(parameter);
	optimalCuts[0] = 0;
	
	for (int j = 1; j < size; j++)
	{
		int currentCut = 0;
		float currentValue = qualities[getIndex(0,j)]->getValue(parameter);
		
		for (int cut = 1; cut < j+1; cut++)
		{
			float value = optimalValues[cut-1] + qualities[getIndex(cut,j-cut)]->getValue(parameter);
			if ((measure->maximize && value > currentValue) || (!measure->maximize && value < currentValue))
			{
				currentValue = value;
				currentCut = cut;
			}
		}
		
		optimalValues[j] = currentValue;
		optimalCuts[j] = currentCut;
	}
}


void OrderedSet::printOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);
	
	std::list< std::pair<int,int> > cutList = std::list< std::pair<int,int> >();

	int last;
	int first = size;
	while (first > 0)
	{
		last = first-1;
		first = optimalCuts[last];
		cutList.push_front(std::make_pair(first,last));
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


Partition *OrderedSet::getOptimalPartition (double parameter)
{
	computeOptimalPartition(parameter);

	Partition *partition = new Partition(measure,parameter);
//	std::list<Part*> *partList = new std::list<Part*>();
	
	int i;
	int k = size-1;
	
	while (k >= 0)
	{
		i = optimalCuts[k];
		int j = k-i;

		Part *part = new Part(qualities[getIndex(i,j)]);
//		part->quality->print();
		for (int l = i; l <= k; l++) { part->addIndividual(l); }
		partition->addPart(part,true);
//		partList->push_front(part);
		
		k = i - 1;
	}

//	for (std::list<Part*>::iterator it = partList->begin(); it != partList->end(); ++it) { partition->addPart(*it); }
	
	return partition;
}



/*

void OrderedSet::printPartitions (vector<part> partitions) {
	for (vector<part>::iterator it = partitions.begin(); it != partitions.end(); ++it) {
		printPartition(*it);
		cout << endl;
	}
}


void OrderedSet::writeAllPartitionsToCSV (vector<part> &partitions, string fileName) {
	ofstream file;
	openOutputCSV(file,fileName+"-PARTITIONS.csv");

	vector<string> headLine;
	headLine.push_back("ID");
	headLine.push_back("COEFF");
	headLine.push_back("SIZE");
	headLine.push_back("NORM_SIZE_REDUCTION");
	headLine.push_back("NORM_KL_DIVERGENCE");
	headLine.push_back("MAX_SIZE_REDUCTION");
	headLine.push_back("MAX_KL_DIVERGENCE");
	addCSVLine(file,headLine);

	int id = 0;	
	for (vector<part>::iterator it = partitions.begin(); it != partitions.end(); ++it) {
		part *p = &(*it);

		vector<string> line;
		line.push_back(int2string(id));
		line.push_back(float2string(p->coefficient));
		line.push_back(int2string(p->size));
		line.push_back(float2string(p->normSizeReduction));
		line.push_back(float2string(p->normDivergence));
		line.push_back(float2string(p->maxSizeReduction));
		line.push_back(float2string(p->maxDivergence));
		addCSVLine(file,line);

		id++;		
	}
	closeOutputCSV(file);
}


void OrderedSet::writeEachPartitionToCSV (vector<part> &partitions, string fileName) {

	vector<string> headLine;
	headLine.push_back("BEGIN_LABEL");
	headLine.push_back("END_LABEL");
	headLine.push_back("SIZE");
	headLine.push_back("VALUE");
	headLine.push_back("REF_VALUE");
	headLine.push_back("NORM_SIZE_REDUCTION");
	headLine.push_back("NORM_KL_DIVERGENCE");

	int id = 0;	
	for (vector<part>::iterator it = partitions.begin(); it != partitions.end(); ++it) {
		part *p = &(*it);
				
		stringstream ss; ss << id;
		string idStr = ss.str();

		ofstream file;
		openOutputCSV(file,fileName+"-PARTITION-"+idStr+".csv");
		addCSVLine(file,headLine);

		for (int i = 0; i < p->size; i++) {
			vector<string> line;
			line.push_back(p->beginLabels[i]);
			line.push_back(p->endLabels[i]);
			line.push_back(int2string(p->sizes[i]));
			line.push_back(float2string(p->values[i]));
			line.push_back(float2string(p->refValues[i]));
			line.push_back(float2string(p->normSizeReductions[i]));
			line.push_back(float2string(p->normDivergences[i]));
			addCSVLine(file,line);
		}
		closeOutputCSV(file);

		id++;		
	}
}

*/
