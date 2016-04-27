#include "../interface/ZPtWeights_class.hh"
#define PDFWEIGHTINDEXMAX 45

//#define DEBUG

ZPtWeights_class::~ZPtWeights_class()
{

}

ZPtWeights_class::ZPtWeights_class(void):
	warningCounter(0)
{

}


double ZPtWeights_class::GetPtWeight(double ZPt_, int pdfWeightIndex)
{


	TString indexName;
	indexName += pdfWeightIndex;
	ZPtweights_map_t::const_iterator ZPtweights_itr =  ZPtweights.find("ZPtWeight_" + indexName);
	if(ZPtweights_itr == ZPtweights.end()) {
		std::cerr << "[ERROR] Pt weight histogram " << "ZPtWeight_" + indexName << " not found!" << std::endl;
		exit(1);
	} else return (--(ZPtweights_itr->second.upper_bound(ZPt_)))->second;

}




void ZPtWeights_class::ReadFromFile(std::string filename)
{
	std::cout << "ZPT REWEIGHTING:: Using file " << filename << std::endl;

	f_ZPt  = new TFile(filename.c_str(), "READ");

	f_ZPt->cd();

	std::cout << "[INFO] File: " << filename << " opened" << std::endl;
	TList *KeyList = f_ZPt->GetListOfKeys();
	std::cout << "[INFO] Number of entries in file: " << KeyList->GetEntries() << std::endl;
	for(int i = 0; i <  KeyList->GetEntries(); i++) {

		TH1F *h = (TH1F *)((TKey *)KeyList->At(i))->ReadObj();
		//    TString keyName = (KeyList->At(i))->GetName();
		if(h == NULL) std::cerr << "[ERROR] histogram null from file: " << filename << std::endl;
		TString keyName = h->GetName();
		//h->Scale(1./h->Integral());

		// if the electron category is not yet defined, add it
		if (ZPtweights.count(keyName) == 0) {
			ZPtweight_map_t t;
#ifdef DEBUG
			std::cout << "[DEBUG] Adding ZPt weight: " << keyName << std::endl;
#endif
			std::pair<TString, ZPtweight_map_t> p(keyName, t);
			ZPtweights.insert(p);
		} else {
			std::cerr << "[ERROR] histogram name found twice:" << keyName << std::endl;
			exit(1);
		}
		// find the map corresponding to the electron category
		ZPtweights_map_t::iterator map_itr = ZPtweights.find(keyName);
		for(int i_bin = 1; i_bin <= h->GetXaxis()->GetNbins(); i_bin++) {
			// fill the weight in the map
			std::pair<double, float> p(h->GetXaxis()->GetBinLowEdge(i_bin), h->GetBinContent(i_bin));
			map_itr->second.insert(p);
#ifdef DEBUG
			std::cout << "[DEBUG] hist= " << keyName << "\t" << i_bin << "\t"
			          << h->GetXaxis()->GetBinLowEdge(i_bin) << "\t" << h->GetBinContent(i_bin) << "\t"
			          << map_itr->second.lower_bound(h->GetXaxis()->GetBinLowEdge(i_bin))->second
			          << "\t" << map_itr->second.lower_bound(h->GetXaxis()->GetBinCenter(i_bin))->second
			          << std::endl;
#endif

		}
		//    std::cout << keyName << "\t" << h->Integral() << std::endl;
		//    current_dir.cd();
		//    current_dir.ls();
		//    ZPtHistMap[keyName]=(TH1F *) h->Clone(); // dal momento che chiudo il file e' bene farne una copia
		//    if(ZPtHistMap[keyName]!=h) std::cerr << "[ERROR] " << std::endl;
		//    ZPtHistMap[keyName]->Print();
	}

	f_ZPt->Close();

	return;
}


// tree is the input MC tree
// fastLoop = false if for any reason you don't want to change the branch status of the MC tree
TTree *ZPtWeights_class::GetTreeWeight(TChain *tree, TString ZPtBranchName, bool fastLoop)
{
	tree->ResetBranchAddresses();

	Float_t ptWeight[50] = {1.};
	TTree *newTree = new TTree("ZPtWeight", "");
	newTree->Branch("ZPtWeight", ptWeight, "ZPtWeight[45]/F");
	if(fastLoop) {
		tree->SetBranchStatus("*", 0);
		tree->SetBranchStatus(ZPtBranchName, 1);
	}

	Float_t ZPt;
	if(tree->GetBranch(ZPtBranchName) == NULL) {
		std::cerr << "[ERROR] Branch " << ZPtBranchName << " not found, impossible to produce ZPt weights " << std::endl;
		exit(1);
	}
	tree->SetBranchAddress(ZPtBranchName, &ZPt);
	// loop over tree
	Long64_t nentries = tree->GetEntriesFast();
#ifdef DEBUG
	nentries = 10;
#endif

	std::cout << "[STATUS] Get ZPTWeightTree for tree: " << tree->GetTitle()
	          << "\t" << "with " << nentries << " entries" << std::endl;
	std::cerr << "[00%]";
	for(Long64_t ientry = 0; ientry < nentries; ientry++) {
		tree->GetEntry(ientry);
		for(int pdfWeightIndex = 1; pdfWeightIndex < PDFWEIGHTINDEXMAX; pdfWeightIndex++) {
			ptWeight[pdfWeightIndex] =  GetPtWeight(ZPt, pdfWeightIndex);
		}
		newTree->Fill();
		if(ientry % (nentries / 100) == 0) std::cerr << "\b\b\b\b" << std::setw(2) << ientry / (nentries / 100) << "%]";
	}
	std::cout << std::endl;

	if(fastLoop) tree->SetBranchStatus("*", 1);
	tree->ResetBranchAddresses();
	//  std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
	return newTree;
}





