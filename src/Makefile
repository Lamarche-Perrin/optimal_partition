CC=g++
CFLAGS=-pg -g -c -Wall
LDFLAGS=-pg
SOURCES= bidimensional_relative_entropy.cpp csv_tools.cpp hierarchical_hierarchical_set.cpp orderedset.cpp timer.cpp dataset.cpp prediction_dataset.cpp hierarchical_ordered_set.cpp objective_function.cpp partition.cpp check_graph_datatree.cpp datatree.cpp hierarchical_set.cpp nonconstrained_ordered_set.cpp relative_entropy.cpp logarithmic_score.cpp abstract_set.cpp graph.cpp main.cpp programs.cpp prediction_programs.cpp nonconstrained_set.cpp ring.cpp uni_set.cpp bi_set.cpp multi_set.cpp # information_bottleneck.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=optimal_partition

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
