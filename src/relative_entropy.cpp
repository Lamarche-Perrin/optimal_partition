
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <math.h>

#include "relative_entropy.hpp"


RelativeEntropy::RelativeEntropy (int s, double *val, double *refVal)
{
	size = s;
	maximize = true;
	values = new double [size];
	refValues = new double [size];

	if (val != 0) { for (int i = 0; i < size; i++) { values[i] = val[i]; } }
	if (refVal != 0 && val != 0) { for (int i = 0; i < size; i++) { refValues[i] = refVal[i]; } }
	if (refVal == 0 && val != 0) { for (int i = 0; i < size; i++) { refValues[i] = 1; } }
}


RelativeEntropy::~RelativeEntropy ()
{
	delete[] values;
	delete[] refValues;
}


void RelativeEntropy::setRandom ()
{
	for (int i = 0; i < size; i++) { values[i] = rand() % 1024; }
	for (int i = 0; i < size; i++) { refValues[i] = 1; }
}


ObjectiveValue *RelativeEntropy::newObjectiveValue (int id)
{
	RelativeObjectiveValue *rq;
	
	rq = new RelativeObjectiveValue(this,id);
	//valueSet->insert(rq);

	return rq;
}


void RelativeEntropy::computeObjectiveValues () {};

void RelativeEntropy::printObjectiveValues (bool v) {};

double RelativeEntropy::getParameter (double unit) { return unit; }

double RelativeEntropy::getUnitDistance (double uMin, double uMax) { return uMax - uMin; }

double RelativeEntropy::getIntermediaryUnit (double uMin, double uMax) { return uMin + (uMax - uMin) / 2; }



RelativeObjectiveValue::RelativeObjectiveValue (RelativeEntropy *m, int id)
{
	objective = m;
	index = id;
	
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	divergence = 0;
	sizeReduction = 0;
}


RelativeObjectiveValue::~RelativeObjectiveValue () {}


void RelativeObjectiveValue::add (ObjectiveValue *q)
{
	RelativeObjectiveValue *value = (RelativeObjectiveValue *) q;
	
	sumValue += value->sumValue;
	sumRefValue += value->sumRefValue;
	microInfo += value->microInfo;
	divergence += value->divergence;
	sizeReduction += value->sizeReduction;
}


void RelativeObjectiveValue::compute ()
{
	sumValue = ((RelativeEntropy*)objective)->values[index];
	sumRefValue = ((RelativeEntropy*)objective)->refValues[index];
	if (sumValue > 0) { microInfo = - sumValue * log2(sumValue/sumRefValue); } else { microInfo = 0; }
	sizeReduction = 0;
	divergence = 0;
}


void RelativeObjectiveValue::compute (ObjectiveValue *q1, ObjectiveValue *q2)
{
	RelativeObjectiveValue *rq1 = (RelativeObjectiveValue *) q1;
	RelativeObjectiveValue *rq2 = (RelativeObjectiveValue *) q2;

	sumValue = rq1->sumValue + rq2->sumValue;
	sumRefValue = rq1->sumRefValue + rq2->sumRefValue;
	microInfo = rq1->microInfo + rq2->microInfo;
	sizeReduction = rq1->sizeReduction + rq2->sizeReduction + 1;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/sumRefValue); }
	else { divergence = 0; }
}


void RelativeObjectiveValue::compute (ObjectiveValueSet *valueSet)
{
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	sizeReduction = 0;
	for (ObjectiveValueSet::iterator it = valueSet->begin(); it != valueSet->end(); ++it)
	{
		RelativeObjectiveValue *rq = (RelativeObjectiveValue *) (*it);

		sumValue += rq->sumValue;
		sumRefValue += rq->sumRefValue;
		microInfo += rq->microInfo;
		sizeReduction += rq->sizeReduction + 1;
	}
	sizeReduction--;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/sumRefValue); }
	else { divergence = 0; }
}


void RelativeObjectiveValue::normalize (ObjectiveValue *q)
{
	RelativeObjectiveValue *rq = (RelativeObjectiveValue *) q;

	if (rq->sizeReduction > 0) { sizeReduction = sizeReduction / rq->sizeReduction; }
	if (rq->divergence > 0) { divergence /= rq->divergence; }
}


void RelativeObjectiveValue::print (bool v)
{
	if (v)
	{
		std::cout << "value = " << std::setw(5) << std::setprecision(3) << sumValue
				  << "   refvalue = " << std::setw(5) << std::setprecision(3) << sumRefValue
				  << "   gain = " << std::setw(5) << std::setprecision(3) << sizeReduction
				  << "   loss = " << std::setw(5) << std::setprecision(3) << divergence << std::endl;
	} else {
		std::cout << "value = " << std::setw(5) << std::setprecision(3) << sumValue
				  << "   refvalue = " << std::setw(5) << std::setprecision(3) << sumRefValue << std::endl;
	}
}


double RelativeObjectiveValue::getValue (double param) { return param * sizeReduction - (1-param) * divergence; }

