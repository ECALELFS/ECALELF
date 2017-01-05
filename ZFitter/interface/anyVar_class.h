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

	/** \param data_chain_ chain with the data to be analyzed.
	 *  \param branchNames vector of <branchName, kType>
	 *  \param cutter already contructed with the right energySC branch set
	 *  \param massBranchName name of the invariant mass branch
	 *  \param outDirFitRes  name of the output directory storing the files with the stats
	 *  \param updateOnly    if true opens the output files in append mode
	 */
	anyVar_class(TChain *data_chain_,
	             std::vector<std::string> branchNames, ElectronCategory_class& cutter,
	             std::string massBranchName,
	             std::string outDirFitRes,
	             TDirectory* dir,
	             bool updateOnly = true
	            );

	~anyVar_class(void);
	void Import(TString commonCut, TString eleID_, std::set<TString>& branchList, unsigned int modulo = 1); ///< to be called in the main
	void TreeAnalyzeShervin(std::string region, TCut cut_ele1, TCut cut_ele2, float scale = 1., float smearing = 0.); ///<
	void SetOutDirName(std::string dirname, bool updateOnly = true);
	void ChangeModulo(unsigned int moduloIndex) {
		reduced_data = reduced_data_vec[moduloIndex].get();
		goodEntries.clear();
		Long64_t nentries = reduced_data->GetEntriesFast();
		for(Long64_t i = 0; i < nentries; ++i) {
			goodEntries.insert(goodEntries.end(), i);
		}
	};
	void SaveReducedTree(TFile* f) {
		f->cd();
		reduced_data->Write();
	}

	void SetDir(TDirectory *dir_) {
		if(_dir != NULL) delete _dir;
		_dir = dir_;
	}
private:
	TChain *data_chain; // pointer fixed in the constructor
	TTree *reduced_data;
	std::vector<std::unique_ptr<TTree> > reduced_data_vec;
	std::set<long long int> goodEntries;
	TDirectory *_dir;
	std::vector<std::string> _branchNames; //fixed in the constructor, these are the branches with the variables to study
	ElectronCategory_class _cutter; // this class provides the TCut for the selections given simple category names coded in the ElectronCategory_class header file

	std::vector<std::unique_ptr<std::ofstream> > _statfiles; ///< one file for each branch, here the stats are saved
	statsCollection _stats_vec;


	std::string massBranchName_; ///< branch name for the di-object invariant mass

	Double_t weight; ///< variable with the total event weight


	void ImportTree(TChain *chain, TCut& commonCut, std::set<TString>& commonCutBranches, std::set<TString>& branchList, unsigned int modulo); ///< add to the chain the entry list with selected events, the returned pointer is the same as the one in input
	void TreeToTree(TChain *chain, std::set<TString>& branchList, unsigned int modulo = 0); ///< skim the input TChain with selected events, copying only active branches

public:
	// define a struct saving the infos:



	void PrintStats(std::ostream outScale);
	bool _exclusiveCategories;
	std::string _outDirFitRes;


};

#endif

