
#include <iostream>
#include <iomanip>
#include <math.h>

#include "logarithmic_score.hpp"


LogarithmicScore::LogarithmicScore (PredictionDataset *ds, int p)
{
	dataset = ds;
	maximize = false;
	prior = p;

	preSize = ds->preMultiSet->atomicMultiSubsetNumber;
	postSize = ds->postMultiSet->atomicMultiSubsetNumber;

	trainCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = prior * preSize; }
	trainCountTotal = prior * preSize * postSize;
}


LogarithmicScore::~LogarithmicScore ()
{
	delete [] trainCountArray;
}


void LogarithmicScore::setRandom () {}


ObjectiveValue *LogarithmicScore::newObjectiveValue (int id)
{
	LogarithmicObjectiveValue *rq;
	rq = new LogarithmicObjectiveValue (this);
	return rq;
}


void LogarithmicScore::computeObjectiveValues ()
{
	for (unsigned int n = 0; n < dataset->trainPreValues->size(); n++)
	{
		MultiSubset *preValue = dataset->trainPreValues->at(n);
		MultiSubset *postValue = dataset->trainPostValues->at(n);
		int countValue = dataset->trainCountValues->at(n);

		trainCountArray[postValue->atomicNum] += countValue;
		trainCountTotal += countValue;

		((LogarithmicObjectiveValue*)preValue->value)->trainCountArray[postValue->atomicNum] += countValue;
		((LogarithmicObjectiveValue*)preValue->value)->trainCountTotal += countValue;
	}

	for (unsigned int n = 0; n < dataset->testPreValues->size(); n++)
	{
		MultiSubset *preValue = dataset->testPreValues->at(n);
		MultiSubset *postValue = dataset->testPostValues->at(n);
		int countValue = dataset->testCountValues->at(n);

		((LogarithmicObjectiveValue*)preValue->value)->testCountArray[postValue->atomicNum] += countValue;
		((LogarithmicObjectiveValue*)preValue->value)->testCountTotal += countValue;
	}
}


void LogarithmicScore::printObjectiveValues (bool v)
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



LogarithmicObjectiveValue::LogarithmicObjectiveValue (LogarithmicScore *m)
{
	objective = m;
	preSize = m->preSize;
	postSize = m->postSize;
	
	trainCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = m->prior; }
	trainCountTotal = m->prior * postSize;

	testCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }
	testCountTotal = 0;
}


LogarithmicObjectiveValue::~LogarithmicObjectiveValue ()
{
	delete [] trainCountArray;
	delete [] testCountArray;
}


void LogarithmicObjectiveValue::add (ObjectiveValue *q) {}


void LogarithmicObjectiveValue::compute ()
{
	if (trainCountTotal > 0)
	{
		score = testCountTotal * log10 (trainCountTotal);
		for (int l = 0; l < postSize; l++) { score -= testCountArray[l] * log10 (trainCountArray[l]); }
	}
	
	else {
		score = testCountTotal * log10 (((LogarithmicScore*)objective)->trainCountTotal);
		for (int l = 0; l < postSize; l++) { score -= testCountArray[l] * log10 (((LogarithmicScore*)objective)->trainCountArray[l]); }
	}	
}


void LogarithmicObjectiveValue::compute (ObjectiveValue *q1, ObjectiveValue *q2)
{
	//std::cout << "compute q1+q2" << std::endl;
	LogarithmicObjectiveValue *rq1 = (LogarithmicObjectiveValue *) q1;
	LogarithmicObjectiveValue *rq2 = (LogarithmicObjectiveValue *) q2;

	trainCountTotal = rq1->trainCountTotal + rq2->trainCountTotal;
	testCountTotal = rq1->testCountTotal + rq2->testCountTotal;
	
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = rq1->trainCountArray[l] + rq2->trainCountArray[l]; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] = rq1->testCountArray[l] + rq2->testCountArray[l]; }

	this->compute();
}


void LogarithmicObjectiveValue::compute (ObjectiveValueSet *valueSet)
{
	//std::cout << "compute q1+..+qn" << std::endl;
	trainCountTotal = 0;
	testCountTotal = 0;
 
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = 0; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }

	for (ObjectiveValueSet::iterator it = valueSet->begin(); it != valueSet->end(); ++it)
	{
		LogarithmicObjectiveValue *rq = (LogarithmicObjectiveValue *) (*it);

		trainCountTotal += rq->trainCountTotal;
		testCountTotal += rq->testCountTotal;
		
		for (int l = 0; l < postSize; l++) { trainCountArray[l] += rq->trainCountArray[l]; }
		for (int l = 0; l < postSize; l++) { testCountArray[l] += rq->testCountArray[l]; }
	}

	this->compute();
}


void LogarithmicObjectiveValue::normalize (ObjectiveValue *q) {}


void LogarithmicObjectiveValue::print (bool v)
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


double LogarithmicObjectiveValue::getValue (double param) { return score; }

