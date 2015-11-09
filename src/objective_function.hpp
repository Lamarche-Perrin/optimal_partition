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
	bool maximize; /** \brief True if one deals with a maximisation problem, and false if one deals with a minimisation problem*/

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
