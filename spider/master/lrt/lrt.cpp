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

#include <lrt.h>
#include <Message.h>

#include <cstring>

#include "specialActors/specialActors.h"

static lrtFct specialActors[6] = {
		&saBroadcast,
		&saFork,
		&saJoin,
		&saRoundbuffer,
		&saInit,
		&saEnd
};

LRT::LRT(Communicator* com){
	com_ = com;
	/* TODO add some heapMemory */
	fcts_ = 0;
	nFct_ = 0;
}
LRT::~LRT(){

}

void LRT::setFctTbl(const lrtFct fct[], int nFct){
	fcts_ = fct;
	nFct_ = nFct;
}

int LRT::runOneJob(){
	void* msg;
	if(com_->recv(0, &msg)){
		switch(((UndefinedMsg*) msg)->msgIx){
		case MSG_START_JOB:{
			StartJobMsg* jobMsg = (StartJobMsg*) msg;
			Fifo *inFifos = (Fifo*) ((char*)jobMsg + 1*sizeof(StartJobMsg));
			Fifo *outFifos = (Fifo*) ((char*)inFifos + jobMsg->nbInEdge*sizeof(Fifo));
			Param *inParams = (Param*) ((char*)outFifos + jobMsg->nbOutEdge*sizeof(Fifo));

			{
				void* inFifosAlloc[jobMsg->nbInEdge];
				void* outFifosAlloc[jobMsg->nbOutEdge];
				Param outParams[jobMsg->nbOutParam];

				for(int i=0; i<jobMsg->nbInEdge; i++){
					inFifosAlloc[i] = (void*) com_->recvData(&inFifos[i]);
				}

				for(int i=0; i<jobMsg->nbOutEdge; i++){
					outFifosAlloc[i] = (void*) com_->pre_sendData(&outFifos[i]);
				}

				if(jobMsg->specialActor && jobMsg->fctIx < 6)
					specialActors[jobMsg->fctIx](inFifosAlloc, outFifosAlloc, inParams, outParams);
				else if(jobMsg->fctIx < nFct_)
					fcts_[jobMsg->fctIx](inFifosAlloc, outFifosAlloc, inParams, outParams);
				else
					throw "Cannot find actor function\n";

				for(int i=0; i<jobMsg->nbOutEdge; i++){
					com_->sendData(&outFifos[i]);
				}
				for(int i=0; i<jobMsg->nbOutParam; i++){
					ParamValueMsg* msgParam = (ParamValueMsg*) com_->alloc(sizeof(ParamValueMsg)+jobMsg->nbOutParam*sizeof(Param));
					Param* params = (Param*)(msgParam+1);

					msgParam->msgIx = MSG_PARAM_VALUE;
					msgParam->srdagIx = jobMsg->srdagIx;
					memcpy(params, outParams, jobMsg->nbOutParam*sizeof(Param));

					com_->send(0);
				}
			}
			break;
		}
		case MSG_CLEAR_TIME:
			Platform::get()->rstTime();
			break;
		case MSG_PARAM_VALUE:
		default:
			throw "Unexpected message received\n";
			break;
		}
		return 1;
	}
	return 0;
}

void LRT::runUntilNoMoreJobs(){
	while(runOneJob());
}

void LRT::runInfinitly(){
	do{
		runOneJob();
	}while(1);
}
