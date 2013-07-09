#include <TGraphErrors.h>
#include <TF1.h>
#include <TFitResult.h>
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

#include "TCanvas.h"
#include "TPostScript.h"
#include "TStopwatch.h"

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


#include <TRandom3.h>

#define FIT_LIMIT 0.01
#define ITER_MAX 20

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

TF1* IterMinimumFit(TGraphErrors *g, RooRealVar *xVar)
{
  double range_min;// = min-40*sigma;
  double range_max;// = min+30*sigma;   
  
  Double_t *X = g->GetX();
  Double_t X_bin=fabs(X[1]-X[0]);

  range_min=TMath::Max(TMath::MinElement(g->GetN(),g->GetX()),xVar->getMin());
  range_max=TMath::Min(TMath::MaxElement(g->GetN(),g->GetX()),xVar->getMax());
  
  g->Fit("pol2","0FR+","",range_min,range_max);
  TFitResult fitres;

  TF1 *fun = (TF1*) g->GetListOfFunctions()->At(g->GetListOfFunctions()->GetSize()-1);
  if(fun==NULL){
    std::cerr << "Error: polynomial fit function not defined" << std::endl;
    exit(1);
  }
  fun->SetRange(range_min,range_max);
  fun->GetRange(range_min, range_max);
#ifdef DEBUG
  fun->Print();
#endif
  //fun->SetRange(0.95,1.04);
  //  double sigma_old; 

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
  f2->SetParameter(0,TMath::MinElement(g->GetN(),g->GetY()));
  f2->SetParameter(1,minX);
  f2->SetParameter(2,1/(2*sigma*sigma));
  f2->SetParameter(3,1/(2*sigma*sigma));
  int iter=0;
  do{
    old_chi2=chi2;

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

    range_min = TMath::Max(minX-9*sigma_minus,xVar->getMin());
    range_max = TMath::Min(minX+12*sigma_plus,xVar->getMax());   
    //range_min = TMath::Max(minX-2*sigma_minus,0.);
    //  range_max = TMath::Min(minX+2*sigma_plus,0.07);   

    while (((range_max - range_min)/X_bin < 10)){
      // incremento del 10% i range
      std::cout << "[WARNING] Incrementing ranges by 20% because less than 10 points in range" << std::endl;
      std::cout << "Old range: [ " << range_min << " : " << range_max << "]" << std::endl;
      range_min-=(range_max-range_min)*0.2;
      range_max+=(range_max-range_min)*0.2;
    }

    f2->SetRange(range_min,range_max);
    //     std::cout << minX_old << " " << minX << " " << minX-minX_old << std::endl;
    if(sigma_plus*100 < 0.01) std::cout << "sigma_plus = " << " " << sigma_plus << std::endl;
    if(sigma_minus*100 < 0.01) std::cout << "sigma_minus = " << " " << sigma_minus << std::endl;
    iter++;
    f2->SetFitResult(fitres);
    
#ifdef DEBUG
    std::cout << "MINUIT STATUS " << fitres.Status() << std::endl; //gMinuit->GetStatus() << std::endl;
    std::cout << "CHI2/NDF" << chi2 << std::endl;
#endif
    //Rescaling graph points so that minimum iteratively goes to 0
    for (int iPoint=0;iPoint<g->GetN();++iPoint)
      *(g->GetY()+iPoint)=*(g->GetY()+iPoint)-f2->GetParameter(0);
    //} While( (((fabs(minX-minX_old)/minX > FIT_LIMIT || fabs(sigma_plus-sigma_plus_old)/sigma_plus>FIT_LIMIT || fabs(sigma_minus-sigma_minus_old)/sigma_minus>FIT_LIMIT )) && iter<20) || iter<10 ); 
  } while (fabs(chi2 - old_chi2)/old_chi2 > FIT_LIMIT && (iter < ITER_MAX || iter < 4 || fitres.Status() != 0));
  
  std::cout << "ITER to converge = " << iter << std::endl;
  if(iter >=ITER_MAX) std::cout << "WARNING: iter not converged" << std::endl;
  
  //  float chi2_rnd = TMath::ChisquareQuantile(gen.Rndm(),1);

  float errorScale=sqrt(f2->GetChisquare()/(f2->GetNumberFitPoints()-f2->GetNumberFreeParameters()));
  std::cout << "SCALING ERROR by " << errorScale;
  for (int i=0;i<g->GetN();++i)
    *(g->GetEY()+i)= (*(g->GetEY()+i)) * errorScale; //*chi2_rnd/TMath::ChisquareQuantile(0.5,1);

  

  return f2;
}
