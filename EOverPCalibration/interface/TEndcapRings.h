#ifndef TEndcapRings_h
#define TEndcapRings_h

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TTree.h"
#include "TVirtualFitter.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TChain.h"
#include "TMath.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>

class TEndcapRings
{
 private:
  int iEndcapRing[101][101][2];
 
 public:
  
  // ctor
  TEndcapRings(); 
  
  // dtor
  ~TEndcapRings();
  
  int GetEndcapRing(int,int,int);
  int GetEndcapIeta(int,int,int);
  int GetEndcapIphi(int,int,int);
  // ClassDef(TEndcapRings,1); //ring class
};

#endif
