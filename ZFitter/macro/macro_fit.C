//#include "./src/SetRegionVec.cc"
#include "RooRealVar.h"
#include <RooArgSet.h>
#include <RooDataHist.h>
#include <RooHistPdf.h>
#include <RooDataSet.h>

#include <iostream>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TString.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <RooRealVar.h>
#include "RooRealVar.h"
#include "RooMinuit.h"
#include "RooBinning.h"
#include "RooFitResult.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TStopwatch.h"

#include "TF1.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TCut.h"
#include <TList.h>
#include <TObject.h>
#include <TKey.h>
#include <TPaveText.h>

#include <iostream>
#include <vector>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>

#include <TRandom3.h>
#include <utility> //std::pair
#include <functional> //std::pair

//#define selectedBase_class_cxx
//#include "RooSmearedHistoCompatibility.h"
//TString filename=outPrefix+"-"+region_set+"-outProfile.root";

#define FIT_LIMIT 0.01

//#define DEBUG
//TRandom3 gen(0);

#define NOT_CONVERGED_LIMIT 0.6

TGraph *SmoothGraph(TGraph *graph)
{
	Double_t *X = graph->GetX();
	Double_t *Y = graph->GetY();

	int N = graph->GetN();
	Double_t x[3000], y[3000]; //, ex[1000]={0}, ey[1000]={0};

	// last point is ignored
	for(int i = 0, ii = 0; i < N - 1; i += 2, ii++) {
		y[ii] = (Y[i] + Y[i + 1]) * 0.5;
		x[ii] = (X[i] + X[i + 1]) * 0.5;
	}
	//  return  new TGraphErrors(N-1, x, y,ex,ey);
	return  new TGraph(N / 2, x, y);
}

void ShiftYGraph(TGraphErrors *graph, double y)
{
	Double_t *Y = graph->GetY();
	int N = graph->GetN();

	for(int i = 0; i < N; i++) {
		Y[i] -= y;
	}
	return;
}


TString fit_img_filename(TString filename, TString prefix, TString region, TString ext = ".eps")
{
	if(filename.Contains("toys")) {
		filename.ReplaceAll(".root", TString("-") + prefix + region + ext);
	} else {
		filename.ReplaceAll("fitres/", "img/");
		filename.ReplaceAll(".root", TString("-") + prefix + region + ext);
	}
	//std::cout << "filename = "<< filename << std::endl;
	return filename;
}

Double_t asymmetricParabola(Double_t* x, Double_t* par)
{
	//if x < par[1]: parabola di una certa apertura;
	//if x > par[1]: parabola di una certa altra apertura;
	Double_t value = 0.;
	if (x[0] <= par[1])
		value = par[2] * (x[0] - par[1]) * (x[0] - par[1]);
	else
		value = par[3] * (x[0] - par[1]) * (x[0] - par[1]);
	return value + par[0];
}

Double_t symmetricParabola(Double_t* x, Double_t* par)
{
	//funzione a una sola variable x (benche' sia un array e 3 parametri
	Double_t value = 0.;
	value = par[2] * (x[0] - par[1]) * (x[0] - par[1]);
	return value + par[0];
}

Double_t likelihoodAsymmetricParabola(Double_t *x, Double_t *par)
{
	return exp(-(asymmetricParabola(x, par)));
}

RooDataSet *getL(Double_t *par, RooRealVar *var1, Long64_t nEvents = 10000)
{
	Double_t range_min = par[1] - 4 / (sqrt(2 * par[2])), range_max = par[1] + 4 / (sqrt(2 * par[3]));
	range_min = std::max(0., range_min);

	TF1* f2 = new TF1("f2", likelihoodAsymmetricParabola, range_min, range_max, 4);
	f2->SetParameter(0, par[0]);
	f2->SetParameter(1, par[1]);
	f2->SetParameter(2, par[2]);
	f2->SetParameter(3, par[3]);
	f2->SetNpx(1e3);

	TH1F *hist = (TH1F*)f2->GetHistogram();

	RooDataHist *dataHist = new RooDataHist(TString(hist->GetName()) + "_dataHist", hist->GetTitle(), RooArgSet(*var1), hist);
	RooHistPdf *histPdf = new RooHistPdf(TString(hist->GetName()) + "_histPdf", hist->GetTitle(), RooArgSet(*var1), *dataHist);

	RooDataSet *data = histPdf->generate(RooArgSet(*var1), nEvents, kTRUE);
	//delete f2;
	delete hist;
	//delete var1;
	delete dataHist;
	delete histPdf;


	return data;
}


void Get_DeltaC_DeltaS(TH1F *DeltaC_hist, TH1F *DeltaS_hist, //to be filled
                       Double_t *rho_par, Double_t *phi_par, Double_t Emean)
{

	Long64_t nEvents = 10000;
	RooRealVar *rho = new RooRealVar("rho", "", 0.1);
	RooDataSet *rho_data = getL(rho_par, rho, nEvents);

	RooRealVar *phi = new RooRealVar("phi", "", 0.1);
	RooDataSet *phi_data = getL(phi_par, phi, nEvents);

	for(Long64_t jentry = 0; jentry < nEvents; jentry++) {
		const RooArgSet *rhoArg = rho_data->get(jentry);
		const RooArgSet *phiArg = phi_data->get(jentry);
		Double_t rhoVal = rhoArg->getRealValue(rho->GetName());
		Double_t phiVal = phiArg->getRealValue(phi->GetName());

		Double_t DeltaC = rhoVal * sin(phiVal);
		Double_t DeltaS = rhoVal * Emean * cos(phiVal);
		if(jentry < 10) std::cout << rhoVal
			                          << "\t"
			                          << phiVal
			                          << "\t"
			                          << DeltaC
			                          << std::endl;

		DeltaC_hist->Fill(DeltaC);
		DeltaS_hist->Fill(DeltaS);
	}

	delete rho_data;
	delete phi_data;
	delete rho;
	delete phi;
	return;

}


int pointsInInterval(Double_t *array, int N, double min, double max)
{
	int count = 0;
	for(int i = 0; i < N; i++)
		if(array[i] < max && array[i] > min) count++;
	return count;
}

double rangeWithPoints(TGraphErrors *g, int nPoints, Double_t *xMin, Double_t* xMax)
{
	g->Sort();
	int iMin = 0;
	Double_t *Y = g->GetY();
	Double_t *X = g->GetX();
	Double_t min = 1e20;
	for(int i = 1; i < g->GetN(); i++) {
		if(Y[i] <= min) {
			iMin = i;
			min = Y[i];
		}
	}

	int _iMax = std::min(g->GetN(), iMin + nPoints);
	int _iMin = std::max(0, iMin - nPoints);

	double dist = std::max(X[_iMax] - X[iMin], X[iMin] - X[_iMin]);
	(*xMin) = X[iMin] - dist / 2;
	(*xMax) = X[iMin] + dist / 2;
	return std::max(Y[_iMin], Y[_iMax]);
}


TF1* IterMinimumFit(TGraphErrors *g, bool isScale, bool isPhi = false)
{
	//grah g has its minimum at zero now. Later you will put shift g -> (g - minimum_Fit)
	int ITER_MAX = 6;
	double rangeLimMax = 1.04;
	double rangeLimMin = 0.96;
	if(!isScale) {
		rangeLimMax = 0.35;
		rangeLimMin = 0.00;
	}
	if(isPhi) {
		rangeLimMin = 1e-10;
		rangeLimMax = 1.58;
	}
	Double_t *X = g->GetX();
	int N = g->GetN();

	//Either you take the graph axis range or your initial range
	double range_min = TMath::Max(TMath::MinElement(g->GetN(), g->GetX()), rangeLimMin);
	double range_max = TMath::Min(TMath::MaxElement(g->GetN(), g->GetX()), rangeLimMax);

	//Adjust the ranges with respect to the position of the minimum
	///if(!isScale){//case constTerm
	int n = g->GetN();
	double* y = g->GetY();
	int locmin = TMath::LocMin(n, y);
	double* x = g->GetX();//x corresponding to y min
	//std::cout<<"adjusting ranges around "<<x[locmin]<<std::endl;
	if(isPhi) {
		range_min = TMath::Max(range_min, x[locmin] - 0.1);
		range_max = TMath::Min(range_max, x[locmin] + 0.1); //larger range on right side
	} else if(!isScale) { //case constTerm
		range_min = TMath::Max(x[locmin] - 0.01, 0.);
		range_max = TMath::Min(range_max, x[locmin] + 0.015); //larger range on right side
	} else {
		range_min = TMath::Max(x[locmin] - 0.01, 0.);
		range_max = TMath::Min(range_max, x[locmin] + 0.01); //symmetric for scale
	}
	///}
	//cout<<"[FIT STATUS]: Preliminary fit to pol2. Range is ["<<range_min<<","<<range_max<<"]"<<endl;
	g->Fit("pol2", "0FR+", "", range_min, range_max); //entire x range has been fitted
	//The idea is to fit with a pol2 and then use the fitted parameters as initialization parameters for the iterative fit to asym parabola
	TF1 *fun = (TF1*) g->GetListOfFunctions()->At(g->GetListOfFunctions()->GetSize() - 1);
	fun->SetRange(range_min, range_max);
	if(fun == NULL) {
		std::cerr << "Error: polynomial fit function not defined" << std::endl;
		return 0;
	}

	fun->GetRange(range_min, range_max);
	double sigma_plus_old;
	double sigma_minus_old;
	double minX_old;
	double sigma = 1. / sqrt(2 * fun->GetParameter(2)); //taken from the pol2 fit
	if(sigma < 1e-5 || sigma > 1e10 || sigma != sigma) sigma = 1e-4;
	double sigma_plus = sigma;
	double sigma_minus = sigma;
	double sigma_mean = 0.;
	double minX = g->GetX()[TMath::LocMin(g->GetN(), g->GetY())];
	if(minX == 0 || minX < rangeLimMin || minX > rangeLimMax) minX = -fun->GetParameter(1) / (2 * fun->GetParameter(2));
	double old_chi2 = 0., chi2 = 0.;

	if(sigma < 0.0001) sigma = 0.0001;
	if(minX < rangeLimMin) minX = rangeLimMin + 0.001;
	std::cout << "[INFO] Init parameters for asym fit------------------------------" << std::endl
	          << "        minX: "  << minX << std::endl
	          << "        sigma: " << sigma << std::endl;

	TF1* f2 = new TF1("f2", asymmetricParabola, range_min, range_max, 4);
	//Initialization parameters for the asymParabola, in the iter you adjust the ranges not these parameters
	f2->SetParameter(0, 0);
	f2->SetParameter(1, minX);
	f2->SetParameter(2, 1 / (2 * sigma * sigma));
	f2->SetParameter(3, 1 / (2 * sigma * sigma));
	f2->SetParLimits(0, -100, 100);
	f2->SetParLimits(1, minX - 2 * sigma, minX + 2 * sigma);
	f2->SetParLimits(2, 0, 1e10);
	f2->SetParLimits(3, 0, 1e10);

	//If you want to change the initialization for some of them
	if(TString(g->GetTitle()).Contains("0_1-gold")) {
		f2->SetParLimits(1, minX - 0.2 * sigma, minX + 0.2 * sigma);
	}

	int iter = 0;
	do { //adjust the ranges and re-fit
		old_chi2 = chi2;
		minX_old = minX;
		sigma_plus_old = sigma_plus;
		sigma_minus_old = sigma_minus;

		if(minX <= rangeLimMin + 1e-9) {
			//std::cout << "[INFO] Fixing min" << std::endl;
			f2->SetParLimits(2, 0, 100);
		}
		if(minX >= rangeLimMax - 1e-9) {
			//std::cout << "[INFO] Fixing max" << std::endl;
			f2->SetParLimits(3, 0, 100);
		}

		TString fname = "f2_iter_";
		fname += iter;
		f2->SetName(fname);
		std::cout << "[FIT STATUS] (isScale,isPhi): (" << isScale << "," << isPhi << ") Iter: " << iter << " : Fit range: [ " << range_min << " : " << range_max << "]" << std::endl;
		f2->SetRange(range_min, range_max);
		g->Fit(f2, "0R+", "", range_min, range_max);
		fun->GetRange(range_min, range_max);
		sigma_minus = 1. / sqrt(2 * f2->GetParameter(2));
		sigma_plus = 1. / sqrt(2 * f2->GetParameter(3));
		minX = f2->GetParameter(1);
		chi2 = fun->GetChisquare() / fun->GetNDF();

		//Setting ranges for next iteration
		if(isScale) { //Scale
			//if(TString(g->GetTitle()).Contains("Et_0_60")){
			//minX=0.99992;//by hand for this category
			//}
			//std::cout<<"[INFO] Adjusting scale range for next iteration"<<std::endl;
			//std::cout<<"[INFO] minX is "<<minX<<std::endl;
			//std::cout<<"[INFO] sigma_minus is "<<sigma_minus<<std::endl;
			//std::cout<<"[INFO] sigma_plus is "<<sigma_plus<<std::endl;
			range_min = TMath::Max(minX - 8 * sigma_minus, rangeLimMin); //15
			range_max = TMath::Min(minX + 8 * sigma_plus, rangeLimMax); //10
			if((sigma_plus / sigma_minus) > 4 || (sigma_plus / sigma_minus) < 0.25) { //if sigmas are too different
				std::cout << "[INFO] sigma_plus and minus are too different: Taking the mean" << std::endl;
				sigma_mean = (sigma_plus + sigma_minus) / 2;
				range_min = TMath::Max(minX - 4 * sigma_mean, rangeLimMin);
				range_max = TMath::Min(minX + 4 * sigma_mean, rangeLimMax);
			}
			//if((TString(g->GetTitle()).Contains("EB-Et_0_100-gainEle_12-eleID_tight-Et_25-noPF-Et_20"))){
		} else if(isPhi) {
			std::cout << "you are fitting isPhi" << std::endl;
			range_min = TMath::Max(minX - 2 * sigma_minus, rangeLimMin);
			range_max = TMath::Min(minX + 3 * sigma_plus,  rangeLimMax);
			if(minX < 0.5) {
				if(!TString(g->GetTitle()).Contains("1_1.4442-gold")) range_max = 0.5;
				if(TString(g->GetTitle()).Contains("0_1-bad")) {
					range_min = 0;
					range_max = 0.3;
				}
				if(TString(g->GetTitle()).Contains("0_1-gold")) range_max = 0.4;
			}
			if(minX > 1.2) {
				if(TString(g->GetTitle()).Contains("0_1-bad")) {
					range_min = 0.5;
					range_max = 1.6;
				};
			}
		} else {//sigma
			range_min = TMath::Max(minX - 5 * sigma_minus, rangeLimMin); //5
			range_max = TMath::Min(minX + 5 * sigma_plus,  rangeLimMax); //5
			if((sigma_minus) > 0.04) { //likelihood is flat
				std::cout << "[INFO] sigma_minus is too big --> taking sigma_plus" << std::endl;
				sigma_mean = sigma_plus;
				range_min = TMath::Max(minX - 4 * sigma_mean, rangeLimMin);
				range_max = TMath::Min(minX + 4 * sigma_mean, rangeLimMax);
			}
		}

		//Points in interval
		//std::cout << "[INFO] Points in interval: " << pointsInInterval(X, N, range_min, range_max) << std::endl;
		//if(!(TString(g->GetTitle()).Contains("0_1-bad"))){
		//  while (pointsInInterval(X, N, range_min, range_max) < 10 && range_min>rangeLimMin && range_max<rangeLimMax){
		//	// incremento del 10% i range
		//	std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
		//	//std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
		//	range_min = TMath::Max(range_min-(rangeLimMax-rangeLimMin)*0.05, rangeLimMin);
		//	range_max = TMath::Min(range_max+(rangeLimMax-rangeLimMin)*0.05, rangeLimMax);
		//  }
		//  while (pointsInInterval(X, N, minX, range_max) < 10 && range_max<rangeLimMax){
		//	// incremento del 10% i range
		//	std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
		//	//std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
		//	range_max = TMath::Min(range_max+(rangeLimMax-rangeLimMin)*0.05, rangeLimMax);
		//  }
		//}

		iter++;
#ifdef DEBUG
		std::cout << "MINUIT STATUS " << gMinuit->GetStatus() << std::endl;
		std::cout << "CHI2/NDF " << chi2 << std::endl;
#endif
	} while ((fabs(chi2 - old_chi2) / old_chi2 > FIT_LIMIT) && (iter < ITER_MAX)); //while (iter<1); //while ((fabs(chi2 - old_chi2)/old_chi2 > FIT_LIMIT) && (iter < ITER_MAX || iter < 4));;

	std::cout << "ITER to converge = " << iter << std::endl;
	if(iter >= ITER_MAX) std::cout << "WARNING: maximum iter reached" << std::endl;
	//  float chi2_rnd = TMath::ChisquareQuantile(gen.Rndm(),1);

	if(gMinuit->GetStatus() == 0) {
#ifdef DEBUG
		std::cout <<  f2->GetChisquare() << "\t" << f2->GetNumberFitPoints() << "\t" << f2->GetNumberFreeParameters() << std::endl;
#endif

		//float errorScale=sqrt(f2->GetChisquare()/(f2->GetNumberFitPoints()-f2->GetNumberFreeParameters()));
		//std::cout << "SCALING ERROR by " << errorScale  << std::endl//NOT DONE ANYMORE
		//std::cout<< "Shifting y axis by " << f2->GetParameter(0) << std::endl;
		if(g->GetEY() != NULL) {
			for (int i = 0; i < g->GetN(); ++i) {
				//*(g->GetEY()+i)= (*(g->GetEY()+i)) * errorScale; //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);//NOT DONE ANYMORE
				// rigid shift along Y axis to have at DeltaNLL=0 the minimum of the fit
				*(g->GetY() + i) -= f2->GetParameter(0); //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);
			}
			f2->SetParameter(0, 0);
		}

	}

	return f2;
}

typedef struct {
	double y;
	double y2;
	int n;
} point_value_t;

typedef  std::map<Double_t, point_value_t> graphic_t;

TGraphErrors *SumGraph(std::vector<TGraph *> g_vec, bool shift = false)
{
	int N = (*g_vec.begin())->GetN(); // number of points
	TString name = (*g_vec.begin())->GetName();
	if(8000 <= N) {
		std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
		return NULL;
	}


	Double_t x[20000] = {0.}, y[20000] = {0.}, y2[20000] = {0.}, dev_st[20000], err_x[20000];
	int n = 0;
	for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
	        g_itr != g_vec.end();
	        g_itr++) { // loop over TGraphs

		TGraph *graph = *g_itr;
		if(graph == NULL) continue; // per sicurezza
		Double_t *Y = graph->GetY();
		Double_t *X = graph->GetX();
		N = graph->GetN();

		double minimum = Y[0];
		for(int i = 0; i < N; i++) { // calcolo il minimo e il punto di minimo
			if(minimum > Y[i]) minimum = Y[i];
		}

		//devo evitare punti con la stessa x fatti dallo stesso scan perche' a causa del deterministic smearing
		//hanno per costruzione la stessa likelihood

		std::set<double> uniqX;
		for (int ip = 0; ip < N; ++ip) { //sottraggo il minimo per avere un DeltaL in ordinata
			if((uniqX.insert(X[ip])).second == false) {
				//std::cout << "[INFO] Point already included: " << X[ip] << std::endl;
				continue;
			}
			x[n] = X[ip];

			if(shift) y[n] += Y[ip] - minimum;
			else  y[n] += Y[ip];

			y2[n] += y[n] * y[n];
			//	std::cout << ip+n << "\t" << x[ip+n] << "\t" << y[ip+n] << "\t" << Y[ip+n] << std::endl;
			err_x[n] = 0;
			dev_st[n]++; // used to count the points per bin
			if(n > 19999) {
				std::cerr << "############################################################" << std::endl;
				std::cerr << "############################################################" << std::endl;
				std::cerr << "############################################################" << std::endl;
				break;
			}
			n++;
		}
		//n+=N;

		if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;
	}

	for (int ip = 0; ip < n; ++ip) {
		//   y2[ip]/=dev_st[ip];
		//    y[ip]/=dev_st[ip];
		//    if(dev_st[ip]!=1) dev_st[ip]=y2[ip]-y[ip]*y[ip];
		//    else
		dev_st[ip] = 0;
	}
	std::cout << "[INFO] n = " << n << std::endl;
	TGraphErrors *g = new TGraphErrors(n, x, y, err_x, dev_st);

	return g;
}

TGraphErrors *MeanGraphNew(TGraphErrors *graph)
{
	std::cout << "Calling macro_fit.C/MeanGraphNew" << std::endl;
	int N = graph->GetN(); // number of points
	if(17000 <= N) {
		std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
		return NULL;
	}

	std::map<Double_t, std::pair<Double_t, Double_t> > g_in_map;

	Double_t x[17000] = {0.}, y[17000] = {0.}, y2[17000] = {0.}, dev_st[17000], err_x[17000];

	graphic_t graphic, graphic2;

	int n_max = 0;

	double y_min = 1e20;

	if(graph == NULL) return NULL;
	if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;

	Double_t *Y = graph->GetY();
	Double_t *X = graph->GetX();
	Double_t X_bin = 0.00001; //fabs(X[1]-X[0]);
	//    double minimum=TMath::MinElement(graph->GetN(),graph->GetY());
	double minimum = 1e20;
	double x_minimum = X[0];
	for(int i = 1; i < graph->GetN(); i++) { // calcolo il minimo e il punto di minimo
		if(minimum > Y[i]) {
			minimum = Y[i];
			x_minimum = X[i];
		}
	}

	//subtract the minimum?
	for(int i = 1; i < graph->GetN(); i++) { // calcolo il minimo e il punto di minimo
		Y[i] -= minimum;
	}

	for(int i = 0 ; i < graph->GetN(); i++) {
		graphic_t::iterator itr = graphic.find(X[i]);
		if(!graphic.empty() && itr == graphic.end()) {
			// controllo se e' vicino al primo bordo, poi al secondo e altrimenti lo butto
			itr = graphic.lower_bound(X[i]);
			//	  if(itr== graphic.end()) std::cout << "Error: graphic empty" << std::endl;
			if(itr != graphic.begin()) itr--;
			if( (fabs(itr->first - X[i]) > 0.00000001)) { // check del bin prima
				itr++;
				if( itr != graphic.end() && (fabs(itr->first - X[i]) > 0.00000001)) { // check del bin
					itr++;
					if( itr != graphic.end() && (fabs(itr->first - X[i]) > 0.00000001)) { // check del bin dopo
						itr = graphic.end();
					} //else std::cout << "[DEBUG] New point: " << itr->first  << "\t" << X[i] << std::endl;
				} //else std::cout << "[DEBUG] moving to upper bound: " << itr->first  << "\t" << X[i] << std::endl;
			} //else std::cout << "[DEBUG] moving to lower bound: " << itr->first  << "\t" << X[i] << std::endl;
		}


		if(itr != graphic.end()) {
			itr->second.y += Y[i];
			itr->second.y2 += Y[i] * Y[i];
			itr->second.n++;
			n_max = std::max(n_max, itr->second.n);
		} else {
			point_value_t p;
			p.y = Y[i];
			p.y2 = Y[i] * Y[i];
			p.n = 1;
			//graphic.insert(std::make_pair<Double_t, point_value_t>(X[i],p));
			graphic.insert(std::make_pair(X[i], p));
			n_max = std::max(n_max, 1);
		}
	}

	/*-----------------------------*/
	//  std::cout << "N_MAX = " << n_max << std::endl;
	int n = 0;
	y_min = 1e10;
	for(graphic_t::iterator itr = graphic.begin(); itr != graphic.end(); itr++) {

		if(itr->second.n < n_max * 0.30) {
			std::cout << "Removing point " << itr->first << "\t" << itr->second.n << "\t" << n_max << std::endl;
			continue;
		}
		double ym = itr->second.y / itr->second.n;
		if(itr->second.n > 1 && itr->second.y2 / itr->second.n >= ym * ym)
			dev_st[n] =  sqrt( (itr->second.y2 / itr->second.n - ym * ym) / (double)(itr->second.n)); //errore sulla media
		else {
			std::cout << itr->first << "\t" << itr->second.y << "\t" << itr->second.n
			          << "\t" << itr->second.y2 / itr->second.n << "\t" << ym*ym
			          << std::endl;
			dev_st[n] =  1;
			//continue;
		}
		x[n] = itr->first;
		y[n] = ym;
		err_x[n] = 0;

		y_min = std::min(y_min, y[n]);
//     std::cout << graph->GetName() << "\t" << graph->GetTitle() << x[n] << " "
//     	      << y[n] << " "
//     	      << dev_st[n] << " "
//     	      << itr->second.y2/itr->second.n << " " << ym*ym   << " "
// 	      << itr->second.n
//     	      << std::endl;
		n++;

	}

	for(int i = 0; i < n; i++) { // calcolo il minimo e il punto di minimo
		y[i] -= y_min;
	}

	TGraphErrors *g = new TGraphErrors(n, x, y, err_x, dev_st);
	g->SetTitle(graph->GetTitle());
	g->SetName(graph->GetTitle());
	//  g->Draw("AP*");
	return g;
}

TGraphErrors *MeanGraph2(std::vector<TGraph *> g_vec, float *y_minimum = NULL, TGraphErrors *g_in = NULL)
{

	TGraphErrors *newGraph = new TGraphErrors;

	Double_t x[20000] = {0.}, y[20000] = {0.}, y2[20000] = {0.}, dev_st[20000], err_x[20000];
	int n = 0;
	for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
	        g_itr != g_vec.end();
	        g_itr++) { // loop over TGraphs

		TGraph *graph = *g_itr;
		if(graph == NULL) continue; // per sicurezza
		Double_t *Y = graph->GetY();
		Double_t *X = graph->GetX();
		Int_t N = graph->GetN();

		double minimum = Y[0];
		for(int i = 0; i < N; i++) { // calcolo il minimo e il punto di minimo
			if(minimum > Y[i]) minimum = Y[i];
		}

		//    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
		for (int ip = 0; ip < N; ++ip) { //sottraggo il minimo per avere un DeltaL in ordinata
			newGraph->SetPoint(ip + n, X[ip], Y[ip]);
		}
		n += N;
	}
	newGraph->Set(n);
	TGraphErrors *returnGraph = MeanGraphNew(newGraph);
	delete newGraph;
	return returnGraph;
}

TGraphErrors *MeanGraph(std::vector<TGraph *> g_vec, float *y_minimum = NULL, TGraphErrors *g_in = NULL)
{
	int N = (*g_vec.begin())->GetN(); // number of points
	TString name = (*g_vec.begin())->GetName();
	if(8000 <= N) {
		std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
		return NULL;
	}


	Double_t x[20000] = {0.}, y[20000] = {0.}, y2[20000] = {0.}, dev_st[20000], err_x[20000];
	int n = 0;
	for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
	        g_itr != g_vec.end();
	        g_itr++) { // loop over TGraphs

		TGraph *graph = *g_itr;
		if(graph == NULL) continue; // per sicurezza
		Double_t *Y = graph->GetY();
		Double_t *X = graph->GetX();

		double minimum = Y[0];
		for(int i = 0; i < N; i++) { // calcolo il minimo e il punto di minimo
			if(minimum > Y[i]) minimum = Y[i];
		}

		//    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
		for (int ip = 0; ip < N; ++ip) { //sottraggo il minimo per avere un DeltaL in ordinata
			if(ip != 0 && 	x[ip] != X[ip]) {
				std::cerr << "[ERROR] Graphs have different bin positions!" << std::endl;
				exit(1);
			}
			x[ip] = X[ip];
			if(Y[ip] < 10) {
				//std::cout << Y[ip] << std::endl;
				continue;
			}
			y[ip] += Y[ip]; //-minimum;
			y2[ip] += Y[ip] * Y[ip];
			//	std::cout << ip+n << "\t" << x[ip+n] << "\t" << y[ip+n] << "\t" << Y[ip+n] << std::endl;
			err_x[ip] = 0;
			dev_st[ip]++; // used to count the points per bin

			if(n > 19999) {
				std::cerr << "############################################################" << std::endl;
				std::cerr << "############################################################" << std::endl;
				std::cerr << "############################################################" << std::endl;
				break;
			}

		}
		if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;
	}

	for (int ip = 0; ip < N; ++ip) {
		if(dev_st[ip] != 0) {
			y2[ip] /= dev_st[ip];
			y[ip] /= dev_st[ip];
			if(dev_st[ip] > 1) {
				//std::cout << y2[ip] << "\t" << y[ip]*y[ip] << "\t" << y[ip] << "\t" << dev_st[ip] << std::endl;
				dev_st[ip] = sqrt(y2[ip] - y[ip] * y[ip]) / sqrt(dev_st[ip]);
			} else dev_st[ip] = 0;
		}
	}

	// calcolo il minimo e il punto di minimo
	double minimum = 1e20;
	for(int i = 2; i < N; i++) if(minimum > y[i] && y[i] > 10) minimum = y[i];
	for(int i = 0; i < N; i++) y[i] -= minimum;



	TGraphErrors *g = new TGraphErrors(N, x, y, err_x, dev_st);
	//  g->Draw("AP*");
	return g;
}


TGraphErrors *MeanGraphOld(std::vector<TGraph *> g_vec, float *y_minimum = NULL, TGraphErrors *g_in = NULL)
{

	int N = (*g_vec.begin())->GetN(); // number of points
	if(3000 <= N) {
		std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
		return NULL;
	}

	std::map<Double_t, std::pair<Double_t, Double_t> > g_in_map;

	Double_t x[3000] = {0.}, y[3000] = {0.}, y2[3000] = {0.}, dev_st[3000], err_x[3000];

	graphic_t graphic;

	int n_max = 0;

	double y_min = 1e20;

	for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
	        g_itr != g_vec.end();
	        g_itr++) { // loop over TGraphs

		graphic_t graphs;
		TGraph *graph = *g_itr;
		if(graph == NULL) continue; // per sicurezza
		if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;

		Double_t *Y = graph->GetY();
		Double_t *X = graph->GetX();
		Double_t X_bin = 0.00001; //fabs(X[1]-X[0]);
		//    double minimum=TMath::MinElement(graph->GetN(),graph->GetY());
		double minimum = 1e20;
		double x_minimum = X[0];
		for(int i = 1; i < graph->GetN(); i++) { // calcolo il minimo e il punto di minimo
			if(minimum > Y[i]) {
				//std::cout << minimum << "\t" << Y[i] << std::endl;
				minimum = Y[i];
				x_minimum = X[i];
			}
			//if(fabs(X[i]-X[i-1])<X_bin) X_bin=fabs(X[i]-X[i-1]); // update the minimum bin size
		}

//     // cerco di capire se il fit di migrad non e' andato a convergenza
//     if(fabs(x_minimum - (X[graph->GetN()-1] + X[0])/2.) > NOT_CONVERGED_LIMIT * (X[graph->GetN()-1] - X[0])){ // shift 10% rispetto all'intervallo dal minimo di migrad
//       std::cout << "[WARNING] Graph not converged" << std::endl;
//       std::cout << X[1] << "\t" << X[graph->GetN()-1] << "\t" << x_minimum << "\t" << X_bin << std::endl;
//       continue; // passo al graph successivo
//     }

		//    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
		for (int ip = 0; ip < graph->GetN(); ++ip) { //sottraggo il minimo per avere un DeltaL in ordinata
			Y[ip] -= minimum;
			//	std::cout << ip << ";" << X[ip] << ";" << Y[ip] << std::endl;
		}

		//    std::cout << "XBIN = " << X_bin << std::endl;
		//    std::cout << "Y[7] = " << Y[7] << std::endl;


		for(int i = 0 ; i < graph->GetN(); i += 3) {
			if(graphs.count(X[i]) == 0) {
				point_value_t p;
				p.y = Y[i];
				p.y2 = Y[i] * Y[i];
				p.n = 1;
				//graphs.insert(std::make_pair<Double_t, point_value_t>(X[i],p));
				graphs.insert(std::make_pair(X[i], p));
			} else {
				//std::cout << X[i] << "\t" << Y[i] << std::endl;
			}
		}

		for(graphic_t::iterator gitr = graphs.begin(); gitr != graphs.end(); gitr++) {

			graphic_t::iterator itr = graphic.find(gitr->first);

			if(!graphic.empty()) {

				if(itr == graphic.end()) {
					// controllo se e' vicino al primo bordo, poi al secondo e altrimenti lo butto
					itr = graphic.lower_bound(gitr->first);
					//	  if(itr== graphic.end()) std::cout << "Error: graphic empty" << std::endl;
					itr--;
					if( !(fabs(itr->first - gitr->first) < X_bin / 5.)) { // check del bin prima
						itr++;
						if( !(fabs(itr->first - gitr->first) < X_bin / 5.)) { // check del bin
							itr++;
							if( !(fabs(itr->first - gitr->first) < X_bin / 5.)) { // check del bin dopo
#ifdef DEBUG
								if(gitr->first < graphic.begin()->first) {
									std::cout << "Value not found: " << gitr->first << "; "
									          << graphic.lower_bound(gitr->first)->first << "; "
									          << graphic.upper_bound(gitr->first)->first << "; "
									          << X_bin / 5.
									          <<	      std::endl;
								}
#endif
								itr = graphic.end();
							}
						}
					}
				}
			} else itr = graphic.end();


			if(itr != graphic.end()) {
				itr->second.y += gitr->second.y;
				itr->second.y2 += gitr->second.y * gitr->second.y;
				itr->second.n++;
				n_max = std::max(n_max, itr->second.n);
				//	if(itr->second.n>100) std::cout << "added point with n>100\t" << i << "\t" << X[i] << "\t" << gitr->second.y << "\t" << itr->second.n << "\t" << n_max << std::endl;
			} else {
				//if(g_itr!=g_vec.begin())	std::cout << "added point" <<  << "\t" << gitr->second.y << "\t" << std::endl;
				// se non c'e' lo aggiungo
				point_value_t p;
				p.y = gitr->second.y;
				p.y2 = gitr->second.y * gitr->second.y;
				p.n = 1;
				n_max = std::max(n_max, 1);
				//graphic.insert(std::make_pair<Double_t, point_value_t>(gitr->first,p));
				graphic.insert(std::make_pair(gitr->first, p));
				//	continue; // se non c'e' lo salto
			}
			//      }

		}
	}



	/*-----------------------------*/
	std::cout << "N_MAX = " << n_max << std::endl;
	int n = 0;
	y_min = 1e10;
	for(graphic_t::iterator itr = graphic.begin(); itr != graphic.end(); itr++) {

		//    if(itr->second.n < n_max-15) continue;
		if(itr->second.n < n_max * 0.2) {
			std::cout << "Removing point " << itr->first << "\t" << itr->second.n << std::endl;
			continue;
		}
		//if(itr->second.n != n_max) std::cout << itr->first << "\tn = " << itr->second.n << " " << n_max << std::endl;
		//    std::cout << itr->first << " " << itr->second.n << " " << itr->second.y << " " << itr->second.y2 << std::endl;
		x[n] = itr->first;
		double ym = itr->second.y / itr->second.n;
		y[n] = ym;
		dev_st[n] = sqrt( (itr->second.y2 / itr->second.n - ym * ym) / (double)(itr->second.n)); //errore sulla media
		err_x[n] = 0;

		y_min = std::min(y_min, y[n]);
		//     std::cout << x[n] << " "
		// 	      << y[n] << " "
		// 	      << dev_st[n] << " "
		// 	      << itr->second.y2/itr->second.n << " " << ym*ym
		// 	      << std::endl;
		n++;

	}

	// metto il minimo a 0
	for(int i = 0 ; i < n ; i++) {
		y[i] -= y_min;
	}

	if(y_minimum != NULL) *y_minimum = y_min;
	TGraphErrors *g = new TGraphErrors(n, x, y, err_x, dev_st);
	//  g->Draw("AP*");
	return g;
}


TH1F *Plot_distr(std::vector<TGraph *> g_vec, int i_bin = 21, int n_bin = 20)
{

	if(gROOT->FindObject("d") != NULL) delete gROOT->FindObject("d");


	//  int N = g_vec.size(); // number of points

	//  double y=0, y2=0, dev_st;
	double y_min = (g_vec[0]->GetY())[i_bin] , y_max = (g_vec[0]->GetY())[i_bin];


	for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
	        g_itr != g_vec.end();
	        g_itr++) { // loop over TGraphs

		TGraph *graph = *g_itr;
		Double_t *Y = graph->GetY();
		//    Double_t *X = graph->GetX();
		//    Double_t X_bin=fabs(X[1]-X[0])/2.;

		//    y+=Y[i_bin];
		//    y2+=Y[i_bin]*Y[i_bin];
		if(y_min > Y[i_bin]) y_min = Y[i_bin];
		if(y_max < Y[i_bin]) y_max = Y[i_bin];
	}
	//  y/=N; //calcolo della media
	//  y2/=N; // y2 medio
	//  dev_st = y2 - y*y;

	TH1F *h = new TH1F("d", "", n_bin, y_min, y_max);
	for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
	        g_itr != g_vec.end();
	        g_itr++) { // loop over TGraphs

		TGraph *graph = *g_itr;
		Double_t *Y = graph->GetY();
		//    Double_t *X = graph->GetX();
		//    Double_t X_bin=fabs(X[1]-X[0])/2.;

		//    y+=Y[i_bin];
		//    y2+=Y[i_bin]*Y[i_bin];
		h->Fill(Y[i_bin]);

	}
	std::cout << "i_bin = " << i_bin << std::endl;
	std::cout << "Entries = " << h->GetEntries() << std::endl;
	std::cout << "Mean = " << h->GetMean() << std::endl;
	std::cout << "RMS = " << h->GetRMS() << std::endl;
	return h;
}


std::vector<TGraph *> GetGraphVector(int nToys, TString region, TFile *f)
{
	std::vector<TGraph *> g_vec;

	for(int i = 0; i < nToys; i++) {
		TString name = "profileChi2_scale-" + region + "-";
		name += i;
		//      std::cout << "name = " << name << std::endl;
		g_vec.push_back((TGraph*)f->Get(name));
		if(g_vec[i] == NULL) {
			std::cout << "Graph " << name << "not found." << std::endl;
			g_vec.pop_back();
			//      	return;
		}
	}
	return g_vec;
}


void Plot(TCanvas *c, TGraphErrors *g, TF1 *fun, TPaveText *pt, bool isScale, bool isPhi, TString variable, double delta_min = 2, double delta_max = 2)
{
	//cout<<"[STATUS] Calling Plot of macro_fit.C"<<endl;
	//cout<<"[STATUS] You are plotting "<<g->GetTitle()<<endl;

	double func_min;
	double func_max;
	fun->GetRange(func_min, func_max);
	//std::cout<<"just before return "<<my_min<<" "<<my_max<<std::endl;

	//so, with the name of g you can change the plot style for a specific g
	gStyle->SetOptTitle(0);
	//std::cout<<"fun->GetParameter(2) "<<fun->GetParameter(2)<<std::endl;
	//std::cout<<"fun->GetParameter(3) "<<fun->GetParameter(3)<<std::endl;
	float sigma = 1. / (sqrt(2 * std::min(fun->GetParameter(2), fun->GetParameter(3)))); //asym Parabola
	if(sigma > 5) {
		sigma = 0.0015;
	}
	//cout<<"Inside macro_fit:: Plot sigma is "<<sigma<<endl;
	c->Clear();

	if(isScale) {//scale
		Double_t xMin, xMax, yMin;
		//Double_t yMax = rangeWithPoints(g, 3, &xMin, &xMax);//useless
		xMin = 0.96;
		xMax = 1.04;
		//xMin=std::max(fun->GetMinimumX()-7*sigma,0.96);//4*sigma
		//xMax=std::min(fun->GetMinimumX()+7*sigma,1.04);
		if(delta_min > 1) {
			xMin = std::max(fun->GetMinimumX() - delta_min * (fun->GetMinimumX() - func_min), 0.96);
			xMax = std::min(fun->GetMinimumX() + delta_max * (func_max - fun->GetMinimumX()), 1.04);
		} else {
			xMin = delta_min;
			xMax = delta_max;
		}

		//xMin=1.00;//by hand
		//xMax=1.02;//
		yMin = TMath::MinElement(g->GetN(), g->GetY());

		std::cout << "In Plot: range x is [" << xMin << "," << xMax << "]" << std::endl;
		g->GetXaxis()->SetRangeUser(xMin, xMax);
		//Settare la x in un grafico NON cambia i range in Y
		int kmin = -1;
		for(kmin = 0; kmin < g->GetN(); kmin++) {
			if(g->GetX()[kmin] > xMin) { //punto in x subito dopo xMin
				break;
			}
		}
		int kmax = -1;
		for(kmax = g->GetN() - 1; kmax > 0; kmax--) {
			if(g->GetX()[kmax] < xMax) { //punto in x subito prima xMax
				break;
			}
		}
		double ymax_set = 0.;
		///ymax_set=std::max(fun->Eval(xMin),fun->Eval(xMax));
		//cout<<fun->Eval(xMin)<<endl;
		//cout<<fun->Eval(xMax)<<endl;
		//cout<<g->GetY()[kmin]<<endl;
		//cout<<g->GetY()[kmax]<<endl;
		///ymax_set=std::max(ymax_set,g->GetY()[kmin]);
		///ymax_set=std::max(ymax_set,g->GetY()[kmax]);
		ymax_set = std::max(g->GetY()[kmin], g->GetY()[kmax]);
		g->GetYaxis()->SetRangeUser(yMin - 1, ymax_set);
		//g->GetYaxis()->SetRangeUser(yMin -1,300);//non meno di 200 pero'
		std::cout << "In Plot: range y is [" << yMin - 1 << "," << ymax_set << "]" << std::endl;
		g->GetYaxis()->SetTitle("NLL (a. u.)");
		g->GetXaxis()->SetTitle("1+#DeltaP");
		g->Draw("AP");
		TLine *line_sigma = new TLine(xMin, 0.5, xMax, 0.5);
		line_sigma->SetLineWidth(2);
		line_sigma->Draw();
	} else { //not scale; constTerm
		double xMin = 0.0;
		double xMax = 0.045;
		if(delta_min > 1) {
			xMin = std::max(0., fun->GetMinimumX() - delta_min * (fun->GetMinimumX() - func_min));
			xMax = std::min(fun->GetMinimumX() + delta_max * (func_max - fun->GetMinimumX()), 0.045); //4
		} else {
			xMin = delta_min;
			xMax = delta_max;
		}
		//xMin=0.0;//by hand
		//xMax=0.035;//by hand
		double yMin = TMath::MinElement(g->GetN(), g->GetY());
		double yMax = TMath::MaxElement(g->GetN(), g->GetY());

		std::cout << "In Plot: range x is [" << xMin << "," << xMax << "]" << std::endl;
		g->GetXaxis()->SetRangeUser(xMin, xMax);
		//Settare la x in un grafico NON cambia i range in Y
		int kmin = -1;
		for(kmin = 0; kmin < g->GetN(); kmin++) {
			if(g->GetX()[kmin] > xMin) { //punto in x subito dopo xMin
				break;
			}
		}
		int kmax = -1;
		for(kmax = g->GetN() - 1; kmax > 0; kmax--) {
			if(g->GetX()[kmax] < xMax) { //punto in x subito prima xMax
				break;
			}
		}

		//double ymax_set=std::max(fun->Eval(xMin),fun->Eval(xMax));
		//ymax_set=std::max(ymax_set,g->GetY()[kmin]);
		//ymax_set=std::max(ymax_set,g->GetY()[kmax]);

		double ymax_set = std::max(g->GetY()[kmin], g->GetY()[kmax]);

		std::cout << "In Plot: range y is [" << yMin - 1 << "," << ymax_set << "]" << std::endl;
		g->GetYaxis()->SetRangeUser(yMin - 1, ymax_set);

		Double_t f_minx, f_miny, f_maxx, f_maxy;
		fun->GetRange(f_minx, f_maxx);
		f_miny = fun->Eval(f_minx);
		f_maxy = fun->Eval(f_maxx);

		g->GetXaxis()->SetRangeUser(f_minx, f_maxx);
		g->GetYaxis()->SetRangeUser(yMin - 1, TMath::Min(TMath::Max(f_maxy, f_miny), TMath::MaxElement(g->GetN(), g->GetY())));
		std::cout << "[PETER RANGE] " << TString::Format("x(%.3f,%.3f) y(%.3f,(%.3f,%.3f,%.3f))", f_minx, f_maxx, yMin - 1, f_maxy, f_miny, TMath::MaxElement(g->GetN(), g->GetY())) << std::endl;
		//double yMin=TMath::MinElement(g->GetN(),g->GetY());
		//double xMin=std::max(fun->GetMinimumX()-4.5*sigma,0.);
		//double xMax= std::min(fun->GetMinimumX()+4.5*sigma,0.045);
		//double xMin=0;
		//double xMax=0.05;
		//std::cout<<"fun->GetXmin()-4.5*sigma "<<fun->GetMinimumX()-4.5*sigma<<std::endl;
		//std::cout<<"fun->GetXmin()+4.5*sigma "<<fun->GetMinimumX()+4.5*sigma<<std::endl;
		//std::cout<<"In Plot: range x is ["<<xMin<<","<<xMax<<"]"<<std::endl;
		//std::cout<<"fun->GetXmin() "<<fun->GetMinimumX()<<std::endl;
		//g->GetXaxis()->SetRangeUser(xMin,xMax);
		//g->GetYaxis()->SetRangeUser(yMin-1, std::max(fun->Eval(xMin), fun->Eval(xMax)));
		//g->GetYaxis()->SetRangeUser(yMin-1,300);
		//cout<<"yMax is "<<std::max(fun->Eval(xMin),fun->Eval(xMax))<<endl;
		//cout<<"fun a xMin is "<<fun->Eval(xMin)<<endl;
		//cout<<"fun a xMax is "<<fun->Eval(xMax)<<endl;
		//g->GetYaxis()->SetRangeUser(yMin-5, std::max(fun->Eval(fun->GetXmin()-2.5*sigma), fun->Eval(fun->GetXmin()+2.5*sigma)));
		g->Draw("AP");
		TLine *line_sigma = new TLine(xMin, 0.5, xMax, 0.5);
		line_sigma->SetLineWidth(2);
		line_sigma->Draw();
		g->GetYaxis()->SetTitle("NLL (a. u.)");
		if(variable.Contains("constTerm")) g->GetXaxis()->SetTitle("#Delta C");
		if(variable.Contains("alpha")) g->GetXaxis()->SetTitle("#Delta S");
		if(variable.Contains("rho")) g->GetXaxis()->SetTitle("#rho");
		if(variable.Contains("phi")) {
			g->GetXaxis()->SetTitle("#phi");
			//g->GetXaxis()->SetRangeUser(0,1.6);
		}
	}

	fun->SetLineWidth(3);
	fun->SetLineColor(4);
	fun->Draw("same");

	char out_char[100];

	//Case asymm parabola

	if(isScale) {
		sprintf(out_char, "%s #DeltaP = %.3f ^{+ %.3f}_{- %.3f} %s" , "", //region.Data(),
		        (fun->GetParameter(1) - 1 ) * 100 ,
		        100. / (sqrt(2 * fun->GetParameter(3))), 100. / (sqrt(2 * fun->GetParameter(2))) , "%");
	} else if(isPhi) {
		sprintf(out_char, "%s #phi = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(),
		        fun->GetParameter(1),
		        1. / (sqrt(2 * fun->GetParameter(3))), 1. / (sqrt(2 * fun->GetParameter(2))), "");
		//std::cout << out_char << std::endl;
	} else {
		sprintf(out_char, "%s #Delta#sigma = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(),
		        fun->GetParameter(1) * 100,
		        1. / (sqrt(2 * fun->GetParameter(3))) * 100, 1. / (sqrt(2 * fun->GetParameter(2))) * 100, "%");
		//std::cout << out_char << std::endl;
	}

	pt->Clear();
	pt->AddText(out_char);
	pt->Draw();

	// Do inset plot
	//if(isPhi && variable.Contains("phi"))
	{
		double yMin = TMath::MinElement(g->GetN(), g->GetY());
		double yMax = TMath::MaxElement(g->GetN(), g->GetY());
		double xMin = TMath::MinElement(g->GetN(), g->GetX());
		double xMax = TMath::MaxElement(g->GetN(), g->GetX());
		TGraphErrors * gclone = (TGraphErrors*) g->Clone();
		gclone->GetXaxis()->SetRangeUser(xMin, xMax);
		gclone->GetYaxis()->SetRangeUser(yMin, yMax);
		TPad * subpad = new TPad("inset", "", 0.7, 0.6, 1, 0.9);
		subpad->Draw();
		subpad->cd();
		gclone->Draw("AP");
		fun->Draw("same");
		c->cd();
	}
	return;
}



void FitProfile2(TString filename, TString energy = "13 TeV", TString lumi = "", bool doScale = true, bool doResolution = true, bool doPhi = false, TString singleRegion = "All", TString variable_to_fit = "All", double delta_min = 2., double delta_max = 2.)
{
	gStyle->SetOptFit(0);
	std::ofstream ffout(fit_img_filename(filename, "FitResult", "", ".dat"));
	typedef std::vector<TGraph *> g_vec_t;
	typedef  std::map<TString, g_vec_t> map_region_t;
	std::map<TString, map_region_t> graph_map;
	//std::cout << "Opening file: " << filename << std::endl;
	TFile f_in(filename, "open");
	if(!f_in.IsOpen()) {
		std::cerr << "Error. File not open" << std::endl;
		return;
	}

	TList *KeyList = f_in.GetListOfKeys();
	std::cout << "[FILE] " << filename << std::endl;
	for(int i = 0; i <  KeyList->GetEntries(); i++) {
		//Questo loop mi serve solo per stampare a schermo le regioni
		TKey *currentKey = (TKey *)KeyList->At(i);
		TString className = currentKey->GetClassName();
		// se non e' un TGraph passa oltre
		if (! (className.CompareTo("TGraph") == 0)) continue;
		TGraph *g = (TGraph *)currentKey->ReadObj();
		TString name = g->GetName();
		name.Remove(0, name.First('_') + 1);
		TString variable = name(0, name.First('_'));
		name.Remove(0, name.First('_') + 1);
		TString region = name(0, name.Last('_'));
		name.Remove(0, name.Last('_') + 1);
		if(variable.EqualTo("scale")) { //giusto per non scrivere piu' volte la stessa regione
			std::cout << "[REGION] ./script/fit.sh " << filename << " " << region << std::endl;
		}
	}
	for(int i = 0; i <  KeyList->GetEntries(); i++) {
		TKey *currentKey = (TKey *)KeyList->At(i);
		TString className = currentKey->GetClassName();
		// se non e' un TGraph passa oltre
		if (! (className.CompareTo("TGraph") == 0)) continue;
		TGraph *g = (TGraph *)currentKey->ReadObj();
		TString name = g->GetName();
		name.Remove(0, name.First('_') + 1);
		TString variable = name(0, name.First('_'));
		name.Remove(0, name.First('_') + 1);
		TString region = name(0, name.Last('_'));
		name.Remove(0, name.Last('_') + 1);
		TString index = name(0, name.Length());
		name = g->GetName();

		if(singleRegion != "All") {
			if(!region.EqualTo(singleRegion)) {
				continue;
			}
		}
		if(variable_to_fit != "All") {
			;
			if(!variable.EqualTo(variable_to_fit)) {
				continue;
			}
		}
		(graph_map[variable])[region].push_back(g);
	}


	TPaveText *pt = new TPaveText(0.65, 0.91, 0.997, 0.997, "NDC");
	pt->SetTextSize(0.05);
	pt->SetFillColor(0);
	pt->SetBorderSize(0);
	TPaveText pave(0.1, 0.92, 0.48, 0.99, "ndc");
	pave.SetFillColor(0);
	pave.SetTextAlign(12);
	pave.SetBorderSize(0);
	pave.SetTextSize(0.04);
	pave.AddText("CMS Preliminary 36.4 fb^{-1} (13 TeV)");

	RooArgSet *vars = new RooArgSet();

	TCanvas *c = new TCanvas("c", "c");
	c->cd();

	//std::cout << "Looping" << std::endl;

	for(std::map<TString, map_region_t>::const_iterator map_itr = graph_map.begin();
	        map_itr != graph_map.end();
	        map_itr++) {

		for(map_region_t::const_iterator map_region_itr = map_itr->second.begin();
		        map_region_itr != map_itr->second.end();
		        map_region_itr++) {

			TString region = map_region_itr->first;
			TString variable = map_itr->first;
			bool isScale = map_itr->first.CompareTo("scale") == 0;
			bool isPhi = map_itr->first.CompareTo("phi") == 0;
			if(!(
			            (isScale && doScale == true) ||
			            (!isScale && !isPhi && doResolution == true) ||
			            (isPhi && doPhi == true)
			        )
			  ) continue;

			std::cout << "region: " << region << " variable: " << variable << "\t second.size: " << map_region_itr->second.size() << std::endl;

			float y_min = 1e10;
			std::cout << "Getting mean graph in macro/FitProfile2" << std::endl;
			//TGraphErrors *g1 = MeanGraph2(map_region_itr->second,&y_min);
			//std::cout << "Getting g mean graph" << std::endl;

			TGraphErrors *g_sum = SumGraph(map_region_itr->second, true);
			TGraphErrors *g = MeanGraphNew(g_sum);
//       if(!variable.Contains("rho")){
// 	continue;
//       }

			if(g == NULL) continue;
			g->SetMarkerStyle(20);
			g->SetMarkerSize(0.9);
			g->SaveAs("tmp/g_toBefitted-" + variable + "_" + region + ".root");
			//g_sum->SaveAs("tmp/g_sumGraph-"+variable+"_"+region+".root");
			if(variable.Contains("alpha")) { //||variable.Contains("constTerm")){
				continue;
			}

			std::cout << "[STATUS] Fitting iteratively for minumum and error estimation with macro_fit.C::IterMinimumFit" << std::endl;
			g->SetTitle(region);
			TF1* fun = IterMinimumFit(g, isScale, isPhi);
			TCanvas * test = new TCanvas("test", "test");
			g->Draw();
			fun->Draw("same");
			test->SaveAs("tmp/test.png");

			if (!fun) {
				std::cout << "FIT HAD PROBLEMS!: fit function not returned" << std::endl;
				return;
			}

			if(fun->GetParameter(2) <= 0 && fun->GetParameter(3) <= 0) continue;
			ffout << variable << " " <<  region  << " "
			      //<< fun->GetParameter(0) << " "
			      << fun->GetParameter(1) << " "
			      << 1. / (sqrt(2 * fun->GetParameter(2))) << " "
			      << 1. / (sqrt(2 * fun->GetParameter(3))) <<  std::endl;

			Plot(c, g, fun, pt, isScale, isPhi, variable, delta_min, delta_max);
			string title = g->GetTitle();

			pave.Draw();
			gPad->Update();
			gPad->SaveAs(fit_img_filename(filename, variable, region));
			gPad->SaveAs(fit_img_filename(filename, variable, region, ".C"));
			gPad->SaveAs(fit_img_filename(filename, variable, region, ".png"));

			RooRealVar *var_ = new RooRealVar(variable + "_" + region, variable + "_" + region, fun->GetParameter(1), fun->GetParameter(1) - 1. / (sqrt(2 * fun->GetParameter(2))) , fun->GetParameter(1) + 1. / (sqrt(2 * fun->GetParameter(3))));
			var_->setError((1. / (sqrt(2 * fun->GetParameter(2))) + 1. / (sqrt(2 * fun->GetParameter(3)))) / 2);
			var_->Print();
			std::cout << fixed << "Shift = " << fun->GetParameter(1) << " + " << 1. / (sqrt(2 * fun->GetParameter(3))) << " - " << 1. / (sqrt(2 * fun->GetParameter(2))) << "" << std::endl;
			vars->add(*var_);
		}
	}

	vars->Print();
	vars->writeToFile(fit_img_filename(filename, "FitResult-", "", ".config"));
	//std::cout << fixed << "Shift = " << fun->GetParameter(1) << " + " << 1./(sqrt(2* fun->GetParameter(3))) << " - " << 1./(sqrt(2* fun->GetParameter(2))) << "" << std::endl;
	//system("cat "+fit_img_filename(filename,"FitResult-","",".config")+" |awk '{printf(\"%s = %.4f +/- %.6f \", $1, $3, $5);print $6,$7,$8}' > "+fit_img_filename(filename,"FitResult","",".config"));

	delete vars;
	delete pt;
	return;
}



TH1F *MinimumProfile(TString filename, TString varname, TString regionname)
{
	gStyle->SetOptFit(0);

	TH1F *minHist = NULL;
	if(varname.Contains("rho") || varname.Contains("constTerm")) minHist = new TH1F(varname + "_hist", "", 1000, 0.005, 0.015);
	if(varname.Contains("alpha")) minHist = new TH1F(varname + "_hist", "", 600, 0.00, 0.06);
	//  TString region_set=filename;
	//region_set.Remove(0,region_set.First('/')+1);
	//  std::cout << "REGION SET = " << region_set << std::endl;
	//  region_set.Remove(region_set.First("/"));

	typedef std::vector<TGraph *> g_vec_t;
	typedef  std::map<TString, g_vec_t> map_region_t;
	std::map<TString, map_region_t> graph_map;

	std::cout << "Opening file: " << filename << std::endl;
	TFile f_in(filename, "open");
	if(!f_in.IsOpen()) {
		std::cerr << "Error. File not open" << std::endl;
		return NULL;
	}

	TList *KeyList = f_in.GetListOfKeys();
	for(int i = 0; i <  KeyList->GetEntries(); i++) {
		TKey *currentKey = (TKey *)KeyList->At(i);
		TString className = currentKey->GetClassName();
		// se non e' un TGraph passa oltre
		//std::cout << i << " " << className << std::endl;
		if (! (className.CompareTo("TGraph") == 0)) continue;
		TGraph *g = (TGraph *)currentKey->ReadObj();


		TString name = g->GetName();
		//    std::cout << "name = " << name << std::endl;
		name.Remove(0, name.First('_') + 1);
		TString variable = name(0, name.First('_'));
		name.Remove(0, name.First('_') + 1);
		TString region = name(0, name.Last('_'));
		name.Remove(0, name.Last('_') + 1);
		TString index = name(0, name.Length());

		if(!variable.Contains(varname)) continue;
		if(!region.Contains(regionname)) continue;
		name = g->GetName();
		//    std::cout << name << " " << variable << " " << region << " " << index << std::endl;


		(graph_map[variable])[region].push_back(g);
	}

	RooArgSet *vars = new RooArgSet();

	TCanvas *c = new TCanvas("c", "c");
	c->cd();

	std::cout << "Looping" << std::endl;
	for(std::map<TString, map_region_t>::const_iterator map_itr = graph_map.begin();
	        map_itr != graph_map.end();
	        map_itr++) {

		for(map_region_t::const_iterator map_region_itr = map_itr->second.begin();
		        map_region_itr != map_itr->second.end();
		        map_region_itr++) {

			TString region = map_region_itr->first;
			TString variable = map_itr->first;

			for(std::vector<TGraph *>::const_iterator g_itr = map_region_itr->second.begin();
			        g_itr != map_region_itr->second.end();
			        g_itr++) { // loop over TGraphs


				TGraph *graph = *g_itr;
				if(graph == NULL) continue; // per sicurezza
				Double_t *Y = graph->GetY();
				Double_t *X = graph->GetX();
				Int_t N = graph->GetN();

				double minimum = Y[0];
				int iMin = 0;
				for(int i = 0; i < N; i++) { // calcolo il minimo e il punto di minimo
					if(minimum > Y[i]) {
						minimum = Y[i];
						iMin = i;
					}
				}

				minHist->Fill(X[iMin]);
			}
		}
	}
	return minHist;
}


void propagate(TString filename = "tmp/out.out", TString myRegion = "absEta_0_1-bad-Et_20-trigger-noPF",  Double_t Emean = 7.179)
{

	gROOT->ProcessLine(".L macro/macro_fit.C+");

	std::ifstream ffin(filename);
	TString variable, region;
	Double_t rho_par[4], phi_par[4];

	while(ffin.peek() != EOF && ffin.good()) {
		ffin >> variable >> region;
		if(region != myRegion) continue;
		if(variable == "rho") ffin >> rho_par[0] >> rho_par[1] >> rho_par[2] >> rho_par[3];
		if(variable == "phi") ffin >> phi_par[0] >> phi_par[1] >> phi_par[2] >> phi_par[3];
	}

	TH1F *DeltaC = new TH1F("DeltaC", "", 200, 0, 0.05);
	TH1F *DeltaS = new TH1F("DeltaS", "", 200, 0, 0.4);


	Get_DeltaC_DeltaS(DeltaC, DeltaS, rho_par, phi_par, Emean);

	std::cout << "Delta C" << " " << myRegion << "\t" << DeltaC->GetMean() << "\t"
	          << DeltaC->GetRMS() << std::endl;
	std::cout << "Delta S" << " " << myRegion << "\t" << DeltaS->GetMean() << "\t"
	          << DeltaS->GetRMS() << std::endl;

}


void propagate(TString filename, int region)
{
	TString myRegion = "";
	Double_t Emean = 0;
	if(region == 0) {
		myRegion = "absEta_0_1-bad-Et_20-trigger-noPF-EB";
		Emean = 7.179;
	}
	if(region == 1) {
		myRegion = "absEta_0_1-gold-Et_20-trigger-noPF-EB";
		Emean = 7.051;
	}
	if(region == 2) {
		myRegion = "absEta_1_1.4442-bad-Et_20-trigger-noPF-EB";
		Emean = 8.712;
	}
	if(region == 3) {
		myRegion = "absEta_1_1.4442-gold-Et_20-trigger-noPF-EB";
		Emean = 8.08;
	}

	propagate(filename, myRegion, Emean);
}



