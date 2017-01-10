#include "../interface/anyVar_class.h"
#include <TTreeFormula.h>
#include <TObject.h>
#include <TBranchElement.h>
#include <TFriendElement.h>
#include <TStopwatch.h>
//#define DEBUG
#define MAXENTRIES 100000
#define MAXBRANCHES  20
#define BUFSIZE 12 /* bytes */
#include <cassert>


anyVar_class::~anyVar_class(void)
{
	std::cerr << "[anyVar_class DESTROYING]" << std::endl;
//	data_chain->SetEntryList(NULL);
//	reduced_data_vec.clear();
	std::cerr << "[anyVar_class DESTROY]" << std::endl;
//	delete data_chain;
}

anyVar_class::anyVar_class(TChain *data_chain_, std::vector<std::string> branchNames, ElectronCategory_class& cutter, std::string massBranchName, std::string outDirFitRes, TDirectory* dir, bool updateOnly):
	data_chain(data_chain_),
	reduced_data(NULL),
	_dir(dir),
	_branchNames(branchNames),
	_cutter(cutter),
	massBranchName_(massBranchName),
	_outDirFitRes(outDirFitRes)
{

	Long64_t entries = data_chain->GetEntriesFast();
	for(auto& branch : _branchNames) {
		TString bname = branch;
		std::cout << bname << std::endl;
		if(updateOnly) _statfiles.emplace_back(new std::ofstream(outDirFitRes + bname + ".dat", std::ofstream::app));
		else _statfiles.emplace_back(new std::ofstream(outDirFitRes + bname + ".dat"));

		stats s(bname.Data(), entries);
		_stats_vec.push_back(s);
	}

	//SetOutDirName(outDirFitRes, updateOnly);
//	if(updateOnly) _statfiles.emplace_back(new std::ofstream(outDirFitRes + massBranchName_ + ".dat",  std::ofstream::app));
//	else _statfiles.emplace_back(new std::ofstream(outDirFitRes + massBranchName_ + ".dat"));
//	stats s(massBranchName_, entries);
//	_stats_vec.push_back(s);

	for(size_t i = 0; i < _stats_vec.size(); ++i) {
		auto& s = _stats_vec[i];
		*(_statfiles[i]) << "" << "catName" << "\t" << s.printHeader() << std::endl;
	}

}

void anyVar_class::SetOutDirName(std::string dirname, bool updateOnly)
{
	_outDirFitRes = dirname;
	system(("mkdir -p " + dirname).c_str());
	_statfiles.clear(); ///momory leak?
	for(auto& branch : _branchNames) {
		TString bname = branch;
		if(updateOnly) _statfiles.emplace_back(new std::ofstream(_outDirFitRes + bname + ".dat", std::ofstream::app));
		else _statfiles.emplace_back(new std::ofstream(_outDirFitRes + bname + ".dat"));
	}

	// if(updateOnly) _statfiles.emplace_back(new std::ofstream(_outDirFitRes + massBranchName_ + ".dat",  std::ofstream::app));
	// else _statfiles.emplace_back(new std::ofstream(_outDirFitRes + massBranchName_ + ".dat"));

	for(size_t i = 0; i < _stats_vec.size(); ++i) {
		auto& s = _stats_vec[i];
		*(_statfiles[i]) << "catName" << "\t" << s.printHeader() << std::endl;
	}

}

// branchList is the list of branches used for selection of categories
void anyVar_class::Import(TString commonCut, TString eleID_, std::set<TString>& branchList, unsigned int modulo)
{
	assert(branchList.size() > 0);
	commonCut += "-eleID_" + eleID_;
	TCut dataCut = _cutter.GetCut(commonCut, false);
	std::set<TString> commonCutBranches = _cutter.GetBranchNameNtuple(commonCut);
	std::cout << "------------------------------ IMPORT DATASETS" << std::endl;
	std::cout << "--------------- Importing: " << data_chain->GetEntries() << std::endl;
	//if(data!=NULL) delete data;
	ImportTree(data_chain, dataCut, commonCutBranches, branchList, modulo);
	//std::cout << "[INFO] imported "  << (data_chain->GetEntryList())->GetN() << " events passing commmon cuts" << std::endl;
	//data->Print();

	//data_chain->SetEntryList(NULL);
	data_chain->SetBranchStatus("*", 0);
	std::cout << "--------------- IMPORT finished" << std::endl;
	return;
}


void anyVar_class::ImportTree(TChain *chain, TCut& commonCut, std::set<TString>& commonCutBranches, std::set<TString>& branchList, unsigned int modulo)
{
	std::cout << "[anyVar_class][STATUS] Disabling branches to fast import" << std::endl;
	chain->SetBranchStatus("*", 0);

	for(auto& branch : commonCutBranches) {
		if(branch.Sizeof()==0) continue;
		std::cout << "[anyVar_class][STATUS] Enabling branch: " << branch << std::endl;
		chain->SetBranchStatus(branch, 1);
//		chain->AddBranchToCache(branch, kTRUE);
	}

	// abilitare la lista dei friend branch
	if(chain->GetBranch("scaleEle"))  chain->SetBranchStatus("scaleEle", 1);
	if(chain->GetBranch("smearEle")) chain->SetBranchStatus("smearEle", 1);
	if(chain->GetBranch("puWeight")) chain->SetBranchStatus("puWeight", 1);
	if(chain->GetBranch("r9Weight")) chain->SetBranchStatus("r9Weight", 1);

	//chain->SetBranchStatus(massBranchName_.c_str(), 1);
	//chain->SetBranchStatus("eleID", 1);
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

	chain->SetBranchStatus("*" , 0);

	TEntryList *elist = (TEntryList*)gROOT->FindObject(evListName);
	assert(elist != NULL);
#ifdef DEBUG
	elist->Print();
#endif
	TECALChain *chain_ecal = (TECALChain*)chain;
	chain_ecal->TECALChain::SetEntryList(elist);

	std::cout << "[INFO] Selected events: " <<  chain_ecal->GetEntryList()->GetN() << std::endl;
	assert(chain_ecal->GetEntryList()->GetN()>0);
	//chain = dynamic_cast<TChain*>(chain_ecal);
	ts.Stop();
	ts.Print();

	// updates the reduced_data
	TreeToTree(chain, branchList, modulo);

	return;
}


void anyVar_class::TreeToTree(TChain *chain, std::set<TString>& branchList, unsigned int modulo)
{
	assert(modulo > 0);
	assert(branchList.size() > 0);
	std::cout << "[anyVar_class][STATUS] Start copying the tree to memory" << std::endl;
	_dir->cd();
	TStopwatch ts;
	ts.Start();
	reduced_data_vec.clear();


	char buffer[BUFSIZE * MAXBRANCHES]; //4bytes * 3 in the array * number of branches
	char *bufferPtr = buffer;

	size_t buffIndex = 0;
	for(unsigned int i = 0; i < modulo; ++i) {
		char title[50];
		sprintf(title, "%s_mod_%d", chain->GetName(), i);

		reduced_data_vec.emplace_back(new TTree(title, title));
		TTree *reduced_data = reduced_data_vec[i].get();
//		reduced_data->SetDirectory(_dir);
	}

	chain->SetBranchStatus("*", 0);
	std::set<TString> allBranches;
	allBranches.insert(branchList.begin(), branchList.end());

	for(auto& branch : _branchNames) {
		allBranches.insert(branch);
	}

	for(auto branch : allBranches) {
		std::cout << "[anyVar_class][STATUS] Enabling branch: " << branch << std::endl;
		chain->SetBranchStatus(branch, 1);
		TBranch * br = chain->GetBranch(branch);
		chain->SetBranchAddress(branch, bufferPtr); //bufferPtr);
		assert(buffIndex < BUFSIZE * MAXBRANCHES);

		for(unsigned int i = 0; i < modulo; ++i) {
			TTree *reduced_data = reduced_data_vec[i].get();
			TString title = br->GetTitle();
			title.ReplaceAll("[3]", "[2]");
			//TBranch *brr = reduced_data->Branch(br->GetName(), &bufferPtr, title);
			reduced_data->Branch(br->GetName(), bufferPtr, title);
		}
		buffIndex += std::max(br->GetEntry(0), BUFSIZE); // BUFSIZE; //12 bytes
		bufferPtr += std::max(br->GetEntry(0), BUFSIZE); // BUFSIZE; //12 bytes
	}
//	reduced_data_vec[0]->Print();

#ifdef DEBUG
	std::cout << "[DEBUG] Friends removed and branches added" << std::endl;
#endif
	Long64_t nentries = chain->GetEntryList()->GetN();
	Long64_t treenumber = -1;
	TTreeFormula *selector = NULL; //(cut == "" ) ? NULL : new TTreeFormula("selector", cut, chain);

	for (Long64_t i = 0; i < nentries; ++i) {
		if(i % (nentries / 100) == 0) std::cerr << "\b\b\b\b\b\b[" << std::setw(3) << i / (nentries / 100) << "%]";

		Long64_t ientry = chain->GetEntryNumber(i);
		Long64_t jentry = chain->LoadTree(ientry);// entry in the tree

		Long64_t nb = chain->GetEntry(ientry);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			if(selector != NULL) selector->UpdateFormulaLeaves();
		}
		if(selector != NULL && selector->EvalInstance() == false) continue;
		if(modulo == 1) {
			reduced_data_vec[0]->Fill();
		} else {
			reduced_data_vec[i % modulo]->Fill();
		}
	}
	std::cout << "[reset branch address]" << std::endl;
	chain->ResetBranchAddresses();

	for(auto &reduced_data : reduced_data_vec) {
		reduced_data->ResetBranchAddresses();
	}

#ifdef DEBUG
	std::cout << "[DEBUG] Entries in tree copied" << std::endl;
#endif
	std::cout << "[DEBUG] Entries in tree copied" << std::endl;

	reduced_data_vec[0]->Print();
	if(reduced_data_vec[0]->GetEntries() > 0) reduced_data_vec[0]->Show(0);

	if(selector != NULL) delete selector;

	ts.Stop();
	std::cout << "Copy tree done: ";
	ts.Print();
	ChangeModulo(0);

}




/**
 * \retval VOID The method is void, but it prints to one file the region name and the \ref stats.
 *         The name of the output file is the same as the branch used to collect the stat.
 *         Refer to stats to see what is the print format
 */
void anyVar_class::TreeAnalyzeShervin(std::string region, TCut cut_ele1, TCut cut_ele2, float scale, float smearing)
{
	_stats_vec.reset();
	if(reduced_data == NULL) {
		std::cerr << "[ERROR] reduced_data is NULL. Maybe you have to call the Import() method" << std::endl;
		return;
	}

// this makes sure that the same category is not processed twice
	bool doProcess = true;
	std::string file = _outDirFitRes + "/" + massBranchName_ + ".dat";
	std::string s = "grep -q  " + region + " " + file;
	doProcess = doProcess && !(system(s.c_str()));
	if(doProcess == true) {
//		for(auto& branch : _branchNames) {
		doProcess = doProcess && (system(s.c_str()));
//			if(doProcess==false) break;
//		}
	}
	if(doProcess == true) return;


	Float_t *mll = NULL;
	Float_t branches_Float_t[MAXBRANCHES][3];
	size_t nBranches = _branchNames.size();
	size_t indexMassBranch =-1 ;
	for(unsigned int ibranch = 0; ibranch < nBranches; ++ibranch) {
		//std::cout << "[DEBUG] " << ibranch << std::endl;
		TString bname = _branchNames[ibranch];
		reduced_data->SetBranchAddress(bname, &branches_Float_t[ibranch]);
		if(bname == massBranchName_){
			mll = &(branches_Float_t[ibranch][0]);
			indexMassBranch = ibranch;
		}
	}

	Float_t weight_ = 1 ;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;


	if(reduced_data->GetBranch("puWeight") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding pileup weight branch from friend" << std::endl;
		reduced_data->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(reduced_data->GetBranch("scaleEle") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding electron energy correction branch from friend" << std::endl;
		reduced_data->SetBranchAddress("scaleEle", corrEle_);
	}

	if(reduced_data->GetBranch("smearEle") != NULL && TString(reduced_data->GetTitle()) != "d") {
		std::cout << "[anyVar_class][STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//reduced_data->Scan("smearEle");
		reduced_data->SetBranchAddress("smearEle", smearEle_);
	}

	if(reduced_data->GetBranch("r9Weight") != NULL) {
		std::cout << "[anyVar_class][STATUS] Adding electron energy correction branch from friend" << std::endl;
		reduced_data->SetBranchAddress("r9Weight", r9weight_);
	}

	TStopwatch TT;
	TT.Start();
	reduced_data->LoadTree(reduced_data->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector_ele1 = (cut_ele1 != "") ? new TTreeFormula("selector_ele1", cut_ele1, reduced_data) : NULL;
	TTreeFormula *selector_ele2 = (cut_ele2 != "") ? new TTreeFormula("selector_ele2", cut_ele2, reduced_data) : NULL;

	Long64_t entries = reduced_data->GetEntriesFast(); //yList()->GetN();

	std::cout << "[anyVar_class][STATUS] anyVar processing: category " << region
	          << "\t" << "with " << entries << " entries" << std::endl;
//	std::cerr << "[ 00%]";


	Long64_t count = 0;
	//for(Long64_t jentry = 0; jentry < entries; ++jentry) {
	for(auto& jentry : goodEntries) {
		Long64_t entryNumber = jentry; //reduced_data->GetEntryNumber(jentry);
		reduced_data->GetEntry(entryNumber);
//		if(jentry % (entries / 100) == 0) std::cerr << "\b\b\b\b\b\b[" << std::setw(3) << jentry / (entries / 100) << "%]";

		if (reduced_data->GetTreeNumber() != treenumber) {
			treenumber = reduced_data->GetTreeNumber();
			if(selector_ele1 != NULL) selector_ele1->UpdateFormulaLeaves();
			if(selector_ele2 != NULL) selector_ele2->UpdateFormulaLeaves();
		}

		std::array<bool, NELE> passing_ele;
		passing_ele.fill(false);
		if(selector_ele1 != NULL && selector_ele1->EvalInstance() == true) passing_ele[0] = true;
		if(selector_ele2 != NULL && selector_ele2->EvalInstance() == true) passing_ele[1] = true;
		if(passing_ele[0] && passing_ele[1]) count++;
#ifdef DEBUG
		if(jentry < 1) {
			std::cout << "[DEBUG] invMass: " << *mll << std::endl;
			std::cout << "[DEBUG] PU: " << pileupWeight_ << std::endl;
			std::cout << "[DEBUG] corrEle[0]: " << corrEle_[0] << std::endl;
			std::cout << "[DEBUG] corrEle[1]: " << corrEle_[1] << std::endl;
			std::cout << "[DEBUG] smearEle[0]: " << smearEle_[0] << std::endl;
			std::cout << "[DEBUG] smearEle[1]: " << smearEle_[1] << std::endl;
			std::cout << "[DEBUG] r9weight[0]: " << r9weight_[0] << std::endl;
			std::cout << "[DEBUG] r9weight[1]: " << r9weight_[1] << std::endl;
			reduced_data->Show(0);
		}
#endif

		// weight = weight_ * pileupWeight_ * r9weight_[0] * r9weight_[1];
		// mass->setVal(mll);
		// smearMass->setVal(mll * sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1])));

		bool bothPassing = true;
		for(size_t iele = 0; iele < passing_ele.size(); ++iele) {
			bothPassing = bothPassing && passing_ele[iele]; //if all electrons are satisfying this category, the event is removed from the list
			if(passing_ele[iele] == false) continue; // fill only the with the electron passing the selection

			for(size_t i = 0; i < _stats_vec.size(); ++i) {
				auto& s = _stats_vec[i];
				if(s.name().find("invMass")!=std::string::npos) continue;
				_stats_vec[i].add(branches_Float_t[i][iele]*scale);
#ifdef DEBUG
				if(i == 0 && jentry % (entries / 100) == 0) std::cout << i << "\t" << iele << "\t" << branches_Float_t[i][iele] << std::endl;
#endif
			}

		}
		//double mllNew = mll * scale;
		*mll *= scale;
		if(bothPassing && *mll > 60. && *mll < 120) 	_stats_vec[indexMassBranch].add(*mll);
		if(_exclusiveCategories && bothPassing) goodEntries.erase(jentry);
	}
	//fprintf(stderr, "\n");
	TT.Stop();
	if(selector_ele1 != NULL) delete selector_ele1;
	if(selector_ele2 != NULL) delete selector_ele2;

	std::cout << "[INFO] Running over events: ";
	TT.Print();

	reduced_data->ResetBranchAddresses();

	if(_exclusiveCategories && (Long64_t) goodEntries.size() != entries) {
		std::cout << "Reduced number of events: " << entries << " -> " <<  goodEntries.size() << std::endl;
	}


	for(size_t i = 0; i < _stats_vec.size(); ++i) {
		auto& s = _stats_vec[i];
		std::cout << "Start sorting " << s.name() << std::endl;
		s.sort();
		*(_statfiles[i]) << region << "\t" << s << std::endl;
#ifdef DEBUG
		std::cout << region << "\t" << s << std::endl;
#endif
	}
	std::cout << "[INFO] Region fully processed" << "\t" << count << std::endl;
//	_stats_vec.dump("testfile.dat");
	_stats_vec.reset();
	return;
}


