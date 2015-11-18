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


/*!
 * \author Robin Lamarche-Perrin
 * \date 22/01/2015
 */

#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <math.h>
#include <cmath>
#include <iterator>
#include <sstream>

#include "uni_set.hpp"
#include "voter_graph.hpp"
#include "csv_tools.hpp"


VoterNode::VoterNode (int i, double w, double c)
{
	id = i;
	weight = w;
	contrarian = c;
	
	inEdgeWeight = 0;
	inEdgeNumber = 0;
	inEdgeSet = new std::set<VoterEdge*>();

	outEdgeWeight = 0;
	outEdgeNumber = 0;	
	outEdgeSet = new std::set<VoterEdge*>();
}


VoterNode::~VoterNode ()
{
	delete inEdgeSet;
	delete outEdgeSet;
}


VoterEdge::VoterEdge (VoterNode *n1, VoterNode *n2, double w)
{
	node1 = n1;
	node2 = n2;
	weight = w;
}


VoterEdge::~VoterEdge () {}


VoterGraph::VoterGraph (int update)
{
	updateProcess = update;
	complete = false;
	nodeNumber = 0;
	edgeNumber = 0;
	nodeWeight = 0;
	edgeWeight = 0;
	nodeMap = new std::map<int,VoterNode*>();
	nodeSet = new std::set<VoterNode*>();
	edgeSet = new std::set<VoterEdge*>();
	
	process = 0;
}


VoterGraph::~VoterGraph ()
{
	delete nodeMap;
	delete nodeSet;
	delete edgeSet;
}


CompleteVoterGraph::CompleteVoterGraph (int size, int update, double contrarian) : VoterGraph (update)
{
	complete = true;
	for (int i = 0; i < size; i++) { addNode(1,contrarian); }

	for (std::set<VoterNode*>::iterator it1 = nodeSet->begin(); it1 != nodeSet->end(); ++it1)
	{
		VoterNode *n1 = *it1;
		for (std::set<VoterNode*>::iterator it2 = nodeSet->begin(); it2 != nodeSet->end(); ++it2)
		{
			VoterNode *n2 = *it2;
			if (n1 != n2) { addEdge(n1,n2); }
		}
	}
}


ChainVoterGraph::ChainVoterGraph (int s, double c, bool r, int update) : VoterGraph (update)
{
	size = s;
	contrarian = c;
	ring = r;
	nodeArray = new VoterNode* [size];

	for (int i = 0; i < size; i++) { nodeArray[i] = addNode(1,contrarian); }
	for (int i = 1; i < size; i++) { addEdge(nodeArray[i-1],nodeArray[i]); addEdge(nodeArray[i],nodeArray[i-1]); }
	if (ring) { addEdge(nodeArray[0],nodeArray[size-1]); addEdge(nodeArray[size-1],nodeArray[0]); }
}


ChainVoterGraph::~ChainVoterGraph ()
{
	delete [] nodeArray;
}


TwoCommunitiesVoterGraph::TwoCommunitiesVoterGraph (int s1, int s2,
													double intraR1, double intraR2, double interR1, double interR2,
													double c1, double c2, int update) : VoterGraph (update)
{
	size1 = s1;
	size2 = s2;

	intraRate1 = intraR1;
	intraRate2 = intraR2;
	interRate1 = interR1;
	interRate2 = interR2;
	
	contrarian1 = c1;
	contrarian2 = c2;
	
	community1 = new std::set<VoterNode*>();
	community2 = new std::set<VoterNode*>();

	for (int i = 0; i < size1; i++) { community1->insert(addNode(1,c1)); }
	for (int i = 0; i < size2; i++) { community2->insert(addNode(1,c2)); }

	for (std::set<VoterNode*>::iterator it1 = community1->begin(); it1 != community1->end(); ++it1)
		for (std::set<VoterNode*>::iterator it2 = community1->begin(); it2 != community1->end(); ++it2)
			if (*it1 != *it2) { addEdge(*it1,*it2,intraR1); }

	for (std::set<VoterNode*>::iterator it1 = community2->begin(); it1 != community2->end(); ++it1)
		for (std::set<VoterNode*>::iterator it2 = community2->begin(); it2 != community2->end(); ++it2)
			if (*it1 != *it2) { addEdge(*it1,*it2,intraR2); }

	for (std::set<VoterNode*>::iterator it1 = community1->begin(); it1 != community1->end(); ++it1)
	{
		for (std::set<VoterNode*>::iterator it2 = community2->begin(); it2 != community2->end(); ++it2)
		{
			addEdge(*it1,*it2,interR1);
			addEdge(*it2,*it1,interR2);
		}
	}
}


TwoCommunitiesVoterGraph::~TwoCommunitiesVoterGraph ()
{
	delete community1;
	delete community2;
}



VoterNode *VoterGraph::addNode (double weight, double contrarian)
{
	if (weight > 0) {		
		VoterNode *node = new VoterNode(nodeNumber,weight,contrarian);
		nodeSet->insert(node);
		nodeMap->insert(std::make_pair(node->id,node));
		nodeNumber++;
		nodeWeight += weight;

		return node;
	}
	
	else { return 0; }
}


VoterEdge *VoterGraph::addEdge (VoterNode *n1, VoterNode *n2, double weight)
{
	if (weight > 0) {
		edgeNumber++;
		edgeWeight += weight;
		
		VoterEdge *e = new VoterEdge(n1,n2,weight);
		edgeSet->insert(e);
		
		n1->outEdgeSet->insert(e);
		n1->outEdgeWeight += weight;	
		n1->outEdgeNumber++;	

		n2->inEdgeSet->insert(e);
		n2->inEdgeWeight += weight;	
		n2->inEdgeNumber++;	
		
		return e;
	}
	
	else { return 0; }
}


void VoterGraph::fillEdges ()
{
	for (std::set<VoterNode*>::iterator it1 = nodeSet->begin(); it1 != nodeSet->end(); ++it1)
	{
		VoterNode *n1 = *it1;
	    
		for (std::set<VoterNode*>::iterator it2 = it1; it2 != nodeSet->end(); ++it2)
		{
			VoterNode *n2 = *it2;
			if (n1 != n2)
			{
				addEdge(n1,n2);
				addEdge(n2,n1);
			}
		}
	}
}


VoterNode *VoterGraph::getRandomNode ()
{
	double r = ((double) rand() / (RAND_MAX));
	for (std::set<VoterNode*>::iterator it = nodeSet->begin(); it != nodeSet->end(); ++it)
	{
		VoterNode *n = *it;
		double p = n->weight/nodeWeight;
		if (r < p) { return n; }
		r -= p;
	}
	return 0;
}


VoterNode *VoterGraph::getUniformRandomNode ()
{
	return nodeMap->at(rand() % nodeNumber);
}


VoterEdge *VoterGraph::getRandomEdge (VoterNode *node)
{
	double r = ((double) rand() / (RAND_MAX));
	for (std::set<VoterEdge*>::iterator it = node->outEdgeSet->begin(); it != node->outEdgeSet->end(); ++it)
	{
		VoterEdge *e = *it;
		double p = e->weight/node->outEdgeWeight;
		if (r < p) { return e; }
		r -= p;
	}
	return 0;
}


void VoterGraph::print ()
{
    for (std::set<VoterNode*>::iterator it1 = nodeSet->begin(); it1 != nodeSet->end(); ++it1)
    {
		VoterNode *n1 = *it1;
	    
		std::cout << "node " << n1->id << " connected to nodes ";
		for (std::set<VoterEdge*>::iterator it2 = n1->outEdgeSet->begin(); it2 != n1->outEdgeSet->end(); ++it2)
		{
			VoterNode *n2 = (*it2)->node2;
			std::cout << n2->id << ", ";
		}
		std::cout << std::endl;
    }
}


MarkovProcess *VoterGraph::getMarkovProcess()
{
    unsigned long int size = 1 << nodeNumber;

    process = new MarkovProcess(size);

    for (unsigned long int i = 0; i < size; i++)
		process->distribution[i] = 1./size;

    for (unsigned long int i = 0; i < size; i++)
		for (unsigned long int j = 0; j < size; j++)
			process->transition[j*size+i] = 0.;

    if (updateProcess == UPDATE_EDGES)
    {
		for (unsigned long int i = 0; i < size; i++)
		{
			if (edgeNumber == 0 || edgeWeight == 0.) { process->transition[i*size+i] = 1.; std::cout << "NO EDGE!\n"; }
			else {
				for (std::set<VoterEdge*>::iterator it = edgeSet->begin(); it != edgeSet->end(); ++it)
				{
					VoterEdge *e = *it;
					int n1 = e->node1->id;
					int n2 = e->node2->id;
		
					bool s1 = (i >> n1) % 2;
					bool s2 = (i >> n2) % 2;
					
					unsigned long int j;
					if (s1 && !s2) { j = i + (1 << n2); }
					else if (!s1 && s2) { j = i - (1 << n2); }
					else { j = i; }
					
					process->transition[j*size+i] += e->weight/edgeWeight * (1 - e->node2->contrarian);


					if (s1 && s2) { j = i - (1 << n2); }
					else if (!s1 && !s2) { j = i + (1 << n2); }
					else { j = i; }
					
					process->transition[j*size+i] += e->weight/edgeWeight * e->node2->contrarian;
				}
			}
		}
    }
	
    if (updateProcess == UPDATE_NODES)
    {
		for (unsigned long int i = 0; i < size; i++)
		{
			for (std::set<VoterNode*>::iterator it1 = nodeSet->begin(); it1 != nodeSet->end(); ++it1)
			{
				VoterNode *node2 = *it1;
				int n2 = node2->id;
				bool s2 = (i >> n2) % 2;

				unsigned long int j;
				if (s2) { j = i - (1 << n2); } else { j = i + (1 << n2); }
				process->transition[j*size+i] += node2->weight/nodeWeight * node2->contrarian;
				
				for (std::set<VoterEdge*>::iterator it2 = node2->inEdgeSet->begin(); it2 != node2->inEdgeSet->end(); ++it2)
				{
					VoterEdge *e = *it2;
					int n1 = e->node1->id;

					bool s1 = (i >> n1) % 2;
					
					if (s1 && !s2) { j = i + (1 << n2); }
					else if (!s1 && s2) { j = i - (1 << n2); }
					else { j = i; }
						
					process->transition[j*size+i] += node2->weight/nodeWeight * e->weight/node2->inEdgeWeight * (1 - node2->contrarian);
				}
			}
		}
    }

    return process;
}



Partition *VoterGraph::getMarkovPartition (VoterProbe *probe, VoterMetric metric)
{
    unsigned long int size = 1 << nodeNumber;
    Partition *partition = new Partition();
	
    switch (metric)
    {
    case MACRO_STATE :
    {
		std::map<int,Part*> partMap;
		for (int nb = 0; nb <= probe->nodeNumber; nb++)
		{
			Part *part = new Part();
			partition->addPart(part);
			partMap[nb] = part;
		}
				
		for (unsigned long int i = 0; i < size; i++)
		{
			int nb = 0;
			for (std::set<VoterNode*>::iterator it = probe->nodeSet->begin(); it != probe->nodeSet->end(); ++it)
				nb += (i >> (*it)->id) % 2;
			partMap[nb]->addIndividual(i,false,nb);
		}
		break;
    }

    /*
	  case MAJORITY : case MAJ_1PC : case MAJ_2PC : case MAJ_3PC : case MAJ_4PC : case MAJ_5PC : case MAJ_6PC : case MAJ_7PC : 
	  case MAJ_8PC : case MAJ_9PC : case MAJ_10PC : case MAJ_20PC : case MAJ_30PC : case MAJ_40PC : case MAJ_50PC :
	  case MAJ_60PC : case MAJ_70PC : case MAJ_80PC : case MAJ_90PC :
	  {
	  Part *part0 = new Part();
	  Part *part1 = new Part();
	  Part *partNA = new Part();

	  partition->addPart(part0);
	  partition->addPart(part1);
	  partition->addPart(partNA);

	  double pc = 0;
	  switch (metric)
	  {
	  case MAJORITY : pc = 0; break;
	  case MAJ_1PC : pc = 0.01; break;
	  case MAJ_2PC : pc = 0.02; break;
	  case MAJ_3PC : pc = 0.03; break;
	  case MAJ_4PC : pc = 0.04; break;
	  case MAJ_5PC : pc = 0.05; break;
	  case MAJ_6PC : pc = 0.06; break;
	  case MAJ_7PC : pc = 0.07; break;
	  case MAJ_8PC : pc = 0.08; break;
	  case MAJ_9PC : pc = 0.09; break;
	  case MAJ_10PC : pc = 0.1; break;
	  case MAJ_20PC : pc = 0.2; break;
	  case MAJ_30PC : pc = 0.3; break;
	  case MAJ_40PC : pc = 0.4; break;
	  case MAJ_50PC : pc = 0.5; break;
	  case MAJ_60PC : pc = 0.6; break;
	  case MAJ_70PC : pc = 0.7; break;
	  case MAJ_80PC : pc = 0.8; break;
	  case MAJ_90PC : pc = 0.9; break;
	  default : break;
	  }
				
	  for (unsigned long int i = 0; i < size; i++)
	  {
	  int nb = 0;
	  for (std::set<VoterNode*>::iterator it = probe->nodeSet->begin(); it != probe->nodeSet->end(); ++it)
	  nb += (i >> (*it)->id) % 2;

	  if (2*nb - size < pc * probe->nodeNumber) { part0->addIndividual(i); }
	  else if (2*nb - size > pc * probe->nodeNumber) { part1->addIndividual(i); }
	  else { partNA->addIndividual(i); }
	  }
	  break;
	  }
		
	
	  case MAJ_2BINS : case MAJ_3BINS : case MAJ_4BINS : case MAJ_6BINS : case MAJ_8BINS : case MAJ_12BINS : case MAJ_24BINS :
	  {
	  int partNumber;
	  switch (metric)
	  {
	  case MAJ_2BINS : partNumber = std::min(2,probe->nodeNumber+1); break;
	  case MAJ_3BINS : partNumber = std::min(3,probe->nodeNumber+1); break;
	  case MAJ_4BINS : partNumber = std::min(4,probe->nodeNumber+1); break;
	  case MAJ_6BINS : partNumber = std::min(6,probe->nodeNumber+1); break;
	  case MAJ_8BINS : partNumber = std::min(8,probe->nodeNumber+1); break;
	  case MAJ_12BINS : partNumber = std::min(12,probe->nodeNumber+1); break;
	  case MAJ_24BINS : partNumber = std::min(24,probe->nodeNumber+1); break;
	  default : std::cout << "ERROR: unimplemented majority measurement!" << std::endl; return 0;
	  }

	  if (probe->nodeNumber+1 % partNumber != 0) { std::cout << "ERROR: not a proper bin size for majority measurement!" << std::endl; return 0; }
	
	  int partVector [partNumber];
	  for (int i = 0; i < partNumber; i++) { partVector[i] = 0; }
		
	  int nb1 = 0;
	  double c = pow(2.,-probe->nodeNumber);
	  for (int i = 0; i < partNumber/2; i++)
	  {
	  int nb2 = nb1;
	  double add = nChoosekProb(probe->nodeNumber,nb2+1);
	  while (c + add < ((double)i+1)/partNumber) { c += add; nb2++; add = nChoosekProb(probe->nodeNumber,nb2+1); }
			
	  partVector[2*i] = nb1;
	  partVector[2*i+1] = nb2;
	  partVector[2*(partNumber-1-i)] = probe->nodeNumber-nb2;
	  partVector[2*(partNumber-1-i)+1] = probe->nodeNumber-nb1;
		
	  nb1 = nb2+1;
	  c += nChoosekProb(probe->nodeNumber,nb1);
	  }
			
	  if (partNumber % 2)
	  {
	  int i = partNumber/2;
	  partVector[2*(partNumber-1-i)] = partVector[2*(partNumber-1-i)-1]+1;
	  partVector[2*(partNumber-1-i)+1] = partVector[2*(partNumber-1-i)+2]-1;
	  }
		
//			for (int i = 0; i < partNumber; i++)
//				std::cout << partVector[2*i] << " - " << partVector[2*i+1] << "\t" << std::endl;

std::map<int,Part*> partMap;
for (int n = 0; n < partNumber; n++)
{
Part *part = new Part();
partition->addPart(part);
partMap[n] = part;
}
				
Part *partNA = new Part();
partition->addPart(partNA);

for (unsigned long int i = 0; i < size; i++)
{
int nb = 0;
for (std::set<VoterNode*>::iterator it = probe->nodeSet->begin(); it != probe->nodeSet->end(); ++it)
nb += (i >> (*it)->id) % 2;

bool found = false;
for (int n = 0; !found && n < partNumber; n++)
if (nb >= partVector[2*n] && nb <= partVector[2*n+1] ) { partMap[n]->addIndividual(i); found = true; }
if (!found) { partNA->addIndividual(i); }
}
break;
}
    */	
		
    case ACTIVE_EDGES :
    {
		if (probe->nodeNumber == 0)
		{
			Part *part = new Part();
			partition->addPart(part);
			for (unsigned long int i = 0; i < size; i++) { part->addIndividual(i); }
		}
			
		else {
			std::map<int,Part*> partMap;
		
			for (unsigned long int i = 0; i < size; i++)
			{
				int nb = 0;
				for (std::set<VoterNode*>::iterator it1 = probe->nodeSet->begin(); it1 != probe->nodeSet->end(); ++it1)
				{
					VoterNode *n2 = *it1;
					int s2 = (i >> n2->id) % 2;
						
					for (std::set<VoterEdge*>::iterator it2 = n2->inEdgeSet->begin(); it2 != n2->inEdgeSet->end(); ++it2)
					{
						VoterEdge *e = *it2;
						VoterNode *n1 = e->node1;
						int s1 = (i >> n1->id) % 2;
							
						if (s1 != s2) { nb += e->weight; }
					}
				}
				
				if (partMap.find(nb) == partMap.end())
				{
					Part *part = new Part();
					partition->addPart(part);
					partMap[nb] = part;
				}
								
				partMap[nb]->addIndividual(i);
			}
		}
		break;
    }
			
    default : std::cout << "ERROR: unimplemented metric!" << std::endl;
    }
	
    return partition;
}


Partition *VoterGraph::getMarkovPartition (VoterMeasurement *m)
{
    if (m->probeNumber == 0)
    {
		Partition *partition = new Partition();
		Part *part = new Part();
		partition->addPart(part);

		unsigned long int size = 1 << nodeNumber;
		for (unsigned long int i = 0; i < size; i++) { part->addIndividual(i,false,0); }

		return partition;
    }
	
    Partition *partition1 = 0;
    for (int num = 0; num < m->probeNumber; num++)
    {
		VoterProbe *p = m->probeMap->at(num);
		VoterMetric metric = m->metricMap->at(num);
		
		if (partition1 == 0) { partition1 = getMarkovPartition(p,metric); }
		else {
			Partition *partition2 = getMarkovPartition(p,metric);
			Partition *partition = new Partition();
			
			for (std::list<Part*>::iterator it1 = partition1->parts->begin(); it1 != partition1->parts->end(); ++it1)
			{
				Part *p1 = *it1;

				for (std::list<Part*>::iterator it2 = partition2->parts->begin(); it2 != partition2->parts->end(); ++it2)
				{
					Part *p2 = *it2;

					Part *newPart = new Part();
					for (std::list<int>::iterator itp1 = p1->individuals->begin(); itp1 != p1->individuals->end(); ++itp1)
						for (std::list<int>::iterator itp2 = p2->individuals->begin(); itp2 != p2->individuals->end(); ++itp2)
							if (*itp1 == *itp2) { newPart->addIndividual(*itp1); }
					
					if (newPart->individuals->size() > 0) { partition->addPart(newPart); }
					else { delete newPart; }
				}
			}
			
			delete partition1;
			partition1 = partition;
		}
    }
	
    return partition1;
}



MarkovProcess *TwoCommunitiesVoterGraph::getCompactMarkovProcess ()
{
    unsigned long int size = 2*size1*(size2+1);

    process = new MarkovProcess(size);

    for (unsigned long int i = 0; i < size; i++)
    {
		int a = i % 2;
		int n1 = (i-a)/2 % size1;
		int n2 = (i-a-2*n1)/(2*size1);
		
		process->distribution[i] = ((double) nChoosek(size1-1,n1) * nChoosek(size2,n2)) * pow(2.,-(size1+size2));
    }

    double sumP = 0;
    for (unsigned long int i = 0; i < size; i++) { sumP += process->distribution[i]; }
    if (std::abs(sumP - 1) > 1e-10) { std::cout << "ERROR: probabilities do not sum to 1!" << std::endl; }
	
    for (unsigned long int i = 0; i < size; i++)
		for (unsigned long int j = 0; j < size; j++)
			process->transition[j*size+i] = 0.;

    if (updateProcess == UPDATE_EDGES)
    {
		for (unsigned long int i = 0; i < size; i++)
		{
			int a = i % 2;
			int n1 = (i-a)/2 % size1;
			int n2 = (i-a-2*n1)/(2*size1);

			sumP = 0;
			double p;
			unsigned long int j;
			j = a + n1 * 2 + n2 * 2 * size1;
			
			// a -> n1
			if (n1 > 0)
			{
				if (a) { j = a + n1 * 2 + n2 * 2 * size1; }
				else { j = a + (n1-1) * 2 + n2 * 2 * size1; }
				p = intraRate1 * n1 * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				if (a) { j = a + (n1-1) * 2 + n2 * 2 * size1; }
				else { j = a + n1 * 2 + n2 * 2 * size1; }
				p = intraRate1 * n1 * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// a -> !n1
			if (n1 < size1-1)
			{
				if (a) { j = a + (n1+1) * 2 + n2 * 2 * size1; }
				else { j = a + n1 * 2 + n2 * 2 * size1; }
				p = intraRate1 * (size1-1 - n1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				if (a) { j = a + n1 * 2 + n2 * 2 * size1; }
				else { j = a + (n1+1) * 2 + n2 * 2 * size1; }
				p = intraRate1 * (size1-1 - n1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// a -> n2
			if (n2 > 0)
			{
				if (a) { j = a + n1 * 2 + n2 * 2 * size1; }
				else { j = a + n1 * 2 + (n2-1) * 2 * size1; }
				p = interRate1 * n2 * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				if (a) { j = a + n1 * 2 + (n2-1) * 2 * size1; }
				else { j = a + n1 * 2 + n2 * 2 * size1; }
				p = interRate1 * n2 * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// a -> !n2
			if (n2 < size2)
			{
				if (a) { j = a + n1 * 2 + (n2+1) * 2 * size1; }
				else { j = a + n1 * 2 + n2 * 2 * size1; }
				p = interRate1 * (size2 - n2) * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				if (a) { j = a + n1 * 2 + n2 * 2 * size1; }
				else { j = a + n1 * 2 + (n2+1) * 2 * size1; }
				p = interRate1 * (size2 - n2) * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n1 -> a
			if (n1 > 0)
			{
				j = 1 + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * n1 * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = 0 + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * n1 * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// !n1 -> a
			if (n1 < size1-1)
			{
				j = 0 + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * (size1-1 - n1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = 1 + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * (size1-1 - n1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// n2 -> a
			if (n2 > 0)
			{
				j = 1 + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * n2 * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = 0 + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * n2 * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// !n2 -> a
			if (n2 < size2)
			{
				j = 0 + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * (size2 - n2) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = 1 + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * (size2 - n2) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// n1 -> !n1
			if (n1 > 0 && n1 < size1-1)
			{
				j = a + (n1+1) * 2 + n2 * 2 * size1;
				p = intraRate1 * n1 * (size1-1 - n1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * n1 * (size1-1 - n1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// !n1 -> n1
			if (n1 > 0 && n1 < size1-1)
			{
				j = a + (n1-1) * 2 + n2 * 2 * size1;
				p = intraRate1 * (size1-1 - n1) * n1 * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * (size1-1 - n1) * n1 * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n2 -> !n2
			if (n2 > 0 && n2 < size2)
			{
				j = a + n1 * 2 + (n2+1) * 2 * size1;
				p = intraRate2 * n2 * (size2 - n2) * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate2 * n2 * (size2 - n2) * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// !n2 -> n2
			if (n2 > 0 && n2 < size2)
			{
				j = a + n1 * 2 + (n2-1) * 2 * size1;
				p = intraRate2 * (size2 - n2) * n2 * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate2 * (size2 - n2) * n2 * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n1 -> !n2
			if (n1 > 0 && n2 < size2)
			{
				j = a + n1 * 2 + (n2+1) * 2 * size1;
				p = interRate1 * n1 * (size2 - n2) * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate1 * n1 * (size2 - n2) * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// !n1 -> n2
			if (n1 < size1-1 && n2 > 0)
			{
				j = a + n1 * 2 + (n2-1) * 2 * size1;
				p = interRate1 * (size1-1 - n1) * n2 * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate1 * (size1-1 - n1) * n2 * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n2 -> !n1
			if (n2 > 0 && n1 < size1-1)
			{
				j = a + (n1+1) * 2 + n2 * 2 * size1;
				p = interRate2 * n2 * (size1-1 - n1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * n2 * (size1-1 - n1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// !n2 -> n1
			if (n2 < size2 && n1 > 0)
			{
				j = a + (n1-1) * 2 + n2 * 2 * size1;
				p = interRate2 * (size2 - n2) * n1 * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * (size2 - n2) * n1 * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n1 -> n1
			if (n1 > 1)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * n1 * (n1 - 1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + (n1-1) * 2 + n2 * 2 * size1;
				p = intraRate1 * n1 * (n1 - 1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}

			// !n1 -> !n1
			if (n1 < size1-2)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate1 * (size1-1 - n1) * (size1-1 - n1 - 1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + (n1+1) * 2 + n2 * 2 * size1;
				p = intraRate1 * (size1-1 - n1) * (size1-1 - n1 - 1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n2 -> n2
			if (n2 > 1)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate2 * n2 * (n2 - 1) * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + (n2-1) * 2 * size1;
				p = intraRate2 * n2 * (n2 - 1) * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// !n2 -> !n2
			if (n2 < size2-1)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = intraRate2 * (size2 - n2) * (size2 - n2 - 1) * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + (n2+1) * 2 * size1;
				p = intraRate2 * (size2 - n2) * (size2 - n2 - 1) * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n1 -> n2
			if (n1 > 0 && n2 > 0)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate1 * n1 * n2 * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + (n2-1) * 2 * size1;
				p = interRate1 * n1 * n2 * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// !n1 -> !n2
			if (n1 < size1-1 && n2 < size2)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate1 * (size1-1 - n1) * (size2 - n2) * (1 - contrarian2);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + n1 * 2 + (n2+1) * 2 * size1;
				p = interRate1 * (size1-1 - n1) * (size2 - n2) * contrarian2;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// n2 -> n1
			if (n2 > 0 && n1 > 0)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * n2 * n1 * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + (n1-1) * 2 + n2 * 2 * size1;
				p = interRate2 * n2 * n1 * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
			
			// !n2 -> !n1
			if (n2 < size2 && n1 < size1-1)
			{
				j = a + n1 * 2 + n2 * 2 * size1;
				p = interRate2 * (size2 - n2) * (size1-1 - n1) * (1 - contrarian1);
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;

				j = a + (n1+1) * 2 + n2 * 2 * size1;
				p = interRate2 * (size2 - n2) * (size1-1 - n1) * contrarian1;
				process->transition[j*size+i] += p/edgeWeight;
				sumP += p/edgeWeight;
			}
						
			if (std::abs(sumP - 1) > 1e-10) { std::cout << "ERROR: probabilities do not sum to 1!" << std::endl; }
		}
    }

    return process;
}


Partition *TwoCommunitiesVoterGraph::getCompactMarkovPartition (VoterProbe *probe, VoterMetric metric)
{
    unsigned long int size = 2 * size1 * (size2 + 1);
    Partition *partition = new Partition();
	
    switch (metric)
    {
    case MACRO_STATE :
    {
		std::map<int,Part*> partMap;
		for (int nb = 0; nb <= probe->nodeNumber; nb++)
		{
			Part *part = new Part();
			partition->addPart(part);
			partMap[nb] = part;
		}
	
		if (probe->nodeNumber == 1 && *probe->nodeSet->begin() == *community1->begin())
		{
			for (unsigned long int i = 0; i < size; i++)
			{
				int a = i % 2;
				partMap[a]->addIndividual(i,false,a);
			}
		}
	
		else if (*probe->nodeSet == *community1)
		{
			for (unsigned long int i = 0; i < size; i++)
			{
				int a = i % 2;
				int n1 = (i-a)/2 % size1;
				partMap[a+n1]->addIndividual(i,false,a+n1);
			}
		}
	
		else if (*probe->nodeSet == *community2)
		{
			for (unsigned long int i = 0; i < size; i++)
			{
				int a = i % 2;
				int n1 = (i-a)/2 % size1;
				int n2 = (i-a-2*n1)/(2*size1);
				partMap[n2]->addIndividual(i,false,n2);
			}
		}
	
		else if (*probe->nodeSet == *nodeSet)
		{
			for (unsigned long int i = 0; i < size; i++)
			{
				int a = i % 2;
				int n1 = (i-a)/2 % size1;
				int n2 = (i-a-2*n1)/(2*size1);
				partMap[a+n1+n2]->addIndividual(i,false,a+n1+n2);
			}
		}
		
		else { std::cout << "ERROR: incompatible probe!" << std::endl; probe->print(true); }
		break;
    }
		
    case MAJORITY : case MAJ_1PC : case MAJ_2PC : case MAJ_3PC : case MAJ_4PC : case MAJ_5PC : case MAJ_6PC : case MAJ_7PC : 
    case MAJ_8PC : case MAJ_9PC : case MAJ_10PC : case MAJ_20PC : case MAJ_30PC : case MAJ_40PC : case MAJ_50PC :
    case MAJ_60PC : case MAJ_70PC : case MAJ_80PC : case MAJ_90PC :
    {
		Part *part0 = new Part();
		Part *part1 = new Part();
		Part *partNA = new Part();

		partition->addPart(part0);
		partition->addPart(part1);
		partition->addPart(partNA);

		double pc = 0;
		switch (metric)
		{
		case MAJORITY : pc = 0; break;
		case MAJ_1PC : pc = 0.01; break;
		case MAJ_2PC : pc = 0.02; break;
		case MAJ_3PC : pc = 0.03; break;
		case MAJ_4PC : pc = 0.04; break;
		case MAJ_5PC : pc = 0.05; break;
		case MAJ_6PC : pc = 0.06; break;
		case MAJ_7PC : pc = 0.07; break;
		case MAJ_8PC : pc = 0.08; break;
		case MAJ_9PC : pc = 0.09; break;
		case MAJ_10PC : pc = 0.1; break;
		case MAJ_20PC : pc = 0.2; break;
		case MAJ_30PC : pc = 0.3; break;
		case MAJ_40PC : pc = 0.4; break;
		case MAJ_50PC : pc = 0.5; break;
		case MAJ_60PC : pc = 0.6; break;
		case MAJ_70PC : pc = 0.7; break;
		case MAJ_80PC : pc = 0.8; break;
		case MAJ_90PC : pc = 0.9; break;
		default : break;
		}
			
		for (unsigned long int i = 0; i < size; i++)
		{
			int a = i % 2;
			int n1 = (i-a)/2 % size1;
			int n2 = (i-a-2*n1)/(2*size1);

			int nb = -1; int s = -1;
			if (probe->nodeNumber == 1 && *probe->nodeSet->begin() == *community1->begin()) { nb = a; s = 1; }
			else if (*probe->nodeSet == *community1) { nb = a+n1; s = size1; }
			else if (*probe->nodeSet == *community2) { nb = n2; s = size2; }
			else if (*probe->nodeSet == *nodeSet) { nb = a+n1+n2; s = size1+size2; }
			else { std::cout << "ERROR: incompatible probe!" << std::endl; probe->print(true); }
				
			if (nb >= 0)
			{
				if (2*nb - s < pc * probe->nodeNumber) { part0->addIndividual(i); }
				else if (2*nb - s > pc * probe->nodeNumber) { part1->addIndividual(i); }
				else { partNA->addIndividual(i); }
			}	
		}
		break;
    }

    case MAJ_2B : case MAJ_3B : case MAJ_4B : case MAJ_6B : case MAJ_8B : case MAJ_10B : case MAJ_12B : case MAJ_20B : case MAJ_40B :
    {
		int partNumber;
		switch (metric)
		{
		case MAJ_2B : partNumber = 2; break;
		case MAJ_3B : partNumber = 3; break;
		case MAJ_4B : partNumber = 4; break;
		case MAJ_6B : partNumber = 6; break;
		case MAJ_8B : partNumber = 8; break;
		case MAJ_10B : partNumber = 10; break;
		case MAJ_12B : partNumber = 12; break;
		case MAJ_20B : partNumber = 20; break;
		case MAJ_40B : partNumber = 40; break;
		default : break;
		}
	
		partNumber = std::min(partNumber,probe->nodeNumber+1);
		if ((probe->nodeNumber+1) % partNumber != 0) { std::cout << "ERROR: not a proper bin size for majority measurement!" << std::endl; return 0; }
		int l = (probe->nodeNumber+1) / partNumber;
	
		std::map<int,Part*> partMap;
		for (int n = 0; n < partNumber; n++)
		{
			Part *part = new Part();
			partition->addPart(part);
			partMap[n] = part;
		}
				
		Part *partNA = new Part();
		partition->addPart(partNA);

		for (unsigned long int i = 0; i < size; i++)
		{
			int a = i % 2;
			int n1 = (i-a)/2 % size1;
			int n2 = (i-a-2*n1)/(2*size1);

			int nb = -1;
			if (probe->nodeNumber == 1 && *probe->nodeSet->begin() == *community1->begin()) { nb = a; }
			else if (*probe->nodeSet == *community1) { nb = a+n1; }
			else if (*probe->nodeSet == *community2) { nb = n2; }
			else if (*probe->nodeSet == *nodeSet) { nb = a+n1+n2; }
			else { std::cout << "ERROR: incompatible probe!" << std::endl; probe->print(true); return 0; }
				
			int n = floor(nb/l);
			partMap[n]->addIndividual(i);

			/*
			  int lim = ((probe->nodeNumber+1) - ((probe->nodeNumber+1) % 2))/2;
			  if (nb < lim)
			  {
			  int n = (nb - (nb % (2*lim/partNumber))) / (2*lim/partNumber);
			  partMap[n]->addIndividual(i);
			  }
	    
			  else if (probe->nodeNumber - nb < lim)
			  {
			  int n = ((probe->nodeNumber - nb) - ((probe->nodeNumber - nb) % (2*lim/partNumber))) / (2*lim/partNumber);
			  partMap[partNumber-n-1]->addIndividual(i);						
			  }
	    
			  else
			  {
			  partNA->addIndividual(i);
			  }
			*/
		}
		break;
    }
		
    default : std::cout << "ERROR: unimplemented metric!" << std::endl;			
    }
		
    return partition;
}


Partition *TwoCommunitiesVoterGraph::getCompactMarkovPartition (VoterMeasurement *m)
{
    if (m->probeNumber == 0)
    {
		Partition *partition = new Partition();
		Part *part = new Part();
		partition->addPart(part);

		unsigned long int size = 2 * size1 * (size2 + 1);
		for (unsigned long int i = 0; i < size; i++) { part->addIndividual(i,false,0); }

		return partition;
    }
	
    Partition *partition1 = 0;
    for (int num = 0; num < m->probeNumber; num++)
    {
		VoterProbe *p = m->probeMap->at(num);
		VoterMetric metric = m->metricMap->at(num);
		
		if (partition1 == 0) { partition1 = getCompactMarkovPartition(p,metric); }
		else {
			Partition *partition2 = getCompactMarkovPartition(p,metric);
			Partition *partition = new Partition();
			
			for (std::list<Part*>::iterator it1 = partition1->parts->begin(); it1 != partition1->parts->end(); ++it1)
			{
				Part *p1 = *it1;

				for (std::list<Part*>::iterator it2 = partition2->parts->begin(); it2 != partition2->parts->end(); ++it2)
				{
					Part *p2 = *it2;

					Part *newPart = new Part();
					for (std::list<int>::iterator itp1 = p1->individuals->begin(); itp1 != p1->individuals->end(); ++itp1)
						for (std::list<int>::iterator itp2 = p2->individuals->begin(); itp2 != p2->individuals->end(); ++itp2)
							if (*itp1 == *itp2) { newPart->addIndividual(*itp1); }
					
					if (newPart->individuals->size() > 0) { partition->addPart(newPart); }
					else { delete newPart; }
				}
			}
			
			delete partition1;
			partition1 = partition;
		}
    }
	
    return partition1;
}


VoterProbe::VoterProbe (VoterGraph *g)
{
    graph = g;
    nodeNumber = 0;
    nodeSet = new std::set<VoterNode*>();
}


VoterProbe::~VoterProbe ()
{
    delete nodeSet;
}

void VoterProbe::setNodeSet (std::set<VoterNode*> *set)
{
    delete nodeSet;
    nodeSet = set;
    nodeNumber = set->size();
}


void VoterProbe::addNode (VoterNode *n)
{
    nodeNumber++;
    nodeSet->insert(n);
}


void VoterProbe::addNodes (unsigned long int i)
{
    int id = 0;
    while (i != 0)
    {
		if (i % 2)
		{
			nodeNumber++;
			nodeSet->insert(graph->nodeMap->at(id));
		}
		i = i >> 1;
		id++;
    }
}


int VoterProbe::getCardinality (VoterMetric metric, int binning)
{
	switch (metric)
	{
	case MACRO_STATE :
		if (binning > 0) { return binning; }
		return nodeNumber+1;
		
	default :
		std::cout << "ERROR: this metric has not been implemented!" << std::endl;
		return -1;
	}	
}


int VoterProbe::getState (VoterState *state, VoterMetric metric, int binning)
{
	switch (metric)
	{
	case MACRO_STATE :
	{
		int probeState = 0;
		for (std::set<VoterNode*>::iterator it = nodeSet->begin(); it != nodeSet->end(); ++it)
		{
			VoterNode *n = *it;
			probeState += state->agentStates[n->id];
		}
		
		if (binning > 0)
		{
			double binningSize = (nodeNumber+1) / binning;
			return floor(probeState/binningSize);
		}
		
		return probeState;
	}
		
	default :
		std::cout << "ERROR: this metric has not been implemented!" << std::endl;
		return -1;
	}
}


void VoterProbe::print (bool endl)
{
    std::cout << "{";
    bool first = true;
    for (std::set<VoterNode*>::iterator it = nodeSet->begin(); it != nodeSet->end(); ++it)
    {
		VoterNode *n = *it;
		if (!first) { std::cout << ", "; }
		std::cout << n->id;
		first = false;
    }
    std::cout << "}";
    if (endl) { std::cout << std::endl; }
}


VoterMeasurement::VoterMeasurement (VoterGraph *g, std::string t)
{
    graph = g;
    type = t;
    partition = 0;
	
    probeNumber = 0;
    probeMap = new std::map<int,VoterProbe*>();
    metricMap = new std::map<int,VoterMetric>();
    binningMap = new std::map<int,int>();
}


VoterMeasurement::~VoterMeasurement ()
{
    if (partition != 0) { delete partition; }
    delete probeMap;
    delete metricMap;
	delete binningMap;
}


void VoterMeasurement::addProbe (VoterProbe *p, VoterMetric metric, int binning)
{
    probeMap->insert(std::make_pair(probeNumber,p));
    metricMap->insert(std::make_pair(probeNumber,metric));
	binningMap->insert(std::make_pair(probeNumber,binning));
    probeNumber++;
}


int VoterMeasurement::getCardinality ()
{
	int card = 1;
	for (int num = 0; num < probeNumber; num++) { card *= probeMap->at(num)->getCardinality(metricMap->at(num),binningMap->at(num)); }
	return card;
}


VoterMeasurementState *VoterMeasurement::getState (VoterState *state)
{
	VoterMeasurementState *measurementState = new VoterMeasurementState(this);

    for (int num = 0; num < probeNumber; num++)
	{
		VoterProbe *probe = probeMap->at(num);
		VoterMetric metric = metricMap->at(num);
		int binning = binningMap->at(num);
		measurementState->probeStates[num] = probe->getState(state,metric,binning);
	}
	return measurementState;
}


OrderedUniSet *VoterMeasurement::getOrderedUniSet ()
{
	if (probeNumber != 1) { std::cout << "ERROR: cannot get ordered uni-dimensional set from multi-probe measurement!"; return 0; }

	VoterProbe *probe = probeMap->at(0);
	OrderedUniSet *uniSet = new OrderedUniSet (probe->nodeNumber+1);
	uniSet->voterMeasurement = this;
	uniSet->voterProbe = probe;
	return uniSet;
}


std::vector<OrderedUniSet*> *VoterMeasurement::getOrderedUniSetVector ()
{
	std::vector<OrderedUniSet*> *setVector = new std::vector<OrderedUniSet*> ();

	for (int p = 0; p < probeNumber; p++)
	{
		VoterProbe *probe = probeMap->at(p);
		OrderedUniSet *uniSet = new OrderedUniSet (probe->nodeNumber+1);
		uniSet->voterMeasurement = this;
		uniSet->voterProbe = probe;
		setVector->push_back(uniSet);
	}
	return setVector;
}


void VoterMeasurement::print (bool endl)
{
    for (int num = 0; num < probeNumber; num++) { probeMap->at(num)->print(true); }
    if (endl) { std::cout << std::endl; }
}


MacroVoterMeasurement::MacroVoterMeasurement (VoterGraph *g, std::set<VoterMetric> metrics) : VoterMeasurement(g,"MACRO")
{
    for (std::set<VoterMetric>::iterator it = metrics.begin(); it != metrics.end(); ++it)
    {
		VoterMetric metric = *it;
		if (metric == MACRO_STATE) { type = type + "_MS"; }
		if (metric == ACTIVE_EDGES) { type = type + "_AE"; }
		
		VoterProbe *probe = new VoterProbe(g);
		for (std::set<VoterNode*>::iterator it = g->nodeSet->begin(); it != g->nodeSet->end(); ++it) { probe->addNode(*it); }
		addProbe(probe,metric);
    }
}


MicroVoterMeasurement::MicroVoterMeasurement (VoterGraph *g, std::set<VoterMetric> metrics) : VoterMeasurement(g,"MICRO")
{
    for (std::set<VoterMetric>::iterator it = metrics.begin(); it != metrics.end(); ++it)
    {
		VoterMetric metric = *it;
		if (metric == MACRO_STATE) { type = type + "_MS"; }
		if (metric == ACTIVE_EDGES) { type = type + "_AE"; }
		
		for (std::set<VoterNode*>::iterator it = g->nodeSet->begin(); it != g->nodeSet->end(); ++it)
		{
			VoterProbe *probe = new VoterProbe(g);
			probe->addNode(*it);
			addProbe(probe,metric);
		}
    }
}


EmptyVoterMeasurement::EmptyVoterMeasurement (VoterGraph *g) : VoterMeasurement(g,"EMPTY") {}


VoterState::VoterState (VoterGraph *VG)
{
    graph = VG;
    size = graph->nodeNumber;
    agentStates = new bool [size];
}


VoterState::VoterState (VoterState *state)
{
    graph = state->graph;
    size = state->size;
    agentStates = new bool [size];
    for (int i = 0; i < size; i++) { agentStates[i] = state->agentStates[i]; }
}


VoterState::~VoterState ()
{
    delete [] agentStates;
}


void VoterState::print ()
{
    for (int i = 0; i < size; i++) { std::cout << agentStates[i]; }    
}


void VoterState::setFromMicroUniform ()
{
    for (int i = 0; i < size; i++) { agentStates[i] = rand() % 2; }
}


void VoterState::setFromMacroUniform ()
{
//for (int i = 0; i < size; i++) { agentStates[i] = rand() % 2; }
}

VoterMeasurementState::VoterMeasurementState (VoterMeasurement *m)
{
	measurement = m;
	size = m->probeNumber;
	probeStates = new int [size];
}


/*
  VoterMeasurementState::VoterMeasurementState (const VoterMeasurementState &state){
  std::cout << "(";
  if (state.size > 0)
  {
  std::cout << state.probeStates[0];
  for (int i = 1; i < state.size; i++) { std::cout << "," << state.probeStates[i]; }
  }
  std::cout << ")" << measurement << "OK" << std::endl;

  measurement = state.measurement;
  size = state.size;
  probeStates = new int [size];
  for (int i = 0; i < size; i++) { probeStates[i] = state.probeStates[i]; };
  }


  VoterMeasurementState &VoterMeasurementState::operator= (const VoterMeasurementState &state)
  {
  measurement = state.measurement;
  size = state.size;
  for (int i = 0; i < size; i++) { probeStates[i] = state.probeStates[i]; };
  return *this;
  }
*/


VoterMeasurementState::~VoterMeasurementState ()
{
	delete [] probeStates;
}


/*
  bool operator< (const VoterMeasurementState& s1, const VoterMeasurementState& s2)
  {
  if (s1.size != s2.size) { std::cout << "ERROR: measurement states are not comparable!" << std::cout; return false; }
  for (int i = 0; i < s1.size; i++) {
  //std::cout << "COMPARE " << s1.probeStates[i] << " AND "<< s2.probeStates[i] << std::endl;
  if (s1.probeStates[i] < s2.probeStates[i])
  {
  //std::cout << "RETURN TRUE" << std::endl;
  return true;
  }
  }
	
  //std::cout << "RETURN FALSE" << std::endl;
  return false;
  }
*/


bool VoterMeasurementState::isEqual (VoterMeasurementState *state)
{
	if (this->size != state->size) { return false; }
	for (int i = 0; i < this->size; i++) {
		if (this->probeStates[i] != state->probeStates[i]) { return false; }
	}
	return true;
}


void VoterMeasurementState::init (int value)
{
	for (int i = 0; i < size; i++) { probeStates[i] = value; }
}


void VoterMeasurementState::print ()
{
	std::cout << "(";
	if (size > 0)
	{
		std::cout << probeStates[0];
		for (int i = 1; i < size; i++) { std::cout << "," << probeStates[i]; }
	}
	std::cout << ")";
}


VoterState *VoterState::getNextState()
{
    VoterState *state = new VoterState(this);

    VoterNode *n1;
	VoterNode *n2;
	if (graph->complete)
	{
		n1 = graph->getUniformRandomNode();
		n2 = graph->getUniformRandomNode();
		while (n1->id == n2->id) { n2 = graph->getUniformRandomNode(); }
	}

	else {
		n1 = graph->getRandomNode();
		n2 = graph->getRandomEdge(n1)->node2;
	}

    double r = ((double) rand() / (RAND_MAX));
	if (r < n1->contrarian) { state->agentStates[n2->id] = (state->agentStates[n1->id] + 1) % 2; }
    else { state->agentStates[n2->id] = state->agentStates[n1->id]; }

    return state;
}


VoterTrajectory::VoterTrajectory (VoterGraph *VG, int t, int l)
{
    graph = VG;
	time = t;
    length = l;

    states = new VoterState *[length];

    if (length > 0)
    {
		VoterState *currentState = new VoterState(graph);
		currentState->setFromMicroUniform();
		for (int t = 0; t < time; t++) { currentState = currentState->getNextState(); }

		states[0] = currentState;
		for (int l = 1; l < length; l++)
			states[l] = states[l-1]->getNextState();
    }
}


VoterTrajectory::~VoterTrajectory ()
{
    for (int l = 0; l < length; l++)
		delete states[l];
	delete [] states;
}


void VoterTrajectory::print ()
{
    std::cout << "START:  ";

    if (length > 0)
    {
		std::cout << "[0] ";
		states[0]->print();

		for (int l = 1; l < length; l++)
		{
			std::cout << "  ->  [" << l << "] ";
			states[l]->print();
		}
    }
    std::cout << std::endl;
}


VoterMeasurementTrajectory::VoterMeasurementTrajectory (VoterMeasurement *m, VoterTrajectory *trajectory)
{
	measurement = m;
    graph = measurement->graph;
    length = trajectory->length;

    states = new VoterMeasurementState *[length];

	for (int l = 0; l < length; l++)
		states[l] = measurement->getState(trajectory->states[l]);
}


VoterMeasurementTrajectory::~VoterMeasurementTrajectory ()
{
    for (int l = 0; l < length; l++)
		delete states[l];
	delete [] states;
}


void VoterMeasurementTrajectory::print ()
{
    std::cout << "START:  ";

    if (length > 0)
    {
		std::cout << "[0] ";
		states[0]->print();

		for (int l = 1; l < length; l++)
		{
			std::cout << "  ->  [" << l << "] ";
			states[l]->print();
		}
    }
    std::cout << std::endl;
}


VoterDataSet::VoterDataSet (VoterGraph *g, int t, int d, int trainS, int testS, int trainL, int testL)
{
    graph = g;
    time = t;
	delay = d;
	trainSize = trainS;
    testSize = testS;
	trainLength = trainL;
	testLength = testL;

	if (VERBOSE) { std::cout << "BUILDING DATA SET: "; }
    trajectories = new VoterTrajectory *[trainSize + testSize];
    for (int t = 0; t < trainSize + testSize; t++)
	{
		if (VERBOSE) { std::cout << floor(t*100/(trainSize+testSize)) << "% "; }
		trajectories[t] = new VoterTrajectory (graph, time, trainLength + testLength + delay);
		//trajectories[t]->print();
	}
	if (VERBOSE) { std::cout << std::endl; }

	transMap = new TransitionMap ();
}


VoterDataSet::~VoterDataSet ()
{
    for (int t = 0; t < trainSize + testSize; t++) { delete trajectories[t]; }
    delete[] trajectories;
	deleteTransitionMap();
}


void VoterDataSet::deleteTransitionMap ()
{
	for (TransitionMap::iterator it1 = transMap->begin(); it1 != transMap->end(); ++it1)
	{
		ProbabilityMap *probMap = it1->second->first;
		for (ProbabilityMap::iterator it2 = probMap->begin(); it2 != probMap->end(); ++it2)
		{
			delete it2->first;
			delete it2->second;
		}
		delete it1->first;
		delete it1->second->first;
		delete it1->second->second;
		delete it1->second;
	}
	delete transMap;
}


PredictionDataset *VoterDataSet::getPredictionDataset (MultiSet *preSet, MultiSet *postSet)
{
	PredictionDataset *dataset = new PredictionDataset (preSet, postSet);

	for (int t = 0; t < trainSize; t++)
    {
		VoterTrajectory *traj = trajectories[t];
		for (int l = 0; l < trainLength; l++)
		{
			int *preIndices = new int [preSet->dimension];
			for (int d = 0; d < preSet->dimension; d++)
			{
				/*
				VoterMeasurementState *preS = preSet->uniSetArray[d]->voterMeasurement->getState(traj->states[l]);
				preIndices[d] = preS->probeStates[0];
				*/
				preIndices[d] = preSet->uniSetArray[d]->voterProbe->getState(traj->states[l],MACRO_STATE);
			}

			int *postIndices = new int [postSet->dimension];
			for (int d = 0; d < postSet->dimension; d++)
			{
				/*
				VoterMeasurementState *postS = postSet->uniSetArray[d]->voterMeasurement->getState(traj->states[l+delay]);
				postIndices[d] = postS->probeStates[0];
				*/
				postIndices[d] = postSet->uniSetArray[d]->voterProbe->getState(traj->states[l+delay],MACRO_STATE);
			}

			dataset->addTrainValue (preSet->getAtomicMultiSubset(preIndices), postSet->getAtomicMultiSubset(postIndices));
			//dataset->addTestValue (preSet->getAtomicMultiSubset(preIndices), postSet->getAtomicMultiSubset(postIndices));
		}
	}

	for (int t = 0; t < trainSize + testSize; t++)
	{
		VoterTrajectory *traj = trajectories[t];

		int start = 0;
		if (t < trainSize) { start = trainLength; }
		for (int l = start; l < trainLength + testLength; l++)
		{
			int *preIndices = new int [preSet->dimension];
			for (int d = 0; d < preSet->dimension; d++)
			{
				/*
				VoterMeasurementState *preS = preSet->uniSetArray[d]->voterMeasurement->getState(traj->states[l]);
				preIndices[d] = preS->probeStates[0];
				*/
				preIndices[d] = preSet->uniSetArray[d]->voterProbe->getState(traj->states[l],MACRO_STATE);
			}

			int *postIndices = new int [postSet->dimension];
			for (int d = 0; d < postSet->dimension; d++)
			{
				/*
				VoterMeasurementState *postS = postSet->uniSetArray[d]->voterMeasurement->getState(traj->states[l+delay]);
				postIndices[d] = postS->probeStates[0];
				*/
				postIndices[d] = postSet->uniSetArray[d]->voterProbe->getState(traj->states[l+delay],MACRO_STATE);
			}

			dataset->addTestValue (preSet->getAtomicMultiSubset(preIndices), postSet->getAtomicMultiSubset(postIndices));
		}
	}
	
	return dataset;
}


void VoterDataSet::estimateTransitionMap (VoterMeasurement *preM, VoterMeasurement *postM)
{
	// initialise
	deleteTransitionMap();
	transMap = new TransitionMap();

    // count occurrences
    for (int t = 0; t < trainSize; t++)
    {
		VoterTrajectory *traj = trajectories[t];
		for (int l = 0; l < trainLength; l++)
		{
			VoterMeasurementState *preS = preM->getState(traj->states[l]);
			VoterMeasurementState *postS = postM->getState(traj->states[l+delay]);

			//preS->print(); postS->print(); std::cout << std::endl;

			ProbabilityPair *result1 = findInTransitionMap (transMap, preS);
			if (result1 != 0)
			{
				ProbabilityMap *probMap = result1->first;
				int *marginCount = result1->second;
				*marginCount = (*marginCount)+1;

				int *count = findInProbabilityMap (probMap, postS);
				if (count != 0) { *count = (*count)+1; }						
				else { probMap->insert(std::pair<VoterMeasurementState*,int*>(postS, new int(1))); }
			}

			else {
				ProbabilityMap *probMap = new ProbabilityMap();
				ProbabilityPair *probPair = new ProbabilityPair(probMap, new int(1));
				transMap->insert(std::pair<VoterMeasurementState*,ProbabilityPair*>(preS, probPair));
				probMap->insert(std::pair<VoterMeasurementState*,int*>(postS, new int(1)));
			}
		}
	}

    // normalise
/*	
	int cardinality = postM->getCardinality();
	for (TransitionMap::iterator it1 = transMap->begin(); it1 != transMap->end(); ++it1)
	{
	ProbabilityMap *probMap = it1->second;
	double total = 0;
	for (ProbabilityMap::iterator it2 = probMap->begin(); it2 != probMap->end(); ++it2) { total += *(it2->second); }
	total += cardinality*prior;
	for (ProbabilityMap::iterator it2 = probMap->begin(); it2 != probMap->end(); ++it2) { *(it2->second) = (*(it2->second) + prior)/total; }
	}
*/
}

void VoterDataSet::printTransitionMap ()
{
	for (TransitionMap::iterator it1 = transMap->begin(); it1 != transMap->end(); ++it1)
	{
		VoterMeasurementState *preS = it1->first;
		ProbabilityMap *probMap = it1->second->first;
		int *marginCount = it1->second->second;
		
		for (ProbabilityMap::iterator it2 = probMap->begin(); it2 != probMap->end(); ++it2)
		{
			VoterMeasurementState *postS = it2->first;
			int *count = it2->second;

			std::cout << "from ";
			preS->print();
			std::cout << " to ";
			postS->print();
			std::cout << " with probability " << *count << " / " << *marginCount << " = " << (*count)/((double)*marginCount) << std::endl;
		}
	}	
}


void VoterDataSet::print (int size)
{
	std::cout << "TRAIN SET:" << std::endl;
	for (int t = 0; t < trainSize && (size < 0 || t < size); t++)
	{
		VoterTrajectory *traj = trajectories[t];
		traj->print();
	}

	std::cout << "TEST SET:" << std::endl;
	for (int t = trainSize; t < trainSize + testSize && (size < 0 || t < trainSize + size); t++)
	{
		VoterTrajectory *traj = trajectories[t];
		traj->print();
	}
}


double VoterDataSet::getLogScore (VoterMeasurement *preM, VoterMeasurement *postM, int prior)
{
	int newCount = 0;
	double score = 0;
	int cardinality = postM->getCardinality();
	
	for (int t = 0; t < trainSize + testSize; t++)
	{
		VoterTrajectory *traj = trajectories[t];

		int start = 0;
		if (t < trainSize) { start = trainLength; }
		for (int l = start; l < trainLength + testLength; l++)
		{
			newCount++;
			VoterMeasurementState *preS = preM->getState(traj->states[l]);
			VoterMeasurementState *postS = postM->getState(traj->states[l+delay]);

			//preS->print(); postS->print(); std::cout << std::endl;

			ProbabilityPair *result1 = findInTransitionMap (transMap, preS);
			if (result1 != 0)
			{
				ProbabilityMap *probMap = result1->first;
				int *marginCount = result1->second;
				int *count = findInProbabilityMap (probMap, postS);

				//std::cout << *count << "/" << *marginCount << " = " << log((double)(*count)/(double)(*marginCount)) << std::endl;
				if (count != 0) { score +=  log10(*marginCount + prior*cardinality) - log10(*count + prior); }
				else if (prior > 0) { score += log10(*marginCount + prior*cardinality) - log10(prior); }
				else { return -1; }
			}

			else { score += log10(cardinality); }
		}
	}
	if (newCount > 0) { return score/newCount; }
	else { return 0; }
}


double VoterDataSet::getQuadScore (VoterMeasurement *preM, VoterMeasurement *postM, int prior)
{
	int newCount = 0;
	double score = 0;
	int cardinality = postM->getCardinality();

	ProbabilityMap quadMap;
	
	for (int t = 0; t < trainSize + testSize; t++)
	{
		VoterTrajectory *traj = trajectories[t];

		int start = 0;
		if (t < trainSize) { start = trainLength; }
		for (int l = start; l < trainLength + testLength; l++)
		{
			newCount++;
			VoterMeasurementState *preS = preM->getState(traj->states[l]);
			VoterMeasurementState *postS = postM->getState(traj->states[l+delay]);

			ProbabilityPair *result1 = findInTransitionMap (transMap, preS);
			if (result1 != 0)
			{
				ProbabilityMap *probMap = result1->first;
				int *marginCount = result1->second;

				int *quad = findInProbabilityMap (&quadMap, preS);
				if (quad == 0)
				{
					int c = 0; int q = 0;

					for (ProbabilityMap::iterator it = probMap->begin(); it != probMap->end(); ++it)
					{ c++; q += pow(*it->second + prior,2); }
					q += (cardinality-c) * pow(prior,2);

					quad = new int(q);
					quadMap.insert(std::pair<VoterMeasurementState*,int*>(preS,quad));
				}

				int *count = findInProbabilityMap (probMap, postS);

				int r = *marginCount + prior * cardinality;
				if (count != 0) { score += 2 * (double)(*count + prior) / r - (double)(*quad) / pow(r,2); }
				else { score += 2 * (double)(prior)/(*marginCount + prior*cardinality) - (double)(*quad) / pow(r,2); }
			}

			else { score += 1. / cardinality; }
		}
	}

	if (newCount > 0) { return score/newCount; }
	else { return 0; }
}


VoterBinning *VoterDataSet::getOptimalBinning (VoterMeasurement *preM, VoterMeasurement *postM, int prior, int realTrainSize, bool verbose)
{
	// Define variables
	
	if (preM->probeNumber > 1) { std::cout << "ERROR: cannot compute optimal binning on multi-probe pre-measurement!" << std::endl; return 0; }
	if (postM->probeNumber > 1) { std::cout << "ERROR: cannot compute optimal binning on multi-probe post-measurement!" << std::endl; return 0; }

	int preSize = preM->getCardinality();
	int postSize = postM->getCardinality();
	int preSizeSquare = pow(preSize,2);
	
	int trainNb = prior * preSize * postSize;
	int *trainKL = new int [preSize * preSize * postSize];
	int *trainK = new int [preSize * preSize];
	int *trainL = new int [postSize];

	int testNb = 0;
	int *testKL = new int [preSize * preSize * postSize];
	int *testK = new int [preSize * preSize];
	int *testL = new int [postSize];


	// Initialise variables
	
	for (int i = 0; i < preSize; i++)
		for (int k = 0; k < postSize; k++)
		{
			trainKL[i + k * preSizeSquare] = prior;
			testKL[i + k * preSizeSquare] = 0;
		}

	for (int i = 0; i < preSize; i++)
	{
		trainK[i] = prior * postSize;
		testK[i] = 0;
	}
	
	for (int k = 0; k < postSize; k++)
	{
		trainL[k] = prior * preSize;
		testL[k] = 0;
	}
	

	// Compute variables at the micro-level
	
    for (int t = 0; t < trainSize + testSize; t++)
    {
		VoterTrajectory *traj = trajectories[t];

		bool realTrain = (realTrainSize < 0 && t < trainSize) || (realTrainSize >= 0 && t < realTrainSize);
		if (realTrain)
		{
			for (int l = 0; l < trainLength; l++)
			{
				int preS = preM->getState(traj->states[l])->probeStates[0];
				int postS = postM->getState(traj->states[l+delay])->probeStates[0];

				trainNb++;
				trainKL[preS + postS * preSizeSquare]++;
				trainK[preS]++;
				trainL[postS]++;
			}
		}

		if (realTrain || t >= trainSize)
		{
			int start = 0;
			if (realTrain) { start = trainLength; }
			for (int l = start; l < trainLength + testLength; l++)
			{
				int preS = preM->getState(traj->states[l])->probeStates[0];
				int postS = postM->getState(traj->states[l+delay])->probeStates[0];

				testNb++;
				testKL[preS + postS * preSizeSquare]++;
				testK[preS]++;
				testL[postS]++;
			}
		}
	}

	
	// Compute variables at the other levels
	
	for (int j = 1; j < preSize; j++)
		for (int i = 0; i < preSize - j; i++)
			for (int k = 0; k < postSize; k++)
			{
				trainKL[i + j * preSize + k * preSizeSquare] = trainKL[i + (j-1) * preSize + k * preSizeSquare] + trainKL[(i+j) + k * preSizeSquare];
				testKL[i + j * preSize + k * preSizeSquare] = testKL[i + (j-1) * preSize + k * preSizeSquare] + testKL[(i+j) + k * preSizeSquare];
			}

	for (int j = 1; j < preSize; j++)
		for (int i = 0; i < preSize - j; i++)
		{
			trainK[i + j * preSize] = trainK[i + (j-1) * preSize] + trainK[(i+j)];
			testK[i + j * preSize] = testK[i + (j-1) * preSize] + testK[(i+j)];
		}


	// Compute the score function at all levels
	
	double *score = new double [preSize * preSize];

	for (int j = 0; j < preSize; j++)
		for (int i = 0; i < preSize - j; i++)
		{
			int index = i + j * preSize;

			if (trainK[index] > 0)
			{
				score[index] = testK[index] * log10(trainK[index]);
				for (int k = 0; k < postSize; k++)
					score[index] -= testKL[index + k * preSizeSquare] * log10(trainKL[index + k * preSizeSquare]);
			}

			else {
				score[index] = testK[index] * log10(trainNb);
				for (int k = 0; k < postSize; k++)
					score[index] -= testKL[index + k * preSizeSquare] * log10(trainL[k]);
			}
		}


	// Compute the optimal cut
	
	int *optimalCut = new int [preSize];
    double *optimalValue = new double [preSize];
	
    for (int j = 0; j < preSize; j++)
    {
		optimalCut[j] = 0;
		optimalValue[j] = score[j * preSize];

		for (int cut = 1; cut <= j; cut++)
		{
			double value = optimalValue[cut-1] + score[cut + (j-cut) * preSize];
			if (isnan(optimalValue[j]) || (!isnan(value) && value < optimalValue[j]))
			{
				optimalCut[j] = cut;
				optimalValue[j] = value;
			}
		}		
    }


	
	// Compute results

	VoterBinning *result = new VoterBinning (this);
	result->size = preSize;
	result->score = optimalValue[preSize-1] / testNb;

	int j = preSize;
	while (j > 0)
	{
		j = optimalCut[j-1];
		result->binNumber++;
	}

	result->cuts = new int [result->binNumber];

	int n = result->binNumber-1;
	j = preSize;
	while (j > 0)
	{
		int cut = optimalCut[j-1];
		result->cuts[n--] = cut;
		j = cut;
	}

	
	// Print results

	if (verbose)
	{
		std::cout << std::endl;
		std::cout << "trainNb   " << trainNb << std::endl;
		
		std::cout << std::endl;
		std::cout << "trainKL" << std::endl;
		for (int j = 0; j < preSize; j++)
		{
			for (int i = 0; i < preSize - j; i++)
			{
				std::cout << "(";
				for (int k = 0; k < postSize; k++)
				{
					if (k > 0) { std::cout << ","; }
					std::cout << trainKL[i + j * preSize + k * preSizeSquare];
				}
				std::cout << ")   ";
			}
			std::cout << std::endl;
		}

		std::cout << std::endl;
		std::cout << "trainK" << std::endl;
		for (int j = 0; j < preSize; j++)
		{
			for (int i = 0; i < preSize - j; i++)
				std::cout << trainK[i + j * preSize] << "   ";
			std::cout << std::endl;
		}

		std::cout << std::endl;
		std::cout << "trainL" << std::endl;
		for (int k = 0; k < postSize; k++)
			std::cout << trainL[k] << "   ";
		std::cout << std::endl;
		
		std::cout << std::endl;
		std::cout << "testKL" << std::endl;
		for (int j = 0; j < preSize; j++)
		{
			for (int i = 0; i < preSize - j; i++)
			{
				std::cout << "(";
				for (int k = 0; k < postSize; k++)
				{
					if (k > 0) { std::cout << ","; }
					std::cout << testKL[i + j * preSize + k * preSizeSquare];
				}
				std::cout << ")   ";
			}
			std::cout << std::endl;
		}

		
		std::cout << std::endl;
		std::cout << "testNb   " << testNb << std::endl;
		
		std::cout << std::endl;
		std::cout << "testK" << std::endl;
		for (int j = 0; j < preSize; j++)
		{
			for (int i = 0; i < preSize - j; i++)
				std::cout << testK[i + j * preSize] << "   ";
			std::cout << std::endl;
		}

		std::cout << std::endl;
		std::cout << "testL" << std::endl;
		for (int k = 0; k < postSize; k++)
			std::cout << testL[k] << "   ";		
		std::cout << std::endl;

		std::cout << std::endl;
		std::cout << "score" << std::endl;
		for (int j = 0; j < preSize; j++)
		{
			for (int i = 0; i < preSize - j; i++)
			{
				if (isnan(score[i + j * preSize])) { std::cout << "infinite   "; }
				else { std::cout << std::setprecision(5) << score[i + j * preSize] << "   "; }
			}
			std::cout << std::endl;
		}

		
		std::cout << std::endl;
		std::cout << "optimalCut" << std::endl;
		for (int j = 0; j < preSize; j++)
			std::cout << optimalCut[j] << "   ";
		std::cout << std::endl;

		std::cout << std::endl;
		std::cout << "optimalValue" << std::endl;
		for (int j = 0; j < preSize; j++)
		{
			if (isnan(optimalValue[j])) { std::cout << "infinite   "; }
			else { std::cout << std::setprecision(5) << optimalValue[j] << "   "; }
		}
		std::cout << std::endl;

		std::cout << std::endl;
		result->print();
	}

		
	// Free memory
	
	delete [] trainKL;
	delete [] trainK;
	delete [] trainL;
	delete [] testKL;
	delete [] testK;
	delete [] testL;

	delete [] score;
	delete [] optimalCut;
	delete [] optimalValue;

	
	// Return result

	return result;
}


ProbabilityPair *VoterDataSet::findInTransitionMap (TransitionMap *transMap, VoterMeasurementState *preState)
{
	bool found = false;
	TransitionMap::iterator it;
	for (it = transMap->begin(); it != transMap->end(); ++it)
	{
		VoterMeasurementState *pS = it->first;
		if (preState->isEqual(pS)) { found = true; break; }
	}

	if (found) { return it->second; } else { return 0; }
}


int *VoterDataSet::findInProbabilityMap (ProbabilityMap *probMap, VoterMeasurementState *postState)
{
	bool found = false;
	ProbabilityMap::iterator it;
	for (it = probMap->begin(); it != probMap->end(); ++it)
	{
		VoterMeasurementState *pS = it->first;
		if (postState->isEqual(pS)) { found = true; break; }
	}

	if (found) { return it->second; } else { return 0; }
}


VoterBinning::VoterBinning (VoterDataSet *d)
{
	data = d;
	size = 0;
	binNumber = 0;
	cuts = 0;
	score = 0;
}


VoterBinning::~VoterBinning ()
{
	if (cuts != 0) { delete [] cuts; }
}


void VoterBinning::print (bool verbose)
{
 	std::cout << "SCORE = " << score << std::endl;
	std::cout << "BINS  = " << binNumber << std::endl;

	if (verbose)
	{
		int n = 0;
		std::cout << "CUTS  = ";
		for (int j = 0; j < size; j++)
			if (cuts[n] == j) { std::cout << "|" << j; n++; } else { std::cout << " " << j; }
		std::cout << "|" << std::endl;

		/*
		std::cout << "CUTS  = ";
		for (int n = 0; n < binNumber; n++)
		{
			int c1 = cuts[n];
			int c2 = size-1;
			if (n+1 < binNumber) { c2 = cuts[n+1]-1; }
			if (c1 == c2) { std::cout << "[" << c1 << "] "; }
			else { std::cout << "[" << c1 << "," << c2 << "] "; }
		}
		std::cout << std::endl;
		*/
	}
}



void addMeasurement (MeasurementSet *set, VoterGraph *VG, MeasurementType type, VoterMetric metric)
{
    switch (type)
    {
    case M_MACRO :
    case M_EMPTY :
    case M_AGENT1 :		
    case M_AGENT1_MACRO :
    case M_MICRO:
    case M_MESO1 :
    case M_MESO2 :
    case M_MESO1_MESO2 :
    case M_AGENT1_MESO1 :
    case M_AGENT1_MESO2 :
    case M_AGENT1_MESO1_MESO2 :
		set->insert(getMeasurement (VG, type, metric));
		break;

    default : addMultiMeasurement (set, VG, type, metric);
    }
}


VoterMeasurement *getMeasurement (VoterGraph *VG, MeasurementType type, VoterMetric metric, int binning)
{
    std::string postfix = "";
    switch (metric)
    {
    case MACRO_STATE :          postfix = "_MS";	break;
    case MAJORITY :		postfix = "_MAJ";	break;
    case MAJ_1PC :		postfix = "_1PC";	break;
    case MAJ_2PC :		postfix = "_2PC";	break;
    case MAJ_3PC :		postfix = "_3PC";	break;
    case MAJ_4PC :		postfix = "_4PC";	break;
    case MAJ_5PC :		postfix = "_5PC";	break;
    case MAJ_6PC :		postfix = "_6PC";	break;
    case MAJ_7PC :		postfix = "_7PC";	break;
    case MAJ_8PC :		postfix = "_8PC";	break;
    case MAJ_9PC :		postfix = "_9PC";	break;
    case MAJ_10PC :		postfix = "_10PC";	break;
    case MAJ_20PC :		postfix = "_20PC";	break;
    case MAJ_30PC :		postfix = "_30PC";	break;
    case MAJ_40PC :		postfix = "_40PC";	break;
    case MAJ_50PC :		postfix = "_50PC";	break;
    case MAJ_60PC :		postfix = "_60PC";	break;
    case MAJ_70PC :		postfix = "_70PC";	break;
    case MAJ_80PC :		postfix = "_80PC";	break;
    case MAJ_90PC :		postfix = "_90PC";	break;
    case MAJ_2B :		postfix = "_2B";	break;
    case MAJ_3B :		postfix = "_3B";	break;
    case MAJ_4B :		postfix = "_4B";	break;
    case MAJ_6B :		postfix = "_6B";	break;
    case MAJ_8B :		postfix = "_8B";	break;
    case MAJ_10B :		postfix = "_10B";	break;
    case MAJ_12B :		postfix = "_12B";	break;
    case MAJ_20B :		postfix = "_20B";	break;
    case MAJ_40B :		postfix = "_40B";	break;
    case ACTIVE_EDGES :	        postfix = "_AE";	break;
    default :			postfix = "_NA";	break;
    }

    // build probes
    TwoCommunitiesVoterGraph *TCVG = dynamic_cast<TwoCommunitiesVoterGraph*>(VG);
    ChainVoterGraph *CVG = dynamic_cast<ChainVoterGraph*>(VG);

    VoterProbe *macroPr = new VoterProbe(VG);
    for (std::set<VoterNode*>::iterator it = VG->nodeSet->begin(); it != VG->nodeSet->end(); ++it) { macroPr->addNode(*it); }

    VoterProbe *agent1Pr = new VoterProbe(VG);
    if (TCVG != NULL) { agent1Pr->addNode(*TCVG->community1->begin()); }
    if (CVG != NULL) { agent1Pr->addNode(CVG->nodeArray[0]); }

	VoterProbe *meso1Pr = new VoterProbe(VG);
	VoterProbe *meso2Pr = new VoterProbe(VG);							
	if (TCVG != NULL)
	{
		for (std::set<VoterNode*>::iterator it = TCVG->community1->begin(); it != TCVG->community1->end(); ++it) { meso1Pr->addNode(*it); }
		for (std::set<VoterNode*>::iterator it = TCVG->community2->begin(); it != TCVG->community2->end(); ++it) { meso2Pr->addNode(*it); }
	}
    
    switch (type)
    {
    case M_MACRO :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"MACRO" + postfix);
		m->addProbe(macroPr,metric,binning);
		return m;
    }
		
    case M_EMPTY :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"EMPTY");
		return m;
    }
		
    case M_AGENT1 :		
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"AGENT1" + postfix);
		m->addProbe(agent1Pr,metric,binning);
		return m;
    }

    case M_AGENT1_MACRO :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"AGENT1_MACRO" + postfix);
		m->addProbe(agent1Pr,metric,binning);
		m->addProbe(macroPr,metric,binning);
		return m;
    }
		
    case M_MICRO:
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"MICRO" + postfix);
		for (std::set<VoterNode*>::iterator it = VG->nodeSet->begin(); it != VG->nodeSet->end(); ++it)
		{
			VoterProbe *probe = new VoterProbe(VG);
			probe->addNode(*it);
			m->addProbe(probe,metric,binning);
		}
		return m;
    }
		
    case M_MESO1 :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"MESO1" + postfix);
		m->addProbe(meso1Pr,metric,binning);
		return m;
    }

    case M_MESO2 :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"MESO2" + postfix);
		m->addProbe(meso2Pr,metric,binning);
		return m;
    }

    case M_MESO1_MESO2 :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"MESO1_MESO2" + postfix);
		m->addProbe(meso1Pr,metric,binning);
		m->addProbe(meso2Pr,metric,binning);
		return m;
    }
			
    case M_AGENT1_MESO1 :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"AGENT1_MESO1" + postfix);
		m->addProbe(agent1Pr,metric,binning);
		m->addProbe(meso1Pr,metric,binning);
		return m;
    }

    case M_AGENT1_MESO2 :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"AGENT1_MESO2" + postfix);
		m->addProbe(agent1Pr,metric,binning);
		m->addProbe(meso2Pr,metric,binning);
		return m;
    }
			
    case M_AGENT1_MESO1_MESO2 :
    {
		VoterMeasurement *m = new VoterMeasurement(VG,"AGENT1_MESO1_MESO2" + postfix);
		m->addProbe(meso1Pr,metric,binning);
		m->addProbe(meso2Pr,metric,binning);
		m->addProbe(agent1Pr,metric,binning);
		return m;
    }
    	    
    default : std::cout << "ERROR: unknown measurement!" << std::endl;
    }

    return 0;
}


void addMultiMeasurement (MeasurementSet *set, VoterGraph *VG, MeasurementType type, VoterMetric metric)
{
    std::string postfix = "";
    switch (metric)
    {
    case MACRO_STATE :          postfix = "_MS";	break;
    case MAJORITY :		postfix = "_MAJ";	break;
    case MAJ_1PC :		postfix = "_1PC";	break;
    case MAJ_2PC :		postfix = "_2PC";	break;
    case MAJ_3PC :		postfix = "_3PC";	break;
    case MAJ_4PC :		postfix = "_4PC";	break;
    case MAJ_5PC :		postfix = "_5PC";	break;
    case MAJ_6PC :		postfix = "_6PC";	break;
    case MAJ_7PC :		postfix = "_7PC";	break;
    case MAJ_8PC :		postfix = "_8PC";	break;
    case MAJ_9PC :		postfix = "_9PC";	break;
    case MAJ_10PC :		postfix = "_10PC";	break;
    case MAJ_20PC :		postfix = "_20PC";	break;
    case MAJ_30PC :		postfix = "_30PC";	break;
    case MAJ_40PC :		postfix = "_40PC";	break;
    case MAJ_50PC :		postfix = "_50PC";	break;
    case MAJ_60PC :		postfix = "_60PC";	break;
    case MAJ_70PC :		postfix = "_70PC";	break;
    case MAJ_80PC :		postfix = "_80PC";	break;
    case MAJ_90PC :		postfix = "_90PC";	break;
    case MAJ_2B :		postfix = "_2B";	break;
    case MAJ_3B :		postfix = "_3B";	break;
    case MAJ_4B :		postfix = "_4B";	break;
    case MAJ_6B :		postfix = "_6B";	break;
    case MAJ_8B :		postfix = "_8B";	break;
    case MAJ_10B :		postfix = "_10B";	break;
    case MAJ_12B :		postfix = "_12B";	break;
    case MAJ_20B :		postfix = "_20B";	break;
    case MAJ_40B :		postfix = "_40B";	break;
    case ACTIVE_EDGES :	        postfix = "_AE";	break;
    default :			postfix = "_NA";	break;
    }

    // build probes
    TwoCommunitiesVoterGraph *TCVG = dynamic_cast<TwoCommunitiesVoterGraph*>(VG);
    ChainVoterGraph *CVG = dynamic_cast<ChainVoterGraph*>(VG);

    VoterProbe *macroPr = new VoterProbe(VG);
    for (std::set<VoterNode*>::iterator it = VG->nodeSet->begin(); it != VG->nodeSet->end(); ++it) { macroPr->addNode(*it); }

    VoterProbe *agent1Pr = new VoterProbe(VG);
    if (TCVG != NULL) { agent1Pr->addNode(*TCVG->community1->begin()); }
    if (CVG != NULL) { agent1Pr->addNode(CVG->nodeArray[0]); }

    VoterProbe *meso1Pr = new VoterProbe(VG);
    for (std::set<VoterNode*>::iterator it = TCVG->community1->begin(); it != TCVG->community1->end(); ++it) { meso1Pr->addNode(*it); }

    VoterProbe *meso2Pr = new VoterProbe(VG);							
    for (std::set<VoterNode*>::iterator it = TCVG->community2->begin(); it != TCVG->community2->end(); ++it) { meso2Pr->addNode(*it); }

    
    switch (type)
    {
    case M_ALLSIZES1 : case M_SOMESIZES1 : case M_AGENT1_ALLSIZES1 : case M_AGENT1_SOMESIZES1 :
    {
		std::set<VoterNode*> *nodeSet = VG->nodeSet;
		TwoCommunitiesVoterGraph *TCVG = dynamic_cast<TwoCommunitiesVoterGraph*>(VG);
		if (TCVG != NULL) { nodeSet = TCVG->community1; }
		int size = VG->nodeSet->size();
	
		VoterProbe **prList = new VoterProbe *[size-1];
		VoterMeasurement **mList = new VoterMeasurement *[size-1];

		for (int i = 1; i <= size; i++)
		{
			std::string str = int2string(i);

			std::string prefix = "";
			if (type == M_AGENT1_ALLSIZES1 || type == M_AGENT1_SOMESIZES1) { prefix = "AGENT1_"; }	    
	    
			prList[i-1] = new VoterProbe(VG);
			mList[i-1] = new VoterMeasurement(VG,prefix + "SIZE" + str + postfix);
			mList[i-1]->addProbe(prList[i-1],metric);
				
			if (type == M_AGENT1_ALLSIZES1 || type == M_AGENT1_SOMESIZES1) { mList[i-1]->addProbe(agent1Pr,metric); }
				
			switch (type)
			{
			case M_ALLSIZES1 : case M_AGENT1_ALLSIZES1 : set->insert(mList[i-1]); break;
			case M_SOMESIZES1 : case M_AGENT1_SOMESIZES1 : if (i % (size/5) == 0) { set->insert(mList[i-1]); } break;	
			default : break;
			}
		}
	
		int j = 1;
	
		for (std::set<VoterNode*>::iterator it = nodeSet->begin(); it != nodeSet->end(); ++it)
		{
			for (int i = j; i <= size; i++) { prList[i-1]->addNode(*it); }
			j++;
		}

		//delete [] prList;
		//delete [] mList;

		break;
    }


    case M_ALLNEIGHBORHOODS :
    {
		ChainVoterGraph *CVG = dynamic_cast<ChainVoterGraph*>(VG);
		if (CVG != NULL)
		{
			if (CVG->ring)
			{
				int size = CVG->size;
				VoterProbe **prList = new VoterProbe *[size/2];
				VoterMeasurement **mList = new VoterMeasurement *[size/2];
		
				for (int i = 1; i <= size/2; i++)
				{
					std::stringstream ss; ss << i;
					std::string str = ss.str();
		
					prList[i-1] = new VoterProbe(VG);
					prList[i-1]->addNode(CVG->nodeArray[0]);
					for (int j = 1; j <= i; j++)
					{
						prList[i-1]->addNode(CVG->nodeArray[j]);
						prList[i-1]->addNode(CVG->nodeArray[size-j]);
					}

					mList[i-1] = new VoterMeasurement(VG,"NEIGHBORHOOD" + str + postfix);
					mList[i-1]->addProbe(prList[i-1],metric);
					set->insert(mList[i-1]);
				}
	    
				delete [] prList;
				delete [] mList;
			}
		}
		break;
    }
	    
    default : std::cout << "ERROR: unknown measurement!" << std::endl;
    }
}

		
