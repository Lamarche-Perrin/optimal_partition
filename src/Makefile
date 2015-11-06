CC=g++
CFLAGS=-pg -g -c -Wall
LDFLAGS=-pg
SOURCES= bidimensional_relative_entropy.cpp csv_tools.cpp hierarchical_hierarchical_set.cpp orderedset.cpp timer.cpp dataset.cpp prediction_dataset.cpp hierarchical_ordered_set.cpp measure.cpp partition.cpp check_graph_datatree.cpp datatree.cpp hierarchical_set.cpp nonconstrained_ordered_set.cpp relative_entropy.cpp logarithmic_score.cpp constrained_set.cpp graph.cpp main.cpp nonconstrained_set.cpp ring.cpp structure.cpp structure2D.cpp hyper_structure.cpp #markov_process.cpp bottleneck_measure.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=full_aggregation

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
