#include "../interface/SmearingImporter.hh"
#include "../interface/BW_CB_pdf_class.hh"
#include "../interface/ZFit_class.hh"
#include <TTreeFormula.h>
#include <TRandom3.h>
#include <TDirectory.h>
#include <TStopwatch.h>
#include <TList.h>
#include <TObject.h>
#include <TFriendElement.h>
#include <TEntryList.h>
#include <fstream>
#include <TTreeFormula.h>
#include <TObjArray.h>
#include <TChainElement.h>
//#define DEBUG


#define SELECTOR
#define FIXEDSMEARINGS
SmearingImporter::SmearingImporter(std::vector<TString> regionList, TString energyBranchName, TString commonCut):
	//  _chain(chain),
	_regionList(regionList),
	_scaleToy(1.01),
	_constTermToy(0.01),
	_energyBranchName(energyBranchName),
	_commonCut(commonCut),
	_eleID("loose"),
	_usePUweight(true),
	_useMCweight(true),
	_useR9weight(false),
	_usePtweight(false),
	_useZPtweight(false),
	_useFSRweight(false),
	_useWEAKweight(false),
	_excludeByWeight(true),
	_onlyDiagonal(false),
	_isSmearingEt(false),
	_pdfWeightIndex(0),
	cutter(false)
{
	cutter.energyBranchName = energyBranchName;
}


SmearingImporter::regions_cache_t SmearingImporter::GetCacheToy(Long64_t nEvents, bool isMC)
{
	regions_cache_t cache;
	TStopwatch myClock;
	myClock.Start();

	for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
	        region_ele1_itr != _regionList.end();
	        region_ele1_itr++) {
		for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
		        region_ele2_itr != _regionList.end();
		        region_ele2_itr++) {
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

void SmearingImporter::ImportToy(Long64_t nEvents, event_cache_t& eventCache, bool isMC)
{
	TRandom3 gen(12345);
	if(isMC) nEvents *= 2;
	//std::cout << "[DEBUG] constTermToy = " << _constTermToy << std::endl;
	for(Long64_t iEvent = 0; iEvent < nEvents; iEvent++) {
		ZeeEvent event;
		event.weight = 1;
		event.energy_ele1 = 45;
		event.energy_ele2 = 45;
		event.invMass = gen.BreitWigner(91.188, 2.48);
		if(isMC == false) event.invMass *= sqrt(gen.Gaus(_scaleToy, _constTermToy) * gen.Gaus(_scaleToy, _constTermToy)); //gen.Gaus(_scaleToy, _constTermToy); //
		eventCache.push_back(event);
	}
	return;
}



void SmearingImporter::Import(TTree *chain, regions_cache_t& cache, TString oddString, bool isMC, Long64_t nEvents, bool isToy, bool externToy)
{

	TRandom3 gen(0);
	if(!isMC) gen.SetSeed(12345);
	TRandom3 excludeGen(12345);
	Long64_t excludedByWeight = 0, includedEvents = 0;

	// for the energy calculation
	Float_t         energyEle[2];
	Float_t         corrEle_[2] = {1, 1};
	Float_t         smearEle_[2] = {1, 1};
	bool hasSmearEle = false;

	// for the angle calculation
	Float_t         etaEle[2];
	Float_t         phiEle[2];

	// for the weight calculation
	Float_t         weight = 1.;
	Float_t         r9weight[2] = {1, 1};
	Float_t         ptweight[2] = {1, 1};
	Float_t         FSRweight = 1.;
	Float_t         WEAKweight = 1.;
	Float_t         zptweight[45] = {1};
	Float_t         mcGenWeight = 1;
	Float_t         LTweight = -1;
	std::vector<double> *pdfWeights = NULL;

	Int_t           smearerCat[2];
	bool hasSmearerCat = false;

	// for toy repartition
	ULong64_t eventNumber;

	//------------------------------
	chain->SetBranchAddress("eventNumber", &eventNumber);
	chain->SetBranchAddress("etaEle", etaEle);
	chain->SetBranchAddress("phiEle", phiEle);

	chain->SetBranchAddress(_energyBranchName, energyEle);
	if(chain->GetBranch("scaleEle") != NULL) {
		if(isToy == false || (externToy == true && isToy == true && isMC == false)) {
			std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
			chain->SetBranchAddress("scaleEle", corrEle_);
			cutter._corrEle = true;
		}
	}

	if(chain->GetBranch("smearEle") != NULL) {
		if(isToy == false || (externToy == true && isToy == true && isMC == false)) {
			std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
			if(isMC) chain->SetBranchAddress("smearSigmaEle", smearEle_);
			else chain->SetBranchAddress("smearEle", smearEle_);
			hasSmearEle = true;
		}
	}

	if(!isMC && chain->GetBranch("pdfWeights_cteq66") != NULL && _pdfWeightIndex > 0) {
		std::cout << "[STATUS] Adding pdfWeight_ctec66 branch from friend" << std::endl;
		chain->SetBranchAddress("pdfWeights_cteq66", &pdfWeights);
	}

	// the second term is to ensure that in case of toy study it is applied only to pseudo-data otherwise to MC
	// the third is only temporary because old ntuples does not have the branch
	// probably it will be needed in the future if the pdfSystematic branches are put in a separate tree
	if(_useFSRweight &&  isMC == false && (isToy == false || (externToy == true && isToy == true && isMC == false)) && chain->GetBranch("fsrWeight") != NULL) {
		std::cout << "[STATUS] Getting fsrWeight branch for tree: " << chain->GetTitle() << std::endl;
		chain->SetBranchAddress("fsrWeight", &FSRweight);
	}
	if(_useWEAKweight  && isMC == false && (isToy == false || (externToy == true && isToy == true && isMC == false)) && chain->GetBranch("weakWeight") != NULL) {
		std::cout << "[STATUS] Getting weakWeight branch for tree: " << chain->GetTitle() << std::endl;
		chain->SetBranchAddress("weakWeight", &WEAKweight);
	}

	if(chain->GetBranch("puWeight") != NULL) {
		std::cout << "[STATUS] Getting puWeight branch for tree: " << chain->GetTitle() << std::endl;
		chain->SetBranchAddress("puWeight", &weight);
	}

	if(chain->GetBranch("r9Weight") != NULL) {
		std::cout << "[STATUS] Getting r9Weight branch for tree: " << chain->GetTitle() << std::endl;
		chain->SetBranchAddress("r9Weight", r9weight);
	}

	if(chain->GetBranch("ptWeight") != NULL) {
		std::cout << "[STATUS] Getting ptWeight branch for tree: " <<  chain->GetTitle() << std::endl;
		chain->SetBranchAddress("ptWeight", ptweight);
	}

	if(_useZPtweight && chain->GetBranch("ZPtWeight") != NULL) {
		std::cout << "[STATUS] Getting ZptWeight branch for tree: " <<  chain->GetTitle() << std::endl;
		chain->SetBranchAddress("ZPtWeight", zptweight);
	}

	if(chain->GetBranch("mcGenWeight") != NULL) {
		std::cout << "[STATUS] Getting mcGenWeight branch for tree: " <<  chain->GetTitle() << std::endl;
		chain->SetBranchAddress("mcGenWeight", &mcGenWeight);
	}

	if(chain->GetBranch("LTweight") != NULL) {
		std::cout << "[STATUS] Getting LTweight for tree: " <<  chain->GetTitle() << std::endl;
		chain->SetBranchAddress("LTweight", &LTweight);
	}

	if(chain->GetBranch("smearerCat") != NULL) {
		std::cout << "[STATUS] Getting smearerCat branch for tree: " <<  chain->GetTitle() << std::endl;
		chain->SetBranchAddress("smearerCat", smearerCat);
		hasSmearerCat = true;
	}

	if(hasSmearerCat == false) {
		std::cerr << "[ERROR] Must have smearerCat branch" << std::endl;
		exit(1);
	}
	Long64_t entries = chain->GetEntryList()->GetN();
	if(nEvents > 0 && nEvents < entries) {
		std::cout << "[INFO] Importing only " << nEvents << " events" << std::endl;
		entries = nEvents;
	}
	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;


	std::vector< std::pair<TTreeFormula*, TTreeFormula*> > catSelectors;
	if(hasSmearerCat == false) {
		for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
		        region_ele1_itr != _regionList.end();
		        region_ele1_itr++) {
			for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
			        region_ele2_itr != _regionList.end();
			        region_ele2_itr++) {

				if(region_ele2_itr == region_ele1_itr) {
					TString region = *region_ele1_itr;
					region.ReplaceAll(_commonCut, "");
					TTreeFormula *selector = new TTreeFormula("selector" + (region), cutter.GetCut(region + oddString, isMC), chain);
					catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(selector, NULL));
					//selector->Print();
				} else if(!_onlyDiagonal) {
					TString region1 = *region_ele1_itr;
					TString region2 = *region_ele2_itr;
					region1.ReplaceAll(_commonCut, "");
					region2.ReplaceAll(_commonCut, "");
					TTreeFormula *selector1 = new TTreeFormula("selector1" + region1 + region2,
					        cutter.GetCut(region1 + oddString, isMC, 1) &&
					        cutter.GetCut(region2 + oddString, isMC, 2),
					        chain);
					TTreeFormula *selector2 = new TTreeFormula("selector2" + region1 + region2,
					        cutter.GetCut(region1 + oddString, isMC, 2) &&
					        cutter.GetCut(region2 + oddString, isMC, 1),
					        chain);
					catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(selector1, selector2));
					//selector1->Print();
					//selector2->Print();

				} else catSelectors.push_back(std::pair<TTreeFormula*, TTreeFormula*>(NULL, NULL));

			}
		}
	}

	for(Long64_t jentry = 0; jentry < entries; jentry++) {
		Long64_t entryNumber = chain->GetEntryNumber(jentry);
		chain->GetEntry(entryNumber);
		if(isToy) {
			int modulo = eventNumber % 5;
			if(jentry < 10) {
				std::cout << "Dividing toyMC events: " << isMC << "\t" << eventNumber << "\t" << modulo
				          << "\t" << mcGenWeight
				          << std::endl;

			}

			if(isMC && modulo < 2) continue;
			if(!isMC && modulo >= 2) continue;
		}

		// reject events:
		if(weight > 3) continue;

		if (hasSmearerCat == false && chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			for(std::vector< std::pair<TTreeFormula*, TTreeFormula*> >::const_iterator catSelector_itr = catSelectors.begin();
			        catSelector_itr != catSelectors.end();
			        catSelector_itr++) {

				catSelector_itr->first->UpdateFormulaLeaves();
				if(catSelector_itr->second != NULL)       catSelector_itr->second->UpdateFormulaLeaves();
			}
		}

		int evIndex = -1;
		bool _swap = false;
		if(!hasSmearerCat) {
			for(std::vector< std::pair<TTreeFormula*, TTreeFormula*> >::const_iterator catSelector_itr = catSelectors.begin();
			        catSelector_itr != catSelectors.end();
			        catSelector_itr++) {
				_swap = false;
				TTreeFormula *sel1 = catSelector_itr->first;
				TTreeFormula *sel2 = catSelector_itr->second;
				if(sel1 == NULL) continue; // is it possible?
				if(sel1->EvalInstance() == false) {
					if(sel2 == NULL || sel2->EvalInstance() == false) continue;
					else _swap = true;
				}

				evIndex = catSelector_itr - catSelectors.begin();
			}
		} else {
			evIndex = smearerCat[0];
			_swap = smearerCat[1];
			if(jentry < 2) std::cout << evIndex << "\t" << _swap << std::endl;
		}
		if(evIndex < 0) continue; // event in no category

		ZeeEvent event;
		//       if(jentry<30){
		// 	//chain->Show(chain->GetEntryNumber(jentry));
		// 	std::cout << "[INFO] corrEle[0] = " << corrEle_[0] << std::endl;
		// 	std::cout << "[INFO] corrEle[1] = " << corrEle_[1] << std::endl;
		// 	std::cout << "[INFO] smearEle[0] = " << smearEle_[0] << std::endl;
		// 	std::cout << "[INFO] smearEle[1] = " << smearEle_[1] << std::endl;
		// 	std::cout << "[INFO] Category = " << evIndex << std::endl;
		//       }

		float t1 = TMath::Exp(-etaEle[0]);
		float t2 = TMath::Exp(-etaEle[1]);
		float t1q = t1 * t1;
		float t2q = t2 * t2;

		if(isMC && hasSmearEle) {
			smearEle_[0] = gen.Gaus(1, smearEle_[0]);
			smearEle_[1] = gen.Gaus(1, smearEle_[1]);
		}

		//------------------------------
		if(_swap) {
			event.energy_ele2 = energyEle[0] * corrEle_[0] * smearEle_[0];
			event.energy_ele1 = energyEle[1] * corrEle_[1] * smearEle_[1];
		} else {
			event.energy_ele1 = energyEle[0] * corrEle_[0] * smearEle_[0];
			event.energy_ele2 = energyEle[1] * corrEle_[1] * smearEle_[1];
		}
		//event.angle_eta_ele1_ele2=  (1-((1-t1q)*(1-t2q)+4*t1*t2*cos(phiEle[0]-phiEle[1]))/((1+t1q)*(1+t2q)));
		event.invMass = sqrt(2 * event.energy_ele1 * event.energy_ele2 *
		                     (1 - ((1 - t1q) * (1 - t2q) + 4 * t1 * t2 * cos(phiEle[0] - phiEle[1])) / ((1 + t1q) * (1 + t2q)))
		                    );
		if(_isSmearingEt) {
			if(_swap) {
				event.energy_ele2 /= cosh(etaEle[0]);
				event.energy_ele1 /= cosh(etaEle[1]);
			} else {
				event.energy_ele1 /= cosh(etaEle[0]);
				event.energy_ele2 /= cosh(etaEle[1]);
			}
		}
		// to calculate the invMass: invMass = sqrt(2 * energy_ele1 * energy_ele2 * angle_eta_ele1_ele2)
		//if(event.invMass < 70 || event.invMass > 110) continue;

		event.weight = 1.;
		if(_usePUweight) event.weight *= weight;
		if(LTweight >= 0) event.weight *= LTweight;
		if(_useR9weight) event.weight *= r9weight[0] * r9weight[1];
		if(_usePtweight) event.weight *= ptweight[0] * ptweight[1];
		if(_useFSRweight) event.weight *= FSRweight;
		if(_useWEAKweight) event.weight *= WEAKweight;
		if(_useZPtweight && isMC && _pdfWeightIndex > 0) event.weight *= zptweight[_pdfWeightIndex];
		if(!isMC && _pdfWeightIndex > 0 && pdfWeights != NULL) {
			if(((unsigned int)_pdfWeightIndex) > pdfWeights->size()) continue;
			event.weight *= ((*pdfWeights)[0] <= 0 || (*pdfWeights)[0] != (*pdfWeights)[0] || (*pdfWeights)[_pdfWeightIndex] != (*pdfWeights)[_pdfWeightIndex]) ? 0 : (*pdfWeights)[_pdfWeightIndex] / (*pdfWeights)[0];


#ifdef DEBUG
			if(jentry < 10 || event.weight != event.weight || event.weight > 1.3) {
				std::cout << "jentry = " << jentry
				          << "\tevent.weight = " << event.weight
				          //<< "\t" << (*pdfWeights)[_pdfWeightIndex]/(*pdfWeights)[0] << "\t" << (*pdfWeights)[_pdfWeightIndex] << "\t" << (*pdfWeights)[0]
				          << "\t" << r9weight[0] << " " << r9weight[1]
				          << "\t" << ptweight[0] << " " << ptweight[1]
				          << "\t" << WEAKweight << "\t" << FSRweight << "\t" << LTweight
				          << std::endl;
			}
#endif

		} else {
			if(!isMC && _pdfWeightIndex > 0) {
				std::cerr << "[ERROR] requested pdfWeights but not set by getentry" << std::endl;
				std::cerr << "[ERROR] jentry=" << jentry << "; chain: " << chain->GetName() << "\t" << chain->GetTitle()  << std::endl;
				if(jentry < 10) continue;
				else exit(1);
			}
		}
		//As simple as that
		if(isMC) {
			if(mcGenWeight > 0) {
				event.weight *= 1;
			} else {
				event.weight *= -1;
			}
		}
		//#ifdef DEBUG
		if(jentry < 10 || event.weight != event.weight || event.weight > 2) {
			std::cout << "jentry = " << jentry
			          << "\tevent.weight = " << event.weight
			          << "\t" << weight << "\t" << mcGenWeight
			          << "\t" << r9weight[0] << " " << r9weight[1]
			          << "\t" << ptweight[0] << " " << ptweight[1]
			          << "\t" << zptweight[0]
			          << "\t" << WEAKweight << "\t" << FSRweight
			          << std::endl;
		}
		//#endif

		//if(event.weight==0){//ok, fixed
		//event.weight=1;
		//}
		//if(event.weight<=0 || event.weight!=event.weight || event.weight>10) {continue;}
		if(event.weight > 10) {
			continue;   //also negative weights are possible
		}

#ifdef FIXEDSMEARINGS
		if(isMC) {
			event.smearings_ele1 = new float[NSMEARTOYLIM];
			event.smearings_ele2 = new float[NSMEARTOYLIM];
			for(int i = 0; i < NSMEARTOYLIM; i++) {
				event.smearings_ele1[i] = (float) gen.Gaus(0, 1);
				event.smearings_ele2[i] = (float) gen.Gaus(0, 1);
			}
		} else {
			event.smearings_ele1 = new float[1];
			event.smearings_ele2 = new float[1];
			event.smearings_ele1[0] = (float) gen.Gaus(0, 1);
			event.smearings_ele2[0] = (float) gen.Gaus(0, 1);
		}
#endif
		includedEvents++;
		cache.at(evIndex).push_back(event);
		//(cache[evIndex]).push_back(event);
	}

	std::cout << "[INFO] Importing events: " << includedEvents << "; events excluded by weight: " << excludedByWeight << std::endl;
	chain->ResetBranchAddresses();
	chain->GetEntry(0);
	return;

}

SmearingImporter::regions_cache_t SmearingImporter::GetCache(TChain *_chain, bool isMC, bool odd, Long64_t nEvents, bool isToy, bool externToy)
{

	TString eleID_ = "eleID_" + _eleID;

	TString oddString;
	if(odd) oddString += "-odd";
	regions_cache_t cache;
	TStopwatch myClock;
	myClock.Start();
	_chain->GetEntry(0);

	_chain->SetBranchStatus("*", 0);

	_chain->SetBranchStatus("etaEle", 1);
	_chain->SetBranchStatus("phiEle", 1);
	_chain->SetBranchStatus(_energyBranchName, 1);
	if(isToy) _chain->SetBranchStatus("eventNumber", 1);
	//  std::cout << _chain->GetBranchStatus("seedXSCEle") <<  std::endl;
	//  std::cout << _chain->GetBranchStatus("etaEle") <<  std::endl;

	if(_chain->GetBranch("scaleEle") != NULL) {
		std::cout << "[STATUS] Activating branch scaleEle" << std::endl;
		_chain->SetBranchStatus("scaleEle", 1);
		cutter._corrEle = true;
	}

	if(_chain->GetBranch("smearEle") != NULL) {
		std::cout << "[STATUS] Activating branch smearEle" << std::endl;
		_chain->SetBranchStatus("smearEle", 1);
		_chain->SetBranchStatus("smearSigmaEle", 1);//This is the sigma used to smear the MC
	}


	if(_chain->GetBranch("r9Weight") != NULL)  _chain->SetBranchStatus("r9Weight", 1);
	if(_chain->GetBranch("puWeight") != NULL)  _chain->SetBranchStatus("puWeight", 1);
	if(_chain->GetBranch("ptWeight") != NULL)  _chain->SetBranchStatus("ptWeight", 1);
	if(_chain->GetBranch("mcGenWeight") != NULL)  _chain->SetBranchStatus("mcGenWeight", 1);
	if(_chain->GetBranch("smearerCat") != NULL) {
		//std::cout << "[STATUS] Activating branch smearerCat" << std::endl;
		_chain->SetBranchStatus("smearerCat", 1);
	}



	for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
	        region_ele1_itr != _regionList.end();
	        region_ele1_itr++) {
		std::set<TString> branchNames = cutter.GetBranchNameNtuple(_commonCut + "-" + eleID_ + "-" + *region_ele1_itr);
		for(std::set<TString>::const_iterator itr = branchNames.begin();
		        itr != branchNames.end(); itr++) {
			_chain->SetBranchStatus(*itr, "1");
		}
	}

	TString evListName = "evList_";
	evListName += _chain->GetTitle();
	evListName += "_all";
	TEntryList *oldList = _chain->GetEntryList();
	if(oldList == NULL) {
		std::cout << "[STATUS] In SmearingImporter.cc, Setting entry list: " << evListName << std::endl;
		_chain->Draw(">>" + evListName, cutter.GetCut(_commonCut + "-" + eleID_, isMC), "entrylist");
		TEntryList *elist_all = (TEntryList*)gDirectory->Get(evListName);
		TECALChain *chain_ecal = (TECALChain*)_chain;
		chain_ecal->TECALChain::SetEntryList(elist_all);
		assert(elist_all != NULL);
		std::cout << "[INFO] Selected events: " <<  chain_ecal->GetEntryList()->GetN() << std::endl;
		_chain = dynamic_cast<TChain*>(chain_ecal);
	}
	for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
	        region_ele1_itr != _regionList.end();
	        region_ele1_itr++) {
		for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
		        region_ele2_itr != _regionList.end();
		        region_ele2_itr++) {

			event_cache_t eventCache;
			cache.push_back(eventCache);
		}
	}

	Import(_chain, cache, oddString, isMC, nEvents, isToy, externToy);
#ifdef DEBUG
	int index = 0;
	for(std::vector<TString>::const_iterator region_ele1_itr = _regionList.begin();
	        region_ele1_itr != _regionList.end();
	        region_ele1_itr++) {
		for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr;
		        region_ele2_itr != _regionList.end();
		        region_ele2_itr++) {
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


