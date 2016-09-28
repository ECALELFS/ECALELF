#include "anyVar_class.h"


TChain *anyVar_class::ImportTree(TChain *chain, TString commonCut, std::set<TString>& branchList)
{

	if(branchList.size() > 0) {
		std::cout << "[STATUS] Disabling branches to fast import" << std::endl;
		chain->SetBranchStatus("*", 0);
	}

	for(std::set<TString>::const_iterator itr = branchList.begin();
	        itr != branchList.end();
	        itr++) {
		std::cout << "[STATUS] Enabling branch: " << *itr << std::endl;
		chain->SetBranchStatus(*itr, 1);
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
	std::cout << "[STATUS] Enabling branch: " << invMass.GetName() << std::endl;
	chain->SetBranchStatus(invMass.GetName(), 1);
	chain->SetBranchStatus("invMass_SC", 1);
	chain->AddBranchToCache("*", kTRUE);

	//std::cout << commonCut << std::endl;
	TString evListName = "evList_";
	evListName += chain->GetTitle();
#ifdef DEBUG
	std::cout << "[DEBUG] commonCut ::" << commonCut << std::endl;
#endif
	chain->Draw(">>" + evListName, commonCut, "entrylist");
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

