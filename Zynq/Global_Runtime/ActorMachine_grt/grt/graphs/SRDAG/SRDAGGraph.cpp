
/********************************************************************************
 * Copyright or � or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-Fran�ois Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 * 
 * @author mpelcat
 */

#include "SRDAGGraph.h"
#include "SRDAGVertex.h"
#include "../CSDAG/CSDAGVertex.h"
#include "SRDAGEdge.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

/**
 Constructor
*/
SRDAGGraph::SRDAGGraph()
{
	// There is no dynamic allocation of graph members
	nbVertices = 0;
//	memset(vertices,'\0',MAX_SRDAG_VERTICES*sizeof(SRDAGVertex));
	nbEdges = 0;
//	memset(edges,'\0',MAX_SRDAG_EDGES*sizeof(SRDAGEdge));
}

/**
 Destructor
*/
SRDAGGraph::~SRDAGGraph()
{
}

/**
 Adding an edge to the graph
 
 @param source: The source vertex of the edge
 @param production: number of tokens (chars) produced by the source
 @param sink: The sink vertex of the edge
 @param consumption: number of tokens (chars) consumed by the sink
 @return the created edge
*/
SRDAGEdge* SRDAGGraph::addEdge(SRDAGVertex* source, int tokenRate, SRDAGVertex* sink){
	SRDAGEdge* edge;
#ifdef _DEBUG
	if(nbEdges >= MAX_SRDAG_EDGES){
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
#endif
	edge = &edges[nbEdges];
	edge->setSource(source);
	edge->setTokenRate(tokenRate);
	edge->setSink(sink);
	source->addOutputEdge(edge);
	sink->addInputEdge(edge);
	nbEdges++;
	return edge;
}

/**
 Removes the last added edge
*/
void SRDAGGraph::removeLastEdge(){
	if(nbEdges > 0){
		nbEdges--;
	}
	else{
		// Removing an edge from an empty graph
		exitWithCode(1007);
	}
}

/**
 Removes all edges and vertices
*/
void SRDAGGraph::flush(){
	nbVertices = nbEdges = 0;
	SRDAGEdge::firstInSinkOrder = NULL;
}

#if 0
/**
 Pivot function for Quick Sort
*/
int partition(SRDAGEdge* edgePointers, int p, int r, char sourceOrSink) {
  SRDAGEdge* x = &edgePointers[r];
  static SRDAGEdge temp;
  int j = p - 1;
  for (int i = p; i < r; i++) {
	  if (((sourceOrSink != 0) && (x->source >= edgePointers[i].source)) ||
		  ((sourceOrSink == 0) && (x->sink >= edgePointers[i].sink))) {
      j = j + 1;
      memcpy(&temp,&edgePointers[j],sizeof(SRDAGEdge));
      memcpy(&edgePointers[j],&edgePointers[i],sizeof(SRDAGEdge));
      memcpy(&edgePointers[i],&temp,sizeof(SRDAGEdge));
    }
  }

  memcpy(&edgePointers[r],&edgePointers[j + 1],sizeof(SRDAGEdge));
  memcpy(&edgePointers[j + 1],x,sizeof(SRDAGEdge));

  return (j + 1);
}

/**
 quick Sort recursive algorithm 
*/
void quickSort(SRDAGEdge* edgePointers, int p, int r, char sourceOrSink) {
  if (p < r) {
    int q = partition(edgePointers, p, r, sourceOrSink);
    quickSort(edgePointers, p, q - 1, sourceOrSink);
    quickSort(edgePointers, q + 1, r, sourceOrSink);
  }
}

/**
 quick Sort algorithm used for sorting edges pointers from their source or sink address
*/
void quickSort(SRDAGEdge* edgePointers, int length, char sourceOrSink) {
  quickSort(edgePointers, 0, length-1, sourceOrSink);
}
#endif

/**
 Gets pointers on the edges of the graph in the order of their source or sink.
 Accelerates getting the input or output edges. The edges are linked together
 to constitute a linked list.
 
 @param startIndex: only the edges after this index are reordered
*/
void SRDAGGraph::sortEdges(int startIndex){
	/*int length = this->nbEdges - startIndex;
	if(length>1){
		quickSort((&edges[startIndex]), length, 0);
	}*/
	SRDAGEdge* currentNewEdge, *currentOldEdge;
	SRDAGVertex* currentNewSink;

	for(int i=startIndex; i<nbEdges; i++){
		currentNewEdge = &edges[i];
		
		// Adding the first edge
		if(SRDAGEdge::firstInSinkOrder == NULL){
			SRDAGEdge::firstInSinkOrder = currentNewEdge;
			SRDAGEdge::lastInSinkOrder = currentNewEdge;
			currentNewEdge->prevInSinkOrder = NULL;
			currentNewEdge->nextInSinkOrder = NULL;
		}
		else{
			currentNewSink = currentNewEdge->getSink();
			
			/*currentOldEdge = SRDAGEdge::firstInSinkOrder;
			// Going through the already ordered edges
			// while the edge has a next one and must be before the one we add, we go to the next one
			while((currentOldEdge->nextInSinkOrder != NULL) && (currentOldEdge->getSink() < currentNewSink)){
				currentOldEdge = currentOldEdge->nextInSinkOrder;
			}*/

			currentOldEdge = SRDAGEdge::lastInSinkOrder;
			// Going through the already ordered edges in reverse order
			// while the edge has a next one and must be before the one we add, we go to the next one
			while((currentOldEdge->prevInSinkOrder != NULL) && (currentOldEdge->getSink() > currentNewSink)){
				currentOldEdge = currentOldEdge->prevInSinkOrder;
			}

			// The next is null and we need to add the new edge after the old one
			if(currentOldEdge->getSink() <= currentNewSink){
				currentNewEdge->prevInSinkOrder = currentOldEdge;
				currentNewEdge->nextInSinkOrder = NULL;
				SRDAGEdge::lastInSinkOrder = currentNewEdge;
				currentOldEdge->nextInSinkOrder = currentNewEdge;
			}
			else{
				// We need to add the new edge before the old one
				currentNewEdge->prevInSinkOrder = currentOldEdge->prevInSinkOrder;
				if(currentOldEdge->prevInSinkOrder != NULL){
					currentOldEdge->prevInSinkOrder->nextInSinkOrder = currentNewEdge;
				}
				else{
					//New start vertex
					SRDAGEdge::firstInSinkOrder = currentNewEdge;
				}
				currentNewEdge->nextInSinkOrder = currentOldEdge;
				currentOldEdge->prevInSinkOrder = currentNewEdge;
			}
		}
	}
}

int SRDAGGraph::getMaxTime(){
	int sum=0;
	for(int i=0; i< nbVertices; i++){
		sum += vertices[i].getCsDagReference()->getIntTiming(0);
	}
	return sum;
}


static void iterateCriticalPath(SRDAGVertex* curVertex, int curLenght, int* max){
	curLenght += curVertex->getCsDagReference()->getIntTiming(0);
	if(curVertex->getNbOutputEdge() == 0){
		if(curLenght > *max) *max = curLenght;
		return;
	}
	for(int i=0; i<curVertex->getNbOutputEdge(); i++){
		iterateCriticalPath(curVertex->getOutputEdge(i)->getSink(), curLenght, max);
	}
}

int SRDAGGraph::getCriticalPath(){
	int max;
	iterateCriticalPath(this->getVertex(0), 0, &max);
	return max;
}
