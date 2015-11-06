#ifndef INCLUDE_OBJECTIVE_FUNCTION
#define INCLUDE_OBJECTIVE_FUNCTION

/*!
 * \file objective_function.hpp
 * \brief Abstract class defining an objective function to be associated to a constrained set in order to define the optimisation problem that one wants to solve
 * \author Robin Lamarche-Perrin
 * \date 06/11/2015
 */

#include <set>

class ObjectiveValue;
typedef std::set<ObjectiveValue*> ObjectiveValueSet;

/*!
 * \class ObjectiveFunction
 * \brief Abstract class defining an objective function to be associated to a constrained set in order to define the optimisation problem that one wants to solve
 */
class ObjectiveFunction
{
	friend class AbstractSet;

public:
	bool maximize; /*!< True if one deals with a maximisation problem, and false if one deals with a minimisation problem*/

	/*!
     * \brief Constructor
     */
	ObjectiveFunction ();

	/*!
	 * \brief Destructor
     */
	virtual ~ObjectiveFunction ();

	/*!
	 * \brief Randomly set the initial data from which the objective function is computed
     */
	virtual void setRandom () = 0;
	
	/*!
	 * \brief This method is called by child classes of AbstractSet (do not use directly)
     */
	virtual void computeObjectiveValues () = 0;
	
	/*!
	 * \brief This method is called by child classes of AbstractSet (do not use directly)
     */
	virtual void printObjectiveValues (bool verbose = true) = 0;

	/*!
	 * \brief This method is called by child classes of AbstractSet (do not use directly)
     */
	virtual ObjectiveValue *newObjectiveValue (int index = -1) = 0;

private:
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
