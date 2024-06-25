/*
 * mzone.h
 *
 *  Created on: Jun 13, 2011
 *      Author: consciousness
 *
 *  MZone class computes the spiking activity for all 'stripe' or 'microzone'
 *  cells, including pc, bc, sc cells, in addition to nc and ios. Computes
 *  the US (see 'setErrDrive' function). Reads in mf spikes from a pointer.
 *  computes pf->pc and mf->nc synaptic plasticity as well.
 */

#ifndef MZONE_H_
#define MZONE_H_

#include <cstdint>

#include "kernels.h"
#include "mzoneactivitystate.h"
#include "mzoneconnectivitystate.h"
#include "sfmt.h"

class MZone {
public:
  MZone();
  MZone(cudaStream_t **stream, MZoneConnectivityState *cs,
        MZoneActivityState *as, int randSeed, uint32_t **apBufGRGPU,
        uint64_t **histGRGPU, int gpuIndStart, int numGPUs);
  ~MZone();

  void writeToState();
  void cpyPFPCSynWCUDA();
  void cpyPFPCWeightStatesCUDA();
  void setErrDrive(float errDriveRelative);
  void updateMFActivities(const uint8_t *actMF);
  void setTrueMFs(const bool *isCollateralMF);

  void calcPCActivities();
  void calcSCActivities();
  void calcBCActivities();
  void calcIOActivities();
  void calcNCActivities();

  void updatePCOut();
  void updateBCPCOut();
  void updateSCPCOut();
  void updateIOOut();
  void updateNCOut();
  void updateMFNCOut();
  void updateMFNCSyn(const uint8_t *histMF, uint32_t t);

  void runPFPCOutCUDA(cudaStream_t **sts, int streamN);
  void runPFPCSumCUDA(cudaStream_t **sts, int streamN);
  void cpyPFPCSumCUDA(cudaStream_t **sts, int streamN);
  void runPFPCSTPCUDA(cudaStream_t **sts, int streamN, uint32_t use_cs,
                      uint32_t use_us);
  void runPFPCGradedPlastCUDA(cudaStream_t **sts, int streamN, uint32_t t);
  void runPFPCBinaryPlastCUDA(cudaStream_t **sts, int streamN, uint32_t t);
  void runPFPCAbbottCascadePlastCUDA(cudaStream_t **sts, int streamN,
                                     uint32_t t);
  void runPFPCMaukCascadePlastCUDA(cudaStream_t **sts, int streamN, uint32_t t);

  void runSumPFSCCUDA(cudaStream_t **sts, int streamN);
  void cpyPFSCSumGPUtoHostCUDA(cudaStream_t **sts, int streamN);

  void runUpdatePFBCSCOutCUDA(cudaStream_t **sts, int streamN);

  void runSumPFBCCUDA(cudaStream_t **sts, int streamN);
  void cpyPFBCSumGPUtoHostCUDA(cudaStream_t **sts, int streamN);

  const uint8_t *exportAPNC();
  const uint8_t *exportAPSC();
  const uint8_t *exportAPBC();
  const uint8_t *exportAPPC();
  const uint8_t *exportAPIO();

  const float *exportVmBC();
  const float *exportVmPC();
  const float *exportVmNC();
  const float *exportVmIO();
  const float *exportgBCPC();
  const float *exportgPFPC();
  const float *exportGREligToState();
  const float *exportPFPCSTPToState();
  const float *exportPFPCWeights();
  const float *exportMFDCNWeights();
  const uint8_t *exportPFPCWeightStates();

  void load_pfpc_weights_from_file(std::fstream &in_file_buf);
  void load_mfdcn_weights_from_file(std::fstream &in_file_buf);

  const uint32_t *exportAPBufBC();
  const uint32_t *exportAPBufPC();
  const uint8_t *exportAPBufIO();
  const uint32_t *exportAPBufNC();

private:
  MZoneConnectivityState *cs;
  MZoneActivityState *as;

  CRandomSFMT0 *randGen;              // host randGen
  curandStateMRG32k3a **mrg32k3aRNGs; // device randGens

  int gpuIndStart;
  int numGPUs;
  int numGRPerGPU;

  unsigned int updatePFPCNumGRPerB;
  unsigned int updatePFPCNumBlocks;

  unsigned int updatePFPCSynWNumGRPerB;
  unsigned int updatePFPCSynWNumBlocks;

  unsigned int updatePFBCSCNumGRPerB;
  unsigned int updatePFBCSCNumBlocks;

  /* ======== not used ====== */
  unsigned int updateGRBCOutNumGRPerR;
  unsigned int updateGRBCOutNumGRRows;

  unsigned int sumGRBCOutNumBCPerB;
  unsigned int sumGRBCOutNumBlocks;
  /* ======== not used ====== */

  float **pfpcSynWRandNums;

  // mossy fiber variables
  const uint8_t *apMFInput;
  // const uint8_t *histMFInput;
  bool *isTrueMF;

  // stellate cell variables
  // host variables
  uint32_t *inputSumPFSCH;
  // end host variables

  // gpu related variables
  uint32_t **inputPFSCGPU;
  size_t *inputPFSCGPUP;
  uint32_t **inputSumPFSCGPU;
  // end gpu related variables
  // end stellate cell variables

  // basket cell variables
  // host variables
  uint32_t *inputSumPFBCH;

  // gpu related variables
  uint32_t **inputPFBCGPU;
  size_t *inputPFBCGPUP;
  uint32_t **inputSumPFBCGPU;
  // end gpu related variables
  // end basket cell variables

  // technically granule
  float **grEligGPU;
  float **pfpcSTPsGPU;

  // purkinje cell variables
  float **pfSynWeightPCGPU;
  float *pfSynWeightPCLinear;
  uint8_t *pfPCSynWeightStatesLinear; // for cascade plasticity only
  uint8_t **pfPCSynWeightStatesGPU;   // for cascade plasticity only
  float **inputPFPCGPU;
  size_t *inputPFPCGPUPitch;
  float **inputSumPFPCMZGPU;
  float *inputSumPFPCMZH;

  uint32_t **apBufGRGPU;
  uint32_t **delayMaskGRGPU;
  uint64_t **histGRGPU;

  // IO cell variables
  float *pfPCPlastStepIO;

  void initCUDA(cudaStream_t **stream);
  void initBCCUDA();
  void initSCCUDA();
  void testReduction();
};

#endif /* MZONE_H_ */
