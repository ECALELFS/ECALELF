#ifndef ElectronRecalibSuperClusterAssociator_h
#define ElectronRecalibSuperClusterAssociator_h

//
// Package:         RecoEgamma/EgammaElectronProducers
// Class:           ElectronRecalibSuperClusterAssociator
//
// Description:


#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

#include <string>

//class PixelMatchElectronAlgo;

class ElectronRecalibSuperClusterAssociator : public edm::EDProducer
{
public:

	explicit ElectronRecalibSuperClusterAssociator(const edm::ParameterSet& conf);

	virtual ~ElectronRecalibSuperClusterAssociator();

	virtual void produce(edm::Event& e, const edm::EventSetup& c);

private:
	edm::InputTag electronSrc_;
	edm::InputTag superClusterCollectionEB_;
	edm::InputTag superClusterCollectionEE_;

	std::string outputLabel_;

	edm::EDGetTokenT<reco::GsfElectronCollection> electronToken_;
	edm::EDGetTokenT<reco::SuperClusterCollection> ebScToken_;
	edm::EDGetTokenT<reco::SuperClusterCollection> eeScToken_;
	edm::EDGetTokenT<EcalRecHitCollection> ebRecHitsToken_;
	edm::EDGetTokenT<EcalRecHitCollection> eeRecHitsToken_;

	reco::GsfElectron::ShowerShape calculateShowerShape_full5x5( const reco::SuperClusterRef & theClus, const EcalRecHitCollection* recHits, const reco::GsfElectron::ShowerShape& ss);

	edm::ESHandle<CaloTopology> _theCaloTopology;
	edm::ESHandle<CaloGeometry> _theGeometry;
};
#endif
