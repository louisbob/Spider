
/****************************************************************************
 * Copyright or � or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-Fran�ois Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include <lrt_1W1RfifoMngr.h>
#include <hwQueues.h>
#include <lrt_core.h>
#include <lrt_taskMngr.h>

static uchar buffer[BUFFER_SIZE];
static struct_VOLsimple VideoObjectLayer_VOLsimple;
static uchar VideoObjectLayer_vop_complexity[5];
static int VideoObjectLayer_pos_o;
static imgDimensionsData outData;

void readVOL(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[]){
	uint nbBytesRead;
	FILE* pFile = NULL;
	long filePosition;
//	OS_TCB* tcb;
//	AM_ACTOR_ACTION_STRUCT* action;

	pFile = fopen(M4V_FILE_PATH, "rb");
	if (pFile == NULL)
	{
	  printf("Cannot open m4v_file file '%s' \n", M4V_FILE_PATH);
	  exit(-1);
	}

	nbBytesRead = 0;

	// Reading Visual Object (I don't know why but there is no VOS).
	readUpToNextStartCode(pFile, buffer, &nbBytesRead);

	// Reading Visual Object Layer.
	readUpToNextStartCode(pFile, buffer, &nbBytesRead);
	VideoObjectLayer(
			BUFFER_START_POSITION,
			nbBytesRead,
			buffer,
			&VideoObjectLayer_VOLsimple,
			VideoObjectLayer_vop_complexity,
			&VideoObjectLayer_pos_o,
			&outData.VideoObjectLayer_xsize_o,
			&outData.VideoObjectLayer_ysize_o);

	// Reading file's current position.
	filePosition = ftell(pFile);

	// Closing video file.
	fclose(pFile);

	/* Sending data */
//	tcb = getCurrTask();
//	action = OSCurActionQuery();
	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VideoObjectLayer_VOLsimple);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], sizeof(uchar) * 5, (UINT8*)VideoObjectLayer_vop_complexity);
	writeFifo(outputFIFOIds[2], outputFIFOAddrs[2], sizeof(long), (UINT8*)&filePosition);
	writeFifo(outputFIFOIds[3], outputFIFOAddrs[3], sizeof(imgDimensionsData), (UINT8*)&outData);
}