/// Produce miscalibration map for EB

#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TApplication.h"
#include "TLatex.h"
#include "TFile.h"
#include "TRandom.h"
#include "TRandom3.h"

void main(int argc, char **argv)
{

  TRandom3 genRand;

  std::ofstream io1;
  io1.open ("scalibMap6.txt");
  
  float eta, scalib;

  for (eta=-85; eta<86; eta++) {

    if (fabs(eta)<30)         scalib = genRand.Gaus(1,0.06);
    else if (fabs(eta)<60)    scalib = genRand.Gaus(1,0.07);
    else if (fabs(eta)<70)    scalib = genRand.Gaus(1,0.08);
    else                      scalib = genRand.Gaus(1,0.09);
 
    io1<<eta<<"    "<<scalib<<"\n";
  }

}
