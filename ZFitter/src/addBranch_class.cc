#include "../interface/addBranch_class.hh"
#include "../interface/ElectronCategory_class.hh"
#include <TTreeFormula.h>
#include <TLorentzVector.h>
#include <iostream> 

//#define DEBUG
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
  if(BranchName.Contains("invMassSigma")) return AddBranch_invMassSigma(originalChain, treename, BranchName, fastLoop, isMC);
  if(BranchName.CompareTo("iSM")==0)       return AddBranch_iSM(originalChain, treename, BranchName, fastLoop);
  if(BranchName.CompareTo("smearerCat")==0)       return AddBranch_smearerCat(originalChain, treename, isMC);
  if(BranchName.Contains("ZPt"))   return AddBranch_ZPt(originalChain, treename, BranchName.ReplaceAll("ZPt_",""), fastLoop);
  std::cerr << "[ERROR] Request to add branch " << BranchName << " but not defined" << std::endl;
  return NULL;
}

TTree* addBranch_class::AddBranch_ZPt(TChain* originalChain, TString treename, TString energyBranchName, bool fastLoop){
  //sanity checks

  TTree* newtree = new TTree(treename, treename);

  //add pt branches
  Float_t         phiEle[2];
  Float_t         etaEle[2];
  Float_t         energyEle[2];
  Float_t         corrEle[2]={1.,1.};
  Float_t         ZPt, ZPta;
  TLorentzVector ele1,ele2;

  originalChain->SetBranchAddress("etaEle", etaEle);
  originalChain->SetBranchAddress("phiEle", phiEle);
  originalChain->SetBranchAddress(energyBranchName, energyEle);

  if(fastLoop){
    originalChain->SetBranchStatus("*",0);
    originalChain->SetBranchStatus("etaEle",1);
    originalChain->SetBranchStatus("phiEle",1);
    originalChain->SetBranchStatus(energyBranchName,1);
    if(originalChain->GetBranch("scaleEle")!=NULL){
      std::cout << "[STATUS] Adding electron energy correction branch from friend " << originalChain->GetTitle() << std::endl;
      originalChain->SetBranchAddress("scaleEle", corrEle);
    }
  }

  newtree->Branch("ZPt_"+energyBranchName, &ZPt, "ZPt/F");
  //px = pt*cosphi; py = pt*sinphi; pz = pt*sinh(eta)
  //p^2 = E^2 - m^2 = pt^2*(1+sinh^2(eta)) = pt^2*(cosh^2(eta))
  float mass = 0.; //0.000511;
  Long64_t nentries= originalChain->GetEntries();
  for(Long64_t ientry = 0; ientry< nentries; ientry++){
    originalChain->GetEntry(ientry);
    float regrCorr_fra_pt0 = sqrt(((energyEle[0]*energyEle[0])-mass*mass)/(1+sinh(etaEle[0])*sinh(etaEle[0])));
    float regrCorr_fra_pt1 = sqrt(((energyEle[1]*energyEle[1])-mass*mass)/(1+sinh(etaEle[1])*sinh(etaEle[1])));
    ZPt = 
      TMath::Sqrt(pow(regrCorr_fra_pt0*TMath::Sin(phiEle[0])+regrCorr_fra_pt1*TMath::Sin(phiEle[1]),2)+pow(regrCorr_fra_pt0*TMath::Cos(phiEle[0])+regrCorr_fra_pt1*TMath::Cos(phiEle[1]),2));

    ele1.SetPtEtaPhiE(energyEle[0]/cosh(etaEle[0]), etaEle[0], phiEle[0], energyEle[0]);
    ele2.SetPtEtaPhiE(energyEle[1]/cosh(etaEle[1]), etaEle[1], phiEle[1], energyEle[1]);
    ZPta = (ele1+ele2).Pt();
    if(fabs(ZPt - ZPta)>0.001){
      std::cerr << "[ERROR] ZPt not well calculated" << ZPt << "\t" << ZPta << std::endl;
      exit(1);
    }

    newtree->Fill();
  }

  originalChain->SetBranchStatus("*",1);
  originalChain->ResetBranchAddresses();
  return newtree;
}


TTree* addBranch_class::AddBranch_invMassSigma(TChain* originalChain, TString treename, TString invMassSigmaName, bool fastLoop, bool isMC){
  if(scaler==NULL){
    std::cerr << "[ERROR] EnergyScaleCorrection class not initialized" << std::endl;
    exit(1);
  }

  //sanity checks
  TString etaEleBranchName="etaEle", phiEleBranchName="phiEle", etaSCEleBranchName="etaSCEle";
  TString R9EleBranchName="R9Ele";
  TString energyBranchName, invMassBranchName, sigmaEnergyBranchName;

  TTree *newtree = new TTree(treename, treename);

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
  Float_t         corrEle[2]={1.,1.};
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
  }else if(invMassSigmaName.Contains("SemiPar")){
    //    invMassSigmaRelBranchName=invMassSigmaName;
    //    invMassSigmaRelBranchName.ReplaceAll("Sigma","SigmaRel");

    invMassBranchName=invMassSigmaName;
    invMassBranchName.ReplaceAll("Sigma","");
    energyBranchName=invMassBranchName;
    energyBranchName.ReplaceAll("invMass_SC","energySCEle");
    sigmaEnergyBranchName=energyBranchName;
    sigmaEnergyBranchName.ReplaceAll("energySCEle","energySigmaSCEle");
  }else{
    std::cerr << "[ERROR] Energy branch and invMass branch for invMassSigma = " << invMassSigmaName << " not defined" << std::endl;
    exit(1);
  }

  if(fastLoop){
    originalChain->SetBranchStatus("*",0);
    originalChain->SetBranchStatus("runNumber",1);
    originalChain->SetBranchStatus(etaEleBranchName,1);
    originalChain->SetBranchStatus(etaSCEleBranchName,1);
    originalChain->SetBranchStatus(phiEleBranchName,1);
    originalChain->SetBranchStatus(etaSCEleBranchName,1);
    originalChain->SetBranchStatus(R9EleBranchName,1);
    if(originalChain->GetBranch("scaleEle")!=NULL){
      std::cout << "[STATUS] Adding electron energy correction branch from friend " << originalChain->GetTitle() << std::endl;
      originalChain->SetBranchAddress("scaleEle", corrEle);
    }

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

  Float_t invMassSigma; //, invMassSigmaRel;
  newtree->Branch(invMassSigmaName, &invMassSigma, invMassSigmaName+"/F");
  //  newtree->Branch(invMassSigmaRelBranchName, &invMassSigmaRel, invMassSigmaRelBranchName+"/F");

  for(Long64_t ientry = 0; ientry<originalChain->GetEntriesFast(); ientry++){

    originalChain->GetEntry(ientry);
    float smearEle_[2];
    smearEle_[0] = scaler->getSmearingRho(runNumber,energyEle[0], fabs(etaSCEle_[0]) < 1.4442, 
			       R9Ele_[0],etaSCEle_[0]);
    smearEle_[1] = scaler->getSmearingRho(runNumber,energyEle[1], fabs(etaSCEle_[1]) < 1.4442, 
			       R9Ele_[1],etaSCEle_[1]);
    if(smearEle_[0]==0 || smearEle_[1] ==0){
      std::cerr << "[ERROR] Smearing = 0 " << "\t" << smearEle_[0] << "\t" << smearEle_[1] << std::endl;
      std::cout << "E_0: " << runNumber << "\t" << energyEle[0] << "\t" 
		<< etaSCEle_[0] << "\t" << R9Ele_[0] << "\t" << etaEle[0] << "\t" << smearEle_[0] << "\t" << invMass << "\t" << corrEle[0] << "\t" << invMassSigma << "\t" << sigmaEnergyEle[0] << std::endl;
      std::cout << "E_1: " << runNumber << "\t" << energyEle[1] << "\t" 
		<< etaSCEle_[1] << "\t" << R9Ele_[1] << "\t" << etaEle[1] << "\t" << smearEle_[1] << "\t" << sigmaEnergyEle[1] << "\t" << corrEle[1] << std::endl;
      exit(1);
    }
    if(isMC) invMass*=sqrt(
			   scaler->getSmearing(runNumber,energyEle[0], fabs(etaSCEle_[0]) < 1.4442, 
							    R9Ele_[0],etaSCEle_[0])
			   *
			   scaler->getSmearing(runNumber,energyEle[1], fabs(etaSCEle_[1]) < 1.4442, 
					       R9Ele_[1],etaSCEle_[1])
			   );

    invMass*=sqrt(corrEle[0]*corrEle[1]);

    invMassSigma = 0.5 * invMass *
      sqrt( 
	   sigmaEnergyEle[0]/(energyEle[0]*corrEle[0]) * sigmaEnergyEle[0]/(energyEle[0]*corrEle[0]) +
	   sigmaEnergyEle[1]/(energyEle[1]*corrEle[1]) * sigmaEnergyEle[1]/(energyEle[1]*corrEle[1]) +
	   smearEle_[0] * smearEle_[0] + smearEle_[1] * smearEle_[1]
	   );
    //    invMassSigmaRel = invMassSigma/invMass;
#ifdef DEBUG
  E_0: 203777     55.7019 0.35335 0.958164        0.39268 inf     86.3919 1       inf     0.00636875
  E_1: 203777     33.6127 -0.309422       0.831792        -0.270196       inf     0.00910235      1
    if(ientry<10){
      std::cout << "E_0: " << runNumber << "\t" << energyEle[0] << "\t" 
		<< etaSCEle_[0] << "\t" << R9Ele_[0] << "\t" << etaEle[0] << "\t" << smearEle_[0] << "\t" << invMass << "\t" << corrEle[0] << "\t" << invMassSigma << "\t" << sigmaEnergyEle[0] << std::endl;
      std::cout << "E_1: " << runNumber << "\t" << energyEle[1] << "\t" 
		<< etaSCEle_[1] << "\t" << R9Ele_[1] << "\t" << etaEle[1] << "\t" << smearEle_[1] << "\t" << sigmaEnergyEle[1] << "\t" << corrEle[1] << std::endl;
    }
#endif
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

