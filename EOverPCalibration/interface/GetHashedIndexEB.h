#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TProfile.h>
#include <vector>
#include "../interface/hChain.h"
#include "../interface/h2Chain.h"
#include <TGraphErrors.h>

#include <TLorentzVector.h>



int GetHashedIndexEB(int iEta, int iPhi, int Zside);

int GetIetaFromHashedIndex(int Index);

int GetIphiFromHashedIndex(int Index);
