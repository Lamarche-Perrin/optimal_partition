
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

