#ifndef INCLUDE_PARTITION
#define INCLUDE_PARTITION

#include <list>

#include "measure.hpp"

class Datatree;
typedef std::set<int> Vertices;

class Part;
class Partition;

typedef std::set<Part*> PartSet;
typedef std::list<Partition*> PartitionList;


class Part
{
public:
	std::list<int> *individuals;
	Quality *quality;
	
	Part (Quality *quality = 0);
	Part (Part *part);
	Part (Datatree *node, Quality *quality = 0);
	virtual ~Part ();
	
	void addIndividual (int i, bool front = false);
	Vertices *getVertices ();
	virtual bool equal (Part *p);
		
	virtual void print (bool endl = false);
	virtual int printSize ();
};


class BiPart: public Part
{
public:
	Part *firstPart;
	Part *secondPart;
	
	BiPart (Part *part1, Part *part2, Quality *quality = 0);
	BiPart (BiPart *biPart);
	~BiPart ();

	bool equal (Part *p);
	void print (bool endl = false);
	int printSize ();
};


class HyperPart: public Part
{
public:
	int dimension;
	Part **partArray;
	
	HyperPart (Part **partArray, int dimension, Quality *quality = 0);
	HyperPart (HyperPart *hyperPart);
	~HyperPart ();

	bool equal (Part *p);
	void print (bool endl = false);
	int printSize ();
};


class Partition
{
public:
	double parameter;
	std::list<Part*> *parts;
	Quality *quality;
		
	Partition (Measure *measure = 0, double parameter = 0);
	Partition (Partition *partition);
	~Partition ();
		
	void addPart (Part *p, bool front = false);
	bool equal (Partition *p);
	void print (bool endl = false);
};


#endif
