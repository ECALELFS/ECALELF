#ifndef SimpleCutBasedElectronIDSelectionFunctorShervin_h
#define SimpleCutBasedElectronIDSelectionFunctorShervin_h

//#define DEBUG

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "PhysicsTools/SelectorUtils/interface/Selector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include <TString.h>



// system include files
#include <memory>
#include <iostream>


// root include files
#include <TTree.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TFile.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"


#define RECHIT
#ifdef RECHIT
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#endif
// //#include "Calibration/EcalCalibAlgos/interface/ElectronCalibAnalyzer.h"

#define NEWRELEASE
#define REGRESSION
//#define REGRESSIONv3
//#define REGRESSIONv2
//#define REGRESSIONv1
#define ClusterTools
#ifdef ClusterTools
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#endif

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

// #include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
// #include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
// #include "DataFormats/TrackReco/interface/TrackFwd.h"
// #include "DataFormats/TrackReco/interface/Track.h"

#define CONVERSIONS
#ifdef CONVERSIONS
//#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "RecoEgamma/EgammaTools/interface/ConversionInfo.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#endif

//#include "DataFormats/BeamSpot/interface/BeamSpot.h"
/*
___________________________________________________________________________________

Description:
^^^^^^^^^^^^
    This is a class that implements the Simple Cut Based Electron 
    Identification cuts.  A more detailed description of the cuts
    and the tuning method can be found on this twiki:
    
    https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID

    For more information on how to calculate the magnetic field
    look here:

    https://twiki.cern.ch/twiki/bin/viewauth/CMS/ConversionBackgroundRejection
___________________________________________________________________________________

How to use:
^^^^^^^^^^^
    From CMSSW39 onwards you can simply define an instance of this class:

      SimpleCutBasedElectronIDSelectionFunctor patSele95
      (SimpleCutBasedElectronIDSelectionFunctor::relIso95);

    and get the decision with the following method:
      pat::Electron *myElec = .....;
      bool pass = patSele90(*myElec);

    The various options are listed in the enumeration Version_t. There
    is also the option to enter as a constructor argument a PSet
    with your favorite cuts.
___________________________________________________________________________________

    Contacts: Nikolaos Rompotis and Chris Seez
    Nikolaos dot Rompotis at Cern dot ch
    Chris    dot Seez     at Cern dot ch

    Author:    Nikolaos Rompotis
               many thanks to Sal Rappoccio
    Imperial College London
    7 June 2010, first commit for CMSSW_3_6_1_patchX
    11July 2010, implementing the ICHEP Egamma recommendation for 
                 removing the Delta Eta cut in the endcaps
    30Sept 2010, simplification of conversion rejection in CMSSW39X
___________________________________________________________________________________


Strongly modified by Shervin
*/


class SimpleCutBasedElectronIDSelectionFunctor : public Selector<reco::GsfElectron>  {

 public: // interface  
  
  enum Version_t { NONE=0, fiducial, WP80PU, WP90PU, WP70PU };
  
  //  SimpleCutBasedElectronIDSelectionFunctor(): {}

#ifdef shervin  
  // initialize it by inserting directly the cut values in a parameter set
  SimpleCutBasedElectronIDSelectionFunctor(edm::ParameterSet const & parameters)
    {
      // get the cuts from the PS
      initialize( parameters.getParameter<Double_t>("trackIso_EB"), 
		  parameters.getParameter<Double_t>("ecalIso_EB"), 
		  parameters.getParameter<Double_t>("hcalIso_EB"), 
		  parameters.getParameter<Double_t>("sihih_EB"), 
		  parameters.getParameter<Double_t>("dphi_EB"), 
		  parameters.getParameter<Double_t>("deta_EB"), 
		  parameters.getParameter<Double_t>("hoe_EB"), 
		  parameters.getParameter<Double_t>("cIso_EB"), 
		  parameters.getParameter<Double_t>("trackIso_EE"), 
		  parameters.getParameter<Double_t>("ecalIso_EE"), 
		  parameters.getParameter<Double_t>("hcalIso_EE"), 
		  parameters.getParameter<Double_t>("sihih_EE"), 
		  parameters.getParameter<Double_t>("dphi_EE"), 
		  parameters.getParameter<Double_t>("deta_EE"), 
		  parameters.getParameter<Double_t>("hoe_EE"), 
		  parameters.getParameter<Int_t>("conversionRejection"), 
		  parameters.getParameter<Int_t>("maxNumberOfExpectedMissingHits"));
      retInternal_ = getBitTemplate();
    }
#endif

  // initialize it by using only the version name
  SimpleCutBasedElectronIDSelectionFunctor(Version_t  version, const edm::Handle<reco::ConversionCollection>& ConversionsHandle, const edm::Handle<reco::BeamSpot>& BeamSpotHandle, const edm::Handle<double>& rhoHandle):
    ConversionsHandle_(ConversionsHandle),
    BeamSpotHandle_(BeamSpotHandle),
    rhoHandle_(rhoHandle)
    {
      if (version == NONE) {
	std::cout << "SimpleCutBasedElectronIDSelectionFunctor: If you want to use version NONE "
		  << "then you have also to provide the selection cuts by yourself " << std::endl;
	std::cout << "SimpleCutBasedElectronIDSelectionFunctor: ID Version is changed to 80cIso "
		  << std::endl;
	version = WP80PU;
      }
      initialize(version);
      retInternal_ = getBitTemplate();
    }

  SimpleCutBasedElectronIDSelectionFunctor(TString versionStr, const edm::Handle<reco::ConversionCollection>& ConversionsHandle, const edm::Handle<reco::BeamSpot>& BeamSpotHandle, const edm::Handle<double>& rhoHandle):
    ConversionsHandle_(ConversionsHandle),
    BeamSpotHandle_(BeamSpotHandle),
    rhoHandle_(rhoHandle)
    {
      Version_t version=NONE;
      if (versionStr.CompareTo("NONE")==0) {
	std::cout << "SimpleCutBasedElectronIDSelectionFunctor: If you want to use version NONE "
		  << "then you have also to provide the selection cuts by yourself " << std::endl;
	std::cout << "SimpleCutBasedElectronIDSelectionFunctor: ID Version is changed to 80cIso "
		  << std::endl;
	version = WP80PU;
      } else if (versionStr.CompareTo("fiducial")==0) version=fiducial;
      else if (versionStr.CompareTo("WP70PU")==0) version=WP70PU;
      else if (versionStr.CompareTo("WP80PU")==0) version=WP80PU;
      else if (versionStr.CompareTo("WP90PU")==0) version=WP90PU;
      else {
	std::cerr << "[ERROR] version type not defined" << std::endl;
	std::cerr << "[ERROR] using WP80PU" << std::endl;
	version=WP80PU;
      }
      
      initialize(version);
      retInternal_ = getBitTemplate();
    }

  void initialize( Version_t version ) 
  {
    version_ = version;

    //------------------------------ initialize the cut variables (to be put in the bitmask)
    // push back the variables
    push_back("fiducial");

    // conversion rejection
    push_back("maxNumberOfExpectedMissingHits" );
    push_back("hasMatchedConversion");

    //eleID EB
    push_back("hoe_EB"     );
    push_back("deta_EB"    );
    push_back("dphi_EB"    );
    push_back("sihih_EB"   );
    push_back("pfmva_EB"   );
    //eleIso EB
    push_back("relTrackIso_EB");
    push_back("relEcalIso_EB" );
    push_back("relHcalIso_EB" );

    //eleID EE
    push_back("hoe_EE"     );
    push_back("deta_EE"    );
    push_back("dphi_EE"    );
    push_back("sihih_EE"   );
    push_back("pfmva_EE"   );
    //eleIso EE
    push_back("relTrackIso_EE");
    push_back("relEcalIso_EE" );
    push_back("relHcalIso_EE" );

    //    push_back("conversionRejection"            );

    // set the cut values and active cuts for WP80 selection PU corrected with rho
    if (version_ == fiducial){
      set("fiducial");
    } else if (version_ == WP80PU) {
      //      std::cout << "[DEBUG] Iniziatializing WP80PU" << std::endl;
      
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      
      set("hoe_EB",      0.04);
      set("deta_EB",     4.0e-03);
      set("dphi_EB",     6.0e-02);
      set("sihih_EB",    1.0e-02);
      //set("pfmva_EB", 0.1);
      set("relTrackIso_EB", 9.0e-02);
      set("relEcalIso_EB",  7.0e-02);
      set("relHcalIso_EB",  1.0e-01);

      set("hoe_EE",      1.5e-01);
      set("deta_EE",     7.0e-03);
      set("dphi_EE",     2.0e-02);
      set("sihih_EE",    3.0e-02);
      //set("pfmva_EE", 0.1);
      set("relTrackIso_EE", 4.0e-02);
      set("relEcalIso_EE",  5.0e-02);
      set("relHcalIso_EE",  2.5e-02);
    } 
    // set the cut values and active cuts for WP80 selection PU corrected with rho
    else if (version_ == WP90PU) {
      //      set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      
      set("hoe_EB",      0.12);
      set("deta_EB",     7.0e-03);
      set("dphi_EB",     8.0e-01);
      set("sihih_EB",    1.0e-02);
      //set("pfmva_EB", 0.1);
      set("relTrackIso_EB", 1.2e-01);
      set("relEcalIso_EB",  9.0e-02);
      set("relHcalIso_EB",  1.0e-01);

      set("hoe_EE",      1.5e-01);
      set("deta_EE",     9.0e-03);
      set("dphi_EE",     7.0e-01);
      set("sihih_EE",    3.0e-02);
      //set("pfmva_EE", 0.1);
      set("relTrackIso_EE", 5.0e-02);
      set("relEcalIso_EE",  6.0e-02);
      set("relHcalIso_EE",  3.0e-02);
    }
    else if (version_ == WP70PU) {
      //      set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      
      set("hoe_EB",      0.12);
      set("deta_EB",     7.0e-03);
      set("dphi_EB",     8.0e-01);
      set("sihih_EB",    1.0e-02);
      //set("pfmva_EB", 0.1);
      set("relTrackIso_EB", 1.2e-01);
      set("relEcalIso_EB",  9.0e-02);
      set("relHcalIso_EB",  1.0e-01);

      set("hoe_EE",      1.5e-01);
      set("deta_EE",     9.0e-03);
      set("dphi_EE",     7.0e-01);
      set("sihih_EE",    3.0e-02);
      //set("pfmva_EE", 0.1);
      set("relTrackIso_EE", 5.0e-02);
      set("relEcalIso_EE",  6.0e-02);
      set("relHcalIso_EE",  3.0e-02);
    }

  }

#ifdef shervin
  // this function is to initialize the cuts to arbitrary values provided from cfg file
  // the others are hard coded
  void initialize(Double_t trackIso_EB, Double_t ecalIso_EB, Double_t hcalIso_EB,
		  Double_t sihih_EB, Double_t  dphi_EB, Double_t deta_EB, Double_t hoe_EB,
		  Double_t cIso_EB,
		  Double_t trackIso_EE, Double_t ecalIso_EE, Double_t hcalIso_EE,
		  Double_t sihih_EE, Double_t  dphi_EE, Double_t deta_EE, Double_t hoe_EE,
		  Double_t cIso_EE, Int_t conversionRejection, 
		  Int_t maxNumberOfExpectedMissingHits)
  {
    version_ = NONE;
    push_back("trackIso_EB");
    push_back("ecalIso_EB" );
    push_back("hcalIso_EB" );
    push_back("sihih_EB"   );
    push_back("dphi_EB"    );
    push_back("deta_EB"    );
    push_back("hoe_EB"     );

    
    push_back("trackIso_EE");
    push_back("ecalIso_EE" );
    push_back("hcalIso_EE" );
    push_back("sihih_EE"   );
    push_back("dphi_EE"    );
    push_back("deta_EE"    );
    push_back("hoe_EE"     );

    
    push_back("conversionRejection"            );
    push_back("maxNumberOfExpectedMissingHits" );
    
   
    set("trackIso_EB", trackIso_EB);
    set("ecalIso_EB",  ecalIso_EB);
    set("hcalIso_EB",  hcalIso_EB);
    set("sihih_EB",    sihih_EB);
    set("dphi_EB",     dphi_EB);
    set("deta_EB",     deta_EB);
    set("hoe_EB",      hoe_EB);
    set("cIso_EB",     cIso_EB);
    
    set("trackIso_EE", trackIso_EE);
    set("ecalIso_EE",  ecalIso_EE);
    set("hcalIso_EE",  hcalIso_EE);
    set("sihih_EE",    sihih_EE);
    set("dphi_EE",     dphi_EE);
    set("deta_EE",     deta_EE);
    set("hoe_EE",      hoe_EE);
    set("cIso_EE",     cIso_EE);
    
    set("conversionRejection",            conversionRejection);
    set("maxNumberOfExpectedMissingHits", maxNumberOfExpectedMissingHits);
    
  }
#endif

  bool operator()( const reco::GsfElectron & electron, pat::strbitset& ret)
  {
    // new electron, clear old electron bitmask
    retInternal_ = getBitTemplate();
    // for the time being only WPxx_PU variable definition
    return WPxx_PU(electron, ret );
  }
  float result(){ 
    // this should be modified in order to return:
    //    // 0: fails,
    // 1: passes electron ID only,
    // 2: passes electron Isolation only,
    // 3: passes electron ID and Isolation only,
    // 4: passes conversion rejection,
    // 5: passes conversion rejection and ID,
    // 6: passes conversion rejection and Isolation,
    // 7: passes the whole selection.
    return (float) retInternal_;
  }

  using Selector<reco::GsfElectron>::operator();
  // function with the Spring10 variable definitions
  bool WPxx_PU( const reco::GsfElectron & electron, pat::strbitset& ret)
  {
    
    //    ret.set(false);
    // effective areas
    float AeffTk_EB = 0;
    float AeffECAL_EB = 0.096;
    float AeffHCAL_EB = 0.020;
    float AeffTk_EE = 0;
    float AeffECAL_EE = 0.044;
    float AeffHCAL_EE = 0.041;

    //
    //    Double_t eleET = electron.p4().Pt();
    Double_t eleET = electron.et();
    //    Double_t etSCEle = electron.superCluster()->energy() *sin(electron.superCluster()->position().theta());
    Double_t trackIso = electron.dr03TkSumPt()/eleET;
    Double_t ecalIso = electron.dr03EcalRecHitSumEt()/eleET;
    Double_t hcalIso = electron.dr03HcalTowerSumEt()/eleET;
    Double_t sihih   = electron.sigmaIetaIeta();
    Double_t Dphi    = electron.deltaPhiSuperClusterTrackAtVtx();
    Double_t Deta    = electron.deltaEtaSuperClusterTrackAtVtx();
    Double_t HoE     = electron.hadronicOverEm();
    Double_t pfMVA   = electron.mva();
#ifdef shervin
    Double_t cIso    = 0;
    if (electron.isEB()) { cIso = 
	( electron.dr03TkSumPt() + std::max(0.,electron.dr03EcalRecHitSumEt() -1.) 
	  + electron.dr03HcalTowerSumEt() ) / eleET;
    }
    else {
      cIso = ( electron.dr03TkSumPt()+electron.dr03EcalRecHitSumEt()+
	       electron.dr03HcalTowerSumEt()  ) / eleET;
    }
#endif
    Int_t innerHits = electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
    // in 39 conversion rejection variables are accessible from Gsf electron
#ifdef shervin
    Double_t dist = electron.convDist(); // default value is -9999 if conversion partner not found
    Double_t dcot = electron.convDcot(); // default value is -9999 if conversion partner not found
    Bool_t isConv = fabs(dist) < 0.02 && fabs(dcot) < 0.02;
#endif

#ifdef DEBUG
    std::cout << "[DEBUG] " << ConversionsHandle_->size() << "\t" << BeamSpotHandle_.isValid() << "\t" << *rhoHandle_ << std::endl;
#endif
    bool hasMatchedConversion = ConversionTools::hasMatchedConversion(electron, ConversionsHandle_, BeamSpotHandle_->position());
    Double_t absEtaSC = fabs(electron.superCluster()->eta());
    Double_t rhoRel = *rhoHandle_ / eleET;

    //------------------------------ Fiducial region cut
    if ( (absEtaSC < 1.4442 || (absEtaSC > 1.566 && absEtaSC < 2.5) ) || ignoreCut("fiducial")) passCut(retInternal_, "fiducial");

    //------------------------------ conversion rejection cut
    if ( innerHits  < cut("maxNumberOfExpectedMissingHits", int()) || ignoreCut("maxNumberOfExpectedMissingHits")) 
      passCut(retInternal_, "maxNumberOfExpectedMissingHits");    
    if ( (!hasMatchedConversion) || ignoreCut("hasMatchedConversion")) passCut(retInternal_, "hasMatchedConversion");
 
   // now apply the cuts
    
    if (electron.isEB()) { // BARREL case
      // check the EB cuts

    if ( HoE        <  cut("hoe_EB",      double()) || ignoreCut("hoe_EB")     ) passCut(retInternal_, "hoe_EB");
    if ( fabs(Deta) <  cut("deta_EB",     double()) || ignoreCut("deta_EB")    ) passCut(retInternal_, "deta_EB");
    if ( fabs(Dphi) <  cut("dphi_EB",     double()) || ignoreCut("dphi_EB")    ) passCut(retInternal_, "dphi_EB");
    if ( sihih      <  cut("sihih_EB",    double()) || ignoreCut("sihih_EB")   ) passCut(retInternal_, "sihih_EB");
    if ( pfMVA      >  cut("pfmva_EB",    double()) || ignoreCut("pfmva_EB")   ) passCut(retInternal_, "pfmva_EB");

    if ( trackIso - AeffTk_EB   *rhoRel <  cut("relTrackIso_EB", double()) || ignoreCut("relTrackIso_EB")) 
      passCut(retInternal_, "relTrackIso_EB");
    if ( ecalIso  - AeffECAL_EB *rhoRel <  cut("relEcalIso_EB",  double()) || ignoreCut("relEcalIso_EB") ) 
      passCut(retInternal_, "relEcalIso_EB");
    if ( hcalIso  - AeffHCAL_EB *rhoRel <  cut("relHcalIso_EB",  double()) || ignoreCut("relHcalIso_EB") ) 
      passCut(retInternal_, "relHcalIso_EB");

    // pass all the EE cuts
    passCut(retInternal_, "hoe_EE");
    passCut(retInternal_, "deta_EE");	
    passCut(retInternal_, "dphi_EE");	
    passCut(retInternal_, "sihih_EE");	
    passCut(retInternal_, "pfmva_EE");
    passCut(retInternal_, "relTrackIso_EE");	
    passCut(retInternal_, "relEcalIso_EE");	
    passCut(retInternal_, "relHcalIso_EE");	
    
    } else {  // ENDCAPS case
    if ( HoE        <  cut("hoe_EE",      double()) || ignoreCut("hoe_EE")     ) passCut(retInternal_, "hoe_EE");
    if ( fabs(Deta) <  cut("deta_EE",     double()) || ignoreCut("deta_EE")    ) passCut(retInternal_, "deta_EE");
    if ( fabs(Dphi) <  cut("dphi_EE",     double()) || ignoreCut("dphi_EE")    ) passCut(retInternal_, "dphi_EE");
    if ( sihih      <  cut("sihih_EE",    double()) || ignoreCut("sihih_EE")   ) passCut(retInternal_, "sihih_EE");
    if ( pfMVA      >  cut("pfmva_EE",    double()) || ignoreCut("pfmva_EE")   ) passCut(retInternal_, "pfmva_EE");

    if ( trackIso - AeffTk_EE   *rhoRel <  cut("relTrackIso_EE", double()) || ignoreCut("relTrackIso_EE")) 
      passCut(retInternal_, "relTrackIso_EE");
    if ( ecalIso  - AeffECAL_EE *rhoRel <  cut("relEcalIso_EE",  double()) || ignoreCut("relEcalIso_EE") ) 
       passCut(retInternal_, "relEcalIso_EE");
    if ( hcalIso  - AeffHCAL_EE *rhoRel <  cut("relHcalIso_EE",  double()) || ignoreCut("relHcalIso_EE") ) 
      passCut(retInternal_, "relHcalIso_EE");

    // pass all the EB cuts
    passCut(retInternal_, "hoe_EB");
    passCut(retInternal_, "deta_EB");	
    passCut(retInternal_, "dphi_EB");	
    passCut(retInternal_, "sihih_EB");	
    passCut(retInternal_, "pfmva_EB");
    passCut(retInternal_, "relTrackIso_EB");	
    passCut(retInternal_, "relEcalIso_EB");	
    passCut(retInternal_, "relHcalIso_EB");	

    }
    setIgnored(retInternal_);   
#ifdef DEBUG
    print(std::cout);
    std::cout << "[DEBUG] retInternal_ = " << retInternal_ << std::endl;
    std::cout << "[DEBUG] ret = " << ret << std::endl;
    ret=retInternal_;
    std::cout << "[DEBUG] copy ret = " << (bool) ret << std::endl;
#endif
    //    ret= getBitTemplate();
    return (bool)retInternal_;
  }

  int bitMask(){
    int mask=0;
    pat::strbitset::bit_vector retBits = retInternal_.bits();
    for(pat::strbitset::bit_vector::const_iterator bitIter = retBits.begin();
	bitIter != retBits.end();
	bitIter++){
      int value=*bitIter << (bitIter-retBits.begin());
      mask = mask | value;
    }
    return mask;
  }

 private: // member variables
  // version of the cuts  
  Version_t version_;
  const edm::Handle<reco::ConversionCollection>& ConversionsHandle_;
  const edm::Handle<reco::BeamSpot>& BeamSpotHandle_;
  const edm::Handle<double>& rhoHandle_;
};


#endif
