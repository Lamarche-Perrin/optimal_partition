#ifndef INCLUDE_CSV_TOOLS
#define INCLUDE_CSV_TOOLS

#include <vector>
#include <string>
#include <time.h>

#include "dataset.hpp"
#include "graph.hpp"

extern bool VERBOSE;
extern int VERBOSE_TAB;

const bool CSV_QUOTES = false;
const char QUOTE_CHAR = '"';
const char ESCAPE_CHAR = '\\';
const char FIELD_DELIM = ',';
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
bool isInputCSVEmpty(std::ifstream &file);
bool hasCSVLine (std::ifstream &file);
void getCSVLine (std::ifstream &file, CSVLine &line, int sizeMax = 8);
void printCSVLine (std::vector<std::string> &line);
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

std::string int2string (int value);
std::string float2string (float value);
std::string double2string (double value);
time_t date2time (std::string date);

#endif
