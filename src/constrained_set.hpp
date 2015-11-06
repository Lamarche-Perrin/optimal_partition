#ifndef INCLUDE_CONSTRAINED_SET
#define INCLUDE_CONSTRAINED_SET

#include "timer.hpp"
#include "measure.hpp"
#include "partition.hpp"
#include "dataset.hpp"


class ConstrainedSet
{
	public:
		Measure *measure;
		double parameter;
		
		virtual ~ConstrainedSet ();
		 
		virtual void setRandom () = 0;
		virtual void setMeasure (Measure *m) = 0;
		virtual void print () = 0;
				
		virtual void buildDataStructure () = 0;
		virtual void computeQuality () = 0;
		virtual void normalizeQuality () = 0;
		virtual void printQuality () = 0;

		virtual void computeOptimalPartition (double parameter) = 0;
		virtual void printOptimalPartition (double parameter) = 0;
		virtual Partition *getOptimalPartition (double parameter) = 0;

		void printOptimalPartitionList (double threshold);
		PartitionList *getOptimalPartitionList (double threshold);
		void printOptimalPartitionListInCSV (double threshold, Dataset *data, int dim, std::string fileName);
		
		void testAggregationAlgorithm (bool verbose = false, int param = -1);
		void complexityAnalysis (char *fName, int start, int stop, int step, int number = 1);

	private:
		void addOptimalPartitionList (PartitionList *partitionList, Partition *minPartition, Partition *maxPartition,
			double minUnit, double maxUnit, double threshold);
};

#endif
