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

enum ltp_type { LTP, LTD, NO_STEP };

class MZone {
public:
  MZone();
  MZone(MZoneConnectivityState *cs, MZoneActivityState *as, int randSeed,
        uint32_t **apBufGRGPU, uint64_t **histGRGPU, int gpuIndStart,
        int numGPUs);
  ~MZone();

  void writeToState();
  void cpyPFPCSynWCUDA();

  void setErrDrive(float errDriveRelative);
  void updateMFActivities(const uint8_t *actMF);
  void setTrueMFs(bool *isCollateralMF);

  void calcPCActivities();
  void calcSCActivities();
  void calcBCActivities();
  void calcIOActivities(uint32_t ts);
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
  void runPFPCPlastCUDA(cudaStream_t **sts, int streamN, uint32_t t,
                        bool mask = false);

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
  const float *exportPFPCWeights();
  const float *exportMFDCNWeights();

  void reset_weight_steps_ltp();
  void reset_weight_steps_ltd();

  void load_pfpc_weight_mask_from_file(std::fstream &in_file_buf);
  void load_pfpc_weights_from_file(std::fstream &in_file_buf);
  void load_mfdcn_weights_from_file(std::fstream &in_file_buf);

  void save_weight_steps_ltp_to_file(std::fstream &out_file_buf);
  void save_weight_steps_ltd_to_file(std::fstream &out_file_buf);

  const uint32_t *exportAPBufBC();
  const uint32_t *exportAPBufPC();
  const uint8_t *exportAPBufIO();
  const uint32_t *exportAPBufNC();

private:
  MZoneConnectivityState *cs;
  MZoneActivityState *as;

  CRandomSFMT0 *randGen;
  CRandomSFMT0 *ioRandGen;

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

  // purkinje cell variables
  float **pfSynWeightPCGPU;
  float *pfSynWeightPCLinear;
  uint8_t *pfpc_weight_mask_h;
  uint8_t **pfpc_weight_mask_d;
  uint32_t *weight_steps_ltp_h;
  uint32_t **weight_steps_ltp_d;
  uint32_t *weight_steps_ltd_h;
  uint32_t **weight_steps_ltd_d;
  float **inputPFPCGPU;
  size_t *inputPFPCGPUPitch;
  float **inputSumPFPCMZGPU;
  float *inputSumPFPCMZH;

  uint32_t **apBufGRGPU;
  uint32_t **delayMaskGRGPU;
  uint64_t **histGRGPU;

  // IO cell variables
  float *pfPCPlastStepIO;
  enum ltp_type *io_step_type;

  void initCUDA();
  void initBCCUDA();
  void initSCCUDA();
  void testReduction();
};

#endif /* MZONE_H_ */
