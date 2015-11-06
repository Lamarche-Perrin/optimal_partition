
#include <iostream>
#include <iomanip>
#include <math.h>

#include "bottleneck_measure.hpp"


BottleneckMeasure::BottleneckMeasure (MarkovProcess *p)
{
	process = p;
	maximize = false;
}


BottleneckMeasure::~BottleneckMeasure () {}


void BottleneckMeasure::setRandom () {}


Quality *BottleneckMeasure::newQuality (int id)
{
	BottleneckQuality *rq;
	
	rq = new BottleneckQuality(this,id);
	qualitySet->insert(rq);

	return rq;
}


double BottleneckMeasure::getParameter (double unit)
{
	if (unit < 1) return unit / (1 - unit);
	else return -1;
}

double BottleneckMeasure::getUnitDistance (double uMin, double uMax) { return uMax - uMin; }

double BottleneckMeasure::getIntermediaryUnit (double uMin, double uMax) { return uMin + (uMax - uMin) / 2; }




BottleneckQuality::BottleneckQuality (BottleneckMeasure *m, int id)
{
	measure = m;
	index = id;
	
	pk = 0;
	Iki = 0;
	Ikj = 0;

	int size = ((BottleneckMeasure*)measure)->process->size;
	pkj = new double [size]; pj = new double [size];
	for (int i = 0; i < size; i++) { pkj[i] = 0; pj[i] = 0; }
}


void BottleneckQuality::add (Quality *q)
{
	BottleneckQuality *quality = (BottleneckQuality *) q;
	int size = ((BottleneckMeasure*)quality->measure)->process->size;

	pk += quality->pk;
	Iki += quality->Iki;
	Ikj += quality->Ikj;
	for (int i = 0; i < size; i++)
	{
		pkj[i] += quality->pkj[i];
		pj[i] += quality->pj[i];
	}
}


BottleneckQuality::~BottleneckQuality ()
{
	delete[] pkj;
	delete[] pj;
}


void BottleneckQuality::compute ()
{
	MarkovProcess *process = ((BottleneckMeasure*)measure)->process;
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


void BottleneckQuality::compute (Quality *q1, Quality *q2)
{
	BottleneckQuality *rq1 = (BottleneckQuality *) q1;
	BottleneckQuality *rq2 = (BottleneckQuality *) q2;

	MarkovProcess *process = ((BottleneckMeasure*)measure)->process;
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


void BottleneckQuality::compute (QualitySet *qualitySet)
{
}


void BottleneckQuality::normalize (Quality *q) {}


void BottleneckQuality::print (bool v)
{
	if (v)
	{
		std::cout << "   Iki = " << std::setw(6) << std::setprecision(3) << Iki
		<< "   Ikj = " << std::setw(6) << std::setprecision(3) << Ikj << std::endl;
	}
	else { std::cout << std::endl; }
}


double BottleneckQuality::getValue (double param)
{
	if (param >= 0) return Iki - param * Ikj;
	return -Ikj;
}

