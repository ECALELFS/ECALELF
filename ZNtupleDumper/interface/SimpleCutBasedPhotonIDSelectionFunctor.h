#ifndef SimpleCutBasedPhotonIDSelectionFunctorShervin_h
#define SimpleCutBasedPhotonIDSelectionFunctorShervin_h

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
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
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


class SimpleCutBasedPhotonIDSelectionFunctor : public Selector<reco::PhotonRef>
{

public: // interface

	enum Version_t { NONE = 0, fiducial, loose, medium, tight, loose25nsRun2, medium25nsRun2, tight25nsRun2};

	SimpleCutBasedPhotonIDSelectionFunctor(TString versionStr,
	                                       const edm::Handle<reco::PhotonCollection>& photonsHandle,
	                                       const edm::Handle<reco::ConversionCollection>& ConversionsHandle,
	                                       const edm::Handle<reco::BeamSpot>& BeamSpotHandle,
	                                       const edm::Handle<reco::VertexCollection>& VertexHandle,
	                                       const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle,
	                                       const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle,
	                                       const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle,
	                                       const edm::Handle<double>& rhoHandle):
		photonsHandle_(photonsHandle),
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
			std::cout << "SimpleCutBasedPhotonIDSelectionFunctor: If you want to use version NONE "
			          << "then you have also to provide the selection cuts by yourself " << std::endl;
			std::cout << "SimpleCutBasedPhotonIDSelectionFunctor: ID Version is changed to loose "
			          << std::endl;
			version = loose;
		} else if (versionStr.CompareTo("fiducial") == 0) version = fiducial;
		else if (versionStr.CompareTo("loose") == 0) version = loose;
		else if (versionStr.CompareTo("medium") == 0) version = medium;
		else if (versionStr.CompareTo("tight") == 0) version = tight;
		else if (versionStr.CompareTo("loose25nsRun2") == 0) version = loose25nsRun2;
		else if (versionStr.CompareTo("medium25nsRun2") == 0) version = medium25nsRun2;
		else if (versionStr.CompareTo("tight25nsRun2") == 0) version = tight25nsRun2;
		else {
			std::cerr << "[ERROR] version type not defined" << std::endl;
			std::cerr << "[ERROR] using loose" << std::endl;
			version = loose;
		}

		initialize(version);
		retInternal_ = getBitTemplate();
	}

	void initialize( Version_t version )
	{
		version_ = version;

		//------------------------------ initialize the cut variables (to be put in the bitmask)

		push_back("fiducial");

		// conversion rejection
		push_back("hasMatchedConversion");

		//eleID EB
		push_back("hoe_EB"     );
		push_back("sihih_EB"   );
		//eleIso EB
		push_back("pfChIso_EB");
		push_back("pfNhIso_EB" );
		push_back("pfPhIso_EB" );

		//eleID EE
		push_back("hoe_EE"     );
		push_back("sihih_EE"   );
		//eleIso EE
		push_back("pfChIso_EE");
		push_back("pfNhIso_EE" );
		push_back("pfPhIso_EE" );

		//    push_back("conversionRejection"            );

		if (version_ == fiducial) {
			set("fiducial");
		} else if (version_ == loose) {    //OLD VALUES - see https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonID2012
			//set("fiducial");
			set("hasMatchedConversion");
			set("hoe_EB",        0.05);
			set("hoe_EE",          0.05);
			set("sihih_EB",      0.012);
			set("sihih_EE",        0.034);
			set("pfChIso_EB", 2.6);
			set("pfChIso_EE", 2.3);
			set("pfNhIso_EB",  3.5);
			set("pfNhIso_EE",  2.9);
			set("pfPhIso_EB",  1.3);
			set("pfPhIso_EE",  100., false);
			set("coef_pfPhIso_EB", 0.005);
			set("coef_pfPhIso_EE", 0.005);
			set("coef_pfNhIso_EB",  0.04);
			set("coef_pfNhIso_EE",  0.04);
		} else if (version_ == medium) {    //OLD VALUES - see https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonID2012
			//set("fiducial");
			set("hasMatchedConversion");
			set("hoe_EB",        0.05);
			set("hoe_EE",          0.05);
			set("sihih_EB",      0.011);
			set("sihih_EE",        0.033);
			set("pfChIso_EB", 1.5);
			set("pfChIso_EE", 1.2);
			set("pfNhIso_EB",  1.0);
			set("pfNhIso_EE",  1.5);
			set("pfPhIso_EB",  0.7);
			set("pfPhIso_EE",  1.0);
			set("coef_pfPhIso_EB", 0.005);
			set("coef_pfPhIso_EE", 0.005);
			set("coef_pfNhIso_EB",  0.04);
			set("coef_pfNhIso_EE",  0.04);
		} else if (version_ == tight) {    //OLD VALUES - see https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonID2012
			//set("fiducial");
			set("hasMatchedConversion");
			set("hoe_EB",        0.05);
			set("hoe_EE",          0.05);
			set("sihih_EB",      0.011);
			set("sihih_EE",        0.031);
			set("pfChIso_EB", 0.7);
			set("pfChIso_EE", 0.5);
			set("pfNhIso_EB",  0.4);
			set("pfNhIso_EE",  1.5);
			set("pfPhIso_EB",  0.5);
			set("pfPhIso_EE",  1.0);
			set("coef_pfPhIso_EB", 0.005);
			set("coef_pfPhIso_EE", 0.005);
			set("coef_pfNhIso_EB",  0.04);
			set("coef_pfNhIso_EE",  0.04);
		} else if (version_ == loose25nsRun2) {    //PHYS14 - see https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
			//set("fiducial");
			set("hasMatchedConversion");
			set("hoe_EB",        0.032);
			set("hoe_EE",          0.023);
			set("sihih_EB",      0.0100);
			set("sihih_EE",        0.0270);
			set("pfChIso_EB", 2.94);
			set("pfChIso_EE", 3.07);
			set("pfNhIso_EB",  3.16);
			set("pfNhIso_EE",  17.16);
			set("pfPhIso_EB",  4.43);
			set("pfPhIso_EE",  2.11);
			set("coef_pfPhIso_EB", 0.0004);
			set("coef_pfPhIso_EE", 0.0037);
			set("coef_pfNhIso_EB",  0.0023);
			set("coef_pfNhIso_EE",  0.0116);
		} else if (version_ == medium25nsRun2) {    //PHYS14 - see https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
			//set("fiducial");
			set("hasMatchedConversion");
			set("hoe_EB",        0.020);
			set("hoe_EE",          0.011);
			set("sihih_EB",      0.0099);
			set("sihih_EE",        0.0269);
			set("pfChIso_EB", 2.62);
			set("pfChIso_EE", 1.40);
			set("pfNhIso_EB",  2.69);
			set("pfNhIso_EE",  4.92);
			set("pfPhIso_EB",  1.35);
			set("pfPhIso_EE",  2.11);
			set("coef_pfPhIso_EB", 0.0004);
			set("coef_pfPhIso_EE", 0.0037);
			set("coef_pfNhIso_EB",  0.0023);
			set("coef_pfNhIso_EE",  0.0116);
		} else if (version_ == tight25nsRun2) {    //PHYS14 - see https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
			//set("fiducial");
			set("hasMatchedConversion");
			set("hoe_EB",        0.012);
			set("hoe_EE",          0.011);
			set("sihih_EB",      0.0098);
			set("sihih_EE",        0.0264);
			set("pfChIso_EB", 1.91);
			set("pfChIso_EE", 1.26);
			set("pfNhIso_EB",  2.55);
			set("pfNhIso_EE",  2.71);
			set("pfPhIso_EB",  1.29);
			set("pfPhIso_EE",  1.91);
			set("coef_pfPhIso_EB", 0.0004);
			set("coef_pfPhIso_EE", 0.0037);
			set("coef_pfNhIso_EB",  0.0023);
			set("coef_pfNhIso_EE",  0.0116);
		}


	}


	bool operator()( const reco::PhotonRef& photon, pat::strbitset& ret)
	{
		retInternal_ = getBitTemplate();
		// for the time being only WPxx_PU variable definition
		return WPxx_PU(photon, ret );
	}
	float result()
	{
		return (float) retInternal_;
	}

	using Selector<reco::PhotonRef>::operator();
	// function with the Spring10 variable definitions
	bool WPxx_PU( const reco::PhotonRef photonRef, pat::strbitset& ret)
	{
		const reco::Photon photon = *photonRef;

		Double_t sihih    = photon.sigmaIetaIeta();
		Double_t HoE      = photon.hadronicOverEm();

		double iso_ch = photon.chargedHadronIso();
		double iso_em = photon.photonIso();
		double iso_nh = photon.neutralHadronIso();

		if (photon.eta() < 1.479) {
			iso_nh = iso_nh - cut("coef_pfNhIso_EB", double()) * photon.pt(); //https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
			iso_em = iso_em - cut("coef_pfPhIso_EB", double()) * photon.pt(); //https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
		} else {
			iso_nh = iso_nh - cut("coef_pfNhIso_EE", double()) * photon.pt(); //https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
			iso_em = iso_em - cut("coef_pfPhIso_EE", double()) * photon.pt(); //https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2
		}

		const reco::SuperCluster photonSC = *(photon.superCluster());
		bool hasMatchedConversion = ConversionTools::hasMatchedConversion(photonSC, ConversionsHandle_, BeamSpotHandle_->position());

		Double_t absEtaSC = fabs(photon.superCluster()->eta());

		//------------------------------ Fiducial region cut
		if ( (absEtaSC < 1.4442 || (absEtaSC > 1.566 && absEtaSC < 2.5) ) || ignoreCut("fiducial")) passCut(retInternal_, "fiducial");

		//------------------------------ conversion rejection cut
		if ( (!hasMatchedConversion) || ignoreCut("hasMatchedConversion")) passCut(retInternal_, "hasMatchedConversion");

		if (photon.isEB()) { // BARREL case
			if ( sihih       <  cut("sihih_EB",    double()) || ignoreCut("sihih_EB")   ) passCut(retInternal_, "sihih_EB");
			if ( HoE         <  cut("hoe_EB",      double()) || ignoreCut("hoe_EB")     ) passCut(retInternal_, "hoe_EB");
			if ( iso_ch <  cut("pfChIso_EB",  double()) || ignoreCut("pfChIso_EB") )      passCut(retInternal_, "pfChIso_EB");
			if ( iso_nh  <  cut("pfNhIso_EB",  double()) || ignoreCut("pfNhIso_EB") )      passCut(retInternal_, "pfNhIso_EB");
			if ( iso_em    <  cut("pfPhIso_EB",  double()) || ignoreCut("pfPhIso_EB") )      passCut(retInternal_, "pfPhIso_EB");

			// pass all the EE cuts
			passCut(retInternal_, "sihih_EE");
			passCut(retInternal_, "hoe_EE");
			passCut(retInternal_, "pfChIso_EE");
			passCut(retInternal_, "pfNhIso_EE");
			passCut(retInternal_, "pfPhIso_EE");
		} else { // ENDCAPS case
			if ( sihih       <  cut("sihih_EE",    double()) || ignoreCut("sihih_EE")   ) passCut(retInternal_, "sihih_EE");
			if ( HoE         <  cut("hoe_EE",      double()) || ignoreCut("hoe_EE")     ) passCut(retInternal_, "hoe_EE");
			if ( iso_ch <  cut("pfChIso_EE",  double()) || ignoreCut("pfChIso_EE") )      passCut(retInternal_, "pfChIso_EE");
			if ( iso_nh  <  cut("pfNhIso_EE",  double()) || ignoreCut("pfNhIso_EE") )      passCut(retInternal_, "pfNhIso_EE");
			if ( iso_em    <  cut("pfPhIso_EE",  double()) || ignoreCut("pfPhIso_EE") )      passCut(retInternal_, "pfPhIso_EE");

			// pass all the EB cuts
			passCut(retInternal_, "sihih_EB");
			passCut(retInternal_, "hoe_EB");
			passCut(retInternal_, "pfChIso_EB");
			passCut(retInternal_, "pfNhIso_EB");
			passCut(retInternal_, "pfPhIso_EB");
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
	const edm::Handle<reco::PhotonCollection>& photonsHandle_;
	const edm::Handle<reco::ConversionCollection>& ConversionsHandle_;
	const edm::Handle<reco::BeamSpot>& BeamSpotHandle_;
	const edm::Handle<reco::VertexCollection>& VertexHandle_;
	const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle_;
	const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle_;
	const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle_;
	const edm::Handle<double>& rhoHandle_;
};


#endif
