#ifndef TSicCrystals_h
#define TSicCrystals_h

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TTree.h"
#include "TVirtualFitter.h"
#include "TStyle.h"
#include "TApplication.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>


/// SIC Crystals MAP ///
class TSicCrystals
{
private:
	int sicMap[100][100][3];
public:
	TSicCrystals();
	virtual ~TSicCrystals();
	int isSic(Int_t, Int_t, Int_t);
};

#endif
