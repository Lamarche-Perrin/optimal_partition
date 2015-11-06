#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#include "csv_tools.hpp"
#include "dataset.hpp"

Dataset *getDatasetFromCSV (std::string inputFileName, std::string labelsFileName1, std::string labelsFileName2,
	bool refValues, bool margeValues) {

	Dataset *data = new Dataset();
	CSVLine line;

	std::ifstream labelsFile1;
	openInputCSV(labelsFile1,labelsFileName1);

	while (hasCSVLine(labelsFile1))
	{
		getCSVLine(labelsFile1,line);
		data->addLabel1(line[0]);
	}

	closeInputCSV(labelsFile1);


	std::ifstream labelsFile2;
	openInputCSV(labelsFile2,labelsFileName2);

	while (hasCSVLine(labelsFile2))
	{
		getCSVLine(labelsFile2,line);
		data->addLabel2(line[0]);
	}

	closeInputCSV(labelsFile2);

	data->buildDataset();
	data->initValues(0);

	if (refValues) { data->initRefValues(0); } else { data->initRefValues(1); }


	if (refValues && margeValues)
	{
		openInputCSV(labelsFile1,labelsFileName1);
		CSVLine line1;
	
		while (hasCSVLine(labelsFile1))
		{
			getCSVLine(labelsFile1,line1);
			std::string str1 = line1[0];
			double value1 = atof(line1[1].c_str());

			CSVLine line2;	
			openInputCSV(labelsFile2,labelsFileName2);
			
			while (hasCSVLine(labelsFile2))
			{
				getCSVLine(labelsFile2,line2);
				std::string str2 = line2[0];
				double value2 = atof(line2[1].c_str());
				
				data->setRefValue(str1,str2,value1*value2);
			}
			
			closeInputCSV(labelsFile2);
		}
	
		closeInputCSV(labelsFile1);
	}

	std::ifstream inputFile;
	openInputCSV(inputFile,inputFileName);

	while (hasCSVLine(inputFile))
	{
		getCSVLine(inputFile,line);
		data->setValue(line[0],line[1],atof(line[2].c_str()));
		if (refValues && !margeValues) { data->setRefValue(line[0],line[1],atof(line[3].c_str())); }
	}

	closeInputCSV(inputFile);

	
	return data;	
}


/*
void fillDatasetFromCSV (Dataset *data, std::string dataFileName, std::string spaceFileName, std::string timeFileName)
{
	CSVLine line;

	int sIndex = 0;
	std::ifstream spaceFile;
	openInputCSV(spaceFile,spaceFileName);
	while (hasCSVLine(spaceFile))
	{
		getCSVLine(spaceFile,line,1);
		(*data->spaceIndexes)[line[0]] = sIndex;
		(*data->spaceLabels)[sIndex] = line[0];
		sIndex++;
	}
	closeInputCSV(spaceFile);

	int tIndex = 0;
	std::ifstream timeFile;
	openInputCSV(timeFile,timeFileName);
	while (hasCSVLine(timeFile))
	{
		getCSVLine(timeFile,line,1);
		(*data->timeIndexes)[line[0]] = tIndex;
		(*data->timeLabels)[tIndex] = line[0];
		tIndex++;
	}
	closeInputCSV(timeFile);


	std::ifstream dataFile;
	openInputCSV(dataFile,dataFileName);

	while (hasCSVLine(dataFile))
	{
		getCSVLine(dataFile,line,3);		
		data->incrementValue((*data->spaceIndexes)[line[1]],(*data->timeIndexes)[line[2]]);
	}
	closeInputCSV(dataFile);
}

void buildGraphFromCSV (Graph *graph, Dataset *data, std::string graphFileName)
{
	CSVLine line;

	std::ifstream graphFile;
	openInputCSV(graphFile,graphFileName);
	while (hasCSVLine(graphFile))
	{
		getCSVLine(graphFile,line,2);
		graph->addEdge((*data->spaceIndexes)[line[0]],(*data->spaceIndexes)[line[1]]);
	}
	closeInputCSV(graphFile);
}
*/


/*
void writeDataToCSV (data &data, std::string fileName) {
	ofstream file;
	openOutputCSV(file,fileName+"-DATA.csv");
	
	for (int i = 0; i < data.size; i++) {
		vector<std::string> line;
		
		line.push_back(data.labels[i]);
		line.push_back(float2string(data.values[i]));
		line.push_back(float2string(data.refValues[i]));

		addCSVLine(file,line);
	}
	closeOutputCSV(file);
}
*/


void openInputCSV (std::ifstream &file, std::string fileName)
{
	if (VERBOSE) { std::cout << "opening " << fileName << " for reading" << std::endl; }
	file.open(fileName.c_str(),std::ifstream::in);
}

bool isInputCSVEmpty(std::ifstream &file) { return file.peek() == std::ifstream::traits_type::eof(); }

void closeInputCSV (std::ifstream &file)
{
	if (VERBOSE) { std::cout << "closing file" << std::endl; }
	file.close();
}

bool hasCSVLine (std::ifstream &file) { return file.good() && file.peek() != EOF; }

void getCSVLine (std::ifstream &file, CSVLine &line, int sizeMax)
{
	std::string field;
	line.clear();
	line.resize(sizeMax);

	int i = 0;
	bool inQuote(false);
	bool escaping(false);
	
	while (file.good() && file.peek() != EOF)
	{
		char c;
		file.get(c);

		switch (c)
		{
			case QUOTE_CHAR:
				if (CSV_QUOTES)
				{
					if (escaping) { field += c; }
					else { inQuote = !inQuote; }
					escaping = false;
				}
				break;
				
			case ESCAPE_CHAR:
				escaping = true;
				field += c;
				break;

			case FIELD_DELIM:
				if (inQuote == true) { field += c; }
				else
				{
					line[i++] = field;
					field.clear();
				}
				escaping = false;
			break;

			case LINE_DELIM:
				if (inQuote == true) { field += c; }
				else {
					line[i++] = field;
					field.clear();
					return;
				}
				escaping = false;
				break;

			default:
				field += c;
				escaping = false;
				break;
		}
	}
}

void printCSVLine (CSVLine &line)
{
	for (CSVLine::iterator it = line.begin(); it != line.end(); ++it) { std::cout << "[" << *it << "] "; }
	std::cout << std::endl;
}

void parseCSVFile (std::string fileName)
{
	std::ifstream file;
	openInputCSV(file,fileName+".csv");
	CSVLine line;

	while (hasCSVLine(file))
	{
		getCSVLine(file,line);
		printCSVLine(line);
	}
	closeInputCSV(file);
}


int getCSVSize (std::string fileName)
{
	int size = 0;
	std::ifstream file;
	CSVLine line;
	openInputCSV(file,fileName);
	while (hasCSVLine(file))
	{
		getCSVLine(file,line);
		size++;
	}
	closeInputCSV(file);
	return size;
}


void openOutputCSV (std::ofstream &file, std::string fileName, bool erase)
{
	if (VERBOSE) { std::cout << "opening " << fileName << " for writing" << std::endl; }
	if (erase) { file.open(fileName.c_str(),std::ofstream::out); }
	else { file.open(fileName.c_str(),std::ofstream::out | std::ios_base::out | std::ios_base::app); }
}

void closeOutputCSV (std::ofstream &file)
{
	if (VERBOSE) { std::cout << "closing file" << std::endl; }
	file.close();
}

void addCSVLine (std::ofstream &file, CSVLine &line)
{
	bool firstLine (true);
	for (CSVLine::iterator it = line.begin(); it != line.end(); ++it)
	{
		if (!firstLine) { file << FIELD_DELIM; }
		file << QUOTE_CHAR << *it << QUOTE_CHAR;
		firstLine = false;
	}
	file << LINE_DELIM;
}

void addCSVField (std::ofstream &file, int field, bool endField)
{
	file << field;
	if (endField) { endCSVField(file); }
}

void addCSVField (std::ofstream &file, double field, bool endField, int prec)
{
	int p = -1;
	if (prec != 0) { p = file.precision(); file.precision(prec); }
	file << std::fixed << field;
	if (endField) { endCSVField(file); }
	if (p != -1) { file.precision(p); }
}

void addCSVField (std::ofstream &file, std::string field, bool endField)
{
	file << QUOTE_CHAR << field << QUOTE_CHAR;
	if (endField) { endCSVField(file); }
}

void addCSVNAField (std::ofstream &file, bool endField)
{
	file << "NA";
	if (endField) { endCSVField(file); }
}

void addCSVNULLField (std::ofstream &file, bool endField)
{
	file << "NULL";
	if (endField) { endCSVField(file); }
}

void endCSVField (std::ofstream &file) { file << FIELD_DELIM; }
void endCSVLine (std::ofstream &file) { file << LINE_DELIM; }


std::string int2string (int value)
{
	std::ostringstream strValue;
	strValue << value;
	return strValue.str();
}

std::string float2string (float value)
{
	std::ostringstream strValue;
	strValue.precision(10);
	strValue << std::fixed <<  value;
	return strValue.str();
}

std::string double2string (double value)
{
	std::ostringstream strValue;
	strValue.precision(10);
	strValue << std::fixed << value;
	return strValue.str();
}

time_t date2time (std::string date)
{
	struct tm time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	time.tm_year = atoi(date.substr(0,4).c_str())-1900;
	time.tm_mon = atoi(date.substr(5,2).c_str())-1;
	time.tm_mday = atoi(date.substr(8,2).c_str());
	return mktime(&time);
}
