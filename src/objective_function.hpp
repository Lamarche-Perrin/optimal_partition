#ifndef INCLUDE_OBJECTIVE_FUNCTION
#define INCLUDE_OBJECTIVE_FUNCTION


#include <set>

class ObjectiveValue;
typedef std::set<ObjectiveValue*> ObjectiveValueSet;

class ObjectiveFunction
{
public:
	bool maximize;

	ObjectiveFunction ();
	virtual ~ObjectiveFunction ();

	virtual void setRandom () = 0;
	virtual ObjectiveValue *newObjectiveValue (int index = -1) = 0;
	virtual void computeObjectiveValues () = 0;
	virtual void printObjectiveValues (bool verbose = true) = 0;
		
	virtual double getParameter (double unit) = 0;
	virtual double getUnitDistance (double uMin, double uMax) = 0;
	virtual double getIntermediaryUnit (double uMin, double uMax) = 0;
};


class ObjectiveValue
{
public:
	ObjectiveFunction *objective;
		
	virtual ~ObjectiveValue ();
		
	virtual void add (ObjectiveValue *value) = 0;
	virtual void compute () = 0;
	virtual void compute (ObjectiveValue *value1, ObjectiveValue *value2) = 0;
	virtual void compute (ObjectiveValueSet *valueSet) = 0;
	virtual void normalize (ObjectiveValue *normalizingValue) = 0;
	virtual double getValue (double param) = 0;

	virtual void print (bool verbose = true) = 0;
};



#endif
