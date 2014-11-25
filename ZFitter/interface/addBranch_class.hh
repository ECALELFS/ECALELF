#ifndef ADDBRANCH_HH
#define ADDBRANCH_HH
/// starting from standard ntuple, produce new tree with additional branches
/**
 * \class addBranch_class addBranch_class.cc Calibration/ZFitter/interface/addBranch_class.hh
 *
 * create tree with following branches:
 * - ZPt
 * - invMassSigma
 * - iSM
 */
/**
   \todo
   - move addbranch_pt to private and correct the usage in ZFitter.cpp
   - move scaler to private
*/

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <TMath.h>
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TBranch.h>
#include <TChain.h>

#include "EnergyScaleCorrection_class.h"

using namespace std;

class addBranch_class{
  
public:
  addBranch_class(void);
  ~addBranch_class(void);

  /// specify the new branch you want in BranchName
  TTree *AddBranch(TChain* originalChain, TChain* secondChain, TString treename, TString BranchName, bool fastLoop=true, bool isMC=false);
  TTree* AddBranch_Pt(TChain* originalTree, TString treename);
  EnergyScaleCorrection_class *scaler;
  TString _commonCut;
  std::vector<TString> _regionList;

	TTree* AddBranch_Map(TChain* originalChain, TChain* secondChain, TString treename, TString BranchName);
private:

  TTree* AddBranch_invMassSigma(TChain* originalChain, TString treename, TString invMassBranchName, bool fastLoop=true, bool isMC=true);
  TTree* AddBranch_iSM(TChain* originalChain, TString treename, TString iSMEleName, bool fastLoop);
  TTree* AddBranch_smearerCat(TChain* originalChain, TString treename, bool isMC);
};




#endif
