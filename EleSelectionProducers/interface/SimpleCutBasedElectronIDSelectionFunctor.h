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


#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

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

//#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "RecoEgamma/EgammaTools/interface/ConversionInfo.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

#include "EgammaAnalysis/ElectronTools/interface/EGammaCutBasedEleId.h"
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


class SimpleCutBasedElectronIDSelectionFunctor : public Selector<reco::GsfElectronRef>  {

 public: // interface  
  
  enum Version_t { NONE=0, fiducial, WP80PU, WP90PU, WP70PU, loose, medium, tight, loose25nsRun2, medium25nsRun2, tight25nsRun2, loose50nsRun2, medium50nsRun2, tight50nsRun2};
  
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


  // initialize it by using only the version name
 SimpleCutBasedElectronIDSelectionFunctor(Version_t  version, 
					  const edm::Handle<reco::ConversionCollection>& ConversionsHandle, 
					  const edm::Handle<reco::BeamSpot>& BeamSpotHandle, 
					  const edm::Handle<double>& rhoHandle,
					  //edm::Handle< edm::ValueMap<reco::IsoDeposit> >,
					  edm::Handle< edm::ValueMap<double> > &isoVals
					  ):
  ConversionsHandle_(ConversionsHandle),
    BeamSpotHandle_(BeamSpotHandle),
    rhoHandle_(rhoHandle),EgammaCutBasedEleId::IsoDepositMaps
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
#endif

 SimpleCutBasedElectronIDSelectionFunctor(TString versionStr, 
					  const edm::Handle<reco::GsfElectronCollection>& electronsHandle,
					  const edm::Handle<reco::ConversionCollection>& ConversionsHandle, 
					  const edm::Handle<reco::BeamSpot>& BeamSpotHandle, 
					  const edm::Handle<reco::VertexCollection>& VertexHandle,
					  const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle,
					  const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle,
					  const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle,
					  const edm::Handle<double>& rhoHandle):
  electronsHandle_(electronsHandle),
    ConversionsHandle_(ConversionsHandle),
    BeamSpotHandle_(BeamSpotHandle),
    VertexHandle_(VertexHandle),
    chIsoValsHandle_(chIsoValsHandle),
    emIsoValsHandle_(emIsoValsHandle),
    nhIsoValsHandle_(nhIsoValsHandle),
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
      else if (versionStr.CompareTo("loose")==0) version=loose;
      else if (versionStr.CompareTo("medium")==0) version=medium;
      else if (versionStr.CompareTo("tight")==0) version=tight;
      else if (versionStr.CompareTo("loose25nsRun2")==0) version=loose25nsRun2;
      else if (versionStr.CompareTo("medium25nsRun2")==0) version=medium25nsRun2;
      else if (versionStr.CompareTo("tight25nsRun2")==0) version=tight25nsRun2;
      else if (versionStr.CompareTo("loose50nsRun2")==0) version=loose50nsRun2;
      else if (versionStr.CompareTo("medium50nsRun2")==0) version=medium50nsRun2;
      else if (versionStr.CompareTo("tight50nsRun2")==0) version=tight50nsRun2;
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

    push_back("ooemoop_EB");      push_back("ooemoop_EE");    
    push_back("d0vtx_EB");        push_back("d0vtx_EE");      
    push_back("dzvtx_EB");        push_back("dzvtx_EE");      
    push_back("pfIso_EB");        push_back("pfIso_EE");      
    push_back("pfIsoLowPt_EB");   push_back("pfIsoLowPt_EE"); 

    //    push_back("conversionRejection"            );

    // set the cut values and active cuts for WP80 selection PU corrected with rho
    if (version_ == fiducial){
      set("fiducial");
    } else if (version_ == WP80PU) {
      //      std::cout << "[DEBUG] Iniziatializing WP80PU" << std::endl;
      
      set("maxNumberOfExpectedMissingHits", 0);
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
      set("maxNumberOfExpectedMissingHits", 0);
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
      set("maxNumberOfExpectedMissingHits", 0);
      set("hasMatchedConversion");
      
      set("hoe_EB",      0.12);         set("hoe_EE",      1.5e-01);
      set("deta_EB",     7.0e-03);      set("deta_EE",     9.0e-03);
      set("dphi_EB",     8.0e-01);      set("dphi_EE",     7.0e-01);
      set("sihih_EB",    1.0e-02);      set("sihih_EE",    3.0e-02);
      //set("pfmva_EB", 0.1);       //set("pfmva_EE", 0.1);
      set("relTrackIso_EB", 1.2e-01);   set("relTrackIso_EE", 5.0e-02);
      set("relEcalIso_EB",  9.0e-02);   set("relEcalIso_EE",  6.0e-02);
      set("relHcalIso_EB",  1.0e-01);   set("relHcalIso_EE",  3.0e-02);
    }
    else if (version_ == loose) {
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.120);         set("hoe_EE",          0.100);
      set("deta_EB",       0.007);         set("deta_EE",         0.009);
      set("dphi_EB",       0.150);         set("dphi_EE",         0.100);
      set("sihih_EB",      0.010);         set("sihih_EE",        0.030);
      set("ooemoop_EB",    0.050,false);   set("ooemoop_EE",      0.050,false);  
      set("d0vtx_EB",      0.020);         set("d0vtx_EE",        0.020);
      set("dzvtx_EB",      0.200);         set("dzvtx_EE",        0.200);
      set("pfIso_EB",      0.150);         set("pfIso_EE",        0.150);         
      set("pfIsoLowPt_EB", 0.150);         set("pfIsoLowPt_EE",   0.100);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == medium) {
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.120);         set("hoe_EE",          0.100);
      set("deta_EB",       0.004);         set("deta_EE",         0.007);
      set("dphi_EB",       0.060);         set("dphi_EE",         0.030);
      set("sihih_EB",      0.010);         set("sihih_EE",        0.030);
      set("ooemoop_EB",    0.050,false);   set("ooemoop_EE",      0.050,false);  
      set("d0vtx_EB",      0.020);         set("d0vtx_EE",        0.020);
      set("dzvtx_EB",      0.100);         set("dzvtx_EE",        0.100);
      set("pfIso_EB",      0.150);         set("pfIso_EE",        0.150);         
      set("pfIsoLowPt_EB", 0.150);         set("pfIsoLowPt_EE",   0.100);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == tight) {
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 0);
      set("hasMatchedConversion");
      set("hoe_EB",        0.120);         set("hoe_EE",          0.100);
      set("deta_EB",       0.004);         set("deta_EE",         0.005);
      set("dphi_EB",       0.030);         set("dphi_EE",         0.020);
      set("sihih_EB",      0.010);         set("sihih_EE",        0.030);
      set("ooemoop_EB",    0.050,false);   set("ooemoop_EE",      0.050,false);  
      set("d0vtx_EB",      0.020);         set("d0vtx_EE",        0.020);
      set("dzvtx_EB",      0.100);         set("dzvtx_EE",        0.100);
      set("pfIso_EB",      0.100);         set("pfIso_EE",        0.100);         
      set("pfIsoLowPt_EB", 0.100);         set("pfIsoLowPt_EE",   0.070);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == loose25nsRun2) {   //PHYS14. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.121476);         set("hoe_EE",          0.131862);
      set("deta_EB",       0.012442);         set("deta_EE",         0.010654);
      set("dphi_EB",       0.072624);         set("dphi_EE",         0.145129);
      set("sihih_EB",      0.010557);         set("sihih_EE",        0.032602);
      set("ooemoop_EB",    0.221803,false);   set("ooemoop_EE",      0.142283,false);  
      set("d0vtx_EB",      0.022664);         set("d0vtx_EE",        0.097358);
      set("dzvtx_EB",      0.173670);         set("dzvtx_EE",        0.198444);
      set("pfIso_EB",      0.120026);         set("pfIso_EE",        0.162914);         
      set("pfIsoLowPt_EB", 0.120026);         set("pfIsoLowPt_EE",   0.162914);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == medium25nsRun2) {  //PHYS14. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.060662);         set("hoe_EE",          0.104263);
      set("deta_EB",       0.007641);         set("deta_EE",         0.009285);
      set("dphi_EB",       0.032643);         set("dphi_EE",         0.042447);
      set("sihih_EB",      0.010399);         set("sihih_EE",        0.029524);
      set("ooemoop_EB",    0.153897,false);   set("ooemoop_EE",      0.137468,false);  
      set("d0vtx_EB",      0.011811 );         set("d0vtx_EE",        0.051682);
      set("dzvtx_EB",      0.070775);         set("dzvtx_EE",        0.180720);
      set("pfIso_EB",      0.097213);         set("pfIso_EE",        0.116708);         
      set("pfIsoLowPt_EB", 0.097213);         set("pfIsoLowPt_EE",   0.116708);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == tight25nsRun2) {  //PHYS14. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.037553);         set("hoe_EE",          0.081902);
      set("deta_EB",       0.006574);         set("deta_EE",         0.005681);
      set("dphi_EB",       0.022868);         set("dphi_EE",         0.032046);
      set("sihih_EB",      0.010181);         set("sihih_EE",        0.028766 );
      set("ooemoop_EB",    0.131191,false);   set("ooemoop_EE",      0.106055,false);  
      set("d0vtx_EB",      0.009924 );         set("d0vtx_EE",        0.027261);
      set("dzvtx_EB",      0.015310);         set("dzvtx_EE",        0.147154);
      set("pfIso_EB",      0.074355);         set("pfIso_EE",        0.090185);         
      set("pfIsoLowPt_EB", 0.074355);         set("pfIsoLowPt_EE",   0.090185);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == loose50nsRun2) { //CSA14. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.150);         set("hoe_EE",          0.120);
      set("deta_EB",       0.016);         set("deta_EE",         0.025);
      set("dphi_EB",       0.080);         set("dphi_EE",         0.097);
      set("sihih_EB",      0.012);         set("sihih_EE",        0.032);
      set("ooemoop_EB",    0.110,false);   set("ooemoop_EE",      0.110,false);  
      set("d0vtx_EB",      0.019);         set("d0vtx_EE",        0.099);
      set("dzvtx_EB",      0.036);         set("dzvtx_EE",        0.880);
      set("pfIso_EB",      0.180);         set("pfIso_EE",        0.210);         
      set("pfIsoLowPt_EB", 0.180);         set("pfIsoLowPt_EE",   0.210);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == medium50nsRun2) { //CSA14. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.100);         set("hoe_EE",          0.099);
      set("deta_EB",       0.015);         set("deta_EE",         0.023);
      set("dphi_EB",       0.051);         set("dphi_EE",         0.056);
      set("sihih_EB",      0.010);         set("sihih_EE",        0.030);
      set("ooemoop_EB",    0.053,false);   set("ooemoop_EE",      0.110,false);  
      set("d0vtx_EB",      0.012);         set("d0vtx_EE",        0.068);
      set("dzvtx_EB",      0.030);         set("dzvtx_EE",        0.780);
      set("pfIso_EB",      0.140);         set("pfIso_EE",        0.150);         
      set("pfIsoLowPt_EB", 0.140);         set("pfIsoLowPt_EE",   0.150);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
    }
    else if (version_ == tight50nsRun2) { //CSA14. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
      //set("fiducial");
      set("maxNumberOfExpectedMissingHits", 1);
      set("hasMatchedConversion");
      set("hoe_EB",        0.074);         set("hoe_EE",          0.080);
      set("deta_EB",       0.012);         set("deta_EE",         0.019);
      set("dphi_EB",       0.024);         set("dphi_EE",         0.043);
      set("sihih_EB",      0.010);         set("sihih_EE",        0.029);
      set("ooemoop_EB",    0.026,false);   set("ooemoop_EE",      0.076,false);  
      set("d0vtx_EB",      0.0091);         set("d0vtx_EE",        0.037);
      set("dzvtx_EB",      0.017);         set("dzvtx_EE",        0.065);
      set("pfIso_EB",      0.100);         set("pfIso_EE",        0.140);         
      set("pfIsoLowPt_EB", 0.100);         set("pfIsoLowPt_EE",   0.140);         
      set("relTrackIso_EB", 0.2,false);    set("relTrackIso_EE", 0.2,false);  
      set("relEcalIso_EB",  0.2,false);	   set("relEcalIso_EE",  0.2,false);
      set("relHcalIso_EB",  0.2,false);	   set("relHcalIso_EE",  0.2,false);
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
    push_back("trackIso_EB");  push_back("trackIso_EE");
    push_back("ecalIso_EB" );  push_back("ecalIso_EE" );
    push_back("hcalIso_EB" );  push_back("hcalIso_EE" );
    push_back("sihih_EB"   );  push_back("sihih_EE"   );
    push_back("dphi_EB"    );  push_back("dphi_EE"    );
    push_back("deta_EB"    );  push_back("deta_EE"    );
    push_back("hoe_EB"     );  push_back("hoe_EE"     );
    
    push_back("conversionRejection"            );
    push_back("maxNumberOfExpectedMissingHits" );
    
   
    set("trackIso_EB", trackIso_EB);    set("trackIso_EE", trackIso_EE);  
    set("ecalIso_EB",  ecalIso_EB);     set("ecalIso_EE",  ecalIso_EE);   
    set("hcalIso_EB",  hcalIso_EB);     set("hcalIso_EE",  hcalIso_EE);   
    set("sihih_EB",    sihih_EB);       set("sihih_EE",    sihih_EE);     
    set("dphi_EB",     dphi_EB);        set("dphi_EE",     dphi_EE);      
    set("deta_EB",     deta_EB);        set("deta_EE",     deta_EE);      
    set("hoe_EB",      hoe_EB);	        set("hoe_EE",      hoe_EE);	      
    set("cIso_EB",     cIso_EB);        set("cIso_EE",     cIso_EE);      
        
    set("conversionRejection",            conversionRejection);
    set("maxNumberOfExpectedMissingHits", maxNumberOfExpectedMissingHits);
    
  }
#endif

  bool operator()( const reco::GsfElectronRef& electron, pat::strbitset& ret)
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

  using Selector<reco::GsfElectronRef>::operator();
  // function with the Spring10 variable definitions
  bool WPxx_PU( const reco::GsfElectronRef electronRef, pat::strbitset& ret)
  {
    const reco::GsfElectron electron = *electronRef;

    //    ret.set(false);
    // effective areas
    float AeffTk_EB = 0;
    float AeffECAL_EB = 0.096;
    float AeffHCAL_EB = 0.020;
    float AeffTk_EE = 0;
    float AeffECAL_EE = 0.044;
    float AeffHCAL_EE = 0.041;

#ifdef CMSSW_7_2_X
    Double_t eleET = electron.et();
#else
    //    Double_t eleET = electron.p4().Pt();
    Double_t eleET = electron.et();
    Double_t etaSC = electron.superCluster()->eta();
    // effective area for isolation

    float AEff = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, 
								 etaSC, ElectronEffectiveArea::kEleEAData2011);
#endif

    float ooemoop     = (1.0/electron.ecalEnergy() - electron.eSuperClusterOverP()/electron.ecalEnergy());
    //    Double_t etSCEle = electron.superCluster()->energy() *sin(electron.superCluster()->position().theta());
    Double_t trackIso = electron.dr03TkSumPt()/eleET;
    Double_t ecalIso  = electron.dr03EcalRecHitSumEt()/eleET;
    Double_t hcalIso  = electron.dr03HcalTowerSumEt()/eleET;
    Double_t sihih    = electron.sigmaIetaIeta();
    Double_t Dphi     = electron.deltaPhiSuperClusterTrackAtVtx();
    Double_t Deta     = electron.deltaEtaSuperClusterTrackAtVtx();
    Double_t HoE      = electron.hadronicOverEm();

#ifdef CMSSW_7_2_X
    Double_t pfMVA    = electron.mva_e_pi();
#else
    Double_t pfMVA    = electron.mva();
#endif

    // get the variables
    Double_t pt         = electron.pt();
    
    // impact parameter variables
    float d0vtx         = 0.0;
    float dzvtx         = 0.0;
    if (VertexHandle_->size() > 0) {
      reco::VertexRef vtx(VertexHandle_, 0);    
      d0vtx = electron.gsfTrack()->dxy(vtx->position());
      dzvtx = electron.gsfTrack()->dz(vtx->position());
    } else {
      d0vtx = electron.gsfTrack()->dxy();
      dzvtx = electron.gsfTrack()->dz();
    }

#ifdef CMSSW_7_2_X
    double iso_ch = electron.pfIsolationVariables().sumChargedHadronPt;
    double iso_em = electron.pfIsolationVariables().sumPhotonEt;
    double iso_nh = electron.pfIsolationVariables().sumNeutralHadronEt;
    double PU_ch  = electron.pfIsolationVariables().sumPUPt;

    // apply to neutrals
    double iso_n = std::max(iso_nh + iso_em - PU_ch * 0.5, 0.0); //relIsoWithDBeta_:look inside EgammaAnalysis/ElectronTools/test/ElectronIDValidationAnalyzer.cc
    // compute final isolation
    double iso = (iso_n + iso_ch) / pt;
#else
    // get particle flow isolation
    double iso_ch = (*chIsoValsHandle_)[electronRef];
    double iso_em = (*emIsoValsHandle_)[electronRef];
    double iso_nh = (*nhIsoValsHandle_)[electronRef];

    // apply to neutrals
    double rhoPrime = std::max(*rhoHandle_, 0.0);
    double iso_n = std::max(iso_nh + iso_em - rhoPrime * AEff, 0.0);
    // compute final isolation
    double iso = (iso_n + iso_ch) / pt;
#endif

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
    // in 39 conversion rejection variables are accessible from Gsf electron
#ifdef shervin
    Double_t dist = electron.convDist(); // default value is -9999 if conversion partner not found
    Double_t dcot = electron.convDcot(); // default value is -9999 if conversion partner not found
    Bool_t isConv = fabs(dist) < 0.02 && fabs(dcot) < 0.02;
#endif

#ifdef CMSSW_7_2_X
    Int_t innerHits = electron.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS);
#else
    Int_t innerHits = electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
#endif

    bool hasMatchedConversion = ConversionTools::hasMatchedConversion(electron, ConversionsHandle_, BeamSpotHandle_->position());

    Double_t absEtaSC = fabs(electron.superCluster()->eta());
    Double_t rhoRel = *rhoHandle_ / eleET;
    // conversion rejection variables




    //------------------------------ Fiducial region cut
    if ( (absEtaSC < 1.4442 || (absEtaSC > 1.566 && absEtaSC < 2.5) ) || ignoreCut("fiducial")) passCut(retInternal_, "fiducial");

    //------------------------------ conversion rejection cut
    if ( innerHits  <= cut("maxNumberOfExpectedMissingHits", int()) || ignoreCut("maxNumberOfExpectedMissingHits")) 
      passCut(retInternal_, "maxNumberOfExpectedMissingHits");    
    if ( (!hasMatchedConversion) || ignoreCut("hasMatchedConversion")) passCut(retInternal_, "hasMatchedConversion");

    if (electron.isEB()) { // BARREL case
      //#ifdef DEBUG
      //std::cout << version_ << "\t" << fabs(Deta) << "\t" << cut("deta_EB", double()) << "\t" << ignoreCut("deta_EB") << std::endl;
      //#endif
      if ( fabs(Deta)  <  cut("deta_EB",     double()) || ignoreCut("deta_EB")    ) passCut(retInternal_, "deta_EB");
      if ( fabs(Dphi)  <  cut("dphi_EB",     double()) || ignoreCut("dphi_EB")    ) passCut(retInternal_, "dphi_EB");
      if ( sihih       <  cut("sihih_EB",    double()) || ignoreCut("sihih_EB")   ) passCut(retInternal_, "sihih_EB");
      if ( HoE         <  cut("hoe_EB",      double()) || ignoreCut("hoe_EB")     ) passCut(retInternal_, "hoe_EB");
      if ( ignoreCut("ooemoop_EB") || ooemoop     <  cut("ooemoop_EB",     double())  ) passCut(retInternal_, "ooemoop_EB");
      if ( fabs(d0vtx) <  cut("d0vtx_EB",       double()) || ignoreCut("d0vtx_EB")   ) passCut(retInternal_, "d0vtx_EB");
      if ( fabs(dzvtx) <  cut("dzvtx_EB",       double()) || ignoreCut("dzvtx_EB")   ) passCut(retInternal_, "dzvtx_EB");

      if ( pfMVA       >  cut("pfmva_EB",    double()) || ignoreCut("pfmva_EB")   ) passCut(retInternal_, "pfmva_EB");

      if ( trackIso - AeffTk_EB   *rhoRel <  cut("relTrackIso_EB", double()) || ignoreCut("relTrackIso_EB")) 
	passCut(retInternal_, "relTrackIso_EB");
      if ( ecalIso  - AeffECAL_EB *rhoRel <  cut("relEcalIso_EB",  double()) || ignoreCut("relEcalIso_EB") ) 
	passCut(retInternal_, "relEcalIso_EB");
      if ( hcalIso  - AeffHCAL_EB *rhoRel <  cut("relHcalIso_EB",  double()) || ignoreCut("relHcalIso_EB") ) 
	passCut(retInternal_, "relHcalIso_EB");
      if(pt>=20){
	if( ignoreCut("pfIso_EB") || iso < cut("pfIso_EB", double()))
	  passCut(retInternal_, "pfIso_EB");
	passCut(retInternal_, "pfIsoLowPt_EB");
      }else{
	if(iso < cut("pfIsoLowPt_EB", double()) || ignoreCut("pfIsoLowPt_EB") )
	  passCut(retInternal_, "pfIsoLowPt_EB");
	passCut(retInternal_, "pfIso_EB");
      }

      // pass all the EE cuts
      passCut(retInternal_, "deta_EE");	
      passCut(retInternal_, "dphi_EE");	
      passCut(retInternal_, "sihih_EE");	
      passCut(retInternal_, "hoe_EE");
      passCut(retInternal_, "ooemoop_EE");
      passCut(retInternal_, "d0vtx_EE");
      passCut(retInternal_, "dzvtx_EE");
      passCut(retInternal_, "pfmva_EE");
      passCut(retInternal_, "relTrackIso_EE");	
      passCut(retInternal_, "relEcalIso_EE");	
      passCut(retInternal_, "relHcalIso_EE");	
      passCut(retInternal_, "pfIso_EE");
      passCut(retInternal_, "pfIsoLowPt_EE");
    } else {  // ENDCAPS case
      //      std::cout << version_ << "\t" << fabs(Deta) << "\t" << cut("deta_EE", double()) << "\t" << ignoreCut("deta_EE") << std::endl;
      if ( fabs(Deta)  <  cut("deta_EE",     double()) || ignoreCut("deta_EE")    ) passCut(retInternal_, "deta_EE");
      if ( fabs(Dphi)  <  cut("dphi_EE",     double()) || ignoreCut("dphi_EE")    ) passCut(retInternal_, "dphi_EE");
      if ( sihih       <  cut("sihih_EE",    double()) || ignoreCut("sihih_EE")   ) passCut(retInternal_, "sihih_EE");
      if ( HoE         <  cut("hoe_EE",      double()) || ignoreCut("hoe_EE")     ) passCut(retInternal_, "hoe_EE");
      if ( ooemoop     <  cut("ooemoop_EE",     double()) || ignoreCut("ooemoop_EE") ) passCut(retInternal_, "ooemoop_EE");
      if ( fabs(d0vtx) <  cut("d0vtx_EE",       double()) || ignoreCut("d0vtx_EE")   ) passCut(retInternal_, "d0vtx_EE");
      if ( fabs(dzvtx) <  cut("dzvtx_EE",       double()) || ignoreCut("dzvtx_EE")   ) passCut(retInternal_, "dzvtx_EE");

      if ( pfMVA       >  cut("pfmva_EE",    double()) || ignoreCut("pfmva_EE")   ) passCut(retInternal_, "pfmva_EE");

      if ( trackIso - AeffTk_EE   *rhoRel <  cut("relTrackIso_EE", double()) || ignoreCut("relTrackIso_EE")) 
	passCut(retInternal_, "relTrackIso_EE");
      if ( ecalIso  - AeffECAL_EE *rhoRel <  cut("relEcalIso_EE",  double()) || ignoreCut("relEcalIso_EE") ) 
	passCut(retInternal_, "relEcalIso_EE");
      if ( hcalIso  - AeffHCAL_EE *rhoRel <  cut("relHcalIso_EE",  double()) || ignoreCut("relHcalIso_EE") ) 
	passCut(retInternal_, "relHcalIso_EE");
      if(pt>=20){
	if(iso < cut("pfIso_EE", double()) || ignoreCut("pfIso_EE") )
	  passCut(retInternal_, "pfIso_EE");
	passCut(retInternal_, "pfIsoLowPt_EE");
      }else{
	if(iso < cut("pfIsoLowPt_EE", double()) || ignoreCut("pfIsoLowPt_EE") )
	  passCut(retInternal_, "pfIsoLowPt_EE");
	passCut(retInternal_, "pfIso_EE");
      }

      // pass all the EB cuts
      passCut(retInternal_, "deta_EB");	
      passCut(retInternal_, "dphi_EB");	
      passCut(retInternal_, "sihih_EB");	
      passCut(retInternal_, "hoe_EB");
      passCut(retInternal_, "ooemoop_EB");
      passCut(retInternal_, "d0vtx_EB");
      passCut(retInternal_, "dzvtx_EB");
      passCut(retInternal_, "pfmva_EB");
      passCut(retInternal_, "relTrackIso_EB");	
      passCut(retInternal_, "relEcalIso_EB");	
      passCut(retInternal_, "relHcalIso_EB");	
      passCut(retInternal_, "pfIso_EB");
      passCut(retInternal_, "pfIsoLowPt_EB");

    }
    setIgnored(retInternal_);   
#ifdef DEBUG
    std::cout << "------------------------------ " << version_ << std::endl;
    std::cout << "[DEBUG] retInternal_ = " << retInternal_ << std::endl;
    std::cout << "[DEBUG] ret = " << ret << std::endl;
    ret=retInternal_;
    std::cout << "[DEBUG] copy ret = " << (bool) ret << std::endl;
    std::cout << "[DEBUG] bitMask = " << bitMask() << std::endl;
    if(((bool)ret)==0){
      print(std::cout);
    }      
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
  const edm::Handle<reco::GsfElectronCollection>& electronsHandle_;
  const edm::Handle<reco::ConversionCollection>& ConversionsHandle_;
  const edm::Handle<reco::BeamSpot>& BeamSpotHandle_;
  const edm::Handle<reco::VertexCollection>& VertexHandle_;
  const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle_;
  const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle_;
  const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle_;
  const edm::Handle<double>& rhoHandle_;
};


#endif
