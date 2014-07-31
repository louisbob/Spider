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

#include <stdlib.h>
#include <platform_types.h>
#include <platform_queue.h>
#include <platform_time.h>

#include <tools/SchedulingError.h>

#include "grt_definitions.h"
#include "monitor.h"
#include <graphs/SRDAG/SRDAGVertex.h>
#include <spider.h>

static taskTime taskTimes[MAX_SRDAG_VERTICES*ITER_MAX];
static UINT32 nbTaskTime;

void Monitor_init(){
	nbTaskTime = 0;
}

void Monitor_startTask(SRDAGVertex* vertex){
	if(nbTaskTime>=MAX_SRDAG_VERTICES*ITER_MAX-1){
		exitWithCode(1017);
	}
	taskTimes[nbTaskTime].srdagIx = vertex->getId();
	taskTimes[nbTaskTime].globalIx = getGlobalIteration();
	taskTimes[nbTaskTime].type = vertex->getType();
	taskTimes[nbTaskTime].pisdfVertex = vertex->getReference();
	taskTimes[nbTaskTime].iter = vertex->getIterationIndex();
	taskTimes[nbTaskTime].repet = vertex->getReferenceIndex();

//	printf("start task %d vxId %d\n", nbTaskTime, vertexID);
	taskTimes[nbTaskTime].start = platform_time_getValue();
}

void Monitor_endTask(){
	taskTimes[nbTaskTime].end = platform_time_getValue();
	nbTaskTime++;
}

int Monitor_getNB(){
	return nbTaskTime;
}

taskTime Monitor_get(int id){
	return taskTimes[id];
}
