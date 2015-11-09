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


ObjectiveValue *BidimensionalRelativeEntropy::newObjectiveValue (int id)
{
	BidimensionalRelativeObjectiveValue *rq;
	
	rq = new BidimensionalRelativeObjectiveValue(this,id);
	valueSet->insert(rq);

	return rq;
}


double BidimensionalRelativeEntropy::getParameter (double unit) { return unit; }

double BidimensionalRelativeEntropy::getUnitDistance (double uMin, double uMax) { return uMax - uMin; }

double BidimensionalRelativeEntropy::getIntermediaryUnit (double uMin, double uMax) { return uMin + (uMax - uMin) / 2; }



BidimensionalRelativeObjectiveValue::BidimensionalRelativeObjectiveValue (BidimensionalRelativeEntropy *m, int id)
{
	objective = m;
	index = id;
	
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	divergence = 0;
	sizeReduction = 0;
}


BidimensionalRelativeObjectiveValue::~BidimensionalRelativeObjectiveValue () {}


void BidimensionalRelativeObjectiveValue::add (ObjectiveValue *q)
{
	BidimensionalRelativeObjectiveValue *value = (BidimensionalRelativeObjectiveValue *) q;
	
	sumValue += value->sumValue;
	sumRefValue += value->sumRefValue;
	microInfo += value->microInfo;
	divergence += value->divergence;
	sizeReduction += value->sizeReduction;
}


void BidimensionalRelativeObjectiveValue::compute ()
{
	sumValue = ((BidimensionalRelativeEntropy*)objective)->values[index];
	sumRefValue = ((BidimensionalRelativeEntropy*)objective)->refValues1[index] * ((BidimensionalRelativeEntropy*)objective)->refValues2[index];
	if (sumValue > 0) { microInfo = - sumValue * log2(sumValue/sumRefValue); } else { microInfo = 0; }
	sizeReduction = 0;
	divergence = 0;
}


void BidimensionalRelativeObjectiveValue::compute (ObjectiveValue *q1, ObjectiveValue *q2, bool dim1)
{
	BidimensionalRelativeObjectiveValue *rq1 = (BidimensionalRelativeObjectiveValue *) q1;
	BidimensionalRelativeObjectiveValue *rq2 = (BidimensionalRelativeObjectiveValue *) q2;

	sumValue = rq1->sumValue + rq2->sumValue;
	if (dim1) { sumRefValue1 = rq1->sumRefValue1 + rq2->sumRefValue1; }
	else { sumRefValue2 = rq1->sumRefValue2 + rq2->sumRefValue2; }
	microInfo = rq1->microInfo + rq2->microInfo;
	sizeReduction = rq1->sizeReduction + rq2->sizeReduction + 1;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/(sumRefValue1*sumRefValue2)); }
	else { divergence = 0; }
}


void BidimensionalRelativeObjectiveValue::compute (ObjectiveValueSet *valueSet, bool dim1)
{
	sumValue = 0;
	sumRefValue = 0;
	microInfo = 0;
	sizeReduction = 0;
	for (ObjectiveValueSet::iterator it = valueSet->begin(); it != valueSet->end(); ++it)
	{
		BidimensionalRelativeObjectiveValue *rq = (BidimensionalRelativeObjectiveValue *) (*it);

		sumValue += rq->sumValue;
		sumRefValue += rq->sumRefValue;
		microInfo += rq->microInfo;
		sizeReduction += rq->sizeReduction + 1;
	}
	sizeReduction--;
				
	if (sumValue > 0) { divergence = - microInfo - sumValue * log2(sumValue/sumRefValue); }
	else { divergence = 0; }
}


void BidimensionalRelativeObjectiveValue::normalize (ObjectiveValue *q)
{
	BidimensionalRelativeObjectiveValue *rq = (BidimensionalRelativeObjectiveValue *) q;

	if (rq->sizeReduction > 0) { sizeReduction = sizeReduction / rq->sizeReduction; }
	if (rq->divergence > 0) { divergence /= rq->divergence; }
}


void BidimensionalRelativeObjectiveValue::print (bool v)
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


double BidimensionalRelativeObjectiveValue::getValue (double param) { return param * sizeReduction - (1-param) * divergence; }
*/
