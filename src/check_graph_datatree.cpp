
#include <iostream>
#include <algorithm>

#include "check_graph_datatree.hpp"


bool checkParts (Graph *graph)
{
	std::cout << "CHECK PARTS";
	bool error = false;

	PartSet *mySet = graph->getParts();

	PartSet *set = new PartSet();
	Part *part = new Part();
	addRecVertex (set, part, 0, graph->size);

	for (PartSet::iterator it = set->begin(); it != set->end(); )
	{
		Vertices *v = (*it)->getVertices();
		if (!graph->isConnected(v)) {
			PartSet::iterator itBis = it;
			itBis++;
			delete *it;
			set->erase(it);
			it = itBis;
		} else { ++it; }
		delete v;
	}

/*
	std::cout << "COUNTING REAL PARTS:" << std::endl;
	for (PartSet::iterator it = set->begin(); it != set->end(); ++it) { (*it)->print(); std::cout << std::endl; }
	std::cout << "-> " << set->size() << " PARTS\n" << std::endl;

	std::cout << "COUNTING OUR PARTS:" << std::endl;
	for (PartSet::iterator it = mySet->begin(); it != mySet->end(); ++it) { (*it)->print(); std::cout << std::endl; }
	std::cout << "-> " << mySet->size() << " PARTS\n" << std::endl;
*/

	for (PartSet::iterator it1 = set->begin(); it1 != set->end(); ++it1)
	{
		Part *part = *it1;

		bool found = false;
		for (PartSet::iterator it2 = mySet->begin(); it2 != mySet->end() && !found; ++it2)
		{
			Part *myPart = *it2;
			found = part->equal(myPart);
		}
		
		if (!found)
		{
			std::cout << " -> ERROR: PART ";
			part->print();
			std::cout << " NOT FOUND" << std::endl;
			error = true;
		}	
	}

	for (PartSet::iterator it1 = mySet->begin(); it1 != mySet->end(); ++it1)
	{
		Part *myPart = *it1;

		bool found = false;
		for (PartSet::iterator it2 = set->begin(); it2 != set->end() && !found; ++it2)
		{
			Part *part = *it2;
			found = myPart->equal(part);
		}
		
		if (!found)
		{
			std::cout << " -> ERROR: PART ";
			myPart->print();
			std::cout << " SHOULD NOT BE THERE" << std::endl;
			error = true;
		}	
	}
	
	for (PartSet::iterator it1 = mySet->begin(); it1 != mySet->end(); ++it1)
	{
		Part *myPart1 = *it1;

		int foundNb = 0;
		for (PartSet::iterator it2 = mySet->begin(); it2 != mySet->end(); ++it2)
		{
			Part *myPart2 = *it2;
			if (myPart1->equal(myPart2)) { foundNb++; }
		}
		
		if (foundNb > 1)
		{
			std::cout << " -> ERROR: PART ";
			myPart1->print();
			std::cout << " FOUND " << foundNb << " TIMES" << std::endl;
			error = true;
		}	
	}

	if (set->size() != mySet->size())
	{
		std::cout << " -> ERROR: " << mySet->size() << " PARTS INSTEAD OF " << set->size() << std::endl;
		error = true;
	}
	
	if (!error)
	{
		std::cout << " -> ALL " << set->size() << " PARTS HAVE BEEN FOUND" << std::endl;
	}
	
	delete set;
	delete mySet;
	
	return error;
}


void checkPartitions (Graph *graph)
{
	std::cout << "CHECK PARTITIONS" << std::endl;
	bool error = false;
	
	VerticesSetsSet *set = new VerticesSetsSet();

	int s[16]; /* s[i] is the number of the set in which the ith element should go */
	int m[16]; /* m[i] is the largest of the first i elements in s */
 
	int n = graph->size;
	int i;

	for (i = 0; i < n; ++i) /* The first way to partition a set is to put all the elements in the same subset. */
	{
		s[i] = 1;
		m[i] = 1;
	}
 
	set->insert(getPartition(s,n));
	while (next(s,m,n)) { set->insert(getPartition(s,n)); }

	for (VerticesSetsSet::iterator it1 = set->begin(); it1 != set->end(); )
	{
		bool connected = true;
		VerticesSet *vSet = *it1;
		
		for (VerticesSet::iterator it2 = vSet->begin(); it2 != vSet->end(); ++it2)
		{
			if (!graph->isConnected(*it2)) { connected = false; }	
		}
		
		if (!connected)
		{
			for (VerticesSet::iterator it2 = vSet->begin(); it2 != vSet->end(); ++it2) { delete *it2; }
			delete *it1;
			set->erase(it1++);
		} else { ++it1; }
	}
	
	int partitionsNb = graph->printPartitions(false);
	if ((int) set->size() != partitionsNb)
	{
		std::cout << "ERROR: " << partitionsNb << " PARTITIONS INSTEAD OF " << set->size() << std::endl;
		error = true;
	}
	
	/*
	for (VerticesSetsSet::iterator it = set->begin(); it != set->end(); ++it)
	{
		VerticesSet *p = *it;
	}*/
	
	if (!error)
	{
		std::cout << "-> ALL " << partitionsNb << " PARTITIONS HAVE BEEN FOUND" << std::endl;
	}
	std::cout << std::endl;

	for (VerticesSetsSet::iterator it1 = set->begin(); it1 != set->end(); ++it1)
	{
		VerticesSet *vSet = *it1;
		for (VerticesSet::iterator it2 = vSet->begin(); it2 != vSet->end(); ++it2)
		{
			delete *it2;
		}
		delete *it1;
	}
	delete set;
}



void addRecVertex(PartSet *set, Part *part, int vMin, int vMax)
{
	for (int v = vMin; v < vMax; v++)
	{
		Part *newPart = new Part(part);
		newPart->addIndividual(v);
		set->insert(newPart);
		
		addRecVertex(set, newPart, v+1, vMax);
	}
}




VerticesSet *getPartition(int *s, int n) {
	VerticesSet *set = new VerticesSet();
	/* Get the total number of partitions. In the exemple above, 2.*/
	int part_num = 1;
	int i;
	for (i = 0; i < n; ++i)
		if (s[i] > part_num)
			part_num = s[i];
 
	/* Print the p partitions. */
	int p;
	for (p = part_num; p >= 1; --p) {
		/* If s[i] == p, then i + 1 is part of the pth partition. */
		Vertices *V = new Vertices();
		for (i = 0; i < n; ++i) {
			if (s[i] == p) {
				V->insert(i);
			}
		}
		set->insert(V);
	}
	return set;
}


 
/*
	next
		- given the partitioning scheme represented by s and m, generate
		the next
 
	Returns: 1, if a valid partitioning was found
		0, otherwise
*/
int next(int *s, int *m, int n) {
	/* Update s: 1 1 1 1 -> 2 1 1 1 -> 1 2 1 1 -> 2 2 1 1 -> 3 2 1 1 ->
	1 1 2 1 ... */
	/*int j;
	printf(" -> (");
	for (j = 0; j &lt; n; ++j)
		printf("%d, ", s[j]);
	printf("\\b\\b)\\n");*/
	int i = 0;
	++s[i];
	while ((i < n - 1) && (s[i] > m[i] + 1)) {
		s[i] = 1;
		++i;
		++s[i];
	}
 
	/* If i is has reached n-1 th element, then the last unique partitiong
	has been found*/
	if (i == n - 1)
		return 0;
 
	/* Because all the first i elements are now 1, s[i] (i + 1 th element)
	is the largest. So we update max by copying it to all the first i
	positions in m.*/
	int max = s[i];
	if (m[i] > max) max = m[i];
	for (i = i - 1; i >= 0; --i)
		m[i] = max;
 
/*  for (i = 0; i &lt; n; ++i)
		printf("%d ", m[i]);
	getchar();*/
	return 1;
}
 
/*
VerticesSetsSet *getAllPartitions (Tree *tree)
{
	VerticesSetsSet *set = new VerticesSetsSet();
	
	std::list<Tree*> *treeList = new std::list<Tree*>();
	std::list<VerticesSet*> *setList = new std::list<VerticesSet*>();

	treeList->push_back(tree->parent);
	strList->push_back(new VerticesSet());
	
	while (!treeList->empty())
	{
		Tree *node = treeList->front();
		treeList->pop_front();

		VerticesSet *set = setList->front();
		setList->pop_front();

		for (PartitionsSet::iterator it = node->partitions->begin(); it != node->partitions->end(); ++it)
		{
			Partition *p = *it;
			
			VerticesSet *nextSet1 = new VerticesSet();
			VerticesSet *nextSet2 = new VerticesSet();
			
			for (VerticesSet::iterator it2 = set->begin(); it2 != set->end(); it2++) {
				Vertices *V = *it2;

				Vertices *nextV1 = new Vertices();				
				Vertices *nextV2 = new Vertices();
						
				for (Vertices::iterator it3 = V->begin(); it3 != V->end(); it3++)
				{
					nextV1->insert(*it3);
					nextV2->insert(*it3);
				}
				
				nextSet1->insert(nextV1);
				nextSet2->insert(nextV2);
			}
			
			if (print)
			{
				std::cout << "{";
				p->first->printVertices(false);
				if (p->second != 0)
				{
					std::cout << ",";
					p->second->printVertices(false);
					nextStr1 = std::string(",") + p->second->toString() + str;
				}
				std::cout << str << "}" << std::endl;
			}
			partitionsNb++;

			treeList->push_back(p->first);
			strList->push_back(nextStr1);
			
			if (p->second != 0)
			{
				nextStr2 = std::string(",") + p->first->toString() + str;
				treeList->push_back(p->second);
				strList->push_back(nextStr2);
			}
		}
	}

	delete treeList;
	delete strList;
	if (print) { std::cout << "-> " << partitionsNb << " PARTITIONS\n" << std::endl; }

	return partitionsNb;
}*/
