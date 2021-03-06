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


#ifndef INCLUDE_BIDIMENSIONAL_RELATIVE_ENTROPY
#define INCLUDE_BIDIMENSIONAL_RELATIVE_ENTROPY

/*
#include "objective_function.hpp"

class BidimensionalRelativeEntropy: public ObjectiveFunction
{
	public:
		int size1;
		int size2;
		double *values;
		double *refValues1;
		double *refValues2;
		
		BidimensionalRelativeEntropy (int size1, int size2, double *values = 0, double *refValues1 = 0, double *refValues2 = 0);
		~BidimensionalRelativeEntropy ();
		
		void setRandom ();
		ObjectiveValue *newObjectiveValue (int index = -1);

		double getParameter (double unit);
		double getUnitDistance (double uMin, double uMax);
		double getIntermediaryUnit (double uMin, double uMax);
};


class BidimensionalRelativeObjectiveValue: public ObjectiveValue
{
	public:
		int index;
		
		double sumValue;
		double sumRefValue;
		double microInfo;
		double divergence;
		double sizeReduction;
		
		BidimensionalRelativeObjectiveValue (BidimensionalRelativeEntropy *objective, int index = -1);
		~BidimensionalRelativeObjectiveValue ();
		
		void add (ObjectiveValue *value);
		void compute ();
		void compute (ObjectiveValue *value1, ObjectiveValue *value2);
		void compute (ObjectiveValueSet *valueset);
		void normalize (ObjectiveValue *q);
		void print (bool verbose = true);
		double getValue (double param);
};

*/
#endif
