/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include "SRDAGGraph.h"
#include "../PiSDF/PiSDFIfVertex.h"
#include "SRDAGEdge.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <grt_definitions.h>
#include <tools/DotWriter.h>
#include <tools/FitedArrayPool.h>
#include <grt_definitions.h>

/**
 Constructor
*/
SRDAGGraph::SRDAGGraph(): vertices(ABSTRACT_VERTICES), implodes(DEFAULT), /*edges(EDGES), */rbs(DEFAULT), brs(DEFAULT){
	edgePool.reset();
	vertexPool.reset();

	vertices.reset();
	implodes.reset();
	brs.reset();
	rbs.reset();

	resetAlloc();

#if DEBUG
	edgePool.setName("Edge Pool of SRDAG Graph");
	vertexPool.setName("Vertex Pool of SRDAG Graph");
	vertices.setName("Vertex List of SRDAG Graph");
	implodes.setName("Implode List of SRDAG Graph");
//	edges.setName("Edge List of SRDAG Graph");
	rbs.setName("RBs List of SRDAG Graph");
	brs.setName("Brs List of SRDAG Graph");
#endif
}

void SRDAGGraph::reset(){
	edgePool.reset();
	vertexPool.reset();

	vertices.reset();
	implodes.reset();
	brs.reset();
	rbs.reset();

	resetAlloc();
}

/**
 Destructor
*/
SRDAGGraph::~SRDAGGraph(){
}

void SRDAGGraph::removeEdge(SRDAGEdge* edge){
	if(edge->getSink())
		edge->disconnectSink();
	if(edge->getSource())
		edge->disconnectSource();
//	edges.remove(edge);
	edgePool.free(edge);
}

void SRDAGGraph::removeVertex(SRDAGVertex *vertex){
	// TODO remove Edges connected to it.
	if(vertex->getState() == SRDAG_Executed){
		printf("Delete executed Vertex\n");
	}
	vertices.remove(vertex);
	switch(vertex->getType()){
	case Implode:
		implodes.remove(vertex);
		break;
	case RoundBuffer:
		rbs.remove(vertex);
		break;
	case Broadcast:
		brs.remove(vertex);
		break;
	case Normal:
	case ConfigureActor:
	case Explode:
	case Init:
	case End:
		break;
	}
	vertexPool.free(vertex);
}

//void SRDAGGraph::removeVertex(int id){
//	SRDAGVertex* vertex = &(vertices[id]);
//	int i=0;
//	while(vertex->nbInput){
//		if(vertex->inputEdges[i] != NULL)
//			vertex->inputEdges[i]->disconnectSink();
//		i++;
//	}
//	i=0;
//	while(vertex->nbOutput){
//		if(vertex->outputEdges[i] != NULL)
//			vertex->outputEdges[i]->disconnectSource();
//		i++;
//	}
//
//	vertex->getReference()->removeChildVertex(vertex);
//
//	nbVertex--;
//	if(nbVertex!=0){
//		vertices[id] = vertices[nbVertex];
//
//		i=0;
//		int j=0;
//		vertex->id = id;
//		while(j<vertex->nbInput){
//			if(vertex->inputEdges[i] != NULL){
//				vertex->inputEdges[i]->sink = vertex;
//				j++;
//			}
//			i++;
//		}
//		i=j=0;
//		while(j<vertex->nbOutput){
//			if(vertex->outputEdges[i] != NULL){
//				vertex->outputEdges[i]->source = vertex;
//				j++;
//			}
//			i++;
//		}
//		vertex->getReference()->replaceChildVertex(&(vertices[nbVertex]), vertex);
//	}
//}

//SRDAGEdge* SRDAGGraph::addEdge(SRDAGVertex* source, UINT32 sourcePortIx, int tokenRate, SRDAGVertex* sink, UINT32 sinkPortIx, PiSDFEdge* refEdge){
//	SRDAGEdge* edge;
//
//	if(source->getType() == RoundBuffer
//			&& source->getNbInputEdge() > 0
//			&& tokenRate == source->getInputEdge(0)->getTokenRate()){
//		SRDAGVertex* rb = source;
//		edge = rb->getInputEdge(0);
//
//		rb->removeInputEdge(edge);
//		edge->setSink(sink);
//		sink->setInputEdge(edge, sinkPortIx);
//		removeVx(rb);
//	}else if(sink->getType() == RoundBuffer
//			&& sink->getNbOutputEdge() > 0
//			&& tokenRate == sink->getOutputEdge(0)->getTokenRate()){
//		SRDAGVertex* rb = sink;
//		edge = rb->getOutputEdge(0);
//
//		rb->removeOutputEdge(edge);
//		edge->setSource(source);
//		source->setOutputEdge(edge, sourcePortIx);
//		removeVx(rb);
//	}else{
//		edge = edges.add();
//		edge->reset();
//		edge->setSource(source);
//		edge->setTokenRate(tokenRate);
//		edge->setSink(sink);
//		edge->setRefEdge(refEdge);
//		source->setOutputEdge(edge, sourcePortIx);
//		sink->setInputEdge(edge, sinkPortIx);
//	}
//	return edge;
//}

