#ifndef INCLUDE_ORDERED_SET
#define INCLUDE_ORDERED_SET

#include "constrained_set.hpp"

class OrderedSet: public ConstrainedSet
{
	public:
		int size;
	
		Quality **qualities;	
		double *optimalValues;
		int *optimalCuts;
	
		OrderedSet (int s);
		virtual ~OrderedSet ();
		
		int getIndex (int i, int j);

		void setRandom ();
		void setMeasure (Measure *m);
		void print ();

		void buildDataStructure ();
		void computeQuality ();
		void normalizeQuality ();
		void printQuality ();

		void computeOptimalPartition (double parameter);
		void printOptimalPartition (double parameter);
		Partition *getOptimalPartition (double parameter);


//		int fillPartition (part &partition, data &data, qualities &qualities, int *cuts, int iC, int iP = 0);
//		void printPartition (part &partition);
//		int getPartitionSize (int n, int *cuts);
		
//		void computeOptimalPartitions (vector<part> &partitions, data &data, qualities &qualities, float threshold);
//		void addOptimalPartitions (vector<part> &partitions, part &minPartition, part &maxPartition, data &data, qualities &qualities, float threshold);
//		void printPartitions (vector<part> partitions);
//		void deletePartition (part &partition);
//		bool arePartitionEqual (part &p1, part &p2);
		
//		void writeAllPartitionsToCSV (vector<part> &partitions, string fileName);
//		void writeEachPartitionToCSV (vector<part> &partitions, string fileName);		
};

#endif
