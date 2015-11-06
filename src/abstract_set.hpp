#ifndef INCLUDE_ABSTRACT_SET
#define INCLUDE_ABSTRACT_SET

/*!
 * \file abstract_set.hpp
 * \brief Abstract class defining a set of elements that one wants to partition while optimising some (decomposable) objective and preserving some algebraic constraints (set of feasible parts)
 * \author Robin Lamarche-Perrin
 * \date 06/11/2015
 */


#include "timer.hpp"
#include "objective_function.hpp"
#include "partition.hpp"
#include "dataset.hpp"


/*!
 * \class AbstractSet
 * \brief Abstract class defining a set of elements that one wants to partition while optimising some (decomposable) objective and preserving some algebraic constraints (set of feasible parts)
 */
class AbstractSet
{
public:
	ObjectiveFunction *objective; /*!< The objective that one wants to optimise (assumed to be decomposable: the objective of a partition is function of the objectives of its parts)*/

	/*!
     * \brief Destructor
     */
	virtual ~AbstractSet ();

	/*!
	 * \brief Randomly set the algebraic constraints for quick experiments (warning: this method is not always implemented)
     */
	virtual void setRandom () = 0;
	
	/*!
	 * \brief Build a proper data structure to represent the set and its algebraic constraints (warning: this method should always be called after instantiating and parameterising a set, and before calling any other method, such as print(), computeObjectiveValues(), computeOptimalPartition (double parameter), etc.)
     */
	virtual void buildDataStructure () = 0;

	/*!
	 * \brief Set the objective that one wants to optimise
     * \param objective : The objective function itself
     */
	virtual void setObjectiveFunction (ObjectiveFunction *objective) = 0;
	
	/*!
	 * \brief Print the set and its algebraic constraints
     */
	virtual void print () = 0;

	/*!
	 * \brief Compute the value of the objective function for each feasible part (warning: setObjectiveFunction (ObjectiveFunction *objective) should have been called first)
     */
	virtual void computeObjectiveValues () = 0;
	
	/*!
	 * \brief Finish computing the value of the objective function for each feasible part when normalisation is required (warning: only after computeObjectiveValues() has been called)
     */
	virtual void normalizeObjectiveValues () = 0;
	
	/*!
	 * \brief Print the value of the objective function for each feasible part
     */
	virtual void printObjectiveValues () = 0;

	/*!
	 * \brief Compute a partition that fits with the algebraic constraints and that optimises the objective function that has been specified
	 * \param parameter : The parameter of the objective function to be optimised (if the objective is parametrised)
     */
	virtual void computeOptimalPartition (double parameter) = 0;

	/*!
	 * \brief Compute and return a partition that fits with the algebraic constraints and that optimises the objective function that has been specified (warning: this method is not always implemented, but one can obtain a similar result by using getOptimalPartition (double parameter) and by calling print() on the result)
	 * \param parameter : The parameter of the objective function to be optimised (if the objective is parametrised)
	 * \return : The resulting optimal partition
     */	
	virtual Partition *getOptimalPartition (double parameter) = 0;

	/*!
	 * \brief Compute and print a partition that fits with the algebraic constraints and that optimises the objective function that has been specified (warning: this method is not always implemented, but one can obtain a similar result by using getOptimalPartition (double parameter) and by calling print() on the result)
	 * \param parameter : The parameter of the objective function to be optimised (if the objective is parametrised)
     */	
	virtual void printOptimalPartition (double parameter) = 0;

	
	/*!
	 * \brief Compute and return a list of partitions that fit with the algebraic constraints and that optimises the objective function that has been specified, while the parameter of the objective function varies on a proper ranged (defined by the objective itself)
	 * \param threshold : The minimal distance between two successive parameters giving birth to two different partitions
	 * \return : The resulting list of optimal partitions
     */	
	PartitionList *getOptimalPartitionList (double threshold);

	/*!
	 * \brief Compute and print a list of partitions that fit with the algebraic constraints and that optimises the objective function that has been specified, while the parameter of the objective function varies on a proper ranged (defined by the objective itself)
	 * \param threshold : The minimal distance between two successive parameters giving birth to two different partitions
     */	
	void printOptimalPartitionList (double threshold);
	
	/*!
	 * \brief Compute and print in a CSV file a list of partitions that fit with the algebraic constraints and that optimises the objective function that has been specified, while the parameter of the objective function varies on a proper ranged (defined by the objective itself)
	 * \param threshold : The minimal distance between two successive parameters giving birth to two different partitions
     */	
	void printOptimalPartitionListInCSV (double threshold, Dataset *data, int dim, std::string fileName);
		
	//void testAggregationAlgorithm (bool verbose = false, int param = -1);
	//void complexityAnalysis (char *fName, int start, int stop, int step, int number = 1);

private:
	void addOptimalPartitionList (PartitionList *partitionList, Partition *minPartition, Partition *maxPartition,
								  double minUnit, double maxUnit, double threshold);
};



#endif
