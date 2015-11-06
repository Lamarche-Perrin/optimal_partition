
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


Quality *RelativeEntropy::newQuality (int id)
{
	RelativeQuality *rq;
	
	rq = new RelativeQuality(this,id);
	//qualitySet->insert(rq);

	return rq;
}


void RelativeEntropy::computeQuality () {};

void RelativeEntropy::printQuality (bool v) {};

double RelativeEntropy::getParameter (double unit) { return unit; }

double RelativeEntropy::getUnitDistance (double uMin, double uMax) { return uMax - uMin; }

double RelativeEntropy::getIntermediaryUnit (double uMin, double uMax) { return uMin + (uMax - uMin) / 2; }



RelativeQuality::RelativeQuality (RelativeEntropy *m, int id)
{
	measure = m;
	index = id;
	
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	divergence = 0;
	sizeReduction = 0;
}


RelativeQuality::~RelativeQuality () {}


void RelativeQuality::add (Quality *q)
{
	RelativeQuality *quality = (RelativeQuality *) q;
	
	sumValue += quality->sumValue;
	sumRefValue += quality->sumRefValue;
	microInfo += quality->microInfo;
	divergence += quality->divergence;
	sizeReduction += quality->sizeReduction;
}


void RelativeQuality::compute ()
{
	sumValue = ((RelativeEntropy*)measure)->values[index];
	sumRefValue = ((RelativeEntropy*)measure)->refValues[index];
	if (sumValue > 0) { microInfo = - sumValue * log2(sumValue/sumRefValue); } else { microInfo = 0; }
	sizeReduction = 0;
	divergence = 0;
}


void RelativeQuality::compute (Quality *q1, Quality *q2)
{
	RelativeQuality *rq1 = (RelativeQuality *) q1;
	RelativeQuality *rq2 = (RelativeQuality *) q2;

	sumValue = rq1->sumValue + rq2->sumValue;
	sumRefValue = rq1->sumRefValue + rq2->sumRefValue;
	microInfo = rq1->microInfo + rq2->microInfo;
	sizeReduction = rq1->sizeReduction + rq2->sizeReduction + 1;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/sumRefValue); }
	else { divergence = 0; }
}


void RelativeQuality::compute (QualitySet *qualitySet)
{
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	sizeReduction = 0;
	for (QualitySet::iterator it = qualitySet->begin(); it != qualitySet->end(); ++it)
	{
		RelativeQuality *rq = (RelativeQuality *) (*it);

		sumValue += rq->sumValue;
		sumRefValue += rq->sumRefValue;
		microInfo += rq->microInfo;
		sizeReduction += rq->sizeReduction + 1;
	}
	sizeReduction--;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/sumRefValue); }
	else { divergence = 0; }
}


void RelativeQuality::normalize (Quality *q)
{
	RelativeQuality *rq = (RelativeQuality *) q;

	if (rq->sizeReduction > 0) { sizeReduction = sizeReduction / rq->sizeReduction; }
	if (rq->divergence > 0) { divergence /= rq->divergence; }
}


void RelativeQuality::print (bool v)
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


double RelativeQuality::getValue (double param) { return param * sizeReduction - (1-param) * divergence; }

