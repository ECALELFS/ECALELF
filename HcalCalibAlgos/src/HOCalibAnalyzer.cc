// -*- C++ -*-
//
// Package:    HOCalibAnalyzer
// Class:      HOCalibAnalyzer
// 
/**\class HOCalibAnalyzer HOCalibAnalyzer.cc Calibration/HOCalibAnalyzer/src/HOCalibAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Gobinda Majumder
//         Created:  Sat Jul  7 09:51:31 CEST 2007
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HcalCalibObjects/interface/HOCalibVariables.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TProfile.h"
#include "TPostScript.h"
#include "TCanvas.h"
#include "TF1.h"


#include <string>

#include <iostream>
#include <fstream>
#include <iomanip>
//#include <sstream>


using namespace std;
using namespace edm;


  //
  //  Look for nearby pixel through eta, phi informations for pixel cross-talk
  // 1. Look PIXEL code from (eta,phi)
  // 2. Go to nearby pixel code 
  // 3. Come back to (eta,phi) from pixel code
  // Though it works, it is a very ugly/crude way to get cross talk, need better algorithms
  //

static const int mapx1[6][3]={{1,4,8}, {12,7,3}, {5,9,13}, {11,6,2}, {16,15,14}, {19,18,17}}; 
//    static const int etamap1[21]={-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, 5,5,5, -1};
//  static const int phimap1[21]={-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0, 2,1,0,-1};

static const int mapx2[6][3]={{1,4,8}, {12,7,3}, {5,9,13}, {11,6,2}, {16,15,14}, {-1,-1,-1}};
//  static const int etamap2[21]={-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, -1,-1,-1, -1};
//  static const int phimap2[21]={-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0,  2, 1, 0, -1};

static const int mapx0p[9][2]={{3,1}, {7,4}, {6,5},  {12,8}, {0,0}, {11,9}, {16,13}, {15,14}, {19,17}};
static const int mapx0m[9][2]={{17,19}, {14,15}, {13,16}, {9,11}, {0,0}, {8,12}, {5,6}, {4,7}, {1,3}};

//  static const int etamap0p[21]={-1, 0,-1,0, 1,2,2, 1,3,5, -1, 5,3,6, 7,7,6, 8,-1,8, -1};
//  static const int phimap0p[21]={-1, 1,-1,0, 1,1,0, 0,1,1, -1, 0,0,1, 1,0,0, 1,-1,0, -1};

//  static const int etamap0m[21]={-1, 8,-1,8, 7,6,6, 7,5,3, -1, 3,5,2, 1,1,2, 0,-1,0, -1};
//  static const int phimap0m[21]={-1, 0,-1,1, 0,0,1, 1,0,0, -1, 1,1,0, 0,1,1, 0,-1,1, -1};

static const int etamap[4][21]={{-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, -1,-1,-1, -1}, //etamap2
		   {-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, 5,5,5, -1},    //etamap1 
		   {-1, 0,-1,0, 1,2,2, 1,3,5, -1, 5,3,6, 7,7,6, 8,-1,8, -1},  //etamap0p
		   {-1, 8,-1,8, 7,6,6, 7,5,3, -1, 3,5,2, 1,1,2, 0,-1,0, -1}}; //etamap0m

static const int phimap[4][21] ={{-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0, 2,1,0, -1},    //phimap2
		    {-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0, 2,1,0, -1},    //phimap1
		    {-1, 1,-1,0, 1,1,0, 0,1,1, -1, 0,0,1, 1,0,0, 1,-1,0, -1},  //phimap0p
		    {-1, 0,-1,1, 0,0,1, 1,0,0, -1, 1,1,0, 0,1,1, 0,-1,1, -1}};  //phimap0m
//swapped phi map for R0+/R0- (15/03/07)  
//for (int i=0; i<4; i++) {
//  for (int j=0; j<21; j++) {
//    cout <<"ieta "<<i<<" "<<j<<" "<<etamap[i][j]<<endl;
//  }
//}

// Character convention for R+/-1/2
//      static const int npixleft[21] = {-1, F, Q,-1, M, D, J,-1, T,-1, C,-1, R, P, H,-1, N, G};
//      static const int npixrigh[21] = { Q, S,-1, D, J, L,-1, K,-1, E,-1, P, H, B,-1, G, A,-1};

//      static const int npixlb1[21]={-1,-1,-1,-1, F, Q, S,-1, M, J, L, T, K,-1, C, R, P, H};
//      static const int npixrb1[21]={-1,-1,-1, F, Q, S,-1, M, D, L,-1, K,-1, C, E, P, H, B};
//      static const int npixlu1[21]={ M, D, J, T, K,-1, C,-1, R, H, B,-1, N, G, A,-1,-1,-1};
//      static const int npixru1[21]={ D, J, L, K,-1, C, E, R, P, B,-1, N, G, A,-1,-1,-1,-1};

static const int npixleft[21]={0, 0, 1, 2, 0, 4, 5, 6, 0, 8, 0, 0,11, 0,13,14,15, 0,17,18,0};
static const int npixrigh[21]={0, 2, 3, 0, 5, 6, 7, 0, 9, 0, 0,12, 0,14,15,16, 0,18,19, 0,0};
static const int npixlebt[21]={0, 0, 0, 0, 0, 1, 2, 3, 0, 4, 0, 6, 7, 8, 9, 0,11,13,14,15,0};
static const int npixribt[21]={0, 0, 0, 0, 1, 2, 3, 0, 4, 5, 0, 7, 0, 9, 0,11,12,14,15,16,0};
static const int npixleup[21]={0, 4, 5, 6, 8, 9, 0,11, 0,13, 0,15,16, 0,17,18,19, 0, 0, 0,0};
static const int npixriup[21]={0, 5, 6, 7, 9, 0,11,12,13,14, 0,16, 0,17,18,19, 0, 0, 0, 0,0};
  


//#define CORREL
//
// class decleration
//

  Double_t gausX(Double_t* x, Double_t* par){
  return par[0]*(TMath::Gaus(x[0], par[1], par[2]));
  }

  Double_t landX(Double_t* x, Double_t* par) {
  return par[0]*(TMath::Landau(x[0], par[1], par[2]));
  }

  Double_t completefit(Double_t* x, Double_t* par) {
  return gausX(x, par) + landX(x, &par[3]);
  }

Double_t langaufun(Double_t *x, Double_t *par) {

   //Fit parameters:
   //par[0]=Width (scale) parameter of Landau density
   //par[1]=Most Probable (MP, location) parameter of Landau density
   //par[2]=Total area (integral -inf to inf, normalization constant)
   //par[3]=Width (sigma) of convoluted Gaussian function
   //
   //In the Landau distribution (represented by the CERNLIB approximation), 
   //the maximum is located at x=-0.22278298 with the location parameter=0.
   //This shift is corrected within this function, so that the actual
   //maximum is identical to the MP parameter.

      // Numeric constants
      Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
      Double_t mpshift  = -0.22278298;       // Landau maximum location

      // Control constants
      Double_t np = 100.0;      // number of convolution steps
      Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

      // Variables
      Double_t xx;
      Double_t mpc;
      Double_t fland;
      Double_t sum = 0.0;
      Double_t xlow,xupp;
      Double_t step;
      Double_t i;


      // MP shift correction
      mpc = par[1] - mpshift * par[0]; 

      // Range of convolution integral
      xlow = x[0] - sc * par[3];
      xupp = x[0] + sc * par[3];

      step = (xupp-xlow) / np;

      // Convolution integral of Landau and Gaussian by sum
      for(i=1.0; i<=np/2; i++) {
         xx = xlow + (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);

         xx = xupp - (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);
      }

      //      cout <<"land "<< par[1]<<" "<<sum<<endl;

      return (par[2] * step * sum * invsq2pi / par[3]);
}

  Double_t totalfunc(Double_t* x, Double_t* par){
  return gausX(x, par) + langaufun(x, &par[3]);
  }


class HOCalibAnalyzer : public edm::EDAnalyzer {
   public:
      explicit HOCalibAnalyzer(const edm::ParameterSet&);
      ~HOCalibAnalyzer();


   private:

      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      TFile* theFile;
      std::string theRootFileName;
      std::string theoutputtxtFile;
      std::string theoutputpsFile;

      bool m_hotime;
      bool m_hbtime;
      bool m_correl;
      bool m_checkmap;
      bool m_hbinfo;
      bool m_combined;
      bool m_constant;

  /*
      Double_t langaufun(Double_t *x, Double_t *par);

  Double_t gausX(Double_t* x, Double_t* par){
  return par[0]*(TMath::Gaus(x[0], par[1], par[2]));
  }

  Double_t totalfunc(Double_t* x, Double_t* par){
  return gausX(x, par) + langaufun(x, &par[3]);
  }


  Double_t landX(Double_t* x, Double_t* par) {
  return par[0]*(TMath::Landau(x[0], par[1], par[2]));
  }

  Double_t completefit(Double_t* x, Double_t* par) {
  return gausX(x, par) + landX(x, &par[3]);
  }
  */


  static const int ncut = 11;
  static const int netamx=32;
  static const int nphimx =72;
  int ipass;
 
  TTree* T1;

  TH1F* muonnm;
  TH1F* muonmm;
  TH1F* muonth;
  TH1F* muonph;    
  TH1F* muonch;    

  TH1F* sel_muonnm;
  TH1F* sel_muonmm;
  TH1F* sel_muonth;
  TH1F* sel_muonph;    
  TH1F* sel_muonch; 

  //  if (m_hotime) { // #ifdef HOTIME
  TProfile* hotime[netamx][nphimx];
  TProfile* hopedtime[netamx][nphimx];
  //  } //  if (m_hotime) #endif
  //  if (m_hbtime) { // #ifdef HBTIME
  TProfile* hbtime[netamx][nphimx];
  //  } // m_hbtime #endif

  //  double pedval[netamx][nphimx]; // ={0};

  //  if (m_correl) { // #ifdef CORREL
  TH1F* corrsglb[netamx][nphimx];
  TH1F* corrsgrb[netamx][nphimx];
  TH1F* corrsglu[netamx][nphimx];
  TH1F* corrsgru[netamx][nphimx];
  TH1F* corrsgall[netamx][nphimx];

  TH1F* corrsgl[netamx][nphimx];
  TH1F* corrsgr[netamx][nphimx];

  TH1F* mncorrsglb;
  TH1F* mncorrsgrb;
  TH1F* mncorrsglu;
  TH1F* mncorrsgru;
  TH1F* mncorrsgall;

  TH1F* mncorrsgl;
  TH1F* mncorrsgr;

  TH1F* rmscorrsglb;
  TH1F* rmscorrsgrb;
  TH1F* rmscorrsglu;
  TH1F* rmscorrsgru;
  TH1F* rmscorrsgall;

  TH1F* rmscorrsgl;
  TH1F* rmscorrsgr;

  TH1F* nevcorrsglb;
  TH1F* nevcorrsgrb;
  TH1F* nevcorrsglu;
  TH1F* nevcorrsgru;
  TH1F* nevcorrsgall;

  TH1F* nevcorrsgl;
  TH1F* nevcorrsgr;

  //  } //m_correl #endif
  //  if (m_checkmap) { // #ifdef CHECKMAP
  TH1F* corrsgc[netamx][nphimx];
  TH1F* mncorrsgc;
  TH1F* rmscorrsgc;
  TH1F* nevcorrsgc;

  //  } //m_checkmap #endif

  TH1F* sigrsg[netamx][nphimx];
  TH1F* crossg[netamx][nphimx];

  TH1F* mnsigrsg;
  TH1F* mncrossg;

  TH1F* rmssigrsg;
  TH1F* rmscrossg;

  TH1F* nevsigrsg;
  TH1F* nevcrossg;

  TH1F* ho_sig2p[9];
  TH1F* ho_sig1p[9];
  TH1F* ho_sig00[9];
  TH1F* ho_sig1m[9];
  TH1F* ho_sig2m[9];

  //  if (m_hbinfo) { // #ifdef HBINFO
  TH1F* hbhe_sig[9];
  //  } // m_hbinfo #endif


  //  if (m_combined) { //#ifdef COMBINED
  static const int ringmx=5;
  static const int sectmx=12;
  //  if (m_hotime) { // #ifdef HOTIME
  TProfile* com_hotime[ringmx][sectmx];
  TProfile* com_hopedtime[ringmx][sectmx];
  //  } //m_hotime #endif
  //  if (m_hbtime) { #ifdef HBTIME
  TProfile* com_hbtime[ringmx][sectmx];
    //  } // m_hbtime #endif
  //  if (m_correl) { //#ifdef CORREL
  TH1F* com_corrsglb[ringmx][sectmx];
  TH1F* com_corrsgrb[ringmx][sectmx];
  TH1F* com_corrsglu[ringmx][sectmx];
  TH1F* com_corrsgru[ringmx][sectmx];
  TH1F* com_corrsgall[ringmx][sectmx];

  TH1F* com_corrsgl[ringmx][sectmx];
  TH1F* com_corrsgr[ringmx][sectmx];
  //  } //m_correl #endif
  //  if (m_checkmap) { #ifdef CHECKMAP
  TH1F* com_corrsgc[ringmx][sectmx];
  //  } // m_checkmap #endif

  TH1F* com_sigrsg[ringmx][sectmx];
  TH1F* com_crossg[ringmx][sectmx];

  //  } //m_combined #endif

  TProfile* sigvsevt[15][11];

  int   irun, ievt, itrg1, itrg2, isect, nrecht, nfound, nlost, ndof, nmuon;
  float trkvx, trkvy, trkvz, trkmm, trkth, trkph, chisq, therr, pherr, hodx, hody, 
    hoang, hosig[9], hocorsig[18], hocro, hbhesig[9];

      // ----------member data ---------------------------

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HOCalibAnalyzer::HOCalibAnalyzer(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed
  ipass = 0;

  
  theRootFileName = iConfig.getUntrackedParameter<string>("RootFileName", "test.root");
  theoutputtxtFile = iConfig.getUntrackedParameter<string>("txtFileName", "test.txt");
  theoutputpsFile = iConfig.getUntrackedParameter<string>("psFileName", "test.ps");

  m_hbinfo = iConfig.getUntrackedParameter<bool>("hbinfo", false);
  m_hbtime = iConfig.getUntrackedParameter<bool>("hbtime", false);
  m_hotime = iConfig.getUntrackedParameter<bool>("hotime", false);
  m_correl = iConfig.getUntrackedParameter<bool>("correl", false);
  m_checkmap = iConfig.getUntrackedParameter<bool>("checkmap", false);
  m_combined = iConfig.getUntrackedParameter<bool>("combined", false);
  m_constant = iConfig.getUntrackedParameter<bool>("get_constant", false);

  theFile = new TFile(theRootFileName.c_str(), "RECREATE");
  theFile->cd();
  
  T1 = new TTree("T1", "DT+CSC+HO");
  
  T1->Branch("irun",&irun,"irun/I");  
  T1->Branch("ievt",&ievt,"ievt/I");  
  T1->Branch("itrg1",&itrg1,"itrg1/I");  
  T1->Branch("itrg2",&itrg2,"itrg2/I");  

  T1->Branch("isect",&isect,"isect/I"); 
  T1->Branch("nrecht",&nrecht,"nrecht/I"); 
  T1->Branch("ndof",&ndof,"ndof/I"); 
  T1->Branch("nmuon",&nmuon,"nmuon/I"); 
  
  T1->Branch("trkvx",&trkvx,"trkvx/F");
  T1->Branch("trkvy",&trkvy,"trkvy/F");
  T1->Branch("trkvz",&trkvz,"trkvz/F");
  T1->Branch("trkmm",&trkmm,"trkmm/F");
  T1->Branch("trkth",&trkth,"trkth/F");
  T1->Branch("trkph",&trkph,"trkph/F");

  T1->Branch("chisq",&chisq,"chisq/F");
  T1->Branch("therr",&therr,"therr/F");
  T1->Branch("pherr",&pherr,"pherr/F");
  T1->Branch("hodx",&hodx,"hodx/F");
  T1->Branch("hody",&hody,"hody/F");
  T1->Branch("hoang",&hoang,"hoang/F");

  T1->Branch("hosig",hosig,"hosig[9]/F");
  T1->Branch("hocro",&hocro,"hocro/F");
  T1->Branch("hocorsig",hocorsig,"hocorsig[18]/F");

  if (m_hbinfo) { // #ifdef HBINFO
    T1->Branch("hbhesig",hbhesig,"hbhesig[9]/F");
  } //m_hbinfo #endif

  muonnm = new TH1F("muonnm", "No of muon", 10, -0.5, 9.5);
  muonmm = new TH1F("muonmm", "P_{mu}", 200, -100., 100.);
  muonth = new TH1F("muonth", "{Theta}_{mu}", 180, 0., 180.);
  muonph = new TH1F("muonph", "{Phi}_{mu}", 180, -180., 180.);
  muonch = new TH1F("muonch", "{chi^2}/ndf", 100, 0., 1000.);

  sel_muonnm = new TH1F("sel_muonnm", "No of muon(sel)", 10, -0.5, 9.5);
  sel_muonmm = new TH1F("sel_muonmm", "P_{mu}(sel)", 200, -100., 100.);
  sel_muonth = new TH1F("sel_muonth", "{Theta}_{mu}(sel)", 180, 0., 180.);
  sel_muonph = new TH1F("sel_muonph", "{Phi}_{mu}(sel)", 180, -180., 180.);
  sel_muonch = new TH1F("sel_muonch", "{chi^2}/ndf(sel)", 100, 0., 1000.);



  char title[200];

  for (int i=0; i<15; i++) {
    
    sprintf(title, "sigvsndof_ring%i", i+1); 
    sigvsevt[i][0] = new TProfile(title, title, 50, 0., 50.,-9., 20.);
    
    sprintf(title, "sigvschisq_ring%i", i+1); 
    sigvsevt[i][1] = new TProfile(title, title, 50, 0., 30.,-9., 20.);
    
    sprintf(title, "sigvsth_ring%i", i+1); 
    sigvsevt[i][2] = new TProfile(title, title, 50, .7, 2.4,-9., 20.);
    
    sprintf(title, "sigvsph_ring%i", i+1); 
    sigvsevt[i][3] = new TProfile(title, title, 50, -2.4, -0.7,-9., 20.);
    
    sprintf(title, "sigvstherr_ring%i", i+1); 
    sigvsevt[i][4] = new TProfile(title, title, 50, 0., 0.2,-9., 20.);
      
    sprintf(title, "sigvspherr_ring%i", i+1); 
    sigvsevt[i][5] = new TProfile(title, title, 50, 0., 0.2,-9., 20.);
    
    sprintf(title, "sigvsdircos_ring%i", i+1); 
    sigvsevt[i][6] = new TProfile(title, title, 50, 0.5, 1.,-9., 20.);
    
    sprintf(title, "sigvstrkmm_ring%i", i+1); 
    sigvsevt[i][7] = new TProfile(title, title, 50, 0., 50.,-9., 20.);
    
    sprintf(title, "sigvsnmuon_ring%i", i+1); 
    sigvsevt[i][8] = new TProfile(title, title, 5, 0.5, 5.5,-9., 20.);
    
    sprintf(title, "sigvserr_ring%i", i+1); 
    sigvsevt[i][9] = new TProfile(title, title, 50, 0., .3, -9., 20.);

    sprintf(title, "sigvsacc_ring%i", i+1); 
    sigvsevt[i][10] = new TProfile(title, title, 1000, -0.5, 999.5, -9., 20.);
    
  }

  for (int j=0; j<netamx; j++) {
    int ieta = (j<15) ? j+1 : 14-j;

    for (int i=0;i<nphimx;i++) {
      if (m_hotime) { //#ifdef HOTIME
	sprintf(title, "hotime_eta%i_phi%i", (j<=14) ? j+1 : 14-j, i+1);
	hotime[j][i] = new TProfile(title, title, 10, -0.5, 9.5, -1.0, 30.0);
	
	sprintf(title, "hopedtime_eta%i_phi%i", (j<=14) ? j+1 : 14-j, i+1);
	hopedtime[j][i] = new TProfile(title, title, 10, -0.5, 9.5, -1.0, 30.0);
	
      } //m_hotime #endif
      if (m_hbtime) { //#ifdef HBTIME
	sprintf(title, "hbtime_eta%i_phi%i", (j<=15) ? j+1 : 15-j, i+1);
	hbtime[j][i] = new TProfile(title, title, 10, -0.5, 9.5, -1.0, 30.0);
      } //m_hbtime #endif
      sprintf(title, "sigrsg_eta%i_phi%i", ieta,i+1);
      sigrsg[j][i] = new TH1F(title, title, 31,-10.5,20.5);

      sprintf(title, "crossg_eta%i_phi%i", ieta,i+1);
      crossg[j][i] = new TH1F(title, title, 31,-10.5,20.5);   

      if (m_correl) { //#ifdef CORREL    
	sprintf(title, "corrsg_eta%i_phi%i_leftbottom", ieta,i+1);
	corrsglb[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	sprintf(title, "corrsg_eta%i_phi%i_rightbottom", ieta,i+1);
	corrsgrb[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	sprintf(title, "corrsg_eta%i_phi%i_leftup", ieta,i+1);
	corrsglu[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	sprintf(title, "corrsg_eta%i_phi%i_rightup", ieta,i+1);
	corrsgru[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	sprintf(title, "corrsg_eta%i_phi%i_all", ieta,i+1);
	corrsgall[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	sprintf(title, "corrsg_eta%i_phi%i_left", ieta,i+1);
	corrsgl[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	sprintf(title, "corrsg_eta%i_phi%i_right", ieta,i+1);
	corrsgr[j][i] = new TH1F(title, title, 31,-10.5,20.5);
      } //m_correl #endif
      if (m_checkmap) {// #ifdef CHECKMAP    
	sprintf(title, "corrsg_eta%i_phi%i_centrl", ieta,i+1);
	corrsgc[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
      } //m_checkmap #endif
    }
  }

  mnsigrsg = new TH1F("mnsigrsg","mnsigrsg", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
  rmssigrsg = new TH1F("rmssigrsg","rmssigrsg", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
  nevsigrsg = new TH1F("nevsigrsg","nevsigrsg", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);  

  mncrossg = new TH1F("mncrossg","mncrossg", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
  rmscrossg = new TH1F("rmscrossg","rmscrossg", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
  nevcrossg = new TH1F("nevcrossg","nevcrossg", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);  

  if (m_correl) { //#ifdef CORREL    
    mncorrsglb = new TH1F("mncorrsglb","mncorrsglb", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsglb = new TH1F("rmscorrsglb","rmscorrsglb", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsglb = new TH1F("nevcorrsglb","nevcorrsglb", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    
    mncorrsgrb = new TH1F("mncorrsgrb","mncorrsgrb", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsgrb = new TH1F("rmscorrsgrb","rmscorrsgrb", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsgrb = new TH1F("nevcorrsgrb","nevcorrsgrb", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);

    mncorrsglu = new TH1F("mncorrsglu","mncorrsglu", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsglu = new TH1F("rmscorrsglu","rmscorrsglu", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsglu = new TH1F("nevcorrsglu","nevcorrsglu", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    
    mncorrsgru = new TH1F("mncorrsgru","mncorrsgru", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsgru = new TH1F("rmscorrsgru","rmscorrsgru", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsgru = new TH1F("nevcorrsgru","nevcorrsgru", netamx*nphimx+60, -0.5, netamx*nphimx+59.5); 
    
    mncorrsgall = new TH1F("mncorrsgall","mncorrsgall", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsgall = new TH1F("rmscorrsgall","rmscorrsgall", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsgall = new TH1F("nevcorrsgall","nevcorrsgall", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    
    mncorrsgl = new TH1F("mncorrsgl","mncorrsgl", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsgl = new TH1F("rmscorrsgl","rmscorrsgl", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsgl = new TH1F("nevcorrsgl","nevcorrsgl", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);  
    
    mncorrsgr = new TH1F("mncorrsgr","mncorrsgr", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsgr = new TH1F("rmscorrsgr","rmscorrsgr", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsgr = new TH1F("nevcorrsgr","nevcorrsgr", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);  
  } //m_correl #endif
  
  if (m_checkmap) { //#ifdef CHECKMAP    
    mncorrsgc = new TH1F("mncorrsgc","mncorrsgc", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    rmscorrsgc = new TH1F("rmscorrsgc","rmscorrsgc", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);
    nevcorrsgc = new TH1F("nevcorrsgc","nevcorrsgc", netamx*nphimx+60, -0.5, netamx*nphimx+59.5);  
  } //m_checkmap #endif
  
  if (m_combined) { //#ifdef COMBINED
    for (int j=0; j<ringmx; j++) {
      for (int i=0;i<sectmx;i++) {
	if (m_hotime) { //#ifdef HOTIME
	  sprintf(title, "com_hotime_ring%i_sect%i", j-2, i+1);
	  com_hotime[j][i] = new TProfile(title, title, 10, -0.5, 9.5, -1.0, 30.0);
	  
	  sprintf(title, "com_hopedtime_ring%i_sect%i", j-2, i+1);
	  com_hopedtime[j][i] = new TProfile(title, title, 10, -0.5, 9.5, -1.0, 30.0);
	} //m_hotime #endif      
	if (m_hbtime){ //#ifdef HBTIME
	  sprintf(title, "_com_hbtime_ring%i_sect%i", j-2, i+1);
	  com_hbtime[j][i] = new TProfile(title, title, 10, -0.5, 9.5, -1.0, 30.0);
	} //m_hbtime #endif
	sprintf(title, "com_sigrsg_ring%i_sect%i", j-2,i+1);
	com_sigrsg[j][i] = new TH1F(title, title, 31,-10.5,20.5);
	
	sprintf(title, "com_crossg_ring%i_sect%i", j-2,i+1);
	com_crossg[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	
	if (m_correl) { //#ifdef CORREL    
	  sprintf(title, "com_corrsg_ring%i_sect%i_leftbottom", j-2,i+1);
	  com_corrsglb[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	  
	  sprintf(title, "com_corrsg_ring%i_sect%i_rightbottom", j-2,i+1);
	  com_corrsgrb[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	  
	  sprintf(title, "com_corrsg_ring%i_sect%i_leftup", j-2,i+1);
	  com_corrsglu[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	  
	  sprintf(title, "com_corrsg_ring%i_sect%i_rightup", j-2,i+1);
	  com_corrsgru[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	  
	  sprintf(title, "com_corrsg_ring%i_sect%i_all", j-2,i+1);
	  com_corrsgall[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	  
	  sprintf(title, "com_corrsg_ring%i_sect%i_left", j-2,i+1);
	  com_corrsgl[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	  
	  sprintf(title, "com_corrsg_ring%i_sect%i_right", j-2,i+1);
	  com_corrsgr[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	} //m_correl #endif
	
	if (m_checkmap) { // #ifdef CHECKMAP    
	  sprintf(title, "com_corrsg_ring%i_sect%i_centrl", j-2,i+1);
	  com_corrsgc[j][i] = new TH1F(title, title, 31,-10.5,20.5);   
	} //m_checkmap #endif
      }
    }
  } //m_combined #endif  

  for (int i=-1; i<=1; i++) {
    for (int j=-1; j<=1; j++) {    
      int k = 3*(i+1)+j+1;
      
      sprintf(title, "hosct2p_eta%i_phi%i", i, j);
      ho_sig2p[k] = new TH1F(title, title, 31, -10.5, 20.5);
      
      sprintf(title, "hosct1p_eta%i_phi%i", i, j);
      ho_sig1p[k] = new TH1F(title, title, 31, -10.5, 20.5);

      sprintf(title, "hosct00_eta%i_phi%i", i, j);
      ho_sig00[k] = new TH1F(title, title, 31, -10.5, 20.5);

      sprintf(title, "hosct1m_eta%i_phi%i", i, j);
      ho_sig1m[k] = new TH1F(title, title, 31, -10.5, 20.5);
      
      sprintf(title, "hosct2m_eta%i_phi%i", i, j);
      ho_sig2m[k] = new TH1F(title, title, 31, -10.5, 20.5);

      if (m_hbinfo) { // #ifdef HBINFO
	sprintf(title, "hbhesig_eta%i_phi%i", i, j);
	hbhe_sig[k] = new TH1F(title, title, 51, -10.5, 40.5);
      } //m_hbinfo #endif
    }
  }
}


HOCalibAnalyzer::~HOCalibAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

  theFile->cd();
  theFile->Write();
  theFile->Close();
  cout <<" Selected events # is "<<ipass<<endl;
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
HOCalibAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  /*
  //FIXGM Put this is initialiser
  int mapx1[6][3]={{1,4,8}, {12,7,3}, {5,9,13}, {11,6,2}, {16,15,14}, {19,18,17}}; 
  //    int etamap1[21]={-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, 5,5,5, -1};
  //  int phimap1[21]={-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0, 2,1,0,-1};

    int mapx2[6][3]={{1,4,8}, {12,7,3}, {5,9,13}, {11,6,2}, {16,15,14}, {-1,-1,-1}};
  //  int etamap2[21]={-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, -1,-1,-1, -1};
  //  int phimap2[21]={-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0,  2, 1, 0, -1};

    int mapx0p[9][2]={{3,1}, {7,4}, {6,5},  {12,8}, {0,0}, {11,9}, {16,13}, {15,14}, {19,17}};
    int mapx0m[9][2]={{17,19}, {14,15}, {13,16}, {9,11}, {0,0}, {8,12}, {5,6}, {4,7}, {1,3}};

  //  int etamap0p[21]={-1, 0,-1,0, 1,2,2, 1,3,5, -1, 5,3,6, 7,7,6, 8,-1,8, -1};
  //  int phimap0p[21]={-1, 1,-1,0, 1,1,0, 0,1,1, -1, 0,0,1, 1,0,0, 1,-1,0, -1};

  //  int etamap0m[21]={-1, 8,-1,8, 7,6,6, 7,5,3, -1, 3,5,2, 1,1,2, 0,-1,0, -1};
  //  int phimap0m[21]={-1, 0,-1,1, 0,0,1, 1,0,0, -1, 1,1,0, 0,1,1, 0,-1,1, -1};

    int etamap[4][21]={{-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, -1,-1,-1, -1}, //etamap2
		       {-1, 0,3,1, 0,2,3, 1,0,2, -1, 3,1,2, 4,4,4, 5,5,5, -1},    //etamap1 
		       {-1, 0,-1,0, 1,2,2, 1,3,5, -1, 5,3,6, 7,7,6, 8,-1,8, -1},  //etamap0p
		       {-1, 8,-1,8, 7,6,6, 7,5,3, -1, 3,5,2, 1,1,2, 0,-1,0, -1}}; //etamap0m

    int phimap[4][21] ={{-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0, 2,1,0, -1},    //phimap2
			{-1, 0,2,2, 1,0,1, 1,2,1, -1, 0,0,2, 2,1,0, 2,1,0, -1},    //phimap1
			{-1, 1,-1,0, 1,1,0, 0,1,1, -1, 0,0,1, 1,0,0, 1,-1,0, -1},  //phimap0p
			{-1, 0,-1,1, 0,0,1, 1,0,0, -1, 1,1,0, 0,1,1, 0,-1,1, -1}};  //phimap0m
  //swapped phi map for R0+/R0- (15/03/07)  
  for (int i=0; i<4; i++) {
    for (int j=0; j<21; j++) {
      cout <<"ieta "<<i<<" "<<j<<" "<<etamap[i][j]<<endl;
    }
  }

  // Character convention for R+/-1/2
  //      int npixleft[21] = {-1, F, Q,-1, M, D, J,-1, T,-1, C,-1, R, P, H,-1, N, G};
  //      int npixrigh[21] = { Q, S,-1, D, J, L,-1, K,-1, E,-1, P, H, B,-1, G, A,-1};
  
  //      int npixlb1[21]={-1,-1,-1,-1, F, Q, S,-1, M, J, L, T, K,-1, C, R, P, H};
  //      int npixrb1[21]={-1,-1,-1, F, Q, S,-1, M, D, L,-1, K,-1, C, E, P, H, B};
  //      int npixlu1[21]={ M, D, J, T, K,-1, C,-1, R, H, B,-1, N, G, A,-1,-1,-1};
  //      int npixru1[21]={ D, J, L, K,-1, C, E, R, P, B,-1, N, G, A,-1,-1,-1,-1};

  int npixleft[21]={0, 0, 1, 2, 0, 4, 5, 6, 0, 8, 0, 0,11, 0,13,14,15, 0,17,18,0};
  int npixrigh[21]={0, 2, 3, 0, 5, 6, 7, 0, 9, 0, 0,12, 0,14,15,16, 0,18,19, 0,0};
  int npixlebt[21]={0, 0, 0, 0, 0, 1, 2, 3, 0, 4, 0, 6, 7, 8, 9, 0,11,13,14,15,0};
  int npixribt[21]={0, 0, 0, 0, 1, 2, 3, 0, 4, 5, 0, 7, 0, 9, 0,11,12,14,15,16,0};
  int npixleup[21]={0, 4, 5, 6, 8, 9, 0,11, 0,13, 0,15,16, 0,17,18,19, 0, 0, 0,0};
  int npixriup[21]={0, 5, 6, 7, 9, 0,11,12,13,14, 0,16, 0,17,18,19, 0, 0, 0, 0,0};
  */
  
  int iaxxx = 0;
  int ibxxx = 0;
  


  using namespace edm;

  float pival = acos(-1.);
  irun = iEvent.id().run();
  ievt = iEvent.id().event();
  
  edm::Handle<HOCalibVariableCollection>HOCalib;
  //  iEvent.getByLabel("HOCalibVariableCollection",HOCalib);
  //  iEvent.getByLabel("hoCalibProducer",HOCalib); 
  bool isCosMu = true;
  try {
    //    iEvent.getByType(HOCalib); 
    iEvent.getByLabel("hoCalibProducer","HOCalibVariableCollection",HOCalib);

  } catch ( cms::Exception &iEvent ) { isCosMu = false; } 
  if (isCosMu) { 
    nmuon = (*HOCalib).size();

    for (HOCalibVariableCollection::const_iterator hoC=(*HOCalib).begin(); hoC!=(*HOCalib).end(); hoC++){
      itrg1 = (*hoC).trig1;
      itrg2 = (*hoC).trig2;
      trkvx = (*hoC).trkvx;
      trkvy = (*hoC).trkvy;
      trkvz = (*hoC).trkvz;
      
      trkmm = (*hoC).trkmm;
      trkth = (*hoC).trkth;
      trkph = (*hoC).trkph;

      ndof   = (int)(*hoC).ndof;
      nrecht = (int)(*hoC).nrecht;
      chisq = (*hoC).chisq;

      therr = (*hoC).therr;
      pherr = (*hoC).pherr;
      trkph = (*hoC).trkph;

      isect = (*hoC).isect;
      hodx  = (*hoC).hodx;
      hody  = (*hoC).hody;
      hoang = (*hoC).hoang;
      for (int i=0; i<9; i++) { hosig[i] = (*hoC).hosig[i];}
      for (int i=0; i<18; i++) { hocorsig[i] = (*hoC).hocorsig[i];}    
      hocro = (*hoC).hocro;

      int ipsall=0;
      int ips0=0; 
      int ips1=0; 
      int ips2=0; 
      int ips3=0; 
      int ips4=0; 
      int ips5=0; 
      int ips6=0; 
      int ips7=0; 
      int ips8=0; 
      int ips9=0;
      int ips10 =0;
      
      int isel = 50*abs(int(hodx))+abs(int(hody));
      if (isect <0) continue; //FIXGM Is it proper place ?
      int ieta = int((abs(isect)%10000)/100.)-30; //an offset to acodate -ve eta values

      if (abs(ieta)>=16) continue;
      int iphi = abs(isect)%100;
      
      int tmpsect = int((iphi + 1)/6.) + 1;
      if (tmpsect >12) tmpsect = 1;
      
      int iring = 0;
      int tmpeta = ieta + 4; //For pixel mapping
      if (ieta >=-15 && ieta <=-11) {iring = -2; tmpeta =-11-ieta; } //abs(ieta)-11;} 
      if (ieta >=-10 && ieta <=-5)  {iring = -1; tmpeta =-5-ieta; } // abs(ieta)-5;}
      if (ieta >=  5 && ieta <= 10) {iring = 1; tmpeta  =ieta-5; }    
      if (ieta >= 11 && ieta <= 15) {iring = 2; tmpeta  =ieta-11;}   
      
      int iring2 = iring + 2;
      
      if (ndof >=20 && ndof<=40) {ips0 = (int)pow(2,0); ipsall += ips0;}
      if (ndof >5 && chisq/ndof<20 && chisq/ndof>3) {ips1 = (int)pow(2,1); ipsall +=ips1;}
      if (fabs(trkth-pival/2) <1.5) {ips2 = (int)pow(2,2); ipsall +=ips2;}
      if (fabs(trkph+pival/2) <1.5) {ips3 = (int)pow(2,3); ipsall +=ips3;}
      if (therr <10.06)             {ips4 = (int)pow(2,4); ipsall +=ips4;}
      if (pherr <10.08)             {ips5 = (int)pow(2,5); ipsall +=ips5;}
      if (hoang >0.75)             {ips6 = (int)pow(2,6);  ipsall +=ips6;}
      if (fabs(trkmm) >0.00)        {ips7 = (int)pow(2,7); ipsall +=ips7;}
      if (nmuon ==1)               {ips8 = (int)pow(2,8);  ipsall +=ips8;}
      if (sqrt(therr*therr+pherr*pherr)>0.03) {ips9=(int)pow(2,9);ipsall +=ips9;}
      
      if (fabs(hodx)<100 && fabs(hody)<100 && fabs(hodx)>2 && fabs(hody)>2)
	{ips10=(int)pow(2,10);ipsall +=ips10;}
      
      
      if (ipsall-ips0==pow(2,ncut)-pow(2,0)-1) sigvsevt[iring2][0]->Fill(ndof, hosig[4]);
      if (ipsall-ips1==pow(2,ncut)-pow(2,1)-1) sigvsevt[iring2][1]->Fill(chisq/TMath::Max(ndof,1), hosig[4]);
      if (ipsall-ips2==pow(2,ncut)-pow(2,2)-1) sigvsevt[iring2][2]->Fill(trkth, hosig[4]);
      if (ipsall-ips3==pow(2,ncut)-pow(2,3)-1) sigvsevt[iring2][3]->Fill(trkph, hosig[4]);
      if (ipsall-ips4==pow(2,ncut)-pow(2,4)-1) sigvsevt[iring2][4]->Fill(therr, hosig[4]);
      if (ipsall-ips5==pow(2,ncut)-pow(2,5)-1) sigvsevt[iring2][5]->Fill(pherr, hosig[4]);
      if (ipsall-ips6==pow(2,ncut)-pow(2,6)-1) sigvsevt[iring2][6]->Fill(hoang, hosig[4]);
      if (ipsall-ips7==pow(2,ncut)-pow(2,7)-1) sigvsevt[iring2][7]->Fill(fabs(trkmm), hosig[4]);
      if (ipsall-ips8==pow(2,ncut)-pow(2,8)-1) sigvsevt[iring2][8]->Fill(nmuon, hosig[4]);
      if (ipsall-ips9==pow(2,ncut)-pow(2,9)-1) sigvsevt[iring2][9]->Fill(sqrt(therr*therr+pherr*pherr), hosig[4]);
      if (ipsall-ips10==pow(2,ncut)-pow(2,10)-1) sigvsevt[iring2][10]->Fill(isel, hosig[4]);
      
      sigvsevt[iring+2][0]->Fill(ndof, hosig[4]);               
      if (ips0 >0) {
	sigvsevt[iring2+5][1]->Fill(chisq/TMath::Max(1,ndof), hosig[4]);   
	if (ips1 >0) {
	  sigvsevt[iring2+5][2]->Fill(trkth, hosig[4]);        
	  if (ips2 >0) {
	    sigvsevt[iring2+5][3]->Fill(trkph, hosig[4]);        
	    if (ips3 >0) {
	      sigvsevt[iring2+5][4]->Fill(therr, hosig[4]);        
	      if (ips4 >0) {
		sigvsevt[iring2+5][5]->Fill(pherr, hosig[4]);        
		if (ips5 >0) {
		  sigvsevt[iring2+5][6]->Fill(hoang, hosig[4]);        
		  if (ips6 >0) {
		    sigvsevt[iring2+5][7]->Fill(fabs(trkmm), hosig[4]);   
		    if (ips7 >0) {
		      sigvsevt[iring2+5][8]->Fill(nmuon, hosig[4]); 
		      if (ips8 >0) {
			sigvsevt[iring2+5][9]->Fill(sqrt(therr*therr+pherr*pherr), hosig[4]);
			if (ips9 >0) {
			  sigvsevt[iring2+5][10]->Fill(isel, hosig[4]);
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
      
      sigvsevt[iring2+10][0]->Fill(ndof, hosig[4]);               
      sigvsevt[iring2+10][1]->Fill(chisq/TMath::Max(1,ndof), hosig[4]);   
      sigvsevt[iring2+10][2]->Fill(trkth, hosig[4]);        
      sigvsevt[iring2+10][3]->Fill(trkph, hosig[4]);        
      sigvsevt[iring2+10][4]->Fill(therr, hosig[4]);        
      sigvsevt[iring2+10][5]->Fill(pherr, hosig[4]);        
      sigvsevt[iring2+10][6]->Fill(hoang, hosig[4]);        
      sigvsevt[iring2+10][7]->Fill(fabs(trkmm), hosig[4]);   
      sigvsevt[iring2+10][8]->Fill(nmuon, hosig[4]); 
      sigvsevt[iring2+10][9]->Fill(sqrt(therr*therr+pherr*pherr), hosig[4]);
      sigvsevt[iring2+10][10]->Fill(isel, hosig[4]);
      
      
      int iselect = ((ipsall == pow(2,ncut) - 1)&& (int(isel/50.)> 0) && (isel%50> 0)) ? 1 : 0;
      
      iselect = 1;  //FIXME
      int iselect2=0;
      
      if (nmuon==1) {
	if (hosig[4]!=-100) iselect2=1;
      }  
      iselect2=1; //FIXME
      
      if (hocro !=-100.0 && hocro <-50.0) hocro +=100.;
      
      muonnm->Fill(nmuon);
      muonmm->Fill(trkmm);
      muonth->Fill(trkth*180/pival);
      muonph->Fill(trkph*180/pival);
      muonch->Fill(chisq);
      
      if (iselect==1) { 
	T1->Fill(); ipass++;
	sel_muonnm->Fill(nmuon);
	sel_muonmm->Fill(trkmm);
	sel_muonth->Fill(trkth*180/pival);
	sel_muonph->Fill(trkph*180/pival);
	sel_muonch->Fill(chisq);
      }
      

      if (isect <0) continue;
      
      int tmpphi = (iphi + 1)%3; //pixel mapping
      int npixel = 0;
      int itag = -1;
      int iflip = 0;
      int fact = 2;
      
      if (iring ==0) { 
	tmpphi = (iphi+1)%2;
	if (tmpsect==2 || tmpsect==3 || tmpsect==6 || tmpsect==7 || tmpsect==10 || tmpsect==11) {
	  npixel = mapx0p[tmpeta][tmpphi];
	  itag = 2;
	} else {
	  npixel = mapx0m[tmpeta][tmpphi];
	  itag = 3;
	}
      } else { 
	fact = 3;
	if (tmpsect%2==1) iflip =1;
	if (abs(iring)==1) {
	  npixel = mapx1[tmpeta][(iflip==0) ? tmpphi : abs(tmpphi-2)];
	  itag = 1;
	} else {
	  npixel = mapx2[tmpeta][(iflip==0) ? tmpphi : abs(tmpphi-2)];
	  itag = 0;
	}
      }
      
      
      int tmpeta1 = (ieta>0) ? ieta -1 : -ieta +14; 
      //Histogram filling for noise study: phi shift according to DTChamberAnalysis
      int tmpphi1 = (iphi+6 <=nphimx) ? iphi+5 : iphi+5-nphimx;
      
      //      cout<<"iring2 "<<iring<<" "<<tmpsect<<" "<<ieta<<" "<<iphi<<" "<<npixel<<" "<<tmpeta<<" "<<tmpphi<<" "<<tmpeta1<<" "<<tmpphi1<<" "<<itag<<" "<<iflip<<" "<<fact<<" "<<endl;
      
      if (iselect2==1) {
	crossg[tmpeta1][tmpphi1]->Fill(hocro);
	if (m_combined) { //#ifdef COMBINED
	  if (tmpsect<12) { // phi shift according to DTChamberAnalysis
	    com_crossg[iring2][tmpsect]->Fill(hocro);
	  } else {
	    com_crossg[iring2][0]->Fill(hocro);
	  }
	} //M_combined #endif	  
      } //Cross talk
      
      if (iselect==1) { 
	
	tmpphi1 = iphi - 1;
	
	sigrsg[tmpeta1][tmpphi1]->Fill(hosig[4]);  
	if (m_combined) { //#ifdef COMBINED
	  com_sigrsg[iring2][tmpsect-1]->Fill(hosig[4]);  
	} //m_combined #endif
	
	if (m_checkmap) { //#ifdef CHECKMAP
	  tmpeta = etamap[itag][npixel];
	  tmpphi = phimap[itag][npixel];
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    if (int((hocorsig[fact*tmpeta+tmpphi]-hosig[4])*10000)/10000.!=0) { // && ieta !=-10)  {
	      iaxxx++;
	      cout<<"iring2xxx "<<irun<<" "<<ievt<<" "<<isect<<" "<<iring<<" "<<tmpsect<<" "<<ieta<<" "<<iphi<<" "<<npixel<<" "<<tmpeta<<" "<<tmpphi<<" "<<tmpeta1<<" "<<tmpphi1<<" itag "<<itag<<" "<<iflip<<" "<<fact<<" "<<hocorsig[fact*tmpeta+tmpphi]<<" "<<fact*tmpeta+tmpphi<<" "<<hosig[4]<<" "<<hodx<<" "<<hody<<endl;
	      
	      for (int i=0; i<18; i++) {cout <<" "<<i<<" "<<hocorsig[i];}
	      cout<<"ix "<<iaxxx<<" "<<ibxxx<<endl;
	    } else { ibxxx++; }
	    
	    corrsgc[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    if (m_combined) { //#ifdef COMBINED
	      com_corrsgc[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }
	} //m_checkmap #endif
	
	if (m_correl) { //#ifdef CORREL
	  float allcorsig = 0.0;
	  
	  tmpeta = etamap[itag][npixleft[npixel]];
	  tmpphi = phimap[itag][npixleft[npixel]];
	  
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    corrsgl[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    allcorsig +=hocorsig[fact*tmpeta+tmpphi];
	    if (m_combined) { //#ifdef COMBINED
	      com_corrsgl[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }	    
	  
	  tmpeta = etamap[itag][npixrigh[npixel]];
	  tmpphi = phimap[itag][npixrigh[npixel]];
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    corrsgr[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    allcorsig +=hocorsig[fact*tmpeta+tmpphi];
	    if (m_combined) { // #ifdef COMBINED
	      com_corrsgr[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }
	  
	  tmpeta = etamap[itag][npixlebt[npixel]];
	  tmpphi = phimap[itag][npixlebt[npixel]];
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    corrsglb[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    allcorsig +=hocorsig[fact*tmpeta+tmpphi];
	    if (m_combined){ //#ifdef COMBINED
	      com_corrsglb[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }
	  
	  tmpeta = etamap[itag][npixribt[npixel]];
	  tmpphi = phimap[itag][npixribt[npixel]];
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    corrsgrb[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    allcorsig +=hocorsig[fact*tmpeta+tmpphi];
	    if (m_combined) { // #ifdef COMBINED
	      com_corrsgrb[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }
	  
	  tmpeta = etamap[itag][npixleup[npixel]];
	  tmpphi = phimap[itag][npixleup[npixel]];
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    corrsglu[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    allcorsig +=hocorsig[fact*tmpeta+tmpphi];
	    if (m_combined) {// #ifdef COMBINED
	      com_corrsglu[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }
	  
	  tmpeta = etamap[itag][npixriup[npixel]];
	  tmpphi = phimap[itag][npixriup[npixel]];
	  if (tmpeta>=0 && tmpphi >=0) {
	    if (iflip !=0) tmpphi = abs(tmpphi-2);
	    corrsgru[tmpeta1][tmpphi1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    allcorsig +=hocorsig[fact*tmpeta+tmpphi];
	    if (m_combined) { // #ifdef COMBINED
	      com_corrsgru[iring2][tmpsect-1]->Fill(hocorsig[fact*tmpeta+tmpphi]);
	    } //m_combined #endif
	  }
	  corrsgall[tmpeta1][tmpphi1]->Fill(allcorsig); 
	  if (m_combined) { // #ifdef COMBINED
	    com_corrsgall[iring2][tmpsect-1]->Fill(allcorsig);
	  } //m_combined #endif
	  
	  
	} //m_correl #endif
	for (int k=0; k<9; k++) {
	  switch (iring) 
	    {
	    case 2 : ho_sig2p[k]->Fill(hosig[k]); break;
	    case 1 : ho_sig1p[k]->Fill(hosig[k]); break;
	    case 0 : ho_sig00[k]->Fill(hosig[k]); break;
	    case -1 : ho_sig1m[k]->Fill(hosig[k]); break;
	    case -2 : ho_sig2m[k]->Fill(hosig[k]); break;
	    }	  
	  if (m_hbinfo) { // #ifdef HBINFO
	    hbhe_sig[k]->Fill(hbhesig[k]);
	  } //m_hbinfo #endif
	}
      } //if (iselect==1)
    } //for (HOCalibVariableCollection::const_iterator hoC=(*HOCalib).begin(); hoC!=(*HOCalib).end(); hoC++){
    
  } //if (isCosMu) 
  
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
HOCalibAnalyzer::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HOCalibAnalyzer::endJob() {

  theFile->cd();
  
  for (int i=0; i<nphimx; i++) {
    for (int j=0; j<netamx; j++) {
      
      nevsigrsg->Fill(netamx*i+j, sigrsg[j][i]->GetEntries());
      mnsigrsg->Fill(netamx*i+j, sigrsg[j][i]->GetMean());
      rmssigrsg->Fill(netamx*i+j, sigrsg[j][i]->GetRMS());
      
      nevcrossg->Fill(netamx*i+j, crossg[j][i]->GetEntries());
      mncrossg->Fill(netamx*i+j, crossg[j][i]->GetMean());
      rmscrossg->Fill(netamx*i+j, crossg[j][i]->GetRMS());
      
      if (m_correl) { //#ifdef CORREL    
	
	nevcorrsglb->Fill(netamx*i+j, corrsglb[j][i]->GetEntries());
	mncorrsglb->Fill(netamx*i+j, corrsglb[j][i]->GetMean());
	rmscorrsglb->Fill(netamx*i+j, corrsglb[j][i]->GetRMS());
	
	nevcorrsgrb->Fill(netamx*i+j, corrsgrb[j][i]->GetEntries());
	mncorrsgrb->Fill(netamx*i+j, corrsgrb[j][i]->GetMean());
	rmscorrsgrb->Fill(netamx*i+j, corrsgrb[j][i]->GetRMS());
	
	nevcorrsglu->Fill(netamx*i+j, corrsglu[j][i]->GetEntries());
	mncorrsglu->Fill(netamx*i+j, corrsglu[j][i]->GetMean());
	rmscorrsglu->Fill(netamx*i+j, corrsglu[j][i]->GetRMS());

	nevcorrsgru->Fill(netamx*i+j, corrsgru[j][i]->GetEntries());
	mncorrsgru->Fill(netamx*i+j, corrsgru[j][i]->GetMean());
	rmscorrsgru->Fill(netamx*i+j, corrsgru[j][i]->GetRMS());
	
	nevcorrsgall->Fill(netamx*i+j, corrsgall[j][i]->GetEntries());
	mncorrsgall->Fill(netamx*i+j, corrsgall[j][i]->GetMean());
	rmscorrsgall->Fill(netamx*i+j, corrsgall[j][i]->GetRMS());
	
	nevcorrsgl->Fill(netamx*i+j, corrsgl[j][i]->GetEntries());
	mncorrsgl->Fill(netamx*i+j, corrsgl[j][i]->GetMean());
	rmscorrsgl->Fill(netamx*i+j, corrsgl[j][i]->GetRMS());
	
	nevcorrsgr->Fill(netamx*i+j, corrsgr[j][i]->GetEntries());
	mncorrsgr->Fill(netamx*i+j, corrsgr[j][i]->GetMean());
	rmscorrsgr->Fill(netamx*i+j, corrsgr[j][i]->GetRMS());
      } //m_correl #endif
      if (m_checkmap) { //#ifdef CHECKMAP
	nevcorrsgc->Fill(netamx*i+j, corrsgc[j][i]->GetEntries());
	mncorrsgc->Fill(netamx*i+j, corrsgc[j][i]->GetMean());
	rmscorrsgc->Fill(netamx*i+j, corrsgc[j][i]->GetRMS());
      } //m_checkmap #endif
    }
  }

  if (m_combined) { // #ifdef COMBINED
    for (int i=0; i<sectmx; i++) {
      for (int j=0; j<ringmx; j++) {
	
	nevsigrsg->Fill(netamx*nphimx+ringmx*i+j, com_sigrsg[j][i]->GetEntries());
	mnsigrsg->Fill(netamx*nphimx+ringmx*i+j, com_sigrsg[j][i]->GetMean());
	rmssigrsg->Fill(netamx*nphimx+ringmx*i+j, com_sigrsg[j][i]->GetRMS());
	
	nevcrossg->Fill(netamx*nphimx+ringmx*i+j, com_crossg[j][i]->GetEntries());
	mncrossg->Fill(netamx*nphimx+ringmx*i+j, com_crossg[j][i]->GetMean());
	rmscrossg->Fill(netamx*nphimx+ringmx*i+j, com_crossg[j][i]->GetRMS());
	
	if (m_correl) { // #ifdef CORREL      
	  nevcorrsglb->Fill(netamx*nphimx+ringmx*i+j, com_corrsglb[j][i]->GetEntries());
	  mncorrsglb->Fill(netamx*nphimx+ringmx*i+j, com_corrsglb[j][i]->GetMean());
	  rmscorrsglb->Fill(netamx*nphimx+ringmx*i+j, com_corrsglb[j][i]->GetRMS());
	  
	  nevcorrsgrb->Fill(netamx*nphimx+ringmx*i+j, com_corrsgrb[j][i]->GetEntries());
	  mncorrsgrb->Fill(netamx*nphimx+ringmx*i+j, com_corrsgrb[j][i]->GetMean());
	  rmscorrsgrb->Fill(netamx*nphimx+ringmx*i+j, com_corrsgrb[j][i]->GetRMS());
	  
	  nevcorrsglu->Fill(netamx*nphimx+ringmx*i+j, com_corrsglu[j][i]->GetEntries());
	  mncorrsglu->Fill(netamx*nphimx+ringmx*i+j, com_corrsglu[j][i]->GetMean());
	  rmscorrsglu->Fill(netamx*nphimx+ringmx*i+j, com_corrsglu[j][i]->GetRMS());
	  
	  nevcorrsgru->Fill(netamx*nphimx+ringmx*i+j, com_corrsgru[j][i]->GetEntries());
	  mncorrsgru->Fill(netamx*nphimx+ringmx*i+j, com_corrsgru[j][i]->GetMean());
	  rmscorrsgru->Fill(netamx*nphimx+ringmx*i+j, com_corrsgru[j][i]->GetRMS());
	  
	  nevcorrsgall->Fill(netamx*nphimx+ringmx*i+j, com_corrsgall[j][i]->GetEntries());
	  mncorrsgall->Fill(netamx*nphimx+ringmx*i+j, com_corrsgall[j][i]->GetMean());
	  rmscorrsgall->Fill(netamx*nphimx+ringmx*i+j, com_corrsgall[j][i]->GetRMS());
	  
	  nevcorrsgl->Fill(netamx*nphimx+ringmx*i+j, com_corrsgl[j][i]->GetEntries());
	  mncorrsgl->Fill(netamx*nphimx+ringmx*i+j, com_corrsgl[j][i]->GetMean());
	  rmscorrsgl->Fill(netamx*nphimx+ringmx*i+j, com_corrsgl[j][i]->GetRMS());
	  
	  nevcorrsgr->Fill(netamx*nphimx+ringmx*i+j, com_corrsgr[j][i]->GetEntries());
	  mncorrsgr->Fill(netamx*nphimx+ringmx*i+j, com_corrsgr[j][i]->GetMean());
	  rmscorrsgr->Fill(netamx*nphimx+ringmx*i+j, com_corrsgr[j][i]->GetRMS());
	} //m_correl #endif
	if (m_checkmap) { // #ifdef CHECKMAP
	  nevcorrsgc->Fill(netamx*nphimx+ringmx*i+j, com_corrsgc[j][i]->GetEntries());
	  mncorrsgc->Fill(netamx*nphimx+ringmx*i+j, com_corrsgc[j][i]->GetMean());
	  rmscorrsgc->Fill(netamx*nphimx+ringmx*i+j, com_corrsgc[j][i]->GetRMS());
	} //m_checkmap #endif
      }
    }
  } //m_combined #endif
  
    //    if (m_hotime) { // #ifdef HOTIME
  //  for (int i=0; i<nphimx; i++) {
  //    cout <<"hophi= "<<i+1;
  //    for (int j=0; j<netamx; j++) {
  //      cout<<" "<<pedval[j][i]/TMath::Max(1,nfile)/4;
  //    }
  //    cout<<endl;
  //  }
  //    } // #endif
  
  if (m_constant) { 
    ofstream file_out(theoutputtxtFile.c_str());  //FIXGM Use .cfg file for name
    int ips=111;
    TPostScript ps(theoutputpsFile.c_str(),ips);  //FIXGM Use .cfg file for name  
    ps.Range(16,20);
    
    for (int ij=0; ij<nphimx; ij++) {
      for (int jk=0; jk<netamx; jk++) {
	
	ps.NewPage();
	int xsiz = 600;
	int ysiz = 800;
	TCanvas *c0 = new TCanvas("c0", " Pedestal vs signal", xsiz, ysiz);
	c0->Divide(1,2);
	c0->cd(1);  
	float mean = crossg[jk][ij]->GetMean();
	float xmn = mean-10.;
	float xmx = mean+10;
	float rms = crossg[jk][ij]->GetRMS();
	float height = 0.5*crossg[jk][ij]->GetEntries();
	
	//      int nbin = crossg[jk][ij]->GetNbinsX();
	TF1* gx0 = new TF1("gx0", gausX, xmn, xmx, 3);   
	
	double par[3] ={height, mean, rms};
	gx0->SetParameters(par);
	
	crossg[jk][ij]->GetXaxis()->SetRangeUser(xmn+4, xmx);
	crossg[jk][ij]->Fit(gx0, "R");
	gx0->GetParameters();
	double gaupr[3];
	double parer[3];
	if (crossg[jk][ij]->GetEntries() >4) {
	  for (int k=0; k<3; k++) {
	    parer[k] = gx0->GetParError(k);
	    gaupr[k] = gx0->GetParameter(k);
	  }
	} else {
	  gaupr[2]=0.9;
	}
	
	if (sigrsg[jk][ij]->GetEntries() >10) {
	  
	  TF1* signal = new TF1("signal", totalfunc, -10, 20, 7);
	  
	  Double_t parall[7];
	  double parserr[7];
	  double fitres[7];
	  //	double fitres0[7];
	  
	  double pedht = sigrsg[jk][ij]->GetBinContent(11);
	  
	  gx0->GetParameters(&parall[0]);
	  parall[0] = 0.9*pedht;
	  parall[1] = 0.0;
	  parall[3] = parall[6]=parall[2];
	  parall[4] = sigrsg[jk][ij]->GetMean();
	  parall[5]=0.4*sigrsg[jk][ij]->GetEntries();
	  
	  cout <<"parall "<<jk<<" "<<ij<<" "<< parall[0]<<" "<< parall[1]<<" "<< parall[2]<<" "<< parall[3]<<" "<< parall[4]<<" "<< parall[5]<<" "<< parall[6]<<endl;
	  file_out <<"parall "<<jk<<" "<<ij<<" "<< parall[0]<<" "<< parall[1]<<" "<< parall[2]<<" "<< parall[3]<<" "<< parall[4]<<" "<< parall[5]<<" "<< parall[6]<<endl;   
	  
	  signal->SetParameters(parall);
	  signal->FixParameter(1, parall[1]);
	  signal->FixParameter(2, parall[2]); 
	  
	  signal->FixParameter(0, 0.0);
	  //      signal->SetParLimits(0, 0.00, 1.05*pedht);
	  signal->SetParLimits(5, 0, 1.25*sigrsg[jk][ij]->GetEntries());
	  
	  signal->SetParLimits(3, 0.25, 3.);
	  signal->SetParLimits(6, 0.25, 3.);    
	  signal->SetParLimits(4, 0.25, 10.); 
	  
	  signal->SetParNames("const", "mean", "sigma","Width","MP","Area","GSigma");   
	  
	  c0->cd(2);       
	  sigrsg[jk][ij]->Fit(signal, "0R+");
	  //      sigrsg[jk][ij]->Fit(signal, " ");
	  sigrsg[jk][ij]->GetXaxis()->SetRangeUser(-5.,20.);
	  sigrsg[jk][ij]->Draw();
	  
	  for (int k=0; k<7; k++) {
	    fitres[k] = signal->GetParameter(k);
	    parserr[k] = signal->GetParError(k);
	  }
	  
	  char temp[20];
	  
	  sprintf(temp, "pedfun");
	  TF1* pedfun = new TF1(temp, gausX, -10, 20, 3);
	  pedfun->SetParameters(fitres);
	  pedfun->SetLineColor(3);
	  pedfun->SetLineWidth(1);
	  pedfun->Draw("same");	
	  
	  sprintf(temp, "signal");
	  TF1* sigfun = new TF1(temp, langaufun, -5, 140, 4);
	  sigfun->SetParameters(&fitres[3]);
	  sigfun->SetLineWidth(1);
	  sigfun->SetLineColor(4);
	  sigfun->Draw("same");
	  
	  sprintf(temp, "total");
	  TF1* signalx = new TF1(temp, totalfunc, -10, 100, 7);
	  signalx->SetParameters(fitres);
	  signalx->SetLineWidth(2);
	  signalx->Draw("same");
	  
	  file_out <<"fitres x "<<jk<<" "<<ij<<" "<< fitres[0]<<" "<< fitres[1]<<" "<< fitres[2]<<" "<< fitres[3]<<" "<< fitres[4]<<" "<< fitres[5]<<" "<< fitres[6]<<endl;
	  file_out <<"parserr  "<<jk<<" "<<ij<<" "<< parserr[0]<<" "<< parserr[1]<<" "<< parserr[2]<<" "<< parserr[3]<<" "<< parserr[4]<<" "<< parserr[5]<<" "<< parserr[6]<<endl;    
	  double diff=fitres[4]-fitres[1];
	  if (diff <=0) diff = 0.000001;
	  double error=parserr[4]*parserr[4]+parer[2]*parer[2];
	  error = pow(error,0.5);
	  
	  file_out <<"Final " 
		   <<std::setw(2)<< jk<<" "
		   <<std::setw(2)<< ij<<" "
	    //		 <<std::setw(2)<<islot<<" "
		   <<std::setw(6)<< signal->GetChisquare()<<" "
		   <<std::setw(5)<<crossg[jk][ij]->GetEntries()<<" "
		   <<std::setw(5)<<gaupr[2]<<" "
		   <<std::setw(5)<<parer[2]<<" "	  
		   <<std::setw(5)<<sigrsg[jk][ij]->GetEntries()<<" "
		   <<std::setw(6)<<fitres[0]<<" "
		   <<std::setw(6)<<fitres[5]<<" "
		   <<std::setw(7)<<diff<<" "
		   <<std::setw(7)<<parserr[4]<<" "
		   <<std::setw(7)<<fitres[3]<<" "
		   <<std::setw(7)<<fitres[6]<<" "
		   <<std::setw(7)<<100*parserr[4]/diff<<" "
		   <<std::setw(7)<<diff/gaupr[2]<<endl;
	}   //if (sigrsg[jk][ij]->GetEntries() >10) {
	c0->Update();   
      } //for (int jk=0; jk<netamx; jk++) { 
    } //for (int ij=0; ij<nphimx; ij++) {

    ps.Close();
  } //m_constant
}
/*
Double_t HOCalibAnalyzer::langaufun(Double_t *x, Double_t *par) {

   //Fit parameters:
   //par[0]=Width (scale) parameter of Landau density
   //par[1]=Most Probable (MP, location) parameter of Landau density
   //par[2]=Total area (integral -inf to inf, normalization constant)
   //par[3]=Width (sigma) of convoluted Gaussian function
   //
   //In the Landau distribution (represented by the CERNLIB approximation), 
   //the maximum is located at x=-0.22278298 with the location parameter=0.
   //This shift is corrected within this function, so that the actual
   //maximum is identical to the MP parameter.

      // Numeric constants
      Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
      Double_t mpshift  = -0.22278298;       // Landau maximum location

      // Control constants
      Double_t np = 100.0;      // number of convolution steps
      Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

      // Variables
      Double_t xx;
      Double_t mpc;
      Double_t fland;
      Double_t sum = 0.0;
      Double_t xlow,xupp;
      Double_t step;
      Double_t i;


      // MP shift correction
      mpc = par[1] - mpshift * par[0]; 

      // Range of convolution integral
      xlow = x[0] - sc * par[3];
      xupp = x[0] + sc * par[3];

      step = (xupp-xlow) / np;

      // Convolution integral of Landau and Gaussian by sum
      for(i=1.0; i<=np/2; i++) {
         xx = xlow + (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);

         xx = xupp - (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);
      }

      //      cout <<"land "<< par[1]<<" "<<sum<<endl;

      return (par[2] * step * sum * invsq2pi / par[3]);
}
*/

//define this as a plug-in
DEFINE_FWK_MODULE(HOCalibAnalyzer);
