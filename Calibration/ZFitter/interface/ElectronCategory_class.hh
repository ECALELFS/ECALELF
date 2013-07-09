/*************************************************************
 * Parser of category class provides a TCut
 *
 * Syntax:
 *      one di-electron category is difined by a series of cuts to be
 *        applied to both electrons
 *        '-' is the cut separator
 *         es. <cut1>-<cut2>-<cut3> 
 *         es. EB-absEta_0_1-gold: 
 *               three cuts are applied: 
 *               EB -> no parameters
 *               absEta_0_1 -> two parameters 0 and 1 (parameters are
 *                             separated by _
 *               gold -> no parameters
 *
 * List of possible cuts without parameter:
 * EB EE EB_EE EBp (EB+) EBm (EB-)  EE EEp EEm RefReg
 *
 * List of possible cuts with 1 parameter:
 * Et_X: Et_
 * eleID_X: eleID == X
 * 
 * List of possible cuts with 2 parameters:  
 * #### ATTENTION! Same syntax but different meaning!
 * runNumber_X_Y: X <= runNumber <= Y
 * IEta_X_Y:      X <= ieta (or iX) <= Y
 * absIEta_X_Y:   X <= abs(ieta) <= Y
 * distIEta_X_Y:  X-Y <= ieta <= X+Y 
 * absEta_X_Y:    X <= |eta| < Y
 * energySC_X_Y:  X <= energySCEle < Y
 * nPV_X_Y:       X <= nPV < Y
 * 
 *************************************************************/
#ifndef ElectronCategory_class_hh
#define ElectronCategory_class_hh

#include <TCut.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>


#include <iostream>
#include <set>

class ElectronCategory_class{

 
public:
  //--------------- constructor and destructor
  ElectronCategory_class(bool isRooFit=false, bool roofitNameAsNtuple=false);
  ~ElectronCategory_class();

  //--------------- flags
  bool _isRooFit; // defines if the cuts are defined for RooFit or not
  bool _roofitNameAsNtuple;
  //------------------------------ 
  // this method is though to give smaller formulas in the selection reducing a bit redundant cuts
  std::set<TString> GetCutSet(TString region); // -> make sure that all the cuts that goes in && are inserted separately
  TCut GetCut(TString region, bool isMC, int nEle=0, bool corrEle=false);
  //inline  TCut GetCut(TString region, bool isMC){ return GetCut(region, isMC, 0,false);};
  TString GetBranchName(TString region, bool isRooFit);

  //inline void SetExternalFile(TString fileName){_extFileName=fileName;};
  //private:
  //TCut GetSpecial(void);
  TString GetBranchNameRooFit(TString region);
  std::set<TString> GetBranchNameNtuple(TString region);
  
private:
  //TString _extFileName;
};





#endif
