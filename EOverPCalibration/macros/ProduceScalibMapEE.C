/// Produce miscalibration map for EE

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
  io1.open ("scalibMapEE.txt");
  
  float r, scalib;

  for (r=0; r<40; r++) {

    if (fabs(r)<5)         scalib = genRand.Gaus(1,0.01);
    else if (fabs(r)<15)    scalib = genRand.Gaus(1,0.005);
    else if (fabs(r)<30)    scalib = genRand.Gaus(1,0.005);
    else if (fabs(r)<40)    scalib = genRand.Gaus(1,0.01);
    else                      scalib = genRand.Gaus(1,0.015);
 
    io1<<r<<"    "<<scalib<<"\n";
  }

}
