//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jul  2 02:30:39 2011 by ROOT version 5.27/06b
// from TTree ntu/ntu
// found on file: /data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root
//////////////////////////////////////////////////////////

#ifndef LaserMonitoringEoP_h
#define LaserMonitoringEoP_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1F.h>
#include <TProfile.h>
#include <vector>
#include "../interface/hChain.h"
#include "../interface/h2Chain.h"
#include <TGraphErrors.h>
#include <TLorentzVector.h>
#include "TVirtualFitter.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TColor.h"
//#include "../../ZFitter/interface/setTDRStyle.h"

#include "../interface/ntpleUtils.h"
#include "../interface/CalibrationUtils.h"
#include "../interface/FastCalibratorEB.h"
#include "../interface/FastCalibratorEE.h"

#include "../interface/TEndcapRings.h"
#include "../CommonTools/histoFunc.h"
//#include "treeReader.h"                                                                                                                                                            
//#include "setTDRStyle.h"                                                                                
#include "../interface/ntpleUtils2.h"
#include "../interface/ConvoluteTemplate.h"
#include "../interface/stabilityUtils.h"
#include "../interface/geometryUtils.h"
//#include "Math/PtEtaPhiE4D.h"
//#include "Math/PtEtaPhiM4D.h"
//#include "Math/LorentzVector.h"

class LaserMonitoringEoP {

  public :

   ///! List of class methods
   
    LaserMonitoringEoP(TTree *tree, TTree *treeMC, int useRegression);
  
    virtual ~LaserMonitoringEoP();
    
    //    virtual Int_t    GetEntry(Long64_t entry);

    //    virtual void     bookHistos(int);

    //    virtual void     saveHistos(TFile *f1);
    
    //    virtual Long64_t LoadTree(Long64_t entry);
    
    virtual void     Init(TTree *tree, TTree *treeMC, int useRegression);
    
    virtual void     Loop(float, float, std::string, int, int, std::string, std::string, std::string, std::string);

    virtual void     setLaserPlotStyle();        
    
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain
    TTree          *fChainMC;   //!pointer to the analyzed TTree or TChain
    Int_t           fCurrent; //!current Tree number in a TChain
    Int_t           fCurrentMC; //!current Tree number in a TChain

    ///! Declaration of leaf types

    int runNumber;
    int runTime;
    int nPU;
    float avgLCSCEle[3], etaSCEle[3], phiSCEle[3], energySCEle[3], esEnergySCEle[3], pAtVtxGsfEle[3], energySCEle_corr[3];
    int seedXSCEle[3], seedYSCEle[3];//, seedZside;    
    //  float seedLaserAlphaSCEle1; 
    

    ///! List of input branches from ntu
    /*    
    TBranch        *b_runNumber;   //!
    TBranch        *b_runTime;  //!
    TBranch        *b_nPU; //!
    TBranch        *b_avgLCSCEle;   //!
    TBranch        *b_etaSCEle;   //!
    TBranch        *b_phiSCEle;   //!
    TBranch        *b_energySCEle;   //!
    TBranch        *b_esEnergySCEle;   //!
    TBranch        *b_pAtVtxGsfEle;   //!
    TBranch        *b_energySCEle_corr;   //!
    TBranch        *b_seedXSCEle;   //!
    TBranch        *b_seedYSCEle;   //!
    */

    ///! Output information 
    //    TGraphErrors *g_ICmeanVsLoop;

};

#endif
