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

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFEdge.h>
#include <string.h>

/** Static Var def */
int PiSDFEdge::globalId = 0;

PiSDFEdge::PiSDFEdge(
		PiSDFGraph* graph,
		Stack* stack){
	id_ = globalId++;
	graph_ = graph;

	src_ = 0; srcPortIx_ = -1;
	snk_ = 0; snkPortIx_ = -1;

	prod_ = cons_ = delay_ = 0;
	setter_ = 0;

	stack_ = stack;
}

PiSDFEdge::~PiSDFEdge(){
	if(delay_ != 0){
		delay_->~Expression();
		stack_->free(delay_);
		delay_ = 0;
	}
	if(prod_ != 0){
		prod_->~Expression();
		stack_->free(prod_);
		prod_ = 0;
	}
	if(cons_ != 0){
		cons_->~Expression();
		stack_->free(cons_);
		cons_ = 0;
	}

}

void PiSDFEdge::connectSrc(PiSDFVertex *src, int srcPortId, const char *prod, Stack* stack){
	if(src_ != 0)
		throw "PiSDFEdge: try to connect to an already connected edge";
	src_ = src;
	srcPortIx_ = srcPortId;

	if(prod_ != 0){
		prod_->~Expression();
		stack->free(prod_);
		prod_ = 0;
	}
	prod_ = CREATE(stack, Expression)(prod, src->getInParams(), src->getNInParam(), stack);
}

void PiSDFEdge::connectSnk(PiSDFVertex *snk, int snkPortId, const char *cons, Stack* stack){
	if(snk_ != 0)
		throw "PiSDFEdge: try to connect to an already connected edge";
	snk_ = snk;
	snkPortIx_ = snkPortId;

	if(cons_ != 0){
		cons_->~Expression();
		stack->free(cons_);
		cons_ = 0;
	}
	cons_ = CREATE(stack, Expression)(cons, snk->getInParams(), snk->getNInParam(), stack);
}







