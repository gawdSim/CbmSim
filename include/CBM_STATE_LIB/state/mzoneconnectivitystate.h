/*
 * mzoneconnectivitystate.h
 *
 *  Created on: Nov 21, 2012
 *      Author: consciousness
 */

#ifndef MZONECONNECTIVITYSTATE_H_
#define MZONECONNECTIVITYSTATE_H_

#include <fstream>
#include <iostream>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "fileIO/rawbytesrw.h"
#include "stdDefinitions/pstdint.h"
#include "randGenerators/sfmt.h"
#include "memoryMgmt/dynamic2darray.h"
#include "params/connectivityparams.h"

class MZoneConnectivityState
{
public:
	MZoneConnectivityState();
	MZoneConnectivityState(int randSeed);
	MZoneConnectivityState(std::fstream &infile);
	//MZoneConnectivityState(const MZoneConnectivityState &state);

	~MZoneConnectivityState();

	void writeState(std::fstream &outfile);

	//basket cells
	ct_uint32_t **pBCfromBCtoPC;
	ct_uint32_t **pBCfromPCtoBC;

	//stellate cells
	ct_uint32_t **pSCfromSCtoPC;

	//purkinje cells
	ct_uint32_t **pPCfromBCtoPC;
	ct_uint32_t **pPCfromPCtoBC;
	ct_uint32_t **pPCfromSCtoPC;
	ct_uint32_t **pPCfromPCtoNC;
	ct_uint32_t *pPCfromIOtoPC;

	//nucleus cells
	ct_uint32_t **pNCfromPCtoNC;
	ct_uint32_t **pNCfromNCtoIO;
	ct_uint32_t **pNCfromMFtoNC;

	//inferior olivary cells
	ct_uint32_t **pIOfromIOtoPC;
	ct_uint32_t **pIOfromNCtoIO;
	ct_uint32_t **pIOInIOIO;
	ct_uint32_t **pIOOutIOIO;

private:
	void allocateMemory();
	void initializeVals();
	void deallocMemory();
	void stateRW(bool read, std::fstream &file);
	
	void connectBCtoPC();
	void connectPCtoBC();
	void connectSCtoPC();
	void connectPCtoNC(int randSeed);
	void connectNCtoIO();
	void connectMFtoNC();
	void connectIOtoPC();
	void connectIOtoIO();
};

#endif /* MZONECONNECTIVITYSTATE_H_ */

