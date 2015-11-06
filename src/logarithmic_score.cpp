
#include <iostream>
#include <iomanip>
#include <math.h>

#include "logarithmic_score.hpp"


LogarithmicScore::LogarithmicScore (PredictionDataset *ds, int p)
{
	dataset = ds;
	maximize = false;
	prior = p;

	preSize = ds->preStructure->atomicAggregateNumber;
	postSize = ds->postStructure->atomicAggregateNumber;

	trainCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = prior * preSize; }
	trainCountTotal = prior * preSize * postSize;
}


LogarithmicScore::~LogarithmicScore ()
{
	delete [] trainCountArray;
}


void LogarithmicScore::setRandom () {}


Quality *LogarithmicScore::newQuality (int id)
{
	LogarithmicQuality *rq;
	rq = new LogarithmicQuality (this);
	return rq;
}


void LogarithmicScore::computeQuality ()
{
	for (unsigned int n = 0; n < dataset->trainPreValues->size(); n++)
	{
		HyperAggregate *preValue = dataset->trainPreValues->at(n);
		HyperAggregate *postValue = dataset->trainPostValues->at(n);
		int countValue = dataset->trainCountValues->at(n);

		trainCountArray[postValue->atomicNum] += countValue;
		trainCountTotal += countValue;

		((LogarithmicQuality*)preValue->quality)->trainCountArray[postValue->atomicNum] += countValue;
		((LogarithmicQuality*)preValue->quality)->trainCountTotal += countValue;
	}

	for (unsigned int n = 0; n < dataset->testPreValues->size(); n++)
	{
		HyperAggregate *preValue = dataset->testPreValues->at(n);
		HyperAggregate *postValue = dataset->testPostValues->at(n);
		int countValue = dataset->testCountValues->at(n);

		((LogarithmicQuality*)preValue->quality)->testCountArray[postValue->atomicNum] += countValue;
		((LogarithmicQuality*)preValue->quality)->testCountTotal += countValue;
	}
}


void LogarithmicScore::printQuality (bool v)
{
	if (v)
	{
		std::cout << std::endl
				  << "   trainCountTotal = " << std::setw(5) << std::setprecision(5) << trainCountTotal << std::endl
				  << "   trainCountArray = [" << std::setw(5) << std::setprecision(5) << trainCountArray[0] << ", ";
		for (int l = 1; l < postSize; l++) { std::cout << std::setw(5) << std::setprecision(5) << trainCountArray[l]; }
		std::cout << "]" << std::endl;
	}
}


double LogarithmicScore::getParameter (double unit) { return 0; }

double LogarithmicScore::getUnitDistance (double uMin, double uMax) { return 0; }

double LogarithmicScore::getIntermediaryUnit (double uMin, double uMax) { return 0; }



LogarithmicQuality::LogarithmicQuality (LogarithmicScore *m)
{
	measure = m;
	preSize = m->preSize;
	postSize = m->postSize;
	
	trainCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = m->prior; }
	trainCountTotal = m->prior * postSize;

	testCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }
	testCountTotal = 0;
}


LogarithmicQuality::~LogarithmicQuality ()
{
	delete [] trainCountArray;
	delete [] testCountArray;
}


void LogarithmicQuality::add (Quality *q) {}


void LogarithmicQuality::compute ()
{
	if (trainCountTotal > 0)
	{
		score = testCountTotal * log10 (trainCountTotal);
		for (int l = 0; l < postSize; l++) { score -= testCountArray[l] * log10 (trainCountArray[l]); }
	}
	
	else {
		score = testCountTotal * log10 (((LogarithmicScore*)measure)->trainCountTotal);
		for (int l = 0; l < postSize; l++) { score -= testCountArray[l] * log10 (((LogarithmicScore*)measure)->trainCountArray[l]); }
	}	
}


void LogarithmicQuality::compute (Quality *q1, Quality *q2)
{
	//std::cout << "compute q1+q2" << std::endl;
	LogarithmicQuality *rq1 = (LogarithmicQuality *) q1;
	LogarithmicQuality *rq2 = (LogarithmicQuality *) q2;

	trainCountTotal = rq1->trainCountTotal + rq2->trainCountTotal;
	testCountTotal = rq1->testCountTotal + rq2->testCountTotal;
	
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = rq1->trainCountArray[l] + rq2->trainCountArray[l]; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] = rq1->testCountArray[l] + rq2->testCountArray[l]; }

	this->compute();
}


void LogarithmicQuality::compute (QualitySet *qualitySet)
{
	//std::cout << "compute q1+..+qn" << std::endl;
	trainCountTotal = 0;
	testCountTotal = 0;
 
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = 0; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }

	for (QualitySet::iterator it = qualitySet->begin(); it != qualitySet->end(); ++it)
	{
		LogarithmicQuality *rq = (LogarithmicQuality *) (*it);

		trainCountTotal += rq->trainCountTotal;
		testCountTotal += rq->testCountTotal;
		
		for (int l = 0; l < postSize; l++) { trainCountArray[l] += rq->trainCountArray[l]; }
		for (int l = 0; l < postSize; l++) { testCountArray[l] += rq->testCountArray[l]; }
	}

	this->compute();
}


void LogarithmicQuality::normalize (Quality *q) {}


void LogarithmicQuality::print (bool v)
{
	if (v)
	{
		std::cout << std::endl
				  << "   trainCountTotal = " << std::setw(5) << std::setprecision(5) << trainCountTotal << std::endl
				  << "   trainCountArray = [" << std::setw(5) << std::setprecision(5) << trainCountArray[0] << ", ";
		for (int l = 1; l < postSize; l++) { std::cout << std::setw(5) << std::setprecision(5) << trainCountArray[l]; }

		std::cout << "]" << std::endl
				  << "   testCountTotal = " << std::setw(5) << std::setprecision(5) << testCountTotal << std::endl
				  << "   testCountArray = [" << std::setw(5) << std::setprecision(5) << testCountArray[0] << ", ";
		for (int l = 1; l < postSize; l++) { std::cout << std::setw(5) << std::setprecision(5) << testCountArray[l]; }

		std::cout << "]" << std::endl
				  << "   score = " << std::setw(5) << std::setprecision(5) << score << std::endl;
	} else {
		std::cout << " -> score = " << std::setw(5) << std::setprecision(5) << score << std::endl;
	}
}


double LogarithmicQuality::getValue (double param) { return score; }

