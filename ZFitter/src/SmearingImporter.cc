#include "../interface/SmearingImporter.hh"
#include "../interface/BW_CB_pdf_class.hh"

#include <TTreeFormula.h>
#include <TRandom3.h>
#include <TDirectory.h>
//#define DEBUG
#include <TStopwatch.h>
#define SELECTOR
SmearingImporter::SmearingImporter(std::vector<TString> regionList, TString energyBranchName, TString commonCut):
  //  _chain(chain),
  _regionList(regionList),
  _scaleToy(1.01),
  _constTermToy(0.01),
  _energyBranchName(energyBranchName),
  _commonCut(commonCut),
  _eleID("loose"),
  _usePUweight(true),
  _useR9weight(false),
  _usePtweight(false),
  _onlyDiagonal(false),
  _isSmearingEt(false),
  cutter(false)
{

}


SmearingImporter::regions_cache_t SmearingImporter::GetCacheToy(Long64_t nEvents, bool isMC){
  regions_cache_t cache;
  TStopwatch myClock;
  myClock.Start();

  for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
      region_ele1_itr != _regionList.end();
      region_ele1_itr++){
    for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
	region_ele2_itr != _regionList.end();
	region_ele2_itr++){
      event_cache_t eventCache;
      if(!(_onlyDiagonal && region_ele2_itr != region_ele1_itr))
	ImportToy(nEvents, eventCache, isMC);
      cache.push_back(eventCache);
    }
  }
  myClock.Stop();
  myClock.Print();
  return cache;
}

void SmearingImporter::ImportToy(Long64_t nEvents, event_cache_t& eventCache, bool isMC){
  TRandom3 gen(12345);
  if(isMC) nEvents*=2;
  //std::cout << "[DEBUG] constTermToy = " << _constTermToy << std::endl;
  for(Long64_t iEvent=0; iEvent < nEvents; iEvent++){
    ZeeEvent event;
    event.weight=1;
    event.energy_ele1=45;
    event.energy_ele2=45;
    event.invMass = gen.BreitWigner(91.188,2.48);
    if(isMC==false) event.invMass*= sqrt(gen.Gaus(_scaleToy, _constTermToy) * gen.Gaus(_scaleToy, _constTermToy));//gen.Gaus(_scaleToy, _constTermToy); //
    eventCache.push_back(event);
  }
  return;
}


void SmearingImporter::Import(TTree *chain, event_cache_t& eventCache, TEntryList *entryList, bool swap){
    
  // for the energy calculation
  Float_t         energyEle[2];
  Float_t         corrEle_[2]={1,1};
  Float_t         smearEle_[2]={1,1};
  // for the angle calculation
  Float_t         etaEle[2];
  Float_t         phiEle[2];

  // for the weight calculation
  Float_t         weight=1;
  Float_t         r9weight[2]={1,1};
  Float_t         ptweight[2]={1,1};

  //------------------------------
  chain->SetBranchAddress(_energyBranchName, energyEle);
  if(chain->GetBranch("scaleEle")!=NULL){
    std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
    chain->SetBranchAddress("scaleEle", corrEle_);
  } 

  if(chain->GetBranch("smearEle")!=NULL){
    std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
    chain->SetBranchAddress("smearEle", smearEle_);
  } 

  chain->SetBranchAddress("etaEle", etaEle);
  chain->SetBranchAddress("phiEle", phiEle);


  if(chain->GetBranch("puWeight")!=NULL){
    std::cout << "[STATUS] Getting puWeight branch for tree: " << chain->GetTitle() << std::endl;
    chain->SetBranchAddress("puWeight", &weight);
  }

  if(chain->GetBranch("r9Weight")!=NULL){
    std::cout << "[STATUS] Getting r9Weight branch for tree: " << chain->GetTitle() << std::endl;
    chain->SetBranchAddress("r9Weight", r9weight);
  }  

  if(chain->GetBranch("ptWeight")!=NULL){
    std::cout << "[STATUS] Getting ptWeight branch for tree: " <<  chain->GetTitle() << std::endl;
    chain->SetBranchAddress("ptWeight", ptweight);
  }

  TEntryList *elist_all = (TEntryList*)chain->GetEntryList()->Clone();
  chain->SetEntryList(entryList);

  Long64_t entries = entryList->GetN();
  std::cout << "___ ENTRIES: " << entries << std::endl;
  for(Long64_t jentry=0; jentry < entries; jentry++){
    //  for(Long64_t jentry=0; jentry<chain->GetEntries(); jentry++){
    //chain->LoadTree(jentry);
    //chain->GetEntry(jentry);
    chain->GetEntry(chain->GetEntryNumber(jentry));

    ZeeEvent event;
    //if(jentry<1){
      //chain->Show(chain->GetEntryNumber(jentry));
//       std::cout << "[INFO] corrEle[0] = " << corrEle_[0] << std::endl;
//       std::cout << "[INFO] corrEle[1] = " << corrEle_[1] << std::endl;
//       std::cout << "[INFO] smearEle[0] = " << smearEle_[0] << std::endl;
//       std::cout << "[INFO] smearEle[1] = " << smearEle_[1] << std::endl;
//     }

    float t1=TMath::Exp(-etaEle[0]);
    float t2=TMath::Exp(-etaEle[1]);
    float t1q = t1*t1;
    float t2q = t2*t2;
  
    //------------------------------
    if(swap){
      event.energy_ele2 = energyEle[0] * corrEle_[0] * smearEle_[0];
      event.energy_ele1 = energyEle[1] * corrEle_[1] * smearEle_[1];
    } else {
      event.energy_ele1 = energyEle[0] * corrEle_[0] * smearEle_[0];
      event.energy_ele2 = energyEle[1] * corrEle_[1] * smearEle_[1];
    }
    //event.angle_eta_ele1_ele2=  (1-((1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/((1+t1q)*(1+t2q)));
    event.invMass= sqrt(2 * event.energy_ele1 * event.energy_ele2 *
			(1-((1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/((1+t1q)*(1+t2q)))
			);
    // to calculate the invMass: invMass = sqrt(2 * energy_ele1 * energy_ele2 * angle_eta_ele1_ele2)

    if(_isSmearingEt){
      if(swap){
	event.energy_ele2 /= TMath::CosH(etaEle[0]);
	event.energy_ele1 /= TMath::CosH(etaEle[1]);
      } else {
	event.energy_ele1 /= TMath::CosH(etaEle[0]);
	event.energy_ele2 /= TMath::CosH(etaEle[1]);
      }
    }	

    event.weight = 1.;
    if(_usePUweight) event.weight *= weight;
    if(_useR9weight) event.weight *= r9weight[0]*r9weight[1];
    if(_usePtweight) event.weight *= ptweight[0]*ptweight[1];
  
   
    eventCache.push_back(event);
  }

  chain->SetEntryList(elist_all);
  chain->ResetBranchAddresses();
  chain->GetEntry(0);
  return;


}

void SmearingImporter::Import(TTree *chain, regions_cache_t& cache, TString oddString, bool isMC, Long64_t nEvents, bool isToy, bool externToy){
    
  // for the energy calculation
  Float_t         energyEle[2];
  Float_t         corrEle_[2]={1,1};
  Float_t         smearEle_[2]={1,1};
  // for the angle calculation
  Float_t         etaEle[2];
  Float_t         phiEle[2];

  // for the weight calculation
  Float_t         weight=1;
  Float_t         r9weight[2]={1,1};
  Float_t         ptweight[2]={1,1};

  Int_t           smearerCat[2];
  bool hasSmearerCat=false;

  // for toy repartition
  ULong64_t eventNumber;

  //------------------------------
  chain->SetBranchAddress("eventNumber", &eventNumber);

  chain->SetBranchAddress(_energyBranchName, energyEle);
  if(chain->GetBranch("scaleEle")!=NULL){
    std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
    chain->SetBranchAddress("scaleEle", corrEle_);
  } 

  if(chain->GetBranch("smearEle")!=NULL){
    if(isToy==false || (externToy==true && isToy==true && isMC==false)){
      std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
      chain->SetBranchAddress("smearEle", smearEle_);
    } 
  }

  chain->SetBranchAddress("etaEle", etaEle);
  chain->SetBranchAddress("phiEle", phiEle);


  if(chain->GetBranch("puWeight")!=NULL){
    std::cout << "[STATUS] Getting puWeight branch for tree: " << chain->GetTitle() << std::endl;
    chain->SetBranchAddress("puWeight", &weight);
  }

  if(chain->GetBranch("r9Weight")!=NULL){
    std::cout << "[STATUS] Getting r9Weight branch for tree: " << chain->GetTitle() << std::endl;
    chain->SetBranchAddress("r9Weight", r9weight);
  }  

  if(chain->GetBranch("ptWeight")!=NULL){
    std::cout << "[STATUS] Getting ptWeight branch for tree: " <<  chain->GetTitle() << std::endl;
    chain->SetBranchAddress("ptWeight", ptweight);
  }

  if(chain->GetBranch("smearerCat")!=NULL){
    std::cout << "[STATUS] Getting smearerCat branch for tree: " <<  chain->GetTitle() << std::endl;
    chain->SetBranchAddress("smearerCat", smearerCat);
    hasSmearerCat=true;
  }

  Long64_t entries = chain->GetEntryList()->GetN();
  if(nEvents>0 && nEvents<entries){
    std::cout << "[INFO] Importing only " << nEvents << " events" << std::endl;
    entries=nEvents;
  }
  chain->LoadTree(chain->GetEntryNumber(0));
  Long64_t treenumber=-1;

  std::vector< std::pair<TTreeFormula*, TTreeFormula*> > catSelectors;
  for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
      region_ele1_itr != _regionList.end();
      region_ele1_itr++){
    for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
	region_ele2_itr != _regionList.end();
	region_ele2_itr++){

      if(region_ele2_itr==region_ele1_itr){
	TString region=*region_ele1_itr;
	region.ReplaceAll(_commonCut,"");
	TTreeFormula *selector = new TTreeFormula("selector"+(region), cutter.GetCut(region+oddString, isMC), chain);
	catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(selector,NULL));
	//selector->Print();
      } else if(!_onlyDiagonal){
	TString region1=*region_ele1_itr;
	TString region2=*region_ele2_itr;
	region1.ReplaceAll(_commonCut,"");
	region2.ReplaceAll(_commonCut,"");
	TTreeFormula *selector1 = new TTreeFormula("selector1"+region1+region2, 
						   cutter.GetCut(region1+oddString, isMC, 1) && 
						   cutter.GetCut(region2+oddString, isMC, 2),
						   chain);
	TTreeFormula *selector2 = new TTreeFormula("selector2"+region1+region2,
						   cutter.GetCut(region1+oddString, isMC, 2) && 
						   cutter.GetCut(region2+oddString, isMC, 1),
						   chain);
	catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(selector1,selector2));
	//selector1->Print();
	//selector2->Print();
	
      } else catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(NULL, NULL));
	
    }
  }

  for(Long64_t jentry=0; jentry < entries; jentry++){
    Long64_t entryNumber= chain->GetEntryNumber(jentry);
    chain->GetEntry(entryNumber);
    if(isToy){
      if(jentry<10){
	std::cout << isMC << "\t" << eventNumber << "\t" << eventNumber%3 << std::endl;
      }
      if(isMC && eventNumber%3==0) continue;
      if(!isMC && eventNumber%3!=0) continue;
    }

    if (hasSmearerCat==false && chain->GetTreeNumber() != treenumber) {
      treenumber = chain->GetTreeNumber();
      for(std::vector< std::pair<TTreeFormula*, TTreeFormula*> >::const_iterator catSelector_itr = catSelectors.begin();
	  catSelector_itr != catSelectors.end();
	  catSelector_itr++){
	
	catSelector_itr->first->UpdateFormulaLeaves();
	if(catSelector_itr->second!=NULL)       catSelector_itr->second->UpdateFormulaLeaves();
      }
    }

    int evIndex=-1;
    bool _swap=false;
    if(!hasSmearerCat){
      for(std::vector< std::pair<TTreeFormula*, TTreeFormula*> >::const_iterator catSelector_itr = catSelectors.begin();
	  catSelector_itr != catSelectors.end();
	  catSelector_itr++){
	_swap=false;
	TTreeFormula *sel1 = catSelector_itr->first;
	TTreeFormula *sel2 = catSelector_itr->second;
	if(sel1==NULL) continue; // is it possible?
	if(sel1->EvalInstance()==false){
	  if(sel2==NULL || sel2->EvalInstance()==false) continue;
	  else _swap=true;
	}

	evIndex=catSelector_itr-catSelectors.begin();
      }
    }else{
      evIndex=smearerCat[0];
      _swap=smearerCat[1];
      if(jentry<2) std::cout << evIndex << "\t" << _swap << std::endl;
    }
    if(evIndex<0) continue; // event in no category

    ZeeEvent event;
    //       if(jentry<30){
    // 	//chain->Show(chain->GetEntryNumber(jentry));
    // 	std::cout << "[INFO] corrEle[0] = " << corrEle_[0] << std::endl;
    // 	std::cout << "[INFO] corrEle[1] = " << corrEle_[1] << std::endl;
    // 	std::cout << "[INFO] smearEle[0] = " << smearEle_[0] << std::endl;
    // 	std::cout << "[INFO] smearEle[1] = " << smearEle_[1] << std::endl;
    // 	std::cout << "[INFO] Category = " << evIndex << std::endl;
    //       }
    
    float t1=TMath::Exp(-etaEle[0]);
    float t2=TMath::Exp(-etaEle[1]);
    float t1q = t1*t1;
    float t2q = t2*t2;
    
    //------------------------------
    if(_swap){
      event.energy_ele2 = energyEle[0] * corrEle_[0] * smearEle_[0];
      event.energy_ele1 = energyEle[1] * corrEle_[1] * smearEle_[1];
    } else {
      event.energy_ele1 = energyEle[0] * corrEle_[0] * smearEle_[0];
      event.energy_ele2 = energyEle[1] * corrEle_[1] * smearEle_[1];
    }
    //event.angle_eta_ele1_ele2=  (1-((1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/((1+t1q)*(1+t2q)));
    event.invMass= sqrt(2 * event.energy_ele1 * event.energy_ele2 *
			(1-((1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/((1+t1q)*(1+t2q)))
			);
    // to calculate the invMass: invMass = sqrt(2 * energy_ele1 * energy_ele2 * angle_eta_ele1_ele2)

    event.weight = 1.;
    if(_usePUweight) event.weight *= weight;
    if(_useR9weight) event.weight *= r9weight[0]*r9weight[1];
    if(_usePtweight) event.weight *= ptweight[0]*ptweight[1];
  
    cache.at(evIndex).push_back(event);
    //(cache[evIndex]).push_back(event);
  }
  
  chain->ResetBranchAddresses();
  chain->GetEntry(0);
  return;


}

SmearingImporter::regions_cache_t SmearingImporter::GetCache(TChain *_chain, bool isMC, bool odd, Long64_t nEvents, bool isToy, bool externToy){

  TString eleID_="eleID_"+_eleID;

  TString oddString;
  if(odd) oddString+="-odd";
  regions_cache_t cache;
  TStopwatch myClock;
  myClock.Start();
  _chain->GetEntry(0);

  _chain->SetBranchStatus("*", 0);

  _chain->SetBranchStatus("etaEle", 1);
  _chain->SetBranchStatus("phiEle", 1);
  _chain->SetBranchStatus(_energyBranchName, 1);
  if(isToy) _chain->SetBranchStatus("eventNumber",1);
  //  std::cout << _chain->GetBranchStatus("seedXSCEle") <<  std::endl;
  //  std::cout << _chain->GetBranchStatus("etaEle") <<  std::endl;

  if(_chain->GetBranch("scaleEle")!=NULL){
    std::cout << "[STATUS] Activating branch scaleEle" << std::endl;
    _chain->SetBranchStatus("scaleEle", 1);
  }

  if(_chain->GetBranch("smearEle")!=NULL){
    std::cout << "[STATUS] Activating branch smearEle" << std::endl;
    _chain->SetBranchStatus("smearEle", 1);
  } 

 
  if(_chain->GetBranch("r9Weight")!=NULL)  _chain->SetBranchStatus("r9Weight", 1);
  if(_chain->GetBranch("puWeight")!=NULL)  _chain->SetBranchStatus("puWeight", 1);
  if(_chain->GetBranch("ptWeight")!=NULL)  _chain->SetBranchStatus("ptWeight", 1);
  if(_chain->GetBranch("smearerCat")!=NULL)_chain->SetBranchStatus("smearerCat", 1);



  for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
      region_ele1_itr != _regionList.end();
      region_ele1_itr++){
    std::set<TString> branchNames = cutter.GetBranchNameNtuple(_commonCut+"-"+eleID_+"-"+*region_ele1_itr);
    for(std::set<TString>::const_iterator itr = branchNames.begin();
	itr != branchNames.end(); itr++){
      _chain->SetBranchStatus(*itr, "1");
    }
  }
 
  std::cout << "Eventlist" << std::endl;
  TString evListName="evList_";
  evListName+=_chain->GetTitle();
  evListName+="_all";
  TEntryList *oldList = _chain->GetEntryList();
  if(oldList==NULL){
  _chain->Draw(">>"+evListName, cutter.GetCut(_commonCut+"-"+eleID_,true), "entrylist");
  //_chain->Draw(">>"+evListName, "", "entrylist");
  TEntryList *elist_all = (TEntryList*)gDirectory->Get(evListName);
  //  elist_all->SetBit(!kCanDelete);
  _chain->SetEntryList(elist_all);
  }
  std::cout << "Eventlist" << std::endl;
  for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
      region_ele1_itr != _regionList.end();
      region_ele1_itr++){
    for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
	region_ele2_itr != _regionList.end();
	region_ele2_itr++){
      
      event_cache_t eventCache;
      cache.push_back(eventCache);
    }
  }

  Import(_chain, cache, oddString, isMC, nEvents, isToy, externToy);
#ifdef DEBUG
  int index=0;
  for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
      region_ele1_itr != _regionList.end();
      region_ele1_itr++){
    for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
	region_ele2_itr != _regionList.end();
	region_ele2_itr++){
      std::cout << "[INFO] Category " << index << " " <<  *region_ele1_itr << *region_ele2_itr
		<< " filled with " << cache[index].size() << " entries" 
		<< std::endl;
      index++;
    }
  }
#endif
  myClock.Stop();
  myClock.Print();
  return cache;
}


