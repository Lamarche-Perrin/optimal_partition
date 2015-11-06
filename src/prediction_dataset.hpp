#ifndef INCLUDE_PREDICTION_DATASET
#define INCLUDE_PREDICTION_DATASET

#include "multi_set.hpp"

class PredictionDataset {
public:
	MultiSet *preMultiSet;
	MultiSet *postMultiSet;

	std::vector<MultiSubset*> *trainPreValues;
	std::vector<MultiSubset*> *trainPostValues;
	std::vector<int> *trainCountValues;

	std::vector<MultiSubset*> *testPreValues;
	std::vector<MultiSubset*> *testPostValues;
	std::vector<int> *testCountValues;
	
	PredictionDataset (MultiSet *preM, MultiSet *post);
	~PredictionDataset ();

	void addTrainValue (MultiSubset *preValue, MultiSubset *postValue, int count = 1);
	void addTestValue (MultiSubset *preValue, MultiSubset *postValue, int count = 1);

	void addTrainValue (int preNum, int postNum, int count = 1);
	void addTestValue (int preNum, int postNum, int count = 1);
};

#endif
