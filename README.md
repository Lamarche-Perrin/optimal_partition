# Optimal Partition

## License

This program is a toolbox to solve special versions of the Set Partitioning
Problem, that is the combinatorial optimisation of a decomposable objective
over a set of feasible partitions (defined according to specific algebraic
structures: e.g., hierachies, sets of intervals, graphs). The objectives
are mainly based on information theory, in the perspective of multilevel
analysis of large-scale datasets, and the algorithms are based on dynamic
programming. For details regarding the formal grounds of this work, please
refer to:

Robin Lamarche-Perrin, Yves Demazeau and Jean-Marc Vincent. A Generic Set
Partitioning Algorithm with Applications to Hierarchical and Ordered Sets.
Technical Report 105/2014, Max-Planck-Institute for Mathematics in the
Sciences, Leipzig, Germany, May 2014.

<http://www.mis.mpg.de/publications/preprints/2014/prepr2014-105.html>

Copyright © 2015 Robin Lamarche-Perrin
(<Robin.Lamarche-Perrin@lip6.fr>)

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.


## Clone and Compile

You need to have **git**, **gcc** and **make** installed and working on your machine.

Then, run:
```
git clone https://github.com/Lamarche-Perrin/optimal_partition.git
cd optimal_partition
make
```

## Directories

Sources: `src/`<br/>
Binaries: `bin/`<br/>
Documentation: `doc/html/index.html`


## Geomediatic Aggregation (end-user program)

Source: `src/geomediatic_aggregation.cpp`<br/>
Binary: `bin/geomediatic_aggregation`

For example, run:
```
bin/geomediatic_aggregation --help
bin/geomediatic_aggregation --data input/GEOMEDIA/EU_cube.csv --output output/EU_partitions.csv --model ST --threshold 0.1
```

## Graph Compression (end-user program)

Source: `src/graph_compression.cpp`<br/>
Binary: `bin/graph_compression`

For example, run:
```
bin/graph_compression --help
bin/graph_compression -v -s 8 -i input/graphs/matrix.size\=8.prob\=0.33.csv -o output/graphs/matrix.size\=8.prob\=0.33.csv -t 0.001
```

## Library of Tests (specialized programs)

Sources: `src/programs.cpp`<br/>
Binary: `src/optimal_parition`

Uncomment the program of interest in `src/optimal_partition.cpp`, then run:
```
make
bin/optimal_partition
```