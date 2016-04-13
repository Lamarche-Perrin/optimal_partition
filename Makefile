CC=g++
CFLAGS=-pg -g -c -Wall -O3
LDFLAGS=-pg -O3
SOURCESA=bidimensional_relative_entropy.cpp csv_tools.cpp hierarchical_hierarchical_set.cpp orderedset.cpp timer.cpp dataset.cpp prediction_dataset.cpp hierarchical_ordered_set.cpp objective_function.cpp partition.cpp check_graph_datatree.cpp datatree.cpp hierarchical_set.cpp nonconstrained_ordered_set.cpp relative_entropy.cpp logarithmic_score.cpp quadratic_score.cpp abstract_set.cpp graph.cpp programs.cpp prediction_programs.cpp nonconstrained_set.cpp ring.cpp uni_set.cpp bi_set.cpp multi_set.cpp markov_process.cpp voter_graph.cpp # information_bottleneck.cpp
OBJECTSA=$(SOURCESA:.cpp=.o)
SOURCESB=optimal_partition.cpp geomediatic_aggregation.cpp
OBJECTSB=$(SOURCESB:.cpp=.o)
EXECUTABLE=$(SOURCESB:.cpp=)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTSA:%=src/%) $(OBJECTSB:%=src/%)
	$(CC) $(LDFLAGS) $(OBJECTSA:%=src/%) src/$@.o -o bin/$@

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) src/$*.cpp -o $@

clean:
	rm -f $(OBJECTSA:%=src/%) $(OBJECTSB:%=src/%) $(EXECUTABLE:%=bin/%)
