/*
 * DPCM.cpp
 *
 *  Created on: 5 juil. 2013
 *      Author: yoliva
 */

#include "DPCM.h"
#include "../graphs/PiCSDF/PiCSDFGraph.h"

void createPiCSDF_DPCM_1(PiCSDFGraph* graph){
	// Vertices.
	CSDAGVertex *vertexRead = graph->addVertex("Read");
	CSDAGVertex *vertexRasterToMB = graph->addVertex("RasterToMB");
	CSDAGVertex *vertexDPCM = graph->addVertex("DPCM");
	CSDAGVertex *vertexMBToRaster = graph->addVertex("MBToRaster");
	CSDAGVertex *vertexWrite = graph->addVertex("Write");

	// Configuration vertices.
	CSDAGVertex *vertexSetHW = graph->addConfigVertex("SetHW");
	CSDAGVertex *vertexSetN = graph->addConfigVertex("SetN");
	CSDAGVertex *vertexSetB = graph->addConfigVertex("SetB");

	// Variables.
//	globalParser.addVariable("H", 3);
	globalParser.addVariable("W", 4);
	globalParser.addVariable("N", 1);
//	globalParser.addVariable("B", 1);

	// Parameters.
	PiCSDFParameter* paramH = graph->addParameter("H");
	PiCSDFParameter* paramW = graph->addParameter("W");
	PiCSDFParameter* paramN = graph->addParameter("N");
	PiCSDFParameter* paramB = graph->addParameter("B");

	// Configuration input ports.
	graph->addConfigPort(vertexRead, paramH, 0);
	graph->addConfigPort(vertexRead, paramW, 0);
	graph->addConfigPort(vertexRead, paramB, 0);
	graph->addConfigPort(vertexRasterToMB, paramH, 0);
	graph->addConfigPort(vertexRasterToMB, paramW, 0);
	graph->addConfigPort(vertexRasterToMB, paramB, 0);
	graph->addConfigPort(vertexDPCM, paramN, 0);
	graph->addConfigPort(vertexDPCM, paramB, 0);
	graph->addConfigPort(vertexMBToRaster, paramH, 0);
	graph->addConfigPort(vertexMBToRaster, paramW, 0);
	graph->addConfigPort(vertexMBToRaster, paramB, 0);
	graph->addConfigPort(vertexWrite, paramH, 0);
	graph->addConfigPort(vertexWrite, paramW, 0);
	graph->addConfigPort(vertexWrite, paramB, 0);

	// Configuration output ports.
	graph->addConfigPort(vertexSetHW, paramH, 1);
	graph->addConfigPort(vertexSetHW, paramW, 1);
	graph->addConfigPort(vertexSetN, paramN, 1);
	graph->addConfigPort(vertexSetB, paramB, 1);

	// Edges.
	graph->addEdge(vertexRead, "W*H*B", vertexRasterToMB, "W*H*B", "0");
	graph->addEdge(vertexRasterToMB, "W*H*B", vertexDPCM, "H*B/N", "0");
//	graph->addEdge(vertexDPCM, "H*B/N", vertexDPCM, "H*B/N", "H*B");
	graph->addEdge(vertexDPCM, "H*B/N", vertexMBToRaster, "W*H*B", "0");
	graph->addEdge(vertexMBToRaster, "W*H*B", vertexWrite, "W*H*B", "0");
}
