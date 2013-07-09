//#include "./src/SetRegionVec.cc"
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

//#define DEBUG
//TString filename=outPrefix+"-"+region_set+"-outProfile.root";

#define FIT_LIMIT 0.01
#define ITER_MAX 6

TRandom3 gen(0);

#define NOT_CONVERGED_LIMIT 0.6

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
  Double_t min=Y[0];
  for(int i=0; i < g->GetN(); i++) if(Y[i] <= min){
    iMin=i;
    min=Y[i];
  }

  int _iMax=std::min(g->GetN(),iMin+ nPoints);  
  int _iMin=std::max(0,iMin- nPoints);

  double dist = std::max(X[_iMax]-X[iMin],X[iMin]-X[_iMin]);
  (*xMin)= X[iMin]-dist;
  (*xMax)= X[iMin]+dist;
  return std::max(Y[_iMin],Y[_iMax]);
}


TF1* IterMinimumFit(TGraphErrors *g, bool isScale)
{
#ifdef DEBUG
  g->SaveAs("ciao.root");
#endif
  double rangeLimMax=1.04;
  double rangeLimMin=0.96;
  if(!isScale){
    rangeLimMax=0.05;
    rangeLimMin=0.;
  }

  Double_t *X = g->GetX();
  Double_t X_bin=fabs(X[1]-X[0]);
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
  double sigma_plus=sigma;
  double sigma_minus=sigma;
  double minX=g->GetX()[TMath::LocMin(g->GetN(),g->GetY())];

  double old_chi2=0., chi2=0.;
   
  //  range_max=range_min+0.01;
  TF1* f2 = new TF1("f2",asymmetricParabola,range_min,range_max,4);
  f2->SetParameter(0,0); //TMath::MinElement(g->GetN(),g->GetY()));
  f2->SetParameter(1,minX);
  f2->SetParameter(2,1/(2*sigma*sigma));
  f2->SetParameter(3,1/(2*sigma*sigma));
  int iter=0;
  do{
    old_chi2=chi2;

    minX_old=minX;
    sigma_plus_old=sigma_plus;
    sigma_minus_old=sigma_minus;

    std::cout << "Fit range: [ " << range_min << " : " << range_max << "]" << std::endl;
    f2->SetName("f2_iter"+iter); 
 
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
      range_min = TMath::Max(minX-30*sigma_minus, rangeLimMin);
      range_max = TMath::Min(minX+30*sigma_plus, rangeLimMax);   
    } else {
      range_min = TMath::Max(minX-2.5*sigma_minus, rangeLimMin);
      range_max = TMath::Min(minX+4*sigma_plus,  rangeLimMax);   
    }

    bool rep=true;
    while (rep){ //((range_max - range_min)/X_bin < 10)){
      rep= (pointsInInterval(X, N, range_min, range_max) < 8);
      // incremento del 10% i range
      std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
      std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
      
      range_min = TMath::Max(range_min-(range_max-range_min)*0.2, rangeLimMin);
      range_max = TMath::Min(range_max+(range_max-range_min)*0.2, rangeLimMax);   
      if(range_min==rangeLimMin && range_max==rangeLimMax) rep=false;
    }

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
  } while (fabs(chi2 - old_chi2)/old_chi2 > FIT_LIMIT && iter < ITER_MAX); //|| iter < 4 || gMinuit->GetStatus() != 0);
  
  std::cout << "ITER to converge = " << iter << std::endl;
  if(iter >=ITER_MAX) std::cout << "WARNING: iter not converged" << std::endl;

  //  float chi2_rnd = TMath::ChisquareQuantile(gen.Rndm(),1);

  float errorScale=sqrt(f2->GetChisquare()/(f2->GetNumberFitPoints()-f2->GetNumberFreeParameters()));
  std::cout << "SCALING ERROR by " << errorScale;
  for (int i=0;i<g->GetN();++i)
    *(g->GetEY()+i)= (*(g->GetEY()+i)) * errorScale; //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);

  

  return f2;
}

typedef struct{
  double y;
  double y2;
  int n;
} point_value_t;

typedef  std::map<Double_t, point_value_t> graphic_t;


TGraphErrors *MeanGraph(std::vector<TGraph *> g_vec, float *y_minimum=NULL, TGraphErrors *g_in=NULL){
  int N = (*g_vec.begin())->GetN(); // number of points
  if(1000 <= N) {
    std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
    return NULL;
  }

  
  Double_t x[1000]={0.}, y[1000]={0.}, y2[1000]={0.}, dev_st[1000], err_x[1000];
  
  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
    TGraph *graph = *g_itr;
    if(graph==NULL) continue; // per sicurezza
    Double_t *Y = graph->GetY();
    Double_t *X = graph->GetX();

    double minimum=Y[0];
    for(int i=1; i< N; i++){ // calcolo il minimo e il punto di minimo
      if(minimum > Y[i]) minimum=Y[i];
    }

    for (int ip=0;ip<N;++ip) //sottraggo il minimo per avere un DeltaL in ordinata
      {
	x[ip]=X[ip];
	y[ip]=Y[ip]-minimum;
	//std::cout << ip << "\t" << x[ip] << "\t" << y[ip] << "\t" << Y[ip] << std::endl;
	err_x[ip]=0;
	dev_st[ip]=0;
      }
    if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;
  }
  
  TGraphErrors *g = new TGraphErrors(N, x, y, err_x, dev_st);
  g->SaveAs("c.root");
  //  g->Draw("AP*");
  return g;
}

TGraphErrors *MeanGraphOld(std::vector<TGraph *> g_vec, float *y_minimum=NULL, TGraphErrors *g_in=NULL){
  
  int N = (*g_vec.begin())->GetN(); // number of points
  if(1000 <= N) {
    std::cerr << "ERROR: N_bins too big in MeanGraph:" << N << std::endl;
    return NULL;
  }

  std::map<Double_t, std::pair<Double_t, Double_t> > g_in_map;
  if(g_in!=NULL){
    Double_t X_in=0, Y_in=0, YE_in=0;
    for(int i =0; i < g_in->GetN(); i++){
      g_in->GetPoint(i, X_in, Y_in);
      //      std::cout << Y_in << std::endl;
      YE_in = g_in->GetErrorY(i);
      std::pair<Double_t, Double_t> point = std::make_pair<Double_t, Double_t>(Y_in, YE_in);
      //std::cout << i << "\t" << X_in << "\t" << Y_in << std::endl;
      g_in_map.insert(std::make_pair<Double_t, std::pair<Double_t, Double_t> >(X_in, point));
    }
  }
  
  Double_t x[1000]={0.}, y[1000]={0.}, y2[1000]={0.}, dev_st[1000], err_x[1000];
  
  graphic_t graphic;
  int n_max=0;
  
  double y_min=1e8;  
  //  if(y_minimum!=NULL) y_min=y_minimum;
  for(std::vector<TGraph *>::const_iterator g_itr = g_vec.begin();
      g_itr!= g_vec.end();
      g_itr++){ // loop over TGraphs
    
    TGraph *graph = *g_itr;
    if(graph==NULL) continue; // per sicurezza
    Double_t *Y = graph->GetY();
    Double_t *X = graph->GetX();
    Double_t X_bin=fabs(X[1]-X[0]);
    //    double minimum=TMath::MinElement(graph->GetN(),graph->GetY());
    double minimum=Y[0];
    double x_minimum=X[0];
    for(int i=1; i<graph->GetN(); i++){ // calcolo il minimo e il punto di minimo
      if(minimum > Y[i]){
	//std::cout << minimum << "\t" << Y[i] << std::endl;
	minimum=Y[i];
	x_minimum=X[i];
      }
    }

    // cerco di capire se il fit di migrad non e' andato a convergenza
    if(fabs(x_minimum - (X[graph->GetN()-1] + X[0])/2.) > NOT_CONVERGED_LIMIT * (X[graph->GetN()-1] - X[0])){ // shift 10% rispetto all'intervallo dal minimo di migrad
      std::cout << "[WARNING] Graph not converged" << std::endl;
      std::cout << X[1] << "\t" << X[graph->GetN()-1] << "\t" << x_minimum << "\t" << X_bin << std::endl;
      continue; // passo al graph successivo
    }

    //    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
    for (int ip=0;ip<graph->GetN();++ip) //sottraggo il minimo per avere un DeltaL in ordinata
      {

	Y[ip] -= minimum;
	//	std::cout << ip << ";" << X[ip] << ";" << Y[ip] << std::endl;
      }

    //    std::cout << "XBIN = " << X_bin << std::endl;
    //    std::cout << "Y[7] = " << Y[7] << std::endl;
    if(graph->GetN() < 3) std::cerr << "WARNING: low number of points" << std::endl;

    for(int i =0 ; i < graph->GetN(); i++){
#ifdef DEBUG
      std::cout << "[DEBUG] i = " << i << std::endl;
#endif
      graphic_t::iterator itr = graphic.find(X[i]);
      if(!graphic.empty()){
	//	graphic_t::iterator itr = graphic.find(X[i]);
	
	if(itr == graphic.end()) {
	  // controllo se e' vicino al primo bordo, poi al secondo e altrimenti lo butto
	  itr = graphic.lower_bound(X[i]);
	  //	  if(itr== graphic.end()) std::cout << "Error: graphic empty" << std::endl;
	  itr--;
	  if( !(fabs(itr->first - X[i]) < X_bin/5.)){ // check del bin prima
	    itr++;
	    if( !(fabs(itr->first - X[i]) < X_bin/5.)){ // check del bin
	      itr++;
	      if( !(fabs(itr->first - X[i]) < X_bin/5.)){ // check del bin dopo
#ifdef DEBUG
		if(X[i] < graphic.begin()->first){
		  std::cout << "Value not found: " << X[i] << "; "
			    << graphic.lower_bound(X[i])->first << "; "
			    << graphic.upper_bound(X[i])->first << "; "
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
	//    itr->first=x[i];
	//std::cout << Y[i] - g_in[itr->first].y << std::endl;
	if(g_in!=NULL){
	  if(g_in_map.find(X[i])!=g_in_map.end()){
	    if( fabs(Y[i] - *y_minimum - g_in_map[X[i]].first) > 3*g_in_map[X[i]].second * sqrt(200)){
	      std::cout << "Not adding: " << X[i] << "\t" << Y[i] - *y_minimum
			<< "\t" << g_in_map[X[i]].first << "\t" << Y[i] - *y_minimum - g_in_map[X[i]].first 
			<< "\t" << 3*g_in_map[X[i]].second * sqrt(150)
			<< std::endl;
	      continue;
	    }
	  }
	}
	itr->second.y+=Y[i];
	itr->second.y2+=Y[i]*Y[i];
	itr->second.n++;
	n_max = std::max(n_max,itr->second.n);
      } else {
	if(g_itr!=g_vec.begin())	std::cout << "added point" << std::endl;
	// se non c'e' lo aggiungo
	point_value_t p;
	p.y=Y[i];
	p.y2=Y[i]*Y[i];
	p.n=1;
	n_max = std::max(n_max,1);
	graphic.insert(std::make_pair<Double_t, point_value_t>(X[i],p));
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
    if(itr->second.n < n_max-n_max*0.2){std::cout << "Removing point " << itr->first << std::endl; continue;}
    if(itr->second.n != n_max) std::cout << "n = " << itr->second.n << " " << n_max << std::endl;
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




void FitProfile2(TString filename, TString energy="8 TeV", TString lumi=""){
  gStyle->SetOptFit(0);  


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
    return;
  }

  TList *KeyList = f_in.GetListOfKeys();
  for(int i =0; i <  KeyList->GetEntries(); i++){
    TKey *currentKey=(TKey *)KeyList->At(i);
    TString className=currentKey->GetClassName();
    // se non e' un TGraph passa oltre
    std::cout << i << " " << className << std::endl;
    if (! className.CompareTo("TGraph")==0) continue;
    TGraph *g = (TGraph *)currentKey->ReadObj(); 
    

    TString name=g->GetName();
    std::cout << "name = " << name << std::endl;
    name.Remove(0,name.First('_')+1);
    TString variable=name(0,name.First('_'));
    name.Remove(0,name.First('_')+1);
    TString region=name(0,name.Length());
    TString index="0";

    name=g->GetName();
    std::cout << name << " " << variable << " " << region << " " << index << std::endl;    
    
  
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
  else   pave.AddText("#sqrt{s}=7 TeV");
  //  gPad->SetTopMargin(0.08391608);

  RooArgSet *vars= new RooArgSet();


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
      //if(isScale) continue;

      std::cout << region << " " << variable << std::endl;

      float y_min=1e10;
      std::cout << "Getting g1 mean graph" << std::endl;
      TGraphErrors *g1 = MeanGraph(map_region_itr->second,&y_min);
      //std::cout << "Getting g mean graph" << std::endl;
      TGraphErrors *g = g1; //MeanGraph(map_region_itr->second, &y_min,g1);

      
      if(g==NULL) continue;
      g->SetMarkerStyle(20);
      g->SetMarkerSize(0.9);
      
      std::cout << "Fitting iteratively for minumum and error estimation" << std::endl;
      g->SaveAs(variable+"_"+region+".root");
      
      TF1* fun = IterMinimumFit(g, isScale);
      if (!fun)
	{
	  std::cout << "HEY FIT HAD BIG PROBLEMS!" << std::endl;
	  return;
	}
      
      float sigma=1./(sqrt(2* fun->GetParameter(2)));
      
      
      TH2F a("a","a",
	     10, max(fun->GetParameter(1)-20.*1./(sqrt(2* fun->GetParameter(2))),0.96),
	     min(fun->GetParameter(1) +20. * 1./(sqrt(2* fun->GetParameter(2))),1.04),
	     10,-1.,100.);
      
      a.GetYaxis()->SetTitle("#Delta L (a. u.)");
      if(isScale) {
       	a.GetXaxis()->SetTitle("1+#DeltaP ");
	//	a.Draw();
	//g->GetXaxis()->SetRangeUser(std::max(fun->GetXmin()-sigma,0.), std::min(fun->GetXmax()+sigma,0.15));
	//	g->Draw("PSAME");

	Double_t xMin, xMax;
	Double_t yMax = rangeWithPoints(g, 8, &xMin, &xMax);
	std::cout << "AAAAAAAAAAAAAAAa" << xMin << "\t" << xMax << "\t" << sigma << "\t" << fun->GetXmin() << std::endl;
	std::cout << "AAAAAAAAAAAAAAAa" << xMin << "\t" << xMax << "\t" << yMax << std::endl;
	xMin=std::min((xMin+xMax)/2.-sigma,xMin);
	xMax=std::max((xMin+xMax)/2.+sigma,xMax);
	
	xMin=std::max(xMin,0.96);
	xMax=std::min(xMax,1.04);
	std::cout << "EEEEEEEEEEEEEEEEE" << xMin << "\t" << xMax << std::endl;
	g->GetXaxis()->SetRangeUser(xMin, xMax); 
	g->GetYaxis()->SetRangeUser(-5, yMax); //std::max(fun->Eval(xMin), fun->Eval(xMax)));

	g->Draw("AP");
	g->GetYaxis()->SetTitle("#Delta L (a. u.)");
	g->GetXaxis()->SetTitle("1+#DeltaP ");

      }else{
	g->GetXaxis()->SetRangeUser(std::max(fun->GetXmin()-sigma,0.), std::min(fun->GetXmax()+sigma,0.15));
	g->GetYaxis()->SetRangeUser(-5, std::max(fun->Eval(fun->GetXmin()-1.5*sigma), fun->Eval(fun->GetXmax()+1.5*sigma)));
	g->Draw("AP");
	g->GetYaxis()->SetTitle("#Delta L (a. u.)");
	g->GetXaxis()->SetTitle("#Delta#sigma");
	//g->GetYaxis()->SetRangeUser(0,2);
      }

      fun->SetLineWidth(3);
      fun->SetLineColor(4);
      fun->Draw("same");

      char out_char[100];
      if(isScale){
	sprintf(out_char,"%s #DeltaP = %.3f ^{+ %.3f}_{- %.3f} %s" , "", //region.Data(), 
		(fun->GetParameter(1) -1 )*100 ,
		100./(sqrt(2* fun->GetParameter(3))), 100./(sqrt(2* fun->GetParameter(2))) , "%");
      } else {
	sprintf(out_char,"%s #Delta#sigma = %.2f ^{+ %.2f}_{- %.2f} %s" , "", //region.Data(), 
		fun->GetParameter(1)*100,
		1./(sqrt(2* fun->GetParameter(3))) *100, 1./(sqrt(2* fun->GetParameter(2))) *100, "%");
      }

      pt->Clear();
      pt->AddText(out_char);
      pt->Draw();
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
      vars->add(*var_);

    }
  }

  vars->Print();
  vars->writeToFile(img_filename(filename,"FitResult-","",".config"));
  system("cat "+img_filename(filename,"FitResult-","",".config")+" |awk '{printf(\"%s = %.4f +/- %.4f \", $1, $3, $5);print $6,$7,$8}' > "+img_filename(filename,"FitResult","",".config"));
  delete vars;
  delete pt;

  return;
}
