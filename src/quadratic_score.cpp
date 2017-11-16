/*
 * This file is part of Optimal Partition.
 *
 * Optimal Partition is a toolbox to solve special versions of the Set
 * Partitioning Problem, that is the combinatorial optimisation of a
 * decomposable objective over a set of feasible partitions (defined
 * according to specific algebraic structures: e.g., hierachies, sets of
 * intervals, graphs). The objectives are mainly based on information theory,
 * in the perspective of multilevel analysis of large-scale datasets, and the
 * algorithms are based on dynamic programming. For details regarding the
 * formal grounds of this work, please refer to:
 * 
 * Robin Lamarche-Perrin, Yves Demazeau and Jean-Marc Vincent. A Generic Set
 * Partitioning Algorithm with Applications to Hierarchical and Ordered Sets.
 * Technical Report 105/2014, Max-Planck-Institute for Mathematics in the
 * Sciences, Leipzig, Germany, May 2014.
 * 
 * <http://www.mis.mpg.de/publications/preprints/2014/prepr2014-105.html>
 * 
 * Copyright © 2015 Robin Lamarche-Perrin
 * (<Robin.Lamarche-Perrin@lip6.fr>)
 * 
 * Optimal Partition is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * Optimal Partition is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */



#include <iostream>
#include <iomanip>
#include <cmath>

#include "quadratic_score.hpp"


QuadraticScore::QuadraticScore (PredictionDataset *ds)
{
	dataset = ds;
	maximize = true;

	preSize = ds->preMultiSet->atomicMultiSubsetNumber;
	postSize = ds->postMultiSet->atomicMultiSubsetNumber;

	trainCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = 0; }
	trainCountTotal = 0;

	testCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }
	testCountTotal = 0;
}


QuadraticScore::~QuadraticScore ()
{
	delete [] trainCountArray;
	delete [] testCountArray;
}


void QuadraticScore::setRandom ()
{
	PredictionDataset *newDataset = new PredictionDataset (dataset->preMultiSet, dataset->postMultiSet);
	delete dataset;
	dataset = newDataset;

	int nb = dataset->preMultiSet->atomicMultiSubsetNumber * dataset->postMultiSet->atomicMultiSubsetNumber;
	for (int i = 0; i < nb; i++)
		dataset->addTrainValue (dataset->preMultiSet->getRandomAtomicMultiSubset(), dataset->postMultiSet->getRandomAtomicMultiSubset());

	for (int i = 0; i < nb/10; i++)
		dataset->addTestValue (dataset->preMultiSet->getRandomAtomicMultiSubset(), dataset->postMultiSet->getRandomAtomicMultiSubset());
}


ObjectiveValue *QuadraticScore::newObjectiveValue (int id)
{
	QuadraticScoreValue *rq;
	rq = new QuadraticScoreValue (this);
	return rq;
}


void QuadraticScore::computeObjectiveValues ()
{
	for (unsigned int n = 0; n < dataset->trainPreValues->size(); n++)
	{
		MultiSubset *preValue = dataset->trainPreValues->at(n);
		MultiSubset *postValue = dataset->trainPostValues->at(n);
		int countValue = dataset->trainCountValues->at(n);

		trainCountArray[postValue->atomicNum] += countValue;
		trainCountTotal += countValue;

		((QuadraticScoreValue*)preValue->value)->trainCountArray[postValue->atomicNum] += countValue;
		((QuadraticScoreValue*)preValue->value)->trainCountTotal += countValue;
	}

	for (unsigned int n = 0; n < dataset->testPreValues->size(); n++)
	{
		MultiSubset *preValue = dataset->testPreValues->at(n);
		MultiSubset *postValue = dataset->testPostValues->at(n);
		int countValue = dataset->testCountValues->at(n);

		testCountArray[postValue->atomicNum] += countValue;
		testCountTotal += countValue;

		((QuadraticScoreValue*)preValue->value)->testCountArray[postValue->atomicNum] += countValue;
		((QuadraticScoreValue*)preValue->value)->testCountTotal += countValue;
	}
}


void QuadraticScore::printObjectiveValues (bool v)
{
	if (v)
	{
		std::cout << std::endl
				  << "   trainCountTotal = " << std::setw(5) << std::setprecision(5) << trainCountTotal << std::endl
				  << "   trainCountArray = [" << std::setw(5) << std::setprecision(5) << trainCountArray[0] << ", ";
		for (int l = 1; l < postSize; l++) { std::cout << std::setw(5) << std::setprecision(5) << trainCountArray[l]; }
		std::cout << "]" << std::endl;
	}
}


double QuadraticScore::getParameter (double unit) { return 0; }

double QuadraticScore::getUnitDistance (double uMin, double uMax) { return 0; }

double QuadraticScore::getIntermediaryUnit (double uMin, double uMax) { return 0; }



QuadraticScoreValue::QuadraticScoreValue (QuadraticScore *m)
{
	objective = m;
	preSize = m->preSize;
	postSize = m->postSize;
	
	trainCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = 0; }
	trainCountTotal = 0;

	testCountArray = new int [postSize];
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }
	testCountTotal = 0;

	score = 0;
}


QuadraticScoreValue::~QuadraticScoreValue ()
{
	delete [] trainCountArray;
	delete [] testCountArray;
}


bool QuadraticScoreValue::equal (ObjectiveValue *v, int prec)
{
	double factor = pow(10,prec);
	QuadraticScoreValue *value = (QuadraticScoreValue*) v;
	return (round(score*factor) == round(value->score*factor));
}


void QuadraticScoreValue::add (ObjectiveValue *v)
{
	QuadraticScoreValue *value = (QuadraticScoreValue*) v;
	for (int l = 0; l < postSize; l++) { trainCountArray[l] += value->trainCountArray[l]; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] += value->testCountArray[l]; }
	trainCountTotal += value->trainCountTotal;
	testCountTotal += value->testCountTotal;
	score += value->score;
}


void QuadraticScoreValue::compute ()
{
	QuadraticScore *obj = (QuadraticScore*) objective;
	if (trainCountTotal > 0)
	{
		for (int l = 0; l < postSize; l++)
		{
			double addScore = 2 * trainCountArray[l] / trainCountTotal;
			for (int l2 = 0; l2 < postSize; l2++) { addScore -= std::pow(trainCountArray[l2] / trainCountTotal, 2); }
			score += addScore * testCountArray[l] / obj->testCountTotal;
		}
	}
	
	else {
		for (int l = 0; l < postSize; l++)
		{
			double addScore = 2 * obj->trainCountArray[l] / obj->trainCountTotal;
			for (int l2 = 0; l2 < postSize; l2++) { addScore -= std::pow(obj->trainCountArray[l2] / obj->trainCountTotal, 2); }
			score += addScore * testCountArray[l] / obj->testCountTotal;
		}
	}
}


void QuadraticScoreValue::compute (ObjectiveValue *q1, ObjectiveValue *q2)
{
	//std::cout << "compute q1+q2" << std::endl;
	QuadraticScoreValue *rq1 = (QuadraticScoreValue *) q1;
	QuadraticScoreValue *rq2 = (QuadraticScoreValue *) q2;

	trainCountTotal = rq1->trainCountTotal + rq2->trainCountTotal;
	testCountTotal = rq1->testCountTotal + rq2->testCountTotal;
	
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = rq1->trainCountArray[l] + rq2->trainCountArray[l]; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] = rq1->testCountArray[l] + rq2->testCountArray[l]; }

	this->compute();
}


void QuadraticScoreValue::compute (ObjectiveValueSet *valueSet)
{
	//std::cout << "compute q1+..+qn" << std::endl;
	trainCountTotal = 0;
	testCountTotal = 0;
 
	for (int l = 0; l < postSize; l++) { trainCountArray[l] = 0; }
	for (int l = 0; l < postSize; l++) { testCountArray[l] = 0; }

	for (ObjectiveValueSet::iterator it = valueSet->begin(); it != valueSet->end(); ++it)
	{
		QuadraticScoreValue *rq = (QuadraticScoreValue *) (*it);

		trainCountTotal += rq->trainCountTotal;
		testCountTotal += rq->testCountTotal;
		
		for (int l = 0; l < postSize; l++) { trainCountArray[l] += rq->trainCountArray[l]; }
		for (int l = 0; l < postSize; l++) { testCountArray[l] += rq->testCountArray[l]; }
	}

	this->compute();
}


void QuadraticScoreValue::normalize (ObjectiveValue *q) {}


void QuadraticScoreValue::print (bool v)
{
	if (v)
	{
		std::cout << std::endl
				  << "   trainCountTotal = " << std::setw(5) << std::setprecision(5) << trainCountTotal << std::endl
				  << "    testCountTotal = " << std::setw(5) << std::setprecision(5) << testCountTotal << std::endl
				  << "                     ";
			
		for (int l = 0; l < postSize; l++) { std::cout << "  "; ((QuadraticScore*)objective)->dataset->postMultiSet->atomicMultiSubsetArray[l]->printIndexSet(); }
		std::cout << std::endl;

		std::cout << "   trainCountArray = [" << std::setw(5) << std::setprecision(5) << trainCountArray[0];
		for (int l = 1; l < postSize; l++) { std::cout << ", " << std::setw(5) << std::setprecision(5) << trainCountArray[l]; }

		std::cout << "]" << std::endl
				  << "    testCountArray = [" << std::setw(5) << std::setprecision(5) << testCountArray[0];
		for (int l = 1; l < postSize; l++) { std::cout << ", " << std::setw(5) << std::setprecision(5) << testCountArray[l]; }

		std::cout << "]" << std::endl
				  << "   score = " << std::setw(5) << std::setprecision(5) << score << std::endl;
	}

	else {
		std::cout << " -> score = " << std::setw(5) << std::setprecision(5) << score << std::endl;
	}
}


double QuadraticScoreValue::getValue (double param) { return score; }

