#ifndef EnergyScaleCorrection_class_hh
#define EnergyScaleCorrection_class_hh
/// Read and get energy scale and smearings from .dat files
/**\class EnergyScaleCorrection_class EnergyScaleCorrection_class.cc Calibration/ZFitter/src/EnergyScaleCorrection_class.cc
 * 
 * 
 */

/** Description Shervin
 */

#include <TString.h>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <TChain.h>
#include <TRandom3.h>
#include <string>
#include <RooRealVar.h>
#include <RooCBShape.h>

using namespace std;

class correctionValue_class{
 public:
  // values
  float scale, scale_err;
  float constTerm, constTerm_err;
  float alpha, alpha_err;
  float Emean, Emean_err;

  correctionValue_class(void){
    scale=1;  scale_err=0;
    constTerm=0; constTerm_err=0;
    alpha=0; alpha_err=0;
    Emean=0; Emean_err=0;
  };
    
  friend ostream& operator << (ostream& os, const correctionValue_class a){
    os <<  " " 
       << a.scale << " +/- " << a.scale_err //<< std::endl
       <<  " " 
       << a.constTerm << " +/- " << a.constTerm_err //<< std::endl
       <<  " " 
       << a.alpha << " +/- " << a.alpha_err
       <<  " " 
       << a.Emean << " +/- " << a.Emean_err;
    return os;
  };
};

class correctionCategory_class{
  // for class definition and ordering
 public:
  unsigned int runmin;
  unsigned int runmax;

 private:
  float r9min;
  float r9max;
  float etmin;
  float etmax;
  float etamin;
  float etamax;

  // name of the category
  //std::string category;


 public:
  inline  correctionCategory_class(const unsigned int runNumber, const float etaEle, const float R9Ele, const float EtEle){
    runmin=runNumber; runmax=runNumber;
    etamin=fabs(etaEle); etamax=fabs(etaEle);
    r9min=R9Ele; r9max=R9Ele;
    etmin=EtEle; etmax=EtEle;
  }

  correctionCategory_class(TString category_); ///< constructor with name of the category according to ElectronCategory_class

  bool operator<(const correctionCategory_class& b) const;

  friend ostream& operator << (ostream& os, const correctionCategory_class a){
    os <<  a.runmin << " " << a.runmax 
       << "\t" << a.etamin << " "<<a.etamax 
       << "\t" << a.r9min << " "<<a.r9max 
       << "\t" << a.etmin << " " << a.etmax;
    return os;
  };
};


//typedef std::map < TString, std::pair<double, double> > correction_map_t;
typedef std::map < correctionCategory_class, correctionValue_class > correction_map_t;

class EnergyScaleCorrection_class{

 public:
  bool noCorrections, noSmearings;

 public:
  EnergyScaleCorrection_class(TString correctionFileName, 
			      TString smearingFileName=""); ///< constructor with correction file names
  ~EnergyScaleCorrection_class(void);

  float getScaleOffset(int runNumber, bool isEBEle, double R9Ele, double etaSCEle, double EtEle); // deprecated
  float ScaleCorrection(int runNumber, bool isEBEle, double R9Ele, double etaSCEle, 
			double EtEle,
			int nPV=0, float nPVmean=0); ///< method to get energy scale corrections
  TTree *GetCorrTree(TChain *tree, bool fastLoop=true, 
		     TString runNumberBranchName="runNumber",
		     TString R9EleBranchName="R9Ele",
		     TString etaEleBranchName="etaEle",
		     TString etaSCEleBranchName="etaSCEle",
		     TString energySCEleBranchName="energySCEle",
		     TString nPVBranchName="nPV");

 private:
  void ReadFromFile(TString filename); ///<   category  "runNumber"   runMin  runMax   deltaP  err_deltaP

  float GetMean_nPV(TChain *tree, bool fastLoop, TString nPVBranchName);
  void Add(TString category_, int runMin_, int runMax_, double deltaP_, double err_deltaP_);

 public:

  //============================== smearings
 private:
  int smearingType_;

  TRandom3 *rgen_;
  correction_map_t scales, scales_not_defined;
  correction_map_t smearings, smearings_not_defined;

  void AddSmearing(TString category_, int runMin_, int runMax_, //double smearing_, double err_smearing_);
		   double constTerm, double err_constTerm, double alpha, double err_alpha, double Emean, double err_Emean);
  float getSmearingSigma(int runNumber, float energy, bool isEBEle, float R9Ele, float etaSCEle); 
  void ReadSmearingFromFile(TString filename); ///< File structure: category constTerm alpha;
 public:
  inline void SetSmearingType(int value){if(value>=0 && value<=1){smearingType_=value;}else{smearingType_=0;}};
  inline void SetSmearingCBAlpha(double value){cut.setVal(value);};
  inline void SetSmearingCBPower(double value){power.setVal(value);};

  float getSmearing(int runNumber, float energy, bool isEBEle, float R9Ele, float etaSCEle);
  float getSmearingRho(int runNumber, float energy, bool isEBEle, float R9Ele, float etaSCEle); ///< public for sigmaE estimate



  TTree *GetSmearTree(TChain *tree, bool fastLoop, 
		      TString energyEleBranchName,
		      TString runNumberBranchName="runNumber",
		      TString R9EleBranchName="R9Ele",
		      TString etaEleBranchName="etaEle",
		      TString etaSCEleBranchName="etaSCEle"
		      );

  //---------- CB parameters
  RooRealVar 	deltaM;
  RooRealVar 	sigma;
  RooRealVar 	cut;
  RooRealVar 	power;
  RooRealVar    x;
  //--------------- BW parameters
  //RooRealVar  mRes;
  //RooRealVar  Gamma;

  //  RooRealVar& invMass_ref;
  //RooBreitWigner bw_pdf;
  RooCBShape resCB;
  RooDataSet *data;
  int index_data;
  // convolution
  //RooFFTConvPdf conv_pdf;

};


#endif
