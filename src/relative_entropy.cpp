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
		std::cout << sumValue << ", " << (sumValue / sumRefValue);
		//std::cout << "value = " << std::setw(5) << std::setprecision(3) << sumValue
		//<< "   refvalue = " << std::setw(5) << std::setprecision(3) << sumRefValue << std::endl;
	}
}


double RelativeObjectiveValue::getValue (double param) { return param * sizeReduction - (1-param) * divergence; }

