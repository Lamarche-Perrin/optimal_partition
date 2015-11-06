
#include "prediction_dataset.hpp"


PredictionDataset::PredictionDataset (MultiSet *preM, MultiSet *postM)
{
	preMultiSet = preM;
	postMultiSet = postM;

	trainPreValues = new std::vector<MultiSubset*> ();
	trainPostValues = new std::vector<MultiSubset*> ();
	trainCountValues = new std::vector<int> ();

	testPreValues = new std::vector<MultiSubset*> ();
	testPostValues = new std::vector<MultiSubset*> ();
	testCountValues = new std::vector<int> ();
}


PredictionDataset::~PredictionDataset ()
{
	delete trainPreValues;
	delete trainPostValues;
	delete trainCountValues;

	delete testPreValues;
	delete testPostValues;
	delete testCountValues;
}


void PredictionDataset::addTrainValue (MultiSubset *preValue, MultiSubset *postValue, int count)
{
	trainPreValues->push_back(preValue);
	trainPostValues->push_back(postValue);
	trainCountValues->push_back(count);
}


void PredictionDataset::addTestValue (MultiSubset *preValue, MultiSubset *postValue, int count)
{
	testPreValues->push_back(preValue);
	testPostValues->push_back(postValue);
	testCountValues->push_back(count);
}


void PredictionDataset::addTrainValue (int preIndex, int postIndex, int count)
{
	addTrainValue(preMultiSet->getAtomicMultiSubset(preIndex), preMultiSet->getAtomicMultiSubset(postIndex), count);
}


void PredictionDataset::addTestValue (int preIndex, int postIndex, int count)
{
	addTestValue(preMultiSet->getAtomicMultiSubset(preIndex), preMultiSet->getAtomicMultiSubset(postIndex), count);
}



