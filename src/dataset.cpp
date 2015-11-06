#include <iostream>

#include "dataset.hpp"

Dataset::Dataset ()
{
	size1 = 0;
	size2 = 0;
	
	indices1 = new std::map<std::string,int>();
	indices2 = new std::map<std::string,int>();
	
	labels1 = new std::map<int,std::string>();
	labels2 = new std::map<int,std::string>();
}


Dataset::~Dataset ()
{
	delete indices1;
	delete indices2;
	
	delete labels1;
	delete labels2;
	
	delete[] values;
	delete[] refValues;
}


void Dataset::buildDataset ()
{
	values = new double [size1*size2];
	refValues = new double [size1*size2];
}


void Dataset::initValues (double v)
{
	for (int i = 0; i < size1; i++)
		for (int j = 0; j < size2; j++)
			values[i+j*size1] = v;
}

void Dataset::initRefValues (double v)
{
	for (int i = 0; i < size1; i++)
		for (int j = 0; j < size2; j++)
			refValues[i+j*size1] = v;
}

void Dataset::addLabel1 (std::string str)
{
	labels1->insert(std::pair<int,std::string>(size1,str));
	indices1->insert(std::pair<std::string,int>(str,size1));
	size1++;
}

void Dataset::addLabel2 (std::string str)
{
	labels2->insert(std::pair<int,std::string>(size2,str));
	indices2->insert(std::pair<std::string,int>(str,size2));
	size2++;
}

std::string Dataset::getLabel1 (int i) { return labels1->at(i); }
std::string Dataset::getLabel2 (int j) { return labels2->at(j); }

int Dataset::getIndex1 (std::string s1) { return indices1->at(s1); }
int Dataset::getIndex2 (std::string s2) { return indices2->at(s2); }

double Dataset::getValue (int i, int j) { return values[i+j*size1]; }
double Dataset::getRefValue (int i, int j) { return refValues[i+j*size1]; }

double Dataset::getValue (std::string s1, std::string s2) { return values[indices1->at(s1)+indices2->at(s2)*size1]; }
double Dataset::getRefValue (std::string s1, std::string s2) { return refValues[indices1->at(s1)+indices2->at(s2)*size1]; }

void Dataset::setValue (int i, int j, double v) { values[i+j*size1] = v; }
void Dataset::setRefValue (int i, int j, double v) { refValues[i+j*size1] = v; }

void Dataset::setValue (std::string s1, std::string s2, double v) { values[indices1->at(s1)+indices2->at(s2)*size1] = v; }
void Dataset::setRefValue (std::string s1, std::string s2, double v) { refValues[indices1->at(s1)+indices2->at(s2)*size1] = v; }

void Dataset::incrementValue (int i, int j) { values[i+j*size1] = values[i+j*size1] + 1; }
void Dataset::incrementRefValue (int i, int j) { refValues[i+j*size1] = refValues[i+j*size1] + 1; }

void Dataset::incrementValue (std::string s1, std::string s2) { values[indices1->at(s1)+indices2->at(s2)*size1] = values[indices1->at(s1)+indices2->at(s2)*size1] + 1; }
void Dataset::incrementRefValue (std::string s1, std::string s2) { refValues[indices1->at(s1)+indices2->at(s2)*size1] = refValues[indices1->at(s1)+indices2->at(s2)*size1] + 1; }


double *Dataset::getValues1 (std::string s2)
{
	int j = getIndex2(s2);
	double *values1 = new double [size1];
	for (int i = 0; i < size1; i++) { values1[i] = getValue(i,j); }
	return values1;
}

double *Dataset::getValues2 (std::string s1)
{
	int i = getIndex1(s1);
	double *values2 = new double [size2];
	for (int j = 0; j < size2; j++) { values2[j] = getValue(i,j); }
	return values2;
}

double *Dataset::getRefValues1 (std::string s2)
{
	int j = getIndex2(s2);
	double *refValues1 = new double [size1];
	for (int i = 0; i < size1; i++) { refValues1[i] = getRefValue(i,j); }
	return refValues1;
}

double *Dataset::getRefValues2 (std::string s1)
{
	int i = getIndex1(s1);
	double *refValues2 = new double [size2];
	for (int j = 0; j < size2; j++) { refValues2[j] = getRefValue(i,j); }
	return refValues2;
}

void Dataset::print()
{
	std::cout << "SIZE 1 = " << size1 << std::endl;
	if (size1 > 0)
	{
		std::cout << "-> " << (*labels1)[0];
		for (int i = 1; i < size1; i++) { std::cout << ", " << (*labels1)[i]; }
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "SIZE 2 = " << size2 << std::endl;
	if (size2 > 0)
	{
		std::cout << "-> " << (*labels2)[0];
		for (int j = 1; j < size2; j++) { std::cout << ", " << (*labels2)[j]; }
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
	if (size1*size2 > 0)
	{
		for (int j = 0; j < std::min(100,size2); j++)
		{
			for (int i = 0; i < std::min(10,size1); i++) { std::cout << getValue(i,j) << "\t"; }
			std::cout << std::endl;
		}
		std::cout << std::endl;

		for (int j = 0; j < std::min(100,size2); j++)
		{
			for (int i = 0; i < std::min(10,size1); i++) { std::cout << getRefValue(i,j) << "\t"; }
			std::cout << std::endl;
		}
	}
}

double *Dataset::getValues (bool order)
{
	if (order) { return values; }
	double *revValues = new double [size1*size2];
	for (int i = 0; i < size1; i++)
		for (int j = 0; j < size2; j++)
			revValues[j+i*size2] = values[i+j*size1];
	return revValues;
}

double *Dataset::getRefValues (bool order)
{
	if (order) { return refValues; }
	double *revRefValues = new double [size1*size2];
	for (int i = 0; i < size1; i++)
		for (int j = 0; j < size2; j++)
			revRefValues[j+i*size2] = refValues[i+j*size1];
	return revRefValues;
}

