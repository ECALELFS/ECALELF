#ifndef SimpleCutBasedMuonIDSelectionFunctorShervin_h
#define SimpleCutBasedMuonIDSelectionFunctorShervin_h

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
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
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


class SimpleCutBasedMuonIDSelectionFunctor : public Selector<reco::MuonRef>
{

public: // interface

	enum Version_t { NONE = 0, fiducial, loose, soft, tight, highPT};

	SimpleCutBasedMuonIDSelectionFunctor(TString versionStr,
	                                     const edm::Handle<reco::MuonCollection>& muonsHandle,
	                                     const edm::Handle<reco::ConversionCollection>& ConversionsHandle,
	                                     const edm::Handle<reco::BeamSpot>& BeamSpotHandle,
	                                     const edm::Handle<reco::VertexCollection>& VertexHandle,
	                                     const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle,
	                                     const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle,
	                                     const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle,
	                                     const edm::Handle<double>& rhoHandle):
		muonsHandle_(muonsHandle),
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
			std::cout << "SimpleCutBasedMuonIDSelectionFunctor: If you want to use version NONE "
			          << "then you have also to provide the selection cuts by yourself " << std::endl;
			std::cout << "SimpleCutBasedMuonIDSelectionFunctor: ID Version is changed to loose "
			          << std::endl;
			version = loose;
		} else if (versionStr.CompareTo("fiducial") == 0) version = fiducial;
		else if (versionStr.CompareTo("loose") == 0) version = loose;
		else if (versionStr.CompareTo("soft") == 0) version = soft;
		else if (versionStr.CompareTo("tight") == 0) version = tight;
		else if (versionStr.CompareTo("highPT") == 0) version = highPT;
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

		push_back("isPFMuon");
		push_back("isGlobalMuon");
		push_back("isTrackerMuon");

		push_back("innerTrackerLayers");
		push_back("innerPixelLayers");
		push_back("quality");
		push_back("dxy_innerTrack");
		push_back("dz_innerTrack");
		push_back("dxy_BestTrack");
		push_back("dz_BestTrack");

		push_back("normalizedChi2");
		push_back("validMuonHitsNumber");
		push_back("matchedStationsNumber");
		push_back("validPixelHitsNumber");

		push_back("isGoodMuon");
		push_back("isGoodMuonPrompt");

		//take a look here: https://cmssdt.cern.ch/SDT/lxr/source/DataFormats/MuonReco/src/MuonSelectors.cc#745
		if (version_ == fiducial) {
			set("fiducial");
		} else if (version_ == loose) {
			set("isPFMuon");
			set("isGlobalMuon");
			set("isTrackerMuon");
			set("isGoodMuon", 0, false);
			set("isGoodMuonPrompt", 0, false);
			set("innerTrackerLayers", 0, false);
			set("innerPixelLayers", 0, false);
			set("quality", 0, false);
			set("dxy_innerTrack", 0, false);
			set("dz_innerTrack", 0, false);
			set("dxy_BestTrack", 0, false);
			set("dz_BestTrack", 0, false);
			set("normalizedChi2", 0, false);
			set("validMuonHitsNumber", 0, false);
			set("matchedStationsNumber", 0, false);
			set("validPixelHitsNumber", 0, false);
		} else if (version_ == soft) {
			set("isPFMuon", 0, false);
			set("isGlobalMuon", 0, false);
			set("isTrackerMuon", 0, false);
			set("isGoodMuon");
			set("isGoodMuonPrompt", 0, false);
			set("innerTrackerLayers", 5);
			set("innerPixelLayers", 1);
			set("quality", 0, false);
			set("dxy_innerTrack", 3.);
			set("dz_innerTrack", 30.);
			set("dxy_BestTrack", false);
			set("dz_BestTrack", false);
			set("normalizedChi2", 1.8);
			set("validMuonHitsNumber", 0, false);
			set("matchedStationsNumber", 0, false);
			set("validPixelHitsNumber", 0, false);
		} else if (version_ == tight) {
			set("isPFMuon");
			set("isGlobalMuon");
			set("isTrackerMuon", 0, false);
			set("isGoodMuon", 0, false);
			set("isGoodMuonPrompt");
			set("innerTrackerLayers", 5);
			set("innerPixelLayers", 0, false);
			set("quality", 0, false);
			set("dxy_innerTrack", 0, false);
			set("dz_innerTrack", 0, false);
			set("dxy_BestTrack", 0.2);
			set("dz_BestTrack", 0.5);
			set("normalizedChi2", 0, false);
			set("validMuonHitsNumber", 0, false);
			set("matchedStationsNumber", 1);
			set("validPixelHitsNumber", 0);
		} else if (version_ == highPT) {
			set("isPFMuon", 0, false);
			set("isGlobalMuon");
			set("isTrackerMuon", 0, false);
			set("isGoodMuon", 0, false);
			set("isGoodMuonPrompt", 0, false);
			set("innerTrackerLayers", 5);
			set("innerPixelLayers", 0, false);
			set("quality", 0.3);
			set("dxy_innerTrack", 0, false);
			set("dz_innerTrack", 0, false);
			set("dxy_BestTrack", 0.2);
			set("dz_BestTrack", 0.5);
			set("normalizedChi2", 0, false);
			set("validMuonHitsNumber", 0);
			set("matchedStationsNumber", 1);
			set("validPixelHitsNumber", 0);
		}

	}


	bool operator()( const reco::MuonRef& muon, pat::strbitset& ret)
	{
		retInternal_ = getBitTemplate();
		// for the time being only WPxx_PU variable definition
		return WPxx_PU(muon, ret );
	}
	float result()
	{
		return (float) retInternal_;
	}

	using Selector<reco::MuonRef>::operator();
	// function with the Spring10 variable definitions
	bool WPxx_PU( const reco::MuonRef muonRef, pat::strbitset& ret)
	{
		const reco::Muon muon = *muonRef;

		bool isPFMuon = muon.isPFMuon();
		bool isGlobalMuon = muon.isGlobalMuon();
		bool isTrackerMuon = muon.isTrackerMuon();

		double innerTrackerLayers = 0;//muon.innerTrack()->hitPattern().trackerLayersWithMeasurement();
		double innerPixelLayers = 0;//muon.innerTrack()->hitPattern().pixelLayersWithMeasurement();
		double quality = 0;//muon.muonBestTrack()->ptError()/muon.muonBestTrack()->pt();

		//    reco::VertexRef vtx(VertexHandle_, 0); //get vertex

		double dxy_innerTrack = 0;//muon.innerTrack()->dxy(vtx->position());
		double dz_innerTrack = 0;//muon.innerTrack()->dz(vtx->position());
		double dxy_BestTrack = 0;//muon.bestTrack()->dxy(vtx->position());
		double dz_BestTrack = 0;//muon.bestTrack()->dz(vtx->position());

		double normalizedChi2 = 0;//muon.innerTrack()->normalizedChi2();
		double validMuonHitsNumber = 0;// muon.globalTrack()->hitPattern().numberOfValidMuonHits();
		double matchedStationsNumber = 0;//muon.numberOfMatchedStations();
		double validPixelHitsNumber = 0;//muon.innerTrack()->hitPattern().numberOfValidPixelHits();

		bool isGoodMuonPrompt = muon::isGoodMuon(muon, muon::GlobalMuonPromptTight);
		bool isGoodMuon = muon::isGoodMuon(muon, muon::TMOneStationTight);


		//------------------------------ Fiducial region cut
		if (( muon.eta() < 4.5 ) || ignoreCut("fiducial")) passCut(retInternal_, "fiducial");

		if ( (isPFMuon) || ignoreCut("isPFMuon")) passCut(retInternal_, "isPFMuon");
		if ( (isGlobalMuon) || ignoreCut("isGlobalMuon")) passCut(retInternal_, "isGlobalMuon");
		if ( (isTrackerMuon) || ignoreCut("isTrackerMuon")) passCut(retInternal_, "isTrackerMuon");
		if ( (isGoodMuon) || ignoreCut("isGoodMuon")) passCut(retInternal_, "isGoodMuon");
		if ( (isGoodMuonPrompt) || ignoreCut("isGoodMuonPrompt")) passCut(retInternal_, "isGoodMuonPrompt");

		if ( innerTrackerLayers       <  cut("innerTrackerLayers",    double()) || ignoreCut("innerTrackerLayers")   ) passCut(retInternal_, "innerTrackerLayers");
		if ( innerPixelLayers       <  cut("innerPixelLayers",    double()) || ignoreCut("innerPixelLayers")   ) passCut(retInternal_, "innerPixelLayers");
		if ( quality       <  cut("quality",    double()) || ignoreCut("quality")   ) passCut(retInternal_, "quality");

		if ( dxy_innerTrack       <  cut("dxy_innerTrack",    double()) || ignoreCut("dxy_innerTrack")   ) passCut(retInternal_, "dxy_innerTrack");
		if ( dz_innerTrack       <  cut("dz_innerTrack",    double()) || ignoreCut("dz_innerTrack")   ) passCut(retInternal_, "dz_innerTrack");
		if ( dxy_BestTrack       <  cut("dxy_BestTrack",    double()) || ignoreCut("dxy_BestTrack")   ) passCut(retInternal_, "dxy_BestTrack");
		if ( dz_BestTrack       <  cut("dz_BestTrack",    double()) || ignoreCut("dz_BestTrack")   ) passCut(retInternal_, "dz_BestTrack");

		if ( normalizedChi2       <  cut("normalizedChi2",    double()) || ignoreCut("normalizedChi2")   ) passCut(retInternal_, "normalizedChi2");
		if ( validMuonHitsNumber       <  cut("validMuonHitsNumber",    double()) || ignoreCut("validMuonHitsNumber")   ) passCut(retInternal_, "validMuonHitsNumber");
		if ( matchedStationsNumber       <  cut("matchedStationsNumber",    double()) || ignoreCut("matchedStationsNumber")   ) passCut(retInternal_, "matchedStationsNumber");
		if ( validPixelHitsNumber       <  cut("validPixelHitsNumber",    double()) || ignoreCut("validPixelHitsNumber")   ) passCut(retInternal_, "validPixelHitsNumber");


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
	const edm::Handle<reco::MuonCollection>& muonsHandle_;
	const edm::Handle<reco::ConversionCollection>& ConversionsHandle_;
	const edm::Handle<reco::BeamSpot>& BeamSpotHandle_;
	const edm::Handle<reco::VertexCollection>& VertexHandle_;
	const edm::Handle< edm::ValueMap<double> >& chIsoValsHandle_;
	const edm::Handle< edm::ValueMap<double> >& emIsoValsHandle_;
	const edm::Handle< edm::ValueMap<double> >& nhIsoValsHandle_;
	const edm::Handle<double>& rhoHandle_;
};


#endif
