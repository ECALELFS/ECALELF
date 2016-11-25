#include "../interface/r9Weights_class.hh"

r9Weights_class::~r9Weights_class()
{

}

r9Weights_class::r9Weights_class(void):
	warningCounter(0)
{

}


double r9Weights_class::GetPtWeight(double PtEle_)
{
	r9weights_map_t::const_iterator r9weights_itr =  r9weights.find("R9PtWeightAllAll");
	if(r9weights_itr == r9weights.end()) {
		std::cerr << "[ERROR] Pt weight histogram not found!" << std::endl;
		return -1;
	} else return (--(r9weights_itr->second.upper_bound(PtEle_)))->second;

}

double r9Weights_class::GetWeight(double etaEle_, double R9Ele_)
{
#ifdef DEBUG
	std::cout << "[DEBUG] GetWeight: " << etaEle_ << "\t" << R9Ele_ << std::endl;
#endif

	double r9Weight = 1.;
	TString categoryName;

	if(fabs(etaEle_) < 1.) categoryName = "EBlowEta";
	else if(fabs(etaEle_) >= 1. && fabs(etaEle_) <= 1.479) categoryName = "EBhighEta";
	else if(fabs(etaEle_) > 1.479 && fabs(etaEle_) < 2) categoryName = "EElowEta";
	else if(fabs(etaEle_) >= 2) categoryName = "EEhighEta";
	else {
		std::cerr << "Category not found: "
		          << "etaEle = " << etaEle_ << "\t"
		          << "R9Ele = " << R9Ele_ << std::endl;
		return 0;
	}
	if (R9Ele_ >= 0.94) categoryName += "Gold";
	else //if (R9Ele_<0.94 )
		categoryName += "Bad";

	if(categoryName.Sizeof() > 1) { // altrimenti non e' delle categorie definite sopra
		std::vector<TString> WeightCat_vec;
		WeightCat_vec.push_back("R9EtaWeight"); // weights of the eta distribution
		WeightCat_vec.push_back("R9Weight");    // weights of the r9 distribution

		r9weights_map_t::const_iterator r9weights_itr; // iterator pointing to one histogram
		for(std::vector<TString>::const_iterator itrV = WeightCat_vec.begin();
		        itrV != WeightCat_vec.end();
		        itrV++) {
			TString hist_prefix = *itrV;
			if(r9weights.count(hist_prefix + categoryName) == 0) {
				std::cerr << "[ERROR]: " << hist_prefix + categoryName << " not found in file " << std::endl;
				std::cerr << etaEle_ << "\t" << R9Ele_ << std::endl;

				exit(1);
				continue;
			}

			r9weights_itr = r9weights.find(hist_prefix + categoryName);
#ifdef DEBUG
			std::cout << hist_prefix << "\t" << categoryName << "\t";
#endif

			r9weight_map_t::const_iterator itr; // first element less than x
			if(hist_prefix.Contains("Eta")) {
				itr = --(r9weights_itr->second.upper_bound(fabs(etaEle_))); // first element less than x
			} else {
				itr = --(r9weights_itr->second.upper_bound(fabs(R9Ele_))); // first element less than x
			}
			float r9Weight_0 = itr->second;
#ifdef DEBUG
			std::cout << etaEle_ << "\t" << R9Ele_ << "\t" << r9Weight_0 << std::endl;
#endif
			if (r9Weight_0 < 0) {
				std::cerr << "[ERROR] weight < 0" << std::endl;
				exit(1);
			}
			r9Weight *= r9Weight_0; // mi assicuro che il peso non sia nullo


		} // altrimenti lascio invariato r9Weight
#ifdef DEBUG
		std::cout << std::endl;
#endif
	}

	return r9Weight;
}



void r9Weights_class::ReadFromFile(std::string filename)
{
	std::cout << "R9 REWEIGHTING:: Using file " << filename << std::endl;

	f_r9  = new TFile(filename.c_str(), "READ");

	f_r9->cd();

	std::cout << "[INFO] File: " << filename << " opened" << std::endl;
	TList *KeyList = f_r9->GetListOfKeys();
	std::cout << "[INFO] Number of entries in file: " << KeyList->GetEntries() << std::endl;
	for(int i = 0; i <  KeyList->GetEntries(); i++) {

		TH1F *h = (TH1F *)((TKey *)KeyList->At(i))->ReadObj();
		//    TString keyName = (KeyList->At(i))->GetName();
		if(h == NULL) std::cerr << "[ERROR] histogram null from file: " << filename << std::endl;
		TString keyName = h->GetName();
		//h->Scale(1./h->Integral());

		// if the electron category is not yet defined, add it
		if (r9weights.count(keyName) == 0) {
			r9weight_map_t t;
			std::pair<TString, r9weight_map_t> p(keyName, t);
			r9weights.insert(p);
		} else {
			std::cerr << "[ERROR] histogram name found twice:" << keyName << std::endl;
			exit(1);
		}
		// find the map corresponding to the electron category
		r9weights_map_t::iterator map_itr = r9weights.find(keyName);
		for(int i_bin = 1; i_bin <= h->GetXaxis()->GetNbins(); i_bin++) {
			// fill the weight in the map
			std::pair<double, float> p(h->GetXaxis()->GetBinLowEdge(i_bin), h->GetBinContent(i_bin));
			map_itr->second.insert(p);
#ifdef DEBUG
			std::cout << "[DEBUG] hist= " << keyName << "\t" << i_bin << "\t"
			          << h->GetXaxis()->GetBinLowEdge(i_bin) << "\t" << h->GetBinContent(i_bin) << "\t"
			          << map_itr->second.lower_bound(h->GetXaxis()->GetBinLowEdge(i_bin))->second
			          << std::endl;
#endif

		}
		//    std::cout << keyName << "\t" << h->Integral() << std::endl;
		//    current_dir.cd();
		//    current_dir.ls();
		//    r9HistMap[keyName]=(TH1F *) h->Clone(); // dal momento che chiudo il file e' bene farne una copia
		//    if(r9HistMap[keyName]!=h) std::cerr << "[ERROR] " << std::endl;
		//    r9HistMap[keyName]->Print();
	}

	f_r9->Close();

	return;
}


// tree is the input MC tree
// fastLoop = false if for any reason you don't want to change the branch status of the MC tree
TTree *r9Weights_class::GetTreeWeight(TChain *tree,  bool fastLoop, TString etaElebranchName, TString R9ElebranchName, TString ptElebranchName)
{
	tree->ResetBranchAddresses();

	Float_t weight[2] = {0.};
	Float_t ptWeight[2] = {0.};
	TTree *newTree = new TTree("r9Weight", "");
	newTree->Branch("r9Weight", weight, "r9Weight[2]/F");
	newTree->Branch("ptWeight", ptWeight, "ptWeight[2]/F");
	if(fastLoop) {
		tree->SetBranchStatus("*", 0);
		tree->SetBranchStatus(etaElebranchName, 1);
		tree->SetBranchStatus(R9ElebranchName, 1);
		tree->SetBranchStatus(ptElebranchName, 1);
	}

	Float_t etaEle[2];
	Float_t R9Ele[2];
	Float_t PtEle[2];
	tree->SetBranchAddress(etaElebranchName, etaEle);
	tree->SetBranchAddress(R9ElebranchName, R9Ele);
	tree->SetBranchAddress(ptElebranchName, PtEle);

	// loop over tree
	Long64_t nentries = tree->GetEntriesFast();
#ifdef DEBUG
	nentries = 10;
#endif

	std::cout << "[STATUS] Get R9WeightTree for tree: " << tree->GetTitle()
	          << "\t" << "with " << nentries << " entries" << std::endl;
	std::cerr << "[00%]";
	for(Long64_t ientry = 0; ientry < nentries; ientry++) {
		tree->GetEntry(ientry);
		weight[0] = GetWeight(etaEle[0], R9Ele[0]);
		weight[1] = GetWeight(etaEle[1], R9Ele[1]);
		ptWeight[0] = GetPtWeight(PtEle[0]);
		ptWeight[1] = GetPtWeight(PtEle[1]);
		newTree->Fill();
		if(ientry % (nentries / 100) == 0) std::cerr << "\b\b\b\b" << std::setw(2) << ientry / (nentries / 100) << "%]";
	}
	std::cout << std::endl;

	if(fastLoop) tree->SetBranchStatus("*", 1);
	tree->ResetBranchAddresses();
	//  std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
	return newTree;
}





