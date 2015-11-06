#ifndef INCLUDE_TIMER
#define INCLUDE_TIMER

#include <time.h>
#include <list>
#include <vector>
#include <string>


typedef struct DataPointStruct
{
	std::vector<int> parameters;
	float time;
	int memory;
} DataPoint;


class Timer
{
public:
	float time;
	int memory;

	Timer (char *file = 0, int dimension = 1, bool append = false);
	~Timer ();

	void start (int size, std::string text = "");
	void start (std::vector<int> parameters, std::string text = "");
	void startTime ();
	void startMemory ();
		
	void stop (std::string text = "");
	void stopTime ();
	void stopMemory ();

	void step (std::string text = "");
	void print (char *fName);

private:
	int id;
	char *fileName;
	clock_t sTime;
	clock_t eTime;
	clock_t iTime;
		
	int sMemory;
	int eMemory;
	int iMemory;
	
	std::list<DataPoint*> *data;
	DataPoint *currentDataPoint;
};

int getMemory ();

#endif
