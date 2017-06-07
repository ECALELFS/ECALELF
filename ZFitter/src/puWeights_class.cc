#include "../interface/puWeights_class.hh"
#include <stdlib.h>
#include <TROOT.h>
//#define DEBUG
puWeights_class::~puWeights_class()
{

}

puWeights_class::puWeights_class(void):
	warningCounter(0)
{
	PUweights_itr = PUWeightsRunDepMap.begin();
}



double puWeights_class::GetWeight(int nPU, int runNumber)
{
	if(PUweights_itr->first != runNumber) {
		PUweights_itr = PUWeightsRunDepMap.find(runNumber);
		if(PUweights_itr == PUWeightsRunDepMap.end() || PUweights_itr->second.empty()) {
			warningCounter++;
			if(warningCounter <= 10) {
				std::cerr << "[WARNING] runNumber " << runNumber << " not found in PUWeightsRunDepMap" << std::endl;
			}
			return 1;
		}
	}

	PUweights_t::const_iterator weight_itr = PUweights_itr->second.find(nPU);
	if(weight_itr == PUweights_itr->second.end()) {
		warningCounter++;
		if(warningCounter <= 10) {
			std::cerr << "[WARNING] in-time nPU " << nPU << " not found in weight files" << std::endl;
			std::cerr << "          Events dropped with applying weight=0" << std::endl;
			std::cerr << (PUweights_itr->second.begin()->first) << std::endl;
		}
		return 0;
	}


	return weight_itr->second;
}


void puWeights_class::ReadFromFile(std::string filename)
{
	std::cout << "PU REWEIGHTING:: Using file " << filename << std::endl;

	PUWeightsRunDepMap_t::iterator weights_itr = PUWeightsRunDepMap.begin();
	if(weights_itr == PUWeightsRunDepMap.end()) {
		PUweights_t PUWeights;
		weights_itr = (PUWeightsRunDepMap.insert(std::pair<int, PUweights_t>(1, PUWeights))).first;
	} else {
		weights_itr->second.clear();
	}

	TFile f_pu(filename.c_str(), "READ");

	f_pu.cd();

	TH1D *puweights = 0;
	TH1D *gen_pu = 0;

	gen_pu = (TH1D*) f_pu.Get("generated_pu");
	puweights = (TH1D*) f_pu.Get("weights");


	if (!puweights || !gen_pu) {
		std::cout << "weights histograms  not found in file " << filename << std::endl;
		return;
	}


	TH1D* weightedPU = (TH1D*)gen_pu->Clone("weightedPU");
	weightedPU->Multiply(puweights);
	//Rescaling weights in order to preserve same integral of events
	TH1D* weights = (TH1D*)puweights->Clone("rescaledWeights");
	weights->Scale( gen_pu->Integral(1, MAX_PU_REWEIGHT) / weightedPU->Integral(1, MAX_PU_REWEIGHT) );

	float sumPuWeights = 0.;

	for (int i = 0; i < MAX_PU_REWEIGHT; i++) {
		float weight = 1.;
		weight = weights->GetBinContent(i + 1);
		sumPuWeights += weight;
		//      PUweights[i]=weight;
		PUWeightsRunDepMap.begin()->second.insert(std::pair<int, double>(i, weight));
	}

	//     TFile PUfile(json_filename,"read");
	//     TH1F *PUhist = (TH1F *) PUfile.Get("weight");
	//     if(PUhist==NULL){
	//       std::cerr << "ERROR: pileup weights histogram not found in " << json_filename << std::endl;
	//       return; // mettere un exit
	//     }
	//     PUweights = generate_flat10_weights(PUhist);
	//     PUfile.Close();

	f_pu.Close();

	return;
}

void puWeights_class::ReadFromFiles(std::string mcPUFile, std::string dataPUFile, int runMin)
{
	runMin = 1; //FIXME
	PUWeightsRunDepMap_t::iterator weights_itr = PUWeightsRunDepMap.find(runMin);
	if(weights_itr == PUWeightsRunDepMap.end()) {
		PUweights_t PUWeights;
		weights_itr = (PUWeightsRunDepMap.insert(std::pair<int, PUweights_t>(runMin, PUWeights))).first;
	} else {
		weights_itr->second.clear();
	}

	std::cout << "[STATUS] Reading PU histograms from files: " << mcPUFile << "\t" << dataPUFile << std::endl;
	TFile data(dataPUFile.c_str());
	TFile mc(mcPUFile.c_str());

	if(!data.IsOpen() || !mc.IsOpen()) {
		std::cerr << "[ERROR] data or mc PU file not opened" << std::endl;
		exit(1);
		return;
	}

	TH1D* puMC_hist = (TH1D*) mc.Get("pileup");
	if(puMC_hist == NULL) {
		TTree *puMC_tree = (TTree*) mc.Get("PUDumper/pileup");
		if(puMC_tree == NULL) {
			std::cerr << "[ERROR] mc pileup histogram is NULL" << std::endl;
			exit(1);
			return;
		}
		puMC_tree->Draw("nPUtrue>>pileupHist_mc(60,0,60)", "BX==0");
		puMC_hist = (TH1D *) gROOT->FindObject("pileupHist_mc");
		puMC_hist->SaveAs("tmp/puMC_hist.root");
	}

	TH1D *puData_hist = (TH1D *) data.Get("pileup");
	//  puData_hist->GetBinWidth(3);
	if(puData_hist == NULL) {
		puData_hist = (TH1D *) data.Get("PUDumper/pileup");
		if(puData_hist == NULL) {
			std::cerr << "[ERROR] data pileup histogram is NULL" << std::endl;
			exit(1);
			return;
		}
	}

	// they should have bin width = 1
	if(puMC_hist->GetBinWidth(2) != 1) {
		std::cerr << "[ERROR] Bin width for mc pileup distribution different from 1" << std::endl;
		exit(1);
		return;
	}

	if(puData_hist->GetBinWidth(2) != 1) {
		std::cerr << "[ERROR] Bin width for data pileup distribution different from 1" << std::endl;
		exit(1);
		return;
	}

	// they should have the same binning otherwise PU weights until the max between them
	int nBins = MAX_PU_REWEIGHT;
	if(puData_hist->GetNbinsX() != puMC_hist->GetNbinsX()) {
		std::cerr << "[WARNING] Different binning between data and mc pileup distributions:" << puData_hist->GetNbinsX() << "\t" << puMC_hist->GetNbinsX() << std::endl;
		nBins = std::min(puData_hist->GetNbinsX(), puMC_hist->GetNbinsX());
		//std::cerr << "Pileup reweighted until nPU max = " << nBins;
	}

	nBins = std::min(MAX_PU_REWEIGHT, nBins);

	float puMC_int = puMC_hist->Integral(0, nBins);
	float puData_int = puData_hist->Integral(0, nBins);
	puMC_hist->Scale(1. / puMC_int);
	puData_hist->Scale(1. / puData_int);

	double puMCweight_int = 0;
	for (int i = 0; i < nBins; i++) {
		double binContent = puMC_hist->GetBinContent(i + 1);
		//Give warnings if the Pileup bins are empty
		if(binContent == 0 && puData_hist->GetBinContent(i + 1) != 0) {
			std::cerr << "[WARNING] mc bin empty while data not: iBin = " << i + 1 << std::endl;
			std::cerr << "          data bin = " << puData_hist->GetBinContent(i + 1) << std::endl;
		}
		double weight = binContent > 0 ? puData_hist->GetBinContent(i + 1) / binContent : 0;
		// the MC normalization should not change, so calculate the
		// integral of the MC and rescale the weights by that
		puMCweight_int += weight * binContent;
		weights_itr->second.insert(std::pair<int, double>(i, weight));
#ifdef DEBUG
		std::cout << "[DEBUG] " << weights_itr->first << "\t" << weights_itr->second[i] << "\t" << weights_itr->second.size() << std::endl;
#endif
	}

	for (int i = 0; i < nBins; i++) {
		weights_itr->second[i] /= puMCweight_int;
	}

	PUweights_itr = PUWeightsRunDepMap.begin();
	if(TString(puMC_hist->GetName()).Contains("Hist")) delete puMC_hist;
	return;
}

// tree is the input MC tree
// fastLoop = false if for any reason you don't want to change the branch status of the MC tree
TTree *puWeights_class::GetTreeWeight(TChain *tree,  bool fastLoop, TString nPUbranchName)
{
	TString runNumberBranchName = "runNumber";
	Float_t weight = 0;
	TTree *newTree = new TTree("pileup", nPUbranchName);
	newTree->Branch("puWeight", &weight, "puWeight/F");

	if(fastLoop) {
		tree->SetBranchStatus("*", 0);
		tree->SetBranchStatus(nPUbranchName, 1);
		tree->SetBranchStatus(runNumberBranchName, 1);
	}

	UChar_t nPU;
	Int_t runNumber;
	tree->SetBranchAddress(nPUbranchName, &nPU);
	tree->SetBranchAddress(runNumberBranchName, &runNumber);
	// loop over tree
	for(Long64_t ientry = 0; ientry < tree->GetEntriesFast(); ientry++) {
		tree->GetEntry(ientry);
		weight = GetWeight((int)nPU, runNumber); //only intime pu
		newTree->Fill();
	}


	if(fastLoop) tree->SetBranchStatus("*", 1);
	tree->ResetBranchAddresses();
	std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
	return newTree;
}


