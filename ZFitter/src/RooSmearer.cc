#include "../interface/RooSmearer.hh"
//#include <RooPullVar.h>
#define FIXEDSMEARINGS
//#define DEBUG
//#define CPU_DEBUG
//#define FUNC_DEBUG
//#define MEM_DEBUG
#include <TSystem.h>
#include <TIterator.h>

RooSmearer::~RooSmearer(void)
{

}

RooSmearer::RooSmearer(const RooSmearer& old, const char* newname)
{
	//  RooSmearer(newname, old._data_chain, old._signal_chain, NULL,
	//	     old.importer._regionList, old._paramSet, old.importer._energyBranchName);
	RooSmearer();
}

RooSmearer::RooSmearer(const char *name,  ///< name of the variable
                       TChain *data_chain_,
                       TChain *signal_chain_,
                       TChain *bkg_chain_,
                       std::vector<TString> regionList, ///<list of the region names (used for the categorization)
                       std::vector<RooArgSet> params, ///< parameters divided by category (also rooformulavar)
                       RooArgSet parset, ///< list of real variables
                       TString energyBranchName
                      ):
	RooAbsReal(name, energyBranchName),
	importer(regionList, energyBranchName),
	_params_vec(params),
	_paramSet("paramSet", "Set of parameters", this),
	invMass_min_(80), invMass_max_(100), invMass_bin_(0.25),
	deltaNLLMaxSmearToy(330),
	_deactive_minEventsDiag(1000), _deactive_minEventsOffDiag(1500), _nSmearToy(20),
	nllBase(0),
	nllVar("nll", "", 0, 1e20),
	_isDataSmeared(false),
	_autoBin(false),
	_autoNsmear(false),
	smearscan(false),
	dataset(NULL)
{
	_data_chain = data_chain_;
	_signal_chain = signal_chain_;

	TIterator *it = parset.createIterator();
	for(RooRealVar *v = (RooRealVar*)it->Next(); v != NULL; v = (RooRealVar*)it->Next()) {
		if( v->isLValue()) { //! v->isConstant() &&)
			_paramSet.add((*v));
		}
	}

//   //std::cout << "[INFO] Creating truthArgs" << std::endl;
//     truthSet=(RooArgSet *)parset.snapshot();
//     RooArgList 	 argList_(parset), truthArgList_(*truthSet);
//     TIterator 	*it_ = argList_.createIterator();
//     TIterator 	*itTruth = truthArgList_.createIterator();
//     RooRealVar *truthvar=(RooRealVar*)itTruth->Next();
//     for(RooRealVar *var = (RooRealVar*)it_->Next();
// 	var != NULL;
// 	var = (RooRealVar*)it_->Next(), truthvar = (RooRealVar*)itTruth->Next()){
//       RooPullVar *pullVar = new RooPullVar(TString(var->GetName())+"_pull","", *var, *truthvar);
//       pullArgs.add(*pullVar);
//     }


//   for(std::vector<RooArgSet>::const_iterator par_itr = params.begin();
//       par_itr != params.end();
//       par_itr++){
//     TIterator *it = par_itr->createIterator();
//     for(RooRealVar *v = (RooRealVar*)it->Next(); v!=NULL; v = (RooRealVar*)it->Next()){
//       if(! v->isConstant() && v->isLValue()) _paramSet.add((*v));
//     }
//     //_paramSet.add((*par_itr));
//     _params.add((*par_itr));
//   }
	//    std::cout << "###############################" << std::endl;
	//    _params.Print();

	nBins_ = (int) ((invMass_max_ - invMass_min_) / invMass_bin_);

	rgen_ = new TRandom3(0); // inizializzo il generatore con seed impostato sull'ora
	myClock = new TStopwatch();

	_markov.SetParameters(_paramSet);
	nllMin = 1e20;
}


void RooSmearer::SetCache(Long64_t nEvents, bool cacheToy, bool externToy)
{
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "[STATUS] Importing cache events" << std::endl;

	if(data_events_cache.empty()) {
		if(cacheToy) {
			std::cout << "[STATUS] --- Setting toy cache for data" << std::endl;
			data_events_cache = importer.GetCache(_signal_chain, false, false, nEvents, true, externToy); //importer.GetCacheToy(nEvents, false);
		} else {
			std::cout << "[STATUS] --- Setting cache for data" << std::endl;
			data_events_cache = importer.GetCache(_data_chain, false, false, nEvents);
		}
	}
	if(!mc_events_cache.empty()) {
		std::cerr << "[ERROR] mc_events_cache not empty: " << mc_events_cache.size() << std::endl;
		exit(1);
	}
	if(mc_events_cache.empty()) {
		if(cacheToy) {
			std::cout << "[STATUS] --- Setting toy cache for mc" << std::endl;
			mc_events_cache = importer.GetCache(_signal_chain, true, false, nEvents, true, externToy); //importer.GetCacheToy(nEvents, true);
		} else {
			std::cout << "[STATUS] --- Setting cache for mc" << std::endl;
			mc_events_cache = importer.GetCache(_signal_chain, true, false, nEvents);
		}
	}


#ifdef DEBUG
	std::cout << "[DEBUG] Data events size:" << data_events_cache.size() << std::endl;
	std::cout << "[DEBUG] MC events size:" << data_events_cache.size() << std::endl;
#endif

	return;
}

void RooSmearer::InitCategories(bool mcToy)
{

	int index = 0;
	ZeeCategories.reserve((int)(importer._regionList.size() * (importer._regionList.size() + 1) / 2 + 1));
	for(std::vector<TString>::const_iterator region_ele1_itr = importer._regionList.begin();
	        region_ele1_itr != importer._regionList.end();
	        region_ele1_itr++) {
		for(std::vector<TString>::const_iterator region_ele2_itr = region_ele1_itr; //importer._regionList.begin();
		        region_ele2_itr != importer._regionList.end();
		        region_ele2_itr++) {
#ifdef DEBUG
			std::cout << "==============================" << std::endl;
#endif
			ZeeCategory cat;
			cat.categoryIndex1 = region_ele1_itr - importer._regionList.begin();
			cat.categoryIndex2 = region_ele2_itr - importer._regionList.begin();

//       if(!mcToy) cat.data_events = &(data_events_cache[index]);
//       else {
// 	std::cout << "[INFO] Initializing data_events with mc" << std::endl;
// 	cat.data_events = &(mc_events_cache[index]);
//       }
			cat.data_events = &(data_events_cache[index]);
			cat.mc_events = &(mc_events_cache[index]);

			cat.categoryName1 += *region_ele1_itr;
			cat.categoryName2 += *region_ele2_itr;

			cat.pars1.add(_params_vec[cat.categoryIndex1]);
			cat.pars2.add(_params_vec[cat.categoryIndex2]);

			cat.scale1 = 1;
			cat.alpha1 = 0;
			cat.constant1 = 0;
			cat.scale2 = 1;
			cat.alpha2 = 0;
			cat.constant2 = 0;

//       RooArgList argList1(cat.pars1);
//       TIterator *it = argList1.createIterator();
//       for(RooRealVar *v = (RooRealVar *) it->Next(); v!=NULL; v = (RooRealVar*) it->Next())
// 	cat.pars1_old.add( *((RooRealVar*) v->Clone(TString(v->GetName())+"_old")));
//       RooArgList argList2(cat.pars2);
//       it = argList2.createIterator();
//       for(RooRealVar *v = (RooRealVar *) it->Next(); v!=NULL; v = (RooRealVar*) it->Next())
// 	cat.pars2_old.add( *((RooRealVar*) v->Clone(TString(v->GetName())+"_old")));


			cat.nBins = nBins_;
			cat.invMass_min = invMass_min_;
			cat.invMass_max = invMass_max_;

#ifdef DEBUG
			std::cout << "[DEBUG] Cat ele1: " << cat.categoryName1 << "\t" << cat.categoryIndex1 << std::endl;
			cat.pars1.Print();
			std::cout << "[DEBUG] Cat ele2: " << cat.categoryName2 << "\t" << cat.categoryIndex2 << std::endl;
			cat.pars2.Print();

			//cat.data_events->size();
#endif

			// allocating the histograms
			TString histoName;
			histoName = cat.categoryName1 + cat.categoryName2 + "_";

			cat.hist_mc = new TH1F(histoName + "MC", histoName + "MC", cat.nBins, cat.invMass_min, cat.invMass_max);
			cat.hist_mc->Sumw2();
			cat.smearHist_mc = new TH1F(histoName + "smearMC", histoName + "smearMC", cat.nBins, cat.invMass_min, cat.invMass_max);
			cat.smearHist_mc->Sumw2();
			cat.hist_data = new TH1F(histoName + "data", histoName + "data", cat.nBins, cat.invMass_min, cat.invMass_max);
			cat.hist_data->Sumw2();
			cat.smearHist_data = new TH1F(histoName + "smeardata", histoName + "smeardata", cat.nBins, cat.invMass_min, cat.invMass_max);
			cat.smearHist_data->Sumw2();
			SetHisto(*(cat.mc_events), cat.hist_mc);
			SetHisto(*(cat.data_events), cat.hist_data);
			if(_isDataSmeared) {
				if(cat.data_events->size() != 0) {
					std::cout << "[INFO toyMC]: In the runToy scenario you want to apply scale/sigma to your data" << std::endl;
					SetSmearedHisto(*(cat.data_events),
					                cat.pars1, cat.pars2,
					                cat.categoryName1, cat.categoryName2, 1,
					                cat.smearHist_data);
				}
			}

			//------------------------------ deactivating category
			cat.active = true;

			//Just print out the category index (useful if you have to make a quick plot)
			if(cat.categoryIndex1 == cat.categoryIndex2) { // diagonal category
				std::cout << "[INFO on index] Diagonal Category: " << ZeeCategories.size()
				          << ": " << cat.categoryName1 << "\t" << cat.categoryName2
				          << "; Integral is " << cat.hist_data->Integral() << std::endl;
			} else { //not diagonal category
				std::cout << "[INFO on index] Off-Diag Category: " << ZeeCategories.size()
				          << ": " << cat.categoryName1 << "\t" << cat.categoryName2
				          << "; Integral is " << cat.hist_data->Integral() << std::endl;
			}

			//Cat deactivation strategy: if events are too few it's just noise in the likelihood!
			unsigned int deactiveMinEvents = _deactive_minEventsDiag;//default is 1000
			unsigned int deactiveMinEvents_off = _deactive_minEventsOffDiag;//default is 2000
			if(cat.categoryIndex1 == cat.categoryIndex2) { // diagonal category
				if(cat.hist_data->Integral() < deactiveMinEvents) {
					std::cout << "[INFO] Diagonal Category: " << ZeeCategories.size()
					          << ": " << cat.categoryName1 << "\t" << cat.categoryName2
					          << " has been deactivated: events in data less " << deactiveMinEvents
					          << "; Integral is " << cat.hist_data->Integral() << std::endl;
					cat.active = false;
				}
			} else { //not diagonal category
				if(cat.hist_data->Integral() < deactiveMinEvents_off) {
					std::cout << "[INFO] Off-Diag Category: " << ZeeCategories.size()
					          << ": " << cat.categoryName1 << "\t" << cat.categoryName2
					          << " has been deactivated: events in data less " << deactiveMinEvents_off
					          << "; Integral is " << cat.hist_data->Integral() << std::endl;
					cat.active = false;
				}
			}

			//Shoulder in the invariant mass spectra (if the mass spectrum is too much asymmetric, the category is de-activated)

			float max = cat.hist_mc->GetMaximum();
			float left = cat.hist_mc->GetBinContent(1);
			float right = cat.hist_mc->GetBinContent(cat.hist_mc->GetNbinsX());
			if((right - left) / max > 0.2 || (left - right) / max > 0.4) {
				cat.active = false;
				std::cout << "[INFO] Category: " << ZeeCategories.size()
				          << ": " << cat.categoryName1 << "\t" << cat.categoryName2
				          << " has been deactivated for high sholder: " << right << " " << left << " " << max
				          << std::endl;
			}

			//------------------------------
			if (_autoBin) {
				//SetAutoBin(cat,cat.invMass_min-20,cat.invMass_max+20);
				AutoNBins(cat);
			}
			cat.nSmearToy = _nSmearToy;
			std::cout << "number of smear toy: " << _nSmearToy << std::endl;
			if(smearscan || (cat.active && _autoNsmear)) AutoNSmear(cat);



			cat.nLLtoy = 1;
			cat.nll = 0;
#ifdef DEBUG
			std::cout << "[INFO] Category: " << ZeeCategories.size()
			          << "\t" << cat.categoryName1 << "\t" << cat.categoryName2
			          << "\t" << cat.categoryIndex1 << "\t" << cat.categoryIndex2
			          << "\t" << cat.nSmearToy
			          << "\t" << cat.nLLtoy
			          << "\t" << cat.nBins
			          << "\t" << cat.mc_events->size()
			          << "\t" << cat.data_events->size()
			          << "\t" << cat.hist_mc->Integral()
			          << "\t" << cat.hist_data->Integral()
			          << "\t" << cat.active
			          << "\t" << right
			          << "\t" << left
			          << "\t" << max
			          << std::endl;
#endif
			ZeeCategories.push_back(cat);

			index++;
		}
	}

}


TH1F *RooSmearer::GetSmearedHisto(ZeeCategory & category, bool isMC,
                                  bool smearEnergy, bool forceNew, bool multiSmearToy)
{

// #ifdef MEM_DEBUG
//   ProcInfo_t info;
//   gSystem->GetProcInfo(&info);
//   std::cout << "GET MEM INFO AT GetSmearedHisto START " << info.fMemResident << std::endl;
// #endif

	TH1F **h = NULL;
	if(isMC) h = (smearEnergy) ? &category.smearHist_mc : &category.hist_mc;
	else h = (smearEnergy) ? &category.smearHist_data : &category.hist_data;

#ifdef DEBUG
	if(h == NULL) {
		std::cerr << "[ERROR] histogram pointer is NULL!!!" << std::endl;
		exit(1);
	}
	//std::cerr << "[DEBUG] isMC: " << isMC << "\tsmearEnergy: " << smearEnergy << std::endl;
#endif

	// to be put as single if
	if(smearEnergy == false && (*h)->GetEntries() != 0) return *h;
	if(smearEnergy == true && isMC == false && (*h)->GetEntries() != 0) return *h;

	zee_events_t *cache = (isMC) ? category.mc_events : category.data_events;
	if(cache->size() == 0) return *h;

	if(smearEnergy) {

		//bool changed=isCategoryChanged(category);

		//if(changed || forceNew){ //else use the old smeared histo!
		(*h)->Reset();
		if(isMC && multiSmearToy) {
			//std::cout << "[DEBUG] Smearing MC: " << category.categoryName1 << " " << category.categoryName2 << std::endl;
			SetSmearedHisto(*cache,
			                category.pars1, category.pars2,
			                category.categoryName1, category.categoryName2, category.nSmearToy,
			                *h);
		} else {
			SetSmearedHisto(*cache,
			                category.pars1, category.pars2,
			                category.categoryName1, category.categoryName2, 1,
			                *h);
		}
		if(isMC) (*h)->Scale(1. / (*h)->Integral());
		//} //else  std::cout << "Not changed: " << category.categoryName1 << "\t" << category.categoryName2 << std::endl;

		(*h)->Smooth();
	} else {
		(*h)->Reset(); // unuseful: histogram should be empty
		SetHisto(*cache, *h);
	}

//   if(isMC==false){
//     delete cache;
//   }
// #ifdef MEM_DEBUG
//   gSystem->GetProcInfo(&info);
//   std::cout << "GET MEM INFO AT GetSmearedHisto END " << info.fMemResident << std::endl;
// #endif

	return *h;
}

void RooSmearer::SetHisto(const zee_events_t& cache, TH1F * hist) const
{
#ifdef DEBUG
	hist->Print();
#endif
	hist->Reset();
	for(zee_events_t::const_iterator event_itr = cache.begin();
	        event_itr != cache.end();
	        event_itr++) {
		hist->Fill( event_itr->invMass,
		            //sqrt(2 * event_itr->energy_ele1 * event_itr->energy_ele2 * event_itr->angle_eta_ele1_ele2),
		            event_itr->weight);
	}
#ifdef DEBUG
	hist->Print();
#endif

	return;
}

void RooSmearer::SetSmearedHisto(const zee_events_t& cache,
                                 RooArgSet pars1, RooArgSet pars2,
                                 TString categoryName1, TString categoryName2, unsigned int nSmearToy,
                                 TH1F * hist) const
{
#ifdef CPU_DEBUG
	//  myClock->Stop(); myClock->Start();
#endif

	// retrieve values from params for the category
	float scale1 = pars1.getRealValue("scale_" + categoryName1, 0., kTRUE);
	float constant1 = pars1.getRealValue("constTerm_" + categoryName1, 0., kTRUE);
	float alpha1 = pars1.getRealValue("alpha_" + categoryName1, 0., kTRUE);

	float scale2 = pars2.getRealValue("scale_" + categoryName2, 0., kTRUE);
	float constant2 = pars2.getRealValue("constTerm_" + categoryName2, 0., kTRUE);
	float alpha2 = pars2.getRealValue("alpha_" + categoryName2, 0., kTRUE);

#ifdef DEBUG
	pars1.writeToStream(std::cout, kFALSE);
	std::cout << "--" << std::endl;
	pars2.writeToStream(std::cout, kFALSE);
	//std::cout << "---" << std::endl;
	//_paramSet.writeToStream(std::cout, kFALSE);
#endif
//   if(nSmearToy>NSMEARTOYLIM){
//     std::cerr << "[ERROR] nSmearToy = " << nSmearToy << std::endl;
//     exit(1);
//   }

	//double smearEne1[NSMEARTOYLIM], smearEne2[NSMEARTOYLIM]; // _nSmearToy<100
	double smearEne1[1000], smearEne2[1000]; // _nSmearToy<100
	if(cache.begin()->smearings_ele1 == NULL) {
		std::cerr << "[ERROR] No smearings" << std::endl;
		exit(1);
	}
	for(zee_events_t::const_iterator event_itr = cache.begin();
	        event_itr != cache.end();
	        event_itr++) {

		//#ifdef FIXEDSMEARINGS
		smearedEnergy(smearEne1, nSmearToy, event_itr->energy_ele1, scale1, alpha1, constant1, event_itr->smearings_ele1);
		smearedEnergy(smearEne2, nSmearToy, event_itr->energy_ele2, scale2, alpha2, constant2, event_itr->smearings_ele2);
		//#else
		//	// random gen time is consuming!!! test different _nSmearToy to verify
		//    smearedEnergy(smearEne1, nSmearToy, event_itr->energy_ele1, scale1, alpha1, constant1,NULL);
		//    smearedEnergy(smearEne2, nSmearToy, event_itr->energy_ele2, scale2, alpha2, constant2,NULL);
		//#endif
//     if(event_itr==cache.begin()){
//       std::cout << "fixedSmearings: " << event_itr->smearings_ele1[0] << "\t"  << scale1 << "\t" << alpha1 << "\t" << event_itr->energy_ele1 << "\t" << constant1 << "\t" << smearEne1[0] << std::endl;
//     }
		for(unsigned int iSmearToy = 0; iSmearToy < nSmearToy; iSmearToy++) {
			hist->Fill(event_itr->invMass * sqrt(smearEne1[iSmearToy] * smearEne2[iSmearToy]),
			           event_itr->weight);
		}
	}
	hist->Scale(1. / nSmearToy);
//   if(hist->GetEntries()<hist->Integral()){
//     hist->Print();
//     std::cout << hist->GetName() << "\t" << hist->GetEntries() << "\t" << hist->Integral() << "\t" << cache.size() << std::endl;
//   }

	//  hist->Scale(1./hist->Integral()); // now saved as pdf

#ifdef CPU_DEBUG
	//  myClock->Stop();
	//  std::cout << "nSmearToy: " << nSmearToy;      myClock->Print();
#endif
	return;
}



double RooSmearer::smearedEnergy(double * smear, unsigned int nGen, float ene, float scale, float alpha, float constant, const float * fixedSmearings) const
{
	// sigmaMB = sigma Material Budget
	// if I want to take into account the non perfet simulation of the
	// material budget in front of ECAL, I have to add in quadrature an
	// additional smearing term sigmaMB

	// float sigmaMB = sigmaMB(ene, eta, phi);
	// sigmaMB non dovrebbe essere una smooth function, ma tenere in
	// conto differenze consistenti in piccole regioni.
	//  float sigmaMB = MB * fabs{etaEle} * fabs{etaEle}
	//   float sigma = sqrt(alpha/sqrt(ene)*alpha/sqrt(ene) + constant * constant + sigmaMB*sigmaMB);

	float sigma = sqrt(alpha * alpha / ene + constant * constant );
	if(sigma == 0) {
		for(unsigned int i = 0; i < nGen; i++) {
			smear[i] = scale;
		}
	} else {
#ifdef FIXEDSMEARINGS
		for(unsigned int i = 0; i < NSMEARTOYLIM && i < nGen; i++) {
			smear[i] = (double) (fixedSmearings[i] * sigma) + (scale);
		}
		for(unsigned int i = NSMEARTOYLIM; i < nGen; i++) {
			smear[i] = rgen_->Gaus(scale, sigma);
		}
#else
		for(unsigned int i = 0; i < nGen; i++) {
			smear[i] = rgen_->Gaus(scale, sigma);
		}
#endif
	}
	return smear[0];
}



double RooSmearer::getLogLikelihood(TH1F * data, TH1F * prob) const
{
	if (!data || !prob) {
		std::cout << "ERROR: empty histograms given" << std::endl;
		return -9999999.;
	}
	if (data->GetNbinsX() != prob->GetNbinsX()) {
		std::cout << "ERROR: data and probabilities are binned differently" << std::endl;
		return -9999999.;
	}

#ifdef FUNC_DEBUG
	//  std::cout << "Probability integral " << prob->Integral() << std::endl;
#endif

	//   if (fabs(prob->Integral() -1 ) > 1e-4 )
	//     {
	//       std::cout << "ERROR: probabilities are not properly normalized" << std::endl;
	//       return -9999999.;
	//     }

	double logL = 0.;
	//Not using underflows and overflows at the moment
	for (int ibin = 1; ibin < data->GetNbinsX() + 1; ++ibin) {
#ifdef FUNC_DEBUG
		std::cout << "ibin " << ibin << "\tdata " << data->GetBinContent(ibin) << "\texpected " << prob->GetBinContent(ibin) << "\t MC " << prob->GetBinContent(ibin) * 514 << "\tlogL " << logL; //<< std::endl;
		std::cout << std::fixed;
		std::cout << setprecision(10) << "\tdeltaLogL =  " <<
		          data->GetBinContent(ibin)* ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)) + (data->Integral() - data->GetBinContent(ibin))*ROOT::Math::Util::EvalLog(1 - prob->GetBinContent(ibin)) << "\t EvalLog = " << ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)) << std::endl;

#endif

		//original version
//      if(prob->GetBinContent(ibin)!=0){
//
//	double weight= 	(data->GetBinContent(ibin)>0) ? data->GetBinError(ibin) * data->GetBinError(ibin) / data->GetBinContent(ibin) : 1;
//	//	std::cout << "weight = " << weight << std::endl;
//#ifdef POISSON_LIKELIHOOD
//	//Poisson likelihood
//	logL+=  weight * (data->GetBinContent(ibin)* ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)) - prob->GetBinContent(ibin));
//#else
		// Binomial likelihood
		if(prob->GetBinContent(ibin) > 0) {
			double weight = 	(data->GetBinContent(ibin) > 0) ? data->GetBinError(ibin) * data->GetBinError(ibin) / data->GetBinContent(ibin) : 1;
			logL += weight * (data->GetBinContent(ibin) * ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)) + (data->Integral() - data->GetBinContent(ibin)) * ROOT::Math::Util::EvalLog(1 - prob->GetBinContent(ibin)));
		}
		//#endif
		//	}

		//Multinomial
		//if(prob->GetBinContent(ibin)>0){
		//double weight= 	(data->GetBinContent(ibin)>0) ? data->GetBinError(ibin) * data->GetBinError(ibin) / data->GetBinContent(ibin) : 1;
		//double weight=1;
		//logL+= weight * (data->GetBinContent(ibin)* ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)));
		//}

#ifdef DEBUG
		else {
			std::cout << "MC ibin = 0: "
			          << "ibin " << ibin << "\tdata " << data->GetBinContent(ibin)
			          << "\texpected " << prob->GetBinContent(ibin)
			          << "\tlogL " << logL
			          << "\tdeltaLogL =  " <<
			          data->GetBinContent(ibin)* ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)) + (data->Integral() - data->GetBinContent(ibin))*ROOT::Math::Util::EvalLog(1 - prob->GetBinContent(ibin))
			          << "\t EvalLog = " << ROOT::Math::Util::EvalLog(prob->GetBinContent(ibin)) << std::endl;
		}
#endif
	}
	//std::cout << "Finale logL = " << logL <<std::endl;
	return logL;
}



double RooSmearer::getCompatibility(bool forceUpdate) const
{
	RooSmearer* myClass = (RooSmearer *) this;
	// myClock->Start();

	std::vector<TH1F *> mcHistos, dataHistos;

	double compatibility = 0.;
	//------------------------------ fill the histograms
//   for(std::vector<ZeeCategory>::iterator cat_itr = myClass->ZeeCategories.begin();
//       cat_itr != myClass->ZeeCategories.end();
//       cat_itr++){
//     if(!cat_itr->active) continue;
//     if(isCategoryChanged(*cat_itr,false)){
//      myClass->UpdateCategoryNLL(*cat_itr, cat_itr->nLLtoy,false); //the new nll has been updated for the category
//     }
//     compatibility+=cat_itr->nll;
//     myClass->lastNLLrms+= (cat_itr->nllRMS*cat_itr->nllRMS);
//   }

	//bool withSmearToy=(compatibility - nllMin < deltaNLLMaxSmearToy);
	//std::cout << "[DEBUG] Compatibility1: " << compatibility << "\t" << compatibility - nllMin << std::endl;
	//compatibility=0;
	bool updated = false;
	for(std::vector<ZeeCategory>::iterator cat_itr = myClass->ZeeCategories.begin();
	        cat_itr != myClass->ZeeCategories.end();
	        cat_itr++) {
		if(!cat_itr->active) continue;
		if(forceUpdate || isCategoryChanged(*cat_itr, true)) { // && withSmearToy){
			updated = true;
#ifdef DEBUG
			std::cout << "[DEBUG] " << cat_itr->categoryName1 << " - " << cat_itr->categoryName2 << "\t isupdated" << std::endl;
#endif
			myClass->UpdateCategoryNLL(*cat_itr, cat_itr->nLLtoy); //the new nll has been updated for the category
		}
		compatibility += cat_itr->nll;
		myClass->lastNLLrms += (cat_itr->nllRMS * cat_itr->nllRMS);
#ifdef DEBUG

#endif
		//    std::cout << std::fixed << std::setprecision(20) << "[DEBUG] Compatibility: " << compatibility << "\t" << compatibility-lastNLL << std::endl;
	}
	//if(withSmearToy) std::cout << "[DEBUG] Compatibility2: " << compatibility << "\t" << compatibility - nllMin << std::endl;
	if(dataset != NULL && updated) {
		myClass->nllVar.setVal(compatibility);
		myClass->dataset->add(RooArgSet(_paramSet, nllVar));
	}

	myClass->lastNLL = compatibility;
	myClass->lastNLLrms = sqrt(lastNLLrms);

	if(nllBase == 0) myClass->nllBase = compatibility;
	else {
		if(nllMin > compatibility) {
			//  std::cout << "[DEBUG] update nllMin: " << nllMin << " -> " << compatibility << std::endl;
			myClass->nllMin = compatibility;
		}
	}

	//  myClock->Stop();
	//  myClock->Print();
	return compatibility;
}





Double_t RooSmearer::evaluate() const
{
#ifdef CPU_DEBUG
	myClock->Start();
#endif

#ifdef MEM_DEBUG
	ProcInfo_t info;
	gSystem->GetProcInfo(&info);
	std::cout << "GET MEM INFO AT  EVALUATE START " << info.fMemResident << std::endl;
#endif

	//update last result
	double comp_mean = getCompatibility();

#ifdef CPU_DEBUG
	myClock->Stop();
	std::cout << "Elapsed time for eval: CPU " << myClock->CpuTime() << "; Real " <<  myClock->RealTime() << " s " << std::endl;
	//  myClock->Start();
#endif

#ifdef MEM_DEBUG
	gSystem->GetProcInfo(&info);
	std::cout << "GET MEM INFO JUST BEFORE END EVALUATE " << info.fMemResident << std::endl;
#endif

	//#ifdef CPU_DEBUG
	//  myClock->Print();
	//#endif

	//_markov.AddFast(*((RooArgSet *)_params.Clone()), comp_mean, 1.);
	RooSmearer* myClass = (RooSmearer *) this;
	double weight = (nllBase * 2 - comp_mean);
	if(weight < 0) weight = 1;
	myClass->_markov.AddFast(myClass->_paramSet, comp_mean, weight);
#ifdef CPU_DEBUG
	///  myClock->Stop();
	//  std::cout << "Elapsed time for add to MarkovChain: CPU " << myClock->CpuTime() << "; Real " <<  myClock->RealTime() << " s " << std::endl;
#endif

	//_paramSet.writeToStream(std::cout, kFALSE);
	//  _paramSet.first()->Print();
	//  std::cout << "    " << comp_mean << std::endl;
	return comp_mean;
	//    return compatibility;
}




int RooSmearer::Trag_eq(int row, int col, int N) const
{
	if (row <= col)
		return row * N - (row - 1) * ((row - 1) + 1) / 2 + col - row;
	else
		return col * N - (col - 1) * ((col - 1) + 1) / 2 + row - col;
}




void RooSmearer::SetAutoBin(ZeeCategory & category, double min, double max)
{
	category.hist_mc->SetBins(1000, min, max);
	category.hist_mc->Reset();
	TH1F* wide = GetSmearedHisto(category, true/*bool isMC*/, false);
	Int_t nq = 100;
	double xq[100], yq[100];
	for (Int_t i = 0; i < nq; i++)
		xq[i] = (double)(i + 1) / nq;
	wide->GetQuantiles(nq, yq, xq);

	//  category.invMass_min = yq[1];
	//  category.invMass_max = yq[95];

	// center the mean of the distribution
	float middle = 0.5 * (category.invMass_max + category.invMass_min);

	if(fabs(middle - wide->GetMean()) > 5) {
		int shift = (int)(wide->GetMean() - middle);
		std::cout << "[INFO] Shifting invMass range of " << shift << std::endl;
		std::cout << "       invMass_min = " << category.invMass_min + shift << std::endl;
		std::cout << "       invMass_max = " << category.invMass_max + shift << std::endl;
		category.invMass_min += shift;
		category.invMass_max += shift;
	}
	ResetBinning(category);
}

void RooSmearer::ResetBinning(ZeeCategory & category)
{
	category.hist_mc->SetBins(category.nBins, category.invMass_min, category.invMass_max);
	category.hist_mc->Reset();
	category.smearHist_mc->SetBins(category.nBins, category.invMass_min, category.invMass_max);
	category.smearHist_mc->Reset();

	category.hist_data->SetBins(category.nBins, category.invMass_min, category.invMass_max);
	category.hist_data->Reset();
	if(category.smearHist_data != NULL) {
		category.smearHist_data->SetBins(category.nBins, category.invMass_min, category.invMass_max);
		category.smearHist_data->Reset();
	}
	return;
}


void RooSmearer::AutoNBins(ZeeCategory & category)
{
	if(!category.active) return;
	if(category.hist_mc->Integral() > 30000) {
		std::cout << "[INFO] Category: " << category.categoryName1 << " " << category.categoryName2 << "\t binning increased to: "  <<     category.nBins * 2 << std::endl;
		category.nBins *= 2;
		ResetBinning(category);
	} else   if(category.hist_mc->Integral() < 2500) { //5000
		std::cout << "[INFO] Category: " << category.categoryName1 << " " << category.categoryName2 << "\t binning reduced to: "  <<     category.nBins / 2 << std::endl;
		category.nBins /= 2;
		ResetBinning(category);
	}

	SetHisto(*(category.mc_events),   category.hist_mc);
	SetHisto(*(category.data_events), category.hist_data);
	if(_isDataSmeared) {
		std::cout << "[INFO toyMC]: Rebinning SmearHist Data" << std::endl;
		SetSmearedHisto(*(category.data_events),
		                category.pars1, category.pars2,
		                category.categoryName1, category.categoryName2, 1,
		                category.smearHist_data);
	}

	return;
	//------------------------------ rescale mc histograms

	//  (isCategoryChanged(category,true)); // && withSmearToy){
	category.scale1 = 1;
	TH1F *data = GetSmearedHisto(category, false, _isDataSmeared, true, false); ///-----> not need to repeate!
	TH1F *mc = GetSmearedHisto(category, true, true, true); // regenerate the histogram: forceNew
	double chi2old = getLogLikelihood(data, mc);
	double min = chi2old;
	int nBin_min = 0;

	std::cout << "[AUTOBIN] Starting scale1 sensitivity check: " << category.scale1 << "\t" << std::fixed << std::setprecision(10) << chi2old << std::endl;
	double valueMin = category.scale1;

	for(int n = 0; n < 10; n++) {
		double value = category.scale1 - 0.01 + 0.002 * n;
		category.pars1.setRealValue("scale_" + category.categoryName1, value , kTRUE);

		ResetBinning(category);
		data = GetSmearedHisto(category, false, _isDataSmeared, true, false); ///-----> not need to repeate!
		mc = GetSmearedHisto(category, true, true, true); // regenerate the histogram: forceNew
		double chi2 = getLogLikelihood(data, mc);
		if(chi2 < min) {
			min = chi2;
			valueMin = value;
		}
		std::cout << "[BINNING] scale1: " << value << "\t" << chi2 - chi2old << "\t" << chi2 - min << std::endl;
	}
	if(category.scale1 != valueMin) {
		std::cout << "[WARNING] scale1 not closing: " << category.scale1 << "\t" << valueMin << std::endl;
	}
	return;

	category.nBins = nBin_min;
	ResetBinning(category);
	return;
}

void RooSmearer::AutoNSmear(ZeeCategory & category)
{


	if(!category.active) return;
	//AutoNBins(category);
	return;
	//  category.nSmearToy=std::max(NSMEARTOYLIM*2,200);
	//Long64_t catSize=category.mc_events->size();
//   if(catSize<3000){
//     category.nBins=40;
//     ResetBinning(category);
//   } else if(catSize>40000){
//     category.nBins=160;
//     ResetBinning(category);
//   }

//   category.nSmearToy=(int)(300000./catSize);
//   if(category.nSmearToy <7) category.nSmearToy = 7; // fix the min to 3
//   else  if( category.nSmearToy > 40) category.nSmearToy = 40; // fix the max to 20
	//  category.nSmearToy=NSMEARTOYLIM;
	if(!smearscan) return;

	//------------------------------ rescale mc histograms
	double stdDev = 10, stdDevLim = 0.3;
	double min = 10; //int nBin_min=0;
	int n = 0;
	unsigned int nSmearToyLim = 1000;
	category.nLLtoy = 1;
	for(int iBin = 160; iBin > 120; iBin /= 2) {
		//category.nBins= iBin;
		//ResetBinning(category);
		//TH1F *data = GetSmearedHisto(category, false, _isDataSmeared, false, false); ///-----> not need to repeate!
		GetSmearedHisto(category, false, _isDataSmeared, false, false); ///-----> not need to repeate!

		for(category.nLLtoy = 1; category.nLLtoy < 2; category.nLLtoy += 2) {
			for(; category.nSmearToy <= nSmearToyLim && stdDev > stdDevLim; category.nSmearToy *= 2) {
				double  sum = 0, sum2 = 0;
				TStopwatch cl;
				cl.Start();
				for(n = 0; n < 50; n++) {
					UpdateCategoryNLL(category, category.nLLtoy); //the new nll has been updated for the category

					sum += category.nll;
					sum2 += category.nll * category.nll;
				}
				cl.Stop();
				sum /= n;
				sum2 /= n;
				stdDev = sqrt(sum2 - sum * sum);
				if(stdDev / sum < min) {
					min = stdDev / sum;
					//nBin_min=category.nBins;
				}


				std::cout << "[SMEARSCAN] "
				          << "\t" << category.categoryIndex1
				          << " " << category.categoryIndex2
				          << "\t" << stdDev
				          << "\t" << sum
				          << " " << stdDev / sum
				          << "\t" << category.nSmearToy
				          << " " << category.nLLtoy
				          << " " << category.nBins
				          << "\tmcEvents=" << category.mc_events->size()
				          << "\tdataEvents=" << category.data_events->size()
				          << "\t" << cl.CpuTime() / 50. << std::endl;
				//<< std::endl;
			}
			//if(category.nSmearToy > nSmearToyLim) category.nSmearToy/=2;
			category.nSmearToy /= 2;
		}
	}

	//  exit(0);
	return;
#ifdef DD
	//============================================================

	AutoNBins(category);

	Long64_t catSize = category.mc_events->size();
	category.nSmearToy = (int)(std::max(1., 1000000. / catSize));
	std::cout
	        << "\t" << category.categoryName1 << " " << category.categoryName2
	        << "\tnSmearToy=" << category.nSmearToy
	        << std::endl;
	return;
#ifndef DUMPHIST
	//  AutoNBins(category);
#endif
	if(!category.active) return;
	TH1F *data = GetSmearedHisto(category, false, _isDataSmeared, false, false); ///-----> not need to repeate!


	//------------------------------ rescale mc histograms
	double stdDev = 0, mean = 0;
	int n = 0;


	float nSmearToyLim = std::max(1., 100000. / catSize);
	for(category.nLLtoy = 1; category.nLLtoy <= 1; category.nLLtoy += 2) {
		for(category.nSmearToy = 1; category.nSmearToy <= nSmearToyLim; category.nSmearToy += 5) {

			double  sum = 0, sum2 = 0;
			for(n = 0; n < 500; n++) {
				double comp = 0, comp2 = 0.;
				for(unsigned int itoy = 0; itoy < category.nLLtoy; itoy++) {
					TH1F *mc = GetSmearedHisto(category, true, true, true); // regenerate the histogram: forceNew
#ifdef DUMPHIST
					TString histName = "tmp/hist3/";
					histName += category.mc_events->size();
					histName += "-";
					histName += category.nSmearToy;
					histName += "-";
					histName += category.nLLtoy;
					histName += "-";
					histName += category.nBins;
					histName += "-";
					histName += n;
					histName += "-";
					histName += importer._constTermToy;
					histName += ".root";
					mc->SaveAs(histName);
#endif
					double c = getLogLikelihood(data, mc);
					comp += c;
					comp2 += c * c;
				}
				comp /= category.nLLtoy;
				comp2 /= category.nLLtoy;
				sum += comp;
				sum2 += comp * comp;
			}

			mean = sum / n;
			stdDev = sqrt(sum2 / n - mean * mean);
			std::cout << "[DEBUG] n = " << n
			          << "\t" << category.categoryIndex1
			          << "\t" << category.categoryIndex2
			          << "\t" << stdDev
			          << "\t" << mean
			          << "\t" << stdDev / mean
			          << "\t" << category.nSmearToy
			          << "\t" << category.nLLtoy
			          << "\t" << category.nBins
			          << "\tmcEvents=" << category.mc_events->size()
			          << "\tdataEvents=" << category.data_events->size()
			          << std::endl;
		}
	}


	exit(0);

	do {
		category.nSmearToy += 5;
		double  sum = 0, sum2 = 0;
		for(n = 0; n < 500; n++) {
			TH1F *mc = GetSmearedHisto(category, true, true, true); // forceNew
			TH1F *h = (TH1F *) mc->Clone(TString(mc->GetName()) + "_c");
			h->Scale(1. / h->Integral()); // now the histogram is a pdf
			double compatibility = fabs(getLogLikelihood(data, h ));
			sum += compatibility;
			sum2 += compatibility * compatibility;
			stdDev = sum2 / (n + 1) - (sum / (n + 1)) * (sum / (n + 1));
			//std::cout << "[DEBUG] n = " << n << "\t" << compatibility << "\t" << stdDev << "\t" << sum2/(n+1) << "\t" << sum/(n+1) << std::endl;
			delete h;
		}
		mean = sum / n;
		stdDev = sqrt(sum2 / n - mean * mean);
		std::cout << "[DEBUG] n = " << n
		          << "\t" << stdDev
		          << "\t" << mean
		          << "\t" << stdDev / mean
		          << "\t" << category.nSmearToy
		          << "\t" << category.nBins
		          << "\tmcEvents=" << category.mc_events->size()
		          << "\tdataEvents=" << category.data_events->size()
		          << std::endl;
	} while (stdDev / mean > 0.0005 && category.nSmearToy < 2);
	std::cout << "[INFO] Category: "
	          << category.categoryIndex1 << " " << category.categoryIndex2
	          //<< "\t" << category.categoryName1 << " " << category.categoryName2
	          << "\tstdDev=" << stdDev << "\tnSmearToy=" << category.nSmearToy
	          << "\tmcEvents=" << category.mc_events->size()
	          << "\tdataEvents=" << category.data_events->size()
	          << std::endl;

	//TH1F *mc = GetSmearedHisto(category, true, true); // forceNew
	//  data->SaveAs("data.root");
	//mc->SaveAs("mc.root");
#endif
	exit(0);
	return;
}



bool RooSmearer::isCategoryChanged(ZeeCategory & category, bool updateVar) const
{
#ifdef DEBUG
	std::cout << "[DEBUG] Checking if category changed: " << category.categoryName1 << "\t" << category.categoryName2 << std::endl;
#endif
	bool changed = false;

	RooArgList argList1(category.pars1);
	RooArgList argList2(category.pars2);

	// checking if one of the variables has changed
	TIterator *it = argList1.createIterator();
	for(RooRealVar *v = (RooRealVar *) it->Next(); v != NULL; v = (RooRealVar*) it->Next()) {
		TString varName = v->GetName();
		double varValue = v->getVal();
		if(varName.Contains("scale") && varValue != category.scale1) {
			changed = true;
#ifdef DEBUG
			std::cout << "scale changed for: " << category.categoryName1 << "\t" << category.scale1 << "\t" << varValue << "\t" << changed << "\t" << updateVar << std::endl;
#endif
			if(updateVar) category.scale1 = varValue;
		}
		if(varName.Contains("alpha")     && varValue != category.alpha1) {
			changed = true;
#ifdef DEBUG
			std::cout << "alpha changed for: " << category.categoryName1 << "\t" << category.alpha1 << "\t" << varValue << "\t" << changed << std::endl;
#endif
			if(updateVar) category.alpha1 = varValue;
		}
		if(varName.Contains("constTerm") && varValue != category.constant1) {
			changed = true;
#ifdef DEBUG
			std::cout << "alpha changed for: " << category.categoryName1 << "\t" << category.constant1 << "\t" << varValue << "\t" << changed << std::endl;
#endif
			if(updateVar) category.constant1 = varValue;
		}
	}

	delete it;
	it = argList2.createIterator();
	for(RooRealVar *v = (RooRealVar *) it->Next(); v != NULL; v = (RooRealVar*) it->Next()) {
		TString varName = v->GetName();
		double varValue = v->getVal();

		if(varName.Contains("scale")     && varValue != category.scale2) {
			changed = true;
			if(updateVar) category.scale2 = varValue;
		}
		if(varName.Contains("alpha")     && varValue != category.alpha2) {
			changed = true;
			if(updateVar) category.alpha2 = varValue;
		}
		if(varName.Contains("constTerm") && varValue != category.constant2) {
			changed = true;
			if(updateVar) category.constant2 = varValue;
		}
	}

	delete it;
	return changed;
}


void RooSmearer::SetNSmear(unsigned int n_smear, unsigned int nlltoy)
{

	for(std::vector<ZeeCategory>::iterator cat_itr = ZeeCategories.begin();
	        cat_itr != ZeeCategories.end();
	        cat_itr++) {
		if(n_smear > 0) cat_itr->nSmearToy = n_smear;
		if(nlltoy > 0) cat_itr->nLLtoy = nlltoy;
	}
	return;
}


void RooSmearer::Init(TString commonCut, TString eleID, Long64_t nEvents, bool mcToy, bool externToy, TString initFile)
{
	if(mcToy) _isDataSmeared = !externToy; //mcToy;
	std::cout << "[INFO} in RooSmearer::Init: _isDataSmeared is" << _isDataSmeared << std::endl;
	if(initFile.Sizeof() > 1) {
		std::cout << "[INFO] Truth values for toys initialized to " << std::endl;
		//truthSet->readFromFile(initFile);
		//truthSet->writeToStream(std::cout, kFALSE);
		std::cout << "------------------------------ Read init toy MC:" << std::endl;
		_paramSet.readFromFile(initFile);
		_paramSet.writeToStream(std::cout, kFALSE);
	}
	SetCommonCut(commonCut);
	SetEleID(eleID);
	SetCache(nEvents, mcToy, externToy);
	InitCategories(mcToy);
	TStopwatch cl;
	cl.Start();
	evaluate();
	cl.Stop();
	std::cout << "[INFO] Time for first eval: ";
	cl.Print();
	if(mcToy && false) {
		RooArgList argList(_paramSet);
		TIterator *it = argList.createIterator();
		for(RooRealVar *var = (RooRealVar *) it->Next(); var != NULL; var =  (RooRealVar *)it->Next()) {
			var->randomize();
		}
		std::cout << "------------------------------ Randomize initial value:" << std::endl;
		_paramSet.writeToStream(std::cout, kFALSE);
	}

	// set initial nll values
	getCompatibility(true);
	return;
}

void RooSmearer::UpdateCategoryNLL(ZeeCategory & cat, unsigned int nLLtoy, bool multiSmearToy)
{
	TH1F *data = GetSmearedHisto(cat, false, _isDataSmeared, true, false); ///-----> not need to repeate! 1 one smearing! otherwise bin errors are wrongly reduced

	double comp = 0., comp2 = 0.;
	for(unsigned int itoy = 0; itoy < nLLtoy; itoy++) {
		TH1F *mc = GetSmearedHisto(cat, true, true, true, true); // regenerate the histogram: forceNew

		double c = getLogLikelihood(data, mc);
		comp += c;
		comp2 += c * c;
	}
	comp /= nLLtoy;
	comp2 /= nLLtoy;
//   std::cout << std::fixed << setprecision(10) << cat.categoryName1 << "  " << cat.categoryName2
// 	    << "\t nll - comp = " << cat.nll +comp << std::endl;
	cat.nll = -comp;
	cat.nllRMS = sqrt(comp2 - comp * comp);
	return;
}

void RooSmearer::DumpNLL(void) const
{
	std::cout << "[DUMP NLL] " << "Cat1\tCat2\tNLL\tNevt mc\tNevt data\tisActive\tNevt mc\tNevt data" << std::endl;

	for(std::vector<ZeeCategory>::const_iterator cat_itr = ZeeCategories.begin();
	        cat_itr != ZeeCategories.end();
	        cat_itr++) {
		if(!cat_itr->active)
			std::cout << "[DUMP NLL] " << std::setprecision(10)
			          << cat_itr->categoryIndex1 << " " << cat_itr->categoryIndex2
			          << "\t" << cat_itr->nll
			          << "\t" << cat_itr->mc_events->size() << "\t" << cat_itr->data_events->size()
			          << "\t1"
			          << "\t" << cat_itr->hist_mc->Integral() << "\t" << cat_itr->hist_data->Integral()
			          << std::endl;
		else
			std::cout << "[DUMP NLL] " << std::setprecision(10)
			          << cat_itr->categoryIndex1 << " " << cat_itr->categoryIndex2
			          << "\t" << cat_itr->nll
			          << "\t" << cat_itr->mc_events->size() << "\t" << cat_itr->data_events->size() << "\t0"
			          << "\t" << cat_itr->hist_mc->Integral() << "\t" << cat_itr->hist_data->Integral()
			          << std::endl;
	}
	return;
}

