/*
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <math.h>

#include "bidimensional_relative_entropy.hpp"


BidimensionalRelativeEntropy::BidimensionalRelativeEntropy (int s1, int s2, double *val, double *refVal1, double *refVal2)
{
	size1 = s1;
	size1 = s2;
	maximize = true;
	values = new double [size1*size2];
	refValues1 = new double [size1];
	refValues2 = new double [size2];

	if (val != 0) {
		for (int i = 0; i < size1; i++)
			for (int j = 0; j < size2; j++)
				values[i+size1*j] = val[i+size1*j];
	}
	
	if (refVal1 != 0 && val != 0) { for (int i = 0; i < size1; i++) { refValues1[i] = refVal1[i]; } }
	if (refVal1 == 0 && val != 0) { for (int i = 0; i < size1; i++) { refValues1[i] = 1; } }

	if (refVal2 != 0 && val != 0) { for (int j = 0; j < size2; j++) { refValues2[j] = refVal2[j]; } }
	if (refVal2 == 0 && val != 0) { for (int j = 0; j < size2; j++) { refValues2[j] = 1; } }
}


BidimensionalRelativeEntropy::~BidimensionalRelativeEntropy ()
{
	delete[] values;
	delete[] refValues1;
	delete[] refValues2;
}


void BidimensionalRelativeEntropy::setRandom ()
{
	for (int i = 0; i < size1; i++)
		for (int j = 0; j < size2; j++)
			values[i+size1*j] = rand() % 1024;
	for (int i = 0; i < size1; i++) { refValues1[i] = 1; }
	for (int j = 0; j < size2; j++) { refValues2[j] = 1; }
}


Quality *BidimensionalRelativeEntropy::newQuality (int id)
{
	BidimensionalRelativeQuality *rq;
	
	rq = new BidimensionalRelativeQuality(this,id);
	qualitySet->insert(rq);

	return rq;
}


double BidimensionalRelativeEntropy::getParameter (double unit) { return unit; }

double BidimensionalRelativeEntropy::getUnitDistance (double uMin, double uMax) { return uMax - uMin; }

double BidimensionalRelativeEntropy::getIntermediaryUnit (double uMin, double uMax) { return uMin + (uMax - uMin) / 2; }



BidimensionalRelativeQuality::BidimensionalRelativeQuality (BidimensionalRelativeEntropy *m, int id)
{
	measure = m;
	index = id;
	
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	divergence = 0;
	sizeReduction = 0;
}


BidimensionalRelativeQuality::~BidimensionalRelativeQuality () {}


void BidimensionalRelativeQuality::add (Quality *q)
{
	BidimensionalRelativeQuality *quality = (BidimensionalRelativeQuality *) q;
	
	sumValue += quality->sumValue;
	sumRefValue += quality->sumRefValue;
	microInfo += quality->microInfo;
	divergence += quality->divergence;
	sizeReduction += quality->sizeReduction;
}


void BidimensionalRelativeQuality::compute ()
{
	sumValue = ((BidimensionalRelativeEntropy*)measure)->values[index];
	sumRefValue = ((BidimensionalRelativeEntropy*)measure)->refValues1[index] * ((BidimensionalRelativeEntropy*)measure)->refValues2[index];
	if (sumValue > 0) { microInfo = - sumValue * log2(sumValue/sumRefValue); } else { microInfo = 0; }
	sizeReduction = 0;
	divergence = 0;
}


void BidimensionalRelativeQuality::compute (Quality *q1, Quality *q2, bool dim1)
{
	BidimensionalRelativeQuality *rq1 = (BidimensionalRelativeQuality *) q1;
	BidimensionalRelativeQuality *rq2 = (BidimensionalRelativeQuality *) q2;

	sumValue = rq1->sumValue + rq2->sumValue;
	if (dim1) { sumRefValue1 = rq1->sumRefValue1 + rq2->sumRefValue1; }
	else { sumRefValue2 = rq1->sumRefValue2 + rq2->sumRefValue2; }
	microInfo = rq1->microInfo + rq2->microInfo;
	sizeReduction = rq1->sizeReduction + rq2->sizeReduction + 1;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/(sumRefValue1*sumRefValue2)); }
	else { divergence = 0; }
}


void BidimensionalRelativeQuality::compute (QualitySet *qualitySet, bool dim1)
{
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	sizeReduction = 0;
	for (QualitySet::iterator it = qualitySet->begin(); it != qualitySet->end(); ++it)
	{
		BidimensionalRelativeQuality *rq = (BidimensionalRelativeQuality *) (*it);

		sumValue += rq->sumValue;
		sumRefValue += rq->sumRefValue;
		microInfo += rq->microInfo;
		sizeReduction += rq->sizeReduction + 1;
	}
	sizeReduction--;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/sumRefValue); }
	else { divergence = 0; }
}


void BidimensionalRelativeQuality::normalize (Quality *q)
{
	BidimensionalRelativeQuality *rq = (BidimensionalRelativeQuality *) q;

	if (rq->sizeReduction > 0) { sizeReduction = sizeReduction / rq->sizeReduction; }
	if (rq->divergence > 0) { divergence /= rq->divergence; }
}


void BidimensionalRelativeQuality::print (bool v)
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


double BidimensionalRelativeQuality::getValue (double param) { return param * sizeReduction - (1-param) * divergence; }
*/
