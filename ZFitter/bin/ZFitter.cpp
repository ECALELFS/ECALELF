#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>

#include <TChain.h>
#include <TStopwatch.h>
#include <TF1.h>

#include "../interface/ZFit_class.hh"
#include "../interface/puWeights_class.hh"
#include "../interface/r9Weights_class.hh"

#include "../interface/runDivide_class.hh"
#include "../interface/EnergyScaleCorrection_class.h"
#include "../interface/addBranch_class.hh"

#include "../interface/RooSmearer.hh"
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

#include "Math/Minimizer.h"
#include "../interface/RooMinimizer.hh"

#include <TPRegexp.h>
#include <RooFormulaVar.h>
#include <RooPullVar.h>

#include <TMatrixDSym.h>

#include <TRandom3.h>
#include <queue>
#define profile
#define PROFILE_NBINS 200

//#define DEBUG
#include "../src/nllProfile.cc"
using namespace std;
using namespace RooStats;

/**
   \todo
   - remove commonCut from category name and add it in the ZFit_class in order to not repeate the cut
   - make alpha fitting more generic (look for alphaName)
*/

typedef std::map< TString, TChain* > chain_map_t;
typedef std::map< TString, chain_map_t > tag_chain_map_t;

std::vector<TString> ReadRegionsFromFile(TString fileName){
  ifstream file(fileName);
  std::vector<TString> regions;
  TString region;

  while(file.peek()!=EOF && file.good()){
    if(file.peek()==10){ // 10 = \n
      file.get(); 
      continue;
    }

    if(file.peek() == 35){ // 35 = #
      file.ignore(1000,10); // ignore the rest of the line until \n
      continue;
    }

    file >> region;
    file.ignore(1000,10); // ignore the rest of the line until \n
#ifdef DEBUG
    std::cout << "[DEBUG] Reading region: " << region<< std::endl;
#endif
    regions.push_back(region);

  }
  return regions;
}


void UpdateFriends(tag_chain_map_t& tagChainMap){

  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
    TChain *chain = (tag_chain_itr->second.find("selected"))->second;
    //    std::cout << chain->GetName() << std::endl;
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){

      if(chain_itr->first!="selected" &&
	 chain->GetFriend(chain_itr->first)==NULL){
	std::cout << "[STATUS] Adding friend branch: " << chain_itr->first
		  << " to tag " << tag_chain_itr->first << std::endl;
	chain->AddFriend(chain_itr->second);
      }
      chain_itr->second->GetEntries();
    }
  }
  return;
}

//Get Profile after smearing
TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level=1, bool warningEnable=true, bool trueEval=true);
TGraph *GetProfile(RooRealVar *var, RooSmearer& compatibility, int level, bool warningEnable, bool trueEval){
  TString name(var->GetName());
  double minValue=var->getVal();
  double minYvalue=compatibility.evaluate();
  std::cout << "[STATUS] Getting profile for " << name << "\t" << level << "\t" << trueEval << std::endl;

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
      bin_width=0.0005; // level1/10
    }else  if(name.Contains("const")){
      bin_width=0.0005; //level1/10
    } else if(name.Contains("alpha")){
      bin_width=0.001;
    } 
    range_min = var->getMin();
    range_max = var->getMax();
    break;
  }

    nBin=(int)((range_max-range_min)/bin_width);
    while(nBin <10){
      //std::cout << "[INFO] Updating from range: " << nBin << "\t" << range_min << "\t" << range_max << std::endl;
      bin_width*=0.9;
      range_min = std::max(var->getMin(),bin_width*((floor)(range_min / bin_width))- bin_width);
      range_max = std::min(var->getMax(),bin_width*((floor)(range_max / bin_width))+ bin_width);
      nBin=(int)((range_max-range_min)/bin_width);
      //std::cout << "[INFO] Updating to range: " << nBin << "\t" << range_min << "\t" << range_max << std::endl;
      if(nBin <=0) exit(1);
    }
    while(nBin >40 && level!=4){
      bin_width*=1.1;
      range_min = bin_width*((floor)(range_min / bin_width));
      range_max = bin_width*((floor)(range_max / bin_width));
      nBin=(int)((range_max-range_min)/bin_width);
    }

   double chi2[PROFILE_NBINS];
   double xValues[PROFILE_NBINS];
   if(trueEval) std::cout << "------------------------------" << std::endl;
   TStopwatch myClock;
   myClock.Start();
  for (int iVal=0;iVal<nBin;++iVal)
    {
      double value = range_min+bin_width*iVal;
#ifdef DEBUG
      std::cout << "testing function @ " << value << std::endl;
#endif
      var->setVal(value);
      xValues[iVal]=value;
      if(trueEval) chi2[iVal]=compatibility.evaluate(); //-minYvalue;
      else chi2[iVal]=0;
      if (warningEnable && trueEval && chi2[iVal] < minYvalue ){
	std::cout << "[WARNING]: min in likelihood profile < min in MIGRAD: " << chi2[iVal]-minYvalue << " < " << minYvalue << std::endl;
	std::cout << "xValue = " << value << "\t" << "[" << range_min << ":" << range_max << "]" << std::endl;
      }
#ifdef DEBUG
      std::cout << "value is " << chi2[iVal] << std::endl;
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


Int_t FindMin1D(RooRealVar *var, Double_t *X, Int_t N, Int_t iMinStart, RooSmearer& smearer, bool update=true){
  var->setVal(X[iMinStart]);
  Double_t chi2, chi2init=smearer.evaluate(); 

  Double_t locmin=1e20;
  Int_t iLocMin=0;
  std::queue<Double_t> chi2old;

  for(Int_t i =iMinStart; i <N; i++){ //loop versus positive side
    var->setVal(X[i]);
    chi2=smearer.evaluate(); 

    if(update==true) 
      std::cout << "[DEBUG] " <<  "\t" << var->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << std::endl;
    if(chi2<=locmin){ //local minimum
	iLocMin=i;
	locmin=chi2;
    }
      
    if(!chi2old.empty() && chi2<chi2old.back()){ //remove history if chi2<chi2old
      while(!chi2old.empty()){
	chi2old.pop();
      }
    } 

    if(chi2old.size()>2 && chi2-chi2old.back() >80 && chi2-locmin >100) break; // jump to the next constTerm
    if(chi2old.size()>3 && chi2-chi2old.back() >40 && chi2-locmin >200) break; // jump to the next constTerm
    if(chi2old.size()>3 && chi2old.front()-chi2old.back()>100 && chi2-locmin >300) break; 

    chi2old.push(chi2);
  }

  while(!chi2old.empty()){
    chi2old.pop();
  }
  for(Int_t i =iMinStart; i >=0; i--){ //loop versus positive side
    var->setVal(X[i]);
    chi2=smearer.evaluate(); 
      
    if(update==true) 
      std::cout << "[DEBUG] " <<  "\t" << var->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << std::endl;
    if(chi2<=locmin){ //local minimum
	iLocMin=i;
	locmin=chi2;
    } 

    if(!chi2old.empty() && chi2<chi2old.back()){ //remove history if chi2<chi2old
      while(!chi2old.empty()){
	chi2old.pop();
      }
    } 
  
    if(chi2old.size()>2 && chi2-chi2old.back() >80 && chi2-locmin >100) break; // jump to the next constTerm
    if(chi2old.size()>3 && chi2-chi2old.back() >40 && chi2-locmin >200) break; // jump to the next constTerm
    if(chi2old.size()>3 && chi2old.front()-chi2old.back()>100 && chi2-locmin >300) break; 
    
    chi2old.push(chi2);
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
  //smearer.SetNSmear(0,10);
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

  std::cout << "--------------------------------- Init eval "  << std::endl;
  //initial values
  Double_t v1=var1->getVal();
  Double_t v2=var2->getVal();
  Double_t chi2, chi2init=smearer.evaluate(); 

  Double_t locmin=1e20;
  Int_t iLocMin1=0, iLocMin2, iLocMin2Prev=0;

  TStopwatch myClock;
  myClock.Start();

  std::cout << "--------------------------------- Grid eval "  << std::endl;
  for(Int_t i1 =0; i1 <N1; i1++){
    var1->setVal(X1[i1]);
    std::queue<Double_t> chi2old;

    iLocMin2Prev = FindMin1D(var2, X2, N2, iLocMin2Prev, smearer, true);

    var2->setVal(X2[iLocMin2Prev]);
    chi2=smearer.evaluate(); 
      
    if(update==true || true) std::cout << "[DEBUG] " << var1->getVal() << "\t" << var2->getVal() << "\t" << chi2-chi2init << "\t" << locmin-chi2init << "\t" << min-chi2init << "\t" << smearer.nllMin-chi2init << std::endl;
    if(chi2<=locmin){ //local minimum
      iLocMin1=i1;
      iLocMin2=iLocMin2Prev;
      locmin=chi2;
    }
    
  }

  // reset to initial value
  var1->setVal(v1);
  var2->setVal(v2);

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
  smearer.SetNSmear(0,1);
  delete g1;
  delete g2;
  return changed;
}


  

bool MinProfile(RooRealVar *var, RooSmearer& smearer, int iProfile, 
		Double_t min_old, Double_t& min){ 
  bool changed=false;
  std::cout << "[STATUS] Starting MinProfile for " << var->GetName() << "\t" << iProfile << std::endl;
  TGraph 	*profil = GetProfile(var, smearer, iProfile, false);
  TGraphErrors 	 g(profil->GetN(),profil->GetX(), profil->GetY());


  Double_t 	*X    = g.GetX();
  Double_t 	*Y    = g.GetY();
  Int_t 	 N    = g.GetN();
  Double_t chi2=Y[0];
  Double_t chi2init=smearer.evaluate();
  //Int_t 	 iMin =	1; //, iLow=1, iHigh=1;
  Int_t iMin_=1; // minimum of the graph
  //find minimum
  for(Int_t i =0; i <N; i++){
    if(Y[i] <= chi2){ iMin_=i; chi2=Y[iMin_];}
    std::cout << "[DEBUG] " << var->GetName() << ": " << i << " " << X[i] << " " << Y[i]-chi2init << " " << chi2-chi2init << " " << min-chi2 << std::endl;
  }
  
  if(chi2<min){ // updated absolute minimum
    min=chi2; 
    if(var->getVal() != X[iMin_]) changed=true; //the value has been updated, need a new iteration
   }

  if(chi2< chi2init && (iMin_<=1 || iMin_>=N-1)){ // on the border, need a new iteration
    var->setVal(X[iMin_]);
    std::cout << "[INFO] Minimized at the border of the interval: need another iteration" << std::endl;
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
  Double_t 	min_old	    = 9999999999.;
  Double_t 	min	    = 999999999.;
  bool 	updateError = false;
  int nIter=0;
  TStopwatch myClock;
  myClock.Start();
  
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
      if(var2!=NULL && var2->isConstant()) var2=NULL;
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

int main(int argc, char **argv) {
  TStopwatch myClock;
  TStopwatch globalClock;
  globalClock.Start();

  puWeights_class puWeights;
  std::cout << "============================== Z General Fitter" << std::endl;

  using namespace boost;
  namespace po = boost::program_options;
  unsigned int nEvents_runDivide=100000;
  std::string chainFileListName;
  std::string regionsFileName;
  std::string runRangesFileName;
  std::string dataPUFileName, mcPUFileName;
  std::vector<TString> dataPUFileNameVec, mcPUFileNameVec;
  std::string r9WeightFile;
  std::string initFileName;
  //  bool savePUweightTree;
  std::string imgFormat="eps", outDirFitResMC="test/MC/fitres", outDirFitResData="test/dato/fitres", outDirImgMC="test/MC/img", outDirImgData="test/dato/img", outDirTable="test/dato/table", selection;
  TString eleID="";
  //std::vector<std::string> signalFiles, bkgFiles, dataFiles;
  std::string commonCut;
  std::string corrEleFile, corrEleType;
  std::string smearEleFile, smearEleType;
  std::string invMass_var;
  float invMass_min=0, invMass_max=0, invMass_binWidth=0.250;
  int fit_type_value=1;
  int signal_type_value=0;
  unsigned long long int nToys = 0;
  float constTermToy=0;
  unsigned long long int nEventsPerToy = 0;
  unsigned int nIter =0;
  unsigned int nEventsMinDiag=0;
  unsigned int nEventsMinOffDiag=0;
  std::string minimType;
  std::vector<std::string> branchList;

  //  std::vector<std::string> signalFriends,
  //  bool _ZFit_class, _RooSmearer;

  po::options_description desc("Main options");
  po::options_description outputOption("Output options");
  po::options_description inputOption("Input options");
  po::options_description fitterOption("Z fitter options");
  po::options_description smearerOption("Z smearer options");
  po::options_description toyOption("toyMC options");

  //po::options_description cmd_line_options;
  //cmd_line_options.add(desc).add(fitOption).add(smearOption);

  desc.add_options()
    ("help,h","Help message")

    ("runDivide", "execute the run division")

    //
    ("dataPU", po::value< string >(&dataPUFileName), "")
    ("mcPU", po::value<string>(&mcPUFileName),"")
    ("noPU", "")
    ("savePUTreeWeight","")
    //
    ("corrEleFile", po::value<string>(&corrEleFile),"File with energy scale corrections")
    ("corrEleType", po::value<string>(&corrEleType),"Correction type/step")
    ("saveCorrEleTree", "")

    ("smearEleFile", po::value<string>(&smearEleFile),"File with energy smearings")
    ("smearEleType", po::value<string>(&smearEleType),"Correction type/step")
    //
    ("r9WeightFile", po::value<string>(&r9WeightFile),"File with r9 photon-electron weights")
    ("saveR9TreeWeight", "")
    ("saveRootMacro","")
    //
    ("selection", po::value<string>(&selection)->default_value("loose"),"")
    ("commonCut", po::value<string>(&commonCut)->default_value("Et_25-trigger-noPF"),"")
    ("invMass_var", po::value<string>(&invMass_var)->default_value("invMass_SC_regrCorr_ele"),"")
    ("invMass_min", po::value<float>(&invMass_min)->default_value(65.),"")
    ("invMass_max", po::value<float>(&invMass_max)->default_value(115.),"")
    ("invMass_binWidth", po::value<float>(&invMass_binWidth)->default_value(0.25),"Smearing binning")
    ("isOddMC", "Activate if use only odd events in MC")
    ("isOddData", "Activate if use only odd events in data")    
    //
    ("readDirect","") //read correction directly from config file instead of passing as a command line arg
    ("addPtBranches", "")  //add new pt branches ( 3 by default, fra, ele, pho)
    ("addBranch", po::value< std::vector<string> >(&branchList), "")
    ("saveAddBranchTree","")
    //    ("signal,s", po::value< std::vector <string> >(&signalFiles), "Signal file (can be called multiple times putting the files in a chain")
    //    ("bkg,b", po::value< std::vector <string> >(&bkgFiles), "Bkg file (can be called multiple times putting the files in a chain")
    //    ("data,d", po::value< std::vector <string> >(&dataFiles), "Data file (can be called multiple times putting the files in a chain")
    //("_ZFit_class", "call the Z fitter")
    //;
    //
    //  po::options_description fitOption("Z Fit options");
    //  po::options_description smearOption("Z smearing options");
    //  smearOption.add_options()
    ;
  fitterOption.add_options()
    ("fit_type_value", po::value<int>(&fit_type_value)->default_value(1),"0=floating tails, 1=fixed tails")
    ("signal_type_value", po::value<int>(&signal_type_value)->default_value(0),"0=BW+CB, 1=Cruijff")
    ("forceNewFit", "refit MC also if fit exists")
    ("updateOnly",  "do not fit data if fit exists")
    ;;
  smearerOption.add_options()
    ("smearerFit",  "call the smearing")
    ("smearerType", po::value<string>(&minimType)->default_value("profile"), "minimization algo")
    ("onlyDiagonal","if want to use only diagonal categories")
    ("autoBin", "")
    ("autoNsmear", "")
    ("smearscan", "")
    ("isDataSmeared","")
    ("plotOnly", "active if you don't want to do the smearing")
    ("profileOnly", "")
    ("numIter", po::value<unsigned int>(&nIter)->default_value(300), "number of MCMC steps")
    ("nEventsMinDiag", po::value<unsigned int>(&nEventsMinDiag)->default_value(1000), "min num events in diagonal categories")
    ("nEventsMinOffDiag", po::value<unsigned int>(&nEventsMinOffDiag)->default_value(2000), "min num events in off-diagonal categories")
    ("constTermFix", "constTerm not depending on Et")
    ("smearingEt", "alpha term depend on sqrt(Et) and not on sqrt(E)")
    ;
  inputOption.add_options()
    ("chainFileList,f", po::value< string >(&chainFileListName), "Configuration file with input file list")
    ("regionsFile", po::value< string >(&regionsFileName), "Configuration file with regions")
    ("runRangesFile", po::value< string >(&runRangesFileName), "Configuration file with run ranges")
    ("initFile", po::value< string >(&initFileName), "Configuration file with init values of fit model")
    ;
  outputOption.add_options()
    ("imgFormat", po::value<string>(&imgFormat)->default_value("eps"),"")
    ("outDirFitResMC", po::value<string>(&outDirFitResMC),"")
    ("outDirFitResData", po::value<string>(&outDirFitResData),"")
    ("outDirImgMC", po::value<string>(&outDirImgMC),"")
    ("outDirImgData", po::value<string>(&outDirImgData),"")
    //("outDirTable", po::value<string>(&outDirTable),"")
    ;
  toyOption.add_options()
    ("runToy","")
    ("nToys", po::value<unsigned long long int>(&nToys)->default_value(1000),"")
    ("constTermToy", po::value<float>(&constTermToy)->default_value(0.01),"")
    ("eventsPerToy", po::value<unsigned long long int>(&nEventsPerToy)->default_value(0),"=0: all events")
    ;
    
  desc.add(inputOption);
  desc.add(outputOption);
  desc.add(fitterOption);
  desc.add(smearerOption);
  desc.add(toyOption);

  po::variables_map vm;
  //
  // po::store(po::parse_command_line(argc, argv, smearOption), vm);
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    



  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  TString energyBranchName="";
  if(invMass_var=="invMass_SC_regrCorr_ele") energyBranchName = "energySCEle_regrCorr_ele";
  else if(invMass_var=="invMass_SC_regrCorr_pho") energyBranchName = "energySCEle_regrCorr_pho";
  else if(invMass_var=="invMass_regrCorr_fra") energyBranchName = "energyEle_regrCorr_fra";
  else if(invMass_var=="invMass_regrCorr_egamma") energyBranchName = "energyEle_regrCorr_egamma";
  else if(invMass_var=="invMass_SC") energyBranchName = "energySCEle";
  else if(invMass_var=="invMass_SC_corr") energyBranchName = "energySCEle_corr";
  else {
    std::cerr << "Energy branch name not define for invariant mass branch: " << invMass_var << std::endl;
    exit(1);
  }

  if(!vm.count("chainFileList") && !vm.count("runToy")){
    std::cerr << "[ERROR] Missing mandatory option \"chainFile\"" << std::endl;
    return 1;
  }


  if(!vm.count("regionsFile") && 
     !vm.count("runDivide") && !vm.count("savePUTreeWeight") && 
     !vm.count("saveR9TreeWeight") && !vm.count("saveCorrEleTree") &&
     !vm.count("saveRootMacro")
     ){
    std::cerr << "[ERROR] Missing mandatory option \"regionsFile\"" << std::endl;
    return 1;
  }
  //  if(!vm.count("runRangesFile")){
  //    std::cerr << "[ERROR] Missing mandatory option \"runRangesFile\"" << std::endl;
  //    return 1;
  //  }

  if(vm.count("dataPU") != vm.count("mcPU")){
    std::cerr << "[ERROR] One PU distribution (data or mc) are not provided" << std::endl;
    return 1;
  }


  //if((vm.count("corrEleType") != vm.count("corrEleFile"))&&(vm.count("corrEleType") != vm.count("readDirect")) ){
  //std::cout << "[ERROR] Either provide correction file name, or provide read direct option" << std::endl;
  //return 1;
  //}

  if( vm.count("corrEleType") && vm.count("corrEleFile") && vm.count("readDirect") ){
    std::cout << "[ERROR] Either provide correction file name, or provide read direct option - choose only one" << std::endl;
    return 1;
  }

  //============================== Check output folders
  bool checkDirectories=true;
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResMC)+" ]");
  if(!checkDirectories){
    std::cerr << "[ERROR] Directory " << outDirFitResMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResData)+" ]");
  if(!checkDirectories){
    std::cerr << "[ERROR] Directory " << outDirFitResData << " not found" << std::endl;
  }
  checkDirectories=checkDirectories &&   !system("[ -d "+TString(outDirImgMC)+" ]");
  if(!checkDirectories){
     std::cerr << "[ERROR] Directory " << outDirImgMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirImgData)+" ]");
  if(!checkDirectories){
    std::cerr << "[ERROR] Directory " << outDirImgData << " not found" << std::endl;
  }
  //   checkDirectories=checkDirectories && !system("[ -d "+TString(outDirTable)+" ]");
  //   if(!checkDirectories){
  //     std::cerr << "[ERROR] Directory " << outDirTable << " not found" << std::endl;
  //   }
  if(!checkDirectories 
     && !vm.count("runDivide") 
     && !vm.count("savePUTreeWeight") 
     && !vm.count("saveCorrEleTree") 
     && !vm.count("saveR9TreeWeight") 
     && !vm.count("saveRootMacro") 
     ) return 1;

  if(!dataPUFileName.empty()) dataPUFileNameVec.push_back(dataPUFileName.c_str());
  if(!mcPUFileName.empty()) mcPUFileNameVec.push_back(mcPUFileName.c_str());
  //============================== Reading the config file with the list of chains
  tag_chain_map_t tagChainMap;
  TString tag, chainName, fileName;

  std::ifstream chainFileList(chainFileListName.c_str());
  while(chainFileList >> tag, chainFileList.good()){
    if(tag.Contains('#')){
      chainFileList.ignore(1000,'\n');
      continue;
    }
    chainFileList >> chainName >> fileName;
    if(chainName.Contains("Hist")){
      // use these value only if not provided by the command line
      if(tag.Contains("d") && dataPUFileName.empty()) dataPUFileNameVec.push_back(fileName);
      else if(tag.Contains("s") && mcPUFileName.empty()) mcPUFileNameVec.push_back(fileName);
      else std::cerr << "[ERROR] in configuration file Hist not recognized" << std::endl;
      continue;
    }
    //else if (chainName.Contains("corr")){
    //if (tag.Contains("d") && corrEleFile.empty()) corrEleFile=fileName;
    //continue;
    //}

    if(!tagChainMap.count(tag)){
#ifdef DEBUG
      std::cout << "[DEBUG] Create new tag map for tag: " << tag << std::endl;
#endif
      std::pair<TString, chain_map_t > pair_tmp(tag,chain_map_t()); // make_pair not work with scram b
      tagChainMap.insert(pair_tmp);
    }

    if(!tagChainMap[tag].count(chainName)){
      std::pair<TString, TChain* > pair_tmp(chainName, new TChain(chainName));
      (tagChainMap[tag]).insert(pair_tmp);
      (tagChainMap[tag])[chainName]->SetTitle(tag);
    }
    std::cout <<"Adding file: " << tag << "\t" << chainName << "\t" << fileName<< std::endl;
    if((tagChainMap[tag])[chainName]->Add(fileName,-1)==0) exit(1);
#ifdef DEBUG
    std::cout << "[DEBUG] " << tag << "\t" << chainName << "\t" << fileName << std::endl;
#endif
  }

  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
#ifdef DEBUG
    std::cout << tag_chain_itr->first << std::endl;
#endif
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){
#ifdef DEBUG
      std::cout << " - " << chain_itr->first << "\t" << chain_itr->second->GetName() << "\t" << chain_itr->second->GetTitle() << std::endl;
#endif
      chain_itr->second->GetEntries();
    }
  }



  ///------------------------------ to obtain run ranges
  if(vm.count("runDivide")){
    runDivide_class runDivider;
    std::vector<TString> v=runDivider.Divide((tagChainMap["d"])["selected"], "data/runRanges/runRangeLimits.dat", nEvents_runDivide);
    runDivider.PrintRunRangeEvents();
    std::vector<TString> runRanges;
    if(runRangesFileName!="") runRanges = ReadRegionsFromFile(runRangesFileName);
    for(std::vector<TString>::const_iterator itr = runRanges.begin();
	itr != runRanges.end();
	itr++){
      std::cout << *itr << "\t" << "-1" << "\t" << runDivider.GetRunRangeTime(*itr) << std::endl;
    }

    return 0;
  }



  TString r(regionsFileName.c_str());
  r.Remove(0,r.Last('/')+1); r.ReplaceAll(".dat","");

  std::vector<TString> regions = ReadRegionsFromFile(regionsFileName);
  std::vector<TString> runRanges = ReadRegionsFromFile(runRangesFileName);
  std::vector<TString> categories;
  for(std::vector<TString>::const_iterator region_itr = regions.begin();
      region_itr != regions.end();
      region_itr++){
    if(runRanges.size()>0){
      for(std::vector<TString>::const_iterator runRange_itr = runRanges.begin();
	  runRange_itr!=runRanges.end();
	  runRange_itr++){
	TString token1,token2; 
	Ssiz_t ss=0;
	runRange_itr->Tokenize(token1,ss,"-");
	ss=runRange_itr->First('-');
	runRange_itr->Tokenize(token2,ss,"-");
	categories.push_back((*region_itr)+"-runNumber_"+token1+"_"+token2+"-"+commonCut.c_str());
      }
    }else categories.push_back((*region_itr)+"-"+commonCut.c_str());
  }


  ///------------------------------ to obtain r9weights
  r9Weights_class r9Weights;
  if(vm.count("saveR9TreeWeight") && !vm.count("r9WeightFile")){
    std::cerr << "[ERROR] No r9WeightFile specified" << std::endl;
    return 1;
  }
  if(vm.count("r9WeightFile")){
    r9Weights.ReadFromFile(r9WeightFile);

    // mc // save it in a file and reload it as a chain to be safe against the reference directory for the tree

    TFile r9File_mc("tmp/r9mcTree.root","recreate");
    if(r9File_mc.IsOpen()){
      r9File_mc.cd();
      TTree *r9Tree = r9Weights.GetTreeWeight((tagChainMap["s"])["selected"]);
      r9Tree->Write();
      delete r9Tree;
      r9File_mc.Write();
      r9File_mc.Close();
      std::pair<TString, TChain* > pair_tmp("r9Weight", new TChain("r9Weight"));
      (tagChainMap["s"]).insert(pair_tmp);
      (tagChainMap["s"])["r9Weight"]->SetTitle("s");
      (tagChainMap["s"])["r9Weight"]->Add("tmp/r9mcTree.root");
    } else {
      std::cerr << "[ERROR] " << std::endl;
      return 1;
    }


    // data
    TFile r9File_data("tmp/r9dataTree.root","recreate");
    if(r9File_data.IsOpen()){
      r9File_data.cd();
      TTree *r9Tree = r9Weights.GetTreeWeight((tagChainMap["d"])["selected"]);
      r9Tree->Write();
      delete r9Tree;
      r9File_data.Write();
      r9File_data.Close();
      std::pair<TString, TChain* > pair_tmp("r9Weight", new TChain("r9Weight"));
      (tagChainMap["d"]).insert(pair_tmp);
      (tagChainMap["d"])["r9Weight"]->SetTitle("d");
      (tagChainMap["d"])["r9Weight"]->Add("tmp/r9dataTree.root");
    }
  }
  if(vm.count("saveR9TreeWeight")) return 0;
  if((tagChainMap["s"]).count("r9Weight")){
    std::cout << "[STATUS] Adding r9Weight chain to signal chain as friend chain" << std::endl;
  }
  if((tagChainMap["d"]).count("r9Weight")){
    std::cout << "[STATUS] Adding r9Weight chain to data chain as friend chain" << std::endl;
    //	if((tagChainMap["d"])["selected"]->AddFriend((tagChainMap["d"])["r9Weight"])==NULL) return 2;
#ifdef DEBUG
    std::cout << "[DEBUG] r9Weight branch address " << (tagChainMap["d"])["selected"]->GetBranch("r9Weight") << std::endl;
#endif
  }


  //==============================

  //  if(vm.count("dataPU")==0 && (tagChainMap["s"]).count("pileupHist")==0 && (tagChainMap["s"]).count("pileup")==0){

  if(vm.count("noPU")==0 && !vm.count("runToy")){
    if(dataPUFileNameVec.empty() && (tagChainMap["s"]).count("pileup")==0){
      std::cerr << "[ERROR] Nor pileup mc tree configured in chain list file either dataPU histograms are not provided" << std::endl;
      return 1;
    }else if( vm.count("dataPU")!=0 || (!dataPUFileNameVec.empty() && (tagChainMap["s"]).count("pileup")==0)){
      std::cout << "[STATUS] Creating pileup weighting tree and saving it" << std::endl;
      for(unsigned int i=0; i < mcPUFileNameVec.size(); i++){
	TString mcPUFileName_=mcPUFileNameVec[i];
	TString dataPUFileName_=dataPUFileNameVec[i];
 	TString runMin_ = "";
	if(!mcPUFileName_.Contains("nPU")){
// 	if(mcPUFileName_.Index(".runMin_")!=-1){
 	  runMin_=mcPUFileName_;
 	  runMin_.Remove(0,runMin_.Last('/')+1);
 	  runMin_.Remove(runMin_.First('-'));
// 	  runMin_.Remove(runMin_.First('.'));
// 	  runMin_.ReplaceAll("runMin_","");
 	}
	int runMin = runMin_.Sizeof()>1 ? runMin_.Atoi() : 1;
	std::cout << "********* runMin = " << runMin << "\t" << runMin_ << std::endl;
	puWeights.ReadFromFiles(mcPUFileName_.Data(),dataPUFileName_.Data(), runMin);
      }
      TFile f("tmp/mcPUtree.root","recreate");
      if(f.IsOpen()){
	f.cd();
	TTree *puTree = puWeights.GetTreeWeight((tagChainMap["s"])["selected"],true);
	puTree->Write();
	delete puTree;
	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp("pileup", new TChain("pileup"));
	(tagChainMap["s"]).insert(pair_tmp);
	(tagChainMap["s"])["pileup"]->SetTitle("s");
	(tagChainMap["s"])["pileup"]->Add("tmp/mcPUtree.root");
      }
    }
  }


  //read corrections directly from file
  if (vm.count("corrEleType")){
    std::cout << "[STATUS] Getting energy scale corrections from file: " << corrEleFile << std::endl;
    TString treeName="scaleEle_"+corrEleType;
    if((tagChainMap["d"]).count(treeName)==0){
      if (vm.count("readDirect")){
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["d"]).insert(pair_tmp);
	(tagChainMap["d"])[treeName]->SetTitle("d");
	if((tagChainMap["d"])[treeName]->Add(TString(corrEleFile))<=0) return 1;
      }
      else {
	//std::cout<<"here, read ele corr from command line: "<<corrEleFile<<std::endl;
	EnergyScaleCorrection_class eScaler(corrEleFile,corrEleType);
	std::cout << "[STATUS] Saving electron scale corrections to root file:" << corrEleType+".root" << std::endl;
	std::cout << "data entries: " << (tagChainMap["d"])["selected"]->GetEntries() << std::endl;
	TFile f(TString(corrEleType+".root"),"recreate");
	if(f.IsOpen()){
	  TTree *corrTree = eScaler.GetCorrTree((tagChainMap["d"])["selected"]);
	  //corrTree->SetName("scaleEle");
	  //corrTree->SetTitle(corrEleType.c_str());
	  f.cd();
	  corrTree->Write();
	  std::cout << "corrEle entries: " << corrTree->GetEntries() << std::endl;
	  delete corrTree;

	  f.Write();
	  f.Close();
	  std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	  (tagChainMap["d"]).insert(pair_tmp);
	  (tagChainMap["d"])[treeName]->SetTitle("d");
	  if((tagChainMap["d"])[treeName]->Add(TString(corrEleType+".root"))<=0) return 1;
	}
      }
    }
    //if( (tagChainMap["d"])["selected"]->AddFriend((tagChainMap["d"])[treeName])==NULL) return 2;
  }

  //read corrections directly from file
  if (vm.count("smearEleType")){
    std::cout << "[STATUS] Getting energy smearings from file: " << smearEleFile << std::endl;
    EnergyScaleCorrection_class eScaler("","", smearEleFile,smearEleType);
    TString treeName="smearEle_"+smearEleType;
    TString treeFile="tmp/smearEle_"+smearEleType+"_mc.root";
    if((tagChainMap["s"]).count(treeName)==0){
      std::cout << "[STATUS] Saving electron smearings to root file:" << treeFile << std::endl;
      std::cout << "data entries: " << (tagChainMap["s"])["selected"]->GetEntries() << std::endl;
      TFile f(treeFile,"recreate");
      if(f.IsOpen()){
	TTree *corrTree = eScaler.GetSmearTree((tagChainMap["s"])["selected"], true, energyBranchName );
	f.cd();
	corrTree->Write();
	std::cout << "smearEle entries: " << corrTree->GetEntries() << std::endl;
	delete corrTree;

	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["s"]).insert(pair_tmp);
	(tagChainMap["s"])[treeName]->SetTitle("s");
	if((tagChainMap["s"])[treeName]->Add(treeFile)<=0) return 1;
	//(tagChainMap["s"])["selected"]->AddFriend((tagChainMap["s"])[treeName]);
      }
    }

#ifdef TOBEFIXED
    treeFile="tmp/smearEle_"+smearEleType+"_data.root";
    if((tagChainMap["d"]).count(treeName)==0){
      std::cout << "[STATUS] Saving electron smearings to root file:" << treeFile << std::endl;
      std::cout << "data entries: " << (tagChainMap["d"])["selected"]->GetEntries() << std::endl;
      TFile f(treeFile,"recreate");
      if(f.IsOpen()){
	TTree *corrTree = eScaler.GetSmearTree((tagChainMap["d"])["selected"], true, energyBranchName );
	f.cd();
	corrTree->Write();
	std::cout << "smearEle entries: " << corrTree->GetEntries() << std::endl;
	delete corrTree;

	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["d"]).insert(pair_tmp);
	(tagChainMap["d"])[treeName]->SetTitle("d");
	if((tagChainMap["d"])[treeName]->Add(treeFile)<=0) return 1;

      }

    }
#endif
  }


  addBranch_class newBrancher;
  newBrancher._commonCut=commonCut.c_str();
  newBrancher._regionList=categories;

  for( std::vector<string>::const_iterator branch_itr = branchList.begin();
       branch_itr != branchList.end();
       branch_itr++){
    UpdateFriends(tagChainMap);
    TString treeName=*branch_itr;
    if(treeName.Contains("invMassSigma")){
      newBrancher.scaler= new EnergyScaleCorrection_class("","", smearEleFile,smearEleType);
    }
    //data
    std::cout <<"[STATUS] Adding branch " << *branch_itr << " to data..." <<std::endl;

    TFile f(TString("tmp/"+*branch_itr+"_data.root"),"recreate");
    if (!f.IsOpen()){
      std::cerr << "[ERROR] File for branch " << *branch_itr << " not created" << std::endl;
      return 1;
    }
    TTree *newTree = newBrancher.AddBranch((tagChainMap["d"])["selected"],treeName+"_d", *branch_itr);

    if(newTree==NULL){
      std::cerr << "[ERROR] New tree for branch " << *branch_itr << " is NULL" << std::endl;
      return 1;
    }

    f.cd();
    newTree->Write();	
    delete newTree;
    f.Write();
    f.Close();
    std::pair<TString, TChain* > pair_tmp(treeName+"_d", new TChain(treeName+"_d"));
    (tagChainMap["d"]).insert(pair_tmp);
    (tagChainMap["d"])[treeName+"_d"]->SetTitle("d");
    if((tagChainMap["d"])[treeName+"_d"]->Add(TString("tmp/"+*branch_itr+"_data.root"))<=0) return 1;
    //    if( (tagChainMap["d"])["selected"]->AddFriend((tagChainMap["d"])[treeName])==NULL) return 2;

    //mc
    std::cout <<"[STATUS] Adding branch " << *branch_itr << " to mc..." <<std::endl;

    TFile f_mc(TString("tmp/"+*branch_itr+"_mc.root"),"recreate");
    if (!f_mc.IsOpen()){
      std::cerr << "[ERROR] File for branch " << *branch_itr << " not created" << std::endl;
      return 1;
    }
    newTree = newBrancher.AddBranch((tagChainMap["s"])["selected"],treeName+"_s", *branch_itr);
    if(newTree==NULL) return 1;
    f_mc.cd();
    newTree->Write();	
    delete newTree;
    f_mc.Write();
    f_mc.Close();
    pair_tmp = std::make_pair<TString, TChain* >(treeName+"_s", new TChain(treeName+"_s"));
    (tagChainMap["s"]).insert(pair_tmp);
    (tagChainMap["s"])[treeName+"_s"]->SetTitle("s");
    if((tagChainMap["s"])[treeName+"_s"]->Add(TString("tmp/"+*branch_itr+"_mc.root"))<=0) return 1;
    //    if( (tagChainMap["s"])["selected"]->AddFriend((tagChainMap["s"])[treeName])==NULL) return 2;
  }



  if (vm.count("addPtBranches")){
    TString treeName="AddedCorrPtBranches";
    //data
    if((tagChainMap["d"]).count(treeName)==0){
      std::cout <<"[STATUS] Adding corrected Pt branches to data..."<<std::endl;
      addBranch_class ptBr;
      TFile f(TString("tmp/AddedCorrPtBranches_data.root"),"recreate");
      if (f.IsOpen()){
	TTree *ptBranches = ptBr.AddBranch_Pt((tagChainMap["d"])["selected"],treeName);
	f.cd();
	ptBranches->Write();	
	delete ptBranches;
	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["d"]).insert(pair_tmp);
	(tagChainMap["d"])[treeName]->SetTitle("d");
	if((tagChainMap["d"])[treeName]->Add(TString("tmp/AddedCorrPtBranches_data.root"))<=0) return 1;
      }
    }
    //	if( (tagChainMap["d"])["selected"]->AddFriend((tagChainMap["d"])[treeName])==NULL) return 2;

    //mc
    if((tagChainMap["s"]).count(treeName)==0){
      std::cout <<"[STATUS] Adding corrected Pt branches to mc signal..."<<std::endl;
      addBranch_class ptBr;
      TFile f(TString("tmp/AddedCorrPtBranches_signal.root"),"recreate");
      if (f.IsOpen()){
	TTree *ptBranches = ptBr.AddBranch_Pt((tagChainMap["s"])["selected"],treeName);
	f.cd();
	ptBranches->Write();	
	delete ptBranches;
	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["s"]).insert(pair_tmp);
	(tagChainMap["s"])[treeName]->SetTitle("s");
	if((tagChainMap["s"])[treeName]->Add(TString("tmp/AddedCorrPtBranches_signal.root"))<=0) return 1;
      }
    }
    //if( (tagChainMap["s"])["selected"]->AddFriend((tagChainMap["s"])[treeName])==NULL) return 2;
  }


  //(tagChainMap["s"])["selected"]->GetEntries();

  UpdateFriends(tagChainMap);
  if(vm.count("saveRootMacro")){
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	  chain_itr!=tag_chain_itr->second.end();
	  chain_itr++){
	chain_itr->second->SaveAs("tmp/"+tag_chain_itr->first+"_"+chain_itr->first+"_chain.root");
	//chain_itr->second->SavePrimitive(std::cout); // not implemented
      }
    }
  }

  if(
     vm.count("saveRootMacro") 
     || vm.count("saveCorrEleTree")
     || vm.count("saveAddBranchTree")
     || vm.count("savePUTreeWeight")
     || vm.count("runDivide") 
     || vm.count("saveCorrEleTree") 
     || vm.count("saveR9TreeWeight") 
     ) return 0;

  eleID+=selection.c_str();
  eleID.ReplaceAll("_","");

  //------------------------------ RooSmearer
  RooArgSet args;
  std::vector<RooArgSet> args_vec;

  for(std::vector<TString>::const_iterator region_itr = categories.begin();
      region_itr != categories.end();
      region_itr++){
    RooRealVar *scale_ = new RooRealVar("scale_"+*region_itr, "scale_"+*region_itr,1.0, 0.95,1.05,"GeV"); //0.9,1.1,"GeV")
    scale_->setError(0.005); // 1%
    //    scale_->setConstant();
    //    scale_->setBinning(RooBinning(
    args.add(*scale_);

    TString varName=*region_itr;
    TPRegexp reg("Et_[0-9]*_[0-9]*");
    reg.Substitute(varName,"");
    varName.ReplaceAll("--","-");
    if(varName.First("-")==0) varName.Remove(0,1);

    //RooRealVar *const_term_ = new RooRealVar("constTerm_"+*region_itr, "constTerm_"+*region_itr, 0.01, 0.0005, 0.05);
    RooAbsReal *const_term_=NULL;
    RooRealVar *const_term_v = args.getSize() ==0 ? NULL : (RooRealVar *) args.find("constTerm_"+varName);
    if(const_term_v==NULL){
      if(vm.count("constTermFix")==0) const_term_v = new RooRealVar("constTerm_"+*region_itr, "constTerm_"+varName,0.01, 0.0005,0.05); 
      else const_term_v = new RooRealVar("constTerm_"+varName, "constTerm_"+varName,0.01, 0.0005,0.02);
      const_term_v->setError(0.03); // 1%
      //const_term_v->setConstant(true);
      args.add(*const_term_v);
    } 
    if(reg.MatchB(*region_itr) && vm.count("constTermFix")==1){
      const_term_ = new RooFormulaVar("constTerm_"+*region_itr, "constTerm_"+varName,"@0", *const_term_v);
      const_term_v->setConstant(false);
    } else const_term_ = const_term_v;



    RooAbsReal *alpha_=NULL;
    RooRealVar *alpha_v = args.getSize() ==0 ? NULL : (RooRealVar *) args.find("alpha_"+varName);
    if(alpha_v==NULL){
      alpha_v = new RooRealVar("alpha_"+varName, "alpha_"+varName,0.0, 0.,0.20);
      alpha_v->setError(0.01);
      alpha_v->setConstant(true);
      //alpha_v->Print();
      args.add(*alpha_v);
    } 
    if(reg.MatchB(*region_itr) && vm.count("constTermFix")==1){
      alpha_ = new RooFormulaVar("alpha_"+*region_itr, "alpha_"+varName,"@0", *alpha_v);
      alpha_v->setConstant(false);
    } else alpha_ = alpha_v;
    
    args_vec.push_back(RooArgSet(*scale_, *alpha_, *const_term_));
  }

  args.sort(kFALSE);
  if(vm.count("smearerFit")){
    std::cout << "------------------------------ smearer parameters" << std::endl;
    args.writeToStream(std::cout, kFALSE);
  }

  TRandom3 g(0);
  TString filename="tmp/tmpFile-"; filename+=g.Integer(1000000);filename+=".root";
  TFile *tmpFile = new TFile(filename,"recreate");
  tmpFile->cd();
  RooSmearer smearer("smearer",(tagChainMap["d"])["selected"], (tagChainMap["s"])["selected"], NULL, 
		     categories,
		     args_vec, args, energyBranchName);
  smearer._isDataSmeared=vm.count("isDataSmeared");
  smearer.SetOnlyDiagonal(vm.count("onlyDiagonal"));
  smearer._autoBin=vm.count("autoBin");
  smearer._autoNsmear=vm.count("autoNsmear");
  smearer.smearscan=vm.count("smearscan");
  //smearer.nEventsMinDiag = nEventsMinDiag;
  smearer._deactive_minEventsOffDiag = nEventsMinOffDiag;
  smearer.SetSmearingEt(vm.count("smearingEt"));

  //------------------------------ Take the list of branches needed for the defined categories
  ElectronCategory_class cutter;
  std::set<TString> activeBranchList;
  for(std::vector<TString>::const_iterator region_itr = categories.begin();
      region_itr != categories.end();
      region_itr++){
    std::set<TString> tmpList = cutter.GetBranchNameNtuple(*region_itr);
    activeBranchList.insert(tmpList.begin(),tmpList.end());
    // add also the friend branches!
  }

  //------------------------------ ZFit_class declare and set the options
  TChain *data = NULL;
  TChain *mc = NULL;
  if(!vm.count("smearerFit")){
    data= (tagChainMap["d"])["selected"];
    mc  = (tagChainMap["s"])["selected"];
  }
  ZFit_class fitter( data, mc, NULL, 
		    invMass_var.c_str(), invMass_min, invMass_max); 

  fitter._oddMC=vm.count("isOddMC");
  fitter._oddData=vm.count("isOddData");

  if(vm.count("r9WeightFile")){
    // if the data are weighted need to use the unbinned likelihood fit to have the correct errors
    fitter._isDataUnbinned=true;
    fitter._isDataSumW2=true;
  }

  fitter._forceNewFit=vm.count("forceNewFit");
  //  fitter._initFitMC=true;
  fitter.SetFitType(fit_type_value);
  fitter._updateOnly=vm.count("updateOnly");

  fitter.imgFormat=imgFormat;
  fitter.outDirFitResMC=outDirFitResMC;
  fitter.outDirFitResData=outDirFitResData;
  fitter.outDirImgMC=outDirImgMC;
  fitter.outDirImgData=outDirImgData;

  // check folder existance
  fitter.SetPDF_model(signal_type_value,0); // (0,0) convolution, CB no_bkg
  //fitter.SetPDF_model(1,0); // cruijff, no_bkg

  if(!vm.count("smearerFit")){ 

    fitter.Import(commonCut.c_str(), eleID, activeBranchList);
    for(std::vector<TString>::const_iterator category_itr=categories.begin();
	category_itr != categories.end();
	category_itr++){
      myClock.Start();
#ifdef DEBUG
      std::cout << "[DEBUG] category: " << *category_itr << std::endl;
#endif
      if (vm.count("runToy")) { 
	cout << "number of toys: "<<nToys<<endl;
	fitter.SetInitFileMC(outDirFitResMC+"/"+*category_itr+".txt");
	fitter.FitToy(*category_itr, nToys, nEventsPerToy); 
      }	
      else	fitter.Fit(*category_itr);
      myClock.Stop();
      myClock.Print();
    }
  }

  myClock.Reset();
  if(vm.count("smearerFit")){

    
	smearer.SetHistBinning(80,100,invMass_binWidth); // to do before Init
	if(vm.count("runToy")){
	  smearer.SetToyScale(1, constTermToy);
	  if(vm.count("initFile")) smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"), initFileName.c_str());
	  else smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"));
	  std::cout << "[DEBUG] " << constTermToy << std::endl;
	} else{
	  if(vm.count("initFile")) args.readFromFile(initFileName.c_str());
	  args.writeToStream(std::cout, kFALSE);
	  smearer.Init(commonCut.c_str(), eleID);
	}
// 	for(std::vector<ZeeCategory>::iterator cat_itr = smearer.ZeeCategories.begin();
// 		cat_itr != smearer.ZeeCategories.end();
// 		cat_itr++){
// 	  if((cat_itr->categoryName1.Contains("absEta_1_1.4442-gold") && !cat_itr->categoryName2.Contains("absEta_0_1-gold"))
// 		  ||
// 		  (cat_itr->categoryName2.Contains("absEta_1_1.4442-gold") && !cat_itr->categoryName1.Contains("absEta_0_1-gold"))
// 		) cat_itr->active=false;
// 	}  


	//RooArgList list(args);
	//m.simplex();
	//for(int i =0; i < list.getSize(); i++){
	//      list[i].Print();
	//if(TString(list[i].GetName()).Contains("constTerm")) ((RooRealVar *)list.at(i))->setConstant(kFALSE);
	//      list[i].Print();
	//}
	//if(vm.count("plotOnly")) //smearer.SetNSmear(10);
	RooMinuit m(smearer);
	if(! vm.count("plotOnly") && ! vm.count("profileOnly")){
	  //m.setStrategy(0);
	  //m.setVerbose();

	  //m.hesse(); // one turn of hess to guess the covariance matrix
	  //RooFitResult *fitres = m.save();
	  ShervinMinuit sh(100, smearer);//(RooArgSet&)fitres->floatParsFinal());
	  sh.SetMinuit(m);
	  //ph.SetVariables();
	  //ph.SetCovMatrix(fitres->covarianceMatrix());
	  //ph.SetUpdateProposalParameters(kTRUE); // auto-create mean vars and add mappings
	  //ph.SetCacheSize(100);
	  //ProposalFunction* pf = ph.GetProposalFunction();
	  
	  if(minimType=="migrad"){
	  
	    //fitres = 
	    m.fit("");
	    //m.migrad();
	    //m.hesse();
	  } else if(minimType=="shervinDNA"){
	    RooArgList 	 argList_(args);
	    TIterator 	*it_ = argList_.createIterator();
	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	      if (var->isConstant() || !var->isLValue()) continue;
	      TString  name(var->GetName());
	      if(!name.Contains("scale")) var->setConstant();
	    }
	    MinimizationProfile(smearer, args, nIter);	
	    delete it_;
	    it_ = argList_.createIterator();
	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	      if (!var->isLValue()) continue;
	      TString  name(var->GetName());
	      if(!name.Contains("scale")) var->setConstant(kFALSE);
	      else var->setConstant();
	    } 
	      
	      
	    args.writeToStream(std::cout, kFALSE);
	    RooMinimizer m(smearer);
	    m._theFitter->Config().SetMinimizer("Genetic","Genetic");
	    m.setEps(100); // default tolerance
	    m._theFitter->Config().MinimizerOptions().SetTolerance(2e-5);
	    // default max number of calls
	    m._theFitter->Config().MinimizerOptions().SetMaxIterations(50);
 	    m._theFitter->Config().MinimizerOptions().SetMaxFunctionCalls(10);
	    //m.setVerbose();
	    m.setPrintLevel(4);
	    m.minimize("Genetic","Genetic");

	  } else if(minimType=="genetic"){
	    RooMinimizer m(smearer);
	    //m.setMinimizerType("genetic");
	    m._theFitter->Config().SetMinimizer("Genetic","Genetic");
	    m.setEps(100); // default tolerance
	    m._theFitter->Config().MinimizerOptions().SetTolerance(2e-5);
	    // default max number of calls
	    m._theFitter->Config().MinimizerOptions().SetMaxIterations(10);
 	    m._theFitter->Config().MinimizerOptions().SetMaxFunctionCalls(10);
	    //	    m.setErrorLevel(0.1);
	    //m.setVerbose();
	    m.setPrintLevel(4);
	    m.minimize("Genetic","Genetic");
	    
	    //fitres = m.save();
	  } else if(minimType=="profile"){
	    MinimizationProfile(smearer, args, nIter);
	    args.writeToStream(std::cout, kFALSE);
	  } else if(minimType=="MCMC"){
	    MinimizationProfile(smearer, args, nIter, true);
	    args.writeToStream(std::cout, kFALSE);
	  } else if(minimType=="sampling"){
	    RooArgList 	 argList_(args);
	    TIterator 	*it_ = argList_.createIterator();
	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	      if (var->isConstant() || !var->isLValue()) continue;
	      TString  name(var->GetName());
	      if(name.Contains("scale")) var->setConstant();
	    }
	    
	    RooStats::ProposalHelper ph;
	    ph.SetVariables(args);
	    ph.SetCovMatrix(*(smearer._markov.GetAsDataSet()->covarianceMatrix(args)));
	    ph.SetUniformFraction(0.05);
	    ph.SetWidthRangeDivisor(2);

	    RooStats::MetropolisHastings MCMC(smearer, args, *(ph.GetProposalFunction()), nIter);
	    MCMC.SetNumBurnInSteps(10);
	    MCMC.SetSign(RooStats::MetropolisHastings::kNegative);
	    MCMC.SetType(RooStats::MetropolisHastings::kLog);
	    //MCMC.SetProposalFunction(sh);
	    RooStats::MarkovChain *mcChain = (MCMC.ConstructChain());
	    mcChain->SaveAs("tmp/newChain.root");
	    delete mcChain;

	  }
	}	    

	std::cout << "[INFO] Minimization time: \t";
	myClock.Stop();
	myClock.Print();
	if(!vm.count("profileOnly") && !vm.count("plotOnly")){
	  args.writeToFile(outDirFitResData+"/params-"+r+"-"+TString(commonCut.c_str())+".txt");
	  smearer._markov.SaveAs((outDirFitResData+"/markov-"+r+"-"+TString(commonCut.c_str())+".root"));
	}

	//RooDataSet *dSet = smearer.GetMarkovChainAsDataSet();
	//dSet->SaveAs("tmp/dataset.root");
	//save the smeared histograms

	for(std::vector<ZeeCategory>::iterator itr= smearer.ZeeCategories.begin();
		itr != smearer.ZeeCategories.end();
		itr++){
	  smearer.GetSmearedHisto(*itr, true, false);
	  smearer.GetSmearedHisto(*itr, true, true);
	  smearer.GetSmearedHisto(*itr, false, smearer._isDataSmeared);
	} 
	
	TFile *f = new TFile(outDirFitResData+"/histos-"+r+"-"+TString(commonCut.c_str())+".root", "recreate");
	f->Print();
	f->cd();
	for(std::vector<ZeeCategory>::iterator itr= smearer.ZeeCategories.begin();
		itr != smearer.ZeeCategories.end();
		itr++){
	  //if(!itr->active) continue;
	  TH1F *MC = smearer.GetSmearedHisto(*itr, true, false);
	  TH1F *smearMC = smearer.GetSmearedHisto(*itr, true, true);
	  TH1F *data = smearer.GetSmearedHisto(*itr, false, smearer._isDataSmeared);
	  
	  MC->Write();
	  smearMC->Write();
	  data->Write();
	  f->Write();    
	} 

	f->Close();
	if(!vm.count("plotOnly")){
	  Int_t oldMarkovSize=smearer._markov.Size();
	  //smearer.SetNSmear(10);

	  std::cout <<"==================PROFILE=================="<<endl;
	  smearer.SetNSmear(0,10);
	  //create profiles
	  TString outFile=outDirFitResData.c_str();
	  outFile+="/outProfile-";
	  outFile+=r+"-"+TString(commonCut.c_str())+".root";
	  TFile *fOutProfile = new TFile(outFile,"recreate");
	  //test/dato/fitres/Hgg_Et_v7/0.03//outProfile-data/regions/test.dat-Et_25-EB.root
	  for (int ivar=0;ivar<args.getSize();++ivar)
	    {
	      RooArgList argList(args);
	      RooRealVar* var=(RooRealVar*)(argList.at(ivar));
	      if (var->isConstant())
		continue;

	      TString name(var->GetName());

	      // special part for alpha fitting 
	      double min=0.;
	      TString  alphaName=name; alphaName.ReplaceAll("constTerm","alpha");
	      RooRealVar *var2= name.Contains("constTerm") ? (RooRealVar *)argList.find(alphaName): NULL;
	      if(var2!=NULL && name.Contains("constTerm") && var2->isConstant()==false){
		smearer.SetDataSet(name,TString(var->GetName())+TString(var2->GetName()));
		MinProfile2D(var, var2, smearer, -1, 0., min, false);
		//MinMCMC2D(var, var2, smearer, 1, 0., min, 1200, false);
		//MinMCMC2D(var, var2, smearer, 2, 0., min, 800, false);
		//MinMCMC2D(var, var2, smearer, 3, 0., min, 100, false);
		smearer.dataset->Write();
		TString of=outFile; of.ReplaceAll(".root",name+".root");
		smearer.dataset->SaveAs(of);
	      }
	      continue;
	      std::cout << "Doing " << name << "\t" << var->getVal() << std::endl;
	      
	      TGraph *profil = GetProfile(var, smearer,0);
	      TString n="profileChi2_"+name;
	      profil->SetName(n);  
	      TCanvas c("c_"+name);
	      profil->Draw("AP*");
	      fOutProfile->cd();
	      profil->Write();
	      std::cout << "Saved profile for " << name << std::endl;
	      delete profil;
	      
	    }
	  std::cout << "Cloning args" << std::endl;
	  //	  RooArgSet *mu = (RooArgSet *)args.snapshot();
	  //std::cout << "creating roomultivargaussian" << std::endl;
	  //RooMultiVarGaussian g("multi","",args, *mu, *(smearer.GetMarkovChainAsDataSet()->covarianceMatrix()));
	  //std::cout << "created" << std::endl;

	  // 	  RooDataSet *dataset = g.generate(args,10);
	  // 	  for(Long64_t iGen=0; iGen<1; iGen++){  
	  // 	    RooArgList argList_(* dataset->get(iGen));
	  // 	    TIterator *it_ = argList_.createIterator();
	  // 	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	  // 	      if (!var->isLValue()) continue;
	  // 	      TString  name(var->GetName());
	  // 	      //std::cout << name << var->getVal() << std::endl;
	  // 	      var->Print();
	  // 	      //args.setRealValue(name,var->getVal());
// 	    }
// 	    //smearer.evaluate();
// 	  }
	  RooStats::MarkovChain mchain;
	  mchain.AddWithBurnIn(smearer._markov, oldMarkovSize);
	  mchain.Write();
	  fOutProfile->Close();
	  
	}
  }
  tmpFile->Close();
  globalClock.Stop();
  std::cout << "[INFO] Total elapsed time: "; globalClock.Print(); 
  return 0;
}

