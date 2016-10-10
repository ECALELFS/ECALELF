#include "anyVar_class.h"
#include <TTreeFormula.h>
#include <TObject.h>
#include <TFriendElement.h>
#include <RooDataSet.h>
//#define DEBUG
#define MAXENTRIES 1000
#define MAXBRANCHES  20


anyVar_class::~anyVar_class(void)
{
	if(data_chain != NULL) delete data_chain;

}

anyVar_class::anyVar_class(TChain *data_chain_, std::vector<std::pair<TString, kType> > branchNames, ElectronCategory_class& cutter, std::string massBranchName):
	data_chain(data_chain_),
	_branchNames(branchNames),
	_cutter(cutter),
        massBranchName_(massBranchName)
{
	for(unsigned int ibranch = 0; ibranch < _branchNames.size(); ++ibranch) {
		RooRealVar * v = new RooRealVar(_branchNames[ibranch].first, "", -10000000000, 10000000000);
		Vars.addOwned(*v);
	}
	idx = new RooRealVar("idx", "", 0, 2);
	Vars.addOwned(*idx);
	mass = new RooRealVar("mass", "", 0, 10000);
	Vars.addOwned(*mass);
	smearMass = new RooRealVar("smearedMass", "", 0, 10000);
	Vars.addOwned(*smearMass);


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
		std::cout << "[STATUS] Enabling branch: " << branch.first << std::endl;
		chain->SetBranchStatus(branch.first, 1);
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



void anyVar_class::TreeToTree(TChain *chain, TCut cut)
{
        TFile * outFile = TFile::Open("dataset.root", "recreate");
        TTree * outree = chain->CloneTree(0);
        // add branches in friends
        /////TIter next(chain->GetListOfFriends());
        /////TObject * obj;
        ///////TFriendElement * obj;
        /////while ((obj = next())) {
        /////        //std::cout << "--> " << obj->GetTree()->Print() << "\n";
        /////        ((TFriendElement *)obj)->GetTree()->Print();
        /////}
	Long64_t nentries = chain->GetEntryList()->GetN();
	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector = new TTreeFormula("selector", cut, chain);
        for (Long64_t i = 0; i < nentries; ++i) {
		Long64_t ientry = chain->GetEntryNumber(i);
		chain->GetEntry(ientry);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			selector->UpdateFormulaLeaves();
		}
		if(selector->EvalInstance() == false) continue;
                outree->Fill();
        }
        outree->Print();
        outree->AutoSave();
        outFile->Close();
}


RooDataSet *anyVar_class::TreeToRooDataSet(TChain *chain, TCut cut, int iEle)
{
	// _branchList is used to make the set branch addresses
	Float_t branches_Float_t[MAXBRANCHES][3];
	Int_t branches_Int_t[MAXBRANCHES][3];
	UInt_t branches_UInt_t[MAXBRANCHES][3];
	ULong64_t branches_ULong64_t[MAXBRANCHES][3];
        //branches.resize(_branchNames.size());
        std::vector<kType> vtype;
	for(unsigned int ibranch = 0; ibranch < _branchNames.size(); ++ibranch) {
		//std::cout << "[DEBUG] " << ibranch << std::endl;
                TString name = _branchNames[ibranch].first;
                kType typ = (kType)(_branchNames[ibranch].second % kArrayTypes);
                switch (typ) {
                        case kInt_t:
                                chain->SetBranchAddress(_branchNames[ibranch].first, &branches_Int_t[ibranch]);
                                break;
                        case kFloat_t:
                                chain->SetBranchAddress(_branchNames[ibranch].first, &branches_Float_t[ibranch]);
                                break;
                        case kUInt_t:
                                chain->SetBranchAddress(_branchNames[ibranch].first, &branches_UInt_t[ibranch]);
                                break;
                        case kULong64_t:
                                chain->SetBranchAddress(_branchNames[ibranch].first, &branches_ULong64_t[ibranch]);
                                break;
                        default:
                                std::cerr << "[ERROR] branch `" << name << "' has type `" << typ << "': not supported, adjust your branch types in ZFitter and try again\n";
                                assert(0);
                }
                vtype.push_back(typ);
	}
	////// MUST COME AFTER ALL THE Branch VARIABLES
	////RooRealVar * idx = new RooRealVar("idx", "", 0, 2);
	////Vars.addOwned(*idx);
	////RooRealVar * mass = new RooRealVar("mass", "", 0, 10000);
	////Vars.addOwned(*mass);
	////RooRealVar * smearMass = new RooRealVar("smearedMass", "", 0, 10000);
	////Vars.addOwned(*smearMass);
	
	Float_t weight_ = 1 ;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;

        Float_t mll;

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

        chain->SetBranchAddress(massBranchName_.c_str(), &mll);

	RooDataSet * data = new RooDataSet(chain->GetTitle(), "dataset", Vars);

	Long64_t entries = chain->GetEntryList()->GetN();

	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector = new TTreeFormula("selector", cut, chain);

	std::cout << "___ ENTRIES: " << entries << std::endl;
	for(Long64_t jentry = 0; jentry < entries; ++jentry) {
		Long64_t entryNumber = chain->GetEntryNumber(jentry);
		chain->GetEntry(entryNumber);

                if (jentry % 1237 == 0) fprintf(stderr, " Processed events: %lld (%.2f%%)\r", jentry, (Float_t)jentry / entries * 100);

		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			selector->UpdateFormulaLeaves();
		}
		if(selector->EvalInstance() == false) continue;

		if(jentry < 1) {
                        std::cout << "[DEBUG] PU: " << pileupWeight_ << std::endl;
                        std::cout << "[DEBUG] corrEle[0]: " << corrEle_[0] << std::endl;
                        std::cout << "[DEBUG] corrEle[1]: " << corrEle_[1] << std::endl;
                        std::cout << "[DEBUG] smearEle[0]: " << smearEle_[0] << std::endl;
                        std::cout << "[DEBUG] smearEle[1]: " << smearEle_[1] << std::endl;
                        std::cout << "[DEBUG] r9weight[0]: " << r9weight_[0] << std::endl;
                        std::cout << "[DEBUG] r9weight[1]: " << r9weight_[1] << std::endl;
                }

                // flatten the ntuple structure
                for (Int_t j = 0; j < 2; ++j) {
					if(iEle!=0 && iEle!=j+1) continue;
                        //loop over the rooargset and fill it with setVal
                        weight = weight_ * pileupWeight_ * r9weight_[0]*r9weight_[1];
                        idx->setVal(j);
                        mass->setVal(mll);
                        smearMass->setVal(mll * sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1])));
                        RooLinkedListIter v_itr = Vars.iterator();
                        for(size_t i = 0; i < _branchNames.size(); ++i) {
                                RooRealVar *v = (RooRealVar *)v_itr.Next();
                                Int_t aj = j;
                                if (vtype[i] < kArrayTypes) aj = 0;
                                switch (vtype[i] % kArrayTypes) {
                                        case kFloat_t:
                                                v->setVal(branches_Float_t[i][aj]);
#ifdef DEBUG
                                                v->Print();
                                                std::cout << "--> branches_Float_t[i][aj] = " << branches_Float_t[i][aj] << "\n";
#endif
                                                break;
                                        case kInt_t:
                                                v->setVal(branches_Int_t[i][aj]);
#ifdef DEBUG
                                                std::cout << "--> branches_Int_t[i][aj] = " << branches_Int_t[i][aj] << "\n";
#endif
                                                break;
                                        case kUInt_t:
                                                v->setVal((Double_t)branches_UInt_t[i][aj]);
#ifdef DEBUG
                                                std::cout << "--> branches_UInt_t[i][aj] = " << branches_UInt_t[i][aj] << "\n";
#endif
                                                break;
                                        case kULong64_t:
                                                v->setVal((Double_t)branches_ULong64_t[i][aj]);
#ifdef DEBUG
                                                std::cout << "--> branches_ULong64_t[i][aj] = " << branches_ULong64_t[i][aj] << "\n";
#endif
                                                break;
                                        default:
                                                std::cerr << "[WARNING] branch type `" << vtype[i] << "' not recognized\n";
                                                break;
                                }
#ifdef DEBUG
                                v->Print();
#endif
                        }
                }
#ifdef DEBUG
                Vars.Print();
#endif
		data->add(Vars, weight);
//		if(invMass_ > invMass.getMin() && invMass_ < invMass.getMax()) data->add(Vars);
	}
        fprintf(stderr, "\n");

	delete selector;
	data->Print();
	chain->ResetBranchAddresses();
	Vars.Clear();
	return data;
}
