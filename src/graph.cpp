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



#include <math.h>
#include <list>
#include <map>
#include <iostream>
#include <algorithm>

#include "graph.hpp"



Graph::Graph (int s)
{
    size = s;
    adjacencySets = new Vertices *[size];
    graphComponents = new GraphComponent *[size];
    graphComponentSet = new GraphComponentSet();

    value = 0;
    reachedVertices = new bool [size];

    for (int v = 0; v < size; v++)
    {
		adjacencySets[v] = new Vertices();
		reachedVertices[v] = false;
    }
}


RandomGraph::RandomGraph (int vNum, int eNum) : Graph (vNum)
{
    for (int i = 0; i < eNum; )
    {
		int v1 = rand() % vNum;
		int v2 = rand() % vNum;
		
		if (v1 == v2) continue; // NOT A GOOD WAY!
		if (adjacencySets[v1]->find(v2) != adjacencySets[v1]->end()) continue;
		
		addEdge(v1,v2);
		i++;
    }
}


FiliformGraph::FiliformGraph (int vNum) : Graph (vNum)
{
    for (int v = 1; v < vNum; v++) { addEdge(v-1,v); }
}


RingGraph::RingGraph (int vNum) : Graph (vNum)
{
    for (int v = 1; v < vNum; v++) { addEdge(v-1,v); }
    addEdge(vNum-1,0);
}



Graph::~Graph ()
{
    for (int v = 0; v < size; v++) { delete adjacencySets[v]; }

    delete[] adjacencySets;
    delete[] graphComponents;
	
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it) { delete *it; } 
    delete graphComponentSet;

    delete[] reachedVertices;
}


void Graph::setRandom ()
{
    for (int v1 = 0; v1 < size; v1++)
		for (int v2 = v1+1; v2 < size; v2++)
			if (rand()/RAND_MAX > 0.5) addEdge(v1,v2);
}


void Graph::addEdge (int v1, int v2)
{
    adjacencySets[v1]->insert(v2);
    adjacencySets[v2]->insert(v1);
}


void Graph::buildFromBinary (int index)
{
    for (int v1 = 0; v1 < size; v1++)
		for (int v2 = v1+1; v2 < size; v2++)
		{
			if (index % 2 == 1) { addEdge(v1,v2); }
			index = index >> 1;
		}
}


bool Graph::areAdjacent (int v1, int v2)
{
    return adjacencySets[v1]->find(v2) != adjacencySets[v1]->end();
}


bool Graph::areAdjacent (int v1, Vertices *v2)
{
    for (Vertices::iterator it = v2->begin(); it != v2->end(); ++it)
		if (adjacencySets[v1]->find(*it) != adjacencySets[v1]->end()) return true;
    return false;
}


bool Graph::areAdjacent (Vertices *v1, int v2) { return areAdjacent(v2,v1); }


bool Graph::areAdjacent (Vertices *v1, Vertices *v2)
{
    for (Vertices::iterator it = v1->begin(); it != v1->end(); ++it)
		if (areAdjacent(*it,v2)) return true;
    return false;
}


bool Graph::areAdjacent (int v1, VVertices *v2)
{
    for (VVertices::iterator it = v2->begin(); it != v2->end(); ++it)
		if (adjacencySets[v1]->find(*it) != adjacencySets[v1]->end()) return true;
    return false;
}


bool Graph::areAdjacent (VVertices *v1, int v2) { return areAdjacent(v2,v1); }


bool Graph::areAdjacent (VVertices *v1, VVertices *v2)
{
    for (VVertices::iterator it = v1->begin(); it != v1->end(); ++it)
		if (areAdjacent(*it,v2)) return true;
    return false;
}



Vertices *Graph::getAdjacentVertices (int v, int vMax)
{
    if (vMax == -1) { vMax = v; }
    Vertices *adjVertices = new Vertices();
    Vertices *adjSet = adjacencySets[v];
    for (Vertices::iterator it = adjSet->begin(); it != adjSet->end(); ++it)
    {
		int adjV = *it;
		if (adjV < vMax) { adjVertices->insert(adjV); }
    }
    return adjVertices;
}



void Graph::buildDataStructure ()
{
    int lastReachedVertex = 0;
	
    while (lastReachedVertex < size)
    {
		if (!reachedVertices[lastReachedVertex])
		{
			GraphComponent *component = new GraphComponent(this);
			std::list<int> *connectedVertices = new std::list<int>();
			std::list<int> *nextVertices = new std::list<int>();
			nextVertices->push_back(lastReachedVertex);
			reachedVertices[lastReachedVertex] = true;
			
			while (!nextVertices->empty())
			{
				int vertex = nextVertices->front();
				nextVertices->pop_front();
				
				connectedVertices->push_back(vertex);
				graphComponents[vertex] = component;
				
				Vertices *adjVertices = getAdjacentVertices(vertex,size);
				for (Vertices::iterator it = adjVertices->begin(); it != adjVertices->end(); ++it)
				{
					int adjVertex = *it;
					if (!reachedVertices[adjVertex])
					{
						nextVertices->push_back(adjVertex);
						reachedVertices[adjVertex] = true;
					}
				}
				delete adjVertices;
			}
			
			delete nextVertices;
			
			component->setVertices(connectedVertices);
			graphComponentSet->insert(component);
			
			delete connectedVertices;			
		}
		
		lastReachedVertex++;
    }
	
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
		(*it)->buildDataStructure();
}




void Graph::enumerateSubsets (VVertices *R, VVertices *F, int m, int n, VVertices *T, int q = 0, int r = 0)
{
    if (q == n)
    {
		VVertices *newR = new VVertices(*R);
		newR->insert(newR->end(), T->begin(), T->end());
		for (VVertices::iterator it = newR->begin(); it != newR->end(); ++it)
			std::cout << (*it) << "\t";
		std::cout << std::endl;
	
		VVertices *newF = new VVertices();
		for (int j = *std::min_element(newR->begin(),newR->end())+1; j < size; j++)
		{
			if (areAdjacent(T,j) && !areAdjacent(R,j) && std::find(newR->begin(),newR->end(),j) == newR->end()) {newF->push_back(j); }
		}
		slyce(newR,newF,m-n);

		delete newR;
		delete newF;
    }

    else
    {
        for (int i = r; i < m; i++)
        {
            T[q] = F[i];
            enumerateSubsets(R,F,m,n,T,q+1,i+1);
        }
    }
}


void Graph::slyce (VVertices *R, VVertices *F, int m)
{
    if (F->size() > 0 && m > 0)
    {
		for (int n = 1; n <= m; n++)
		{
			VVertices *T = new VVertices();
			enumerateSubsets (R,F,m,n,T);
			delete T;
		}
    }
}

	    
void Graph::buildDataStructureWithSlyce ()
{
    for (int i = 0; i < size; i++)
    {
		VVertices *R = new VVertices();
		VVertices *F = new VVertices(); F->push_back(i);
		slyce(R,F,size);
    }
}



bool Graph::isConnected ()
{
    int reachedsize = 0;
    for (int v = 0; v < size; v++) { reachedVertices[v] = false; }

    std::list<int> *nextVertices = new std::list<int>();
    nextVertices->push_back(0);
    reachedVertices[0] = true;
    reachedsize++;

    while (!nextVertices->empty())
    {
		int currentVertex = nextVertices->front();
		nextVertices->pop_front();

		Vertices *adjList = adjacencySets[currentVertex];
		for (Vertices::iterator it = adjList->begin(); it != adjList->end(); ++it) {
			int adjVertex = *it;
			if (!reachedVertices[adjVertex]) {
				nextVertices->push_back(adjVertex);
				reachedVertices[adjVertex] = true;
				reachedsize++;
			}
		}
    }
	
    delete nextVertices;
	
    return reachedsize == size;
}


bool Graph::isConnected (Vertices *V)
{
    Vertices *nextVertices = new Vertices();
    int reachedsize = 1;

    bool first = true;
    for (Vertices::iterator it = V->begin(); it != V->end(); ++it)
    {
		int v = *it;
		if (first)
		{
			nextVertices->insert(v);
			reachedVertices[v] = true;
			first = false;
		}
		else { reachedVertices[v] = false; }
    }

    while (!nextVertices->empty())
    {
		Vertices::iterator it = nextVertices->end(); --it;
		int currentVertex = *it;
		nextVertices->erase(it);
	
		Vertices *adjList = adjacencySets[currentVertex];
		for (Vertices::iterator it = adjList->begin(); it != adjList->end(); ++it) {
			int adjV = *it;
			if (V->find(adjV) != V->end())
			{
				if (!reachedVertices[adjV]) {
					reachedVertices[adjV] = true;
					reachedsize++;
					nextVertices->insert(adjV);
				}
			}
		}
    }
	
    delete nextVertices;
	
    return reachedsize == (int) V->size();
}



void Graph::print ()
{
    std::cout << "PRINTING GRAPH:" << std::endl;
    for (int v = 0; v < size; v++)
    {
		std::cout << "VERTEX " << v << " adj=";
		printVertices(adjacencySets[v]);
		std::cout << std::endl;
    }
    std::cout << std::endl;
}


void Graph::printVertices (Vertices *V)
{
    std::cout << "{";
    bool first = true;
    for (Vertices::iterator it = V->begin(); it != V->end(); ++it)
    {
		if (!first) { std::cout << ","; }
		std::cout << *it;
		first = false;
    }
    std::cout << "}";
}




void Graph::setObjectiveFunction (ObjectiveFunction *m)
{
    objective = m;
    value = m->newObjectiveValue();
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
		(*it)->datatree->setObjectiveFunction(m);
}


void Graph::computeObjectiveValues ()
{
	objective->computeObjectiveValues();
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
    {
		GraphComponent *component = *it;
		component->datatree->computeObjectiveValues();
		value->add(component->datatree->value);
    }
}


void Graph::normalizeObjectiveValues ()
{
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
		(*it)->datatree->normalizeObjectiveValues(value);
}


void Graph::printObjectiveValues ()
{
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
		(*it)->datatree->printObjectiveValues();
}


void Graph::computeOptimalPartition (double parameter)
{
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
		(*it)->datatree->computeOptimalPartition(parameter);
}


void Graph::printOptimalPartition (double parameter)
{
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
		(*it)->datatree->printOptimalPartition(parameter);
}


Partition *Graph::getOptimalPartition (double parameter)
{
    Partition *partition = new Partition();
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
    {
		Partition *p = (*it)->datatree->getOptimalPartition(parameter);
		for (std::list<Part*>::iterator it2 = p->parts->begin(); it2 != p->parts->end(); ++it2)
			partition->addPart(*it2);
    }
    return partition;
}


void Graph::printDataStructure (bool verbose)
{
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it) { (*it)->datatree->print(verbose); } 	
}


PartSet *Graph::getParts ()
{
    PartSet *set = new PartSet ();
	
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it)
    {
		PartSet *newSet = (*it)->datatree->getParts();
		set->insert(newSet->begin(),newSet->end());
		delete newSet;
    }
	
    return set;
}


void Graph::printParts ()
{
    std::cout << "COUNTING PARTS:" << std::endl;

    PartSet *set = getParts();
    for (PartSet::iterator it = set->begin(); it != set->end(); ++it) { (*it)->print(); std::cout << std::endl; }

    std::cout << "-> " << set->size() << " PARTS\n" << std::endl;
}


int Graph::printPartitions (bool print) // CARTESIAN PRODUCT TODO
{
    for (GraphComponentSet::iterator it = graphComponentSet->begin(); it != graphComponentSet->end(); ++it) { (*it)->datatree->printPartitions(print); }
    return 0;
}




GraphComponent::GraphComponent (Graph *g)
{
    graph = g;
    datatree = 0;
    size = 0;
    vertices = 0;
    order = 0;
}


void GraphComponent::setVertices (std::list<int> *vertexList)
{
    datatree = new Datatree();
    size = vertexList->size();
    vertices = new int [size];
    order = new std::map<int,int>();
	
    int i = 0;
    while (!vertexList->empty())
    {
		int vertex = vertexList->front();
		vertexList->pop_front();

		vertices[i] = vertex;
		order->insert(std::make_pair(vertex,i));
		i++;
    }
}


GraphComponent::~GraphComponent ()
{
    delete[] vertices;
    delete order;
    delete datatree;
}


struct TreeToAdd
{
    Datatree *node;
    Datatree *nodeToAdd;
    Vertices *vertices;
    Vertices *adjVertices;
};


void GraphComponent::buildDataStructure ()
{
    for (int index = 0; index < size; index++)
    {
		int firstVertex = vertices[index];
		Datatree *tree = new Datatree(firstVertex);
		Vertices *firstVertices = new Vertices();
		firstVertices->insert(firstVertex);
	
		std::list<TreeToAdd> treeToAddList;
		struct TreeToAdd firstTreeToAdd = {tree,0,firstVertices,0};
		treeToAddList.push_back(firstTreeToAdd);
	
		while (!treeToAddList.empty())
		{
			struct TreeToAdd treeToAdd = treeToAddList.front();
			Datatree *node = treeToAdd.node;
			Vertices *vertices = treeToAdd.vertices;
			treeToAddList.pop_front();
	
			if (VERBOSE) {
				for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
				std::cout << "(" << VERBOSE_TAB << ") CURRENTLY ON NODE ";
				node->printVertices();
				VERBOSE_TAB++;
			}
			
			Vertices *adjVertices = graph->getAdjacentVertices(node->vertex,graph->size);
			for (Vertices::iterator it = adjVertices->begin(); it != adjVertices->end(); ++it)
			{
				int childVertex = *it;
				if (order->at(childVertex) >= order->at(firstVertex) && vertices->find(childVertex) == vertices->end())
				{
					vertices->insert(childVertex);

					Datatree *child = node->addChild(childVertex);
					Vertices *childVertices = new Vertices(*vertices);
					struct TreeToAdd childTreeToAdd = {child,0,childVertices,0};
					treeToAddList.push_back(childTreeToAdd);
										
					if (node->wholeSet)
					{
						datatree->parent = child;
						child->wholeSet = true;
						node->wholeSet = false;
					}
				}
			}
			
			delete adjVertices;
			delete vertices;
			
			if (VERBOSE) { VERBOSE_TAB--; }
		}
		
		treeToAddList.clear();
		struct TreeToAdd newTreeToAdd = {datatree,tree,new Vertices (),0};
		treeToAddList.push_back(newTreeToAdd);
		
		while (!treeToAddList.empty())
		{
			struct TreeToAdd treeToAdd = treeToAddList.front();
			Datatree *node = treeToAdd.node;
			Datatree *nodeToAdd = treeToAdd.nodeToAdd;
			Vertices *vertices = treeToAdd.vertices;
//			Vertices *adjVertices = treeToAdd.adjVertices;
			treeToAddList.pop_front();
	
			if (VERBOSE) {
				for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
				std::cout << "(" << VERBOSE_TAB << ") ADJUSTING NODE ";
				node->printVertices(false);
				std::cout << " WITH ";
				nodeToAdd->printVertices(false);
				std::cout << " KNOWING ";
				graph->printVertices(vertices);
				std::cout << std::endl;				
				VERBOSE_TAB++;
			}

			if (vertices->find(nodeToAdd->vertex) == vertices->end())
			{
				node = node->addChild(nodeToAdd->vertex);
				vertices->insert(nodeToAdd->vertex);
			}
			
			for (TreesSet::iterator it = nodeToAdd->children->begin(); it != nodeToAdd->children->end(); ++it)
			{
				Datatree *child = *it;
				
				if (vertices->find(child->vertex) == vertices->end())
				{
					Vertices *childVertices = new Vertices (*vertices);
					struct TreeToAdd childToAdd = {node,child,childVertices,0};
					treeToAddList.push_back(childToAdd);
					vertices->insert(child->vertex);
				}
			}
			
			if (nodeToAdd->parent != 0)
			{
				Vertices *parentVertices = new Vertices (*vertices);
				struct TreeToAdd parentToAdd = {node,nodeToAdd->parent,parentVertices,0};
				treeToAddList.push_back(parentToAdd);
			}
			
			delete vertices;
			//delete adjVertices;
	
			if (VERBOSE) { VERBOSE_TAB--; }
		}
    }
	
    /*
    // COMPUTE COMPLEMENTS
    datatree->complement = datatree;
	
    for (int index = 0; index < size; index++)
    {
    TreesList *nodeList = new TreesList();

    // ADD NEW COMPLEMENTS TO COMPLEMENT LISTS
    nodeList->push_back(datatree);
    while (!nodeList->empty())
    {
    Datatree *node = nodeList->front();
    nodeList->pop_front();
			
    node->complementList->push_front(node->complement);
    node->complement = 0; // NOT NECESSARY? (TO ERASE)

    for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
    {
    if (order->at(child->vertex) < index)
    {
    nodeList->push_back(child);
    }
    }
    }
		
    // ADD VERTICES WITH INDEX = index
    nodeList->push_back(datatree);
    while (!nodeList->empty())
    {
    Datatree *node = nodeList->front();
    nodeList->pop_front();
			
    for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it)
    {
    Datatree *child = *it;

    if (order->at(child->vertex) < index)
    {
    nodeList->push_back(child);
    }

    if (order->at(child->vertex) == index)
    {
    TreesList::iterator it2 = node->complementList->begin();
					
    while (child != 0)
    {
    child->complement = *it2;
    child->complement->complement = child;
					
    child = 0;
    //if (child->children->size() > 1) { std::cout << "WARNING: several added branches!" << std::endl; }
    for (TreesSet::iterator it3 = child->children->begin(); it3 != child->children->end(); ++it3)
    {
    Datatree *newChild = *it;
    if (order->at(newChild->vertex) < index)
    {
    newChild->complement = *it2;
    newChild->complement->complement = newChild;
    it2++;
    }
    }
    }
    }
    }
    }
    */
}


/*

  struct TreeToAdd
  {
  Datatree *node;
  Datatree *tree;
  Vertices *vertices;
  bool nodeFound;
  };


  void GraphComponent::buildDataStructure ()
  {
  std::map<int,Datatree*> treeToAddMap;
  std::list<TreeToAdd> treeToAddList;

  datatree->wholeSet = true;
  datatree->complement = datatree;

  // Adding vertex after vertex
  for (int index = 0; index < size; index++)
  {
  int vertex = vertices[index];
		
  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") ADD VERTEX " << vertex << std::endl;
  VERBOSE_TAB++;
  }

  // Add node to root
  Datatree *newNode = datatree->addChild(vertex);		
  newNode->newComplement = datatree->complement;
  newNode->newComplement->newComplement = newNode;
  newNode->wholeSet = datatree->wholeSet;
  if (newNode->wholeSet) { datatree->parent = newNode; }
  datatree->wholeSet = false;
		
  // Build and initialize map of nodes to add
  std::list<int> *verticesList = new std::list<int>();
		
  Vertices *adjVertices = graph->getAdjacentVertices(vertex,graph->size);
  for (Vertices::iterator it1 = adjVertices->begin(); it1 != adjVertices->end(); )
  {
  int adjVertex = *it1;
  if (order->at(adjVertex) < order->at(vertex))
  {
  bool added = false;
  for (std::list<int>::iterator it2 = verticesList->begin(); it2 != verticesList->end() && !added; ++it2)
  {
  if (order->at(*it2) < order->at(adjVertex)) { verticesList->insert(it2,adjVertex); added = true; }
  }
  if (!added) { verticesList->push_back(adjVertex); }
				
  treeToAddMap[adjVertex] = new Datatree(vertex);
  ++it1;
  }
  else { adjVertices->erase(it1++); }
  }
		
  // Add nodes to each adjacent vertex
  while (!verticesList->empty())
  {
  int adjVertex = verticesList->front();
  verticesList->pop_front();

  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") CONNECT VERTEX " << vertex << " TO VERTEX " << adjVertex << std::endl;
  VERBOSE_TAB++;
  }
			
  // Initialize the list
  struct TreeToAdd firstTreeToAdd = {datatree,treeToAddMap[adjVertex],adjVertices,false};
  treeToAddList.push_back(firstTreeToAdd);
					
  // While there is a node to check
  while (!treeToAddList.empty())
  {
  struct TreeToAdd treeToAdd = treeToAddList.front();
  Datatree *node = treeToAdd.node;
  Datatree *tree = treeToAdd.tree;
  Vertices *vertices = treeToAdd.vertices;
  bool nodeFound = treeToAdd.nodeFound;
  treeToAddList.pop_front();

  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") TRY TO CONNECT VERTEX " << vertex << " TO ";
  node->printVertices();
  VERBOSE_TAB++;
  }

  // If we are on the adjacent vertex
  if (node->vertices != 0 && node->containsVertex(adjVertex))
  {
  nodeFound = true;
  }

  // Continue to run through the tree
  for (TreesSet::iterator it1 = node->children->begin(); it1 != node->children->end(); ++it1)
  {
  Datatree *child = *it1;
  Datatree *childTreeToAdd = 0;

  // Which tree adding for the following nodes?
  std::list< std::pair<Datatree*,Datatree*> > nodeToAddList;
  nodeToAddList.push_back(std::make_pair(tree,childTreeToAdd));
					
  while (!nodeToAddList.empty())
  {
  Datatree *nodeToAdd = nodeToAddList.front().first;
  Datatree *currentNode = nodeToAddList.front().second;
  nodeToAddList.pop_front();
						
  if (!(order->at(child->getOnlyVertex()) > order->at(adjVertex) && graph->areAdjacent(nodeToAdd->getOnlyVertex(),child->getOnlyVertex())
  || nodeToAdd->getOnlyVertex() != vertex
  && (nodeToAdd->getOnlyVertex() == child->getOnlyVertex()
  || graph->areAdjacent(nodeToAdd->getOnlyVertex(),child->getOnlyVertex()))))
  {
  Datatree *nextNode;
  if (currentNode == 0)
  {
  childTreeToAdd = new Datatree(nodeToAdd->getOnlyVertex());
  nextNode = childTreeToAdd;
  }
  else { nextNode = currentNode->addChild(nodeToAdd->getOnlyVertex(),false); }
							
  for (TreesSet::iterator it2 = nodeToAdd->children->begin(); it2 != nodeToAdd->children->end(); it2++)
  {
  nodeToAddList.push_back(std::make_pair((*it2),nextNode));
  }
  }
  }
					
  // Add next move if non empty
  if (childTreeToAdd != 0)
  {
  // Which adjacent vertices are concerned by the node adding?
  Vertices *childVertices = new Vertices();
  for (Vertices::iterator it2 = vertices->begin(); it2 != vertices->end(); ++it2)
  {
  int v2 = *it2;
  if (!child->containsVertex(v2) && !graph->areAdjacent(v2,child->vertices)) { childVertices->insert(v2); }
  }

  struct TreeToAdd newTreeToAdd = {child,childTreeToAdd,childVertices,nodeFound};
  treeToAddList.push_back(newTreeToAdd);
  }
  }
				
  // Now we can add nodes
  if (nodeFound)
  {
  std::list< std::pair<Datatree*,Datatree*> > nodeToAddList;
  nodeToAddList.push_back(std::make_pair(tree,node));
					
  while (!nodeToAddList.empty())
  {
  Datatree *nodeToAdd = nodeToAddList.front().first;
  Datatree *currentNode = nodeToAddList.front().second;
  nodeToAddList.pop_front();
						
  Datatree *nextNode = currentNode->addChild(nodeToAdd->getOnlyVertex());
						
  for (TreesSet::iterator it2 = nodeToAdd->children->begin(); it2 != nodeToAdd->children->end(); it2++)
  {
  nodeToAddList.push_back(std::make_pair((*it2),nextNode));
  }								
						
  // Complete the tree to add of other adjacent vertices
  for (Vertices::iterator it2 = vertices->begin(); it2 != vertices->end(); ++it2)
  {
  int otherAdjVertex = *it2;
							
  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") ADJUST TREE OF NODE " << otherAdjVertex << std::endl;
  VERBOSE_TAB++;
  }

  std::list<int> vertexList;
  Datatree *nextNode2 = nextNode;
  while (nextNode2->vertices != 0)
  {
  if (nextNode2->getOnlyVertex() != vertex && nextNode2->getOnlyVertex() != adjVertex)
  { vertexList.push_front(nextNode2->getOnlyVertex()); }
  nextNode2 = nextNode2->parent;
  }

  Datatree *treeToAdd = treeToAddMap[otherAdjVertex];
  treeToAdd = treeToAdd->findOrAddChild(adjVertex);							
  while (!vertexList.empty())
  {
  int vertex = vertexList.front();
  vertexList.pop_front();
  treeToAdd = treeToAdd->findOrAddChild(vertex);
  }

  if (VERBOSE) { VERBOSE_TAB--; }

  }
  }
  }
				
  if (VERBOSE) { VERBOSE_TAB--; }
  }
			
  treeToAddList.clear();
  if (VERBOSE) { VERBOSE_TAB--; }
  }

  delete adjVertices;
  delete verticesList;
		
		
  treeToAddMap.clear();
		
  if (VERBOSE) { VERBOSE_TAB--; }
  }
	
  datatree->addBipartition(datatree->parent,0);
  }


  struct InfoToAdd
  {
  Datatree *node;
  Vertices *vertices;
  TreesSet *nodesToAdd;
  bool foundVertex;
  };



  void GraphComponent::buildDataStructure ()
  {
  std::map<int,TreesSet*> nodesToAddMap;
  std::list<InfoToAdd> nodesToAddList;

  datatree->wholeSet = true;
  datatree->complement = datatree;

  // Adding vertex after vertex
  for (int index = 0; index < size; index++)
  {
  int vertex = vertices[index];
		
  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") ADD VERTEX " << vertex << std::endl;
  VERBOSE_TAB++;
  }

  // Add node to root
  Datatree *newNode = datatree->addChild(vertex);		
  newNode->newComplement = datatree->complement;
  newNode->newComplement->newComplement = newNode;
  newNode->wholeSet = datatree->wholeSet;
  if (newNode->wholeSet) { datatree->parent = newNode; }
  datatree->wholeSet = false;
		
  // Build and initialize map of nodes to add
  std::list<int> *verticesList = new std::list<int>();
		
  Vertices *adjVertices = graph->getAdjacentVertices(vertex,graph->size);
  for (Vertices::iterator it1 = adjVertices->begin(); it1 != adjVertices->end(); )
  {
  int adjVertex = *it1;
  if (order->at(adjVertex) < order->at(vertex))
  {
  bool added = false;
  for (std::list<int>::iterator it2 = verticesList->begin(); it2 != verticesList->end() && !added; ++it2)
  {
  if (order->at(*it2) < order->at(adjVertex)) { verticesList->insert(it2,adjVertex); added = true; }
  }
  if (!added) { verticesList->push_back(adjVertex); }
				
  nodesToAddMap[adjVertex] = new TreesSet();
  nodesToAddMap[adjVertex]->insert(newNode);
  ++it1;
  }
  else { adjVertices->erase(it1++); }
  }
		
  // Add nodes to each adjacent vertex
  while (!verticesList->empty())
  {
  int adjVertex = verticesList->front();
  verticesList->pop_front();

  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") CONNECT VERTEX " << vertex << " TO VERTEX " << adjVertex << std::endl;
  VERBOSE_TAB++;
  }
			
  // Initialize the list
  struct InfoToAdd newInfo = {datatree,adjVertices,nodesToAddMap[adjVertex],false};
  nodesToAddList.push_back(newInfo);
					
  // While there is a node to check
  while (!nodesToAddList.empty())
  {
  struct InfoToAdd info = nodesToAddList.front();
  Datatree *node = info.node;
  Vertices *vertices = info.vertices;
  TreesSet *nodesToAdd = info.nodesToAdd;
  bool foundVertex = info.foundVertex;
  nodesToAddList.pop_front();

  if (VERBOSE) {
  for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
  std::cout << "(" << VERBOSE_TAB << ") TRY TO CONNECT VERTEX " << vertex << " TO ";
  node->printVertices();
  VERBOSE_TAB++;
  }

  // If we are on the adjacent vertex
  if (node->vertices != 0 && node->containsVertex(adjVertex))
  {
  //if (nodesToAdd != 0) { std::cout << "WARNING! Two times the same vertex in one branch." << std::endl; } // TO BE SUPPRESSED
//					nodesToAdd = nodesToAddMap[adjVertex];
foundVertex = true;
}

// Continue to run through the tree
//				if (order->at(node->getLastVertex()) < order->at(adjVertex) || nodesToAdd->size() != 0)
//				{
for (TreesSet::iterator it1 = node->children->begin(); it1 != node->children->end(); ++it1)
{
Datatree *child = *it1;
Vertices *childVertices = new Vertices();
TreesSet *childNodesToAdd = new TreesSet();

int minOrder = graph->size;
for (Vertices::iterator it = child->vertices->begin(); it != child->vertices->end(); ++it)
{
int newOrder = order->at(*it);
if (newOrder < minOrder) { minOrder = newOrder; }
}
					
if (foundVertex || minOrder <= order->at(adjVertex))
{
//						std::cout << "OK ";
// Which adjacent vertices are concerned by the node adding?
for (Vertices::iterator it2 = vertices->begin(); it2 != vertices->end(); ++it2)
{
int v2 = *it2;
if (!child->containsVertex(v2) && !graph->areAdjacent(v2,child->vertices)) { childVertices->insert(v2); }
}
						
// Which node adding for the following nodes?
for (TreesSet::iterator it2 = nodesToAdd->begin(); it2 != nodesToAdd->end(); ++it2)
{
//							std::cout << "TEST";
Datatree *nodeToAdd = *it2;
Datatree *currentNode = nodeToAdd;
							
bool adj = false;
while (!adj && currentNode->vertices != 0)
{
for (Vertices::iterator it3 = currentNode->vertices->begin(); it3 != currentNode->vertices->end() && !adj; ++it3)
for (Vertices::iterator it4 = child->vertices->begin(); it4 != child->vertices->end() && !adj; ++it4)
{
int addV = *it3;
int childV = *it4;
if (order->at(childV) > order->at(adjVertex) && graph->areAdjacent(addV,childV)
|| addV != vertex && graph->areAdjacent(addV,child->vertices)) { adj = true; }
}

currentNode = currentNode->parent;
}
							
if (!adj) { childNodesToAdd->insert(nodeToAdd); } //std::cout << " NEXT\n"; } else { std::cout << std::endl; }
}
					
// Add next move if non empty
if (!childNodesToAdd->empty())
{
InfoToAdd newInfo = {child,childVertices,childNodesToAdd,foundVertex};
nodesToAddList.push_back(newInfo);
} else {
delete childVertices;
delete childNodesToAdd;
}
}
}
				
// If there is at least one node to add
if (foundVertex)
{
for (TreesSet::iterator it1 = nodesToAdd->begin(); it1 != nodesToAdd->end(); ++it1)
{
Datatree *nodeToAdd = *it1;
Datatree *newNode = node->addChild(nodeToAdd->getAllVertices());
						
// Set up new complement
if (node->complement != 0)
{
if (nodeToAdd->parent->vertices == 0)
{
newNode->newComplement = node->complement;
newNode->newComplement->newComplement = newNode;
} else {
newNode->newComplement = node->complement;
while (std::includes(newNode->vertices->begin(),newNode->vertices->end(),
newNode->newComplement->vertices->begin(),newNode->newComplement->vertices->end()))
newNode->newComplement = newNode->newComplement->parent;
									
bool ok = true;
Datatree *currentNode = newNode->newComplement;
while (currentNode->vertices != 0 && ok)
{
for (Vertices::iterator it = currentNode->vertices->begin(); it != currentNode->vertices->end() && ok; ++it)
{ ok = !newNode->containsVertex(*it); }
currentNode = currentNode->parent;
}
if (!ok) { newNode->newComplement = 0; }
else { newNode->newComplement->newComplement = newNode; }
}
}

// Here, we assume that all iterative subgraphs are connex
newNode->wholeSet = node->wholeSet;
if (newNode->wholeSet) { datatree->parent = newNode; }
node->wholeSet = false;
						
for (Vertices::iterator it2 = vertices->begin(); it2 != vertices->end(); ++it2) { nodesToAddMap[*it2]->insert(newNode); }
}
}
				
if (VERBOSE) { VERBOSE_TAB--; }
}
			
nodesToAddList.clear();
if (VERBOSE) { VERBOSE_TAB--; }
}		

delete adjVertices;
delete verticesList;
		
// Reinitialize complements
std::list<Datatree*> *list = new std::list<Datatree*>();
list->push_back(datatree);
		
while (!list->empty())
{
Datatree *node = list->front();
list->pop_front();
node->complement = node->newComplement;
node->newComplement = 0;
			
for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
}

										
// Add bipartitions
if (index > 0) { list->push_back((datatree->findChild(vertices[0]))->findChild(vertices[1])); }

while (!list->empty())
{
Datatree *node = list->front();
for (TreesSet::iterator it = node->children->begin(); it != node->children->end(); ++it) { list->push_back(*it); }
list->pop_front();
			
if (node->vertices == 0) continue;
if (!node->bipartitions->empty()) continue;
		
if (VERBOSE) {
for (int i = 0; i < VERBOSE_TAB; i++) { std::cout << "..."; }
std::cout << "(" << VERBOSE_TAB << ") FIND BIPARTITIONS OF ";
node->printVertices();
VERBOSE_TAB++;
}

if (node->wholeSet)
{	
Datatree *currentNode = node;
while (currentNode->parent->vertices != 0) { currentNode = currentNode->parent; }
				
if (currentNode != node)
{
std::list<Datatree*> *list = new std::list<Datatree*>();
list->push_back(currentNode);
									
while (!list->empty())
{
currentNode = list->front();
list->pop_front();
						
if (currentNode->complement != 0) { node->addBipartition(currentNode,currentNode->complement); }
						
for (TreesSet::iterator it2 = currentNode->children->begin(); it2 != currentNode->children->end(); ++it2)
if (*it2 != node) { list->push_back(*it2); }
}
}
}
			
else {
for (BipartitionsSet::iterator it2 = node->parent->bipartitions->begin(); it2 != node->parent->bipartitions->end(); ++it2)
{
Bipartition *p = *it2;
Datatree *n1 = p->first;
Datatree *n2 = p->second;
									
if (graph->areAdjacent(node->vertices,n1->getAllVertices()))
node->addBipartition(n2,n1->findChild(node->vertices));
	
if (graph->areAdjacent(node->vertices,n2->getAllVertices()))
node->addBipartition(n1,n2->findChild(node->vertices));
}
}
			
if (VERBOSE) { VERBOSE_TAB--; }

}

delete list;
		
nodesToAddMap.clear();
		
if (VERBOSE) { VERBOSE_TAB--; }
}
	
datatree->addBipartition(datatree->parent,0);
}
*/


/*
  void GraphComponent::print ()
  {
  std::cout << "PRINTING GRAPH:" << std::endl;
  for (int v = 0; v < size; v++)
  {
  std::cout << "VERTEX " << v << " adj=";
  printVertices(adjacencySets[v]);
  std::cout << std::endl;
  }
  std::cout << std::endl;
  }
*/



void GraphComponent::computeObjectiveValues () { datatree->computeObjectiveValues(); }
void GraphComponent::normalizeObjectiveValues () { datatree->normalizeObjectiveValues(); }
void GraphComponent::printObjectiveValues () { datatree->printObjectiveValues(); }

void GraphComponent::computeOptimalPartition (double parameter) { datatree->computeOptimalPartition(parameter); }
void GraphComponent::printOptimalPartition (double parameter) { datatree->printOptimalPartition(parameter); }
Partition *GraphComponent::getOptimalPartition (double parameter) { return datatree->getOptimalPartition(parameter); }

