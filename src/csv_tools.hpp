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


#ifndef INCLUDE_CSV_TOOLS
#define INCLUDE_CSV_TOOLS

#include <vector>
#include <string>
#include <time.h>
#include <sstream>
#include <fstream>

#include "dataset.hpp"
#include "graph.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;

const bool CSV_QUOTES = false;
const char QUOTE_CHAR = '"';
const char ESCAPE_CHAR = '\\';
const char FIELD_DELIM = ';';
const char LINE_DELIM = '\n';

const int DATE_INDEX = 3;
const int TITLE_INDEX = 4;
const int BODY_INDEX = 5;
const int MAX_INDEX = 17;

typedef std::vector<std::string> CSVLine;

void fillDatasetFromCSV (Dataset *data, std::string dataFileName, std::string spaceFileName, std::string timeFileName);
void buildGraphFromCSV (Graph *graph, Dataset *data, std::string graphFileName);

Dataset *getDatasetFromCSV (std::string inputFileName, std::string labelsFileName1, std::string labelsFileName2,
	bool refValues = false, bool margeValues = false);

//void readDataFromCSV (data &data, std::string fileName, bool refAgg = false);
//void writeDataToCSV(data &data, std::string fileName);

void openInputCSV (std::ifstream &file, std::string fileName);
bool isInputCSVEmpty (std::ifstream &file);
bool hasCSVLine (std::ifstream &file);
void getCSVLine (std::ifstream &file, CSVLine &line, int sizeMax = 8);
void printCSVLine (CSVLine &line);
void parseCSVFile (std::string fileName);
int getCSVSize (std::string fileName);
void closeInputCSV (std::ifstream &file);

void openOutputCSV (std::ofstream &file, std::string fileName, bool erase = false);

void addCSVLine (std::ofstream &file, std::vector<std::string> &line);
void addCSVField (std::ofstream &file, int field, bool endField = true);
void addCSVField (std::ofstream &file, double field, bool endField = true, int prec = 0);
void addCSVField (std::ofstream &file, std::string field, bool endField = true);
void addCSVNAField (std::ofstream &file, bool endField = true);
void addCSVNULLField (std::ofstream &file, bool endField = true);

void endCSVField (std::ofstream &file);
void endCSVLine (std::ofstream &file);

void closeOutputCSV (std::ofstream &file);

int string2int (std::string str);
double string2double (std::string str);
std::string int2string (int value);
std::string float2string (float value, int prec = 10);
std::string double2string (double value, int prec = 10);
time_t date2time (std::string date);

#endif
