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
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "lrt_cfg.h"
#include "lrt_monitor.h"
#include "lrt_msgMngr.h"
#include "lrt_taskMngr.h"

#include <platform.h>
#include <platform_queue.h>
#include <platform_print.h>
#include <platform_gpio.h>
#include <platform_time.h>

#include "lrt_core.h"
#include "lrt_actorMngr.h"

/* GLOBAL VARIABLES */
OS_TCB 		*OSTCBCur;               	/* Pointer to currently scheduled TCB. 		*/
OS_TCB 		OSTCBTbl[OS_MAX_TASKS];		/* Table of TCBs							*/
LRTActor	LRTActorTbl[OS_MAX_TASKS];
BOOL 	lrt_running; 				/* Flag indicating that kernel is running   */
UINT32		clearAfterCompletion;		/* Indicates that the tasks table must be cleared after the execution
											of the current set of tasks   */

void mainLoop(){
//	switchMonitor(CtrlFifoHandling);
	wait_ext_msg();

	switchMonitor(Default);
	if (lrt_running && getSw()){
#ifdef ARM
//		platform_puts("vertex ");platform_putdec(OSTCBCur->am.currVertexId);platform_puts("\n");
#endif

//		if(OSTCBCur->am.currVertexId == OSTCBCur->am.nbVertices-1){
//			printResult();
//			resetMonitor();
//		}

		LRTStartCurrTask();

	}
	if((OSTaskCntr == 0) && (clearAfterCompletion)){
		sendExecData();
		clearTCBTbl();
		clearAfterCompletion = FALSE;
	}
}

/* Initialize the Runtime */
void LRTInit() {
	OSTCBCur = (OS_TCB *) 0;
	memset(OSTCBTbl, 0, sizeof(OSTCBTbl)); /* Clear all the TCBs */

	if(platform_getCoreId() == 0){
		/* Clear all the data FIFOs */
		platform_flushFIFO(-1);
	}

	initMonitor();
	OSWorkingMemoryInit();

	lrt_running = FALSE;
	clearAfterCompletion = FALSE;
}


/*
 * Initialize control queues for communications with the GRT.
 */
void LRTCtrlStart(){
//	releaseMboxMemMx();
	while (TRUE)
		mainLoop();
}


/*
 * Launch the Runtime.
 */
void LRTStart(){
	while(OSTCBCur)
		LRTStartCurrTask();
}

/* Starts the task pointed by the OSTCBCur. */
void LRTStartCurrTask() {

	// Executes the vertex's code.
	MonitorAction Act = switchMonitor(AMManagement);
	if(OSTCBCur->isAM){
		OSTCBCur->task_func(0, 0, 0, 0, 0); // Fifos are not required when calling the AM default function.
	}
	else{
//		time(&rawtime);
//		OSTCBCur->startTime = localtime(&rawtime);
//		printf("Execute fn %d\n", OSTCBCur->functionId);
		OSTCBCur->startTime = platform_time_getValue();
		OSTCBCur->task_func(OSTCBCur->actor->inputFifoId,
							OSTCBCur->actor->inputFifoDataOff,
							OSTCBCur->actor->outputFifoId,
							OSTCBCur->actor->outputFifoDataOff,
							OSTCBCur->actor->params);
//		OSTCBCur->startTime = ((float)nbCpuCyclesStart)/CLOCKS_PER_SEC;
//		OSTCBCur->nbCpuCycles = clock() - nbCpuCyclesStart;
		OSTCBCur->execTime = platform_time_getValue() - OSTCBCur->startTime;
	}

	if(OSTCBCur->stop)
		LrtTaskDeleteCur();
//	am_funct[OSTCBCur->am_vertices[OSTCBCur->am.currVertexId].type]();

//	if(OSTCBCur->stop && (OSTCBCur->am.stopVertexId == -1 || OSTCBCur->am.stopVertexId == OSTCBCur->am.currVertexId)){
//		int temp = OSTCBCur->am.currVertexId;
//		LrtTaskDeleteCur();
//		OS_CtrlQPush(&temp, sizeof(int));
//	}
//	switchMonitor(Act);
}

/* Determine the next Task to run */
void LRTSched() {
	/* Round robin Scheduler between Tasks */
	if (OSTCBCur != (OS_TCB*) 0)
		OSTCBCur = OSTCBCur->OSTCBNext;
	else
		/* No Available Tasks : stop the LRT */
		lrt_running = FALSE;
}


OS_TCB* getCurrTask(){
	return OSTCBCur;
}

