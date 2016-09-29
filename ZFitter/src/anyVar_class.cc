#include "anyVar_class.h"
#include <TTreeFormula.h>
#include <RooDataSet.h>
#define DEBUG
#define MAXENTRIES 100

anyVar_class::~anyVar_class(void)
{
	if(data_chain != NULL) delete data_chain;

}

anyVar_class::anyVar_class(TChain *data_chain_, std::vector<TString> branchNames, ElectronCategory_class& cutter):
	data_chain(data_chain_),
	_branchNames(branchNames),
	_cutter(cutter),
	weight("weight", "weight", 1, 0, 100)
{

}



void anyVar_class::Import(TString commonCut, TString eleID_, std::set<TString>& branchList)
{
	commonCut += "-eleID_" + eleID_;
	TCut dataCut = _cutter.GetCut(commonCut, false);
	std::cout << "------------------------------ IMPORT DATASETS" << std::endl;
	std::cout << "--------------- Importing: " << data_chain->GetEntries() << std::endl;
	//if(data!=NULL) delete data;
	TChain *data = ImportTree(data_chain, dataCut, branchList);
	commonData = new TEntryList(*(data->GetEntryList()));
	//commonData->Print();
	std::cout << "[INFO] imported " << commonData->GetN() << "\t" << (data->GetEntryList())->GetN() << " events passing commmon cuts" << std::endl;
	//data->Print();
	std::cout << "--------------- IMPORT finished" << std::endl;
	return;
}


TChain *anyVar_class::ImportTree(TChain *chain, TCut commonCut, std::set<TString>& branchList)
{

	if(branchList.size() > 0) {
		std::cout << "[STATUS] Disabling branches to fast import" << std::endl;
		chain->SetBranchStatus("*", 0);
	}

	for(auto branch : branchList) {
		std::cout << "[STATUS] Enabling branch: " << branch << std::endl;
		chain->SetBranchStatus(branch, 1);
	}

	for(auto branch : _branchNames) {
		std::cout << "[STATUS] Enabling branch: " << branch << std::endl;
		chain->SetBranchStatus(branch, 1);
	}
	// abilitare la lista dei friend branch
	if(chain->GetBranch("scaleEle"))  chain->SetBranchStatus("scaleEle", 1);
	if(chain->GetBranch("smearEle")) chain->SetBranchStatus("smearEle", 1);
	if(chain->GetBranch("puWeight")) chain->SetBranchStatus("puWeight", 1);
	if(chain->GetBranch("r9Weight")) chain->SetBranchStatus("r9Weight", 1);
	chain->SetBranchStatus("R9Ele", 1);
	chain->SetBranchStatus("etaEle", 1);
	chain->SetBranchStatus("recoFlagsEle", 1);
	chain->SetBranchStatus("eleID", 1);
	chain->SetBranchStatus("eventNumber", 1);

	chain->AddBranchToCache("*", kTRUE);

	//std::cout << commonCut << std::endl;
	TString evListName = "evList_";
	evListName += chain->GetTitle();
#ifdef DEBUG
	std::cout << "[DEBUG] commonCut ::" << commonCut << std::endl;
	chain->Draw(">>" + evListName, commonCut, "entrylist", MAXENTRIES);
#else
	chain->Draw(">>" + evListName, commonCut, "entrylist");
#endif

	TEntryList *elist = (TEntryList*)gROOT->FindObject(evListName);
#ifdef DEBUG
	std::cout << "[DEBUG] chain ptr :\t" << chain << std::endl;
#endif
	TECALChain *chain_ecal = (TECALChain*)chain;
	chain_ecal->TECALChain::SetEntryList(elist);
	assert(elist != NULL);
	std::cout << "[INFO] Selected events: " <<  chain_ecal->GetEntryList()->GetN() << std::endl;
	chain = dynamic_cast<TChain*>(chain_ecal);
#ifdef DEBUG
	std::cout << "[DEBUG] new chain ptr :\t" << chain_ecal << std::endl;
	std::cout << "[DEBUG]     chain ptr :\t" << chain      << std::endl;
#endif
	return chain;
}





RooDataSet *anyVar_class::TreeToRooDataSet(TChain *chain, TCut cut)
{
	// _branchList is used to make the set branch addresses
	std::vector<Float_t> branches;
	for(unsigned int ibranch = 0; ibranch < _branchNames.size(); ++ibranch) {
		std::cout << "[DEBUG] " << ibranch << std::endl;
		branches.push_back(-1);
		chain->SetBranchAddress(_branchNames[ibranch], &branches[ibranch]);
		RooRealVar *v = new RooRealVar(_branchNames[ibranch], "", -1000, 1000);
		Vars.addOwned(*v);
	}
	// now the size of branches is the same as _branchNames

	Float_t weight_ = 1 ;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;


	if(chain->GetBranch("puWeight") != NULL) {
		std::cout << "[STATUS] Adding pileup weight branch from friend" << std::endl;
		chain->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(chain->GetBranch("scaleEle") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("scaleEle", corrEle_);
	}

	if(chain->GetBranch("smearEle") != NULL && TString(chain->GetTitle()) != "d") {
		std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//chain->Scan("smearEle");
		chain->SetBranchAddress("smearEle", smearEle_);
	}

	if(chain->GetBranch("r9Weight") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("r9Weight", r9weight_);
	}

	RooDataSet *data = new RooDataSet(chain->GetTitle(), "dataset", Vars);

	Long64_t entries = chain->GetEntryList()->GetN();
#ifdef DEBUG
	entries = MAXENTRIES;
#endif
	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector = new TTreeFormula("selector", cut, chain);

	std::cout << "___ ENTRIES: " << entries << std::endl;
	for(Long64_t jentry = 0; jentry < entries; jentry++) {
		Long64_t entryNumber = chain->GetEntryNumber(jentry);
		chain->GetEntry(entryNumber);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			selector->UpdateFormulaLeaves();
		}
		if(selector->EvalInstance() == false) continue;

		if(jentry < 1)  std::cout << "[DEBUG] PU: " << pileupWeight_
			                          << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] corrEle[0]: " << corrEle_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] corrEle[1]: " << corrEle_[1] << std::endl;

		if(jentry < 1)  std::cout << "[DEBUG] smearEle[0]: " << smearEle_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] smearEle[1]: " << smearEle_[1] << std::endl;

		if(jentry < 1)  std::cout << "[DEBUG] r9weight[0]: " << r9weight_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] r9weight[1]: " << r9weight_[1] << std::endl;

//		invMass_ *= sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1]));
//		invMass.setVal(invMass_ );

		//loop over the rooargset and fill it with setVal
		weight.setVal(weight_ * pileupWeight_ * r9weight_[0]*r9weight_[1]);
		RooLinkedListIter v_itr = Vars.iterator();
		for(unsigned int i = 0; i < Vars.getSize(); ++i) {
			RooRealVar *v = (RooRealVar *) v_itr.Next();
			v->setVal(branches[i]);
#ifdef DEBUG
			v->Print();
#endif
		}

		data->add(Vars);
//		if(invMass_ > invMass.getMin() && invMass_ < invMass.getMax()) data->add(Vars);
	}
	delete selector;
	data->Print();
	chain->ResetBranchAddresses();

	return data;
}
