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
#include "DataFormats/PatCandidates/interface/Electron.h"

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

#define MINIAOD
#ifndef MINIAOD
typedef reco::GsfElectron electron_t;
typedef reco::GsfElectronRef electronRef_t;
typedef reco::GsfElectronCollection electronCollection_t;
#else
typedef pat::Electron electron_t;
typedef pat::ElectronRef electronRef_t;
typedef pat::ElectronCollection electronCollection_t;
#endif

class SimpleCutBasedElectronIDSelectionFunctor : public Selector<electronRef_t>
{


public: // interface


	enum Version_t { NONE = 0, fiducial, WP80PU, WP90PU, WP70PU, loose, medium, tight, loose25nsRun2, medium25nsRun2, tight25nsRun2, loose50nsRun2, medium50nsRun2, tight50nsRun2, medium25nsRun2Boff, looseElectronStream, mediumElectronStream, tightElectronStream};

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
		            parameters.getParameter<Int_t>("maxNumberOfExpectedMissingHits_EB"));
		parameters.getParameter<Int_t>("maxNumberOfExpectedMissingHits_EE"));
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
		rhoHandle_(rhoHandle), EgammaCutBasedEleId::IsoDepositMaps {
		if (version == NONE)
		{
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
	        const edm::Handle<electronCollection_t>& electronsHandle,
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
		Version_t version = NONE;
		if (versionStr.CompareTo("NONE") == 0) {
			std::cout << "SimpleCutBasedElectronIDSelectionFunctor: If you want to use version NONE "
			          << "then you have also to provide the selection cuts by yourself " << std::endl;
			std::cout << "SimpleCutBasedElectronIDSelectionFunctor: ID Version is changed to 80cIso "
			          << std::endl;
			version = WP80PU;
		} else if (versionStr.CompareTo("fiducial") == 0) version = fiducial;
		else if (versionStr.CompareTo("WP70PU") == 0) version = WP70PU;
		else if (versionStr.CompareTo("WP80PU") == 0) version = WP80PU;
		else if (versionStr.CompareTo("WP90PU") == 0) version = WP90PU;
		else if (versionStr.CompareTo("loose") == 0) version = loose;
		else if (versionStr.CompareTo("medium") == 0) version = medium;
		else if (versionStr.CompareTo("tight") == 0) version = tight;
		else if (versionStr.CompareTo("loose25nsRun2") == 0) version = loose25nsRun2;
		else if (versionStr.CompareTo("medium25nsRun2") == 0) version = medium25nsRun2;
		else if (versionStr.CompareTo("tight25nsRun2") == 0) version = tight25nsRun2;
		else if (versionStr.CompareTo("loose50nsRun2") == 0) version = loose50nsRun2;
		else if (versionStr.CompareTo("medium50nsRun2") == 0) version = medium50nsRun2;
		else if (versionStr.CompareTo("tight50nsRun2") == 0) version = tight50nsRun2;
		else if (versionStr.CompareTo("medium25nsRun2Boff") == 0) version = medium25nsRun2Boff;
		else if (versionStr.CompareTo("looseElectronStream") == 0) version = loose25nsRun2;
		else if (versionStr.CompareTo("mediumElectronStream") == 0) version = medium25nsRun2;
		else if (versionStr.CompareTo("tightElectronStream") == 0) version = tight25nsRun2;
		else {
			std::cerr << "[ERROR] version type not defined" << std::endl;
			std::cerr << "[ERROR] using WP80PU" << std::endl;
			version = WP80PU;
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
		push_back("maxNumberOfExpectedMissingHits_EB" );
		push_back("maxNumberOfExpectedMissingHits_EE" );
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

		push_back("ooemoop_EB");
		push_back("ooemoop_EE");
		push_back("d0vtx_EB");
		push_back("d0vtx_EE");
		push_back("dzvtx_EB");
		push_back("dzvtx_EE");
		push_back("pfIso_EB");
		push_back("pfIso_EE");
		push_back("pfIsoLowPt_EB");
		push_back("pfIsoLowPt_EE");

		//    push_back("conversionRejection"            );

		// set the cut values and active cuts for WP80 selection PU corrected with rho
		if (version_ == fiducial) {
			set("fiducial");
		} else if (version_ == WP80PU) {
			//      std::cout << "[DEBUG] Iniziatializing WP80PU" << std::endl;

			set("maxNumberOfExpectedMissingHits_EB", 0);
			set("maxNumberOfExpectedMissingHits_EE", 0);
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
			set("maxNumberOfExpectedMissingHits_EB", 0);
			set("maxNumberOfExpectedMissingHits_EE", 0);
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
		} else if (version_ == WP70PU) {
			//      set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 0);
			set("maxNumberOfExpectedMissingHits_EE", 0);
			set("hasMatchedConversion");

			set("hoe_EB",      0.12);
			set("hoe_EE",      1.5e-01);
			set("deta_EB",     7.0e-03);
			set("deta_EE",     9.0e-03);
			set("dphi_EB",     8.0e-01);
			set("dphi_EE",     7.0e-01);
			set("sihih_EB",    1.0e-02);
			set("sihih_EE",    3.0e-02);
			//set("pfmva_EB", 0.1);       //set("pfmva_EE", 0.1);
			set("relTrackIso_EB", 1.2e-01);
			set("relTrackIso_EE", 5.0e-02);
			set("relEcalIso_EB",  9.0e-02);
			set("relEcalIso_EE",  6.0e-02);
			set("relHcalIso_EB",  1.0e-01);
			set("relHcalIso_EE",  3.0e-02);
		} else if (version_ == loose) {
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 1);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.120);
			set("hoe_EE",          0.100);
			set("deta_EB",       0.007);
			set("deta_EE",         0.009);
			set("dphi_EB",       0.150);
			set("dphi_EE",         0.100);
			set("sihih_EB",      0.010);
			set("sihih_EE",        0.030);
			set("ooemoop_EB",    0.050, false);
			set("ooemoop_EE",      0.050, false);
			set("d0vtx_EB",      0.020);
			set("d0vtx_EE",        0.020);
			set("dzvtx_EB",      0.200);
			set("dzvtx_EE",        0.200);
			set("pfIso_EB",      0.150);
			set("pfIso_EE",        0.150);
			set("pfIsoLowPt_EB", 0.150);
			set("pfIsoLowPt_EE",   0.100);
			set("relTrackIso_EB", 0.2, false);
			set("relTrackIso_EE", 0.2, false);
			set("relEcalIso_EB",  0.2, false);
			set("relEcalIso_EE",  0.2, false);
			set("relHcalIso_EB",  0.2, false);
			set("relHcalIso_EE",  0.2, false);
		} else if (version_ == medium) {
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 1);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.120);
			set("hoe_EE",          0.100);
			set("deta_EB",       0.004);
			set("deta_EE",         0.007);
			set("dphi_EB",       0.060);
			set("dphi_EE",         0.030);
			set("sihih_EB",      0.010);
			set("sihih_EE",        0.030);
			set("ooemoop_EB",    0.050, false);
			set("ooemoop_EE",      0.050, false);
			set("d0vtx_EB",      0.020);
			set("d0vtx_EE",        0.020);
			set("dzvtx_EB",      0.100);
			set("dzvtx_EE",        0.100);
			set("pfIso_EB",      0.150);
			set("pfIso_EE",        0.150);
			set("pfIsoLowPt_EB", 0.150);
			set("pfIsoLowPt_EE",   0.100);
			set("relTrackIso_EB", 0.2, false);
			set("relTrackIso_EE", 0.2, false);
			set("relEcalIso_EB",  0.2, false);
			set("relEcalIso_EE",  0.2, false);
			set("relHcalIso_EB",  0.2, false);
			set("relHcalIso_EE",  0.2, false);
		} else if (version_ == tight) {
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 0);
			set("maxNumberOfExpectedMissingHits_EE", 0);
			set("hasMatchedConversion");
			set("hoe_EB",        0.120);
			set("hoe_EE",          0.100);
			set("deta_EB",       0.004);
			set("deta_EE",         0.005);
			set("dphi_EB",       0.030);
			set("dphi_EE",         0.020);
			set("sihih_EB",      0.010);
			set("sihih_EE",        0.030);
			set("ooemoop_EB",    0.050, false);
			set("ooemoop_EE",      0.050, false);
			set("d0vtx_EB",      0.020);
			set("d0vtx_EE",        0.020);
			set("dzvtx_EB",      0.100);
			set("dzvtx_EE",        0.100);
			set("pfIso_EB",      0.100);
			set("pfIso_EE",        0.100);
			set("pfIsoLowPt_EB", 0.100);
			set("pfIsoLowPt_EE",   0.070);
			set("relTrackIso_EB", 0.2, false);
			set("relTrackIso_EE", 0.2, false);
			set("relEcalIso_EB",  0.2, false);
			set("relEcalIso_EE",  0.2, false);
			set("relHcalIso_EB",  0.2, false);
			set("relHcalIso_EE",  0.2, false);
		} else if (version_ == loose25nsRun2) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.104);
			set("hoe_EE",          0.0897);
			set("deta_EB",       0.0105);
			set("deta_EE",         0.00814);
			set("dphi_EB",       0.115);
			set("dphi_EE",         0.182);
			set("sihih_EB",      0.0103);
			set("sihih_EE",        0.0301);
			//      set("ooemoop_EB",    0.102,false);   set("ooemoop_EE",      0.0897,false);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      0.0261);
			set("d0vtx_EE",        0.118);
			set("dzvtx_EB",      0.41);
			set("dzvtx_EE",        0.822);
			set("pfIso_EB",      0.0893);
			set("pfIso_EE",        0.121);
			set("pfIsoLowPt_EB", 0.0893);
			set("pfIsoLowPt_EE",   0.121);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == medium25nsRun2 || version_ == medium25nsRun2Boff) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0876);
			set("hoe_EE",          0.0678);
			set("deta_EB",       0.0103);
			set("deta_EE",         0.00733);
			set("dphi_EB",       0.0336);
			set("dphi_EE",         0.114);
			set("sihih_EB",      0.0101);
			set("sihih_EE",        0.0283);
			//      set("ooemoop_EB",    0.0174,false);  set("ooemoop_EE",      0.0898,false);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      0.0118 );
			set("d0vtx_EE",        0.0739);
			set("dzvtx_EB",      0.373);
			set("dzvtx_EE",        0.602);
			set("pfIso_EB",      0.0766);
			set("pfIso_EE",        0.0678);
			set("pfIsoLowPt_EB", 0.0766);
			set("pfIsoLowPt_EE",   0.0678);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);

			if(version_ == medium25nsRun2Boff) {
				set("pfIso_EB",      100, false);
				set("pfIso_EE",      100, false);
				set("pfIsoLowPt_EB", 100, false);
				set("pfIsoLowPt_EE", 100, false);
			}

		} else if (version_ == tight25nsRun2) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0597);
			set("hoe_EE",          0.0615);
			set("deta_EB",       0.00926);
			set("deta_EE",         0.00724);
			set("dphi_EB",       0.0336);
			set("dphi_EE",         0.0918);
			set("sihih_EB",      0.0101);
			set("sihih_EE",        0.0279 );
			//      set("ooemoop_EB",    0.012,false);   set("ooemoop_EE",      0.00999,false);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      0.0111 );
			set("d0vtx_EE",        0.0351);
			set("dzvtx_EB",      0.0466);
			set("dzvtx_EE",        0.417);
			set("pfIso_EB",      0.0354);
			set("pfIso_EE",        0.0646);
			set("pfIsoLowPt_EB", 0.0354);
			set("pfIsoLowPt_EE",   0.0646);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == loose50nsRun2) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0765);
			set("hoe_EE",          0.0824);
			set("deta_EB",       0.00976);
			set("deta_EE",         0.00952);
			set("dphi_EB",       0.0929);
			set("dphi_EE",         0.181);
			set("sihih_EB",      0.0105);
			set("sihih_EE",        0.0318);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			//      set("ooemoop_EB",    0.184,false);   set("ooemoop_EE",      0.125,false);
			set("d0vtx_EB",      0.0227);
			set("d0vtx_EE",        0.242);
			set("dzvtx_EB",      0.379);
			set("dzvtx_EE",        0.921);
			set("pfIso_EB",      0.118);
			set("pfIso_EE",        0.118);
			set("pfIsoLowPt_EB", 0.118);
			set("pfIsoLowPt_EE",   0.118);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == medium50nsRun2) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0372);
			set("hoe_EE",          0.0546);
			set("deta_EB",       0.0094);
			set("deta_EE",         0.00773);
			set("dphi_EB",       0.0296);
			set("dphi_EE",         0.148);
			set("sihih_EB",      0.0101);
			set("sihih_EE",        0.0287);
			//      set("ooemoop_EB",    0.118,false);   set("ooemoop_EE",      0.104,false);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      0.0151);
			set("d0vtx_EE",        0.0535);
			set("dzvtx_EB",      0.238);
			set("dzvtx_EE",        0.572);
			set("pfIso_EB",      0.0987);
			set("pfIso_EE",        0.0902);
			set("pfIsoLowPt_EB", 0.0987);
			set("pfIsoLowPt_EE",   0.0902);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == tight50nsRun2) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0342);
			set("hoe_EE",          0.0544);
			set("deta_EB",       0.00864);
			set("deta_EE",         0.00762);
			set("dphi_EB",       0.0286);
			set("dphi_EE",         0.0439);
			set("sihih_EB",      0.0101);
			set("sihih_EE",        0.0287);
			//      set("ooemoop_EB",    0.0116,false);  set("ooemoop_EE",      0.0544,false);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      0.0103);
			set("d0vtx_EE",        0.0377);
			set("dzvtx_EB",      0.170);
			set("dzvtx_EE",        0.571);
			set("pfIso_EB",      0.0591);
			set("pfIso_EE",        0.0759);
			set("pfIsoLowPt_EB", 0.0591);
			set("pfIsoLowPt_EE",   0.0759);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == looseElectronStream) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.104);
			set("hoe_EE",          0.0897);
			set("deta_EB",       0.0105);
			set("deta_EE",         0.00814);
			set("dphi_EB",       0.115);
			set("dphi_EE",         0.182);
			set("sihih_EB",      0.0103);
			set("sihih_EE",        0.0301);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      100., false); //0.0261);
			set("d0vtx_EE",        100., false); //0.118);
			set("dzvtx_EB",      100., false); //0.41);
			set("dzvtx_EE",        100., false); //0.822);
			set("pfIso_EB",      0.0893);
			set("pfIso_EE",        0.121);
			set("pfIsoLowPt_EB", 0.0893);
			set("pfIsoLowPt_EE",   0.121);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == mediumElectronStream) { //SPRING15. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0876);
			set("hoe_EE",          0.0678);
			set("deta_EB",       0.0103);
			set("deta_EE",         0.00733);
			set("dphi_EB",       0.0336);
			set("dphi_EE",         0.114);
			set("sihih_EB",      0.0101);
			set("sihih_EE",        0.0283);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      100., false); //0.0118 );
			set("d0vtx_EE",        100., false); //0.0739);
			set("dzvtx_EB",      100., false); //0.373);
			set("dzvtx_EE",        100., false); //0.602);
			set("pfIso_EB",      0.0766);
			set("pfIso_EE",        0.0678);
			set("pfIsoLowPt_EB", 0.0766);
			set("pfIsoLowPt_EE",   0.0678);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
		} else if (version_ == tightElectronStream) {
			//set("fiducial");
			set("maxNumberOfExpectedMissingHits_EB", 2);
			set("maxNumberOfExpectedMissingHits_EE", 1);
			set("hasMatchedConversion");
			set("hoe_EB",        0.0597);
			set("hoe_EE",          0.0615);
			set("deta_EB",       0.00926);
			set("deta_EE",         0.00724);
			set("dphi_EB",       0.0336);
			set("dphi_EE",         0.0918);
			set("sihih_EB",      0.0101);
			set("sihih_EE",        0.0279);
			set("ooemoop_EB",    100., false);
			set("ooemoop_EE",      100., false);
			set("d0vtx_EB",      100., false);
			set("d0vtx_EE",        100., false);
			set("dzvtx_EB",      100., false);
			set("dzvtx_EE",        100., false);
			set("pfIso_EB",      0.0354);
			set("pfIso_EE",        0.0646);
			set("pfIsoLowPt_EB", 0.0354);
			set("pfIsoLowPt_EE",   0.0646);
			set("relTrackIso_EB", 100., false);
			set("relTrackIso_EE", 100., false);
			set("relEcalIso_EB",  100., false);
			set("relEcalIso_EE",  100., false);
			set("relHcalIso_EB",  100., false);
			set("relHcalIso_EE",  100., false);
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
	                Int_t maxNumberOfExpectedMissingHits_EB, Int_t maxNumberOfExpectedMissingHits_EE)
	{
		version_ = NONE;
		push_back("trackIso_EB");
		push_back("trackIso_EE");
		push_back("ecalIso_EB" );
		push_back("ecalIso_EE" );
		push_back("hcalIso_EB" );
		push_back("hcalIso_EE" );
		push_back("sihih_EB"   );
		push_back("sihih_EE"   );
		push_back("dphi_EB"    );
		push_back("dphi_EE"    );
		push_back("deta_EB"    );
		push_back("deta_EE"    );
		push_back("hoe_EB"     );
		push_back("hoe_EE"     );

		push_back("conversionRejection"            );
		push_back("maxNumberOfExpectedMissingHits_EB" );
		push_back("maxNumberOfExpectedMissingHits_EE" );


		set("trackIso_EB", trackIso_EB);
		set("trackIso_EE", trackIso_EE);
		set("ecalIso_EB",  ecalIso_EB);
		set("ecalIso_EE",  ecalIso_EE);
		set("hcalIso_EB",  hcalIso_EB);
		set("hcalIso_EE",  hcalIso_EE);
		set("sihih_EB",    sihih_EB);
		set("sihih_EE",    sihih_EE);
		set("dphi_EB",     dphi_EB);
		set("dphi_EE",     dphi_EE);
		set("deta_EB",     deta_EB);
		set("deta_EE",     deta_EE);
		set("hoe_EB",      hoe_EB);
		set("hoe_EE",      hoe_EE);
		set("cIso_EB",     cIso_EB);
		set("cIso_EE",     cIso_EE);

		set("conversionRejection",            conversionRejection);
		set("maxNumberOfExpectedMissingHits_EB", maxNumberOfExpectedMissingHits_EB);
		set("maxNumberOfExpectedMissingHits_EE", maxNumberOfExpectedMissingHits_EE);

	}
#endif

	bool operator()( const electronRef_t& electron, pat::strbitset& ret)
	{
		// new electron, clear old electron bitmask
		retInternal_ = getBitTemplate();
		// for the time being only WPxx_PU variable definition
		return WPxx_PU(electron, ret );
	}
	float result()
	{
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

	using Selector<electronRef_t>::operator();
	// function with the Spring10 variable definitions
	bool WPxx_PU( const electronRef_t electronRef, pat::strbitset& ret)
	{
		const auto electron = *electronRef;

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

		float ooemoop     = (1.0 / electron.ecalEnergy() - electron.eSuperClusterOverP() / electron.ecalEnergy());
		//    Double_t etSCEle = electron.superCluster()->energy() *sin(electron.superCluster()->position().theta());
		Double_t trackIso = electron.dr03TkSumPt() / eleET;
		Double_t ecalIso  = electron.dr03EcalRecHitSumEt() / eleET;
		Double_t hcalIso  = electron.dr03HcalTowerSumEt() / eleET;
		Double_t sihih    = electron.full5x5_sigmaIetaIeta(); //need to check if this one works
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
		Double_t etaSC = electron.superCluster()->eta();
		float AEff;

//25ns effective areas. see https://indico.cern.ch/event/369239/contribution/4/attachments/1134761/1623262/talk_effective_areas_25ns.pdf
		if      (fabs(etaSC) < 1.0)                          AEff = 0.1752;
		else if (fabs(etaSC) >= 1.0   && fabs(etaSC) < 1.479)  AEff = 0.1862;
		else if (fabs(etaSC) >= 1.479 && fabs(etaSC) < 2.0  )  AEff = 0.1411;
		else if (fabs(etaSC) >= 2.0   && fabs(etaSC) < 2.2  )  AEff = 0.1534;
		else if (fabs(etaSC) >= 2.2   && fabs(etaSC) < 2.3  )  AEff = 0.1903;
		else if (fabs(etaSC) >= 2.3   && fabs(etaSC) < 2.4  )  AEff = 0.2243;
		else                                               AEff = 0.2687;

		double iso_ch = electron.pfIsolationVariables().sumChargedHadronPt;
		double iso_em = electron.pfIsolationVariables().sumPhotonEt;
		double iso_nh = electron.pfIsolationVariables().sumNeutralHadronEt;
		//    double PU_ch  = electron.pfIsolationVariables().sumPUPt;

		// apply to neutrals
		double rhoPrime = std::max(*rhoHandle_, 0.0);
		//    double iso_n = std::max(iso_nh + iso_em - PU_ch * 0.5, 0.0); //relIsoWithDBeta_:look inside EgammaAnalysis/ElectronTools/test/ElectronIDValidationAnalyzer.cc
		double iso_n = std::max(iso_nh + iso_em - rhoPrime * AEff, 0.0); //relIsoWithEA
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
		if (electron.isEB()) {
			cIso =
			    ( electron.dr03TkSumPt() + std::max(0., electron.dr03EcalRecHitSumEt() - 1.)
			      + electron.dr03HcalTowerSumEt() ) / eleET;
		} else {
			cIso = ( electron.dr03TkSumPt() + electron.dr03EcalRecHitSumEt() +
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
		Int_t innerHits = electron.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
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
		if ( (!hasMatchedConversion) || ignoreCut("hasMatchedConversion")) passCut(retInternal_, "hasMatchedConversion");

		if (electron.isEB()) { // BARREL case
			//#ifdef DEBUG
			//std::cout << version_ << "\t" << fabs(Deta) << "\t" << cut("deta_EB", double()) << "\t" << ignoreCut("deta_EB") << std::endl;
			//#endif
			if ( innerHits  <= cut("maxNumberOfExpectedMissingHits_EB", int()) || ignoreCut("maxNumberOfExpectedMissingHits_EB"))
				passCut(retInternal_, "maxNumberOfExpectedMissingHits_EB");
			if ( fabs(Deta)  <  cut("deta_EB",     double()) || ignoreCut("deta_EB")    ) passCut(retInternal_, "deta_EB");
			if ( fabs(Dphi)  <  cut("dphi_EB",     double()) || ignoreCut("dphi_EB")    ) passCut(retInternal_, "dphi_EB");
			if ( sihih       <  cut("sihih_EB",    double()) || ignoreCut("sihih_EB")   ) passCut(retInternal_, "sihih_EB");
			if ( HoE         <  cut("hoe_EB",      double()) || ignoreCut("hoe_EB")     ) passCut(retInternal_, "hoe_EB");
			if ( ignoreCut("ooemoop_EB") || ooemoop     <  cut("ooemoop_EB",     double())  ) passCut(retInternal_, "ooemoop_EB");
			if ( fabs(d0vtx) <  cut("d0vtx_EB",       double()) || ignoreCut("d0vtx_EB")   ) passCut(retInternal_, "d0vtx_EB");
			if ( fabs(dzvtx) <  cut("dzvtx_EB",       double()) || ignoreCut("dzvtx_EB")   ) passCut(retInternal_, "dzvtx_EB");

			if ( pfMVA       >  cut("pfmva_EB",    double()) || ignoreCut("pfmva_EB")   ) passCut(retInternal_, "pfmva_EB");

			if ( trackIso - AeffTk_EB   * rhoRel <  cut("relTrackIso_EB", double()) || ignoreCut("relTrackIso_EB"))
				passCut(retInternal_, "relTrackIso_EB");
			if ( ecalIso  - AeffECAL_EB * rhoRel <  cut("relEcalIso_EB",  double()) || ignoreCut("relEcalIso_EB") )
				passCut(retInternal_, "relEcalIso_EB");
			if ( hcalIso  - AeffHCAL_EB * rhoRel <  cut("relHcalIso_EB",  double()) || ignoreCut("relHcalIso_EB") )
				passCut(retInternal_, "relHcalIso_EB");
			if(pt >= 20) {
				if( ignoreCut("pfIso_EB") || iso < cut("pfIso_EB", double()))
					passCut(retInternal_, "pfIso_EB");
				passCut(retInternal_, "pfIsoLowPt_EB");
			} else {
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
			passCut(retInternal_, "maxNumberOfExpectedMissingHits_EE");
		} else {  // ENDCAPS case
			//      std::cout << version_ << "\t" << fabs(Deta) << "\t" << cut("deta_EE", double()) << "\t" << ignoreCut("deta_EE") << std::endl;
			if ( innerHits  <= cut("maxNumberOfExpectedMissingHits_EE", int()) || ignoreCut("maxNumberOfExpectedMissingHits_EE"))
				passCut(retInternal_, "maxNumberOfExpectedMissingHits_EE");
			if ( fabs(Deta)  <  cut("deta_EE",     double()) || ignoreCut("deta_EE")    ) passCut(retInternal_, "deta_EE");
			if ( fabs(Dphi)  <  cut("dphi_EE",     double()) || ignoreCut("dphi_EE")    ) passCut(retInternal_, "dphi_EE");
			if ( sihih       <  cut("sihih_EE",    double()) || ignoreCut("sihih_EE")   ) passCut(retInternal_, "sihih_EE");
			if ( HoE         <  cut("hoe_EE",      double()) || ignoreCut("hoe_EE")     ) passCut(retInternal_, "hoe_EE");
			if ( ooemoop     <  cut("ooemoop_EE",     double()) || ignoreCut("ooemoop_EE") ) passCut(retInternal_, "ooemoop_EE");
			if ( fabs(d0vtx) <  cut("d0vtx_EE",       double()) || ignoreCut("d0vtx_EE")   ) passCut(retInternal_, "d0vtx_EE");
			if ( fabs(dzvtx) <  cut("dzvtx_EE",       double()) || ignoreCut("dzvtx_EE")   ) passCut(retInternal_, "dzvtx_EE");

			if ( pfMVA       >  cut("pfmva_EE",    double()) || ignoreCut("pfmva_EE")   ) passCut(retInternal_, "pfmva_EE");

			if ( trackIso - AeffTk_EE   * rhoRel <  cut("relTrackIso_EE", double()) || ignoreCut("relTrackIso_EE"))
				passCut(retInternal_, "relTrackIso_EE");
			if ( ecalIso  - AeffECAL_EE * rhoRel <  cut("relEcalIso_EE",  double()) || ignoreCut("relEcalIso_EE") )
				passCut(retInternal_, "relEcalIso_EE");
			if ( hcalIso  - AeffHCAL_EE * rhoRel <  cut("relHcalIso_EE",  double()) || ignoreCut("relHcalIso_EE") )
				passCut(retInternal_, "relHcalIso_EE");
			if(pt >= 20) {
				if(iso < cut("pfIso_EE", double()) || ignoreCut("pfIso_EE") )
					passCut(retInternal_, "pfIso_EE");
				passCut(retInternal_, "pfIsoLowPt_EE");
			} else {
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
			passCut(retInternal_, "maxNumberOfExpectedMissingHits_EB");

		}
		setIgnored(retInternal_);
#ifdef DEBUG
		std::cout << "------------------------------ " << version_ << std::endl;
		std::cout << "[DEBUG] retInternal_ = " << retInternal_ << std::endl;
		std::cout << "[DEBUG] ret = " << ret << std::endl;
		ret = retInternal_;
		std::cout << "[DEBUG] copy ret = " << (bool) ret << std::endl;
		std::cout << "[DEBUG] bitMask = " << bitMask() << std::endl;
		if(((bool)ret) == 0) {
			print(std::cout);
		}
#endif
		//    ret= getBitTemplate();
		return (bool)retInternal_;
	}

	int bitMask()
	{
		int mask = 0;
		pat::strbitset::bit_vector retBits = retInternal_.bits();
		for(pat::strbitset::bit_vector::const_iterator bitIter = retBits.begin();
		        bitIter != retBits.end();
		        bitIter++) {
			int value = *bitIter << (bitIter - retBits.begin());
			mask = mask | value;
		}
		return mask;
	}

private: // member variables
	// version of the cuts
	Version_t version_;
	const edm::Handle<electronCollection_t>& electronsHandle_;
	const edm::Handle<reco::ConversionCollection>& ConversionsHandle_;
	const edm::Handle<reco::BeamSpot>& BeamSpotHandle_;
	const edm::Handle<reco::VertexCollection>& VertexHandle_;
	const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle_;
	const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle_;
	const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle_;
	const edm::Handle<double>& rhoHandle_;
};


#endif
