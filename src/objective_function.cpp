
#include "objective_function.hpp"


ObjectiveFunction::ObjectiveFunction ()
{
	//valueSet = new std::set<ObjectiveValue*>();
}


ObjectiveFunction::~ObjectiveFunction()
{
	//for (std::set<ObjectiveValue*>::iterator it = valueSet->begin(); it != valueSet->end(); it++) { delete *it; }
	//delete valueSet;
}


ObjectiveValue::~ObjectiveValue() {}
