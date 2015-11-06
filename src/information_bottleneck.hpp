#ifndef INCLUDE_INFORMATION_BOTTLENECK
#define INCLUDE_INFORMATION_BOTTLENECK

#include "objective_function.hpp"
#include "markov_process.hpp"

class InformationBottleneck: public ObjectiveFunction
{
	public:
		MarkovProcess *process;
		
		InformationBottleneck (MarkovProcess *process);
		~InformationBottleneck ();
		
		void setRandom ();
		ObjectiveValue *newObjectiveValue (int index = -1);

		double getParameter (double unit);
		double getUnitDistance (double uMin, double uMax);
		double getIntermediaryUnit (double uMin, double uMax);
};


class BottleneckObjectiveValue: public ObjectiveValue
{
	public:
		int index;
		double pk;
		double *pkj;
		double *pj;
		double Iki;
		double Ikj;

		BottleneckObjectiveValue (InformationBottleneck *objective, int index = -1);
		~BottleneckObjectiveValue ();
		
		void add (ObjectiveValue *value);
		void compute ();
		void compute (ObjectiveValue *value1, ObjectiveValue *value2);
		void compute (ObjectiveValueSet *valueSet);
		void normalize (ObjectiveValue *q);
		void print (bool verbose = true);
		double getValue (double param);
};


#endif
