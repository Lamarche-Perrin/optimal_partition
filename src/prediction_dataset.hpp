#ifndef INCLUDE_PREDICTION_DATASET
#define INCLUDE_PREDICTION_DATASET

#include "hyper_structure.hpp"

class PredictionDataset {
public:
	HyperStructure *preStructure;
	HyperStructure *postStructure;

	std::vector<HyperAggregate*> *trainPreValues;
	std::vector<HyperAggregate*> *trainPostValues;
	std::vector<int> *trainCountValues;

	std::vector<HyperAggregate*> *testPreValues;
	std::vector<HyperAggregate*> *testPostValues;
	std::vector<int> *testCountValues;
	
	PredictionDataset (HyperStructure *preM, HyperStructure *post);
	~PredictionDataset ();

	void addTrainValue (HyperAggregate *preValue, HyperAggregate *postValue, int count = 1);
	void addTestValue (HyperAggregate *preValue, HyperAggregate *postValue, int count = 1);

	void addTrainValue (int preNum, int postNum, int count = 1);
	void addTestValue (int preNum, int postNum, int count = 1);
};

#endif
