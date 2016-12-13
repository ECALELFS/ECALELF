#include "../interface/anyVar_class.h"
#include <TTreeFormula.h>
#include <TObject.h>
#include <TBranchElement.h>
#include <TFriendElement.h>
#include <RooDataSet.h>
#include <TStopwatch.h>
//#define DEBUG
#define MAXENTRIES 10000
#define MAXBRANCHES  20


anyVar_class::~anyVar_class(void)
{
}

anyVar_class::anyVar_class(TChain *data_chain_, std::vector<std::pair<TString, kType> > branchNames, ElectronCategory_class& cutter, std::string massBranchName, std::string outDirFitRes):
	data_chain(data_chain_),
	dir(),
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



	Long64_t entries = data_chain->GetEntries();
	for(auto& branch : _branchNames) {
		TString& bname = branch.first;
		_statfiles.emplace_back(new std::ofstream(outDirFitRes + bname + ".dat"));

		stats s(bname.Data(), entries);
		_stats_vec.push_back(s);
	}

	_statfiles.emplace_back(new std::ofstream(outDirFitRes + massBranchName + ".dat"));
	stats s(massBranchName, entries);
	_stats_vec.push_back(s);
}

void anyVar_class::SetOutDirName(std::string dirname)
{
	_statfiles.clear(); ///momory leak?
	for(auto& branch : _branchNames) {
		TString& bname = branch.first;
		_statfiles.emplace_back(new std::ofstream(dirname + bname + ".dat"));
	}
}

void anyVar_class::Import(TString commonCut, TString eleID_, std::set<TString>& branchList, unsigned int modulo, unsigned int moduloIndex)
{
	commonCut += "-eleID_" + eleID_;
	TCut dataCut = _cutter.GetCut(commonCut, false);
	std::cout << "------------------------------ IMPORT DATASETS" << std::endl;
	std::cout << "--------------- Importing: " << data_chain->GetEntries() << std::endl;
	//if(data!=NULL) delete data;
	TChain *data = ImportTree(data_chain, dataCut, branchList, modulo, moduloIndex);
	std::cout << "[INFO] imported "  << (data->GetEntryList())->GetN() << " events passing commmon cuts" << std::endl;
	//data->Print();
	std::cout << "--------------- IMPORT finished" << std::endl;
	return;
}


TChain *anyVar_class::ImportTree(TChain *chain, TCut commonCut, std::set<TString>& branchList, unsigned int modulo, unsigned int moduloIndex)
{

	if(branchList.size() > 0) {
		std::cout << "[STATUS] Disabling branches to fast import" << std::endl;
		chain->SetBranchStatus("*", 0);
	}

	for(auto branch : branchList) {
		std::cout << "[STATUS] Enabling branch: " << branch << std::endl;
		chain->SetBranchStatus(branch, 1);
		chain->AddBranchToCache(branch, kTRUE);
	}

	for(auto branch : _branchNames) {
		std::cout << "[STATUS] Enabling branch: " << branch.first << std::endl;
		chain->SetBranchStatus(branch.first, 1);
		chain->AddBranchToCache(branch.first, kTRUE);
	}
	// abilitare la lista dei friend branch
	if(chain->GetBranch("scaleEle"))  chain->SetBranchStatus("scaleEle", 1);
	if(chain->GetBranch("smearEle")) chain->SetBranchStatus("smearEle", 1);
	if(chain->GetBranch("puWeight")) chain->SetBranchStatus("puWeight", 1);
	if(chain->GetBranch("r9Weight")) chain->SetBranchStatus("r9Weight", 1);
	chain->SetBranchStatus(massBranchName_.c_str(), 1);
//	chain->SetBranchStatus("R9Ele", 1);
//	chain->SetBranchStatus("etaEle", 1);
//	chain->SetBranchStatus("recoFlagsEle", 1);
	chain->SetBranchStatus("eleID", 1);
//	chain->SetBranchStatus("eventNumber", 1);

	//chain->SetCacheSize(5000000000);
	TStopwatch ts;
	ts.Start();
	chain->SetEntryList(NULL); // remove any prior entry list
	TString evListName = "evList_";
	evListName += chain->GetTitle();
#ifdef DEBUG
	std::cout << "[DEBUG] commonCut ::" << commonCut << std::endl;
	chain->Draw(">>" + evListName, commonCut, "entrylist", MAXENTRIES);
#else
//	chain->Draw(">>" + evListName, commonCut, "entrylist", MAXENTRIES);
	chain->Draw(">>" + evListName, commonCut, "entrylist");
#endif

	TEntryList *elist = (TEntryList*)gROOT->FindObject(evListName);
	assert(elist != NULL);
#ifdef DEBUG
	elist->Print();
#endif
	TECALChain *chain_ecal = (TECALChain*)chain;
	chain_ecal->TECALChain::SetEntryList(elist);

	std::cout << "[INFO] Selected events: " <<  chain_ecal->GetEntryList()->GetN() << std::endl;
	//chain = dynamic_cast<TChain*>(chain_ecal);
	ts.Stop();
	ts.Print();

#ifndef dump_root_tree
	TreeToTreeShervin(chain, "", modulo, moduloIndex);
#else
	TreeToTree(chain, "1");
#endif

	return chain;
}


void anyVar_class::TreeToTreeShervin(TChain *chain, TCut cut, unsigned int modulo, unsigned int moduloIndex)
{
	std::cout << "[INFO] Start copying the tree in memory" << std::endl;
	TStopwatch ts;
	ts.Start();
	reduced_data  =  chain->CloneTree(0, "fast");
	reduced_data->SetDirectory(&dir);

	TIter next(reduced_data->GetListOfFriends());
	TObject * obj;
	while ((obj = next())) {
		//std::cout << "--> " << ((TFriendElement *)obj)->GetTree()->GetName() << "\n";
		TTree * t = ((TFriendElement *)obj)->GetTree();
		TObjArray * branches = t->GetListOfBranches();
		Int_t nb = branches->GetEntriesFast();
		for (Int_t i = 0; i < nb; ++i) {
			TBranch * br = chain->GetBranch(branches->At(i)->GetName());
			TString title = br->GetTitle();
			if (chain->GetBranchStatus(br->GetName())) {
				reduced_data->Branch(br->GetName(), br->GetAddress(), br->GetTitle());
			}
		}
		reduced_data->RemoveFriend(t);
	}

	// add branches in friends
	/////TIter next(chain->GetListOfFriends());
	/////TObject * obj;
	///////TFriendElement * obj;
	/////while ((obj = next())) {
	/////        //std::cout << "--> " << obj->GetTree()->Print() << "\n";
	/////        ((TFriendElement *)obj)->GetTree()->Print();
	/////}
	Long64_t nentries = chain->GetEntryList()->GetN();
	std::cout << "[[ Entries = " << nentries << std::endl;
	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector = (cut == "" ) ? NULL : new TTreeFormula("selector", cut, chain);
	for (Long64_t i = 0; i < nentries; ++i) {
		if(modulo > 0 && i % modulo != moduloIndex) continue;
		Long64_t ientry = chain->GetEntryNumber(i);
		chain->GetEntry(ientry);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			if(selector != NULL) selector->UpdateFormulaLeaves();
		}
		if(selector != NULL && selector->EvalInstance() == false) continue;
		reduced_data->Fill();
//
	}

	TString evListName = "evList_";
	evListName += chain->GetTitle();
	evListName += "_red";

	reduced_data->Draw(">>" + evListName, "", "entrylist");
	TEntryList *elist = (TEntryList*)gROOT->FindObject(evListName);
	assert(elist != NULL);
	TECALChain *chain_ecal = (TECALChain*)reduced_data;
	chain_ecal->TECALChain::SetEntryList(elist);

	reduced_data = dynamic_cast<TChain*>(chain_ecal);

	reduced_data->Print();

	// assert(reduced_data->GetEntryList()!=NULL);
	//reduced_data->GetEntryList()->Print();


	ts.Stop();
	std::cout << "Copy tree done: ";
	ts.Print();
}


void anyVar_class::TreeToTree(TChain *chain, TCut cut)
{
	// _branchList is used to make the set branch addresses
	Float_t branches_Float_t[MAXBRANCHES][3];
	Int_t branches_Int_t[MAXBRANCHES][3];
	UInt_t branches_UInt_t[MAXBRANCHES][3];
	ULong64_t branches_ULong64_t[MAXBRANCHES][3];
	//branches.resize(_branchNames.size());
	std::vector<kType> vtype;
	for(unsigned int ibranch = 0; ibranch < _branchNames.size(); ++ibranch) {
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
        //chain->Scan("runNumber:eventNumber:runTime:eleIDTree.runNumber:eleIDTree.eventNumber:eleIDTree.runTime", "runNumber > 276316");
        UInt_t ck_run, fr_run;
        ULong64_t ck_evt, fr_evt;
        chain->SetBranchAddress("runNumber", &ck_run);
        chain->SetBranchAddress("eventNumber", &ck_evt);
        chain->SetBranchAddress("eleIDTree.runNumber", &fr_run);
        chain->SetBranchAddress("eleIDTree.eventNumber", &fr_evt);

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
	TFile * outFile = TFile::Open("dataset.root", "recreate");
	TTree * outree = chain->CloneTree(0);
	//chain->GetEntry(0);
        // get list of active branches already in the main tree
        // so that they don't get duplicated when merging TFriends
        TObjArray * branches = outree->GetListOfBranches();
        Int_t nb = branches->GetEntriesFast();
        std::set<std::string> blist;
        for (Int_t i = 0; i < nb; ++i) {
                if (chain->GetBranchStatus(branches->At(i)->GetName())) {
                        blist.insert(branches->At(i)->GetName());
                }
        }
	// add branches of friends
	TIter next(outree->GetListOfFriends());
	TObject * obj;
	while ((obj = next())) {
		std::cout << "--> " << ((TFriendElement *)obj)->GetTree()->GetName() << "\n";
		TTree * t = ((TFriendElement *)obj)->GetTree();
		TObjArray * branches = t->GetListOfBranches();
		Int_t nb = branches->GetEntriesFast();
		for (Int_t i = 0; i < nb; ++i) {
			TBranch * br = chain->GetBranch(branches->At(i)->GetName());
			TString title = br->GetTitle();
			if (chain->GetBranchStatus(br->GetName()) && blist.find(br->GetName()) == blist.end()) {
				outree->Branch(br->GetName(), br->GetAddress(), br->GetTitle());
                                blist.insert(br->GetName());
			}
		}
		outree->RemoveFriend(t);
	}
	Long64_t nentries = chain->GetEntryList()->GetN();
	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector = new TTreeFormula("selector", cut, chain);
	Long64_t selected = 0;
	for (Long64_t i = 0; i < nentries; ++i) {
		Long64_t ientry = chain->GetEntryNumber(i);
		chain->GetEntry(ientry);
                if (ck_run != fr_run || ck_evt != fr_evt) {
                        fprintf(stderr, "[ERROR] event in the main chain do not correspond to event in the TFriend\n");
                        fprintf(stderr, "--> entry: %ld (main chain) %d %llu vs %d %llu (friend)  %s (tree number: %d)\n", ientry, ck_run, ck_evt, fr_run, fr_evt, chain->GetFile()->GetName(), chain->GetTreeNumber());
                        exit(10);
                }
		if (ientry % 1237 == 0) fprintf(stderr, " Processed events: %lld (%.2f%%)\r", ientry, (Float_t)i / nentries * 100);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			selector->UpdateFormulaLeaves();
		}
		if(selector->EvalInstance() == false) continue;
		++selected;
		outree->Fill();
	}
	std::cout << "[INFO] selected " << selected << " event(s) out of " << nentries << "\n";
	outree->Print();
        outree->GetListOfFriends()->Print();
	outree->AutoSave();
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
	stats statWeight("weight", entries);

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
			if(iEle != 0 && iEle != j + 1) continue;
			//loop over the rooargset and fill it with setVal
			weight = weight_ * pileupWeight_ * r9weight_[0] * r9weight_[1];
			idx->setVal(j);
			mass->setVal(mll);
			smearMass->setVal(mll * sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1])));

			statWeight.add(branches_Float_t[0][0]);
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
	std::cout << "[DEBUG stats] " << statWeight.mean() << "\t" << statWeight.stdDev() << "\t" << statWeight.n() << std::endl;
	return data;
}



/**
 * \retval VOID The method is void, but it prints to one file the region name and the \ref stats.
 *         The name of the output file is the same as the branch used to collect the stat.
 *         Refer to stats to see what is the print format
 */
void anyVar_class::TreeAnalyzeShervin(std::string region, TCut cut_ele1, TCut cut_ele2, float scale, float smearing)
{
	_stats_vec.reset();
	TECALChain *chain_ecal = (TECALChain *)reduced_data;

	Long64_t entries = chain_ecal->GetEntryList()->GetN();
	size_t nBranches = _branchNames.size();

	// _branchList is used to make the set branch addresses
	Float_t branches_Float_t[MAXBRANCHES][3];
	for(unsigned int ibranch = 0; ibranch < nBranches; ++ibranch) {
		//std::cout << "[DEBUG] " << ibranch << std::endl;
		TString& bname = _branchNames[ibranch].first;
		chain_ecal->SetBranchAddress(bname, &branches_Float_t[ibranch]);
	}

	Float_t weight_ = 1 ;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;

	Float_t mll;

	if(chain_ecal->GetBranch("puWeight") != NULL) {
		std::cout << "[STATUS] Adding pileup weight branch from friend" << std::endl;
		chain_ecal->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(chain_ecal->GetBranch("scaleEle") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain_ecal->SetBranchAddress("scaleEle", corrEle_);
	}

	if(chain_ecal->GetBranch("smearEle") != NULL && TString(chain_ecal->GetTitle()) != "d") {
		std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//chain_ecal->Scan("smearEle");
		chain_ecal->SetBranchAddress("smearEle", smearEle_);
	}

	if(chain_ecal->GetBranch("r9Weight") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain_ecal->SetBranchAddress("r9Weight", r9weight_);
	}

	chain_ecal->SetBranchAddress(massBranchName_.c_str(), &mll);

	TStopwatch TT;
	TT.Start();
	chain_ecal->LoadTree(chain_ecal->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector_ele1 = (cut_ele1 != "") ? new TTreeFormula("selector_ele1", cut_ele1, chain_ecal) : NULL;
	TTreeFormula *selector_ele2 = (cut_ele2 != "") ? new TTreeFormula("selector_ele2", cut_ele2, chain_ecal) : NULL;

	std::cout << "[STATUS] anyVar processing: categories "
	          << "\t" << "with " << entries << " entries" << std::endl;
	std::cerr << "[ 00%]";

	TEntryList *exclusiveEventList = new TEntryList(*chain_ecal->GetEntryList());

	for(Long64_t jentry = 0; jentry < entries; ++jentry) {
		Long64_t entryNumber = chain_ecal->GetEntryNumber(jentry);
		chain_ecal->GetEntry(entryNumber);
		if(jentry % (entries / 100) == 0) std::cerr << "\b\b\b\b\b\b[" << std::setw(3) << jentry / (entries / 100) << "%]";

		if (chain_ecal->GetTreeNumber() != treenumber) {
			treenumber = chain_ecal->GetTreeNumber();
			if(selector_ele1 != NULL) selector_ele1->UpdateFormulaLeaves();
			if(selector_ele2 != NULL) selector_ele2->UpdateFormulaLeaves();
		}

		std::array<bool, NELE> passing_ele;
		passing_ele.fill(false);
		if(selector_ele1 != NULL && selector_ele1->EvalInstance() == true) passing_ele[0] = true;
		if(selector_ele2 != NULL && selector_ele2->EvalInstance() == true) passing_ele[1] = true;

#ifdef DEBUG
		if(jentry < 1) {
			std::cout << "[DEBUG] invMass: " << mll << std::endl;
			std::cout << "[DEBUG] PU: " << pileupWeight_ << std::endl;
			std::cout << "[DEBUG] corrEle[0]: " << corrEle_[0] << std::endl;
			std::cout << "[DEBUG] corrEle[1]: " << corrEle_[1] << std::endl;
			std::cout << "[DEBUG] smearEle[0]: " << smearEle_[0] << std::endl;
			std::cout << "[DEBUG] smearEle[1]: " << smearEle_[1] << std::endl;
			std::cout << "[DEBUG] r9weight[0]: " << r9weight_[0] << std::endl;
			std::cout << "[DEBUG] r9weight[1]: " << r9weight_[1] << std::endl;
			chain_ecal->Show(0);
		}
#endif

		// weight = weight_ * pileupWeight_ * r9weight_[0] * r9weight_[1];
		// mass->setVal(mll);
		// smearMass->setVal(mll * sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1])));

		bool bothPassing = true;
		for(size_t iele = 0; iele < passing_ele.size(); ++iele) {
			bothPassing = bothPassing && passing_ele[iele]; //if all electrons are satisfying this category, the event is removed from the list
			if(passing_ele[iele] == false) continue; // fill only the with the electron passing the selection

			for(size_t i = 0; i < nBranches; ++i) {
				_stats_vec[i].add(branches_Float_t[i][iele]*scale);
#ifdef DEBUG
				if(i == 0 && jentry % (entries / 100) == 0) std::cout << i << "\t" << iele << "\t" << branches_Float_t[i][iele] << std::endl;
#endif
			}

		}
		mll *= scale;
		if(bothPassing && mll > 60. && mll < 120) 	_stats_vec[nBranches].add(mll);
		if(_exclusiveCategories && bothPassing) exclusiveEventList->Remove(entryNumber); // remove the event
	}
	fprintf(stderr, "\n");
	TT.Stop();
	std::cout << "[INFO] Running over events: ";
	TT.Print();

	delete selector_ele1;
	delete selector_ele2;
	chain_ecal->ResetBranchAddresses();
	std::cout << "Original number of entries: " << chain_ecal->GetEntryList()->GetN() << std::endl;
	if(_exclusiveCategories && exclusiveEventList->GetN() != chain_ecal->GetEntryList()->GetN()) {
		//delete chain_ecal->GetEntryList();
		//chain_ecal->TECALChain::SetEntryList(exclusiveEventList);
		//chain_ecal->TECALTree::SetEntryList(exclusiveEventList);
		std::cout << "New number of entries: " <<  chain_ecal->GetEntryList()->GetN() << std::endl;

	}


	for(size_t i = 0; i < _stats_vec.size(); ++i) {
		auto& s = _stats_vec[i];
		std::cout << "Start sorting " << s.name() << std::endl;
		s.sort();
		*(_statfiles[i]) << region << "\t" << s << std::endl;
	}
	std::cout << "[INFO] Region fully processed" << std::endl;
//	_stats_vec.dump("testfile.dat");
	return;
}
