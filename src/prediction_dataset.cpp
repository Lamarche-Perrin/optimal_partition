
#include "prediction_dataset.hpp"


PredictionDataset::PredictionDataset (HyperStructure *preM, HyperStructure *postM)
{
	preStructure = preM;
	postStructure = postM;

	trainPreValues = new std::vector<HyperAggregate*> ();
	trainPostValues = new std::vector<HyperAggregate*> ();
	trainCountValues = new std::vector<int> ();

	testPreValues = new std::vector<HyperAggregate*> ();
	testPostValues = new std::vector<HyperAggregate*> ();
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


void PredictionDataset::addTrainValue (HyperAggregate *preValue, HyperAggregate *postValue, int count)
{
	trainPreValues->push_back(preValue);
	trainPostValues->push_back(postValue);
	trainCountValues->push_back(count);
}


void PredictionDataset::addTestValue (HyperAggregate *preValue, HyperAggregate *postValue, int count)
{
	testPreValues->push_back(preValue);
	testPostValues->push_back(postValue);
	testCountValues->push_back(count);
}


void PredictionDataset::addTrainValue (int preIndex, int postIndex, int count)
{
	addTrainValue(preStructure->getAtomicAggregate(preIndex), preStructure->getAtomicAggregate(postIndex), count);
}


void PredictionDataset::addTestValue (int preIndex, int postIndex, int count)
{
	addTestValue(preStructure->getAtomicAggregate(preIndex), preStructure->getAtomicAggregate(postIndex), count);
}



