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

#include <RooMinuit.h>
#include <RooStats/UniformProposal.h>
#include <RooStats/PdfProposal.h>
//#include <RooStats/SequentialProposal.h>
#include <RooGaussian.h>
#include <RooStats/MetropolisHastings.h>
#include "RooBinning.h"
//#include "../src/ShervinMinuit.cc"
#include <RooStats/ProposalHelper.h>
#include <RooMultiVarGaussian.h>

#include <TRandom3.h>

#define FIT_LIMIT 0.01
#define ITER_MAX 20
//#define DEBUG
#define PROFILE_NBINS 2000

using namespace RooStats;

//Get Profile after smearing
TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level=1, bool warningEnable=true, bool trueEval=true);
TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level, bool warningEnable, bool trueEval){
  TString name(var->GetName());
  std::cout << "[STATUS] Getting profile for " << name << "\t" << level << "\t" << trueEval << std::endl;
  double minValue=var->getVal();
  double minYvalue=compatibility.evaluate();


  //std::cout << "Prof: " << name << "\t" << minValue << "\t" << minYvalue << std::endl;
  //double error=var->getError();

  double bin_width=0., range_min=0., range_max=0., sigma=0.;
  int nBin=0;
  switch(level){
  case -1:
    return GetProfile(var, compatibility, 5,false,false);
    break;
  case 0:
    TGraph *g1; g1 =GetProfile(var, compatibility, 1,false);
    TGraph *g2; g2 =GetProfile(var, compatibility, 2,false);
    Double_t X[PROFILE_NBINS], Y[PROFILE_NBINS];

    Int_t N_; N_=g1->GetN();
    Double_t *X_, *Y_; X_=g1->GetX(); Y_=g1->GetY();

    for(Int_t i=0; i < N_; i++){
      X[i]=X_[i];
      Y[i] =Y_[i];
    }
    nBin=N_;
    N_=g2->GetN();
    X_=g2->GetX(); Y_=g2->GetY();
    
    for(Int_t i=0; i < N_; i++){
      X[i+nBin]=X_[i];
      Y[i+nBin] =Y_[i];
    }
    nBin+=N_;
    return  new TGraph(nBin,X,Y);

    break;
  
  case 1:
    if(name.Contains("scale")){
      bin_width=0.005;
    } else if(name.Contains("const")){
      bin_width=0.005;
    } else if(name.Contains("alpha")){
      bin_width=0.01;
    } 

    range_min = var->getMin();
    range_max = var->getMax();
    
    break;
  case 2:
    if(name.Contains("scale")){
      bin_width=0.0005; // level1/10
      sigma=10*bin_width;    //
    }else  if(name.Contains("const")){
      bin_width=0.0005; //level1/10
      sigma=10*bin_width; // +/- 0.5%
    } else if(name.Contains("alpha")){
	bin_width=0.005;
	sigma=5*bin_width; // +/- 0.5%
    } 


    //sigma=var->getError();
    range_min = std::max(var->getMin(), minValue -  sigma);
    range_max = std::min(var->getMax(), minValue +  sigma);
    break;
  case 3:
    if(name.Contains("scale")){
      bin_width=0.0001;     // level2/5
      sigma=8*bin_width;    // 
    }else  if(name.Contains("const")){
      bin_width=0.0002;    // level2 /3
      sigma=6*bin_width;    
    } else if(name.Contains("alpha")){
	bin_width=0.003;
	sigma=5*bin_width; // +/- 0.5%
    } 

    //sigma=var->getError();
    range_min = std::max(var->getMin(), minValue - sigma); //4sigma
    range_max = std::min(var->getMax(), minValue + sigma);
    break;
  case 4:
    if(name.Contains("scale")){
      bin_width=0.0005; // level1/10
    }else  if(name.Contains("const")){
      bin_width=0.001; //level1/10
    } else if(name.Contains("alpha")){
	bin_width=0.005;
    } 

    range_min = var->getMin();
    range_max = var->getMax();
    break;
    
  case 5:
    if(name.Contains("scale")){
      bin_width=0.0001; // level1/10
    }else  if(name.Contains("const")){
      bin_width=0.0001; //level1/10
    } else if(name.Contains("alpha")){
      bin_width=0.001;
    } 
    range_min = var->getMin();
    range_max = var->getMax();
    break;
  }

    nBin=(int)((range_max-range_min)/bin_width);
#ifdef DEBUG
    std::cout << "nBin = " << nBin << std::endl;
#endif
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
   TStopwatch myClock;
   myClock.Start();
  for (int iVal=0;iVal<nBin;++iVal)
    {
      double value = range_min+bin_width*iVal;
#ifdef DEBUG
      //      std::cout << "testing function @ " << value << std::endl;
#endif

      xValues[iVal]=value;
      if(trueEval){
	var->setVal(value);
	chi2[iVal]=compatibility.evaluate(); //-minYvalue;
      } else chi2[iVal]=0;
      if (warningEnable && trueEval && chi2[iVal] < minYvalue ){
	std::cout << "[WARNING]: min in likelihood profile < min in MIGRAD: " << chi2[iVal]-minYvalue << " < " << minYvalue << std::endl;
	std::cout << "xValue = " << value << "\t" << "[" << range_min << ":" << range_max << "]" << std::endl;
      }
#ifdef DEBUG
      if(trueEval) std::cout << "value is " << chi2[iVal] << std::endl;
#endif
    }
  myClock.Stop();
  if(trueEval){
    std::cout << name << "\tlevel=" << level << "; nBins=" << nBin << "; range_min=" << range_min << "; range_max=" << range_max << "; bin_width=" << bin_width << "\t";
    myClock.Print();
    std::cout << "------------------------------" << std::endl;
  }

  var->setVal(minValue);
  TGraph *profil = new TGraph(nBin,xValues,chi2);
  return profil;
}

TTree *dataset2tree(RooDataSet *dataset){

  const RooArgSet *args = dataset->get();
  RooArgList argList(*args);

  Double_t variables[50];
  Long64_t nEntries= dataset->numEntries();
  //nEntries=1;
  TTree *tree = new TTree("tree","tree");
  tree->SetDirectory(0);
  TIterator *it1=NULL; 
  it1 = argList.createIterator();
  int index1=0;
  for(RooRealVar *var = (RooRealVar *) it1->Next(); var!=NULL;
      var = (RooRealVar *) it1->Next(),index1++){
    TString name(var->GetName());
    name.ReplaceAll("-","_");
    tree->Branch(name, &(variables[index1]), name+"/D");
  }

  //  tree->Print();

  for(Long64_t jentry=0; jentry<nEntries; jentry++){
    
    TIterator *it=NULL; 
    RooArgList argList1(*(dataset->get(jentry)));
    it = argList1.createIterator();
    //(dataset->get(jentry))->Print();
    int index=0;
    for(RooRealVar *var = (RooRealVar *) it->Next(); var!=NULL;
	var = (RooRealVar *) it->Next(), index++){
      variables[index]=var->getVal();
      //var->Print();
    }
   
    delete it;
    tree->Fill();
  }

  //  tree->Print();
  return tree;
}

TMatrixDSym* GetCovariance( RooStats::MarkovChain *chain, TString var1, TString var2){

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

bool MinMCMC2D(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, int iProfile, 
	       Double_t min_old, Double_t& min, long unsigned int nIter, bool update=true){ 
  Double_t chi2, chi2init=smearer.evaluate(); 
  bool changed=false;

  TStopwatch myClock;
  myClock.Start();

  Double_t v1=var1->getVal();
  Double_t v2=var2->getVal();

  // fix the scales to constant!
  RooArgSet floatingVars;
  RooArgList 	 argList_(smearer.GetParams());
  TIterator 	*it_ = argList_.createIterator();
  for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
    if (var->isConstant() || !var->isLValue()) continue;
    TString  name(var->GetName());
    if(name!=var1->GetName() && name!=var2->GetName()){
      floatingVars.add(*var);
      var->setConstant();
    }
  }
  delete it_;

  //creating MCMC proposal
  RooStats::ProposalHelper ph;
  ph.SetVariables(smearer.GetParams());
  if(iProfile>1){
    ph.SetCovMatrix(*smearer._markov.GetAsDataSet()->covarianceMatrix(smearer.GetParams())); ////*GetCovariance(&smearer._markov, var1->GetName(), var2->GetName()));
    ph.SetUniformFraction(0.2/iProfile);
    if(iProfile==2) ph.SetWidthRangeDivisor(5);
    if(iProfile==3) ph.SetWidthRangeDivisor(20);
  }else{
    ph.SetUniformFraction(1);
    ph.SetWidthRangeDivisor(1);
  }

  //running the mcmc
  RooStats::MetropolisHastings MCMC(smearer, smearer.GetParams(), *(ph.GetProposalFunction()), nIter);
  MCMC.SetNumBurnInSteps(10);
  MCMC.SetSign(RooStats::MetropolisHastings::kNegative);
  MCMC.SetType(RooStats::MetropolisHastings::kLog);
  RooStats::MarkovChain *mcChain = (MCMC.ConstructChain());

  //taking the results
  RooDataSet *dataset = mcChain->GetAsDataSet();
  TTree *tree=dataset2tree(dataset);
  TString nllVarName=mcChain->GetNLLVar()->GetName();
  chi2= tree->GetMinimum(nllVarName);
  std::cout << "[DEBUG] chi2 = tree->GetMinimum(nllVarName): " << chi2 << std::endl;
  TString selMin=nllVarName+" == "; selMin+=chi2;
  
  tree->Draw(">>entrylist",selMin,"entrylist");
  TEntryList *entrylist=(TEntryList*) gROOT->FindObject("entrylist");
  if(entrylist->GetN()!=1){
    std::cout << "Entrylist N=" << entrylist->GetN() << std::endl;
    return "";
  }
  Long64_t minEntry = entrylist->GetEntry(0);
  
  TString var1Name=var1->GetName(); var1Name.ReplaceAll("-","_");
  TString var2Name=var2->GetName(); var2Name.ReplaceAll("-","_");

  TObjArray *branchArray = tree->GetListOfBranches();
  for(int i=0; i < branchArray->GetEntriesFast(); i++){
    TBranch *b= (TBranch *) branchArray->At(i);
    TString name=b->GetName();
    if(name!=var1Name && name!=var2Name) continue;
    Double_t address;
    b->SetAddress(&address);
    b->GetEntry(minEntry);
    if(name==var1Name) var1->setVal(address);
    else var2->setVal(address);
  }

  chi2=smearer.evaluate(); // local minimum

  if(chi2<chi2init){
    if(chi2<min) min=chi2;
    if(update==true){// && (min<min_old)){
      std::cout << "[INFO] Updating variables:" << std::endl
		<< "       " << var1->GetName() << "\t" << var1->getVal() << std::endl
		<< "       " << var2->GetName() << "\t" << var2->getVal() << std::endl
		<< "       " << min-chi2init << "\t" << chi2-chi2init << "\t" << chi2init << std::endl;
      changed=true;
    } else {
      if(chi2<chi2init) std::cout << "[INFO] New minimum found but not updating" << std::endl;
      
      std::cout << "[INFO] Not-updating variables:" << std::endl
		<< "       " << var1->GetName() << "\t" << var1->getVal() << std::endl
		<< "       " << var2->GetName() << "\t" << var2->getVal() << std::endl
		<< "       " << min-chi2init << "\t" << chi2-chi2init << std::endl;
      var1->setVal(v1);
      var2->setVal(v2);
      std::cout
	<< "       " << var1->GetName() << "\t" << var1->getVal() << std::endl
	<< "       " << var2->GetName() << "\t" << var2->getVal() << std::endl
	<< "       " << min-chi2init << std::endl;
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
  for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
    var->setConstant(kFALSE);
  }
  delete it_;

  delete mcChain;
  delete tree;
  delete entrylist;
  
  return changed;
}


void Smooth(TH2F *h, Int_t ntimes, Option_t *option)
{
   Double_t k5a[5][5] =  { { 0, 0, 1, 0, 0 },
                           { 0, 2, 2, 2, 0 },
                           { 1, 2, 5, 2, 1 },
                           { 0, 2, 2, 2, 0 },
                           { 0, 0, 1, 0, 0 } };
   Double_t k5b[5][5] =  { { 0, 1, 2, 1, 0 },
                           { 1, 2, 4, 2, 1 },
                           { 2, 4, 8, 4, 2 },
                           { 1, 2, 4, 2, 1 },
                           { 0, 1, 2, 1, 0 } };
   Double_t k3a[3][3] =  { { 0, 1, 0 },
                           { 1, 2, 1 },
                           { 0, 1, 0 } };

   //  if (ntimes > 1) {
   // Warning("Smooth","Currently only ntimes=1 is supported");
   //   }
   TString opt = option;
   opt.ToLower();
   Int_t ksize_x=5;
   Int_t ksize_y=5;
   Double_t *kernel = &k5a[0][0];
   if (opt.Contains("k5b")) kernel = &k5b[0][0];
   if (opt.Contains("k3a")) {
      kernel = &k3a[0][0];
      ksize_x=3;
      ksize_y=3;
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
   Int_t bufSize  = (nx+2)*(ny+2);
   Double_t *buf  = new Double_t[bufSize];
   Double_t *ebuf = 0;
   if (h->GetSumw2()->fN) ebuf = new Double_t[bufSize];

   // Copy all the data to the temporary buffers
   Int_t i,j,bin;
   for (i=ifirst; i<=ilast; i++){
      for (j=jfirst; j<=jlast; j++){
         bin = h->GetBin(i,j);
         buf[bin] =h->GetBinContent(bin);
         if (ebuf) ebuf[bin]=h->GetBinError(bin);
      }
   }

   // Kernel tail sizes (kernel sizes must be odd for this to work!)
   Int_t x_push = (ksize_x-1)/2;
   Int_t y_push = (ksize_y-1)/2;

   // main work loop
   for (i=ifirst; i<=ilast; i++){
      for (j=jfirst; j<=jlast; j++) {
         Double_t content = 0.0;
         Double_t error = 0.0;
         Double_t norm = 0.0;
	 if(h->GetBinContent(i,j)<=0) continue;
         for (Int_t n=0; n<ksize_x; n++) {
            for (Int_t m=0; m<ksize_y; m++) {
               Int_t xb = i+(n-x_push);
               Int_t yb = j+(m-y_push);
               if ( (xb >= 1) && (xb <= nx) && (yb >= 1) && (yb <= ny)) {
                  bin = h->GetBin(xb,yb);
		  if(buf[bin]>0){
                  Double_t k = kernel[n*ksize_y +m];
                  //if ( (k != 0.0 ) && (buf[bin] != 0.0) ) { // General version probably does not want the second condition
                  if ( k != 0.0 ) {
                     norm    += k;
                     content += k*buf[bin];
                     if (ebuf) error   += k*k*buf[bin]*buf[bin];
                  }
		  }
               }
            }
         }

         if ( norm != 0.0 ) {
            h->SetBinContent(i,j,content/norm);
            if (ebuf) {
               error /= (norm*norm);
               h->SetBinError(i,j,sqrt(error));
            }
         }
      }
   }
   //fEntries = nentries;

   delete [] buf;
   delete [] ebuf;
}

bool stopFindMin1D(Int_t i, Int_t iLocMin, Double_t chi2, Double_t min, Double_t locmin){
  if(abs(i-iLocMin)>2 && chi2-min > 300){ std::cout << "stop 1" << std::endl; return true;}
  if(abs(i-iLocMin)>5 && chi2-min > 200){ std::cout << "stop 2" << std::endl; return true;}
//   if(abs(i-iLocMin)>6 && chi2-locmin > 120){ std::cout << "stop 3" << std::endl; return true;}
//   if(abs(i-iLocMin)>7 && chi2-locmin > 80){ std::cout << "stop 4" << std::endl; return true;}
//   if(abs(i-iLocMin)>8 && chi2-locmin > 50){ std::cout << "stop 8" << std::endl; return true;}
//   if(abs(i-iLocMin)>9 && chi2-locmin > 20){ std::cout << "stop 9" << std::endl; return true;}
//   if(abs(i-iLocMin)>10 && chi2-locmin > 10){ std::cout << "stop 10" << std::endl; return true;}
//   if(abs(i-iLocMin)>11 && chi2-locmin > 5){ std::cout << "stop 11" << std::endl; return true;}
//   if(abs(i-iLocMin)>12 && chi2-locmin > 3){ std::cout << "stop 12" << std::endl; return true;}

  if(abs(i-iLocMin)>3 && chi2-locmin > 20){ std::cout << "stop 3" << std::endl; return true;}
  if(abs(i-iLocMin)>4 && chi2-locmin > 10){ std::cout << "stop 4" << std::endl; return true;}
  if(abs(i-iLocMin)>5 && chi2-locmin > 5){ std::cout << "stop 8" << std::endl; return true;}
  if(abs(i-iLocMin)>6 && chi2-locmin > 3){ std::cout << "stop 9" << std::endl; return true;}

  return false;
}

Int_t FindMin1D(RooRealVar *var, Double_t *X, Int_t N, Int_t iMinStart, Double_t min, RooSmearer& smearer, bool update=true, Double_t *Y=NULL){
  Double_t vInit=var->getVal();
  var->setVal(X[iMinStart]);
  TStopwatch timer;
  timer.Start();
  Double_t chi2, chi2init=smearer.evaluate(); 
  timer.Stop();
  timer.Print();

  Double_t locmin=1e20;
  Int_t iLocMin=iMinStart;
  std::queue<Double_t> chi2old;
  Double_t NY[PROFILE_NBINS]={0.}; 

  for(Int_t i =iMinStart; i <N; i++){ //loop versus positive side
    var->setVal(X[i]);
    chi2=smearer.evaluate(); 
    if(Y!=NULL){ 
      Y[i] += chi2;
      NY[i]++;
    }
#ifdef DEBUG
    //    if(update==true) 

#endif
      std::cout << "[DEBUG] " <<  "\t" << var->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << "\t" << min-chi2init << std::endl;
    if(chi2<=locmin){ //local minimum
	iLocMin=i;
	locmin=chi2;
    }
      
    if(!chi2old.empty() && chi2<chi2old.back()-15){ //remove history if chi2<chi2old
      while(!chi2old.empty()){
	chi2old.pop();
      }
    } 

    if(stopFindMin1D(i,iLocMin, chi2, min, locmin)) break;
    if(i==iLocMin && i-iMinStart==3 && Y[iMinStart]==Y[iMinStart+1] && Y[iMinStart]==Y[iMinStart+2] && Y[iMinStart]==Y[iMinStart+3]){
      std::cerr << "[WARNING] No sensitivity to variable: " << var->GetName() << std::endl;
      std::cerr << "          Variable changed to constant" << std::endl;
      std::cout << "[WARNING] No sensitivity to variable: " << var->GetName() << std::endl;
      std::cout << "          Variable changed to constant" << std::endl;
      return -1;
    }
    chi2old.push(chi2);
  }

  while(!chi2old.empty()){
    chi2old.pop();
  }
  for(Int_t i =iMinStart; i >=0; i--){ //loop versus positive side
    var->setVal(X[i]);
    chi2=smearer.evaluate(); 
    if(Y!=NULL){ 
      Y[i] += chi2;
      NY[i]++;
    }

#ifdef DEBUG      
    //if(update==true) 
      std::cout << "[DEBUG] " <<  "\t" << var->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << std::endl;
#endif
      std::cout << "[DEBUG] " <<  "\t" << var->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << "\t" << min-chi2init << std::endl;

    if(chi2<=locmin){ //local minimum
	iLocMin=i;
	locmin=chi2;
    } 

    if(!chi2old.empty() && chi2<chi2old.back()-15){ //remove history if chi2<chi2old
      while(!chi2old.empty()){
	chi2old.pop();
      }
    } 

    if(stopFindMin1D(i,iLocMin, chi2, min, locmin)) break;
    chi2old.push(chi2);
  }

  if(Y!=NULL){  //take the mean!
    for(Int_t i =0; i < N; i++){ 
      if(NY[i]!=0) Y[i] /= NY[i];
    }
  }

  return iLocMin;
}


/**
   min is updated
 */
bool MinProfile2D(RooRealVar *var1, RooRealVar *var2, RooSmearer& smearer, int iProfile, 
		  Double_t min_old, Double_t& min, bool update=true){ 
  if(iProfile==2) iProfile=-1;
  if(iProfile>2)  return false;

  bool changed=false;
  std::cout << "------------------------------ MinProfile2D: iProfile==" << iProfile << std::endl;
  std::cout << "--------------------------------- GetProfile for " << var1->GetName() << std::endl;
  TGraph 	*g1 = GetProfile(var1, smearer, iProfile, false, false); // take only the binning
  std::cout << "--------------------------------- GetProfile for " << var2->GetName() << std::endl;
  TGraph 	*g2 = GetProfile(var2, smearer, iProfile, false, false); // take only the binning

  
  Double_t 	*X1    = g1->GetX();
  Double_t 	*X2    = g2->GetX();
  Int_t 	 N1    = g1->GetN();
  Int_t 	 N2    = g2->GetN();
  Double_t      *Y     = new Double_t[N2];

  std::cout << "--------------------------------- Init eval "  << std::endl;
  //initial values
  Double_t v1=var1->getVal();
  Double_t v2=var2->getVal();
  if(var1->isConstant()){
    X1[0] = v1;
    N1=1;
  }
  if(var2->isConstant()){
    X2[0] = v2;
    N2=1;
  }

      
  Double_t chi2, chi2init=smearer.evaluate(); 

  Double_t locmin=1e20, locminSmooth=1e20;
  Int_t iLocMin1=0, iLocMin2, iLocMin2Prev=N2/3;

  TStopwatch myClock;
  myClock.Start();

  std::cout << "--------------------------------- Grid eval "  << std::endl;
  TH2F h("hist2","",N1,0,N1,N2,0,N2);
  for(Int_t i1 =0; i1 <N1; i1++){
    if(iLocMin2Prev<3 && X1[i1] > 0.02 && chi2>locmin+200){
      N1=i1;
      break;
    }
    if(chi2>locmin+10){
      std::cout << "[STATUS] Stopping 2D scan in constantTerm direction" << std::endl;
      N1=i1;
      break;
    }

    for(Int_t i2=0; i2 < N2; i2++){ // reset Y values
      Y[i2]=0;
    }

    var1->setVal(X1[i1]);
    std::queue<Double_t> chi2old;

    iLocMin2Prev = FindMin1D(var2, X2, N2, iLocMin2Prev, locmin, smearer, true, Y);
    for(Int_t i2=0; i2 < N2; i2++){
#ifdef DEBUG
      if(Y[i2]!=0) std::cout << i1 << "\t" << i2 << "\t" << X1[i1] << "\t" << X2[i2] << "\t" << Y[i2];
#endif
      h.Fill(i1,i2,Y[i2]);
#ifdef DEBUG
      if(Y[i2]!=0) std::cout << "\t" << h.GetBinContent(i1+1,i2+1) << std::endl;
#endif
    }

    var2->setVal(X2[iLocMin2Prev]);
    chi2=smearer.evaluate(); 

#ifndef smooth
    if(update==true) std::cout << "[DEBUG] " << var1->getVal() << "\t" << var2->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << "\t" << min-chi2init << "\t" << smearer.nllMin-chi2init << std::endl;
#endif
    if(chi2<=locmin){ //local minimum
      iLocMin1=i1;
      iLocMin2=iLocMin2Prev;
      locmin=chi2;
    }
  }

  // reset to initial value
  var1->setVal(v1);
  var2->setVal(v2);

#ifdef smooth
  for(int iSmooth=0; iSmooth<10; iSmooth++){
    Smooth(&h, 1, "k3a");
  }
  //Smooth(&h, 1, "k3a");
  for(Int_t i1=0; i1<N1; i1++){
    for(Int_t i2=0; i2<N2; i2++){
      Double_t content = h.GetBinContent(i1+1,i2+1);
      if(content>0){
#ifdef DEBUG
	if(update==true) std::cout << "[DEBUG] " << X1[i1] << "\t" << X2[i2] << "\t" << content-chi2init << "\t" << locminSmooth-chi2init << "\t" << min-chi2init << "\t" << smearer.nllMin-chi2init << std::endl;
#endif
	if(content>0 && locminSmooth > content){
	  locminSmooth=content;
	  iLocMin1=i1;
	  iLocMin2=i2;
	}
      }
    }
  }
  locmin=locminSmooth;
#endif
  if(update && locmin<min){ // updated absolute minimum
    min=locmin;
    if(v1!=X1[iLocMin1] || v2!=X2[iLocMin2]) changed=true; //the value has been updated, need a new iteration
  }

  // if a new minimum has been found now, or due to another variable, need to update the scale
  if(update && (min<min_old || changed)) { 
    std::cout << "[INFO] Updating variables:" << std::endl
	      << "       " << var1->GetName() << "\t" << v1 << " -> " << X1[iLocMin1] << std::endl
	      << "       " << var2->GetName() << "\t" << v2 << " -> " << X2[iLocMin2] << std::endl
	      << "       " << min << "\t" << locmin-chi2init << "\t" << min-chi2init << std::endl;
    var1->setVal(X1[iLocMin1]);
    var2->setVal(X2[iLocMin2]);
  } else{
    std::cout << "[INFO] Not-Updating variable:" << std::endl
	      << "       " << var1->GetName() << "\t" << v1 << " -> " << X1[iLocMin1] << std::endl
	      << "       " << var2->GetName() << "\t" << v2 << " -> " << X2[iLocMin2] << std::endl
	      << "       " << min << "\t" << locmin-chi2init << "\t" << min-chi2init << std::endl;
  }   

  std::cout << "------------------------------" << std::endl;
  myClock.Stop();
  std::cout << var1->GetName() << "\tlevel=" << iProfile << "; nBins=" << N1 << "; range_min=" << X1[0] << "; range_max=" << X1[N1-1] << "; bin_width=" << X1[1]-X1[0] << std::endl;
  std::cout << var2->GetName() << "\tlevel=" << iProfile << "; nBins=" << N2 << "; range_min=" << X2[0] << "; range_max=" << X2[N2-1] << "; bin_width=" << X2[1]-X2[0] << std::endl;
  myClock.Print();
  std::cout << "------------------------------" << std::endl;

  //  std::cout << "[INFO] Level " << iProfile << " variable:" << var->GetName() << "\t" << var->getVal() << "\t" << min << std::endl;

  delete g1;
  delete g2;
  delete Y;
  return changed;
}


  

bool MinProfile(RooRealVar *var, RooSmearer& smearer, int iProfile, 
		Double_t min_old, Double_t& min){ 
  bool changed=false;
  bool trueEval=true;
  Double_t v1=var->getVal();
  
#ifndef old
  if(iProfile>2) return false;
  // don't put SetNSmear here otherwise it will recalculate every category!
  if(iProfile>1) iProfile=-1;
  else if(iProfile==1) iProfile=2;

  trueEval=false;
#endif

  std::cout << "[STATUS] Starting MinProfile for " << var->GetName() << "\t" << iProfile << std::endl;
  TGraph 	*profil = GetProfile(var, smearer, iProfile, false,trueEval);
  TGraphErrors 	 g(profil->GetN(),profil->GetX(), profil->GetY());


  Double_t 	*X    = g.GetX();
  Double_t 	*Y    = g.GetY();
  Int_t 	 N    = g.GetN();
#ifndef old
  Double_t      *Y2     = new Double_t[PROFILE_NBINS];
#endif
  Double_t chi2=Y[0];
  Double_t chi2init=smearer.evaluate();
  //Int_t 	 iMin =	1; //, iLow=1, iHigh=1;
  Int_t iMin_=1; // minimum of the graph
  //find minimum
#ifndef old
  for(Int_t i2=0; i2 < N; i2++){ // reset Y values
    Y2[i2]=0;
  }
#endif
#ifdef old
  for(Int_t i =0; i <N; i++){
    if(Y[i] <= chi2){ iMin_=i; chi2=Y[iMin_];}
    std::cout << "[DEBUG] " << var->GetName() << ": " << i << " " << X[i] << " " << Y[i]-chi2init << " " << chi2-chi2init << " " << min-chi2 << std::endl;
  }
#else
  iMin_= FindMin1D(var, X, N, N/2, min, smearer, true, Y2);
  if(iMin_<0){ // in case of no sensitivity to the variable it is put has constant and the minimization is interrupted
    var->setVal(v1);
    var->setConstant();
    return false;
  }
  chi2=Y2[iMin_];
  var->setVal(v1);
#endif
 
  if(chi2<min){ // updated absolute minimum
    min=chi2; 
    if(var->getVal() != X[iMin_]) changed=true; //the value has been updated, need a new iteration
   }

  if(chi2< chi2init && (iMin_<=1 || iMin_>=N-1)){ // on the border, need a new iteration
    var->setVal(X[iMin_]);
    std::cout << "[INFO] Minimized at the border of the interval: need another iteration" << std::endl;
    std::cout << "       iMin_ = " << iMin_ << "\t" << X[iMin_] << std::endl;
    for(Int_t i2=0; i2 < N; i2++){ // reset Y values
      std::cout << i2 << "\t" << Y2[i2]-Y2[0] << std::endl;
    }
    
#ifndef old
    exit(1);
#endif
    MinProfile(var, smearer, iProfile, min_old, min);
    changed=true;
    return changed;
  }

  if(min<min_old || changed) { // if a new minimum has been found now, or due to another variable, need to update the scale
    std::cout << "[INFO] Updating variable:" << var->GetName() << "\t" << X[iMin_] 
	      << " " << min << " " << Y[iMin_]-min << std::endl;
    var->setVal(X[iMin_]);
    //var->setError(newError);
  } else{
    std::cout << "[INFO] Not-Updating variable:" << var->GetName() << "\t" << X[iMin_] 
	      << " " << min << " " << Y[iMin_]-min << std::endl;
  }   
  std::cout << "[INFO] Level " << iProfile << " variable:" << var->GetName() << "\t" << var->getVal() << "\t" << min << std::endl;

  delete profil;
  return changed;
}

void MinimizationProfile(RooSmearer& smearer, RooArgSet args, long unsigned int nIterMCMC, bool mcmc=false){
  std::cout << "[INFO] Minimization: profile" << std::endl;
  std::cout << "[INFO] Re-initialize nllMin: 1e20"  << std::endl;
  smearer.nllMin=1e20;
  std::cout << "[INFO] Setting initial evaluation" << std::endl;

  smearer.evaluate();
  Double_t 	min_old	    = 9999999999.;
  Double_t 	min	    = 999999999.;
  bool 	updateError = false;
  int nIter=0;
  TStopwatch myClock;
  myClock.Start();
  
  std::cout << "[INFO] Starting minimization" << std::endl;
  RooArgList 	 argList_(args);
  TIterator 	*it_   = NULL;
  while((min < min_old || updateError==true) && nIter <1){
    updateError=false;
    TStopwatch iterClock;
    iterClock.Start();
    if(min < min_old) min_old=min;
    it_ = argList_.createIterator();
    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
      if (var->isConstant() || !var->isLValue()) continue;
      TString  name(var->GetName());
      if(!name.Contains("scale")) continue; // looping only for the scale
      for(int iProfile=1; iProfile<2; iProfile++){
	updateError += MinProfile(var, smearer, iProfile, min_old, min);
      }
    }
    delete it_;
    iterClock.Stop();
    std::cout << "[INFO] nIter scale: " << nIter;
    iterClock.Print();
    nIter++;
  }

  
  //minimization of additional smearing
  updateError=true;
  min_old=min;
  nIter=0;
  while((min < min_old || updateError==true) && nIter <2){
    updateError=false;
    TStopwatch iterClock;
    iterClock.Start();
    if(min < min_old) min_old=min;

    it_= argList_.createIterator();
    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
      if (var->isConstant() || !var->isLValue()) continue;
      TString  name(var->GetName());
      if(!name.Contains("scale")) continue; // looping only for the scale
      for(int iProfile=2; iProfile<4; iProfile++){
	updateError += MinProfile(var, smearer, iProfile, min_old, min);
      }
    }
    delete it_;
    iterClock.Stop();
    std::cout << "[INFO] nIter fine scale: " << nIter;
    iterClock.Print();
    nIter++;
  }

  updateError=true;
  min_old = min;
  nIter=0;
  while((min < min_old && nIter < 5) || (updateError==true && nIter <2)){
    updateError=false;
    TStopwatch iterClock;
    iterClock.Start();
    it_= argList_.createIterator();
    if(min < min_old) min_old=min;
    for(RooRealVar *var = (RooRealVar*)it_->Next(); var!=NULL; var = (RooRealVar*)it_->Next()){
      if (var->isConstant()) continue;
      TString  name(var->GetName());
      
      // special part for alpha fitting 
      TString  alphaName=name; alphaName.ReplaceAll("constTerm","alpha");
      RooRealVar *var2= name.Contains("constTerm") ? (RooRealVar *)argList_.find(alphaName): NULL;
      if(name.Contains("alpha")) continue;
      //if(var2!=NULL && var2->isConstant()) var2=NULL; // to use MinProfile 1D instead of 2D
      //taking large profile
      for(int iProfile=2; iProfile<4; iProfile++){
	if(var2==NULL) MinProfile(var, smearer, iProfile, min_old, min); //updateError += MinProfile(var, smearer, iProfile, min_old, min);
	else { 
	  if(mcmc && iProfile>2) updateError += MinMCMC2D(var, var2, smearer, iProfile, min_old, min, nIterMCMC);
	  else updateError+=MinProfile2D(var, var2, smearer, iProfile, min_old, min);
	}
      }
    }
    delete it_;
    iterClock.Stop();
    std::cout << "[DEBUG] nIter= " << nIter << "\t"; iterClock.Print();
    nIter++;
    smearer.GetParams().writeToStream(std::cout, kFALSE);
  }
}


 

class ShervinMinuit: public PdfProposal {
  
public:
  //inline ShervinMinuit(){};
  //ShervinMinuit(int nBurnSteps);
  ShervinMinuit(int nBurnSteps, RooSmearer& smearer);

  ProposalHelper *ph;
  ProposalFunction* proposal;
  void Propose(RooArgSet& xPrime, RooArgSet& x);
  inline Bool_t IsSymmetric(RooArgSet& xPrime, RooArgSet& x){return true;}; //proposal->IsSymmetric(xPrime,x);};
  inline Double_t GetProposalDensity(RooArgSet& xPrime, RooArgSet& x){return 0;}; //proposal->GetProposalDensity(xPrime, x);};

  inline void SetMinuit(RooMinuit& m){_m = &m;};
private:
  int _nBurnSteps;
  int _iStep;
  int _iBurnStep;
  RooMinuit* _m;
  
  
  RooSmearer& _smearer;
  //RooArgSet& _params;
  RooArgSet _paramSet;
  RooStats::UniformProposal propUnif;
  RooMultiVarGaussian *gausPdf;
  TRandom3 gen;
  std::vector<TH1F*> rmsHist;
  float fSigmaRangeDivisor;
};


ShervinMinuit::ShervinMinuit(int nBurnSteps, RooSmearer& smearer): 
  _smearer(smearer),gausPdf(NULL), gen(1245), fSigmaRangeDivisor(-1)
  //  _params(smearer.GetParams())
{

  _iStep =0;
		     
  _nBurnSteps = nBurnSteps;
  
  RooArgList argList(smearer.GetParams());
  TIterator *it = argList.createIterator();
  std::cout << "###############################" << std::endl;
  for(RooRealVar *v = (RooRealVar*)it->Next(); v!=NULL; v = (RooRealVar*)it->Next()){
    if(! v->isConstant()){
      _paramSet.add((*v));
      rmsHist.push_back(new TH1F(v->GetName(),"",1000, v->getMin(), v->getMax()));
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
    (*fCovMatrix)(i,i) = range / fSigmaRangeDivisor;
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


void ShervinMinuit::Propose(RooArgSet& xPrime, RooArgSet& x){
  TIterator *it=NULL; 
//   RooRealVar v1("v1","",0.01,0,0.04);
//   RooRealVar v2("v2","",0.02,0,0.04);
//   RooArgSet arg1(v1);
//   RooArgSet arg2(v2);

  if(_iStep==20){
    fSigmaRangeDivisor=4;
    RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
    float varErr = (var->getMax()-var->getMin())/fSigmaRangeDivisor;
    std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor 
	      << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
	      << std::endl;
  }    
  else if(_iStep==50){
    fSigmaRangeDivisor=10;
    std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor << std::endl;
    RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
    float varErr = (var->getMax()-var->getMin())/fSigmaRangeDivisor;
    std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor 
	      << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
	      << std::endl;

  }
  else if(_iStep==80){
    fSigmaRangeDivisor=15;
    std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor << std::endl;
    RooRealVar *var = (RooRealVar *)x.find("constTerm_EB-absEta_0_1-gold-Et_25");
    float varErr = (var->getMax()-var->getMin())/fSigmaRangeDivisor;
    std::cout << "[INFO] Changing sigma divisor to: " << fSigmaRangeDivisor 
	      << "\t" << var->getVal() << ": " << var->getMin() << " - " << var->getMax() << "\t" << varErr
	      << std::endl;
    var = (RooRealVar *)x.find("scale_EB-absEta_0_1-gold-Et_25");
    varErr = (var->getMax()-var->getMin())/fSigmaRangeDivisor;
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
  int index=0;
  for(RooRealVar *var = (RooRealVar *) it->Next(); var!=NULL;
      var = (RooRealVar *) it->Next()){
    
    //var->setError(var->getError()/100);

    
    RooRealVar *v_x = ((RooRealVar *)x.find(var->GetName()));
    rmsHist[index]->Fill(v_x->getVal());

    float varErr = std::min(rmsHist[index]->GetRMS(), (var->getMax()-var->getMin())/fSigmaRangeDivisor);


    if(fSigmaRangeDivisor<3)     var->setVal(gen.Uniform(v_x->getMin(), v_x->getMax()));
    else {
      if(varErr < (var->getMax()-var->getMin())/100) std::cout << "[WARNING] " << varErr << std::endl;
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


  
  if(_iStep == _nBurnSteps){
    std::cout << "Now Gaussian proposal" << std::endl;

    it = xPrime.createIterator();
    for(RooRealVar *var = (RooRealVar *) it->Next(); var!=NULL;
	var = (RooRealVar *) it->Next()){
      //var->setError(var->getError()/100);
      //float varErr = var->getVal()/100;
      //if(TString(var->GetName()).Contains("constTerm")) varErr*=10;
      //var->setMin(std::max(var->getMin(),var->getVal()- varErr*3));
      //var->setMax(std::min(var->getMax(),var->getVal()+ varErr*3));
      var->setMin(0.0);
      var->setMax(0.05);
      //var->setVal(0.03);
      std::cout << var -> GetName() << "\t" << var->getError() << "\t" << var -> getMin() << "\t" << var->getMax()<< std::endl;
    }
    //_m->hesse();
    //RooFitResult *fitres = _m->save();
  } 
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


