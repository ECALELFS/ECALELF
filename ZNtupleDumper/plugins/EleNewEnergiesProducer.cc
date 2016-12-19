// -*- C++ -*-
//
// Package:    EleNewEnergiesProducer
// Class:      EleNewEnergiesProducer
//
/**\class EleNewEnergiesProducer EleNewEnergiesProducer.cc Calibration/ZNtupleDumper/plugins/EleNewEnergiesProducer.cc

 Description: [one line class summary]

 Implementation:
 TODO:
     - add the error on the  regression energy
     - define the name of the collection
     - implement a switch to select which energies to produce
     - remove unuseful variables


     [Notes on implementation]
*/
//
// Original Author:  Shervin Nourbakhsh,40 1-B24,+41227671643,
//         Created:  Thu Jul  5 20:17:56 CEST 2012
// $Id$
//
//

// system include files
#include <memory>
#include <cassert>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

// for the output
#include "DataFormats/Common/interface/ValueMap.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "RecoEgamma/EgammaTools/interface/ConversionInfo.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"


#include "RecoEgamma/EgammaTools/interface/SCEnergyCorrectorSemiParm.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include "FWCore/Utilities/interface/EDGetToken.h"

#include <DataFormats/Math/interface/deltaR.h>
//
// class declaration
//

class EleNewEnergiesProducer : public edm::EDProducer
{

	typedef float v_t;
	typedef edm::ValueMap<v_t> NewEnergyMap;

public:
	explicit EleNewEnergiesProducer(const edm::ParameterSet&);
	~EleNewEnergiesProducer();

	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
	virtual void beginJob() ;
	virtual void produce(edm::Event&, const edm::EventSetup&);
	virtual void endJob() ;

	virtual void beginRun(edm::Run&, edm::EventSetup const&);
	virtual void endRun(edm::Run&, edm::EventSetup const&);
	virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

	virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

	// ----------member data ---------------------------

	// ----------member data ---------------------------

private:

	bool isMC;
	// input tag for primary vertex
	edm::InputTag vtxCollectionTAG;
	edm::InputTag BeamSpotTAG;

	/// input tag for electrons
	//  edm::InputTag electronsTAG;

	/// input rho
	edm::InputTag rhoTAG;

	edm::InputTag conversionsProducerTAG;

	std::string foutName;
	std::string puWeightFile;

private:
	// Handle to the electron collection

	edm::EDGetTokenT< edm::View<pat::Electron> > electronsTAG;
	edm::EDGetTokenT< edm::View<pat::Photon> > photonsTAG;
	edm::Handle<edm::View<pat::Electron> > electronsHandle;
	edm::Handle<edm::View<pat::Photon> > photonsHandle;
	edm::Handle<edm::SortedCollection<EcalRecHit, edm::StrictWeakOrdering<EcalRecHit> > > recHitCollectionEBHandle;
	edm::Handle<edm::SortedCollection<EcalRecHit, edm::StrictWeakOrdering<EcalRecHit> > > recHitCollectionEEHandle;
	edm::Handle<reco::BeamSpot> bsHandle;
	edm::Handle<reco::VertexCollection> primaryVertexHandle;
	edm::ESHandle<CaloTopology> topologyHandle;
	edm::Handle<double> rhoHandle;
	edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
	edm::Handle<reco::ConversionCollection> conversionsHandle;

	//------------------------------ Josh's regression (Hgg)
	SCEnergyCorrectorSemiParm mustache_regr_;
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
EleNewEnergiesProducer::EleNewEnergiesProducer(const edm::ParameterSet& iConfig):
	electronsTAG ( consumes<edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>( "electronCollection" ) ) ),
	photonsTAG ( consumes<edm::View<pat::Photon> >(iConfig.getParameter<edm::InputTag>( "photonCollection" ) ) )
	//  electronsTAG(iConfig.getParameter<edm::InputTag>("electronCollection"))
{
	//	std::cout << electronsTAG << std::endl;
	// this name are hard coded, should be put in the cfi
	produces< NewEnergyMap >("energySCEleMust");
	produces< NewEnergyMap >("energySCEleMustVar");
	produces< NewEnergyMap >("energySCElePho");
	produces< NewEnergyMap >("energySCElePhoVar");

//now do what ever other initialization is needed
	edm::ConsumesCollector iC = consumesCollector();
	mustache_regr_.setTokens(iConfig.getParameterSet("scEnergyCorrectorSemiParm"), iC);
}



EleNewEnergiesProducer::~EleNewEnergiesProducer()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
EleNewEnergiesProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;


	std::auto_ptr<NewEnergyMap> energySC_mustMap(new NewEnergyMap());
	std::auto_ptr<NewEnergyMap> energySC_mustVarMap(new NewEnergyMap());
	std::auto_ptr<NewEnergyMap> energySC_phoMap(new NewEnergyMap());
	std::auto_ptr<NewEnergyMap> energySC_phoVarMap(new NewEnergyMap());

	iEvent.getByToken(electronsTAG, electronsHandle);
	iEvent.getByToken(photonsTAG, photonsHandle);
	mustache_regr_.setEvent(iEvent);

	std::vector<v_t>  energySC_must(electronsHandle->size(), -999.);
	std::vector<v_t>  energySC_mustVar(electronsHandle->size(), -999);
	std::vector<v_t>  energySC_pho(electronsHandle->size(), -999.);
	std::vector<v_t>  energySC_phoVar(electronsHandle->size(), -999);

	//iSetup.get<CaloTopologyRecord>().get(topologyHandle);
	for(unsigned int iEle = 0; iEle < electronsHandle->size(); ++iEle) {
		edm::Ptr<pat::Electron> ele_itr = electronsHandle->ptrAt(iEle);

		if(!ele_itr->ecalDriven())
			continue;
		if (ele_itr->pt() < 5) //clusters not available in MINIAOD
			continue;

		if(photonsHandle.isValid()) {
			//now associate electron to photon via SC
			for (auto pho_itr = photonsHandle->begin(); pho_itr != photonsHandle->end(); ++pho_itr) {
				float dR = deltaR(ele_itr->superCluster()->eta(), ele_itr->superCluster()->phi(), pho_itr->superCluster()->eta(), pho_itr->superCluster()->phi());
				if (dR < 1E-2) {
					energySC_pho[iEle] = pho_itr->energy();
					energySC_phoVar[iEle] = pho_itr->getCorrectedEnergyError(pho_itr->getCandidateP4type());
					break;
				}
			}
		}

		reco::SuperCluster correctedSuperCluster;

		if (ele_itr->parentSuperCluster().isAvailable())
			correctedSuperCluster = *(ele_itr->parentSuperCluster());
		else
			continue;
//			correctedSuperCluster = *(ele_itr->superCluster()); //just to make it run also on MINIAOD...

		if (!correctedSuperCluster.clusters().isAvailable())
			continue;

		//  mustache_regr_.modifyObject(correctedSuperCluster);
		//fill the vector with the energies
		energySC_must[iEle] = correctedSuperCluster.correctedEnergy();
		energySC_mustVar[iEle] = correctedSuperCluster.correctedEnergyUncertainty();

	}

	//prepare product
	// declare the filler of the ValueMap
	NewEnergyMap::Filler energySC_must_filler(   *energySC_mustMap);
	NewEnergyMap::Filler energySC_mustVar_filler(*energySC_mustVarMap);
	NewEnergyMap::Filler energySC_pho_filler(   *energySC_phoMap);
	NewEnergyMap::Filler energySC_phoVar_filler(*energySC_phoVarMap);

	//fill and insert valuemapv
	energySC_must_filler.insert( electronsHandle, energySC_must.begin(), energySC_must.end());
	energySC_mustVar_filler.insert( electronsHandle, energySC_mustVar.begin(), energySC_mustVar.end()); //fra
	energySC_pho_filler.insert( electronsHandle,    energySC_pho.begin(),   energySC_pho.end());
	energySC_phoVar_filler.insert( electronsHandle, energySC_phoVar.begin(), energySC_phoVar.end()); //fra

	energySC_must_filler.fill();
	energySC_mustVar_filler.fill();
	energySC_pho_filler.fill();
	energySC_phoVar_filler.fill();

	//------------------------------
	// put the ValueMap in the event
	iEvent.put(energySC_mustMap, "energySCEleMust");
	iEvent.put(energySC_mustVarMap, "energySCEleMustVar");
	iEvent.put(energySC_phoMap, "energySCElePho");
	iEvent.put(energySC_phoVarMap, "energySCElePhoVar");

}

// ------------ method called once each job just before starting event loop  ------------
void
EleNewEnergiesProducer::beginJob()
{

}

// ------------ method called once each job just after ending the event loop  ------------
void
EleNewEnergiesProducer::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
void
EleNewEnergiesProducer::beginRun(edm::Run&, edm::EventSetup const& iSetup)
{
}

// ------------ method called when ending the processing of a run  ------------
void
EleNewEnergiesProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block ------------
void  EleNewEnergiesProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const& iSetup)
{
//	mustache_regr_.setEventSetup(iSetup);
}

// ------------ method called when ending the processing of a luminosity block  ------------
void
EleNewEnergiesProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EleNewEnergiesProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EleNewEnergiesProducer);
