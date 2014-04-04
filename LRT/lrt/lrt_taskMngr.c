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

#include <stdio.h>
#include <platform_queue.h>
#include <platform_print.h>
#include <platform.h>
#include "lrt_taskMngr.h"
#include "lrt_debug.h"
#include "lrt_amMngr.h"
#include "lrt_cfg.h"


/*
*********************************************************************************************************
*                                         GLOBAL DECLARATIONS
*********************************************************************************************************
*/

/* GLOBAL VARIABLES */
FUNCTION_TYPE functions_tbl[NB_LOCAL_FUNCTIONS]; /* Table of Action Fcts */

UINT8 OSTaskCntr = 0;                     			// Tasks' counter.
UINT8 OSTaskIndex = 0;                     			// Tasks' index.

static UINT8 workingMemory[WORKING_MEMORY_SIZE];
static UINT8* freeWorkingMemoryPtr;

void clearTCBTbl(){
	OSTaskCntr = 0;
	OSTaskIndex = 0;
}

/*
*********************************************************************************************************
*                                     CREATE A LRT's TASK
*
* Description: Creates a Lrt's task.
*
*********************************************************************************************************
*/

OS_TCB *  LrtTaskCreate (){
	OS_TCB *new_tcb;

	if(OSTaskCntr >= OS_MAX_TASKS){
		platform_puts("Create Task ");platform_putdec(OSTaskIndex);platform_puts("\n");
		exitWithCode(1003);
	}

	// Finding an unused TCB.
	new_tcb = &OSTCBTbl[OSTaskIndex];

	new_tcb->OSTCBState = OS_STAT_READY;/* Reserve the priority to prevent others from doing ...  */

	/* Store task ID */
	new_tcb->OSTCBId = OSTaskIndex++;
	if(OSTaskIndex == (OS_MAX_TASKS)) OSTaskIndex = 0;

	// Incrementing the task counter.
	OSTaskCntr++;

	// Set the current TCB pointer if it is not already done.
	if(OSTCBCur == (OS_TCB*)0){
		/* If no running Task */
		OSTCBCur = new_tcb;
//			new_tcb->OSTCBNext = new_tcb;
//		}else{
//			new_tcb->OSTCBNext = OSTCBCur;
	}
	return new_tcb;
}

#if USE_AM
AM_ACTOR_ACTION_STRUCT* OSCurActionQuery(){
	return &OSTCBCur->am.am_actions[OSTCBCur->am.am_vertices[OSTCBCur->am.currVertexId].actionID];
}
#endif


/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task from the list of available tasks...
*
* Arguments  : id    is the identifier of the task to delete.
* 			   curr_vertex_id is will contain the id. of the current vertex of the deleted task.
*
* Returns    : OS_ERR_NONE             if the call is successful
*              OS_ERR_TASK_NOT_EXIST   if the task you want to delete does not exist.
*
*********************************************************************************************************
*/

void  LrtTaskDeleteCur(){
	UINT8	id = OSTCBCur->OSTCBId;
//    OS_TCB    *del_tcb = &OSTCBTbl[id];

//    if (del_tcb->OSTCBState == OS_STAT_READY) { /* Make sure task doesn't already exist at this id  */
	OSTCBCur->OSTCBState = OS_STAT_UNINITIALIZED;

    	/* Update current running Task List */
//		if(del_tcb->OSTCBNext == del_tcb){
//			OSTCBCur = (OS_TCB*)0;
//			lrt_running = FALSE;
//		}else{
//			OSTCBCur = del_tcb->OSTCBNext;
//		}
	/* Decrement the tasks counter */
	OSTaskCntr--;

    if(OSTaskCntr > 0){
    	id++;
    	if (id < OS_MAX_TASKS )
    		OSTCBCur = &OSTCBTbl[id];
    	else
    		OSTCBCur = &OSTCBTbl[0];
    }
	else{
		OSTCBCur = (OS_TCB*)0;
		lrt_running = FALSE;
    }
}

void  OSTaskDel (){
	UINT8 taskId   = platform_queue_pop_UINT32(PlatformCtrlQueue);
	UINT8 vectorId = platform_queue_pop_UINT32(PlatformCtrlQueue);
    OS_TCB       *ptcb = &OSTCBTbl[taskId];
    ptcb->stop = TRUE;
#if USE_AM
    ptcb->am.stopVertexId = vectorId;
#endif
    platform_puts("Stop Task ID"); platform_putdec(taskId);
    platform_puts(" at Vector ID");  platform_putdec(vectorId);
    platform_puts("\n");
}


void PrintTasksIntoDot(){
	char name[20];
	FILE *pFile;
	UINT32 i, j, k;
	OS_TCB *tcb;
	LRTActor *actor;
	static UINT32 stepCntr = 0;

	sprintf(name, "Slave%d_%d.gv", platform_getCoreId(), stepCntr);
	pFile = fopen (name,"w");
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "digraph Actors {\n");
		fprintf (pFile, "node [color=Black];\n");
		fprintf (pFile, "edge [color=Black];\n");
//				fprintf (pFile, "rankdir=LR;\n");

		j = 0;
		k = 0;
		while(j < OSTaskCntr && k < OS_MAX_TASKS) {
			tcb = &OSTCBTbl[k];
			k++;
			if(tcb != (OS_TCB*)0){
				if(tcb->OSTCBState == OS_STAT_READY){
					j++;
					fprintf (pFile, "\t%d [label=\"Function F%d\\n", j, tcb->functionId);
					actor = &tcb->actor;
					for (i = 0; i < actor->nbInputFifos; i++){
//						fprintf (pFile, "Fin  %d ", actor->inputFifoId[i]);
//						fprintf (pFile, "addr  %d\\n ", actor->inputFifoDataOff[i]);
					}
					for (i = 0; i < actor->nbOutputFifos; i++){
//						fprintf (pFile, "Fout %d ", actor->outputFifoId[i]);
//						fprintf (pFile, "addr  %d\\n ", actor->outputFifoDataOff[i]);
					}
					for(i = 0; i < actor->nbParams; i++)
						fprintf (pFile, "Param %d\\n", actor->params[i]);

					fprintf (pFile, "\",shape=box];\n");
				}
			}
		}
	}
	fprintf (pFile, "}\n");
	fclose (pFile);
	stepCntr++;
}


void OSWorkingMemoryInit(){
	freeWorkingMemoryPtr = workingMemory;
}

void* OSAllocWorkingMemory(int size){
	void* mem;
	if(freeWorkingMemoryPtr-workingMemory+size > WORKING_MEMORY_SIZE){
		platform_puts("Asked ");platform_putdec(size);platform_puts(" bytes, but ");platform_putdec(WORKING_MEMORY_SIZE-(int)freeWorkingMemoryPtr+(int)workingMemory);platform_puts(" bytes available\n");
		exitWithCode(1015);
	}
	mem = freeWorkingMemoryPtr;
	freeWorkingMemoryPtr += size;
	return mem;
}

void OSFreeWorkingMemory(){
	freeWorkingMemoryPtr = workingMemory;
}
