#include "../interface/EnergyScaleCorrection_class.hh"
#include <RooDataSet.h>
#include <RooArgSet.h>

#define NGENMAX 10000

// for exit(0)
#include <stdlib.h>
// for setw
#include <iomanip>
//for istreamstring
#include <sstream>

//#define DEBUG

EnergyScaleCorrection_class::EnergyScaleCorrection_class(TString correctionFileName, 
							 TString smearingFileName):
  noCorrections(true), noSmearings(true), 
  smearingType_(0),
  rgen_(NULL),
  deltaM("\\Delta m",   "Bias",   0, -35, 10, "GeV/c^{2}"),
  sigma(  "\\sigma_{CB}","Width", 2.14, 0.,  10.0, "GeV/c^{2}"),
  cut(    "\\alpha",     "Cut",   1.424, 0.01, 5.0),
  power(  "\\gamma",     "Power", 1.86, 0.5, 100.0),
  x("x","",0,-50,100,"GeV/c^{2}"),
  resCB("resCB", "A  Crystal Ball Lineshape", x, deltaM, sigma, cut, power)
{
  if(smearingFileName.Sizeof()>1) noSmearings=false;
  else noSmearings=true;

  if(correctionFileName.Sizeof()>1) noCorrections=false;
  else noCorrections=true;

  if(!noCorrections){
    ReadFromFile(correctionFileName);
    if(scales.empty()){
      std::cerr << "[ERROR] scale correction map empty" << std::endl;
      noCorrections=true;
      exit(1);
    }
  }
  
  if(!noSmearings){
    ReadSmearingFromFile(smearingFileName);
    if(smearings.empty()){
      std::cerr << "[ERROR] scale correction map empty" << std::endl;
      noCorrections=true;
      exit(1);
    }
  }
  rgen_=new TRandom3(0); // inizializzo il generatore con seed impostato sull'ora

  return;
}

EnergyScaleCorrection_class::~EnergyScaleCorrection_class(void){
  return;
}



  
float EnergyScaleCorrection_class::getScaleOffset(int runNumber, bool isEBEle, double R9Ele, double etaSCEle, double EtEle){
  if(noCorrections) return 1;
  
  correctionCategory_class category(runNumber, etaSCEle, R9Ele, EtEle);
  correction_map_t::const_iterator corr_itr = scales.find(category);
  if(corr_itr==scales.end()){ // if not in the standard classes, add it in the list of not defined classes
    if(scales_not_defined.count(category)==0){
      correctionValue_class corr;
      scales_not_defined[category]=corr;
    }
    corr_itr = scales_not_defined.find(category);
    std::cout << "[ERROR] Category not found: " << std::endl;
    std::cout << category << std::endl;
    //     exit(1);
  }

#ifdef DEBUG
  std::cout << "[DEBUG] Checking correction for category: " << category << std::endl;
  std::cout << "[DEBUG] Correction is: " << corr_itr->second
	    << std::endl 
    	    << "        given for category " <<  corr_itr->first << std::endl;;
#endif

  return corr_itr->second.scale;

}

void EnergyScaleCorrection_class::Add(TString category_, int runMin_, int runMax_, double deltaP_, double err_deltaP_){

  correctionCategory_class cat(category_);
  cat.runmin=runMin_;
  cat.runmax=runMax_;

  if(scales.count(cat)!=0){
    std::cerr << "[ERROR] Category already defined!" << std::endl;
    std::cerr << "        Adding category:  " << cat << std::endl;
    std::cerr << "        Defined category: " << scales[cat] << std::endl;
    exit(1);
  }

  correctionValue_class corr;
  corr.scale=deltaP_;
  corr.scale_err=err_deltaP_;
  scales[cat]=corr;

  std::cout << "[INFO:scale correction] " << cat << corr << std::endl;
  return;
}

  /*
   *  File structure:
   *  category  "runNumber"   runMin  runMax   deltaP  err_deltaP
   *  
   */
void EnergyScaleCorrection_class::ReadFromFile(TString filename){
  std::cout << "[STATUS] Reading recalibration values from file: " << filename << std::endl;
  ifstream f_in(filename);
  if(!f_in.good()){
    std::cerr << "[ERROR] file " << filename << " not readable" << std::endl;
    exit(1);
    return;
  }
  
  int runMin, runMax;
  TString category, region2;
  double deltaP, err_deltaP;


  for(f_in >> category; f_in.good(); f_in >> category){
    f_in >> region2 
	 >> runMin >> runMax 
	 >> deltaP >> err_deltaP;

    Add(category, runMin, runMax, deltaP, err_deltaP);
  }
  
  f_in.close();

  noCorrections=false;
  return;
}



// tree is the input MC tree
// fastLoop = false if for any reason you don't want to change the branch status of the MC tree

TTree *EnergyScaleCorrection_class::GetCorrTree(TChain *tree, bool fastLoop, 
						TString runNumberBranchName,
						TString R9EleBranchName,
						TString etaEleBranchName,
						TString etaSCEleBranchName,
						TString energySCEleBranchName,
						TString nPVBranchName
						){
  float nPVmean = 0;
//   if(correctionType.Contains("nPV"))
//     nPVmean = GetMean_nPV(tree, fastLoop,nPVBranchName);

  Int_t nPV_;
  Int_t runNumber_;
  Float_t etaEle_[2];
  Float_t etaSCEle_[2];
  Float_t R9Ele_[2];
  Float_t energySCEle_[2];

  Float_t scaleEle_[2];

  TTree *newTree = new TTree("scaleEle",""); //+correctionType,correctionType);
  newTree->Branch("scaleEle", scaleEle_, "scaleEle[2]/F");

  if(fastLoop){
    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus(runNumberBranchName,1);
    tree->SetBranchStatus(etaEleBranchName,1);
    tree->SetBranchStatus(etaSCEleBranchName,1);
    tree->SetBranchStatus(energySCEleBranchName,1);
    tree->SetBranchStatus(R9EleBranchName,1);
    tree->SetBranchStatus(nPVBranchName,1);
  }

  tree->SetBranchAddress(nPVBranchName, &nPV_);
  tree->SetBranchAddress(runNumberBranchName, &runNumber_);
  tree->SetBranchAddress(etaEleBranchName,etaEle_);
  tree->SetBranchAddress(etaSCEleBranchName,etaSCEle_);
  tree->SetBranchAddress(energySCEleBranchName,energySCEle_);
  tree->SetBranchAddress(R9EleBranchName, R9Ele_);
  Long64_t nentries = tree->GetEntries();



  std::cout << "[STATUS] Get scaleEle for tree: " << tree->GetTitle() 
	    << "\t" << "with " << nentries << " entries" << std::endl;
  std::cerr << "[00%]";

  // loop over tree 
  for(Long64_t ientry = 0; ientry<nentries; ientry++){
    tree->GetEntry(ientry);
    scaleEle_[0] = ScaleCorrection(runNumber_, fabs(etaSCEle_[0]) < 1.4442, 
				   R9Ele_[0],etaEle_[0], energySCEle_[0]/cosh(etaSCEle_[0]), nPV_, nPVmean);
    scaleEle_[1] = ScaleCorrection(runNumber_, fabs(etaSCEle_[1]) < 1.4442, 
				   R9Ele_[1],etaEle_[1], energySCEle_[1]/cosh(etaSCEle_[1]), nPV_, nPVmean);
    newTree->Fill();
    if(ientry%(nentries/100)==0) std::cerr << "\b\b\b\b" << std::setw(2) << ientry/(nentries/100) << "%]";
  }

  
  if(fastLoop) tree->SetBranchStatus("*",1);
  tree->ResetBranchAddresses();
  tree->GetEntry(0);
  //  std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
  return newTree;
}



//============================== SMEARING
void EnergyScaleCorrection_class::AddSmearing(TString category_, int runMin_, int runMax_, 
					      double constTerm, double err_constTerm, double alpha, double err_alpha,
					      double Emean, double err_Emean){

  correctionCategory_class cat(category_);
  cat.runmin= (runMin_ < 0) ? 0 : runMin_;
  cat.runmax=runMax_;

  if(scales.count(cat)!=0){
    std::cerr << "[ERROR] Category already defined!" << std::endl;
    std::cerr << "        Adding category:  " << cat << std::endl;
    std::cerr << "        Defined category: " << scales[cat] << std::endl;
    exit(1);
  }

  correctionValue_class corr;
  corr.constTerm    = constTerm;
  corr.constTerm_err= err_constTerm;
  corr.alpha        = alpha;
  corr.alpha_err    = err_alpha;
  corr.Emean        = Emean; 
  corr.Emean_err    = err_Emean;
  smearings[cat]=corr;

  std::cout << "[INFO:smearings] " << cat << corr << std::endl;
#ifdef DEBUG
  std::cout << corr << std::endl;
#endif
  return;
}

/** 
 *  File structure:
EBlowEtaBad8TeV    0 0.0 1.0 -999. 0.94 -999999 999999 6.73 0. 7.7e-3  6.32e-4 0.00 0.16
EBlowEtaGold8TeV   0 0.0 1.0 0.94  999. -999999 999999 6.60 0. 7.4e-3  6.50e-4 0.00 0.16
EBhighEtaBad8TeV   0 1.0 1.5 -999. 0.94 -999999 999999 6.73 0. 1.26e-2 1.03e-3 0.00 0.07
EBhighEtaGold8TeV  0 1.0 1.5 0.94  999. -999999 999999 6.52 0. 1.12e-2 1.32e-3 0.00 0.22
##################################################################################################
EElowEtaBad8TeV    0 1.5 2.0 -999. 0.94 -999999 999999 0.   0. 1.98e-2 3.03e-3 0.  0.
EElowEtaGold8TeV   0 1.5 2.0 0.94  999. -999999 999999 0.   0. 1.63e-2 1.22e-3 0.  0.
EEhighEtaBad8TeV   0 2.0 3.0 -999. 0.94 -999999 999999 0.   0. 1.92e-2 9.22e-4 0.  0.
EEhighEtaGold8TeV  0 2.0 3.0 0.94  999. -999999 999999 0.   0. 1.86e-2 7.81e-4 0.  0.
##################################################################################################
*
 */

void EnergyScaleCorrection_class::ReadSmearingFromFile(TString filename){
  std::cout << "[STATUS] Reading smearing values from file: " << filename << std::endl;
  ifstream f_in(filename);
  if(!f_in.good()){
    std::cerr << "[ERROR] file " << filename << " not readable" << std::endl;
    exit(1);
  }
  
  int runMin=0, runMax=900000;
  int unused=0;
  TString category, region2;
  //double smearing, err_smearing;
  double rho, phi, Emean, constTerm, alpha, err_rho, err_phi, err_Emean, err_alpha=0., err_constTerm=0.;
  double etaMin, etaMax, r9Min, r9Max;

  int format=0; ///< 0=unknown; 1=globe; 2=ECALELF toy
  while(f_in.peek()!=EOF && f_in.good()){
    if(f_in.peek()==10){ // 10 = \n
      f_in.get(); 
      continue;
    }

    if(f_in.peek() == 35){ // 35 = #
      f_in.ignore(1000,10); // ignore the rest of the line until \n
      continue;
    }

    if(format==0){
      std::string line;
      std::getline(f_in, line);
      std::istringstream s_in(line);
      s_in >> category >> unused >> etaMin >> etaMax 
	   >> r9Min >> r9Max >> runMin >> runMax 
	   >> Emean >> err_Emean 
	   >> rho >> err_rho >> phi >> err_phi;
      if(s_in.eof()){ // this is not the globe format but the ECALELF format
	std::cout << "[INFO] format=2" << std::endl;
	format=2;
	s_in >> category >> constTerm >> alpha;
	AddSmearing(category, runMin, runMax, constTerm,  err_constTerm, alpha, err_alpha, Emean, err_Emean);
      }else{
	format=1;
	std::cout << "[INFO] format=1" << std::endl;
	if(Emean!=0){
	  constTerm=rho*sin(phi);
	  alpha=rho*Emean*cos(phi);
	}else{
	  alpha=0;
	  constTerm=rho;
	}
	
	AddSmearing(category, runMin, runMax, constTerm,  err_constTerm, alpha, err_alpha, Emean, err_Emean);
	
      }
    }else if(format==1){
      f_in >> category >> unused >> etaMin >> etaMax >> r9Min >> r9Max >> runMin >> runMax >> 
	Emean >> err_Emean >> 
	rho >> err_rho >> phi >> err_phi;
      if(Emean!=0){
	constTerm=rho*sin(phi);
	alpha=rho*Emean*cos(phi);
      }else{
	alpha=0;
	constTerm=rho;
      }
      
      AddSmearing(category, runMin, runMax, constTerm,  err_constTerm, alpha, err_alpha, Emean, err_Emean);
      
    }else{
      f_in >> category >> constTerm >> alpha;
      AddSmearing(category, runMin, runMax, constTerm,  err_constTerm, alpha, err_alpha, Emean, err_Emean);
    } 
#ifdef DEBUG
    std::cout << category << "\t" << etaMin << "\t" << etaMax << "\t" << r9Min << "\t" << r9Max << "\t" << runMin << "\t" << runMax << "\tEmean=" << Emean << "\t" << rho << "\t" << phi << std::endl;
#endif
  }
  
  f_in.close();
  //  runCorrection_itr=runMin_map.begin();
  
  noSmearings=false;
  
  return;
}



float EnergyScaleCorrection_class::getSmearingSigma(int runNumber, float energy, bool isEBEle, float R9Ele, float etaSCEle){
  
  correctionCategory_class category(runNumber, etaSCEle, R9Ele, energy/cosh(etaSCEle));
  correction_map_t::const_iterator corr_itr = smearings.find(category);
  if(corr_itr==smearings.end()){ // if not in the standard classes, add it in the list of not defined classes
    if(smearings_not_defined.count(category)==0){
      correctionValue_class corr;
      smearings_not_defined[category]=corr;
    }
    corr_itr = smearings_not_defined.find(category);
    std::cerr << "[WARNING] Category not found: " << std::endl;
    std::cerr << category << std::endl;
    //     exit(1);
  }

#ifdef DEBUG
  std::cout << "[DEBUG] Checking correction for category: " << category << std::endl;
  std::cout << "[DEBUG] Correction is: " << corr_itr->second
	    << std::endl 
    	    << "        given for category " <<  corr_itr->first;
#endif

  double constTerm=corr_itr->second.constTerm;
  double alpha=corr_itr->second.alpha;
  return sqrt(constTerm*constTerm + alpha*alpha/(energy/cosh(etaSCEle)));

}

float EnergyScaleCorrection_class::getSmearingRho(int runNumber, float energy, bool isEBEle, float R9Ele, float etaSCEle){
  
  correctionCategory_class category(runNumber, etaSCEle, R9Ele, energy/cosh(etaSCEle));
  correction_map_t::const_iterator corr_itr = smearings.find(category);
  if(corr_itr==smearings.end()){ // if not in the standard classes, add it in the list of not defined classes
    if(smearings_not_defined.count(category)==0){
      correctionValue_class corr;
      smearings_not_defined[category]=corr;
    }
    corr_itr = smearings_not_defined.find(category);
  }

  double constTerm=corr_itr->second.constTerm;
  double alpha = (corr_itr->second.Emean==0) ? 0 : corr_itr->second.alpha/corr_itr->second.Emean;
  
  //std::cout << (*corr_itr).second << std::endl;
  return sqrt(constTerm*constTerm + alpha*alpha); ///< return rho

}

float EnergyScaleCorrection_class::getSmearing(int runNumber, float energy, bool isEBEle, float R9Ele, float etaSCEle){
  
  if(noSmearings) return 1;
  float smear =getSmearingSigma(runNumber, energy, isEBEle, R9Ele, etaSCEle); 
  
  if(smear==0) return 0;
  if( smear!=smear) return 0;
//     correctionCategory_class category(runNumber, etaSCEle, R9Ele, energy/cosh(etaSCEle));
//     correction_map_t::const_iterator corr_itr = smearings.find(category);

//     std::cerr << category << std::endl << corr_itr->second << std::endl;
//     exit(1);
//   }
  if(smearingType_==1){
    if(sigma.getVal()!=smear){
      sigma.setVal(smear);
      index_data=NGENMAX; //force to produce new random values
    }
    
    if(index_data>=NGENMAX-1){
      if(data!=NULL) delete data;
      data=resCB.generate(x,NGENMAX);
      index_data=-1;
    }
    index_data++;
    return 1+data->get(index_data)->getRealValue("x");


  }else return rgen_->Gaus(1,getSmearingSigma(runNumber, energy, isEBEle, R9Ele, etaSCEle));
  
}


TTree *EnergyScaleCorrection_class::GetSmearTree(TChain *tree, 
						 bool fastLoop, 
						 TString energyEleBranchName,
						 TString runNumberBranchName,
						 TString R9EleBranchName,
						 TString etaEleBranchName,
						 TString etaSCEleBranchName
						 
						 ){
  Int_t runNumber_;
  Float_t energyEle_[2];
  Float_t etaEle_[2];
  Float_t etaSCEle_[2];
  Float_t R9Ele_[2];

  Float_t smearEle_[2];
  Float_t smearSigmaEle_[2];

  TTree *newTree = new TTree("smearEle_",""); //+correctionType,correctionType);
  newTree->Branch("smearEle", smearEle_, "smearEle[2]/F");
  newTree->Branch("smearSigmaEle", smearSigmaEle_, "smearSigmaEle[2]/F");

  if(tree==NULL){
    std::cerr << "[ERROR] original chain in GetSmearTree is NULL" << std::endl;
    exit(1);
  }
  if(fastLoop){
    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus(runNumberBranchName,1);
    tree->SetBranchStatus(energyEleBranchName,1);
    tree->SetBranchStatus(etaEleBranchName,1);
    tree->SetBranchStatus(etaSCEleBranchName,1);
    tree->SetBranchStatus(R9EleBranchName,1);
  }

  tree->SetBranchAddress(runNumberBranchName, &runNumber_);
  tree->SetBranchAddress(energyEleBranchName, energyEle_);
  tree->SetBranchAddress(etaEleBranchName,etaEle_);
  tree->SetBranchAddress(etaSCEleBranchName,etaSCEle_);
  tree->SetBranchAddress(R9EleBranchName, R9Ele_);
  
  // loop over tree 
  std::cerr << "Adding smearEle branch \t";
  std::cerr << "[00%]";

  Long64_t nentries = tree->GetEntries();
  for(Long64_t ientry = 0; ientry<tree->GetEntriesFast(); ientry++){
    tree->GetEntry(ientry);
    smearSigmaEle_[0] = getSmearingSigma(runNumber_, energyEle_[0], fabs(etaSCEle_[0]) < 1.4442, 
					 R9Ele_[0],etaEle_[0]);
    smearSigmaEle_[1] = getSmearingSigma(runNumber_, energyEle_[1], fabs(etaSCEle_[1]) < 1.4442, 
					 R9Ele_[1],etaEle_[1]);
    smearEle_[0] = getSmearing(runNumber_, energyEle_[0], fabs(etaSCEle_[0]) < 1.4442, 
			       R9Ele_[0],etaEle_[0]);
    smearEle_[1] = getSmearing(runNumber_, energyEle_[1], fabs(etaSCEle_[1]) < 1.4442, 
			       R9Ele_[1],etaEle_[1]);
    
    newTree->Fill();
    if(ientry%(nentries/100)==0) std::cerr << "\b\b\b\b" << std::setw(2) << ientry/(nentries/100) << "%]";

  }
  std::cerr << std::endl;
  
  if(fastLoop) tree->SetBranchStatus("*",1);
  tree->ResetBranchAddresses();
  tree->GetEntry(0);
  return newTree;
}




float EnergyScaleCorrection_class::ScaleCorrection(int runNumber, bool isEBEle, 
						   double R9Ele, double etaSCEle, double EtEle, 
						   int nPV, float nPVmean){
  float correction = 1;
  //  if(correctionType.Contains("Run", TString::kIgnoreCase)) 
  correction*=getScaleOffset(runNumber, isEBEle, R9Ele, etaSCEle, EtEle);
  
  //  if(correctionType.Contains("nPV",TString::kIgnoreCase)) correction *= 1- (-0.02 * (nPV -nPVmean))/100;
  
  return correction;

}


float EnergyScaleCorrection_class::GetMean_nPV(TChain *tree, bool fastLoop, 
						TString nPVBranchName
						){
  float nPVmean=0;
  Int_t nPV_;
  if(fastLoop){
    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus(nPVBranchName,1);
  }

  tree->SetBranchAddress(nPVBranchName, &nPV_);
  Long64_t nentries = tree->GetEntries();

  // loop over tree 
  for(Long64_t ientry = 0; ientry<nentries; ientry++){
    tree->GetEntry(ientry);
    nPVmean+=nPV_;
  }
  nPVmean/=nentries;
  
  if(fastLoop) tree->SetBranchStatus("*",1);
  tree->ResetBranchAddresses();
  tree->GetEntry(0);
  //  std::cout << "[WARNING] nPU > nPU max for " << warningCounter << " times" << std::endl;
  return nPVmean;
}


bool correctionCategory_class::operator<(const correctionCategory_class& b) const{
    if(runmin < b.runmin && runmax < b.runmax) return true;  
    if(runmax > b.runmax && runmin > b.runmin) return false; 
       
    if(etamin < b.etamin && etamax<b.etamax) return true; 
    if(etamax > b.etamax && etamin>b.etamin) return false;
       
    if(r9min  < b.r9min && r9max < b.r9max) return true; 
    if(r9max  > b.r9max && r9min > b.r9min) return  false;
       
    if(etmin  < b.etmin && etmax < b.etmax) return true;  
    if(etmax  > b.etmax && etmin > b.etmin) return  false;
    return false;

}    

correctionCategory_class::correctionCategory_class(TString category_){
    std::string category(category_.Data());
#ifdef DEBUG
    std::cout << "[DEBUG] correctionClass defined for category: " << category << std::endl;
#endif
    // default values 
    runmin=0; runmax=999999;
    etamin=-1;etamax=10;
    r9min=-1;r9max=10;
    etmin=-1;etmax=99999.;
    
    size_t p1,p2; // boundary
    // eta region
    p1 = category.find("absEta_");
    p2 = p1+1;
    if(category.find("absEta_0_1")!=std::string::npos){ etamin=0; etamax=1;}
    else if(category.find("absEta_1_1.4442")!=std::string::npos){ etamin=1; etamax=1.479;} //etamax=1.4442; }
    else if(category.find("absEta_1.566_2")!=std::string::npos){ etamin=1.479; etamax=2;} //etamin=1.566; etamax=2;}
    else if(category.find("absEta_2_2.5")!=std::string::npos){ etamin=2; etamax=3;}
    else{
      if(p1!=std::string::npos){ 
	p1 = category.find("_",p1);
	p2 = category.find("_",p1+1);
	etamin = TString(category.substr(p1+1, p2-p1-1)).Atof();
	p1 = p2;
	p2 = category.find("-",p1); 
	etamax = TString(category.substr(p1+1, p2-p1-1)).Atof();
      }
    }

    if(category.find("EBlowEta")!=std::string::npos){ etamin=0; etamax=1;};
    if(category.find("EBhighEta")!=std::string::npos){ etamin=1; etamax=1.479;};
    if(category.find("EElowEta")!=std::string::npos){ etamin=1.479; etamax=2;};
    if(category.find("EEhighEta")!=std::string::npos){ etamin=2; etamax=7;};

    // Et region
    p1 = category.find("-Et_");
    p2 = p1+1;
    //    std::cout << "p1 = " << p1 << "\t" << std::string::npos << "\t" << category.size() << std::endl;
    //std::cout << etmin << "\t" << etmax << "\t" << category.substr(p1+1, p2-p1-1) << "\t" << p1 << "\t" << p2 << std::endl;
    if(p1 !=std::string::npos){ 
      p1 = category.find("_",p1);
      p2 = category.find("_",p1+1);
      etmin = TString(category.substr(p1+1, p2-p1-1)).Atof();
      p1 = p2;
      p2 = category.find("-",p1); 
      etmax = TString(category.substr(p1+1, p2-p1-1)).Atof();
      std::cout << etmin << "\t" << etmax << "\t" << category.substr(p1+1, p2-p1-1) << std::endl;
    }

    if(category.find("gold")!=std::string::npos || category.find("Gold")!=std::string::npos){ r9min=0.94; r9max=10;}
    else if(category.find("bad")!=std::string::npos || category.find("Bad")!=std::string::npos){ r9min=-1; r9max=0.94;};
    
}
