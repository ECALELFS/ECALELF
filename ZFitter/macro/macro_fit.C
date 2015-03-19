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

//#define selectedBase_class_cxx
//#include "RooSmearedHistoCompatibility.h"

#define DEBUG
//TString filename=outPrefix+"-"+region_set+"-outProfile.root";

#define FIT_LIMIT 0.01
#define ITER_MAX 6

TRandom3 gen(0);

#define NOT_CONVERGED_LIMIT 0.6

TGraph *SmoothGraph(TGraph *graph){
  Double_t *X = graph->GetX();
  Double_t *Y = graph->GetY();
 
  int N = graph->GetN();
  Double_t x[3000], y[3000]; //, ex[1000]={0}, ey[1000]={0};

  // last point is ignored
  for(int i=0,ii=0; i < N-1; i+=2, ii++){
    y[ii]=(Y[i]+Y[i+1])*0.5;
    x[ii]=(X[i]+X[i+1])*0.5;
  }
  //  return  new TGraphErrors(N-1, x, y,ex,ey);
  return  new TGraph(N/2, x, y);
}

void ShiftYGraph(TGraphErrors *graph, double y){
  Double_t *Y = graph->GetY();
  int N = graph->GetN();

  for(int i=0; i < N; i++){
    Y[i]-=y;
  }
  return;
}


TString img_filename(TString filename, TString prefix, TString region, TString ext=".eps"){
  //  TString filename=file->GetName(); //"testing"+region+"_Histos.root";
  //  TFile file(filename,"open");
  
  filename.ReplaceAll("fitres/","img/");
    
  //  if(region.Length()!=0) region.Insert(0,"_");
  //filename.ReplaceAll("/outProfile","/"+prefix+"-"+region+"-");
  filename.ReplaceAll(".root",TString("-")+prefix+region+ext);
  //  filename.Insert(filename.Length()-4,region);
  std::cout << "filename = "<< filename << std::endl;
  //filename="tmp/Hgg/"+prefix+region+ext;
  return filename;
}

Double_t asymmetricParabola(Double_t* x,Double_t* par)
{
  // qualche commento non farebbe male
  Double_t value=0.;
  if (x[0]<=par[1])
    value=par[2]*(x[0]-par[1])*(x[0]-par[1]);
  else
    value=par[3]*(x[0]-par[1])*(x[0]-par[1]);
  return value+par[0];
}

Double_t likelihoodAsymmetricParabola(Double_t *x, Double_t *par){
  return exp(-(asymmetricParabola(x, par)));
}

RooDataSet *getL(Double_t *par, RooRealVar *var1, Long64_t nEvents=10000){
  Double_t range_min=par[1]-4/(sqrt(2*par[2])), range_max=par[1]+4/(sqrt(2*par[3]));
  range_min=std::max(0.,range_min);

  TF1* f2 = new TF1("f2", likelihoodAsymmetricParabola,range_min,range_max,4);
  f2->SetParameter(0, par[0]);
  f2->SetParameter(1, par[1]);
  f2->SetParameter(2, par[2]);
  f2->SetParameter(3, par[3]);
  f2->SetNpx(1e3);

  TH1F *hist = (TH1F*)f2->GetHistogram();

  RooDataHist *dataHist = new RooDataHist(TString(hist->GetName())+"_dataHist",hist->GetTitle(), RooArgSet(*var1), hist);
  RooHistPdf *histPdf = new RooHistPdf(TString(hist->GetName())+"_histPdf",hist->GetTitle(), RooArgSet(*var1), *dataHist);
  
  RooDataSet *data = histPdf->generate(RooArgSet(*var1), nEvents, kTRUE);
  //delete f2;
  delete hist;
  //delete var1;
  delete dataHist;
  delete histPdf;


  return data;
}


void Get_DeltaC_DeltaS(TH1F *DeltaC_hist, TH1F *DeltaS_hist, //to be filled
		       Double_t *rho_par, Double_t *phi_par, Double_t Emean){
		       
  Long64_t nEvents=10000;
  RooRealVar *rho = new RooRealVar("rho","",0.1);
  RooDataSet *rho_data = getL(rho_par, rho, nEvents);

  RooRealVar *phi = new RooRealVar("phi","",0.1);
  RooDataSet *phi_data = getL(phi_par, phi, nEvents);
  
  for(Long64_t jentry=0; jentry<nEvents; jentry++){
    const RooArgSet *rhoArg = rho_data->get(jentry);
    const RooArgSet *phiArg = phi_data->get(jentry);
    Double_t rhoVal=rhoArg->getRealValue(rho->GetName());
    Double_t phiVal=phiArg->getRealValue(phi->GetName());

    Double_t DeltaC = rhoVal * sin(phiVal);
    Double_t DeltaS = rhoVal * Emean * cos(phiVal);
    if(jentry<10) std::cout << rhoVal 
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


int pointsInInterval(Double_t *array, int N, double min, double max){
  int count=0;
  for(int i=0; i < N; i++)
    if(array[i] < max && array[i] > min) count++;
  return count;
}

double rangeWithPoints(TGraphErrors *g, int nPoints, Double_t *xMin, Double_t* xMax){
  g->Sort();
  int iMin=0;
  Double_t *Y = g->GetY();
  Double_t *X = g->GetX();
  Double_t min=1e20;
  for(int i=1; i < g->GetN(); i++){
    if(Y[i] <= min){
      iMin=i;
      min=Y[i];
    }
  }

  int _iMax=std::min(g->GetN(),iMin+ nPoints);  
  int _iMin=std::max(0,iMin- nPoints);

  double dist = std::max(X[_iMax]-X[iMin],X[iMin]-X[_iMin]);
  (*xMin)= X[iMin]-dist/2;
  (*xMax)= X[iMin]+dist/2;
  return std::max(Y[_iMin],Y[_iMax]);
}


TF1* IterMinimumFit(TGraphErrors *g, bool isScale, bool isPhi=false)
{
#ifdef DEBUG
  g->SaveAs("ciao.root");
#endif
  double rangeLimMax=1.04;
  double rangeLimMin=0.96;
  if(!isScale){
    rangeLimMax=0.15;
    rangeLimMin=0.00;
  }
  if(isPhi){
    rangeLimMin=1e-10;
    rangeLimMax=1.58;
  }

  Double_t *X = g->GetX();
  //Double_t X_bin=fabs(X[1]-X[0]);
  int N = g->GetN();

  double range_min=TMath::Max(TMath::MinElement(g->GetN(),g->GetX()),rangeLimMin);
  double range_max=TMath::Min(TMath::MaxElement(g->GetN(),g->GetX()),rangeLimMax);
  
  
  g->Fit("pol2","0FR+","",range_min,range_max);

  TF1 *fun = (TF1*) g->GetListOfFunctions()->At(g->GetListOfFunctions()->GetSize()-1);
  fun->SetRange(range_min,range_max);
  if(fun==NULL){
    std::cerr << "Error: polynomial fit function not defined" << std::endl;
    return 0;
  }


  fun->GetRange(range_min, range_max);
#ifdef DEBUG
  fun->Print();
#endif
  //fun->SetRange(0.95,1.04);
  //  double sigma_old; 
  double sigma_plus_old; 
  double sigma_minus_old; 
  double minX_old; 
  //  double sigma=1./sqrt(2* fun->GetParameter(2));
  //  double minX=-fun->GetParameter(1) / (2* fun->GetParameter(2));
  // double sigma=(range_max-range_min)/2;
  double sigma=1./sqrt(2* fun->GetParameter(2));
  if(sigma<1e-5 || sigma>1e10 || sigma!=sigma) sigma=1e-4;
  double sigma_plus=sigma;
  double sigma_minus=sigma;
  double minX=g->GetX()[TMath::LocMin(g->GetN(),g->GetY())];
  if(minX==0 || minX<rangeLimMin || minX > rangeLimMax) minX= -fun->GetParameter(1) / (2* fun->GetParameter(2));

  double old_chi2=0., chi2=0.;
   
  if(sigma<0.0001) sigma=0.0001;
  if(minX<rangeLimMin) minX=rangeLimMin+0.001;
  std::cout << "[INFO] Init ------------------------------" << std::endl
	    << "        minX: "  << minX << std::endl
	    << "        sigma: " << sigma << std::endl;
  //  range_max=range_min+0.01;
  TF1* f2 = new TF1("f2",asymmetricParabola,range_min,range_max,4);
  f2->SetParameter(0,0); //TMath::MinElement(g->GetN(),g->GetY()));
  f2->SetParameter(1,minX);
  f2->SetParameter(2,1/(2*sigma*sigma));
  f2->SetParameter(3,1/(2*sigma*sigma));
  f2->SetParLimits(0,-100,100);
  f2->SetParLimits(1,rangeLimMin,rangeLimMax);
  //f2->SetParLimits(2,1e5,1e9);
  f2->SetParLimits(2,0,1e10);
  f2->SetParLimits(3,0,1e10);
  int iter=0;
  do{
    old_chi2=chi2;

    minX_old=minX;
    sigma_plus_old=sigma_plus;
    sigma_minus_old=sigma_minus;
    
    //f2->FixParameter(1,0);
    if(minX<=rangeLimMin+1e-9){
      std::cout << "[INFO] Fixing min" << std::endl;
      f2->SetParLimits(2,0,100);
      //f2->FixParameter(1,rangeLimMin);
    }
    if(minX>=rangeLimMax-1e-9){
      std::cout << "[INFO] Fixing max" << std::endl;
      f2->SetParLimits(3,0,100);
      //f2->FixParameter(1,rangeLimMin);
    }

    TString fname="f2_iter_"; fname+=iter;
    f2->SetName(fname); 
     std::cout << "Fit range: [ " << range_min << " : " << range_max << "]" << std::endl;
     f2->SetRange(range_min,range_max);
    //    g->Fit(fun,"F+","",range_min, range_max);   
    g->Fit(f2,"0R+","",range_min, range_max);   
    //    g->Fit(fun,"R+","",min-2*sigma, min+2*sigma);
    //    sigma = 1./sqrt(2* fun->GetParameter(2));

    sigma_minus = 1./sqrt(2* f2->GetParameter(2));
    sigma_plus = 1./sqrt(2* f2->GetParameter(3));
    //    min=- fun->GetParameter(1) / (2* fun->GetParameter(2));
    minX=f2->GetParameter(1);
    chi2 = fun->GetChisquare()/fun->GetNDF(); //g->Chisquare(fun);

    if(isScale){
      range_min = TMath::Max(minX-15*sigma_minus, rangeLimMin);
      range_max = TMath::Min(minX+10*sigma_plus, rangeLimMax);   
    } else if(isPhi){
      range_min = TMath::Max(minX-2*sigma_minus, rangeLimMin);
      range_max = TMath::Min(minX+3*sigma_plus,  rangeLimMax);   
      if(minX<0.5){
	if(!TString(g->GetTitle()).Contains("1_1.4442-gold")) range_max=0.5;
	if(TString(g->GetTitle()).Contains("0_1-bad")){ range_min=0; range_max=0.3;}
	//if(TString(g->GetTitle()).Contains("0_1-bad")){ range_min=0.5; range_max=0.9;};
	//if(TString(g->GetTitle()).Contains("0_1-gold")) range_max=0.8;
	if(TString(g->GetTitle()).Contains("0_1-gold")) range_max=0.4;
      }
      if(minX>1.2){
	if(TString(g->GetTitle()).Contains("0_1-bad")){ range_min=0.5; range_max=1.6;};
      }
    } else {
      range_min = TMath::Max(minX-5*sigma_minus, rangeLimMin);
      range_max = TMath::Min(minX+5*sigma_plus,  rangeLimMax);   
    }

    std::cout << "[INFO] Points in interval: " << pointsInInterval(X, N, range_min, range_max) << std::endl;
    if(!(TString(g->GetTitle()).Contains("0_1-bad"))){
    while (pointsInInterval(X, N, range_min, range_max) < 10 && range_min>rangeLimMin && range_max<rangeLimMax){ //((range_max - range_min)/X_bin < 10)){
      // incremento del 10% i range
      std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
      std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
      
      range_min = TMath::Max(range_min-(rangeLimMax-rangeLimMin)*0.05, rangeLimMin);
      range_max = TMath::Min(range_max+(rangeLimMax-rangeLimMin)*0.05, rangeLimMax);   
    }
    while (pointsInInterval(X, N, minX, range_max) < 10 && range_max<rangeLimMax){ //((range_max - range_min)/X_bin < 10)){
      // incremento del 10% i range
      std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
      std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
      
      //range_min = TMath::Max(range_min-(rangeLimMax-rangeLimMin)*0.05, rangeLimMin);
      range_max = TMath::Min(range_max+(rangeLimMax-rangeLimMin)*0.05, rangeLimMax);   
    }
    }

    std::cout << "Fitted range: [ " << range_min << " : " << range_max << "]" << std::endl;
      f2->SetRange(range_min,range_max);
//     std::cout << minX_old << " " << minX << " " << minX-minX_old << std::endl;
    if(sigma_plus*100 < 0.01) std::cout << "sigma_plus = " << " " << sigma_plus << std::endl;
    if(sigma_minus*100 < 0.01) std::cout << "sigma_minus = " << " " << sigma_minus << std::endl;
    iter++;
#ifdef DEBUG
    std::cout << "MINUIT STATUS " << gMinuit->GetStatus() << std::endl;
    std::cout << "CHI2/NDF" << chi2 << std::endl;
#endif
    //Rescaling graph points so that minimum iteratively goes to 0
    //for (int iPoint=0;iPoint<g->GetN();++iPoint)
    //  *(g->GetY()+iPoint)=*(g->GetY()+iPoint)-f2->GetParameter(0);
    //} While( (((fabs(minX-minX_old)/minX > FIT_LIMIT || fabs(sigma_plus-sigma_plus_old)/sigma_plus>FIT_LIMIT || fabs(sigma_minus-sigma_minus_old)/sigma_minus>FIT_LIMIT )) && iter<20) || iter<10 ); 
  } while (fabs(chi2 - old_chi2)/old_chi2 > FIT_LIMIT && iter < ITER_MAX || iter < 4); //|| iter < 4 || gMinuit->GetStatus() != 0);
  
  std::cout << "ITER to converge = " << iter << std::endl;
  if(iter >=ITER_MAX) std::cout << "WARNING: iter not converged" << std::endl;

  //  float chi2_rnd = TMath::ChisquareQuantile(gen.Rndm(),1);
  if(gMinuit->GetStatus()==0){
#ifdef DEBUG
    std::cout <<  f2->GetChisquare() << "\t" << f2->GetNumberFitPoints() << "\t" << f2->GetNumberFreeParameters() << std::endl;
#endif

  float errorScale=sqrt(f2->GetChisquare()/(f2->GetNumberFitPoints()-f2->GetNumberFreeParameters()));
  std::cout << "SCALING ERROR by " << errorScale  << std::endl
	    << "Shifting by " << f2->GetParameter(0) << std::endl;
  if(g->GetEY()!=NULL){
    for (int i=0;i<g->GetN();++i){
      *(g->GetEY()+i)= (*(g->GetEY()+i)) * errorScale; //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);
      // shift to have at DeltaNLL=0 the minimum of the fit
      *(g->GetY()+i) -= f2->GetParameter(0); //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);
    }
    f2->SetParameter(0,0);
  }
  }

  return f2;
}

typedef struct{
  double y;
  double y2;
  int n;
} point_value_t;

typedef  std::map<Double_t, point_value_t> graphic_t;

TGraphErrors *SumGraph(std::vector<TGraph *> g_vec, bool shift=false){
  int N = (*g_vec.begin())->GetN(); // number of points
  TString name=(*g_vec.begin())->GetName();
  if(8000 <= N) {
    std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
    return NULL;
  }

  
  Double_t x[20000]={0.}, y[20000]={0.}, y2[20000]={0.}, dev_st[20000], err_x[20000];
  int n=0;
  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
    TGraph *graph = *g_itr;
    if(graph==NULL) continue; // per sicurezza
    Double_t *Y = graph->GetY();
    Double_t *X = graph->GetX();
    N = graph->GetN();

    double minimum=Y[0];
    for(int i=0; i< N; i++){ // calcolo il minimo e il punto di minimo
      if(minimum > Y[i]) minimum=Y[i];
    }

    //devo evitare punti con la stessa x fatti dallo stesso scan perche' a causa del deterministic smearing 
    //hanno per costruzione la stessa likelihood

    std::set<double> uniqX;
    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
      {
	if((uniqX.insert(X[ip])).second==false){
	  std::cout << "[INFO] Point already included: " << X[ip] << std::endl;
	  continue;
	}
	x[n]=X[ip];
	
	if(shift) y[n]+=Y[ip]-minimum;
	else  y[n]+=Y[ip];

	y2[n]+=y[n]*y[n];
	//	std::cout << ip+n << "\t" << x[ip+n] << "\t" << y[ip+n] << "\t" << Y[ip+n] << std::endl;
	err_x[n]=0;
	dev_st[n]++; // used to count the points per bin
	if(n>19999){
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

  for (int ip=0;ip<n;++ip){
    //   y2[ip]/=dev_st[ip];
    //    y[ip]/=dev_st[ip];
    //    if(dev_st[ip]!=1) dev_st[ip]=y2[ip]-y[ip]*y[ip];
    //    else 
    dev_st[ip]=0;
  }
  std::cout << "[INFO] n = " << n << std::endl;
  TGraphErrors *g = new TGraphErrors(n, x, y, err_x, dev_st);

  return g;
}

TGraphErrors *MeanGraphNew(TGraphErrors *graph){
  
  int N = graph->GetN(); // number of points
  if(17000 <= N) {
    std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
    return NULL;
  }

  std::map<Double_t, std::pair<Double_t, Double_t> > g_in_map;
  
  Double_t x[17000]={0.}, y[17000]={0.}, y2[17000]={0.}, dev_st[17000], err_x[17000];
  
  graphic_t graphic, graphic2;

  int n_max=0;
  
  double y_min=1e20;  

  if(graph==NULL) return NULL;
  if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;

  Double_t *Y = graph->GetY();
  Double_t *X = graph->GetX();
  Double_t X_bin=0.00001; //fabs(X[1]-X[0]);
  //    double minimum=TMath::MinElement(graph->GetN(),graph->GetY());
  double minimum=1e20;
  double x_minimum=X[0];
  for(int i=1; i<graph->GetN(); i++){ // calcolo il minimo e il punto di minimo
    if(minimum > Y[i]){
      minimum=Y[i];
      x_minimum=X[i];
    }
  }
  
  //subtract the minimum?
  for(int i=1; i<graph->GetN(); i++){ // calcolo il minimo e il punto di minimo
      Y[i]-=minimum;
  }

  for(int i =0 ; i < graph->GetN(); i++){
    graphic_t::iterator itr = graphic.find(X[i]);
    if(!graphic.empty() && itr == graphic.end()) {
      // controllo se e' vicino al primo bordo, poi al secondo e altrimenti lo butto
      itr = graphic.lower_bound(X[i]);
      //	  if(itr== graphic.end()) std::cout << "Error: graphic empty" << std::endl;
      if(itr!=graphic.begin()) itr--;
      if( (fabs(itr->first - X[i]) > 0.00000001)){ // check del bin prima
	itr++;
	if( itr!=graphic.end() && (fabs(itr->first - X[i]) > 0.00000001)){ // check del bin
	  itr++;
	  if( itr!=graphic.end() && (fabs(itr->first - X[i]) > 0.00000001)){ // check del bin dopo
	    itr=graphic.end();
	  } //else std::cout << "[DEBUG] New point: " << itr->first  << "\t" << X[i] << std::endl; 
	} //else std::cout << "[DEBUG] moving to upper bound: " << itr->first  << "\t" << X[i] << std::endl;
      } //else std::cout << "[DEBUG] moving to lower bound: " << itr->first  << "\t" << X[i] << std::endl;
    }
    
    
    if(itr!=graphic.end()){
      itr->second.y+=Y[i];
      itr->second.y2+=Y[i]*Y[i];
      itr->second.n++;
      n_max = std::max(n_max,itr->second.n);
    } else {
      point_value_t p;
      p.y=Y[i];
      p.y2=Y[i]*Y[i];
      p.n=1;
      graphic.insert(std::make_pair<Double_t, point_value_t>(X[i],p));
      n_max = std::max(n_max,1);
    }
  }
    
  /*-----------------------------*/
  //  std::cout << "N_MAX = " << n_max << std::endl;
  int n=0;
  y_min=1e10;
  for(graphic_t::iterator itr=graphic.begin(); itr!=graphic.end(); itr++){
    
    if(itr->second.n < n_max*0.30){std::cout << "Removing point " << itr->first << "\t" << itr->second.n << "\t" << n_max << std::endl; continue;}
    double ym = itr->second.y/itr->second.n;
    if(itr->second.n>1 && itr->second.y2/itr->second.n >= ym*ym)
      dev_st[n] =  sqrt( (itr->second.y2/itr->second.n - ym*ym)/(double)(itr->second.n)); //errore sulla media
    else{
      std::cout << itr->first << "\t" << itr->second.y << "\t" << itr->second.n 
		<< "\t" << itr->second.y2/itr->second.n << "\t" << ym*ym
		<< std::endl;
      dev_st[n] =  1;
      //continue;
    }
    x[n] = itr->first;
    y[n] = ym;
    err_x[n]=0;

    y_min = std::min(y_min,y[n]);
//     std::cout << graph->GetName() << "\t" << graph->GetTitle() << x[n] << " " 
//     	      << y[n] << " "
//     	      << dev_st[n] << " "
//     	      << itr->second.y2/itr->second.n << " " << ym*ym   << " " 
// 	      << itr->second.n
//     	      << std::endl;
    n++;

  }

  for(int i=0; i< n; i++){ // calcolo il minimo e il punto di minimo
    y[i]-=y_min;
  }

//   for(int i =0 ; i < graph->GetN(); i++){
//     graphic_t::iterator itr1 = graphic.find(X[i]);
//     graphic_t::iterator itr = graphic2.find(X[i]);

//     double ym = itr1->second.y/itr1->second.n;
//     double devst = sqrt( (itr1->second.y2/itr1->second.n - ym*ym)/(double)(itr1->second.n)); //errore sulla media
//     //std::cout << ym << "\t" << devst << std::endl;
//     //if(devst < 0.1 && fabs(Y[i]-ym) < 4 * devst) continue;
    
//     if(itr!=graphic2.end()){
//       itr->second.y+=Y[i];
//       itr->second.y2+=Y[i]*Y[i];
//       itr->second.n++;
//       n_max = std::max(n_max,itr->second.n);
//     } else {
//       point_value_t p;
//       p.y=Y[i];
//       p.y2=Y[i]*Y[i];
//       p.n=1;
//       graphic2.insert(std::make_pair<Double_t, point_value_t>(X[i],p));
//       n_max = std::max(n_max,1);
//     }
//   }
    
//   /*-----------------------------*/
//   //  std::cout << "N_MAX = " << n_max << std::endl;
//   n=0;
//   y_min=1e10;
//   for(graphic_t::iterator itr=graphic2.begin(); itr!=graphic2.end(); itr++){
//     x[n] = itr->first;
//     double ym = itr->second.y/itr->second.n;
//     y[n] = ym;
//     dev_st[n] = sqrt( (itr->second.y2/itr->second.n - ym*ym)/(double)(itr->second.n)); //errore sulla media
//     err_x=0;

//     y_min = std::min(y_min,y[n]);
//     n++;
//   }

  TGraphErrors *g = new TGraphErrors(n, x, y, err_x, dev_st);
  g->SetTitle(graph->GetTitle());
  g->SetName(graph->GetTitle());
  //  g->Draw("AP*");
  return g;
}

TGraphErrors *MeanGraph2(std::vector<TGraph *> g_vec, float *y_minimum=NULL, TGraphErrors *g_in=NULL){
  
  TGraphErrors *newGraph = new TGraphErrors;

  Double_t x[20000]={0.}, y[20000]={0.}, y2[20000]={0.}, dev_st[20000], err_x[20000];
  int n=0;
  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
    TGraph *graph = *g_itr;
    if(graph==NULL) continue; // per sicurezza
    Double_t *Y = graph->GetY();
    Double_t *X = graph->GetX();
    Int_t N = graph->GetN();

    double minimum=Y[0];
    for(int i=0; i< N; i++){ // calcolo il minimo e il punto di minimo
      if(minimum > Y[i]) minimum=Y[i];
    }

    //    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
      {
	newGraph->SetPoint(ip+n, X[ip], Y[ip]);
      }
    n+=N;
  }
  newGraph->Set(n);
  TGraphErrors *returnGraph = MeanGraphNew(newGraph);
  delete newGraph;
  return returnGraph;
}

TGraphErrors *MeanGraph(std::vector<TGraph *> g_vec, float *y_minimum=NULL, TGraphErrors *g_in=NULL){
  int N = (*g_vec.begin())->GetN(); // number of points
  TString name=(*g_vec.begin())->GetName();
  if(8000 <= N) {
    std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
    return NULL;
  }

  
  Double_t x[20000]={0.}, y[20000]={0.}, y2[20000]={0.}, dev_st[20000], err_x[20000];
  int n=0;
  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
    TGraph *graph = *g_itr;
    if(graph==NULL) continue; // per sicurezza
    Double_t *Y = graph->GetY();
    Double_t *X = graph->GetX();

    double minimum=Y[0];
    for(int i=0; i< N; i++){ // calcolo il minimo e il punto di minimo
      if(minimum > Y[i]) minimum=Y[i];
    }

    //    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
      {
	if(ip!=0 && 	x[ip]!=X[ip]){
	  std::cerr << "[ERROR] Graphs have different bin positions!" << std::endl;
	  exit(1);
	}
	x[ip]=X[ip];
	if(Y[ip]<10){
	  //std::cout << Y[ip] << std::endl;
	  continue;
	}
	y[ip]+=Y[ip]; //-minimum;
	y2[ip]+=Y[ip]*Y[ip];
	//	std::cout << ip+n << "\t" << x[ip+n] << "\t" << y[ip+n] << "\t" << Y[ip+n] << std::endl;
	err_x[ip]=0;
	dev_st[ip]++; // used to count the points per bin	

	if(n>19999){
	  std::cerr << "############################################################" << std::endl;
	  std::cerr << "############################################################" << std::endl;
	  std::cerr << "############################################################" << std::endl;
	  break;
	}

      }
    if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;
  }

  for (int ip=0;ip<N;++ip){
    if(dev_st[ip]!=0){
      y2[ip]/=dev_st[ip];
      y[ip]/=dev_st[ip];
      if(dev_st[ip]>1){
	//std::cout << y2[ip] << "\t" << y[ip]*y[ip] << "\t" << y[ip] << "\t" << dev_st[ip] << std::endl;
	dev_st[ip]=sqrt(y2[ip]-y[ip]*y[ip])/sqrt(dev_st[ip]);
      }
      else dev_st[ip]=0;
    } 
  }

  // calcolo il minimo e il punto di minimo
  double minimum=1e20;
  for(int i=2; i< N; i++) if(minimum > y[i] && y[i]>10) minimum=y[i];
  for(int i=0; i< N; i++) y[i]-=minimum;

  

  TGraphErrors *g = new TGraphErrors(N, x, y, err_x, dev_st);
  //  g->Draw("AP*");
  return g;
}


TGraphErrors *MeanGraphOld(std::vector<TGraph *> g_vec, float *y_minimum=NULL, TGraphErrors *g_in=NULL){
  
  int N = (*g_vec.begin())->GetN(); // number of points
  if(3000 <= N) {
    std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
    return NULL;
  }

  std::map<Double_t, std::pair<Double_t, Double_t> > g_in_map;
  
  Double_t x[3000]={0.}, y[3000]={0.}, y2[3000]={0.}, dev_st[3000], err_x[3000];
  
  graphic_t graphic;

  int n_max=0;
  
  double y_min=1e20;  

  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
     
    graphic_t graphs;
    TGraph *graph = *g_itr;
    if(graph==NULL) continue; // per sicurezza
    if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;

    Double_t *Y = graph->GetY();
    Double_t *X = graph->GetX();
    Double_t X_bin=0.00001; //fabs(X[1]-X[0]);
    //    double minimum=TMath::MinElement(graph->GetN(),graph->GetY());
    double minimum=1e20;
    double x_minimum=X[0];
    for(int i=1; i<graph->GetN(); i++){ // calcolo il minimo e il punto di minimo
      if(minimum > Y[i]){
	//std::cout << minimum << "\t" << Y[i] << std::endl;
	minimum=Y[i];
	x_minimum=X[i];
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
    for (int ip=0;ip<graph->GetN();++ip) //sottraggo il minimo per avere un DeltaL in ordinata
      {
	Y[ip] -= minimum;
	//	std::cout << ip << ";" << X[ip] << ";" << Y[ip] << std::endl;
      }

    //    std::cout << "XBIN = " << X_bin << std::endl;
    //    std::cout << "Y[7] = " << Y[7] << std::endl;


    for(int i =0 ; i < graph->GetN(); i+=3){
      if(graphs.count(X[i])==0){
	point_value_t p;
	p.y=Y[i];
	p.y2=Y[i]*Y[i];
	p.n=1;
	graphs.insert(std::make_pair<Double_t, point_value_t>(X[i],p));
      }else{
	//std::cout << X[i] << "\t" << Y[i] << std::endl;
      }
    }
    
    for(graphic_t::iterator gitr=graphs.begin(); gitr!=graphs.end(); gitr++){
    
    graphic_t::iterator itr = graphic.find(gitr->first);

    if(!graphic.empty()){
      
      if(itr == graphic.end()) {
	// controllo se e' vicino al primo bordo, poi al secondo e altrimenti lo butto
	itr = graphic.lower_bound(gitr->first);
	//	  if(itr== graphic.end()) std::cout << "Error: graphic empty" << std::endl;
	itr--;
	if( !(fabs(itr->first - gitr->first) < X_bin/5.)){ // check del bin prima
	  itr++;
	  if( !(fabs(itr->first - gitr->first) < X_bin/5.)){ // check del bin
	    itr++;
	    if( !(fabs(itr->first - gitr->first) < X_bin/5.)){ // check del bin dopo
#ifdef DEBUG
	      if(gitr->first < graphic.begin()->first){
		std::cout << "Value not found: " << gitr->first << "; "
			  << graphic.lower_bound(gitr->first)->first << "; "
			  << graphic.upper_bound(gitr->first)->first << "; "
			  << X_bin/5.
			  <<	      std::endl; 
	      }
#endif
	      itr=graphic.end();
	    }
	  }
	}
      }
      } else itr=graphic.end();
    

      if(itr!=graphic.end()){
	itr->second.y+=gitr->second.y;
	itr->second.y2+=gitr->second.y*gitr->second.y;
	itr->second.n++;
	n_max = std::max(n_max,itr->second.n);
	//	if(itr->second.n>100) std::cout << "added point with n>100\t" << i << "\t" << X[i] << "\t" << gitr->second.y << "\t" << itr->second.n << "\t" << n_max << std::endl;
      } else {
	//if(g_itr!=g_vec.begin())	std::cout << "added point" <<  << "\t" << gitr->second.y << "\t" << std::endl;
	// se non c'e' lo aggiungo
	point_value_t p;
	p.y=gitr->second.y;
	p.y2=gitr->second.y*gitr->second.y;
	p.n=1;
	n_max = std::max(n_max,1);
	graphic.insert(std::make_pair<Double_t, point_value_t>(gitr->first,p));
	//	continue; // se non c'e' lo salto
      }
	//      }
    
    }
  }


      
  /*-----------------------------*/
  std::cout << "N_MAX = " << n_max << std::endl;
  int n=0;
  y_min=1e10;
  for(graphic_t::iterator itr=graphic.begin(); itr!=graphic.end(); itr++){
 
    //    if(itr->second.n < n_max-15) continue;
    if(itr->second.n < n_max*0.2){std::cout << "Removing point " << itr->first << "\t" << itr->second.n << std::endl; continue;}
    //if(itr->second.n != n_max) std::cout << itr->first << "\tn = " << itr->second.n << " " << n_max << std::endl;
    //    std::cout << itr->first << " " << itr->second.n << " " << itr->second.y << " " << itr->second.y2 << std::endl;
    x[n] = itr->first;
    double ym = itr->second.y/itr->second.n;
    y[n] = ym;
    dev_st[n] = sqrt( (itr->second.y2/itr->second.n - ym*ym)/(double)(itr->second.n)); //errore sulla media
    err_x[n]=0;

    y_min = std::min(y_min,y[n]);
    //     std::cout << x[n] << " " 
    // 	      << y[n] << " "
    // 	      << dev_st[n] << " "
    // 	      << itr->second.y2/itr->second.n << " " << ym*ym       
    // 	      << std::endl;
    n++;

  }
  
  // metto il minimo a 0
  for(int i =0 ; i < n ; i++){
    y[i]-=y_min;
  }

  if(y_minimum!=NULL) *y_minimum = y_min;
  TGraphErrors *g = new TGraphErrors(n, x, y, err_x, dev_st);
  //  g->Draw("AP*");
  return g;
}


TH1F *Plot_distr(std::vector<TGraph *> g_vec, int i_bin=21, int n_bin=20){
  
  if(gROOT->FindObject("d") != NULL) delete gROOT->FindObject("d");


  //  int N = g_vec.size(); // number of points

  //  double y=0, y2=0, dev_st;
  double y_min=(g_vec[0]->GetY())[i_bin] , y_max=(g_vec[0]->GetY())[i_bin];


  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
    TGraph *graph = *g_itr;
    Double_t *Y = graph->GetY();
    //    Double_t *X = graph->GetX();
    //    Double_t X_bin=fabs(X[1]-X[0])/2.;

    //    y+=Y[i_bin];
    //    y2+=Y[i_bin]*Y[i_bin];
    if(y_min > Y[i_bin]) y_min=Y[i_bin];
    if(y_max < Y[i_bin]) y_max=Y[i_bin];
  }
  //  y/=N; //calcolo della media
  //  y2/=N; // y2 medio
  //  dev_st = y2 - y*y;

  TH1F *h = new TH1F("d", "", n_bin, y_min, y_max);
  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
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


std::vector<TGraph *> GetGraphVector(int nToys, TString region, TFile *f){
  std::vector<TGraph *> g_vec;

    for(int i =0; i < nToys; i++){
      TString name="profileChi2_scale-"+region+"-";
      name+=i;
      //      std::cout << "name = " << name << std::endl;
      g_vec.push_back((TGraph*)f->Get(name));
      if(g_vec[i]==NULL) {
	std::cout << "Graph " << name << "not found." << std::endl;
	g_vec.pop_back();
	//      	return;
      }
    }
    return g_vec;
}


void Plot(TCanvas *c, TGraphErrors *g, TF1 *fun, TPaveText *pt, bool isScale, bool isPhi, TString variable){
      float sigma=1./(sqrt(2* std::max(fun->GetParameter(2),fun->GetParameter(3))));      
      
      TH2F a("a","a",
	     10, max(fun->GetParameter(1)-20.*1./(sqrt(2* fun->GetParameter(2))),0.96),
	     min(fun->GetParameter(1) +20. * 1./(sqrt(2* fun->GetParameter(2))),1.04),
	     10,-1.,100.);
      
      a.GetYaxis()->SetTitle("#Delta L (a. u.)");
      c->Clear();
      if(isScale) {
	
       	a.GetXaxis()->SetTitle("1+#DeltaP ");
	//	a.Draw();
	//g->GetXaxis()->SetRangeUser(std::max(fun->GetXmin()-sigma,0.), std::min(fun->GetXmax()+sigma,0.15));
	//	g->Draw("PSAME");

	Double_t xMin, xMax;
	Double_t yMax = rangeWithPoints(g, 3, &xMin, &xMax);
	std::cout << "AAAAAAAAAAAAAAAa" << xMin << "\t" << xMax << "\t" << sigma << "\t" << fun->GetXmin() << std::endl;
	std::cout << "AAAAAAAAAAAAAAAa" << xMin << "\t" << xMax << "\t" << yMax << std::endl;
	xMin=std::min(fun->GetMinimumX()-7*sigma,xMin);
	xMax=std::max(fun->GetMinimumX()+7*sigma,xMax);
	
	xMin=std::max(xMin,0.96);
	xMax=std::min(xMax,1.04);
	yMax=std::max(yMax, std::max(fun->Eval(xMin),fun->Eval(xMax)));
	std::cout << "EEEEEEEEEEEEEEEEE" << xMin << "\t" << xMax << std::endl;
	g->GetXaxis()->SetRangeUser(xMin, xMax); 
	g->GetYaxis()->SetRangeUser(-5, yMax); //std::max(fun->Eval(xMin), fun->Eval(xMax)));

	g->Draw("AP");
	g->GetYaxis()->SetTitle("#Delta L (a. u.)");
	g->GetXaxis()->SetTitle("1+#DeltaP ");

      }else {
	g->GetXaxis()->SetRangeUser(std::max(fun->GetXmin()-2*sigma,0.), std::min(fun->GetXmax()+sigma,0.15));
	//g->GetXaxis()->SetRangeUser(0., 0.05);
	g->GetYaxis()->SetRangeUser(-5, std::max(fun->Eval(fun->GetXmin()-1.5*sigma), fun->Eval(fun->GetXmax()+1.5*sigma)));
	g->Draw("AP");
	g->GetYaxis()->SetTitle("#Delta L (a. u.)");
	if(variable.Contains("constTerm")) g->GetXaxis()->SetTitle("#Delta C");
	if(variable.Contains("alpha")) g->GetXaxis()->SetTitle("#Delta S");
	if(variable.Contains("rho")) g->GetXaxis()->SetTitle("#rho");
	if(variable.Contains("phi")){
	  g->GetXaxis()->SetTitle("#phi");
	  g->GetXaxis()->SetRangeUser(0,1.6);
	  //g->GetYaxis()->SetRangeUser(-5, 30);
	}

      }

      fun->SetLineWidth(3);
      fun->SetLineColor(4);
      fun->Draw("same");

      char out_char[100];
      if(isScale){
	sprintf(out_char,"%s #DeltaP = %.3f ^{+ %.3f}_{- %.3f} %s" , "", //region.Data(), 
		(fun->GetParameter(1) -1 )*100 ,
		100./(sqrt(2* fun->GetParameter(3))), 100./(sqrt(2* fun->GetParameter(2))) , "%");
      } else if(isPhi){
	sprintf(out_char,"%s #Delta#phi = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(), 
		fun->GetParameter(1),
		1./(sqrt(2* fun->GetParameter(3))), 1./(sqrt(2* fun->GetParameter(2))), "");
	//std::cout << out_char << std::endl;
      } else {
	sprintf(out_char,"%s #Delta#sigma = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(), 
		fun->GetParameter(1)*100,
		1./(sqrt(2* fun->GetParameter(3))) *100, 1./(sqrt(2* fun->GetParameter(2))) *100, "%");
	//std::cout << out_char << std::endl;
      }

      pt->Clear();
      pt->AddText(out_char);
      pt->Draw();
      return;
}



void FitProfile2(TString filename, TString energy="8 TeV", TString lumi="", bool doScale=true, bool doResolution=true, bool doPhi=false){
  gStyle->SetOptFit(0);  
  std::ofstream ffout("tmp/out.out");

  //  TString region_set=filename;
  //region_set.Remove(0,region_set.First('/')+1);
  //  std::cout << "REGION SET = " << region_set << std::endl;
  //  region_set.Remove(region_set.First("/"));
  
  typedef std::vector<TGraph *> g_vec_t;
  typedef  std::map<TString, g_vec_t> map_region_t;
  std::map<TString, map_region_t> graph_map;
  
  //std::cout << "Opening file: " << filename << std::endl;
  TFile f_in(filename,"open");
  if(!f_in.IsOpen()){
    std::cerr << "Error. File not open" << std::endl;
    return;
  }

  TList *KeyList = f_in.GetListOfKeys();
  for(int i =0; i <  KeyList->GetEntries(); i++){
    TKey *currentKey=(TKey *)KeyList->At(i);
    TString className=currentKey->GetClassName();
    // se non e' un TGraph passa oltre
    //std::cout << i << " " << className << std::endl;
    if (! className.CompareTo("TGraph")==0) continue;
    TGraph *g = (TGraph *)currentKey->ReadObj(); 
    

    TString name=g->GetName();
    //std::cout << "name = " << name << std::endl;
    name.Remove(0,name.First('_')+1);
    TString variable=name(0,name.First('_'));
    name.Remove(0,name.First('_')+1);
    TString region=name(0,name.Last('_'));
    name.Remove(0,name.Last('_')+1);
    TString index=name(0,name.Length());

    name=g->GetName();
    //std::cout << name << " " << variable << " " << region << " " << index << std::endl;    
    
    
    (graph_map[variable])[region].push_back(g);  
  }

  std::cout << "Create TPaveText" << std::endl;
  //  TPaveText *pt = new TPaveText(0.7,0.92,0.997,0.997,"NDC");
  //  pt->SetTextSize(0.033);
  TPaveText *pt = new TPaveText(0.65,0.91,0.997,0.997,"NDC");
  pt->SetTextSize(0.05);
  pt->SetFillColor(0);
  pt->SetBorderSize(0);
  TPaveText pave(0.182,0.92,0.48,0.99, "ndc");
  // 	pv->DeleteText();
  pave.SetFillColor(0);
  pave.SetTextAlign(12);
  pave.SetBorderSize(0);
  pave.SetTextSize(0.04);
  pave.AddText("CMS Preliminary");

  if(lumi.Sizeof()>1)  pave.AddText("#sqrt{s}="+energy+"   L="+lumi+" fb^{-1}");
  else   pave.AddText("#sqrt{s}=8 TeV");
  //  gPad->SetTopMargin(0.08391608);

  RooArgSet *vars= new RooArgSet();

  TCanvas *c = new TCanvas("c","c");
  c->cd();

  std::cout << "Looping" << std::endl;

  for(std::map<TString, map_region_t>::const_iterator map_itr=graph_map.begin(); 
      map_itr!=graph_map.end(); 
      map_itr++){

    for(map_region_t::const_iterator map_region_itr=map_itr->second.begin();
	map_region_itr!=map_itr->second.end();
	map_region_itr++){
      
      TString region= map_region_itr->first;
      TString variable = map_itr->first;
      bool isScale=map_itr->first.CompareTo("scale")==0;
      bool isPhi=map_itr->first.CompareTo("phi")==0;
      if(!(
	   (isScale && doScale==true) ||
	   (!isScale && !isPhi && doResolution==true) ||
	   (isPhi && doPhi==true)
	   )
	 ) continue;

      std::cout << region << " " << variable << "\t" << map_region_itr->second.size() << std::endl;

      float y_min=1e10;
      std::cout << "Getting g1 mean graph" << std::endl;
      //      TGraphErrors *g1 = MeanGraph2(map_region_itr->second,&y_min);
      //std::cout << "Getting g mean graph" << std::endl;

      TGraphErrors *g_sum = SumGraph(map_region_itr->second,true);
      TGraphErrors *g = MeanGraphNew(g_sum);
//       if(!variable.Contains("rho")){
// 	continue;
//       }
      
      if(g==NULL) continue;
      g->SetMarkerStyle(20);
      g->SetMarkerSize(0.9);
      
      //g_sum->SaveAs("tmp/g_sum-"+variable+"_"+region+".root");
      g->SaveAs("tmp/g_sum-"+variable+"_"+region+".root");
      g_sum->SaveAs("tmp/g_sumGraph-"+variable+"_"+region+".root");
      if(variable.Contains("alpha")){ //||variable.Contains("constTerm")){
	continue;
      }

      std::cout << "Fitting iteratively for minumum and error estimation" << std::endl;
      g->SetTitle(region);
      TF1* fun = IterMinimumFit(g, isScale, isPhi);
      if (!fun)
	{
	  std::cout << "HEY FIT HAD BIG PROBLEMS!" << std::endl;
	  return;
	}
      
      if(fun->GetParameter(2)<=0 && fun->GetParameter(3)<=0) continue;
      Plot(c, g, fun, pt, isScale, isPhi,variable);



      ffout << variable << "\t" <<  region  << "\t" 
	    << fun->GetParameter(0) << "\t" 
	    << fun->GetParameter(1) << "\t" 
	    << fun->GetParameter(2) << "\t" 
	    << fun->GetParameter(3) <<  std::endl;

      //ffout << "# " << region << "\t" << variable << std::endl;
//       float sigma=1./(sqrt(2* std::max(fun->GetParameter(2),fun->GetParameter(3))));
      
      
//       TH2F a("a","a",
// 	     10, max(fun->GetParameter(1)-20.*1./(sqrt(2* fun->GetParameter(2))),0.96),
// 	     min(fun->GetParameter(1) +20. * 1./(sqrt(2* fun->GetParameter(2))),1.04),
// 	     10,-1.,100.);
      
//       a.GetYaxis()->SetTitle("#Delta L (a. u.)");
//       c->Clear();
//       if(isScale) {
	
//        	a.GetXaxis()->SetTitle("1+#DeltaP ");
// 	//	a.Draw();
// 	//g->GetXaxis()->SetRangeUser(std::max(fun->GetXmin()-sigma,0.), std::min(fun->GetXmax()+sigma,0.15));
// 	//	g->Draw("PSAME");

// 	Double_t xMin, xMax;
// 	Double_t yMax = rangeWithPoints(g, 8, &xMin, &xMax);
// 	std::cout << "AAAAAAAAAAAAAAAa" << xMin << "\t" << xMax << "\t" << sigma << "\t" << fun->GetXmin() << std::endl;
// 	std::cout << "AAAAAAAAAAAAAAAa" << xMin << "\t" << xMax << "\t" << yMax << std::endl;
// 	xMin=std::min((xMin+xMax)/2.-sigma,xMin);
// 	xMax=std::max((xMin+xMax)/2.+sigma,xMax);
	
// 	xMin=std::max(xMin,0.96);
// 	xMax=std::min(xMax,1.04);
// 	std::cout << "EEEEEEEEEEEEEEEEE" << xMin << "\t" << xMax << std::endl;
// 	g->GetXaxis()->SetRangeUser(xMin, xMax); 
// 	g->GetYaxis()->SetRangeUser(-5, yMax); //std::max(fun->Eval(xMin), fun->Eval(xMax)));

// 	g->Draw("AP");
// 	g->GetYaxis()->SetTitle("#Delta L (a. u.)");
// 	g->GetXaxis()->SetTitle("1+#DeltaP ");

//       }else {
// 	g->GetXaxis()->SetRangeUser(std::max(fun->GetXmin()-2*sigma,0.), std::min(fun->GetXmax()+sigma,0.15));
// 	//g->GetXaxis()->SetRangeUser(0., 0.05);
// 	g->GetYaxis()->SetRangeUser(-5, std::max(fun->Eval(fun->GetXmin()-1.5*sigma), fun->Eval(fun->GetXmax()+1.5*sigma)));
// 	g->Draw("AP");
// 	g->GetYaxis()->SetTitle("#Delta L (a. u.)");
// 	if(variable.Contains("constTerm")) g->GetXaxis()->SetTitle("#Delta C");
// 	if(variable.Contains("alpha")) g->GetXaxis()->SetTitle("#Delta S");
// 	if(variable.Contains("rho")) g->GetXaxis()->SetTitle("#rho");
// 	if(variable.Contains("phi")){
// 	  g->GetXaxis()->SetTitle("#phi");
// 	  g->GetXaxis()->SetRangeUser(0,1.6);
// 	  g->GetYaxis()->SetRangeUser(-5, 30);
// 	}

//       }

//       fun->SetLineWidth(3);
//       fun->SetLineColor(4);
//       fun->Draw("same");

//       char out_char[100];
//       if(isScale){
// 	sprintf(out_char,"%s #DeltaP = %.3f ^{+ %.3f}_{- %.3f} %s" , "", //region.Data(), 
// 		(fun->GetParameter(1) -1 )*100 ,
// 		100./(sqrt(2* fun->GetParameter(3))), 100./(sqrt(2* fun->GetParameter(2))) , "%");
//       } else if(isPhi){
// 	sprintf(out_char,"%s #Delta#phi = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(), 
// 		fun->GetParameter(1),
// 		1./(sqrt(2* fun->GetParameter(3))), 1./(sqrt(2* fun->GetParameter(2))), "");
// 	//std::cout << out_char << std::endl;
//       } else {
// 	sprintf(out_char,"%s #Delta#sigma = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(), 
// 		fun->GetParameter(1)*100,
// 		1./(sqrt(2* fun->GetParameter(3))) *100, 1./(sqrt(2* fun->GetParameter(2))) *100, "%");
// 	//std::cout << out_char << std::endl;
//       }

//       pt->Clear();
//       pt->AddText(out_char);
//       pt->Draw();
      pave.Draw();
      //      pt->Print();
      
      RooRealVar *var_ = new RooRealVar(variable+"_"+region,variable+"_"+region, fun->GetParameter(1), fun->GetParameter(1)-1./(sqrt(2* fun->GetParameter(2))) , fun->GetParameter(1) + 1./(sqrt(2* fun->GetParameter(3))));  
      //      var_->setAsymError(1./(sqrt(2* fun->GetParameter(2))),1./(sqrt(2* fun->GetParameter(3))));
      var_->setError((1./(sqrt(2* fun->GetParameter(2))) + 1./(sqrt(2* fun->GetParameter(3))))/2);
      var_->Print();
      std::cout << fixed << "Scale shift = " << fun->GetParameter(1) << " + " << 1./(sqrt(2* fun->GetParameter(3))) << " - " << 1./(sqrt(2* fun->GetParameter(2))) << "" << std::endl;
      
      

      gPad->Update();
      gPad->SaveAs(img_filename(filename, variable, region));
      gPad->SaveAs(img_filename(filename, variable, region,".C"));
      gPad->SaveAs(img_filename(filename, variable, region,".png"));
      vars->add(*var_);
    }
  }
  

  vars->Print();
  vars->writeToFile(img_filename(filename,"FitResult-","",".config"));
  system("cat "+img_filename(filename,"FitResult-","",".config")+" |awk '{printf(\"%s = %.4f +/- %.6f \", $1, $3, $5);print $6,$7,$8}' > "+img_filename(filename,"FitResult","",".config"));
  delete vars;
  delete pt;

  return;
}


  
  
TH1F *MinimumProfile(TString filename, TString varname, TString regionname){
  gStyle->SetOptFit(0);  

  TH1F *minHist = NULL;
  if(varname.Contains("rho") || varname.Contains("constTerm")) minHist = new TH1F(varname+"_hist", "", 1000,0.005,0.015);
  if(varname.Contains("alpha")) minHist = new TH1F(varname+"_hist", "", 600,0.00,0.06);
  //  TString region_set=filename;
  //region_set.Remove(0,region_set.First('/')+1);
  //  std::cout << "REGION SET = " << region_set << std::endl;
  //  region_set.Remove(region_set.First("/"));
  
  typedef std::vector<TGraph *> g_vec_t;
  typedef  std::map<TString, g_vec_t> map_region_t;
  std::map<TString, map_region_t> graph_map;
  
  std::cout << "Opening file: " << filename << std::endl;
  TFile f_in(filename,"open");
  if(!f_in.IsOpen()){
    std::cerr << "Error. File not open" << std::endl;
    return NULL;
  }

  TList *KeyList = f_in.GetListOfKeys();
  for(int i =0; i <  KeyList->GetEntries(); i++){
    TKey *currentKey=(TKey *)KeyList->At(i);
    TString className=currentKey->GetClassName();
    // se non e' un TGraph passa oltre
    //std::cout << i << " " << className << std::endl;
    if (! className.CompareTo("TGraph")==0) continue;
    TGraph *g = (TGraph *)currentKey->ReadObj(); 
    

    TString name=g->GetName();
    //    std::cout << "name = " << name << std::endl;
    name.Remove(0,name.First('_')+1);
    TString variable=name(0,name.First('_'));
    name.Remove(0,name.First('_')+1);
    TString region=name(0,name.Last('_'));
    name.Remove(0,name.Last('_')+1);
    TString index=name(0,name.Length());

    if(!variable.Contains(varname)) continue;
    if(!region.Contains(regionname)) continue;
    name=g->GetName();
    //    std::cout << name << " " << variable << " " << region << " " << index << std::endl;    
    
    
    (graph_map[variable])[region].push_back(g);  
  }

  RooArgSet *vars= new RooArgSet();

  TCanvas *c = new TCanvas("c","c");
  c->cd();

  std::cout << "Looping" << std::endl;
  for(std::map<TString, map_region_t>::const_iterator map_itr=graph_map.begin(); 
      map_itr!=graph_map.end(); 
      map_itr++){

    for(map_region_t::const_iterator map_region_itr=map_itr->second.begin();
	map_region_itr!=map_itr->second.end();
	map_region_itr++){
      
      TString region= map_region_itr->first;
      TString variable = map_itr->first;

      for(std::vector<TGraph *>::const_iterator g_itr = map_region_itr->second.begin();
	  g_itr!= map_region_itr->second.end();
	  g_itr++){ // loop over TGraphs
	
	
	TGraph *graph = *g_itr;
	if(graph==NULL) continue; // per sicurezza
	Double_t *Y = graph->GetY();
	Double_t *X = graph->GetX();
	Int_t N = graph->GetN();

	double minimum=Y[0];
	int iMin=0;
	for(int i=0; i< N; i++){ // calcolo il minimo e il punto di minimo
	  if(minimum > Y[i]){
	    minimum=Y[i];
	    iMin=i;
	  }
	}
	
	minHist->Fill(X[iMin]);
      }
    }
  }
  return minHist;
}


void propagate(TString filename="tmp/out.out", TString myRegion="absEta_0_1-bad-Et_20-trigger-noPF",  Double_t Emean=7.179){

  gROOT->ProcessLine(".L macro/macro_fit.C+");
  
  std::ifstream ffin(filename);
  TString variable, region; 
  Double_t rho_par[4], phi_par[4];

  while(ffin.peek()!=EOF && ffin.good()){
    ffin >> variable >> region;
    if(region!=myRegion) continue;
    if(variable=="rho") ffin >> rho_par[0] >> rho_par[1] >> rho_par[2] >> rho_par[3];
    if(variable=="phi") ffin >> phi_par[0] >> phi_par[1] >> phi_par[2] >> phi_par[3];
  }

  TH1F *DeltaC = new TH1F("DeltaC", "", 200,0,0.05);
  TH1F *DeltaS = new TH1F("DeltaS", "", 200,0,0.4);
  
  
  Get_DeltaC_DeltaS(DeltaC, DeltaS, rho_par, phi_par, Emean);

  std::cout << "Delta C" << " " << myRegion << "\t" << DeltaC->GetMean() << "\t" 
	    << DeltaC->GetRMS() << std::endl;
  std::cout << "Delta S" << " " << myRegion << "\t" << DeltaS->GetMean() << "\t" 
	    << DeltaS->GetRMS() << std::endl;

}


void propagate(TString filename, int region){
  TString myRegion="";
  Double_t Emean=0;
  if(region==0){myRegion="absEta_0_1-bad-Et_20-trigger-noPF-EB";        Emean=7.179;}
  if(region==1){myRegion="absEta_0_1-gold-Et_20-trigger-noPF-EB";       Emean=7.051;}
  if(region==2){myRegion="absEta_1_1.4442-bad-Et_20-trigger-noPF-EB";   Emean=8.712;}
  if(region==3){myRegion="absEta_1_1.4442-gold-Et_20-trigger-noPF-EB";  Emean=8.08; }

  propagate(filename, myRegion, Emean);
}



