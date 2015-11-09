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


#ifndef INCLUDE_DATASET
#define INCLUDE_DATASET

#include <string>
#include <map>

class Dataset {
	public:
		int size1;
		int size2;
	
		std::map<std::string,int> *indices1;
		std::map<std::string,int> *indices2;

		std::map<int,std::string> *labels1;
		std::map<int,std::string> *labels2;

		double *values;
		double *refValues;

		Dataset ();
		~Dataset ();

		void print ();
		void buildDataset ();
		
		void initValues (double v = 0);
		void initRefValues (double v = 0);

		void addLabel1 (std::string str);
		void addLabel2 (std::string str);

		std::string getLabel1 (int i);
		std::string getLabel2 (int j);

		int getIndex1 (std::string s1);
		int getIndex2 (std::string s2);
		
		double getValue (int i, int j);
		double getRefValue (int i, int j);
		
		double getValue (std::string s1, std::string s2);
		double getRefValue (std::string s1, std::string s2);
		
		void setValue (int i, int j, double v);
		void setRefValue (int i, int j, double v);
		
		void setValue (std::string s1, std::string s2, double v);
		void setRefValue (std::string s1, std::string s2, double v);
		
		void incrementValue (int i, int j);
		void incrementRefValue (int i, int j);
		
		void incrementValue (std::string s1, std::string s2);
		void incrementRefValue (std::string s1, std::string s2);

		double *getValues1 (std::string s2);
		double *getValues2 (std::string s1);
		
		double *getRefValues1 (std::string s2);
		double *getRefValues2 (std::string s1);

		double *getValues (bool order = true);
		double *getRefValues (bool order = true);
};


#endif
