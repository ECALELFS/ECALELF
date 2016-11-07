// classe per i fit alla Z
#ifndef anyVar_class_hh
#define anyVar_class_hh

/*
 * FIXME mettere i throw exception e sistemare gli errori
 */

#include <iostream>
#include <stdlib.h>
// root headers
#include <TChain.h>
#include <TString.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TAttText.h>
#include <TH1F.h>
#include <TDirectory.h>
#include <THStack.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
// roofit headers
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <TMath.h>
#include <TString.h>
// needed for TEntryList fix
#include <TEntryList.h>
#include <TChainElement.h>

#include "Stats.hh"

//*********************************
#include "ElectronCategory_class.hh"

//#include <functions.h>
#define NELE 2

/** \class anyVar_class anyVar_class anyVar_class
	\brief class for estimation of basic quantities for any Float_t branch in the tree

\par How to test linearity
\verbatim
for scale in 0.98 0.99 0.995 1 1.005 1.01 1.02 ; do mkdir -p scale_${scale}; ./bin/ZFitter.exe -f data/validation/test_ss.dat --regionsFile=data/regions/validation.dat  --noPU --commonCut="Et_25" --outDirFitResData=scale_${scale}/ --scale=$scale > scale_${scale}/scale.log; done
\endverbatim
with fish:
\verbatim
for scale in scale_*; set scale (echo $scale | sed 's|scale_||') ; echo -ne "$scale\t"; grep EB-gold scale_{$scale}/invMass*.dat; end > p.dat
\endverbatim
in gnuplot:
\verbatim
f(x) = m * x + q
p 'p.dat' u ($1-1):($7) w lp
fit f(x) 'p.dat' u ($1-1):($7) via m,q
rep f(x)
print m/q
\endverbatim
*/


#include "TECALChain.h"
class anyVar_class
{
public:

	/// simple variable types
	typedef enum { kInt_t = 1, ///< Int_t
	               kUInt_t,    ///< UInt_t
	               kULong64_t, ///< ULong64_t
	               kFloat_t,   ///< Float_t
	               kArrayTypes = 10,  // not used, just to increment the index of the array types
	               kAInt_t,    ///< Array of Int_t
	               kAUInt_t,   ///< Array of UInt_t
	               kAULong64_t,///< Array of ULong64_t
	               kAFloat_t,  ///< Array of Float_t
	               kMaxType    // used only for checks
	             } kType;

	/** \param data_chain_ chain with the data to be analyzed.
	 *  \param branchNames vector of <branchName, kType>
	 *  \param cutter already contructed with the right energySC branch set
	 *  \param massBranchName name of the invariant mass branch
	 *  \param outDirFitRes  name of the output directory storing the files with the stats
	 */
	anyVar_class(TChain *data_chain_,
	             std::vector<std::pair<TString, kType> > branchNames, ElectronCategory_class& cutter,
	             std::string massBranchName,
	             std::string outDirFitRes
	            );

	~anyVar_class(void);
	void Import(TString commonCut, TString eleID_, std::set<TString>& branchList, unsigned int modulo = 0, unsigned int moduloIndex = 0); ///< to be called in the main
	void TreeAnalyzeShervin(std::string region, TCut cut_ele1, TCut cut_ele2, float scale = 1., float smearing = 0.); ///<
	void SetOutDirName(std::string dirname);

private:
	TChain *data_chain; // pointer fixed in the constructor
	TTree *reduced_data;
	TDirectory dir;
	std::vector<std::pair<TString, kType> > _branchNames; //fixed in the constructor, these are the branches with the variables to study
	ElectronCategory_class _cutter; // this class provides the TCut for the selections given simple category names coded in the ElectronCategory_class header file

	std::vector<std::unique_ptr<std::ofstream> > _statfiles; ///< one file for each branch, here the stats are saved
	statsCollection _stats_vec;


	RooArgSet Vars; ///< argSet containing the RooRealVars of branches under study

	std::string massBranchName_; ///< branch name for the di-object invariant mass

	Double_t weight; ///< variable with the total event weight

	RooRealVar * idx, * mass, * smearMass;


	TChain *ImportTree(TChain *chain, TCut commonCut, std::set<TString>& branchList, unsigned int modulo, unsigned int moduloIndex); ///< add to the chain the entry list with selected events, the returned pointer is the same as the one in input
	void TreeToTreeShervin(TChain *chain, TCut cut, unsigned int modulo = 0, unsigned int moduloIndex = 0); ///< skim the input TChain with selected events, copying only active branches
	void TreeToTree(TChain *chain, TCut cut); ///< skim the input TChain with selected events, copying only active branches
	RooDataSet *TreeToRooDataSet(TChain *chain, TCut cut, int iEle = 0); //< returns a RooDataset with selected events and weight


public:
	// define a struct saving the infos:



	double GetEffectiveSigma(RooAbsData *dataset, float quant);

	void PrintStats(std::ostream outScale);
	bool _exclusiveCategories;



};

#endif

