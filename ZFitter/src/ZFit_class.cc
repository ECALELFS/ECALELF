/// Zee Fit class: BW+CB and Cruijff

/**\class ZFit_class ZFit_class.cc Calibration/ZFitter/src/ZNtupleDumper.cc
Description: class that provide flexible and simple way to fit the Zee invariant mass shape with different pdfs:
 - BW+CB
 - Cruijff
*/

/** \todo
 */

#include "../interface/ZFit_class.hh"
#include "../interface/ZFit_class.hh"
#include <TList.h>
#include <TObject.h>
#include <TFriendElement.h>
#include <TEntryList.h>
#include <TStopwatch.h>
#include <fstream>
#include <TTreeFormula.h>
//       gSystem -> to be replaced with standard c or c++ commands
//       implement dynamic change between unbinned and binned fit for < 400 events > 400 event

// this is because GetBranch seems not working properly, so will ignore that
//#define BUG
#include <TObjArray.h>
#include <TChainElement.h>


#define DEBUG
//#define FIT_DEBUG
//#define NOIMPORT
//#define NUM_EVENTS 100
ZFit_class::~ZFit_class(void)
{
	std::cout << "[STATUS] DESTROY ZFitter" << std::endl;
	if(commonMC  != NULL) delete commonMC;
	if(commonData != NULL) delete commonData;
	if(reducedMC  != NULL) delete reducedMC;
	if(reducedData != NULL) delete reducedData;

	return;
}


ZFit_class::ZFit_class(TChain *data_chain_,
                       TChain *signal_chain_,
                       TChain *bkg_chain_,
                       TString invMass_VarName, double invMass_min, double invMass_max, double bin_width
                      ):
	_isDataUnbinned(false), _isDataSumW2(false),
	_isMCUnbinned(true),    _isMCSumW2(true),
	_isToyUnbinned(false),  _isToySumW2(false),

	_forceNewFit(false),
	_initFitMC(true),
	_updateOnly(false),
	_oddMC(false), _oddData(false),

	_fit_type(1),
	data_chain(data_chain_),
	signal_chain(signal_chain_),
	bkg_chain(bkg_chain_),

	invMass(invMass_VarName, "Mee", invMass_min, invMass_max, "GeV/c^{2}"),
	invMass_highBinning(NULL),
	convBwCbPdf(invMass),
	cruijffPdf(invMass),

	weight("weight", "weight", 1, 0, 100),

	Vars(weight, invMass),

	plot_data(NULL),
	plot_MC(NULL),
	data(NULL),
	signal(NULL),

	cutter(),
	commonMC(NULL), commonData(NULL), reducedMC(NULL), reducedData(NULL)
{
	energyVar_name = GetEnergyVarName(invMass_VarName);
	cutter.energyBranchName = energyVar_name;

	/*--------------- imposto il binning */
	//double bin_width=.5;
	int nBins = (int)((invMass.getMax() - invMass.getMin()) / bin_width);
	invMass.setBins(10000);
	invMass.setBins(nBins, "plotRange");
	std::cout << "nBins = " << nBins << "\t" << invMass.getBins("plotRange") << std::endl;

	invMass.Print();

	SetPDF_model();

	setTDRStyle();
	//  TGaxis::SetMaxDigits(3);
	//  gStyle->SetLegendFillColor(0);
	gStyle->SetLegendBorderSize(1);
	gStyle->SetOptTitle(0);


	return;
}


void ZFit_class::Import(TString commonCut, TString eleID_, std::set<TString>& branchList)
{
	signal_chain->Draw(">>list", "runNumber>1", "entrylist", 10);
	TEntryList *l = (TEntryList*) gROOT->FindObject("list");
	assert(l!=NULL);

//	commonCut += "-eleID_" + eleID_;
	TString mcCut, dataCut;
	if(l->GetN() > 0) { // runDependent MC, treat it has data
		std::cout << "[INFO] Importing run dependent MC" << std::endl;
		if(_oddMC) mcCut = cutter.GetCut(commonCut + "-odd", false);
		else mcCut = cutter.GetCut(commonCut, false);
	} else {
		std::cout << "[INFO] Importing std MC" << std::endl;
		if(_oddMC) mcCut = cutter.GetCut(commonCut + "-odd", true);
		else mcCut = cutter.GetCut(commonCut, true);
	}
	delete l;

	if(_oddData) dataCut = cutter.GetCut(commonCut + "-odd", false);
	else dataCut = cutter.GetCut(commonCut, false);
#ifdef DEBUG	
	std::cout << "MC CUT: " << mcCut << std::endl;
	std::cout << "DATA CUT: " << dataCut << std::endl;
#endif
	std::cout << "------------------------------ IMPORT DATASETS" << std::endl;
	std::cout << "--------------- Importing signal mc: " << signal_chain->GetEntries() << std::endl;
	//if(signal!=NULL) delete signal;

	signal = ImportTree(signal_chain, mcCut, branchList);
	assert(signal->GetEntryList()!=NULL);
	commonMC = new TEntryList(*(signal->GetEntryList()));
	//signal->Print();
	//exit(0);
	std::cout << "--------------- Importing data: " << data_chain->GetEntries() << std::endl;
	//if(data!=NULL) delete data;
	data = ImportTree(data_chain, dataCut, branchList);
	commonData = new TEntryList(*(data->GetEntryList()));
	//commonData->Print();
	std::cout << "[INFO] imported " << commonData->GetN() << "\t" << (data->GetEntryList())->GetN() << " events passing commmon cuts" << std::endl;
	//data->Print();
	std::cout << "--------------- IMPORT finished" << std::endl;
	//  exit(0);
	return;
}




TChain *ZFit_class::ImportTree(TChain *chain, TString commonCut, std::set<TString>& branchList)
{

	if(branchList.size() > 0) {
		std::cout << "[STATUS] Disabling branches to fast import" << std::endl;
		chain->SetBranchStatus("*", 0);
	}

	for(std::set<TString>::const_iterator itr = branchList.begin();
	        itr != branchList.end();
	        itr++) {
		std::cout << "[STATUS] Enabling branch: " << *itr << std::endl;
		chain->SetBranchStatus(*itr, 1);
	}
	// abilitare la lista dei friend branch
	if(chain->GetBranch("scaleEle"))  chain->SetBranchStatus("scaleEle", 1);
	if(chain->GetBranch("smearEle")) chain->SetBranchStatus("smearEle", 1);
	if(chain->GetBranch("puWeight")) chain->SetBranchStatus("puWeight", 1);
	if(chain->GetBranch("r9Weight")) chain->SetBranchStatus("r9Weight", 1);
	chain->SetBranchStatus("R9Ele", 1);
	chain->SetBranchStatus("etaEle", 1);
	chain->SetBranchStatus("recoFlagsEle", 1);
	chain->SetBranchStatus("eleID", 1);
	chain->SetBranchStatus("eventNumber", 1);
	std::cout << "[STATUS] Enabling branch: " << invMass.GetName() << std::endl;
	chain->SetBranchStatus(invMass.GetName(), 1);
	chain->SetBranchStatus("invMass_SC", 1);
	chain->AddBranchToCache("*", kTRUE);

	//std::cout << commonCut << std::endl;
	TString evListName = "evList_";
	evListName += chain->GetTitle();
#ifdef DEBUG
	std::cout << commonCut << std::endl;
#endif
	commonCut="";
	chain->Draw(">>" + evListName, commonCut, "entrylist");
	TEntryList *elist = (TEntryList*)gDirectory->Get(evListName);
	assert(elist!=NULL);
	std::cout << "[INFO] Selected events: " << chain->GetTitle() << "\t" <<  elist->GetN() << std::endl;
#ifdef DEBUG
	elist->Print();
	TObjArray *fFiles = chain->GetListOfFiles();
    Int_t ne = fFiles->GetEntries();
    Int_t listfound=0;
    TString treename, filename;
 
    TEntryList *templist = 0;
    for (Int_t ie = 0; ie<ne; ie++){
       auto chainElement = (TChainElement*)fFiles->UncheckedAt(ie);
       treename = chainElement->GetName();
       filename = chainElement->GetTitle();
       templist = elist->GetEntryList(filename+"/"+treename, filename, "ne");
       if (templist) {
          listfound++;
          templist->SetTreeNumber(ie);
       }
	   std::cout << listfound << "\t"<< treename << "\t" << filename << std::endl;
    }

#endif
	chain->SetEntryList(elist, "ne");
	assert(chain->GetEntryList()!=NULL);
	//delete elist; // do not delete
	//std::cout << "[INFO] Selected events: " <<  chain->GetEntryList()->GetN() << std::endl;

	return chain;
}


RooDataHist *ZFit_class::ImportHist(TH1F *hist)
{

	// specifico se il tutto viene fatto su binned o unbinned datasets
	//  _isBinned=true;

	RooDataHist *data_hist = new RooDataHist(invMass.GetName(), invMass.GetTitle(),
	        RooArgSet(invMass), hist);

	return data_hist;
}




RooDataSet *ZFit_class::TreeToRooDataSet(TChain *chain, TEntryList *entryList)
{


	Float_t invMass_;

	Float_t weight_;
	weight_ = 1;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;

	chain->SetBranchAddress(invMass.GetName(), &invMass_);

	if(chain->GetBranch("puWeight") != NULL) {
		std::cout << "[STATUS] Adding pileup weight branch from friend" << std::endl;
		chain->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(chain->GetBranch("scaleEle") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("scaleEle", corrEle_);
	}

	if(chain->GetBranch("smearEle") != NULL && TString(chain->GetTitle()) != "d") {
		std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//chain->Scan("smearEle");
		chain->SetBranchAddress("smearEle", smearEle_);
	}

	if(chain->GetBranch("r9Weight") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("r9Weight", r9weight_);
	}

	//chain->Show(entryList->GetEntry(0));
	RooDataSet *data = new RooDataSet(chain->GetTitle(), "dataset", Vars);
	//  entryList->Print("all");
	Long64_t entries = entryList->GetN();
	//chain->LoadTree(0);
	//chain->GetEntry(0);
	std::cout << "___ ENTRIES: " << entries << std::endl;
	for(Long64_t jentry = 0; jentry < entries; jentry++) {
		//Int_t treenum=0;
		//Long64_t treeEntry = entryList->GetEntryAndTree(jentry,treenum);
		//Long64_t chainEntry = treeEntry+chain->GetTreeOffset()[treenum];
		//chain->LoadTree(chainEntry); // this also returns treeEntry
		//    chain->GetEntry(chainEntry);
		chain->GetEntry(chain->GetEntryNumber(jentry));
//     Long64_t ientry = entryList->GetEntry(jentry);
//     //std::cerr << "ientry = " << ientry << std::endl;
//     if(chain->LoadTree(ientry)<=0) exit(1);
//     if(chain->GetEntry(ientry)<=0) exit(1);
//     if( != ientry) exit(1);
		if(jentry < 1)  std::cout << "[DEBUG] PU: " << pileupWeight_
			                          << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] corrEle[0]: " << corrEle_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] corrEle[1]: " << corrEle_[1] << std::endl;

		if(jentry < 1)  std::cout << "[DEBUG] smearEle[0]: " << smearEle_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] smearEle[1]: " << smearEle_[1] << std::endl;

		if(jentry < 1)  std::cout << "[DEBUG] r9weight[0]: " << r9weight_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] r9weight[1]: " << r9weight_[1] << std::endl;

		invMass_ *= sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1]));
		invMass.setVal(invMass_ );
		weight.setVal(weight_ * pileupWeight_ * r9weight_[0]*r9weight_[1]);
		if(invMass_ > invMass.getMin() && invMass_ < invMass.getMax()) data->add(Vars);
	}
	data->Print();
	chain->ResetBranchAddresses();
	return data;
}

RooDataSet *ZFit_class::TreeToRooDataSet(TChain *chain, TCut cut)
{


	Float_t invMass_;

	Float_t weight_;
	weight_ = 1;
	Float_t r9weight_[2] = {1, 1}; //r9weight_[0]=1; r9weight_[1]=1;
	Float_t pileupWeight_ = 1;

	Float_t corrEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;
	Float_t smearEle_[2] = {1, 1}; //corrEle_[0]=1; corrEle_[1]=1;

	chain->SetBranchAddress(invMass.GetName(), &invMass_);

	if(chain->GetBranch("puWeight") != NULL) {
		std::cout << "[STATUS] Adding pileup weight branch from friend" << std::endl;
		chain->SetBranchAddress("puWeight", &pileupWeight_);
	}

	if(chain->GetBranch("scaleEle") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("scaleEle", corrEle_);
	}

	if(chain->GetBranch("smearEle") != NULL && TString(chain->GetTitle()) != "d") {
		std::cout << "[STATUS] Adding electron energy smearing branch from friend" << std::endl;
		//chain->Scan("smearEle");
		chain->SetBranchAddress("smearEle", smearEle_);
	}

	if(chain->GetBranch("r9Weight") != NULL) {
		std::cout << "[STATUS] Adding electron energy correction branch from friend" << std::endl;
		chain->SetBranchAddress("r9Weight", r9weight_);
	}

	RooDataSet *data = new RooDataSet(chain->GetTitle(), "dataset", Vars);

	Long64_t entries = chain->GetEntryList()->GetN();
	chain->LoadTree(chain->GetEntryNumber(0));
	Long64_t treenumber = -1;
	TTreeFormula *selector = new TTreeFormula("selector", cut, chain);

	std::cout << "___ ENTRIES: " << entries << std::endl;
	for(Long64_t jentry = 0; jentry < entries; jentry++) {
		Long64_t entryNumber = chain->GetEntryNumber(jentry);
		chain->GetEntry(entryNumber);
		if (chain->GetTreeNumber() != treenumber) {
			treenumber = chain->GetTreeNumber();
			selector->UpdateFormulaLeaves();
		}
		if(selector->EvalInstance() == false) continue;

		if(jentry < 1)  std::cout << "[DEBUG] PU: " << pileupWeight_
			                          << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] corrEle[0]: " << corrEle_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] corrEle[1]: " << corrEle_[1] << std::endl;

		if(jentry < 1)  std::cout << "[DEBUG] smearEle[0]: " << smearEle_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] smearEle[1]: " << smearEle_[1] << std::endl;

		if(jentry < 1)  std::cout << "[DEBUG] r9weight[0]: " << r9weight_[0] << std::endl;
		if(jentry < 1)  std::cout << "[DEBUG] r9weight[1]: " << r9weight_[1] << std::endl;

		invMass_ *= sqrt(corrEle_[0] * corrEle_[1] * (smearEle_[0]) * (smearEle_[1]));
		invMass.setVal(invMass_ );
		weight.setVal(weight_ * pileupWeight_ * r9weight_[0]*r9weight_[1]);
		if(invMass_ > invMass.getMin() && invMass_ < invMass.getMax()) data->add(Vars);
	}
	delete selector;
	data->Print();
	chain->ResetBranchAddresses();

	return data;
}

RooAbsData *ZFit_class::ReduceDataset(TChain *data, TString region, bool isMC, bool isUnbinned)
{

	std::cout << "------------------------------ REDUCE DATASETS: region = " << region << std::endl;
	if(region.Sizeof() > 1)  std::cout << " - reducing " << data->GetName() << " dataset to " << region
		                                   << std::endl;
	else std::cout << " - removing trakerDriven electrons" << std::endl;

	// reduce the tree to the selected events

	//std::cout << cutter.GetCut(region,isMC) << std::endl;
	TStopwatch myClock;
	myClock.Start();
	RooAbsData *reduced = TreeToRooDataSet(data, cutter.GetCut(region, isMC));
	myClock.Stop();
	if(!isUnbinned) { //if not an unbinned fit, transform the roodataset in histogram
		std::cout << "[INFO] Binned fit" << std::endl;
#ifdef DEBUG
		std::cout << "[DEBUG] Binned reduced dataset: nBins = " << invMass.getBins("plotRange") << "\tbin_min = " << invMass.getMin() << std::endl;
#endif
		TH1F histogram("dataset_his", "dataset_his", invMass.getBins("plotRange"), invMass.getMin(), invMass.getMax());

		histogram.Sumw2();
		Long64_t nevent = reduced->numEntries() ;
		for(Long64_t i = 0; i < nevent; ++i) {
			const RooArgSet* data = reduced->get(i);
			float invMass_val = ((const RooRealVar*) (data->find(invMass.GetName())))->getVal();
			float weight_val = ((const RooRealVar*) (data->find(weight.GetName())))->getVal();
			histogram.Fill(invMass_val, weight_val);
#ifdef DEBUG
			if(i < 1) std::cout << "invMass_val= " << invMass_val << "\t" << "weight_val= " << weight_val << "\t" << "sumW2=" << histogram.GetSumOfWeights()  << std::endl;
#endif
		}
#ifdef DEBUG
		std::cout << "bin 20 content= " << histogram.GetBinContent(20) << "\t" << "error= " << histogram.GetBinError(20) << std::endl;
#endif

		delete reduced; // unbinned reduced no more useful
		RooDataHist *dataset_hist = new RooDataHist("roodataset_hist", "roodataset_hist", invMass, &histogram);
		reduced = dataset_hist;
	} else {
		std::cout << "[INFO] Unbinned fit" << std::endl;
	}
	reduced->Print();

	std::cout << "------------------------------" << std::endl;


	return reduced;

}


void ZFit_class::SetPDF_signal(int pdf_index)
{

	signal_pdf_type = pdf_index;
	// va fatta la scelta tra cruijff e convolution
	switch(pdf_index) {
	case 0:
		std::cout << "[SETTING SIGNAL PDF]: BW and CB convolution" << std::endl;
		signal_pdf = &(convBwCbPdf.GetPdf());
		params = (convBwCbPdf.GetParams());
		break;
	case 1:
		std::cout << "[SETTING SIGNAL PDF]: Cruijff pdf" << std::endl;
		signal_pdf = &(cruijffPdf.GetPdf());
		params = (cruijffPdf.GetParams());
		break;
	default:
		std::cerr << "[WARNING] signal pdf index not defined, using default" << std::endl;
		std::cout << "[SETTING SIGNAL PDF]: BW and CB convolution" << std::endl;
		signal_pdf = &(convBwCbPdf.GetPdf());
		params = (convBwCbPdf.GetParams());
		break;
	}

	return;
}

void ZFit_class::SetPDF_model(int signal_pdf_index, int bkg_pdf_index)
{

	SetPDF_signal(signal_pdf_index);


	// to be put in a separate method setting only the bkg pdf
	bkg_pdf_type = bkg_pdf_index;

	if(bkg_pdf_index == 0) {
		// we use only signal hypothesis
		std::cout << "[SETTING] model = only signal" << std::endl;
		model_pdf = signal_pdf;
	} else {
		std::cerr << "[WARNING] pdf model not defined!!!! Still to be implemented" << std::endl;
		std::cerr << "          Using only signal hypothesis instead" << std::endl;
		model_pdf = signal_pdf;
	}
	return;
}


/**
 * if fitres_MC != NULL
 * take the fit parameters from that and set fit_type for the pdf to fit_type,
 * put fit_type=0 otherwise
 */
void ZFit_class::SetFitPar(RooFitResult *fitres_MC)
{

	if(fitres_MC != NULL) {
		switch(signal_pdf_type) { // this is not elegant....
		case 0:
			convBwCbPdf.SetFitType(_fit_type);
		case 1:
			cruijffPdf.SetFitType(_fit_type);
		}

		RooArgList argList = fitres_MC->floatParsFinal();
		for(int i = 0; i < argList.getSize(); i++) { // copy fitresults to
			RooRealVar *listVar = (RooRealVar *) &argList[i];
			RooRealVar *var;
			var =  (RooRealVar *)  & ((*params)[listVar->GetName()]);
			var->setVal(listVar->getVal()); // unuseful the pdf is the same, it has the previous value by default
		}
	} else {
		switch(signal_pdf_type) { // this is not elegant....
		case 0:
			convBwCbPdf.SetFitType(0);
			convBwCbPdf.InitFitValues();
		case 1:
			cruijffPdf.SetFitType(0);
			cruijffPdf.InitFitValues();
		}
	}

	return;
}

//get effective sigma
double ZFit_class::GetEffectiveSigma(RooAbsData *dataset, float quant = 0.68)
{

	// it's up to the function that calls GetEffectiveSigma to delete the stored histogram
//	if(invMass_highBinning==NULL) invMass_highBinning = dataset->createHistogram(invMass.GetName(),invMass.getBins("plotRange"));
	if(invMass_highBinning == NULL) invMass_highBinning = dataset->createHistogram(invMass.GetName(), 6000);

	TH1* h = invMass_highBinning;

	double TotEvents = h->Integral(1, h->GetNbinsX() - 1);
	double LocEvents = 0.;
	int binI = h->FindBin(h->GetMean());
	int binF = h->GetNbinsX() - 1;
// bool keepGoing = false;
#ifdef DEBUG
	double frac = 0.;
#endif
	for(int jBin = binI; jBin > 0; --jBin) {
		LocEvents = 0.;
		// keepGoing = false;
#ifdef DEBUG
		std::cout << "j: " << h->GetBinCenter(jBin) << " I: " << h->GetBinCenter(binI) << " F: " << h->GetBinCenter(binF) << " frac: " << frac << std::endl;
#endif
		for(int iBin = jBin; iBin < binF; ++iBin) {
			LocEvents += h->GetBinContent(iBin);
			if(LocEvents / TotEvents >= quant) {
				if(iBin - jBin < binF - binI) {
					binF = iBin;
					binI = jBin;
					//  keepGoing = true;
				}
				break;
			}
			if(iBin == binF - 1 && binF == h->GetNbinsX() - 1) {
				//keepGoing = true;
#ifdef DEBUG
				frac = LocEvents / TotEvents;
				std::cout << "trovato nuovo intervallo piu piccolo di prima" << std::endl;
#endif
				--binI;
			}
			//    if(iBin == binF-1 && binF != h->GetNbinsX()-1) break;
			if(iBin == binF - 1 && binF != h->GetNbinsX() - 1) {
#ifdef DEBUG
				std::cout << "qui: esci" << std::endl;
#endif
				//	keepGoing=true;
				break;
			}
		}
		// if(keepGoing == false) break;
	}
	double sigma = (h->GetBinCenter(binF) - h->GetBinCenter(binI)) / 2.;
	//std::cout << " >>> effective sigma: " << sigma << std::endl;
	std::cout << ">>> - effective sigma: " << sigma << " interval start from: " << h->GetBinCenter(binI) << " finish to: " << h->GetBinCenter(binF) << std::endl;
	return sigma;
}


void ZFit_class::Fit(TH1F *hist, bool isMC)
{
	RooAbsData *data_red = NULL;

	data_red = ImportHist(hist);

	//  SetFitPar();

	//RooFitResult *fitres =
	model_pdf->fitTo(*data_red, RooFit::Save(), //RooFit::Range(range.c_str()),
	                 RooFit::NumCPU(1),
#ifdef FIT_DEBUG
	                 RooFit::Verbose(kTRUE), RooFit::PrintLevel(3),
	                 RooFit::Warnings(kTRUE), RooFit::PrintEvalErrors(kTRUE),
#else
	                 RooFit::Verbose(kFALSE), RooFit::PrintLevel(-1),
	                 RooFit::Warnings(kFALSE), RooFit::PrintEvalErrors(kFALSE),
#endif
	                 RooFit::SumW2Error(kTRUE)
	                );

#ifdef DEBUG
	//fitres -> Print();
#endif

	// si puo' fare in moodo piu' intelligente?
	// mettere il retrieve della larghezza e dello shift del picco dipendente dal tipo di pdf segnale

	std::pair<double, double> deltaM;
	std::pair<double, double> width;

	switch(signal_pdf_type) {
	case 0:
		deltaM = convBwCbPdf.GetDeltaM();
		width  = convBwCbPdf.GetWidth();
		break;
	case 1:
		deltaM = cruijffPdf.GetDeltaM();
		width  = cruijffPdf.GetWidth();
		break;
	}

#ifdef DEBUG
	std::cout << "deltaM = " << deltaM.first << " \\pm " << deltaM.second << std::endl;
	std::cout << "width = "  << width.first  << " \\pm " << width.second  << std::endl;
#endif

	if(isMC) {
		deltaM_MC_map.push_back(deltaM);
		width_MC_map.push_back(width);
	} else {
		deltaM_data_map.push_back(deltaM);
		width_data_map.push_back(width);
	}



	PlotFit(data_red, isMC);
	return;
}


RooFitResult *ZFit_class::FitData(TString region, bool doPlot, RooFitResult *fitres_MC)
{
	RooAbsData *data_red   = ReduceDataset(data, region, false, _isDataUnbinned);
	nEvents_region_data = data_red->sumEntries();
	if(nEvents_region_data < 100) {
		data_red   = ReduceDataset(data, region, false, true);
//   nEvents_region_data=data_red->sumEntries();
	}
// if(nEvents_region_data < 100) return NULL;
	int numcpu = 4;
	if(_isDataUnbinned) numcpu = 1; //this is because in previous versions of ROOT, the unbinned fit did not support nCPU>1 (to be checked in newer versions)

	//EFFECTIVE SIGMA
	sigmaeff_data = GetEffectiveSigma(data_red);
	//overwriting values
	sigmaeff_data_map[0.68] = GetEffectiveSigma(data_red);
	sigmaeff_data_map[0.50] = GetEffectiveSigma(data_red, 0.50);
	sigmaeff_data_map[0.30] = GetEffectiveSigma(data_red, 0.30);

	SetFitPar(fitres_MC);
	RooFitResult *fitres_data = model_pdf->fitTo(*data_red, RooFit::Save(), //RooFit::Range(range.c_str()),
	                            RooFit::NumCPU(numcpu),
	                            RooFit::Verbose(kFALSE), RooFit::PrintLevel(-1),
	                            RooFit::Warnings(kFALSE), RooFit::PrintEvalErrors(kFALSE),
	                            RooFit::SumW2Error(_isDataSumW2)
	                                            );
	fitres_data->Print("V");

	// salvo in una mappa i valori del fit // replace all this asking the Width and peak from the pdfs!!!
	//deltaM_data_map.push_back(convBwCbPdf.GetDeltaM(fitres_data));
	//width_data_map.push_back(convBwCbPdf.GetWidth(fitres_data));

	//make sure the params are the same as the fit result
	SetFitPar(fitres_data);

	// add the rescaled width map

	if(doPlot) {
		PlotFit(data_red, false);
		plot_data->Print();
		//chi2_data = (model_pdf->createChi2(*data_red))->getValue();
		chi2_data = plot_data->chiSquare();//invMass.getBins("plotRange")-fitres_data->floatParsFinal().getSize());
	}
	delete data_red;
	delete invMass_highBinning;
	invMass_highBinning = NULL;
	return fitres_data;
}

RooFitResult *ZFit_class::FitMC(TString region, bool doPlot)
{
	RooAbsData *signal_red = ReduceDataset(signal, region, true, _isMCUnbinned);
	nEvents_region_MC = signal_red->sumEntries();
	if( nEvents_region_MC < 30) return NULL;
	int numcpu = 4;
	if(!_isMCUnbinned) numcpu = 2; // would prefer 1, but bug in RooFit: wrong error estimation for weighted datahist if 1 cpu

	//EFFECTIVE SIGMA
	sigmaeff_MC = GetEffectiveSigma(signal_red);

	//overwriting values
	sigmaeff_MC_map[0.68] = GetEffectiveSigma(signal_red);
	sigmaeff_MC_map[0.50] = GetEffectiveSigma(signal_red, 0.50);
	sigmaeff_MC_map[0.30] = GetEffectiveSigma(signal_red, 0.30);
	//std::cout<<sigmaeff<<std::endl;

	SetFitPar();
	RooFitResult *fitres_MC = model_pdf->fitTo(*signal_red, RooFit::Save(), //RooFit::Range(range.c_str()),
	                          RooFit::NumCPU(numcpu),
	                          RooFit::Verbose(kFALSE), RooFit::PrintLevel(-1),
	                          RooFit::Warnings(kFALSE), RooFit::PrintEvalErrors(kFALSE),
	                          RooFit::SumW2Error(_isMCSumW2)
	                                          );

	fitres_MC->Print("V");

	// salvo in una mappa i valori del fit

	//deltaM_MC_map.push_back(convBwCbPdf.GetDeltaM(fitres_MC));
	//  width_MC_map.push_back(convBwCbPdf.GetWidth(fitres_MC));

	//make sure the params are the same as the fit result
	SetFitPar(fitres_MC);


	if(doPlot) {
		PlotFit(signal_red, true);
		chi2_MC = plot_MC->chiSquare(); //FIXME
	}

	delete signal_red; // delete the reduced dataset
	delete invMass_highBinning;
	invMass_highBinning = NULL;
	return fitres_MC;
}



void ZFit_class::Fit(TString region, bool doPlot)
{

	std::cout << "============================== ";
	std::cout << "[STATUS] Fitting region: " << region << std::endl;
	RooFitResult *fitres_MC = NULL;

	// for the MC skip the runNumber
	TString regionMC = region;
	int p = regionMC.Index("-runNumber");
	int pp = regionMC.Index("-", p + 1);

	if(p > 0) { //there is the runNumber string
		TString region_tmp = region;
		region_tmp.Remove(0, p);  // remove the part before
		if(pp > 0) region_tmp.Remove(pp - p); // remove the part after if exists
		region_tmp.ReplaceAll("-runNumber_", ""); // remove runNumber string
		TString region_tmp2 = region_tmp;
		region_tmp.Remove(region_tmp.First('_')); // take the runMin
		region_tmp2.Remove(0, region_tmp2.First('_') + 1); //take the runMax
		int runMin = region_tmp.Atoi();
		int runMax = region_tmp2.Atoi();
		std::cout << region << "\t" << p << " " << pp << "\ttmp " << region_tmp << "\ttmp2 " << region_tmp2 << "\trunMin " << runMin << "\t" << runMax << std::endl;
		if(pp <= 0) regionMC.Remove(p);
		else regionMC.Remove(p, pp - p);
		// cuts for runDependentMC!
		if(runMin >= 190456 && runMax <= 196531) regionMC.Insert(p, "-runNumber_194533_194533");
		if(runMin >= 198111 && runMax <= 203742) regionMC.Insert(p, "-runNumber_200519_200519");
		if(runMin >= 203756 && runMax <= 208686) regionMC.Insert(p, "-runNumber_206859_206859");
	}

	TString paramsMCFileName = outDirFitResMC + "/" + regionMC + ".txt";
	TString fitResMCFileName = outDirFitResMC + "/" + regionMC + ".root";
	TString plotMCFileName = outDirImgMC + "/" + regionMC + "." + imgFormat;

	TString paramsDataFileName = outDirFitResData + "/" + region + ".txt";
	TString fitResDataFileName = outDirFitResData + "/" + region + ".root";
	TString plotDataFileName = outDirImgData + "/" + region + "." + imgFormat;

	bool newFitMC = false;
	if(_forceNewFit || gSystem->AccessPathName(fitResMCFileName)) { // if the file does not exist
		newFitMC = true;
		fitres_MC = FitMC(regionMC, doPlot);
		if(fitres_MC != NULL) {
			fitres_MC->SetName("MC");
			SaveFitRes(fitres_MC, fitResMCFileName, chi2_MC, nEvents_region_MC, sigmaeff_MC_map); //
			params->writeToFile(paramsMCFileName);
			if(doPlot) SaveFitPlot(plotMCFileName, true);
		} else {
			std::cerr << "[WARNING] MC fit for region: " << region << " not possible because nEvents < 100" << std::endl;
			std::cout << "[WARNING] MC fit for region: " << region << " not possible because nEvents < 100" << std::endl;
			return;
		}
	} else {
		newFitMC = false;
		// take the fit result from the file
		std::cout << "[INFO] Fit results for MC taken from file:" << fitResMCFileName << std::endl;
		TFile fitResMCFile(fitResMCFileName, "OPEN");
		if (((RooFitResult *)fitResMCFile.Get("data"))) {
			fitres_MC = (RooFitResult *) ((RooFitResult *)fitResMCFile.Get("data"))->Clone("MC_fitres");
		} else if((RooFitResult *)fitResMCFile.Get("MC")) {
			fitres_MC = (RooFitResult *) ((RooFitResult *)fitResMCFile.Get("MC"))->Clone("MC_fitres");
		} else {
			std::cout << "ERROR specified fit res directory contains no fitres. exit " ;
			exit(1);
		}
		fitResMCFile.Close();
		fitres_MC->Print();

	}


	if(_updateOnly == false || newFitMC || gSystem->AccessPathName(fitResDataFileName)) {
		RooFitResult *fitres_data = FitData(region, doPlot, fitres_MC);
		if(fitres_data != NULL) {
			fitres_data->SetName("data");
			params->writeToFile(paramsDataFileName);
			SaveFitRes(fitres_data, fitResDataFileName, chi2_data, nEvents_region_data, sigmaeff_data_map); // isMC=false
			if(doPlot) SaveFitPlot(plotDataFileName, false);
			delete fitres_data;
		} else {
			if(nEvents_region_data < 100) {
				std::cerr << "[WARNING] Data fit for region: " << region << " not possible because nEvents < 100" << std::endl;
				std::cout << "[WARNING] Data fit for region: " << region << " not possible because nEvents < 100" << std::endl;
			} else {
				std::cerr << "[WARNING] Data fit for region: " << region << " not possible" << std::endl;
				std::cout << "[WARNING] Data fit for region: " << region << " not possible" << std::endl;
			}
		}
	}

	//devo salvare i risultati deltaM e width


	delete fitres_MC;
	return;

}


void ZFit_class::FitToy(TString region, int nToys, int nEvents, bool doPlot)
{
	RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING) ;
	std::cout << "============================== ";
	std::cout << "[STATUS] Toy Fitting region: " << region << std::endl;

	TTree* toytree = InitTree();


	BW_CB_pdf_class tempconvBwCbPdf(invMass);

	RooAbsPdf *generator_pdf = &(tempconvBwCbPdf.GetPdf()); //with original initial values

	//	  cout <<"before reading"<<endl;
	//	  (tempconvBwCbPdf.params).Print("v");

	//	  cout <<"name of init file: "<<_initFileNameMC<<endl;
	(tempconvBwCbPdf.params).readFromFile(_initFileNameMC, 0, 0, kTRUE);
	//	  (convBwCbPdf.params).Print("v");

	params = tempconvBwCbPdf.GetParams();
	//  cout <<" before set init: gamma"<<((RooRealVar *)params->find("\\gamma"))->getVal()<<endl;
	//cout <<" before set init: dm "<<((RooRealVar *)params->find("\\Delta m"))->getVal()<<endl;



	SetInitParamsfromRead(params);

	RooAbsPdf* fit_pdf = &(convBwCbPdf.GetPdf()); //generator pdf

	/*RooPlot* plot = invMass.frame();
	  fit_pdf->plotOn(plot, RooFit::LineColor(kRed));
	  fit_pdf->paramOn(plot);
	  generator_pdf->plotOn(plot, RooFit::LineColor(kBlue));
	  generator_pdf->paramOn(plot);
	  TCanvas c("canvas","");
	  c.cd();
	  plot->Draw();
	  c.SaveAs("toy/pdf_"+region+".png");
	*/
	TFile* f = new TFile("toy/" + region + "_toy.root", "RECREATE");

	if (_isToyUnbinned) {
		int numcpu = 4;
		for (int i = 0; i < nToys; ++i) {
			RooDataSet* toydata = generator_pdf->generate(invMass, nEvents);
			SetFitPar();
			RooFitResult *fitres_Toy = fit_pdf->fitTo(*toydata, RooFit::Save(), //RooFit::Range(range.c_str()),
			                           RooFit::NumCPU(numcpu),
			                           RooFit::Verbose(kFALSE), RooFit::PrintLevel(-1),
			                           RooFit::Warnings(kFALSE), RooFit::PrintEvalErrors(kFALSE),
			                           RooFit::SumW2Error(_isToySumW2)
			                                         );
			toy_deltaM = ((RooRealVar *) (fitres_Toy->floatParsFinal()).find("\\Delta m"))->getVal();
			toy_width = ((RooRealVar *) (fitres_Toy->floatParsFinal()).find("\\sigma_{CB}"))->getVal();
			toy_gamma =  ((RooRealVar *) (fitres_Toy->floatParsFinal()).find("\\gamma"))->getVal();
			toy_alpha = ((RooRealVar*) (fitres_Toy->floatParsFinal()).find("\\alpha"))->getVal();
			toytree->Fill();
			//		delete toydata;
			//		delete fitres_Toy;
		}
	} else {
		std::cout << "Using binned for toy MC generator" << std::endl;
		int numcpu = 2;
		for (int i = 0; i < nToys; ++i) {
			RooDataHist* toydata = generator_pdf->generateBinned(invMass, nEvents);
			SetFitPar();

			//std::cout <<"init gamma: "<<init_gamma<<std::endl;
			RooFitResult *fitres_Toy = fit_pdf->fitTo(*toydata, RooFit::Save(), //RooFit::Range(range.c_str()),
			                           RooFit::NumCPU(numcpu),
			                           RooFit::Verbose(kFALSE), RooFit::PrintLevel(-1),
			                           RooFit::Warnings(kFALSE), RooFit::PrintEvalErrors(kFALSE),
			                           RooFit::SumW2Error(_isToySumW2),
			                           RooFit::PrintLevel(-1)
			                                         );
			//[XH]
			//cout <<"[XH] iToys "<<i<<endl;
			//fitres_Toy->Print("V");
			//cout <<"[XH] fit result status: "<<fitres_Toy->status()<<endl;
			toy_deltaM = ((RooRealVar *) (fitres_Toy->floatParsFinal()).find("\\Delta m"))->getVal();
			toy_width = ((RooRealVar *) (fitres_Toy->floatParsFinal()).find("\\sigma_{CB}"))->getVal();
			toy_gamma =  ((RooRealVar *) (fitres_Toy->floatParsFinal()).find("\\gamma"))->getVal();
			toy_alpha = ((RooRealVar*) (fitres_Toy->floatParsFinal()).find("\\alpha"))->getVal();
			//	  cout <<"gamma: "<<toy_gamma<<endl;
			toytree->Fill();
			//RooPlot* finalplot = invMass.frame();
			/*  if (nToys ==2){
			toydata->plotOn(finalplot, RooFit::DrawOption("B"), RooFit::FillColor(kGray));
			generator_pdf->plotOn(finalplot, RooFit::LineColor(kBlue));
			generator_pdf->paramOn(finalplot);
			fit_pdf->plotOn(finalplot, RooFit::LineColor(kRed));
			fit_pdf->paramOn(finalplot);

			TCanvas c1("canvas","");
			c1.cd();
			finalplot->Draw();
			c1.SaveAs("toy/result.png");
			}
			*/
			//		delete toydata;
			//		delete fitres_Toy;
		}
	}
	toytree->Write();
	f->Close();
	//delete toytree;
	//delete f;
	return;
}

void ZFit_class::SaveFitRes(RooFitResult *fitres, TString fileName, float chi2, double nEvents, std::map<float, float> sigmaeff )
{
	RooCmdArg LatexFormat(RooFit::Format("NEU", RooFit::AutoPrecision(2), RooFit::VerbatimName(kFALSE)));

	TFile fitResFile(fileName, "RECREATE");
	fitResFile.cd();
	fitres->Write();
	fitResFile.Write();
	fitResFile.Close();
	fitres->floatParsFinal().printLatex(LatexFormat, RooFit::OutputFile(fileName.ReplaceAll(".root", ".tex")));
	std::ofstream f(fileName, std::ios_base::app);
	f << "nEvents=" << nEvents << std::endl;
	f << "chi2=" << chi2 << std::endl;
	f << "sigeff=" << sigmaeff[0.68] << std::endl;
	f << "sigeff30=" << sigmaeff[0.30] << std::endl;
	f << "sigeff50=" << sigmaeff[0.50] << std::endl;
	f << "sigeff68=" << sigmaeff[0.68] << std::endl;

	f.close();
	return;
}

void ZFit_class::PrintScale(std::ostream outScale)
{
	// std::map<TString, std::pair<double,double> > deltaM_map;
	//          region ,          val    , err

	for(unsigned int i = 0; i < deltaM_data_map.size(); i++) {
		//    TString region=itr->first;
		//    double deltaG=(deltaM_data_map[region].first - deltaM_MC_map[region].first)/mRes.getVal() * 100;
		//    double deltaG_err = (deltaM_data_map[region].second*deltaM_data_map[region].second +
		//			 deltaM_MC_map[region].first*deltaM_MC_map[region].first)/mRes.getVal() * 100;
		outScale << region_map[i] << "\t"
		         << deltaM_data_map[i].first << "\t" << deltaM_data_map[i].second << "\t"
		         << deltaM_MC_map[i].first << "\t" << deltaM_MC_map[i].second << "\t"
		         //     << deltaG  << "\t" << deltaG_err << "\t"
		         << std::endl;

	}
	return;
}


void ZFit_class::PrintRes(std::ostream outRes)
{
	//   for(std::map<TString, std::pair<double, double> >::const_iterator itr=width_data_map.begin();
	//       itr!=width_data_map.end(); itr++){
	//     TString region=itr->first;
	//     outRes << region << "\t"
	// 	   << itr->second.first << "\t" << itr->second.second
	// 	   << std::endl;
	//   }

	for(unsigned int i = 0; i < deltaM_data_map.size(); i++) {
		//    TString region=itr->first;
		//    double deltaG=(deltaM_data_map[region].first - deltaM_MC_map[region].first)/mRes.getVal() * 100;
		//    double deltaG_err = (deltaM_data_map[region].second*deltaM_data_map[region].second +
		//			 deltaM_MC_map[region].first*deltaM_MC_map[region].first)/mRes.getVal() * 100;
		outRes << region_map[i] << "\t"
		       << width_data_map[i].first << "\t" << width_data_map[i].second << "\t"
		       //	   << deltaM_MC_map[i].first << "\t" << deltaM_MC_map[i].second << "\t"
		       //     << deltaG  << "\t" << deltaG_err << "\t"
		       << std::endl;

	}

	return;
}


void ZFit_class::PlotFit(RooAbsData *data_red, bool isMC)
{

	if(isMC) {
		if(plot_MC != NULL) delete plot_MC;
		plot_MC = invMass.frame(//RooFit::Range(invMass_min,invMass_max),
		              RooFit::Title(""),
		              RooFit::Name("MC_plot"));

		data_red->plotOn(plot_MC,
		                 RooFit::DrawOption("B"),
		                 RooFit::DataError(RooAbsData::SumW2), // RooFit::DataError(RooAbsData::None),
		                 // RooFit::XErrorSize(0), //RooFit::YErrorSize(0)
		                 RooFit::FillColor(kGray),
		                 RooFit::Binning("plotRange"));
		model_pdf->plotOn(plot_MC, RooFit::LineColor(kRed));
		model_pdf->paramOn(plot_MC, RooFit::Layout(0.62, 0.98, 0.9));
		TAttText *text_MC = plot_MC->getAttText();
		text_MC->SetTextSize(0.03);
		plot_MC->SetTitleOffset(0.9, "y");
	} else {
		if(plot_data != NULL) delete plot_data;
		plot_data = invMass.frame(//RooFit::Range(invMass_min,invMass_max),
		                RooFit::Title(""),
		                RooFit::Name("data_plot"));

		data_red->plotOn(plot_data, RooFit::Binning("plotRange"));
		model_pdf->plotOn(plot_data, RooFit::LineColor(kBlue));
		//  model_pdf->plotOn(plot_data, RooFit::LineColor(kRed), RooFit::Components(bkg),RooFit::LineStyle(kDashed));
		model_pdf->paramOn(plot_data, RooFit::Layout(0.62, 0.98, 0.9));
		TAttText *text_data = plot_data->getAttText();
		text_data->SetTextSize(0.03);
	}

	return;
}

void ZFit_class::PlotFit(RooAbsData *signal_red, RooAbsData *data_red)
{
	//  SetTDRStyle();
	if(plot_MC != NULL) delete plot_MC;
	if(plot_data != NULL) delete plot_data;

	plot_MC = invMass.frame(//RooFit::Range(invMass_min,invMass_max),
	              //RooFit::Title("MC"),
	              RooFit::Name("MC_plot"));

	plot_data = invMass.frame(//RooFit::Range(invMass_min,invMass_max),
	                // RooFit::Title("Data"),
	                RooFit::Name("data_plot"));//(data_plot->GetName()+plot_name).c_str()));

	// plot del MC
	signal_red->plotOn(plot_MC,
	                   RooFit::DrawOption("B"),
	                   RooFit::DataError(RooAbsData::None),
	                   RooFit::XErrorSize(0), //RooFit::YErrorSize(0)
	                   RooFit::FillColor(kGray),
	                   RooFit::Binning("plotRange"));
	model_pdf->plotOn(plot_MC, RooFit::LineColor(kRed));
	model_pdf->paramOn(plot_MC, RooFit::Layout(0.62, 0.98, 0.9));
	TAttText *text_MC = plot_MC->getAttText();
	text_MC->SetTextSize(0.03);
	plot_MC->SetTitleOffset(0.9, "y");

	// plot dei dati
	data_red->plotOn(plot_data, RooFit::Binning("plotRange"));
	model_pdf->plotOn(plot_data, RooFit::LineColor(kBlue));
	//  model_pdf->plotOn(plot_data, RooFit::LineColor(kRed), RooFit::Components(bkg),RooFit::LineStyle(kDashed));
	model_pdf->paramOn(plot_data, RooFit::Layout(0.62, 0.98, 0.9));
	TAttText *text_data = plot_data->getAttText();
	text_data->SetTextSize(0.03);

	return;
}

void ZFit_class::SaveFitPlot(TString fileName, bool isMC)
{


	//  if(region.Sizeof() <=1) region="all";
	TString canvasName = isMC ? "c_MC" : "c_data";
	TCanvas c(canvasName, "");

	c.cd();
	if(isMC) plot_MC->Draw();
	else plot_data->Draw();
	c.SaveAs(fileName);
	c.SaveAs(fileName.ReplaceAll(imgFormat, "C"));

	return;
}

// modificare per avere in return un pair con valore e errore
double ZFit_class::GetLastDeltaM(bool isMC)
{
	if(isMC) return deltaM_MC_map.back().first;
	else return deltaM_data_map.back().first;
}

double ZFit_class::GetLastWidth(bool isMC)
{
	if(isMC) return width_MC_map.back().first;
	else return width_data_map.back().first;
}



#ifdef shervin
void ZFit_class::PlotInvMass(TString region)
{

#ifdef DEBUG
	std::cout << "------------------------------ Plotting invariant mass histograms" << std::endl;
#endif

	double nMC = 0; //, nDATA=0;
	//  TString region="EB";
	THStack stack("stack", "");
	TLegend legend(0.8, 0.8, 1, 1, "", "NDC");
	legend.SetFillColor(0);
	legend.SetBorderSize(1);

	/* creo la mappa con gli istogrammi per ogni voce di legenda
	 * std::map< legend, TH1F *>
	 */

	std::cout << "--------------- Plotting data" << std::endl;
	TH1F *data_hist = NULL;
	hist_map_t data_hist_map = MakeHistMap(data_chain_map, region);
	if(data_hist_map.empty()) return;

	std::cout << "--------------- Plotting signal" << std::endl;
	hist_map_t signal_hist_map = MakeHistMap(signal_chain_map, region);
	if(signal_hist_map.empty()) return;

	std::cout << "--------------- Plotting bkg chain" << std::endl;
	hist_map_t bkg_hist_map = MakeHistMap(bkg_chain_map, region);
	if(bkg_hist_map.empty()) return;


	// ciclo per avere il numero di eventi MC e dati
	for(hist_map_t::const_iterator hist_itr = bkg_hist_map.begin();
	        hist_itr != bkg_hist_map.end();
	        hist_itr++) {
		// conto il numero di eventi MC del fondo
#ifdef DEBUG
		std::cout << hist_itr->first << "\t" << hist_itr->second->Integral() << std::endl;
#endif
		nMC += hist_itr->second->Integral();
	}


	for(hist_map_t::const_iterator hist_itr = signal_hist_map.begin();
	        hist_itr != signal_hist_map.end();
	        hist_itr++) {
		// conto il numero di eventi MC del fondo
#ifdef DEBUG
		std::cout << hist_itr->first << "\t" << hist_itr->second->Integral() << std::endl;
#endif
		nMC += hist_itr->second->Integral();

	}



	for(hist_map_t::const_iterator hist_itr = data_hist_map.begin();
	        hist_itr != data_hist_map.end();
	        hist_itr++) {
		// conto il numero di eventi MC del fondo
#ifdef DEBUG
		std::cout << hist_itr->first << "\t" << hist_itr->second->Integral() << std::endl;
#endif
		// aggiungo la voce alla legenda
		//legend.AddEntry(hist_itr->second, hist_itr->first, "p");
		data_hist = hist_itr->second;
	}

#ifdef DEBUG
	std::cout << "nMC = " << nMC << std::endl;
#endif


	for(hist_map_t::const_iterator hist_itr = bkg_hist_map.begin();
	        hist_itr != bkg_hist_map.end();
	        hist_itr++) {
		hist_itr->second->Scale(data_hist->Integral() / nMC);
		// aggiungo la voce alla legenda
		legend.AddEntry(hist_itr->second, hist_itr->first, "F");

		hist_itr->second->SetFillColor(3);
		// aggiungo l'istogramma allo stack
		stack.Add(hist_itr->second);
	}



	TString xLabel = "M_{ee} GeV/c^{2}";
	TString yLabel = "Events/ ";
	yLabel += invMass.getBinning("plotRange").binWidth(1);
	yLabel += "GeV/c^{2}";

	TCanvas c("c", "c");
	stack.Draw();
	stack.GetXaxis()->SetTitle(xLabel);
	stack.GetYaxis()->SetTitle(yLabel);
	legend.Draw();

	data_hist->SetMarkerSize(1);
	data_hist->SetMarkerStyle(20);
	data_hist->Draw("pesame");

	c.SaveAs(GetDir() + "/invMass/" + region + "_linear.eps");
	c.SaveAs(GetDir() + "/invMass/" + region + "_linear.C");
	c.SetLogy();
	c.SaveAs(GetDir() + "/invMass/" + region + "_log.eps");
	c.SaveAs(GetDir() + "/invMass/" + region + "_log.C");
	//creo un istogramma per ogni voce di legenda
	// devo prendere quindi la lista delle voci di legenda
	// normalizzo l'istogramma ad 1 pb-1 (automatico con il weight della chain)
	// costruisco lo stack
	// 165133
	return;
}

#endif





float ZFit_class::GetRescaledWidthError(float width, float width_error, float peakShift, float peakShiftError)
{

	float b = (91.187 + peakShift);
	float b2 = b * b;


	return 100 / b * sqrt(width_error * width_error + width * width * peakShiftError * peakShiftError / b2);
	// in %
}



//implement method for getting the corresponding energy name from invMass_name
TString	ZFit_class::GetEnergyVarName(TString invMass_name)
{
	TString energyBranchName = "";
	TString invMass_var = invMass_name;
	if(invMass_var == "invMass_SC_regrCorr_ele") energyBranchName = "energySCEle_regrCorr_ele";
	else if(invMass_var == "invMass_SC_regrCorr_pho") energyBranchName = "energySCEle_regrCorr_pho";
	else if(invMass_var == "invMass_regrCorr_fra") energyBranchName = "energyEle_regrCorr_fra";
	else if(invMass_var == "invMass_regrCorr_egamma") energyBranchName = "energyEle_regrCorr_egamma";
	else if(invMass_var == "invMass_rawSC") energyBranchName = "rawEnergySCEle";
	else if(invMass_var == "invMass_rawSC_esSC") energyBranchName = "rawEnergySCEle+esEnergySCEle";
	else if(invMass_var == "invMass_SC") energyBranchName = "energySCEle";
	else if(invMass_var == "invMass_SC_corr") energyBranchName = "energySCEle_corr";
	else if(invMass_var == "invMass_SC_must") energyBranchName = "energySCEle_must";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV4_ele") energyBranchName = "energySCEle_regrCorrSemiParV4_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV4_pho") energyBranchName = "energySCEle_regrCorrSemiParV4_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV5_ele") energyBranchName = "energySCEle_regrCorrSemiParV5_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV5_pho") energyBranchName = "energySCEle_regrCorrSemiParV5_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV6_ele") energyBranchName = "energySCEle_regrCorrSemiParV6_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV6_pho") energyBranchName = "energySCEle_regrCorrSemiParV6_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV7_ele") energyBranchName = "energySCEle_regrCorrSemiParV7_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV7_pho") energyBranchName = "energySCEle_regrCorrSemiParV7_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV8_ele") energyBranchName = "energySCEle_regrCorrSemiParV8_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV8_pho") energyBranchName = "energySCEle_regrCorrSemiParV8_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV6_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV6_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV6_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV6_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV7_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV7_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV7_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV7_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV8_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV8_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV8_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV8_pho";
	else if(invMass_var == "invMass_SC_must_regrCorr_ele") energyBranchName = "energySCEle_must_regrCorr_ele";
	return energyBranchName;
}



TTree* ZFit_class::InitTree()
{
	TTree* tree = new TTree("toy", "tree");
	tree->Branch("deltaM_fit", &toy_deltaM, "toy_deltaM/F");
	tree->Branch("width_fit", &toy_width, "toy_width/F");
	tree->Branch("gamma_fit", &toy_gamma, "toy_gamma/F");
	tree->Branch("alpha_fit", &toy_alpha, "toy_alpha/F");
	tree->Branch("deltaM_init", &init_deltaM, "init_deltaM/F");
	tree->Branch("width_init", &init_width, "init_width/F");
	tree->Branch("gamma_init", &init_gamma, "init_gamma/F");
	tree->Branch("alpha_init", &init_alpha, "init_alpha/F");
	return tree;
}


void ZFit_class::SetInitParamsfromRead(RooArgSet* pars)
{
	init_deltaM = ((RooRealVar *)params->find("\\Delta m"))->getVal();
	init_width = ((RooRealVar *)params->find("\\sigma_{CB}"))->getVal();
	init_gamma = ((RooRealVar *)params->find("\\gamma"))->getVal();
	//	  cout <<"init gamma inside set init parms: "<<init_gamma<<endl;
	init_alpha = ((RooRealVar *)params->find("\\alpha"))->getVal();
}

























/*
  std::map<TString, float> ZFit_class::ReadInitValuesFromFile(TString fileName){
  ifstream file(fileName);
  std::map<TString, float> values;
  TString region;
  TString name;
  float value;
  while(file.peek()!=EOF && file.good()){
  file >> name>>value;
  #ifdef DEBUG
  std::cout << "[DEBUG] Reading region: " << region<< std::endl;
  #endif
  values.insert(pair<TString,float>(name, value));
  }
  return values;
  }
*/
