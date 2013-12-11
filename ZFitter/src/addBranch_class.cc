#include "../interface/addBranch_class.hh"
#include "../interface/ElectronCategory_class.hh"
#include <TTreeFormula.h>

//#define NOFRIEND

addBranch_class::addBranch_class(void):
  scaler(NULL)
{
}

addBranch_class::~addBranch_class(void){
}

  /** \param originalChain standard ntuple
   *  \param treename name of the new tree (not important)
   *  \param BranchName invMassSigma or iSMEle (important, define which new branch you want)
   */
TTree *addBranch_class::AddBranch(TChain* originalChain, TString treename, TString BranchName, bool fastLoop, bool isMC){
  if(BranchName.Contains("invMassSigma")) return AddBranch_invMassSigma(originalChain, treename, BranchName, fastLoop);
  if(BranchName.CompareTo("iSM")==0)       return AddBranch_iSM(originalChain, treename, BranchName, fastLoop);
  if(BranchName.CompareTo("smearerCat")==0)       return AddBranch_smearerCat(originalChain, treename, isMC);

  std::cerr << "[ERROR] Request to add branch " << BranchName << " but not defined" << std::endl;
  return NULL;
}

TTree* addBranch_class::AddBranch_Pt(TChain* originalChain, TString treename){
  //sanity checks

  TTree* newtree = new TTree(treename, treename);
  //delete branches
  //TBranch *b = originalChain-> GetBranch("name of branch");
  //originalChain->GetListOfBranches()->Remove(b);

  //add pt branches
  Float_t         phiEle[2];
  Float_t         etaEle[2];
  Float_t         energyEle_regrCorr_fra[2];
  Float_t         energySCEle_regrCorr_ele[2];
  Float_t         energySCEle_regrCorr_pho[2];
  TBranch        *b_etaEle;   
  TBranch        *b_phiEle;   
  TBranch        *b_energySCEle_regrCorr_ele;   
  TBranch        *b_energySCEle_regrCorr_pho;   
  TBranch        *b_energyEle_regrCorr_fra;   
  originalChain->SetBranchAddress("etaEle", etaEle, &b_etaEle);
  originalChain->SetBranchAddress("phiEle", phiEle, &b_phiEle);
  originalChain->SetBranchAddress("energySCEle_regrCorr_ele", energySCEle_regrCorr_ele, &b_energySCEle_regrCorr_ele);
  originalChain->SetBranchAddress("energySCEle_regrCorr_pho", energySCEle_regrCorr_pho, &b_energySCEle_regrCorr_pho);
  originalChain->SetBranchAddress("energyEle_regrCorr_fra", energyEle_regrCorr_fra, &b_energyEle_regrCorr_fra);

  float ZPt_regrCorr_pho, ZPt_regrCorr_ele, ZPt_regrCorr_fra;
  newtree->Branch("ZPt_regrCorr_fra", &ZPt_regrCorr_fra, "ZPt_regrCorr_fra/F");
  newtree->Branch("ZPt_regrCorr_ele", &ZPt_regrCorr_ele, "ZPt_regrCorr_ele/F");
  newtree->Branch("ZPt_regrCorr_pho", &ZPt_regrCorr_pho, "ZPt_regrCorr_pho/F");
  //px = pt*cosphi; py = pt*sinphi; pz = pt*sinh(eta)
  //p^2 = E^2 - m^2 = pt^2*(1+sinh^2(eta)) = pt^2*(cosh^2(eta))
  float mass = 0.000511;
  for(Long64_t ientry = 0; ientry<originalChain->GetEntriesFast(); ientry++){
	originalChain->GetEntry(ientry);
	float regrCorr_fra_pt0 = sqrt((pow(energyEle_regrCorr_fra[0],2)-mass*mass)/(1+sinh(etaEle[0])*sinh(etaEle[0])));
	float regrCorr_fra_pt1 = sqrt((pow(energyEle_regrCorr_fra[1],2)-mass*mass)/(1+sinh(etaEle[1])*sinh(etaEle[1])));
	ZPt_regrCorr_fra = 
	  TMath::Sqrt(pow(regrCorr_fra_pt0*TMath::Sin(phiEle[0])+regrCorr_fra_pt1*TMath::Sin(phiEle[1]),2)+pow(regrCorr_fra_pt0*TMath::Cos(phiEle[0])+regrCorr_fra_pt1*TMath::Cos(phiEle[1]),2));

	float regrCorr_ele_pt0 = sqrt((pow(energySCEle_regrCorr_ele[0],2)-mass*mass)/(1+sinh(etaEle[0])*sinh(etaEle[0])));
	float regrCorr_ele_pt1 = sqrt((pow(energySCEle_regrCorr_ele[1],2)-mass*mass)/(1+sinh(etaEle[1])*sinh(etaEle[1])));
	ZPt_regrCorr_ele = 
	  TMath::Sqrt(pow(regrCorr_ele_pt0*TMath::Sin(phiEle[0])+regrCorr_ele_pt1*TMath::Sin(phiEle[1]),2)+pow(regrCorr_ele_pt0*TMath::Cos(phiEle[0])+regrCorr_ele_pt1*TMath::Cos(phiEle[1]),2));

	float regrCorr_pho_pt0 = sqrt((pow(energySCEle_regrCorr_pho[0],2)-mass*mass)/(1+sinh(etaEle[0])*sinh(etaEle[0])));
	float regrCorr_pho_pt1 = sqrt((pow(energySCEle_regrCorr_pho[1],2)-mass*mass)/(1+sinh(etaEle[1])*sinh(etaEle[1])));
	ZPt_regrCorr_pho = 
	  TMath::Sqrt(pow(regrCorr_pho_pt0*TMath::Sin(phiEle[0])+regrCorr_pho_pt1*TMath::Sin(phiEle[1]),2)+pow(regrCorr_pho_pt0*TMath::Cos(phiEle[0])+regrCorr_pho_pt1*TMath::Cos(phiEle[1]),2));
	newtree->Fill();
  }
  originalChain->ResetBranchAddresses();
  return newtree;
}


TTree* addBranch_class::AddBranch_invMassSigma(TChain* originalChain, TString treename, TString invMassSigmaName, bool fastLoop){
  if(scaler==NULL) exit(1);

  //sanity checks
  TString etaEleBranchName="etaEle", phiEleBranchName="phiEle", etaSCEleBranchName="etaSCEle";
  TString R9EleBranchName="R9Ele";
  TString energyBranchName, invMassBranchName, sigmaEnergyBranchName;
#ifndef NOFRIEND
  TTree *newtree = new TTree(treename, treename);
#else 
  TTree *newtree = originalChain->CloneTree();
#endif
  if(newtree==NULL){
    std::cerr << "[ERROR] New tree for branch " << invMassSigmaName << " is NULL" << std::endl;
    exit(1);
  }
  //delete branches
  //TBranch *b = originalChain-> GetBranch("name of branch");
  //originalChain->GetListOfBranches()->Remove(b);

  Int_t runNumber;
  //add pt branches
  Float_t         phiEle[2];
  Float_t         etaEle[2];
  Float_t         energyEle[2];
  Float_t         sigmaEnergyEle[2];
  Float_t         invMass;
  //Float_t         smearEle[2]={1,1};
  
  Float_t etaSCEle_[2];
  Float_t R9Ele_[2];

  //TBranch         *smearEle_b;
  if(invMassSigmaName=="invMassSigma_SC"){ 
    invMassBranchName="invMass_SC";
    energyBranchName="energySCEle";
    sigmaEnergyBranchName="";
    std::cerr << "[ERROR] No energy error estimation for std. SC" << std::endl;
    exit(1);
  } else if(invMassSigmaName=="invMassSigma_SC_regrCorr_ele"){
    invMassBranchName="invMass_SC_regrCorr_ele";
    energyBranchName="energySCEle_regrCorr_ele";
    sigmaEnergyBranchName="energySigmaSCEle_regrCorr_ele";
  } else if(invMassSigmaName=="invMassSigma_SC_regrCorr_pho"){ 
    energyBranchName="energySCEle_regrCorr_pho";
    invMassBranchName="invMass_SC_regrCorr_pho";
    sigmaEnergyBranchName="energySigmaSCEle_regrCorr_pho";    
  } else if(invMassSigmaName=="invMassSigma_regrCorr_fra"){
    invMassBranchName="invMass_regrCorr_fra";
    energyBranchName="energyEle_regrCorr_fra";
    sigmaEnergyBranchName="energysigmaEle_regrCorr_fra";    
  }

  if(fastLoop){
    originalChain->SetBranchStatus("*",0);
    originalChain->SetBranchStatus("runNumber",1);
    originalChain->SetBranchStatus(etaEleBranchName,1);
    originalChain->SetBranchStatus(etaSCEleBranchName,1);
    originalChain->SetBranchStatus(phiEleBranchName,1);
    originalChain->SetBranchStatus(etaSCEleBranchName,1);
    originalChain->SetBranchStatus(R9EleBranchName,1);

    //originalChain->SetBranchStatus(smearEleBranchName, true);
    originalChain->SetBranchStatus(energyBranchName,1);
    originalChain->SetBranchStatus(sigmaEnergyBranchName,1);
    originalChain->SetBranchStatus(invMassBranchName,1);

  }
  originalChain->SetBranchAddress("runNumber", &runNumber);
  if(originalChain->SetBranchAddress(etaEleBranchName, etaEle) < 0){
    std::cerr << "[ERROR] Branch etaEle not defined" << std::endl;
    exit(1);
  }
  if(originalChain->SetBranchAddress(phiEleBranchName, phiEle) < 0) exit(1);
  //if(originalChain->SetBranchAddress(smearEleBranchName, smearEle) < 0){
  //std::cerr << "[ERROR] Branch smearEle not defined" << std::endl;
  //exit(1);
  //}
  originalChain->SetBranchAddress(etaSCEleBranchName,etaSCEle_);
  originalChain->SetBranchAddress(R9EleBranchName, R9Ele_);

  if(originalChain->SetBranchAddress(energyBranchName, energyEle) < 0 ) exit(1);
  originalChain->SetBranchAddress(sigmaEnergyBranchName, sigmaEnergyEle);
  originalChain->SetBranchAddress(invMassBranchName, &invMass);

  Float_t invMassSigma;
  newtree->Branch(invMassSigmaName, &invMassSigma, invMassSigmaName+"/F");

  for(Long64_t ientry = 0; ientry<originalChain->GetEntriesFast(); ientry++){

    originalChain->GetEntry(ientry);
    float smearEle_[2];
    smearEle_[0] = scaler->getSmearingSigma(runNumber,energyEle[0], fabs(etaSCEle_[0]) < 1.4442, 
			       R9Ele_[0],etaEle[0]);
    smearEle_[1] = scaler->getSmearingSigma(runNumber,energyEle[1], fabs(etaSCEle_[1]) < 1.4442, 
			       R9Ele_[1],etaEle[1]);

    invMassSigma = 0.5 * invMass * sqrt( 
					sigmaEnergyEle[0]/energyEle[0] * sigmaEnergyEle[0]/energyEle[0] +
					sigmaEnergyEle[1]/energyEle[1] * sigmaEnergyEle[1]/energyEle[1] +
					smearEle_[0] * smearEle_[0] + smearEle_[1] * smearEle_[1]
					);
    newtree->Fill();
  }

  if(fastLoop)   originalChain->SetBranchStatus("*",1);
  originalChain->ResetBranchAddresses();
  return newtree;
}



TTree* addBranch_class::AddBranch_iSM(TChain* originalChain, TString treename, TString iSMEleName, bool fastLoop){

  TString seedXSCEleBranchName="seedXSCEle", seedYSCEleBranchName="seedYSCEle";

  TTree *newtree = new TTree(treename, treename);

  Float_t       seedXSCEle_[2];
  Float_t       seedYSCEle_[2];
  

  if(fastLoop){
    originalChain->SetBranchStatus("*",0);
    originalChain->SetBranchStatus(seedXSCEleBranchName,1);
    originalChain->SetBranchStatus(seedYSCEleBranchName,1);
  }

  if(originalChain->SetBranchAddress(seedXSCEleBranchName, seedXSCEle_) < 0){
    std::cerr << "[ERROR] Branch seedXSCEle not defined" << std::endl;
    exit(1);
  }

  if(originalChain->SetBranchAddress(seedYSCEleBranchName, seedYSCEle_) < 0){
    std::cerr << "[ERROR] Branch seedYSCEle not defined" << std::endl;
    exit(1);
  }


  Int_t iSM_[2];
  newtree->Branch(iSMEleName, iSM_, iSMEleName+"[2]/I");

  for(Long64_t ientry = 0; ientry<originalChain->GetEntriesFast(); ientry++){
    iSM_[0]=-1;
    iSM_[1]=-1;
    originalChain->GetEntry(ientry);
    if(seedXSCEle_[0]!=0){
      if(seedXSCEle_[0] > 0) {
	// EB+
	iSM_[0] = (int)((seedYSCEle_[0]-1))/20 + 1;
      } else {
	// EB-
	iSM_[0] = (int)((seedYSCEle_[0]-1))/20 + 19;
      }
    }
    
    if(seedYSCEle_[1] !=0){
      if(seedXSCEle_[1] > 0) {
	// EB+
	iSM_[1] = (int)((seedYSCEle_[1]-1))/20 + 1;
      } else {
      // EB-
	iSM_[1] = (int)((seedYSCEle_[1]-1))/20 + 19;
      }
    }
    if(ientry < 10) std::cout << seedXSCEle_[0] << "\t" << seedYSCEle_[0] << "\t" << iSM_[0] << std::endl;
    if(ientry < 10) std::cout << seedXSCEle_[1] << "\t" << seedYSCEle_[1] << "\t" << iSM_[1] << std::endl;
    if(seedXSCEle_[1] < 0 && iSM_[1] < 19) std::cout << seedXSCEle_[1] << "\t" << seedYSCEle_[1] << "\t" << iSM_[1] << std::endl;
    newtree->Fill();
  }

  if(fastLoop)   originalChain->SetBranchStatus("*",1);
  originalChain->ResetBranchAddresses();
  return newtree;
}



// branch with the smearing category index
TTree* addBranch_class::AddBranch_smearerCat(TChain* originalChain, TString treename, bool isMC){

  ElectronCategory_class cutter;
  if(originalChain->GetBranch("scaleEle")!=NULL){
    cutter._corrEle=true;
    std::cout << "[INFO] Activating scaleEle for smearerCat" << std::endl;
    
  }
  TString oddString="";

  //setting the new tree
  TTree *newtree = new TTree(treename, treename);
  Int_t  smearerCat[2];
  Char_t cat1[10];
  sprintf(cat1,"XX");
  newtree->Branch("smearerCat", smearerCat, "smearerCat[2]/I");
  newtree->Branch("catName", cat1, "catName/C");
  //  newtree->Branch("catName2", cat2, "catName2/C");
  
  /// \todo disable branches using cutter
  originalChain->SetBranchStatus("*",0);
  
  std::vector< std::pair<TTreeFormula*, TTreeFormula*> > catSelectors;
  for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
      region_ele1_itr != _regionList.end();
      region_ele1_itr++){

    // \todo activating branches // not efficient in this loop
    std::set<TString> branchNames = cutter.GetBranchNameNtuple(*region_ele1_itr);
    for(std::set<TString>::const_iterator itr = branchNames.begin();
	itr != branchNames.end(); itr++){
      originalChain->SetBranchStatus(*itr, 1);
    }
    if(    cutter._corrEle==true) originalChain->SetBranchStatus("scaleEle", 1);


    for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
	region_ele2_itr != _regionList.end();
	region_ele2_itr++){
      
      if(region_ele2_itr==region_ele1_itr){
	TString region=*region_ele1_itr;
	region.ReplaceAll(_commonCut,""); //remove the common Cut!
	TTreeFormula *selector = new TTreeFormula("selector-"+(region), cutter.GetCut(region+oddString, isMC), originalChain);
	catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(selector,NULL));
	//selector->Print();
	//std::cout << cutter.GetCut(region+oddString, isMC) << std::endl;
	//exit(0);
      } else {
	TString region1=*region_ele1_itr;
	TString region2=*region_ele2_itr;
	region1.ReplaceAll(_commonCut,"");
	region2.ReplaceAll(_commonCut,"");
	TTreeFormula *selector1 = new TTreeFormula("selector1-"+region1+region2, 
						   cutter.GetCut(region1+oddString, isMC, 1) && 
						   cutter.GetCut(region2+oddString, isMC, 2),
						   originalChain);
	TTreeFormula *selector2 = new TTreeFormula("selector2-"+region1+region2,
						   cutter.GetCut(region1+oddString, isMC, 2) && 
						   cutter.GetCut(region2+oddString, isMC, 1),
						   originalChain);
	catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(selector1,selector2));
	//selector1->Print();
	//	selector2->Print();
	//exit(0);
      } 
	
    }
  }


  Long64_t entries = originalChain->GetEntries();
  originalChain->LoadTree(originalChain->GetEntryNumber(0));
  Long64_t treenumber=-1;

  std::cout << "[STATUS] Get smearerCat for tree: " << originalChain->GetTitle() 
	    << "\t" << "with " << entries << " entries" << std::endl;
  std::cerr << "[00%]";

  for(Long64_t jentry=0; jentry < entries; jentry++){
    originalChain->GetEntry(jentry);
    if (originalChain->GetTreeNumber() != treenumber) {
      treenumber = originalChain->GetTreeNumber();
      for(std::vector< std::pair<TTreeFormula*, TTreeFormula*> >::const_iterator catSelector_itr = catSelectors.begin();
	  catSelector_itr != catSelectors.end();
	  catSelector_itr++){
	
	catSelector_itr->first->UpdateFormulaLeaves();
	if(catSelector_itr->second!=NULL)       catSelector_itr->second->UpdateFormulaLeaves();
      }
    }

    int evIndex=-1;
    bool _swap=false;
    for(std::vector< std::pair<TTreeFormula*, TTreeFormula*> >::const_iterator catSelector_itr = catSelectors.begin();
	catSelector_itr != catSelectors.end() && evIndex<0;
	catSelector_itr++){
      _swap=false;
      TTreeFormula *sel1 = catSelector_itr->first;
      TTreeFormula *sel2 = catSelector_itr->second;
      //if(sel1==NULL) continue; // is it possible?
      if(sel1->EvalInstance()==false){
	if(sel2==NULL || sel2->EvalInstance()==false) continue;
	else{
	  _swap=true;
	  //sprintf(cat1,"%s", sel2->GetName());
	}
      } //else sprintf(cat1,"%s", sel1->GetName());
      
      evIndex=catSelector_itr-catSelectors.begin();
    }
    
    smearerCat[0]=evIndex;
    smearerCat[1]=_swap ? 1 : 0;
    newtree->Fill();
    if(jentry%(entries/100)==0) std::cerr << "\b\b\b\b" << std::setw(2) << jentry/(entries/100) << "%]";
  }
  std::cout << std::endl;

  //if(fastLoop) 
  originalChain->SetBranchStatus("*",1);
  originalChain->ResetBranchAddresses();
  return newtree;
}



#ifdef shervinM



TTree *addBranch_class::GetTreeDistIEtaDistiPhi(TChain *tree,  bool fastLoop, int xDist, int yDist, TString iEtaBranchName, TString iPhiBranchName){

  
  Int_t iEta;
  Int_t iPhi;
  TTree *newTree = new TTree("distIEtaIPhi","");
  newTree->Branch("distIEta", &distIEta, "distIEta/I");
  newTree->Branch("distIPhi", &distIPhi, "distIPhi/I");

  if(fastLoop){
    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus(iEtaBranchName,1);
    tree->SetBranchStatus(iPhiBranchName,1);
  }

  Int_t seedXSCEle[2];
  Int_t seedYSCEle[2];
  tree->SetBranchAddress(iEtaBranchName, seedXSCEle);
  tree->SetBranchAddress(iPhiBranchName, seedYSCEle);

  // loop over tree 
  for(Long64_t ientry = 0; ientry<tree->GetEntriesFast(); ientry++){
    tree->GetEntry(ientry);
    weight = GetWeight((int)nPU[0]); //only intime pu
    newTree->Fill();
  }

  
  if(fastLoop) tree->SetBranchStatus("*",1);
  tree->ResetBranchAddresses();
  std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
  return newTree;
}



  
#endif

