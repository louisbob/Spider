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

#include "Tests.h"
#include <spider.h>

/*******************************************************************************/
/****************************     TEST 0     ***********************************/
/*******************************************************************************/

PiSDFGraph* test0(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);
	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	1,
			/*Params*/	1,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	2,
			stack);

	// Parameters.
//	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
	PiSDFParam *paramN = graph->addStaticParam("N", 3);

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 0,
			/*Par*/ 0, /*Cfg*/ 0);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);

	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 1);
	vxB->addInParam(0, paramN);

	// Edges.
	PiSDFEdge* edge;
	edge = graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "N",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxB->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test0(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test0(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

/*******************************************************************************/
/****************************     TEST 1     ***********************************/
/*******************************************************************************/

PiSDFGraph* test1(Stack* stack, int N){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	1,
			/*Params*/	1,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	1,
			stack);

	// Parameters.
//	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
	PiSDFParam *paramN = graph->addStaticParam("N", N);

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 1,
			/*Par*/ 0, /*Cfg*/ 0);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 1);
	vxA->addInParam(0, paramN);

	// Edges.
	PiSDFEdge* edge;
	edge = graph->connect(
			/*Src*/ vxC, /*SrcPrt*/ 0, /*Prod*/ "3",
			/*Snk*/ vxA, /*SnkPrt*/ 0, /*Cons*/ "N",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test1(Stack* stack, int N){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test1(stack, N));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

/*******************************************************************************/
/****************************     TEST 2     ***********************************/
/*******************************************************************************/

PiSDFGraph* test2(Stack* stack, int N){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	2,
			/*Params*/	1,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	2,
			stack);

	// Parameters.
//	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
	PiSDFParam *paramN = graph->addStaticParam("N", N);

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 1,
			/*Par*/ 0, /*Cfg*/ 0);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 1);
	vxA->addInParam(0, paramN);

	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 2, PISDF_SUBTYPE_NORMAL,
			/*In*/ 2, /*Out*/ 0,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxC, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "N",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test2(Stack* stack, int N){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test2(stack, N));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

/*******************************************************************************/
/****************************     TEST 3     ***********************************/
/*******************************************************************************/

PiSDFGraph* test3(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	3,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	4,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 0, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxF = graph->addBodyVertex(
			"F", /*Fct*/ -1, PISDF_SUBTYPE_FORK,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxF, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 1, /*Prod*/ "1",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test3(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test3(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

/*******************************************************************************/
/****************************     TEST 4     ***********************************/
/*******************************************************************************/

PiSDFGraph* test4(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	3,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	4,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 0, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxJ = graph->addBodyVertex(
			"J", /*Fct*/ -1, PISDF_SUBTYPE_JOIN,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxB, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 1, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxJ, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test4(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test4(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

/*******************************************************************************/
/****************************     TEST 5     ***********************************/
/*******************************************************************************/

PiSDFGraph* test5(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	3,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	4,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 0, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxF = graph->addBodyVertex(
			"F", /*Fct*/ -1, PISDF_SUBTYPE_FORK,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxF, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 1, /*Prod*/ "2",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test5(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test5(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

/*******************************************************************************/
/****************************     TEST 6     ***********************************/
/*******************************************************************************/

PiSDFGraph* test6(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	3,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	4,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 0, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxJ = graph->addBodyVertex(
			"J", /*Fct*/ -1, PISDF_SUBTYPE_JOIN,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxB, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxJ, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test6(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test6(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}


/*******************************************************************************/
/****************************     TEST 7     ***********************************/
/*******************************************************************************/

PiSDFGraph* test7_sub(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	2,
			/*Params*/	0,
			/*InIf*/	1,
			/*OutIf*/	1,
			/*Config*/	0,
			/*Normal*/	1,
			stack);

	// Parameters.

	// Configure vertices

	// Interfaces
	PiSDFVertex *ifIn = graph->addInputIf(
			"in",
			0 /*Par*/);
	PiSDFVertex *ifOut = graph->addOutputIf(
			"out",
			0 /*Par*/);

	// Other vertices
	PiSDFVertex *vxH = graph->addBodyVertex(
			"H", /*Fct*/ -1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ ifIn, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ ifOut, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* test7(Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	2,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	3,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 0, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxH = graph->addBodyVertex(
			"H_top", /*Fct*/ -1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);
	vxH->setSubGraph(test7_sub(stack));
	vxH->getSubGraph()->setParentVertex(vxH);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test7(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test7(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}
