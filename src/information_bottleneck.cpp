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



#include <iostream>
#include <iomanip>
#include <math.h>

#include "bottleneck_objective.hpp"


InformationBottleneck::InformationBottleneck (MarkovProcess *p)
{
	process = p;
	maximize = false;
}


InformationBottleneck::~InformationBottleneck () {}


void InformationBottleneck::setRandom () {}


ObjectiveValue *InformationBottleneck::newObjectiveValue (int id)
{
	BottleneckObjectiveValue *rq;
	
	rq = new BottleneckObjectiveValue(this,id);
	valueSet->insert(rq);

	return rq;
}


double InformationBottleneck::getParameter (double unit)
{
	if (unit < 1) return unit / (1 - unit);
	else return -1;
}

double InformationBottleneck::getUnitDistance (double uMin, double uMax) { return uMax - uMin; }

double InformationBottleneck::getIntermediaryUnit (double uMin, double uMax) { return uMin + (uMax - uMin) / 2; }




BottleneckObjectiveValue::BottleneckObjectiveValue (InformationBottleneck *m, int id)
{
	objective = m;
	index = id;
	
	pk = 0;
	Iki = 0;
	Ikj = 0;

	int size = ((InformationBottleneck*)objective)->process->size;
	pkj = new double [size]; pj = new double [size];
	for (int i = 0; i < size; i++) { pkj[i] = 0; pj[i] = 0; }
}


void BottleneckObjectiveValue::add (ObjectiveValue *q)
{
	BottleneckObjectiveValue *value = (BottleneckObjectiveValue *) q;
	int size = ((InformationBottleneck*)value->objective)->process->size;

	pk += value->pk;
	Iki += value->Iki;
	Ikj += value->Ikj;
	for (int i = 0; i < size; i++)
	{
		pkj[i] += value->pkj[i];
		pj[i] += value->pj[i];
	}
}


BottleneckObjectiveValue::~BottleneckObjectiveValue ()
{
	delete[] pkj;
	delete[] pj;
}


void BottleneckObjectiveValue::compute ()
{
	MarkovProcess *process = ((InformationBottleneck*)objective)->process;
	int size = process->size;

	for (int j = 0; j < size; j++) { pj[j] = 0; }
	for (int i = 0; i < size; i++)
	{
		double pi = process->distribution[i];
		for (int j = 0; j < size; j++) { pj[j] += pi * process->transition[j*size+i]; }
	}
		
	pk = process->distribution[index];
	for (int j = 0; j < size; j++) { pkj[j] = process->distribution[index] * process->transition[j*size+index]; }
}


void BottleneckObjectiveValue::compute (ObjectiveValue *q1, ObjectiveValue *q2)
{
	BottleneckObjectiveValue *rq1 = (BottleneckObjectiveValue *) q1;
	BottleneckObjectiveValue *rq2 = (BottleneckObjectiveValue *) q2;

	MarkovProcess *process = ((InformationBottleneck*)objective)->process;
	int size = process->size;
	
	for (int j = 0; j < size; j++) { pj[j] = rq1->pj[j]; }
	pk = rq1->pk + rq2->pk;
	for (int j = 0; j < size; j++) { pkj[j] = rq1->pkj[j] + rq2->pkj[j]; }

	Iki = 0;
	if (pk > 0) { Iki = - pk * log2 (pk); }
			
	Ikj = Iki;
	for (int j = 0; j < size; j++)
	{
		if (pkj[j] > 0) { Ikj += pkj[j] * log2 (pkj[j]); }
		if (pj[j] > 0) { Ikj -= pkj[j] * log2 (pj[j]); }
	}
}


void BottleneckObjectiveValue::compute (ObjectiveValueSet *valueSet)
{
}


void BottleneckObjectiveValue::normalize (ObjectiveValue *q) {}


void BottleneckObjectiveValue::print (bool v)
{
	if (v)
	{
		std::cout << "   Iki = " << std::setw(6) << std::setprecision(3) << Iki
		<< "   Ikj = " << std::setw(6) << std::setprecision(3) << Ikj << std::endl;
	}
	else { std::cout << std::endl; }
}


double BottleneckObjectiveValue::getValue (double param)
{
	if (param >= 0) return Iki - param * Ikj;
	return -Ikj;
}

