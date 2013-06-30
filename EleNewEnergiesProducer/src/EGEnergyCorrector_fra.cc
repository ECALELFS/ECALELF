// $Id: EGEnergyCorrector_fra.cc,v 1.11 2012/06/17 20:19:52 bendavid Exp $

#include <TFile.h>
#include <TString.h>
#include "../interface/EGEnergyCorrector_fra.h"
#ifdef CMSSW42X
#include "CondFormats/EgammaObjects/interface/GBRWrapper.h"
#endif
#include "CondFormats/EgammaObjects/interface/GBRForest.h"
#include "CondFormats/DataRecord/interface/GBRWrapperRcd.h"
#include "FWCore/Framework/interface/ESHandle.h" 
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include <iterator>

using namespace reco;
using namespace std;

//--------------------------------------------------------------------------------------------------
EGEnergyCorrector_fra::EGEnergyCorrector_fra() :
  fReadereb(0),
  fReaderebvariance(0),
  fReaderee(0),
  fReadereevariance(0),
  fIsInitialized(kFALSE),
  fOwnsForests(kFALSE),
  fVals(0),
  _GetCorrections(true)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
EGEnergyCorrector_fra::~EGEnergyCorrector_fra()
{

  if (fVals) delete [] fVals;

  if (fOwnsForests) {
    if (fReadereb) delete fReadereb;
    if (fReaderebvariance) delete fReaderebvariance;  
    if (fReaderee) delete fReaderee;
    if (fReadereevariance) delete fReadereevariance;
  }

}

//--------------------------------------------------------------------------------------------------
void EGEnergyCorrector_fra::Initialize(const edm::EventSetup &iSetup, std::string regweights, bool weightsFromDB, bool ptSplit) {
  fIsInitialized = kTRUE;

  if (fVals) delete [] fVals;
  if (fOwnsForests) {
    if (fReadereb) delete fReadereb;
    //      if (fReaderebvariance) delete fReaderebvariance;  
    if (fReaderee) delete fReaderee;
    //      if (fReadereevariance) delete fReadereevariance;    
  }    

  fVals = new Float_t[73];

  if (weightsFromDB) { //weights from event setup
#ifndef CMSSW42X
    edm::ESHandle<GBRForest> readereb;
    edm::ESHandle<GBRForest> readerebvar;
    edm::ESHandle<GBRForest> readeree;
    edm::ESHandle<GBRForest> readereevar;
#else
    edm::ESHandle<GBRWrapper> readereb;
    edm::ESHandle<GBRWrapper> readerebvar;
    edm::ESHandle<GBRWrapper> readeree;
    edm::ESHandle<GBRWrapper> readereevar;
#endif
    iSetup.get<GBRWrapperRcd>().get(std::string(TString::Format("%s_EBCorrection",regweights.c_str())),readereb);
    iSetup.get<GBRWrapperRcd>().get(std::string(TString::Format("%s_EBUncertainty",regweights.c_str())),readerebvar);
    iSetup.get<GBRWrapperRcd>().get(std::string(TString::Format("%s_EECorrection",regweights.c_str())),readeree);
    iSetup.get<GBRWrapperRcd>().get(std::string(TString::Format("%s_EEUncertainty",regweights.c_str())),readereevar);

#ifndef CMSSW42X
    fReadereb 	      = readereb.product();
    fReaderebvariance = readerebvar.product();
    fReaderee 	      = readeree.product();
    fReadereevariance = readereevar.product();
#else
    fReadereb 	      = &readereb->GetForest();
    fReaderebvariance = &readerebvar->GetForest();
    fReaderee 	      = &readeree->GetForest();
    fReadereevariance = &readereevar->GetForest();
#endif
  } else { 			//weights from root file
    if((TString(regweights)).Contains("noweight",TString::kIgnoreCase) || (TString(regweights)).Contains("nocorrection",TString::kIgnoreCase)){
      std::cout << "[WARNING] Fra corrections not used" << std::endl;
      _GetCorrections=false;
    }else{

      fOwnsForests = kTRUE;
      TFile *fgbr = TFile::Open(regweights.c_str(),"READ");
      if(!fgbr->IsOpen() || fgbr->IsZombie()){
	edm::LogError("EGEnergyCorrector_fra") << "Reading file " << regweights << " error: file not opended";
	exit(1);
      }

      if(ptSplit== false){
	fReadereb = (GBRForest*)fgbr->Get("EBCorrection");
	fReaderebvariance = (GBRForest*)fgbr->Get("EBUncertainty");  
	fReaderee = (GBRForest*)fgbr->Get("EECorrection");
	fReadereevariance = (GBRForest*)fgbr->Get("EEUncertainty");      
      }else{
	std::cout<<"reading forest"<<std::endl;
	fReadereb_lowPt = (GBRForest*)fgbr->Get("EBCorrection_lowPt");
	fReaderee_lowPt = (GBRForest*)fgbr->Get("EECorrection_lowPt");
	 
	fReadereb_highPt = (GBRForest*)fgbr->Get("EBCorrection_highPt");
	fReaderee_highPt = (GBRForest*)fgbr->Get("EECorrection_highPt");

	if(fReadereb_lowPt==NULL)std::cout<<"ebcorr not found"<<std::endl;
      }


      varnameseb=(vector<string> *)fgbr->Get("varlisteb");
      varnamesee=(vector<string> *)fgbr->Get("varlistee");
      //      vector<int*>::iterator g;
       
      if(varnameseb==NULL)std::cout<<" var name file not found";
      fgbr->Close();
    
    }
  }
  
}

//--------------------------------------------------------------------------------------------------

std::pair<double,double> EGEnergyCorrector_fra::CorrectedEnergyWithErrorTracker(const GsfElectron &e, const reco::VertexCollection& vtxcol, double rho, EcalClusterLazyTools &clustertools, const edm::EventSetup &es, bool ptSplit) {
  if(!_GetCorrections) return std::make_pair<double,double>(0,0);

  const SuperClusterRef s = e.superCluster();
  const CaloClusterPtr b = s->seed(); //seed  basic cluster

  Bool_t isbarrel = b->hitsAndFractions().at(0).first.subdetId()==EcalBarrel;

  //seed basic cluster variables

  double bemax = clustertools.eMax(*b);
  double be2nd = clustertools.e2nd(*b);
  double betop = clustertools.eTop(*b);
  double bebottom = clustertools.eBottom(*b);
  double beleft = clustertools.eLeft(*b);
  double beright = clustertools.eRight(*b);


  double be2x5max = clustertools.e2x5Max(*b);
  double be2x5top = clustertools.e2x5Top(*b);
  double be2x5bottom = clustertools.e2x5Bottom(*b);
  double be2x5left = clustertools.e2x5Left(*b);
  double be2x5right = clustertools.e2x5Right(*b);



  
  //local coordinates and crystal indices (barrel only)    
  
  //seed cluster
  float betacry, bphicry, bthetatilt, bphitilt;
  int bieta, biphi;
  

  if(isbarrel){
    _ecalLocal.localCoordsEB(*b,es,betacry,bphicry,bieta,biphi,bthetatilt,bphitilt);
  }

  std::vector<std::string> *varVector;

  if(isbarrel){
    varVector=varnameseb;
  }else{
    varVector=varnamesee;
  }
    
  int j=0;
  for(std::vector<std::string>::iterator i=varVector->begin();i<varVector->end();i++) {
    fVals[j]=-1000;
    if(*i =="SCRawEnergy")fVals[j]=s->rawEnergy();
    else if(*i =="scEta")fVals[j]=s->eta();
    else if(*i =="scPhi")fVals[j]=s->phi();
    else if(*i =="R9")fVals[j]=clustertools.e3x3(*b)/s->rawEnergy(); //r9 
    else if(*i =="E5x5Seed/SCRawEnergy")fVals[j]=clustertools.e5x5(*b)/s->rawEnergy();      
    else if(*i =="etawidth")fVals[j]=  s->etaWidth();	
    else if(*i =="phiwidth")fVals[j]=  s->phiWidth();	
    else if(*i =="NClusters")fVals[j]= s->clustersSize();
#ifndef CMSSW42X
    else if(*i =="HoE")fVals[j]=	  e.hcalOverEcalBc();
#else 
    else if(*i =="HoE")fVals[j]=	  e.hadronicOverEm();
#endif
    else if(*i =="rho")fVals[j]=  rho;		
    else if(*i =="vertices")fVals[j]= vtxcol.size();    
    else if(*i =="EtaSeed-scEta")fVals[j]=b->eta()-s->eta();
    else if(*i =="atan2(sin(PhiSeed-scPhi),cos(PhiSeed-scPhi))")fVals[j]=reco::deltaPhi(b->phi(),s->phi());
    else if(*i =="ESeed/SCRawEnergy")fVals[j]=b->energy()/s->rawEnergy();
    else if(*i =="E3x3Seed/ESeed")fVals[j]=clustertools.e3x3(*b)/b->energy();
    else if(*i =="E5x5Seed/ESeed")fVals[j]=clustertools.e5x5(*b)/b->energy();
    else if(*i =="see")fVals[j]= sqrt(clustertools.localCovariances(*b)[0]); //sigietaieta
    else if(*i =="spp")fVals[j]=  sqrt(clustertools.localCovariances(*b)[2]); //sigiphiiphi
    else if(*i =="sep")fVals[j]=   clustertools.localCovariances(*b)[1];       //sigietaiphi
    else if(*i =="EMaxSeed/ESeed")fVals[j]=bemax/b->energy();
    else if(*i =="E2ndSeed/ESeed")fVals[j]=be2nd/b->energy();
    else if(*i =="ETopSeed/ESeed")fVals[j]=betop/b->energy();
    else if(*i =="EBottomSeed/ESeed")fVals[j]=bebottom/b->energy();
    else if(*i =="ELeftSeed/ESeed")fVals[j]=beleft/b->energy();
    else if(*i =="ERightSeed/ESeed")fVals[j]=beright/b->energy();
    else if(*i =="E2x5MaxSeed/ESeed")fVals[j]=be2x5max/b->energy();
    else if(*i =="E2x5TopSeed/ESeed")fVals[j]=be2x5top/b->energy();
    else if(*i =="E2x5BottomSeed/ESeed")fVals[j]=be2x5bottom/b->energy();
    else if(*i =="E2x5LeftSeed/ESeed")fVals[j]=be2x5left/b->energy();
    else if(*i =="E2x5RightSeed/ESeed")fVals[j]=be2x5right/b->energy();
    else if(*i =="pmeangsf/pmodegsf")fVals[j]=e.gsfTrack()->p()/e.gsfTrack()->pMode();
    else if(*i =="fbrem")fVals[j]=e.fbrem();
    else if(*i =="Charge")fVals[j]=e.charge();
    else if(*i =="EoP")fVals[j]=e.eSuperClusterOverP();
    else if(*i =="IEtaSeed")fVals[j]=bieta;
    else if(*i =="IPhiSeed")fVals[j]=biphi;
    else if(*i =="IEtaSeed%5")fVals[j]=bieta%5;
    else if(*i =="IPhiSeed%2")fVals[j]=biphi%2;
    else if(*i =="(abs(IEtaSeed)<=25)*(IEtaSeed%25) + (abs(IEtaSeed)>25)*((IEtaSeed-25*abs(IEtaSeed)/IEtaSeed)%20)")fVals[j]=(TMath::Abs(bieta)<=25)*(bieta%25) + (TMath::Abs(bieta)>25)*((bieta-25*TMath::Abs(bieta)/bieta)%20);
    else if(*i =="IPhiSeed%20")fVals[j]=biphi%20;
    else if(*i =="EtaCrySeed")fVals[j]=betacry;
    else if(*i =="PhiCrySeed")fVals[j]= bphicry;
    else if(*i =="PreShowerOverRaw")fVals[j]=s->preshowerEnergy()/s->rawEnergy();
    else if(*i == "gsfchi2")fVals[j]=e.gsfTrack()->normalizedChi2();
    else if(*i == "deta")fVals[j]=e.deltaEtaSuperClusterTrackAtVtx();
    else if(*i == "dphi")fVals[j]=e.deltaPhiSuperClusterTrackAtVtx();
    else if(*i == "pmeangsf/pmeankf")
      fVals[j] = (e.closestCtfTrackRef().isNonnull()) ? e.gsfTrack()->p()/e.closestCtfTrackRef()->p() : -1;
    else if(*i == "detacalo")fVals[j]=e.deltaEtaSeedClusterTrackAtCalo();
    else if(*i == "dphicalo")fVals[j]=e.deltaPhiSeedClusterTrackAtCalo();
    else if(*i == "kfhits")
      fVals[j] = (e.closestCtfTrackRef().isNonnull()) ? e.closestCtfTrackRef()->found() : fVals[j]=-1;
    else if(*i == "pterrorgsf")fVals[j]=e.gsfTrack()->ptModeError();
    else if(*i == "pt")fVals[j]=e.pt();

    else if(fVals[j]==-1000){
      edm::LogError("EGEnergyCorrector_fra") << "Variable " << *i <<" not found ";
      exit(1);
    }
    j++;
  }


    
  Double_t den;
  const GBRForest *reader;
  const GBRForest *readervar;
  if (isbarrel) {
    if(!ptSplit){
      den = s->rawEnergy();
      reader = fReadereb;
      readervar = fReaderebvariance;
    }else{
      readervar = fReaderebvariance;
      den = s->rawEnergy();
      if(den/cosh(s->eta())<15){
	reader = fReadereb_lowPt;
      }else{
	reader = fReadereb_highPt;
      }
    }
  }  else {
    if(!ptSplit){
      den = s->rawEnergy() + s->preshowerEnergy();
      reader = fReaderee;
      readervar = fReadereevariance;
    }else{
      readervar = fReadereevariance;
      den = s->rawEnergy()+ s->preshowerEnergy();
      if(den/cosh(s->eta())<15){
	reader = fReaderee_lowPt;
      }else{
	reader = fReaderee_highPt;
      }

    }
  }
  
  Double_t ecor = reader->GetResponse(fVals)*den;
  Double_t ecorerr = readervar->GetResponse(fVals)*den;


  //printf("ecor = %5f, ecorerr = %5f\n",ecor,ecorerr);
  
  return std::pair<double,double>(ecor,ecorerr);
}

