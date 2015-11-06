#ifndef INCLUDE_MEASURE
#define INCLUDE_MEASURE


#include <set>

class Quality;
typedef std::set<Quality*> QualitySet;

class Measure
{
public:
	bool maximize;
	//std::set<Quality*> *qualitySet;

	Measure ();
	virtual ~Measure ();

	virtual void setRandom () = 0;
	virtual Quality *newQuality (int index = -1) = 0;
	virtual void computeQuality () = 0;
	virtual void printQuality (bool verbose = true) = 0;
		
	virtual double getParameter (double unit) = 0;
	virtual double getUnitDistance (double uMin, double uMax) = 0;
	virtual double getIntermediaryUnit (double uMin, double uMax) = 0;
};


class Quality
{
public:
	Measure *measure;
		
	virtual ~Quality ();
		
	virtual void add (Quality *quality) = 0;
	virtual void compute () = 0;
	virtual void compute (Quality *quality1, Quality *quality2) = 0;
	virtual void compute (QualitySet *qualitySet) = 0;
	virtual void normalize (Quality *q) = 0;
	virtual double getValue (double param) = 0;

	virtual void print (bool value = true) = 0;
};



#endif
