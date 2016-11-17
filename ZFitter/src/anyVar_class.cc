#include "../interface/anyVar_class.h"
#include <TTreeFormula.h>
#include <TObject.h>
#include <TBranchElement.h>
#include <TFriendElement.h>
#include <TStopwatch.h>
//#define DEBUG
#define MAXENTRIES 10000
#define MAXBRANCHES  20
#include <cassert>

anyVar_class::~anyVar_class(void)
{
}

anyVar_class::anyVar_class(TChain *data_chain_, std::vector<std::pair<TString, kType> > branchNames, ElectronCategory_class& cutter, std::string massBranchName, std::string outDirFitRes, bool updateOnly):
	data_chain(data_chain_),
	reduced_data(NULL),
	dir(),
	_branchNames(branchNames),
	_cutter(cutter),
	massBranchName_(massBranchName),
	_outDirFitRes(outDirFitRes)
{

	Long64_t entries = data_chain->GetEntriesFast();
	for(auto& branch : _branchNames) {
		TString& bname = branch.first;
		if(updateOnly) _statfiles.emplace_back(new std::ofstream(outDirFitRes + bname + ".dat", std::ofstream::app));
		else _statfiles.emplace_back(new std::ofstream(outDirFitRes + bname + ".dat"));
		
		stats s(bname.Data(), entries);
		_stats_vec.push_back(s);
	}

	//SetOutDirName(outDirFitRes, updateOnly);
	if(updateOnly) _statfiles.emplace_back(new std::ofstream(outDirFitRes + massBranchName_ + ".dat",  std::ofstream::app));
	else _statfiles.emplace_back(new std::ofstream(outDirFitRes + massBranchName_ + ".dat"));
	stats s(massBranchName_, entries);
	_stats_vec.push_back(s);

	for(size_t i = 0; i < _stats_vec.size(); ++i) {
		auto& s = _stats_vec[i];
		*(_statfiles[i]) << "#H" << "catName" << s.printHeader() << std::endl;
	}

}

void anyVar_class::SetOutDirName(std::string dirname, bool updateOnly)
{
	_outDirFitRes = dirname;
	system(("mkdir -p "+dirname).c_str());
	_statfiles.clear(); ///momory leak?
	for(auto& branch : _branchNames) {
		TString& bname = branch.first;
		if(updateOnly) _statfiles.emplace_back(new std::ofstream(_outDirFitRes + bname + ".dat", std::ofstream::app));
		else _statfiles.emplace_back(new std::ofstream(_outDirFitRes + bname + ".dat"));
	}

	if(updateOnly) _statfiles.emplace_back(new std::ofstream(_outDirFitRes + massBranchName_ + ".dat",  std::ofstream::app));
	else _statfiles.emplace_back(new std::ofstream(_outDirFitRes + massBranchName_ + ".dat"));

	for(size_t i = 0; i < _stats_vec.size(); ++i) {
		auto& s = _stats_vec[i];
		*(_statfiles[i]) << s.printHeader() << std::endl;
	}

}

void anyVar_class::Import(TString commonCut, TString eleID_, std::set<TString>& branchList, unsigned int modulo)
{
	commonCut += "-eleID_" + eleID_;
	TCut dataCut = _cutter.GetCut(commonCut, false);
	std::cout << "------------------------------ IMPORT DATASETS" << std::endl;
	std::cout << "--------------- Importing: " << data_chain->GetEntries() << std::endl;
	//if(data!=NULL) delete data;
	TChain *data = ImportTree(data_chain, dataCut, branchList, modulo);
	std::cout << "[INFO] imported "  << (data->GetEntryList())->GetN() << " events passing commmon cuts" << std::endl;
	//data->Print();
	std::cout << "--------------- IMPORT finished" << std::endl;
	return;
}


TChain *anyVar_class::ImportTree(TChain *chain, TCut commonCut, std::set<TString>& branchList, unsigned int modulo)
{

	if(branchList.size() > 0) {
		std::cout << "[anyVar_class][STATUS] Disabling branches to fast import" << std::endl;
		chain->SetBranchStatus("*", 0);
	}

	for(auto branch : branchList) {
		std::cout << "[anyVar_class][STATUS] Enabling branch: " << branch << std::endl;
		chain->SetBranchStatus(branch, 1);
		chain->AddBranchToCache(branch, kTRUE);
	}

	for(auto branch : _branchNames) {
		std::cout << "[anyVar_class][STATUS] Enabling branch: " << branch.first << std::endl;
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
	// updates the reduced_data
	TreeToTreeShervin(chain, "", modulo);
#else
	TreeToTree(chain, "1");
#endif

	return chain;
}


void anyVar_class::TreeToTreeShervin(TChain *chain, TCut cut, unsigned int modulo)
{
	assert(modulo>0);
	std::cout << "[anyVar_class][STATUS] Start copying the tree in memory" << std::endl;
	TStopwatch ts;
	ts.Start();
	reduced_data_vec.clear();
		
	TIter next(chain->GetListOfFriends()); // cannot use reduced_data otherwise crashes
	TObject * obj = NULL;
	bool firstLoop=true;
	while ((obj = next())) {
		TTree *t = ((TFriendElement *)obj)->GetTree();
		TObjArray* branches= t->GetListOfBranches();
		Int_t nb = branches->GetEntriesFast();
		for (Int_t i = 0; i < nb; ++i) {
			TBranch * br = chain->GetBranch(branches->At(i)->GetName());
			if (chain->GetBranchStatus(br->GetName())) {
				for(unsigned int i=0; i < modulo; ++i){
					if(firstLoop){
						reduced_data_vec.emplace_back(chain->CloneTree(0));
					}
					TTree *reduced_data = reduced_data_vec[i].get();
					if(firstLoop){
						char title[50];
						sprintf(title, "%s_mod_%d", reduced_data->GetTitle(), i);
#ifdef DEBUG
						std::cout << "[DEBUG] " << modulo << "\t" << i << "\t" << title << std::endl;
#endif
						reduced_data->SetDirectory(&dir);
						reduced_data->SetTitle(title);
						reduced_data->RemoveFriend(reduced_data->GetFriend(t->GetName())); // only way to avoid memory leak
					}
					reduced_data->Branch(br->GetName(), br->GetAddress(), br->GetTitle());
				}
				firstLoop=false;
			}
		}
	}

//#ifdef DEBUG
	std::cout << "[DEBUG] Friends removed and branches added" << std::endl;
//#endif
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
	TTreeFormula *selector = (cut == "" ) ? NULL : new TTreeFormula("selector", cut, chain);
	for (Long64_t i = 0; i < nentries; ++i) {
		Long64_t ientry = chain->GetEntryNumber(i);
		chain->GetEntry(ientry);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			if(selector != NULL) selector->UpdateFormulaLeaves();
		}
		if(selector != NULL && selector->EvalInstance() == false) continue;
		reduced_data_vec[i%modulo]->Fill();
	}

#ifdef DEBUG
	std::cout << "[DEBUG] Entries in tree copied" << std::endl;
#endif

	for(auto& reduced_data : reduced_data_vec){
		TString evListName = "evList_";
		evListName += reduced_data->GetTitle();
		evListName += "_red";

		reduced_data->Draw(">>" + evListName, "", "entrylist");
		TEntryList *elist = (TEntryList*)gROOT->FindObject(evListName);
		assert(elist != NULL);
		elist->Print();
		reduced_data->SetEntryList(elist);
	}
	reduced_data_vec[0]->Print();

	if(selector!=NULL) delete selector;

	ts.Stop();
	std::cout << "Copy tree done: ";
	ts.Print();
	ChangeModulo(0);
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

	Float_t weight_ = 1 ;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;

	Float_t mll;

	if(chain->GetBranch("puWeight") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding pileup weight branch from friend" << std::endl;
		chain->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(chain->GetBranch("scaleEle") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("scaleEle", corrEle_);
	}

	if(chain->GetBranch("smearEle") != NULL && TString(chain->GetTitle()) != "d") {
		std::cout << "[anyVar_class][STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//chain->Scan("smearEle");
		chain->SetBranchAddress("smearEle", smearEle_);
	}

	if(chain->GetBranch("r9Weight") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("r9Weight", r9weight_);
	}
	TFile * outFile = TFile::Open("dataset.root", "recreate");
	TTree * outree = chain->CloneTree(0);
	//chain->GetEntry(0);
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
			if (chain->GetBranchStatus(br->GetName())) {
				outree->Branch(br->GetName(), br->GetAddress(), br->GetTitle());
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





/**
 * \retval VOID The method is void, but it prints to one file the region name and the \ref stats.
 *         The name of the output file is the same as the branch used to collect the stat.
 *         Refer to stats to see what is the print format
 */
void anyVar_class::TreeAnalyzeShervin(std::string region, TCut cut_ele1, TCut cut_ele2, float scale, float smearing)
{
	_stats_vec.reset();
	if(reduced_data==NULL){
		std::cerr << "[ERROR] reduced_data is NULL. Maybe you have to call the Import() method" << std::endl;
		return;
	}

// this makes sure that the same category is not processed twice
	bool doProcess=true;
	std::string file=_outDirFitRes+"/"+massBranchName_+".dat";
	std::string s="grep -q  "+region+" "+file;
	doProcess = doProcess && !(system(s.c_str()));
	if(doProcess==true){
	for(auto& branch : _branchNames) {
		TString& bname = branch.first;
		doProcess = doProcess && (system(s.c_str()));
		if(doProcess==false) break;
	}
	}
	if(doProcess==true) return;

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
		std::cout << "[anyVar_class][STATUS] Adding pileup weight branch from friend" << std::endl;
		chain_ecal->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(chain_ecal->GetBranch("scaleEle") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain_ecal->SetBranchAddress("scaleEle", corrEle_);
	}

	if(chain_ecal->GetBranch("smearEle") != NULL && TString(chain_ecal->GetTitle()) != "d") {
		std::cout << "[anyVar_class][STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//chain_ecal->Scan("smearEle");
		chain_ecal->SetBranchAddress("smearEle", smearEle_);
	}

	if(chain_ecal->GetBranch("r9Weight") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain_ecal->SetBranchAddress("r9Weight", r9weight_);
	}

	chain_ecal->SetBranchAddress(massBranchName_.c_str(), &mll);

	TStopwatch TT;
	TT.Start();
	chain_ecal->LoadTree(chain_ecal->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector_ele1 = (cut_ele1 != "") ? new TTreeFormula("selector_ele1", cut_ele1, chain_ecal) : NULL;
	TTreeFormula *selector_ele2 = (cut_ele2 != "") ? new TTreeFormula("selector_ele2", cut_ele2, chain_ecal) : NULL;

	std::cout << "[anyVar_class][STATUS] anyVar processing: categories "
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
		double mllNew = mll * scale;
		if(bothPassing && mll > 60. && mll < 120) 	_stats_vec[nBranches].add(mllNew);
		if(_exclusiveCategories && bothPassing) exclusiveEventList->Remove(entryNumber); // remove the event
	}
	fprintf(stderr, "\n");
	TT.Stop();
	std::cout << "[INFO] Running over events: ";
	TT.Print();

	if(selector_ele1!=NULL) delete selector_ele1;
	if(selector_ele2!=NULL) delete selector_ele2;
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
		//*(_statfiles[i]) << "#H" << s.printHeader() << std::endl;
		*(_statfiles[i]) << region << "\t" << s << std::endl;
	}
	std::cout << "[INFO] Region fully processed" << std::endl;
//	_stats_vec.dump("testfile.dat");
	return;
}
