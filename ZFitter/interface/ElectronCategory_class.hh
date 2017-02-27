#ifndef ElectronCategory_class_hh
#define ElectronCategory_class_hh
/// Di-electron events are selected using a short special syntax defined in this class
/**\class ElectronCategory_class ElectronCategory_class.cc Calibration/ZFitter/src/ElectronCategory_class.cc
 */

/***********************************************************//**
 * One di-electron category is difined by a series of cuts to be
 * applied to both electrons
 *
 * \par Syntax:
 *  - '-' is the cut separator <BR>
 *         es. <cut1>-<cut2>-<cut3> <BR>
 *         es. EB-absEta_0_1-gold:  <BR>
 *               three cuts are applied:
 *               - EB -> no parameters,
 *               - absEta_0_1 -> two parameters 0 and 1 (parameters are separated by _,
 *               - gold -> no parameters
 *  - List of possible cuts without parameter:
 * EB EE EB_EE EBp (EB+) EBm (EB-)  EE EEp EEm RefReg
 *  - List of possible cuts with 1 parameter:
 *    - Et_X: Et_
 *    - eleID_X: eleID == X
 *    - gainEle_X: 12, 6, 1, 6and1
 *  - List of possible cuts with 2 parameters:  <BR>
 * <B> ATTENTION! Same syntax but different meaning! </B> <BR>
 *   - \b runNumber_X_Y: X <= runNumber <= Y
 *   - \b IEta_X_Y:      X <= ieta (or iX) <= Y
 *   - \b absIEta_X_Y:   X <= abs(ieta) <= Y
 *   - \b distIEta_X_Y:  X-Y <= ieta <= X+Y
 *   - \b absEta_X_Y:    X <= |eta| < Y
 *   - \b energySC_X_Y:  X <= energySCEle < Y
 *   - \b nPV_X_Y:       X <= nPV < Y
 *
 *************************************************************/

#include <TCut.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>


#include <iostream>
#include <set>

class ElectronCategory_class
{


public:
	//--------------- constructor and destructor
	ElectronCategory_class(bool isRooFit = false, bool roofitNameAsNtuple = false);
	~ElectronCategory_class();

	//--------------- flags
	bool _isRooFit; // defines if the cuts are defined for RooFit or not
	bool _roofitNameAsNtuple;
	TString invMassBranchName;
	TString energyBranchName;
	bool _corrEle;
	//------------------------------
	TCut GetCut(TString region, bool isMC, int nEle = 0, bool corrEle = false); ///< Returns the TCut correspoding to the categories

	//inline void SetExternalFile(TString fileName){_extFileName=fileName;};
	//private:
	std::set<TString> GetBranchNameNtuple(TString region); ///< returns the list of the branch names used by the cuts
	std::vector<TString> GetBranchNameNtupleVec(TString region); ///< returns a vector for PyROOT

private:
	TString GetBranchNameRooFit(TString region);

	// this method is though to give smaller formulas in the selection reducing a bit redundant cuts
	std::set<TString> GetCutSet(TString region); // -> make sure that all the cuts that goes in && are inserted separately

	//TString _extFileName;
};





#endif
