#include <TChain.h>
#include <TTree.h>

TTree *new_branch (TChain *tree)
{

	Float_t invMass_e3x3 = 0;
	Float_t invMass_eSeed = 0;
	Float_t invMass_pModeGsf = 0;

	TTree *newTree = new TTree("new_energy", "");
	newTree->Branch("invMass_e3x3", &invMass_e3x3, "invMass_e3x3/F");
	newTree->Branch("invMass_eSeed", &invMass_eSeed, "invMass_eSeed/F");
	newTree->Branch("invMass_pModeGsf", &invMass_pModeGsf, "invMass_pModeGsf/F");

	Float_t R9Ele[2];
	Float_t rawEnergySCEle[2];
	Float_t eSeedSCEle[2];
	Float_t pModeGsfEle[2];
	Float_t invMass_SC;
	Float_t energySCEle[2];
	Float_t energySCEle_regrCorr_ele[2];
	Float_t invMass_SC_regrCorr_ele;

	tree->SetBranchAddress("R9Ele", R9Ele);
	tree->SetBranchAddress("rawEnergySCEle", rawEnergySCEle);
	tree->SetBranchAddress("eSeedSCEle", eSeedSCEle);
	tree->SetBranchAddress("pModeGsfEle", pModeGsfEle);
	tree->SetBranchAddress("invMass_SC", &invMass_SC);
	tree->SetBranchAddress("energySCEle", energySCEle);
	tree->SetBranchAddress("energySCEle_regrCorr_ele", energySCEle_regrCorr_ele);
	tree->SetBranchAddress("invMass_SC_regrCorr_ele", &invMass_SC_regrCorr_ele);

	Float_t angle = 0;

// loop over tree
	for(Long64_t ientry = 0; ientry < tree->GetEntriesFast(); ientry++) {
		tree->GetEntry(ientry);
		Float_t energy_1 = R9Ele[0] * rawEnergySCEle[0];
		Float_t energy_2 = R9Ele[1] * rawEnergySCEle[1];

		angle = invMass_SC / sqrt(energySCEle[0] * energySCEle[1]);
		invMass_e3x3 = angle * sqrt(energy_1 * energy_2);

		invMass_eSeed = angle * sqrt(eSeedSCEle[0] * eSeedSCEle[1]);

		invMass_pModeGsf = angle * sqrt(pModeGsfEle[0] * pModeGsfEle[1]);

#ifdef DEBUG
		Float_t invMass_SC_regrCorr_ele_tmp = angle * sqrt(energySCEle_regrCorr_ele[0] * energySCEle_regrCorr_ele[1]);
		if(abs(invMass_SC_regrCorr_ele_tmp - invMass_SC_regrCorr_ele) > 0.001) std::cout << "[ERROR] closure test failed" << std::endl;
#endif
		newTree->Fill();
	}


	//if(fastLoop) tree->SetBranchStatus("*",1);
	//std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
	return newTree;
}

