/*
 * ecmfpopulation.h
 *
 *  Created on: Jul 11, 2014
 *      Author: consciousness
 */

#ifndef ECMFPOPULATION_H_
#define ECMFPOPULATION_H_

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "activityparams.h"
#include "connectivityparams.h"
#include "mzone.h"
#include "randomc.h"
#include "sfmt.h"

enum mf_type { BKGD, CS };

class ECMFPopulation {
public:
  ECMFPopulation();
  ECMFPopulation(std::fstream &infile);

  ~ECMFPopulation();

  void writeToFile(std::fstream &outfile);
  void writeMFLabels(std::string labelFileName);

  const float *getBGFreq();
  const float *getCSFreq();

  const bool *getCSIds();

  const bool *getCollIds();

  void calcGammaActivity(enum mf_type type, MZone **mZoneList);
  const uint8_t *getAPs();

private:
  void allocateMemory();
  /* frequency pop functions */
  void setMFs(int numTypeMF, int numMF, CRandomSFMT0 &randGen, bool *isAny,
              bool *isType);

  /* poisson gen functions */
  void prepCollaterals(int rSeed);
  /* frequency population variables */

  float *mfFreqBG;
  float *mfFreqCS;

  bool *isCS;
  bool *isAny;
  bool *isColl;

  int randSeed = 3;
  float fracCS = 0.01;
  float fracColl = 0.02;
  float bgFreqMin = 10.0;
  float csFreqMin = 100.0;
  float bgFreqMax = 30.0;
  float csFreqMax = 110.0;

  /* poisson spike generator vars */
  uint32_t nThreads = 1;   // hard-coded
  uint32_t kappa = 4;      // the other parameter in a gamma dist
  uint32_t num_spike_mask = (1 << kappa) - 1;
  uint32_t numZones = 1;

  float sPerTS = msPerTimeStep / 1000;
  float noiseSigma = 0;
  bool turnOnColls = true;

  CRandomSFMT0 *randSeedGen; // the uber seed
  CRandomSFMT0 **randGens;   // diff randgens per thread, if use openmp
  std::normal_distribution<float> *normDist; // for noise
  std::mt19937 *noiseRandGen;                // for noise

  uint8_t *aps;
  uint32_t *apBufs;

  uint32_t *dnCellIndex;
  uint32_t *mZoneIndex;
};

#endif /* ECMFPOPULATION_H_ */
