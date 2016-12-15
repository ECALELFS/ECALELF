//////////////////////////////////////////////////////////
// Class for the monitoring of laser corrections with E/p
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

class LaserMonitoringEoP
{

public :

	///! List of class methods

	LaserMonitoringEoP(TTree *tree, TTree *treeMC, int useRegression);

	virtual ~LaserMonitoringEoP();

	virtual void     Init(TTree *tree, TTree *treeMC, int useRegression);

	virtual void     Loop(float, float, std::string, int, int, std::string, std::string, std::string, std::string, int);

	virtual void     setLaserPlotStyle();

	TTree          *fChain;   //!pointer to the analyzed TTree or TChain
	TTree          *fChainMC;   //!pointer to the analyzed TTree or TChain
	Int_t           fCurrent; //!current Tree number in a TChain
	Int_t           fCurrentMC; //!current Tree number in a TChain

	///! Declaration of leaf types

	int runNumber;
	int runTime;
	int nPU;
	int nPV;
	float avgLCSCEle[3], etaSCEle[3], phiSCEle[3], energySCEle[3], esEnergySCEle[3], pAtVtxGsfEle[3], energySCEle_corr[3];
	int seedXSCEle[3], seedYSCEle[3];//, seedZside;
	//  float seedLaserAlphaSCEle1;

};

#endif
