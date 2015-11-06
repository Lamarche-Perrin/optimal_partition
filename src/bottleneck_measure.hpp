#ifndef INCLUDE_BOTTLENECK_MEASURE
#define INCLUDE_BOTTLENECK_MEASURE

#include "measure.hpp"
#include "markov_process.hpp"

class BottleneckMeasure: public Measure
{
	public:
		MarkovProcess *process;
		
		BottleneckMeasure (MarkovProcess *process);
		~BottleneckMeasure ();
		
		void setRandom ();
		Quality *newQuality (int index = -1);

		double getParameter (double unit);
		double getUnitDistance (double uMin, double uMax);
		double getIntermediaryUnit (double uMin, double uMax);
};


class BottleneckQuality: public Quality
{
	public:
		int index;
		double pk;
		double *pkj;
		double *pj;
		double Iki;
		double Ikj;

		BottleneckQuality (BottleneckMeasure *measure, int index = -1);
		~BottleneckQuality ();
		
		void add (Quality *quality);
		void compute ();
		void compute (Quality *quality1, Quality *quality2);
		void compute (QualitySet *qualitySet);
		void normalize (Quality *q);
		void print (bool value = true);
		double getValue (double param);
};


#endif
