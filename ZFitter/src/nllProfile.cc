/// ZFitter function library
/** \file
 * function library for ZFitter.cpp
 */
#include "../interface/nllProfile.hh"

TGraph *GetProfile(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, bool trueEval, double rho, double Emean, float phi)
{
	if(var2 == NULL) return GetProfile(var1, smearer, -1, false, trueEval, rho, Emean);

	Double_t v1 = var1->getVal();
	Double_t v2 = var2->getVal();

	TGraph *g1 = GetProfile(var1, smearer, -1, false, false, rho, Emean, phi); // take only the binning
	TGraph *g2 = GetProfile(var2, smearer, -1, false, false, rho, Emean, phi); // take only the binning
	TGraph *g3 = (rho != 0) ?  GetProfile(NULL, smearer, -1, false, false, rho, Emean, phi) : GetProfile(var1, smearer, -1, false, false, rho, Emean, phi); // take only the binning

	Double_t 	*X1    = g1->GetX();
	Double_t 	*X2    = g2->GetX();
	Int_t 	 N     = g1->GetN();
	Double_t *Y = g3->GetY();

	if(N != g2->GetN()) {
		std::cerr << "[ERROR] N1!=N2 for phi minimization: " << N << "\t" << g2->GetN() << std::endl;
		exit(1);
	}

	for(Int_t i = 0; i < N; i++) { // reset Y values
		var1->setVal(X1[i]);
		var2->setVal(X2[i]);
		Y[i] = smearer.evaluate();
	}

	var1->setVal(v1);
	var2->setVal(v2);

	delete g1;
	delete g2;
	return g3;
}

TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level, bool warningEnable, bool trueEval, float rho, float Emean, float phi)
{
	if(var == NULL) trueEval = false;
	TString name(var == NULL ? "phi" : var->GetName());
	//if(trueEval)
	std::cout << "[STATUS] Getting profile for " << name << "\t" << level << "\t" << trueEval << "\t" << rho << "\t" << Emean << std::endl;
	double minValue = var == NULL ? 0 : var->getVal();
	//double minYvalue=compatibility.evaluate();

	double bin_width = 0., sigma = 0.;
	double range_min = var == NULL ? 0      : var->getMin();
	double range_max = var == NULL ? M_PI_2 : var->getMax();
	if(Emean != 0) {
		range_min =  0     ;
		range_max =  M_PI_2;
	}

	int nBin = 0;
	switch(level) {
	case -1:
		return GetProfile(var, compatibility, 5, false, false, rho, Emean);
		break;
	case 0:
		TGraph *g1;
		g1 = GetProfile(var, compatibility, 1, false);
		TGraph *g2;
		g2 = GetProfile(var, compatibility, 2, false);
		TGraph *g3;
		g3 = GetProfile(var, compatibility, 4, false);
		Double_t X[PROFILE_NBINS], Y[PROFILE_NBINS];

		Int_t N_;
		N_ = g1->GetN();
		Double_t *X_, *Y_;
		X_ = g1->GetX();
		Y_ = g1->GetY();

		for(Int_t i = 0; i < N_; i++) {
			X[i] = X_[i];
			Y[i] = Y_[i];
		}
		nBin = N_;
		N_ = g2->GetN();
		X_ = g2->GetX();
		Y_ = g2->GetY();

		for(Int_t i = 0; i < N_; i++) {
			X[i + nBin] = X_[i];
			Y[i + nBin] = Y_[i];
		}
		nBin += N_;

		N_ = g3->GetN();
		X_ = g3->GetX();
		Y_ = g3->GetY();
		for(Int_t i = 0; i < N_; i++) {
			X[i + nBin] = X_[i];
			Y[i + nBin] = Y_[i];
		}
		nBin += N_;
		return  new TGraph(nBin, X, Y);

		break;

	case 1:
		if(name.Contains("scale")) {
			bin_width = 0.005;
		} else if(name.Contains("const") || (rho == 0 && Emean != 0)) {
			bin_width = 0.005;
		} else if(name.Contains("alpha")) {
			bin_width = 0.01;
		}
		if((var == NULL || name.Contains("alpha") || name.Contains("const")) && (Emean != 0 && rho != 0)) bin_width = 0.025;
		break;
	case 2:
		if(name.Contains("scale")) {
			bin_width = 0.0005; // level1/10
			sigma = 10 * bin_width; //
		} else  if(name.Contains("const") || (rho == 0 && Emean != 0)) {
			bin_width = 0.0005; //level1/10
			sigma = 10 * bin_width; // +/- 0.5%
		} else if(name.Contains("alpha")) {
			bin_width = 0.005;
			sigma = 5 * bin_width; // +/- 0.5%
		}

		if((var == NULL || name.Contains("alpha") || name.Contains("const")) && (Emean != 0 && rho != 0)) bin_width = 0.025;
		//sigma=var->getError();
		range_min = std::max(range_min, minValue -  sigma);
		range_max = std::min(range_max, minValue +  sigma);
		break;
	case 3:
		if(name.Contains("scale")) {
			bin_width = 0.0001;   // level2/5
			sigma = 8 * bin_width; //
		} else  if(name.Contains("const") || (rho == 0 && Emean != 0)) {
			bin_width = 0.0002;  // level2 /3
			sigma = 6 * bin_width;
		} else if(name.Contains("alpha")) {
			bin_width = 0.003;
			sigma = 5 * bin_width; // +/- 0.5%
		}
		//sigma=var->getError();
		range_min = std::max(range_min, minValue - sigma); //4sigma
		range_max = std::min(range_max, minValue + sigma);
		break;
	case 4:
		if(name.Contains("scale")) {
			bin_width = 0.0001; // level1/10
			sigma = 5 * bin_width; // +/- 0.5%
		} else  if(name.Contains("const") || (rho == 0 && Emean != 0)) {
			bin_width = 0.0001; //level1/10
			sigma = 10 * bin_width; // +/- 0.5%
		} else if(name.Contains("alpha")) {
			bin_width = 0.001;
			sigma = 5 * bin_width; // +/- 0.5%
		}
		range_min = std::max(range_min, minValue - sigma); //4sigma
		range_max = std::min(range_max, minValue + sigma);

		break;

	case 5:
		if(name.Contains("scale")) {
			bin_width = 0.0001; // level1/10
		} else  if(name.Contains("const") || (rho == 0 && Emean != 0)) {
			bin_width = 0.0001; //level1/10
		} else if(name.Contains("alpha")) {
			bin_width = 0.001;
		}
		if((var == NULL || name.Contains("alpha") || name.Contains("const")) && (Emean != 0 && rho != 0)) bin_width = 0.025;
		break;

	}
	if((var == NULL || name.Contains("alpha") || name.Contains("const")) && (Emean != 0)) bin_width = 0.025;
	nBin = (int)((range_max - range_min) / bin_width);
	//    if(Emean!=0 && rho!=0) nBin=(int) (M_PI_2 / bin_width);

#ifdef DEBUG

#endif
	std::cout << "nBin = " << nBin << "\t" << bin_width << "\t" << Emean << "\t" << rho << std::endl;
//     if(var->isConstant()==false){
//       while(nBin <10 && nBin>1){
//       //std::cout << "[INFO] Updating from range: " << nBin << "\t" << range_min << "\t" << range_max << std::endl;
//       bin_width*=0.9;
//       range_min = std::max(var->getMin(),bin_width*((floor)(range_min / bin_width))- bin_width);
//       range_max = std::min(var->getMax(),bin_width*((floor)(range_max / bin_width))+ bin_width);
//       nBin=(int)((range_max-range_min)/bin_width);
//       //std::cout << "[INFO] Updating to range: " << nBin << "\t" << range_min << "\t" << range_max << std::endl;
//       if(nBin <=0){
// 	std::cout << "[ERROR] NBins < 10" << std::endl;
// 	exit(1);
//       }
//     }
//     while(nBin >40 && level<4 && level > 0){
//       bin_width*=1.1;
//       range_min = bin_width*((floor)(range_min / bin_width));
//       range_max = bin_width*((floor)(range_max / bin_width));
//       nBin=(int)((range_max-range_min)/bin_width);
//     }
//     }
	double chi2[PROFILE_NBINS];
	double xValues[PROFILE_NBINS];
	if(trueEval) std::cout << "------------------------------" << std::endl;
	for (int iVal = 0; iVal < nBin; ++iVal) {
		double value = range_min + bin_width * iVal;
		if(rho != 0 && Emean != 0) {
			if(name.Contains("alpha")) value = rho * Emean * cos(bin_width * iVal);
			if(name.Contains("const")) value = rho * sin(bin_width * iVal);
			//std::cout << "[DEBUG] " << "\t" << rho << "\t" << Emean << "\t" << bin_width << "\t" << bin_width*iVal << "\t" << value << std::endl;
		}
		if(rho == 0 && Emean != 0) {
			if(name.Contains("alpha")) value = value * Emean * cos(phi);
			if(name.Contains("const")) value = value * sin(phi);
			//std::cout << "[DEBUG] " << "\t" << rho << "\t" << Emean << "\t" << bin_width << "\t" << bin_width*iVal << "\t" << value << std::endl;
		}

#ifdef DEBUG
		std::cout << "testing function @ " << value << std::endl;
#endif

		xValues[iVal] = value;
		if(trueEval) {
			var->setVal(value);
			chi2[iVal] = compatibility.evaluate(); //-minYvalue;
		} else chi2[iVal] = 0;
#ifdef DEBUG
		if(trueEval) std::cout << "value is " << chi2[iVal] << std::endl;
#endif
	}
	if(trueEval) {
		std::cout << name << "\tlevel=" << level << "; nBins=" << nBin << "; range_min=" << range_min << "; range_max=" << range_max << "; bin_width=" << bin_width << "\t";
		std::cout << "------------------------------" << std::endl;
	}

	if(var != NULL) var->setVal(minValue);
	TGraph *profil = new TGraph(nBin, xValues, chi2);
	return profil;
}

TTree *dataset2tree(RooDataSet *dataset)
{

	const RooArgSet *args = dataset->get();
	RooArgList argList(*args);

	Double_t variables[50];
	Long64_t nEntries = dataset->numEntries();
	//nEntries=1;
	TTree *tree = new TTree("tree", "tree");
	tree->SetDirectory(0);
	TIterator *it1 = NULL;
	it1 = argList.createIterator();
	int index1 = 0;
	for(RooRealVar *var = (RooRealVar *) it1->Next(); var != NULL;
	        var = (RooRealVar *) it1->Next(), index1++) {
		TString name(var->GetName());
		name.ReplaceAll("-", "_");
		tree->Branch(name, &(variables[index1]), name + "/D");
	}

	//  tree->Print();

	for(Long64_t jentry = 0; jentry < nEntries; jentry++) {

		TIterator *it = NULL;
		RooArgList argList1(*(dataset->get(jentry)));
		it = argList1.createIterator();
		//(dataset->get(jentry))->Print();
		int index = 0;
		for(RooRealVar *var = (RooRealVar *) it->Next(); var != NULL;
		        var = (RooRealVar *) it->Next(), index++) {
			variables[index] = var->getVal();
			//var->Print();
		}

		delete it;
		tree->Fill();
	}

	//  tree->Print();
	return tree;
}

TMatrixDSym* GetCovariance( RooStats::MarkovChain *chain, TString var1, TString var2)
{

//   RooRealVar *nll = chain->GetNLLVar();
//   //RooRealVar *weight= chain->GetWeightVar();
//   RooFormulaVar newWeight("f","","-@0",*nll);

	const RooArgSet *args = chain->Get();
	RooArgSet newArg;
//   RooArgSet oldArg;
//   oldArg.add(*(args->find(var1)));
//   oldArg.add(*(args->find(var2)));
//   oldArg.add(*nll);
//   oldArg.Print();
	newArg.add(*(args->find(var1)));
	newArg.add(*(args->find(var2)));
//   newArg.Print();
//   //newArg.add(*nll);
//   //newArg.add(newWeight);

//   RooDataSet dataset("dataset","",   chain->GetAsDataSet(), RooArgSet(oldArg,newWeight), "", newWeight.GetName());
	chain->GetAsDataSet()->covarianceMatrix()->Print();
	return chain->GetAsDataSet()->covarianceMatrix();
}

#ifdef MCMC
bool MinMCMC2D(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, int iProfile,
               Double_t min_old, Double_t& min, long unsigned int nIter, bool update = true)
{
	Double_t chi2, chi2init = smearer.evaluate();
	bool changed = false;

	TStopwatch myClock;
	myClock.Start();

	Double_t v1 = var1->getVal();
	Double_t v2 = var2->getVal();

	// fix the scales to constant!
	RooArgSet floatingVars;
	RooArgList 	 argList_(smearer.GetParams());
	TIterator 	*it_ = argList_.createIterator();
	for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()) {
		if (var->isConstant() || !var->isLValue()) continue;
		TString  name(var->GetName());
		if(name != var1->GetName() && name != var2->GetName()) {
			floatingVars.add(*var);
			var->setConstant();
		}
	}
	delete it_;

	//creating MCMC proposal
	RooStats::ProposalHelper ph;
	ph.SetVariables(smearer.GetParams());
	if(iProfile > 1) {
		ph.SetCovMatrix(*smearer._markov.GetAsDataSet()->covarianceMatrix(smearer.GetParams())); ////*GetCovariance(&smearer._markov, var1->GetName(), var2->GetName()));
		ph.SetUniformFraction(0.2 / iProfile);
		if(iProfile == 2) ph.SetWidthRangeDivisor(5);
		if(iProfile == 3) ph.SetWidthRangeDivisor(20);
	} else {
		ph.SetUniformFraction(1);
		ph.SetWidthRangeDivisor(1);
	}

	//running the mcmc
	RooStats::MetropolisHastings MCMC(smearer, smearer.GetParams(), *(ph.GetProposalFunction()), (Int_t)nIter);
	MCMC.SetNumBurnInSteps(10);
	MCMC.SetSign(RooStats::MetropolisHastings::kNegative);
	MCMC.SetType(RooStats::MetropolisHastings::kLog);
	RooStats::MarkovChain *mcChain = (MCMC.ConstructChain());

	//taking the results
	RooDataSet *dataset = mcChain->GetAsDataSet();
	TTree *tree = dataset2tree(dataset);
	TString nllVarName = mcChain->GetNLLVar()->GetName();
	chi2 = tree->GetMinimum(nllVarName);
	std::cout << "[DEBUG] chi2 = tree->GetMinimum(nllVarName): " << chi2 << std::endl;
	TString selMin = nllVarName + " == ";
	selMin += chi2;

	tree->Draw(">>entrylist", selMin, "entrylist");
	TEntryList *entrylist = (TEntryList*) gROOT->FindObject("entrylist");
	if(entrylist->GetN() != 1) {
		std::cout << "Entrylist N=" << entrylist->GetN() << std::endl;
		return "";
	}
	Long64_t minEntry = entrylist->GetEntry(0);

	TString var1Name = var1->GetName();
	var1Name.ReplaceAll("-", "_");
	TString var2Name = var2->GetName();
	var2Name.ReplaceAll("-", "_");

	TObjArray *branchArray = tree->GetListOfBranches();
	for(int i = 0; i < branchArray->GetEntriesFast(); i++) {
		TBranch *b = (TBranch *) branchArray->At(i);
		TString name = b->GetName();
		if(name != var1Name && name != var2Name) continue;
		Double_t address;
		b->SetAddress(&address);
		b->GetEntry(minEntry);
		if(name == var1Name) var1->setVal(address);
		else var2->setVal(address);
	}

	chi2 = smearer.evaluate(); // local minimum

	if(chi2 < chi2init) {
		if(chi2 < min) min = chi2;
		if(update == true) { // && (min<min_old)){
			std::cout << std::setprecision(5) << "[INFO] Updating variables:" << std::endl
			          << "       " << var1->GetName() << "\t" << var1->getVal() << std::endl
			          << "       " << var2->GetName() << "\t" << var2->getVal() << std::endl
			          << "       " << min - chi2init << "\t" << chi2 - chi2init << "\t" << chi2init << std::endl;
			changed = true;
		} else {
			if(chi2 < chi2init) std::cout << "[INFO] New minimum found but not updating" << std::endl;

			std::cout << "[INFO] Not-updating variables:" << std::endl
			          << "       " << var1->GetName() << "\t" << var1->getVal() << std::endl
			          << "       " << var2->GetName() << "\t" << var2->getVal() << std::endl
			          << "       " << min - chi2init << "\t" << chi2 - chi2init << std::endl;
			var1->setVal(v1);
			var2->setVal(v2);
			std::cout
			        << "       " << var1->GetName() << "\t" << var1->getVal() << std::endl
			        << "       " << var2->GetName() << "\t" << var2->getVal() << std::endl
			        << "       " << min - chi2init << std::endl;
		}
	}

	std::cout << "------------------------------" << std::endl;
	myClock.Stop();
	myClock.Print();
	std::cout << "------------------------------" << std::endl;

	//  std::cout << "[INFO] Level " << iProfile << " variable:" << var->GetName() << "\t" << var->getVal() << "\t" << min << std::endl;



	//floating the scales
	RooArgList 	 argList(floatingVars);
	it_ = argList.createIterator();
	for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()) {
		var->setConstant(kFALSE);
	}
	delete it_;

	delete mcChain;
	delete tree;
	delete entrylist;

	return changed;
}
#endif

void Smooth(TH2F *h, Int_t ntimes, Option_t *option)
{
	Double_t k5a[5][5] =  { { 0, 0, 1, 0, 0 },
		{ 0, 2, 2, 2, 0 },
		{ 1, 2, 5, 2, 1 },
		{ 0, 2, 2, 2, 0 },
		{ 0, 0, 1, 0, 0 }
	};
	Double_t k5b[5][5] =  { { 0, 1, 2, 1, 0 },
		{ 1, 2, 4, 2, 1 },
		{ 2, 4, 8, 4, 2 },
		{ 1, 2, 4, 2, 1 },
		{ 0, 1, 2, 1, 0 }
	};
	Double_t k3a[3][3] =  { { 0, 1, 0 },
		{ 1, 2, 1 },
		{ 0, 1, 0 }
	};

	//  if (ntimes > 1) {
	// Warning("Smooth","Currently only ntimes=1 is supported");
	//   }
	TString opt = option;
	opt.ToLower();
	Int_t ksize_x = 5;
	Int_t ksize_y = 5;
	Double_t *kernel = &k5a[0][0];
	if (opt.Contains("k5b")) kernel = &k5b[0][0];
	if (opt.Contains("k3a")) {
		kernel = &k3a[0][0];
		ksize_x = 3;
		ksize_y = 3;
	}

	// find i,j ranges
	Int_t ifirst = h->GetXaxis()->GetFirst();
	Int_t ilast  = h->GetXaxis()->GetLast();
	Int_t jfirst = h->GetYaxis()->GetFirst();
	Int_t jlast  = h->GetYaxis()->GetLast();

	// Determine the size of the bin buffer(s) needed
	//Double_t nentries = h->GetEntries();
	Int_t nx = h->GetNbinsX();
	Int_t ny = h->GetNbinsY();
	Int_t bufSize  = (nx + 2) * (ny + 2);
	Double_t *buf  = new Double_t[bufSize];
	Double_t *ebuf = 0;
	if (h->GetSumw2()->fN) ebuf = new Double_t[bufSize];

	// Copy all the data to the temporary buffers
	Int_t i, j, bin;
	for (i = ifirst; i <= ilast; i++) {
		for (j = jfirst; j <= jlast; j++) {
			bin = h->GetBin(i, j);
			buf[bin] = h->GetBinContent(bin);
			if (ebuf) ebuf[bin] = h->GetBinError(bin);
		}
	}

	// Kernel tail sizes (kernel sizes must be odd for this to work!)
	Int_t x_push = (ksize_x - 1) / 2;
	Int_t y_push = (ksize_y - 1) / 2;

	// main work loop
	for (i = ifirst; i <= ilast; i++) {
		for (j = jfirst; j <= jlast; j++) {
			Double_t content = 0.0;
			Double_t error = 0.0;
			Double_t norm = 0.0;
			if(h->GetBinContent(i, j) <= 0) continue;
			for (Int_t n = 0; n < ksize_x; n++) {
				for (Int_t m = 0; m < ksize_y; m++) {
					Int_t xb = i + (n - x_push);
					Int_t yb = j + (m - y_push);
					if ( (xb >= 1) && (xb <= nx) && (yb >= 1) && (yb <= ny)) {
						bin = h->GetBin(xb, yb);
						if(buf[bin] > 0) {
							Double_t k = kernel[n * ksize_y + m];
							//if ( (k != 0.0 ) && (buf[bin] != 0.0) ) { // General version probably does not want the second condition
							if ( k != 0.0 ) {
								norm    += k;
								content += k * buf[bin];
								if (ebuf) error   += k * k * buf[bin] * buf[bin];
							}
						}
					}
				}
			}

			if ( norm != 0.0 ) {
				h->SetBinContent(i, j, content / norm);
				if (ebuf) {
					error /= (norm * norm);
					h->SetBinError(i, j, sqrt(error));
				}
			}
		}
	}
	//fEntries = nentries;

	delete [] buf;
	delete [] ebuf;
}

bool stopFindMin1D(Int_t i, Int_t iLocMin, Double_t chi2, Double_t min, Double_t locmin, float phiMin)
{
	if(abs(i - iLocMin) > 2 && chi2 - min > 300) {
		std::cout << "stop 1" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 5 && chi2 - min > 200) {
		std::cout << "stop 2" << std::endl;
		return true;
	}
//   if(abs(i-iLocMin)>6 && chi2-locmin > 120){ std::cout << "stop 3" << std::endl; return true;}
//   if(abs(i-iLocMin)>7 && chi2-locmin > 80){ std::cout << "stop 4" << std::endl; return true;}
//   if(abs(i-iLocMin)>8 && chi2-locmin > 50){ std::cout << "stop 8" << std::endl; return true;}
//   if(abs(i-iLocMin)>9 && chi2-locmin > 20){ std::cout << "stop 9" << std::endl; return true;}
//   if(abs(i-iLocMin)>10 && chi2-locmin > 10){ std::cout << "stop 10" << std::endl; return true;}
//   if(abs(i-iLocMin)>11 && chi2-locmin > 5){ std::cout << "stop 11" << std::endl; return true;}
//   if(abs(i-iLocMin)>12 && chi2-locmin > 3){ std::cout << "stop 12" << std::endl; return true;}

	if(abs(i - iLocMin) > 3 && chi2 - locmin > phiMin * 20) {
		std::cout << "stop 3" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 4 && chi2 - locmin > phiMin * 10) {
		std::cout << "stop 4" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 5 && chi2 - locmin > phiMin * 5) {
		std::cout << "stop 8" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 6 && chi2 - locmin > phiMin * 4) {
		std::cout << "stop 9" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 7 && chi2 - locmin > phiMin * 3) {
		std::cout << "stop 10" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 8 && chi2 - locmin > phiMin * 2) {
		std::cout << "stop 10" << std::endl;
		return true;
	}
	if(abs(i - iLocMin) > 9 && chi2 - locmin > phiMin * 1) {
		std::cout << "stop 10" << std::endl;
		return true;
	}

	return false;
}

Int_t FindMin1D(RooRealVar *var, Double_t *X, Int_t N, Int_t iMinStart, Double_t min, RooSmearer& smearer, bool update, Double_t *Y, RooRealVar *var2, Double_t *X2)
{
	//Double_t vInit=var->getVal();
	var->setVal(X[iMinStart]);
	float phiMin = 1;
	if(var2 != NULL) {
		var2->setVal(X2[iMinStart]);
		phiMin = 5;
	}
	TString varName = var->GetName();
	if(varName.Contains("constTerm") || varName.Contains("alpha")) {
		phiMin = 2;
	}
	if(Y == NULL) exit(1);

	Double_t chi2, chi2init = smearer.evaluate(); //, chi2old=chi2init;

	Double_t locmin = 1e20;
	Int_t iLocMin = iMinStart;

	Double_t NY[PROFILE_NBINS] = {0.};
	int iStep = 1;
	for(Int_t i = iMinStart; i < N; i += iStep) { //loop versus positive side
		var->setVal(X[i]);
		if(var2 != NULL) var2->setVal(X2[i]);
		chi2 = smearer.evaluate();
		Y[i] += chi2;
		NY[i]++;

		//iStep = chi2-chi2old != 0 ? std::max(1,((int) (fabs(0.1/(chi2-chi2old))))) : 1;
		iStep = 1;
#ifdef debug_fit
		if(var2 != NULL) std::cout << "[FindMin1D] "  << std::setprecision(4) <<  "\t" << var->getVal() << "\t" << var2->getVal() << "\t" <<
			                           chi2 - chi2init << "\t" << locmin - chi2init << "\t" << min - chi2init << std::endl;
		else std::cout  << std::setprecision(4) << "[DEBUG] " <<  "\t" << iStep << "\t" << var->getVal() << "\t" << chi2 - chi2init << "\t" << locmin - chi2init << "\t" << min - chi2init << "\tchi2= " <<  chi2 << std::endl;
#endif
		if(chi2 <= locmin) { //local minimum
			iLocMin = i;
			locmin = chi2;
		}

		if(stopFindMin1D(i, iLocMin, chi2, min, locmin, phiMin)) break;
		//chi2old=chi2;
		if(i == iLocMin && i - iMinStart == 3 && Y[iMinStart] == Y[iMinStart + 1] && Y[iMinStart] == Y[iMinStart + 2] && Y[iMinStart] == Y[iMinStart + 3]) {
			std::cerr << "[WARNING] No sensitivity to variable: " << var->GetName() << std::endl;
			std::cerr << "          Variable changed to constant" << std::endl;
			std::cout << "[WARNING] No sensitivity to variable: " << var->GetName() << std::endl;
			std::cout << "          Variable changed to constant" << std::endl;
			return -1;
		}
	}

	iStep = 1;
	for(Int_t i = iMinStart; i >= 0; i -= iStep) { //loop versus positive side
		var->setVal(X[i]);
		if(var2 != NULL) var2->setVal(X2[i]);
		chi2 = smearer.evaluate();
		Y[i] += chi2;
		NY[i]++;

		//iStep = chi2-chi2old != 0 ? std::max(1,((int) (fabs(0.1/(chi2-chi2old))))) : 1;
		iStep = 1;
#ifdef DEBUG
		//if(update==true)
		std::cout << "[DEBUG] " <<  "\t" << var->getVal() << "\t" << chi2 - chi2init << "\t" << locmin - chi2init << std::endl;
#endif
		if(var2 != NULL) std::cout <<   "[DEBUG] Neg"  << std::setprecision(3) <<  "\t" << var->getVal() << "\t" << var2->getVal() << "\t" <<
			                           chi2 - chi2init << "\t" << locmin - chi2init << "\t" << min - chi2init << std::endl;
		else std::cout << std::setprecision(4) << "[DEBUG] Neg" <<  "\t" <<  iStep << "\t" << var->getVal() << "\t" << chi2 - chi2init << "\t" << locmin - chi2init << "\t" << min - chi2init << std::endl;

		if(chi2 <= locmin) { //local minimum
			iLocMin = i;
			locmin = chi2;
		}

		if(stopFindMin1D(i, iLocMin, chi2, min, locmin, phiMin)) break;
		//chi2old=chi2;
	}

	if(Y != NULL) { //take the mean!
		for(Int_t i = 0; i < N; i++) {
			if(NY[i] != 0) Y[i] /= NY[i];
		}
	}

	return iLocMin;
}

bool MinProfile(RooRealVar *var, RooSmearer& smearer, int iProfile,
                Double_t min_old, Double_t& min, double rho, double Emean, bool update, bool dscan)
{
	bool changed = false;

	Double_t v1 = var->getVal();
	//Double_t chi2init=
	smearer.evaluate();

	std::cout << "[STATUS] Starting MinProfile for " << var->GetName() << "\t" << iProfile << std::endl;
	TGraph 	*profil = GetProfile(var, smearer, iProfile, false, false, rho, Emean);
	//TGraphErrors 	 g(profil->GetN(),profil->GetX(), profil->GetY());


	Double_t 	*X    = profil->GetX();
	Double_t 	*Y    = profil->GetY();
	Int_t 	 N    = profil->GetN();

	Double_t chi2 = Y[0];


	Int_t iMin_ = 0; // minimum of the graph

	for(Int_t i2 = 0; i2 < N; i2++) { // reset Y values
		Y[i2] = 0;
		if(X[iMin_] < v1) iMin_++;
	}
	iMin_ = FindMin1D(var, X, N, iMin_, min, smearer, true, Y);
	if(iMin_ < 0) { // in case of no sensitivity to the variable it is put has constant and the minimization is interrupted
		std::cerr << "[WARNING] No sensitivity to var: " << var->GetName() << std::endl;
		var->setVal(v1);
		var->setConstant();
		return false;
	}
	chi2 = Y[iMin_];

	var->setVal(v1); //reset to initial value

	if(chi2 < min) { // updated absolute minimum
		min = chi2;
		if(v1 != X[iMin_]) changed = true; //the value has been updated, need a new iteration
		//else a fluctuation changed the min value
	}

	if(update && (min < min_old || changed)) { // if a new minimum has been found now, or due to another variable, need to update the scale
		std::cout << "[INFO] Updating variable:" << var->GetName() << " from " << v1 << " to " << X[iMin_]  << std::endl
		          << "       min = " << min << "; Y[iMin_]-min =  " << Y[iMin_] - min << std::endl;
		var->setVal(X[iMin_]);
		//var->setError(newError);
	} else {
		std::cout << "[INFO] Not-Updating variable:" << var->GetName() << "\t" << X[iMin_]
		          << " " << min << " " << Y[iMin_] - min << std::endl;
	}
	//  std::cout << "[INFO] Level " << iProfile << " variable:" << var->GetName() << "\t" << var->getVal() << "\t" << min << std::endl;
	delete profil;
	//  delete X;
	//delete Y;
	return changed;
}

/**
   min is updated
 */
bool MinProfile2D(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, int iProfile,
                  Double_t min_old, Double_t& min, double& rho, double& Emean, bool update, bool dscan)
{
#ifdef MEM_DEBUG
	ProcInfo_t info;
	gSystem->GetProcInfo(&info);
	std::cout << "GET MEM INFO AT MinProfile2D START " << info.fMemResident << std::endl;
#endif

	bool changed = false;
	std::cout << "------------------------------ MinProfile2D: iProfile==" << iProfile << std::endl;

	std::cout << "--------------------------------- Init eval "  << std::endl;
	//initial values
	Double_t v1 = var1->getVal();
	Double_t v2 = var2->getVal();
	smearer.evaluate();
	Double_t chi2; //, chi2init=smearer.evaluate();
	Double_t locmin = 1e20;

	TStopwatch myClock;
	myClock.Start();

	var2->setVal(0);
	var1->setVal(sqrt(v1 * v1 + v2 * v2 / 50));
	changed += MinProfile(var1, smearer, -1, min_old, locmin); // best DeltaC with DeltaS=0
	// rho = best DeltaC with DeltaS=0
	rho = var1->getVal();

	locmin = 1e20;
	var1->setVal(0);
	var2->setVal(rho * 7);
	MinProfile(var2, smearer, -1, min_old, locmin); // best DeltaS with DeltaC=0

	//sqrt(<E>) = DeltaS/DeltaC
	Emean = var2->getVal() / rho;

	std::cout << "--------------------------------- GetProfile for " << var1->GetName() << std::endl;
	TGraph 	*g1 = GetProfile(var1, smearer, iProfile, false, false, rho, Emean); // take only the binning
	std::cout << "--------------------------------- GetProfile for " << var2->GetName() << std::endl;
	TGraph 	*g2 = GetProfile(var2, smearer, iProfile, false, false, rho, Emean); // take only the binning

	Double_t 	*X1    = g1->GetX();
	Double_t 	*X2    = g2->GetX();
	Int_t 	 N    = g1->GetN();

	if(N != g2->GetN()) {
		std::cerr << "[ERROR] N1!=N2 for phi minimization: " << N << "\t" << g2->GetN() << std::endl;
		exit(1);
	}


	Double_t *Y = g1->GetY();

	//Int_t iLocMin1=0, iLocMin2,
	Int_t iLocMin2Prev = 10;
	//Double_t      Y2[PROFILE_NBINS];
	for(Int_t i2 = 0; i2 < N; i2++) { // reset Y values
		Y[i2] = 0;
		//    if(X1[iLocMin2Prev]<v1) iLocMin2Prev++;
	}


	std::cout << "[STATUS] Starting MinProfile for " << "phi" << "\t" << iProfile << "\t" << iLocMin2Prev << std::endl;
	Int_t iMin_ = -1; // minimum of the graph
	locmin = 1e20;
	//find minimum
	iMin_ = FindMin1D(var1, X1, N, iLocMin2Prev, locmin, smearer, true, Y, var2, X2);
	if(iMin_ < 0) { // in case of no sensitivity to the variable it is put has constant and the minimization is interrupted
		var1->setVal(v1);
		var1->setConstant();
		var2->setVal(v2);
		var2->setConstant();
		return false;
	}

	chi2 = Y[iMin_];
	// reset initial values
	var1->setVal(v1);
	var2->setVal(v2);

	if(chi2 < min) { // updated absolute minimum
		min = chi2;
		if(var1->getVal() != X1[iMin_]) changed = true; //the value has been updated, need a new iteration
		if(var2->getVal() != X2[iMin_]) changed = true; //the value has been updated, need a new iteration
	}

	myClock.Stop();
	myClock.Print();

	//------------------------------ making the 2D scan around the minimum
	if(update == false && dscan == true) {
		myClock.Start();
		var1->setVal(X1[iMin_]);
		var2->setVal(X2[iMin_]);
		TGraph 	*G1 = GetProfile(var1, smearer, -1, false, false); // take only the binning
		TGraph 	*G2 = GetProfile(var2, smearer, -1, false, false); // take only the binning
		Double_t *XG1 = G1->GetX();
		Double_t *XG2 = G2->GetX();
		Int_t iBin1 = 0, iBin2 = 0;


		while(XG1[iBin1] < X1[iMin_]) { //nearest bin to the minimum
			//std::cout << "[DEBUG] Nearest bin to minimum: " << iBin1 << "\t" << XG1[iBin1] << "  " << X1[iMin_] << std::endl;
			iBin1++;
		}
		while(XG2[iBin2] < X2[iMin_]) { //nearest bin to the minimum
			//std::cout << "[DEBUG] Nearest bin to minimum: " << iBin2 << "\t" << XG2[iBin2] << "  " << X2[iMin_] << std::endl;
			iBin2++;
		}
		Int_t iBin1min = std::max(iBin1 - 6, 0);
		Int_t iBin2min = std::max(iBin2 - 6, 0);
		Int_t iBin1max = std::min(iBin1 + 6, G1->GetN());
		Int_t iBin2max = std::min(iBin2 + 6, G1->GetN());

		//Int_t iBin1_ = iBin1, iBin2_=iBin2,
		Int_t iBinM = (int)(sqrt(iBin1 * iBin1 + iBin2 * iBin2));

		iBin1min = 0;
		iBin1max = G1->GetN();
		iBin2min = 0;
		iBin2max = G1->GetN();
		std::cout << "[INFO] " << "iBin1min = " << iBin1min << std::endl
		          << "       " << "iBin1max = " << iBin1max << std::endl
		          << "       " << "iBin2min = " << iBin2min << std::endl
		          << "       " << "iBin2max = " << iBin2max << std::endl;

		for(iBin1 = iBin1min; iBin1 < iBin1max; iBin1++) {
			var1->setVal(XG1[iBin1]);
			for(iBin2 = iBin2min; iBin2 < iBin2max; iBin2++) {
				if(sqrt(iBin1 * iBin1 + Emean * iBin2 * iBin2) < iBinM - 10) continue;
				if(sqrt(iBin1 * iBin1 + Emean * iBin2 * iBin2) > iBinM + 10) continue;
				var2->setVal(XG2[iBin2]);
				smearer.evaluate();
				std::cout << "[DEBUG] " << XG1[iBin1] << "\t" << XG2[iBin2] << "\t" << smearer.evaluate() - chi2 << std::endl;
			}
		}

		myClock.Stop();
		myClock.Print();
		delete G1;
		delete G2;
//     delete XG1;
//     delete XG2;
		var1->setVal(v1);
		var2->setVal(v2);
	}


	if(update && (min < min_old || changed)) {
		// if a new minimum has been found now, or due to another variable, need to update the scale
		std::cout << "[INFO] Updating variable:" << var1->GetName() << "\t" << X1[iMin_]
		          << "\t" << var2->GetName() << "\t" << X2[iMin_]
		          << " " << min << " " << Y[iMin_] - min << std::endl;
		var1->setVal(X1[iMin_]);
		var2->setVal(X2[iMin_]);
	} else {
		std::cout << "[INFO] Not-Updating variable:" << var1->GetName() << "\t" << X1[iMin_]  << std::endl
		          << "                             " << var2->GetName() << "\t" << X2[iMin_]  << std::endl
		          << " " << min << " " << Y[iMin_] - min << std::endl;
	}
	std::cout << "[INFO] Level " << iProfile << " variable:" << var1->GetName() << "\t" << var1->getVal() << "\t" << min << std::endl;
	std::cout << "[INFO] Level " << iProfile << " variable:" << var2->GetName() << "\t" << var2->getVal() << "\t" << min << std::endl;

	delete g1;
	delete g2;
#ifdef MEM_DEBUG
	gSystem->GetProcInfo(&info);
	std::cout << "GET MEM INFO JUST BEFORE END MinProfile2D " << info.fMemResident << std::endl;
#endif

	return changed;
}





void MinimizationProfile(RooSmearer& smearer, RooArgSet args, long unsigned int nIterMCMC, bool mcmc)
{
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "[INFO] Minimization: profile" << std::endl;
	std::cout << "[INFO] Re-initialize nllMin: 1e20"  << std::endl;
	smearer.nllMin = 1e20;

	std::cout << "[INFO] Setting initial evaluation" << std::endl;
	smearer.evaluate();
	Double_t 	min_old	    = 9999999999.;
	Double_t 	min	    = 999999999.;
	bool 	updateError = false;
	int nIter = 0;
	TStopwatch myClock;
	myClock.Start();

	std::cout << "[INFO] Starting scale minimization" << std::endl;
	RooArgList 	 argList_(args);
	TIterator 	*it_   = NULL;
	while((min < min_old || updateError == true) && nIter < 3) {
		updateError = false;
		TStopwatch iterClock;
		iterClock.Start();
		if(min < min_old) min_old = min;
		it_ = argList_.createIterator();
		for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()) {
			if (var->isConstant() || !var->isLValue()) continue;
			TString  name(var->GetName());
			if(!name.Contains("scale")) continue; // looping only for the scale
			updateError += MinProfile(var, smearer, -1, min_old, min);
		}
		delete it_;
		iterClock.Stop();
		std::cout << "[INFO] nIter scale: " << nIter << "\t";
		iterClock.Print();
		nIter++;
	}

	std::cout << "[STATUS] Scale minimization done";
	std::cout << "------------------------------" << std::endl;
	std::cout << "[STATUS] Smearing + scale minimization starting" << std::endl;

	//minimization of additional smearing
	updateError = true;
	min_old = min;
	nIter = 0;
	while((min < min_old && nIter < 5) || (updateError == true && nIter < 2)) {
		updateError = false;
		TStopwatch iterClock;
		iterClock.Start();
		it_ = argList_.createIterator();
		if(min < min_old) min_old = min;
		for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()) {
			if (var->isConstant()) continue;
			TString  name(var->GetName());
			// special part for alpha fitting
			TString  alphaName = name;
			alphaName.ReplaceAll("constTerm", "alpha");
			RooRealVar *var2 = name.Contains("constTerm") ? (RooRealVar *)argList_.find(alphaName) : NULL;
			if(name.Contains("alpha")) continue; //if alpha parameter exists, need a 2D scan
			//if(var2!=NULL && var2->isConstant()) var2=NULL; // to use MinProfile 1D instead of 2D
			//taking large profile
			if(var2 == NULL || var2->isConstant()) MinProfile(var, smearer, -1, min_old, min); //updateError += MinProfile(var, smearer, iProfile, min_old, min);
			else {
				//if(mcmc && iProfile>2) updateError += MinMCMC2D(var, var2, smearer, iProfile, min_old, min, nIterMCMC);
				//  else
				double rho = 0, Emean = 0;
				updateError += MinProfile2D(var, var2, smearer, -1, min_old, min, rho, Emean);
			}
		}
		delete it_;
		iterClock.Stop();
		std::cout << "[DEBUG] nIter= " << nIter << "\t";
		iterClock.Print();
		nIter++;
		smearer.GetParams().writeToStream(std::cout, kFALSE);
	}
}




ShervinMinuit::ShervinMinuit(int nBurnSteps, RooSmearer& smearer):
	_smearer(smearer), gausPdf(NULL), gen(1245), fSigmaRangeDivisor(-1)
	//  _params(smearer.GetParams())
{

	_iStep = 0;

	_nBurnSteps = nBurnSteps;

	RooArgList argList(smearer.GetParams());
	TIterator *it = argList.createIterator();
	std::cout << "###############################" << std::endl;
	for(RooRealVar *v = (RooRealVar*)it->Next(); v != NULL; v = (RooRealVar*)it->Next()) {
		if(! v->isConstant()) {
			_paramSet.add((*v));
			rmsHist.push_back(new TH1F(v->GetName(), "", 1000, v->getMin(), v->getMax()));
		}
	}
	delete it;
	//  _paramSet.Print();
//   ph = new ProposalHelper();
//   ph->SetVariables(_paramSet);
//   ph->SetUpdateProposalParameters(kTRUE); // auto-create mean vars and add mappings
//   ph->SetUniformFraction(1);
//   ph->SetCluesFraction(0);
//   ph->SetWidthRangeDivisor(100);
	//  proposal = ph->GetProposalFunction();

	RooArgList xVec(_paramSet);
	Int_t size = xVec.getSize();
	Float_t fSigmaRangeDivisor = 100;
	TMatrixDSym *fCovMatrix = new TMatrixDSym(size);
	RooRealVar* r;
	for (Int_t i = 0; i < size; i++) {
		r = (RooRealVar*)xVec.at(i);
		Double_t range = r->getMax() - r->getMin();
		(*fCovMatrix)(i, i) = range / fSigmaRangeDivisor;
	}

//   gausPdf = new RooMultiVarGaussian("multiVar","", RooArgList(_paramSet), *fCovMatrix);
//   gausPdf->initGenerator(12345);
//   gausPdf->  generateEvent(1);
//   SetPdf(*gausPdf);

//   //  gausPdf->generate(_paramSet,10)->Print();
//   SetCacheSize(10);
	//proposal = ph->GetProposalFunction();
	//proposal = &propUnif;
	//_iBurnStep = (int) _nBurnSteps/100;
	//if(_iBurnStep==0)   _iBurnStep = (int) _nBurnSteps/10;
	//if(_iBurnStep==0)   _iBurnStep = (int) _nBurnSteps/5;
}


void ShervinMinuit::Propose(RooArgSet& xPrime, RooArgSet& x)
{
	TIterator *it = NULL;
//   RooRealVar v1("v1","",0.01,0,0.04);
//   RooRealVar v2("v2","",0.02,0,0.04);
//   RooArgSet arg1(v1);
//   RooArgSet arg2(v2);

	if(_iStep == 20) {
		fSigmaRangeDivisor = 4;
		RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
		float varErr = (var->getMax() - var->getMin()) / fSigmaRangeDivisor;
		std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor
		          << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
		          << std::endl;
	} else if(_iStep == 50) {
		fSigmaRangeDivisor = 10;
		std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor << std::endl;
		RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
		float varErr = (var->getMax() - var->getMin()) / fSigmaRangeDivisor;
		std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor
		          << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
		          << std::endl;

	} else if(_iStep == 80) {
		fSigmaRangeDivisor = 15;
		std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor << std::endl;
		RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
		float varErr = (var->getMax() - var->getMin()) / fSigmaRangeDivisor;
		std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor
		          << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
		          << std::endl;
		var = (RooRealVar *)x.find("scale_EB-absEta_0_1-gold-Et_25");
		varErr = (var->getMax() - var->getMin()) / fSigmaRangeDivisor;
		std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor
		          << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
		          << std::endl;

//  }  else if(_iStep==100){
//     fSigmaRangeDivisor=20;
//     std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor << std::endl;
//     RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
//     float varErr = (var->getMax()-var->getMin())/fSigmaRangeDivisor;
//     std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor
// 	      << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
// 	      << std::endl;

	}


	//else if(_iStep==100) fSigmaRangeDivisor=10;

	it = RooArgList(xPrime).createIterator();
	int index = 0;
	for(RooRealVar *var = (RooRealVar *) it->Next(); var != NULL;
	        var = (RooRealVar *) it->Next()) {

		//var->setError(var->getError()/100);


		RooRealVar *v_x = ((RooRealVar *)x.find(var->GetName()));
		rmsHist[index]->Fill(v_x->getVal());

		float varErr = std::min(rmsHist[index]->GetRMS(), (var->getMax() - var->getMin()) / fSigmaRangeDivisor);


		if(fSigmaRangeDivisor < 3)     var->setVal(gen.Uniform(v_x->getMin(), v_x->getMax()));
		else {
			if(varErr < (var->getMax() - var->getMin()) / 100) std::cout << "[WARNING] " << varErr << std::endl;
			var->setVal(gen.Gaus(
			                ((RooRealVar *)x.find(var->GetName()))->getVal(),
			                varErr)
			           );
		}

//     if(TString(var->GetName())=="constTerm_EB-absEta_0_1-gold-Et_25"){
//       std::cout << var->GetName() << "\t" << v_x->getVal() << " --> " << var->getVal() << "\t" << varErr << std::endl;
//     }
		index++;
	}

	_iStep++;
	return;



	if(_iStep == _nBurnSteps) {
		std::cout << "Now Gaussian proposal" << std::endl;

		it = xPrime.createIterator();
		for(RooRealVar *var = (RooRealVar *) it->Next(); var != NULL;
		        var = (RooRealVar *) it->Next()) {
			//var->setError(var->getError()/100);
			//float varErr = var->getVal()/100;
			//if(TString(var->GetName()).Contains("constTerm")) varErr*=10;
			//var->setMin(std::max(var->getMin(),var->getVal()- varErr*3));
			//var->setMax(std::min(var->getMax(),var->getVal()+ varErr*3));
			var->setMin(0.0);
			var->setMax(0.05);
			//var->setVal(0.03);
			std::cout << var -> GetName() << "\t" << var->getError() << "\t" << var -> getMin() << "\t" << var->getMax() << std::endl;
		}
		//_m->hesse();
		//RooFitResult *fitres = _m->save();
	}
}

Double_t asymmetricParabola(Double_t* x, Double_t* par)
{
	// qualche commento non farebbe male
	Double_t value = 0.;
	if (x[0] <= par[1])
		value = par[2] * (x[0] - par[1]) * (x[0] - par[1]);
	else
		value = par[3] * (x[0] - par[1]) * (x[0] - par[1]);
	return value + par[0];
}

TF1* IterMinimumFit(TGraphErrors *g, RooRealVar *xVar)
{
	double range_min;// = min-40*sigma;
	double range_max;// = min+30*sigma;

	Double_t *X = g->GetX();
	Double_t X_bin = fabs(X[1] - X[0]);

	range_min = TMath::Max(TMath::MinElement(g->GetN(), g->GetX()), xVar->getMin());
	range_max = TMath::Min(TMath::MaxElement(g->GetN(), g->GetX()), xVar->getMax());

	g->Fit("pol2", "0FR+", "", range_min, range_max);
	TFitResult fitres;

	TF1 *fun = (TF1*) g->GetListOfFunctions()->At(g->GetListOfFunctions()->GetSize() - 1);
	if(fun == NULL) {
		std::cerr << "Error: polynomial fit function not defined" << std::endl;
		exit(1);
	}
	fun->SetRange(range_min, range_max);
	fun->GetRange(range_min, range_max);
#ifdef DEBUG
	fun->Print();
#endif
	//fun->SetRange(0.95,1.04);
	//  double sigma_old;

	//  double sigma=1./sqrt(2* fun->GetParameter(2));
	//  double minX=-fun->GetParameter(1) / (2* fun->GetParameter(2));
	// double sigma=(range_max-range_min)/2;
	double sigma = 1. / sqrt(2 * fun->GetParameter(2));
	double sigma_plus = sigma;
	double sigma_minus = sigma;
	double minX = g->GetX()[TMath::LocMin(g->GetN(), g->GetY())];

	double old_chi2 = 0., chi2 = 0.;

	//  range_max=range_min+0.01;
	TF1* f2 = new TF1("f2", asymmetricParabola, range_min, range_max, 4);
	f2->SetParameter(0, TMath::MinElement(g->GetN(), g->GetY()));
	f2->SetParameter(1, minX);
	f2->SetParameter(2, 1 / (2 * sigma * sigma));
	f2->SetParameter(3, 1 / (2 * sigma * sigma));
	int iter = 0;
	do {
		old_chi2 = chi2;

		std::cout << "Fit range: [ " << range_min << " : " << range_max << "]" << std::endl;
		f2->SetName("f2_iter" + iter);

		//    g->Fit(fun,"F+","",range_min, range_max);
		g->Fit(f2, "0R+", "", range_min, range_max);
		//    g->Fit(fun,"R+","",min-2*sigma, min+2*sigma);
		//    sigma = 1./sqrt(2* fun->GetParameter(2));
		sigma_minus = 1. / sqrt(2 * f2->GetParameter(2));
		sigma_plus = 1. / sqrt(2 * f2->GetParameter(3));
		//    min=- fun->GetParameter(1) / (2* fun->GetParameter(2));
		minX = f2->GetParameter(1);
		chi2 = fun->GetChisquare() / fun->GetNDF(); //g->Chisquare(fun);

		range_min = TMath::Max(minX - 9 * sigma_minus, xVar->getMin());
		range_max = TMath::Min(minX + 12 * sigma_plus, xVar->getMax());
		//range_min = TMath::Max(minX-2*sigma_minus,0.);
		//  range_max = TMath::Min(minX+2*sigma_plus,0.07);

		while (((range_max - range_min) / X_bin < 10)) {
			// incremento del 10% i range
			std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
			std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
			range_min -= (range_max - range_min) * 0.2;
			range_max += (range_max - range_min) * 0.2;
		}

		f2->SetRange(range_min, range_max);
		//     std::cout << minX_old << " " << minX << " " << minX-minX_old << std::endl;
		if(sigma_plus * 100 < 0.01) std::cout << "sigma_plus = " << " " << sigma_plus << std::endl;
		if(sigma_minus * 100 < 0.01) std::cout << "sigma_minus = " << " " << sigma_minus << std::endl;
		iter++;
		f2->SetFitResult(fitres);

#ifdef DEBUG
		std::cout << "MINUIT STATUS " << fitres.Status() << std::endl; //gMinuit->GetStatus() << std::endl;
		std::cout << "CHI2/NDF" << chi2 << std::endl;
#endif
		//Rescaling graph points so that minimum iteratively goes to 0
		for (int iPoint = 0; iPoint < g->GetN(); ++iPoint)
			*(g->GetY() + iPoint) = *(g->GetY() + iPoint) - f2->GetParameter(0);
		//} While( (((fabs(minX-minX_old)/minX > FIT_LIMIT || fabs(sigma_plus-sigma_plus_old)/sigma_plus>FIT_LIMIT || fabs(sigma_minus-sigma_minus_old)/sigma_minus>FIT_LIMIT )) && iter<20) || iter<10 );
	} while (fabs(chi2 - old_chi2) / old_chi2 > FIT_LIMIT && (iter < ITER_MAX || iter < 4 || fitres.Status() != 0));

	std::cout << "ITER to converge = " << iter << std::endl;
	if(iter >= ITER_MAX) std::cout << "WARNING: iter not converged" << std::endl;

	//  float chi2_rnd = TMath::ChisquareQuantile(gen.Rndm(),1);

	float errorScale = sqrt(f2->GetChisquare() / (f2->GetNumberFitPoints() - f2->GetNumberFreeParameters()));
	std::cout << "SCALING ERROR by " << errorScale;
	for (int i = 0; i < g->GetN(); ++i)
		*(g->GetEY() + i) = (*(g->GetEY() + i)) * errorScale; //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);



	return f2;
}


