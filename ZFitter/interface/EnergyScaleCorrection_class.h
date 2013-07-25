#ifndef EnergyScaleCorrection_class_hh
#define EnergyScaleCorrection_class_hh
#include <TString.h>
#include <iostream>
//#include "../include/root_header.h"
#include<fstream>
#include<map>
#include<math.h>
//#include "correction_class.h"
#include <TChain.h>
#include <TRandom3.h>

typedef std::map < TString, std::pair<double, double> > correction_map_t;


typedef struct {
 int runMax;
 correction_map_t correction_map;
} correction_t;

class EnergyScaleCorrection_class{

 public:
  bool isHggCat;
  bool noCorrections, noSmearings;
  TString correctionType, smearingType;
 public:
  //  EnergyScaleCorrection_class(TString correctionFileName, bool isHggCat_=false);
  EnergyScaleCorrection_class(TString correctionFileName, TString correctionType_, 
			      TString smearingFileName="", TString smearingType_="");
  ~EnergyScaleCorrection_class(void);
  void ReadFromFile(TString filename);
  float getScaleOffset(int runNumber, bool isEBEle, double R9Ele, double etaSCEle); // deprecated
  float ScaleCorrection(int runNumber, bool isEBEle, double R9Ele, double etaSCEle, 
			int nPV, float nPVmean);
  TTree *GetCorrTree(TChain *tree, bool fastLoop=true, 
		     TString runNumberBranchName="runNumber",
		     TString R9EleBranchName="R9Ele",
		     TString etaEleBranchName="etaEle",
		     TString etaSCEleBranchName="etaSCEle",
		     TString nPVBranchName="nPV");

 private:
  TString GetElectronCategory(bool isEBEle, double R9Ele, double etaSCEle);
  float GetMean_nPV(TChain *tree, bool fastLoop, TString nPVBranchName);

  void Add(TString category_, int runMin_, int runMax_, double deltaP_, double err_deltaP_);
  std::map< int, correction_t >::const_iterator runCorrection_itr;
  std::map< int, correction_t >::const_iterator FindRunCorrection_itr(int runNumber);
  std::map< int, correction_t > runMin_map;

  //============================== smearings
 private:
  TRandom3 *rgen_;

  correction_map_t smearings;
  void AddSmearing(TString category_, int runMin_, int runMax_, //double smearing_, double err_smearing_);
		   double constTerm, double err_constTerm, double alpha, double err_alpha);

  float getSmearing(float energy, bool isEBEle, float R9Ele, float etaSCEle);


 public:
  float getSmearingSigma(float energy, bool isEBEle, float R9Ele, float etaSCEle);
  void ReadSmearingFromFile(TString filename);
  TTree *GetSmearTree(TChain *tree, bool fastLoop, 
		      TString energyEleBranchName,
		      TString R9EleBranchName="R9Ele",
		      TString etaEleBranchName="etaEle",
		      TString etaSCEleBranchName="etaSCEle"
		      );

};

#endif
