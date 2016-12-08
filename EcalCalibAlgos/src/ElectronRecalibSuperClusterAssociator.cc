// -*- C++ -*-
//
//

// user include files
#include "FWCore/Framework/interface/MakerMacros.h"
#include "Calibration/EcalCalibAlgos/interface/ElectronRecalibSuperClusterAssociator.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include <iostream>
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "FWCore/Utilities/interface/isFinite.h"
//#define DEBUG


ElectronRecalibSuperClusterAssociator::ElectronRecalibSuperClusterAssociator(const edm::ParameterSet& iConfig)
{
#ifdef DEBUG
	std::cout << "ElectronRecalibSuperClusterAssociator::ElectronRecalibSuperClusterAssociator" << std::endl;
#endif

	//register your products
	produces<reco::GsfElectronCollection>();
	produces<reco::GsfElectronCoreCollection>() ;
	//  produces<SuperClusterCollection>();

	outputLabel_ = iConfig.getParameter<std::string>("outputLabel");

	electronToken_  = consumes<reco::GsfElectronCollection>(iConfig.getParameter<edm::InputTag > ("electronSrc"));
	ebScToken_      = consumes<reco::SuperClusterCollection>(iConfig.getParameter<edm::InputTag > ("superClusterCollectionEB"));
	eeScToken_      = consumes<reco::SuperClusterCollection>(iConfig.getParameter<edm::InputTag > ("superClusterCollectionEE"));
	ebRecHitsToken_ = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag > ("recHitCollectionEB"));
	eeRecHitsToken_ = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag > ("recHitCollectionEE"));
	
#ifdef DEBUG
	std::cout << "ElectronRecalibSuperClusterAssociator::ElectronRecalibSuperClusterAssociator::end" << std::endl;
#endif


//	ElectronHcalHelper::Configuration hcalCfg;

}

ElectronRecalibSuperClusterAssociator::~ElectronRecalibSuperClusterAssociator()
{
}

// ------------ method called to produce the data  ------------
void ElectronRecalibSuperClusterAssociator::produce(edm::Event& e, const edm::EventSetup& iSetup)
{
#ifdef DEBUG
	std::cout << "GEDElectronRecalibSuperClusterAssociator::produce" << std::endl;
#endif

	iSetup.get<CaloTopologyRecord>().get(_theCaloTopology);
	iSetup.get<CaloGeometryRecord>().get(_theGeometry);

	// Create the output collections
	std::auto_ptr<reco::GsfElectronCollection> pOutEle(new reco::GsfElectronCollection);
	std::auto_ptr<reco::GsfElectronCoreCollection> pOutEleCore(new reco::GsfElectronCoreCollection);

	// Get SuperClusters in EB
	edm::Handle<reco::SuperClusterCollection> superClusterEBHandle;
	e.getByToken(ebScToken_, superClusterEBHandle);
#ifdef DEBUG
	std::cout << "EB scCollection->size()" << superClusterEBHandle->size() << std::endl;
#endif

	// Get SuperClusters in EE
	edm::Handle<reco::SuperClusterCollection> superClusterEEHandle;
	e.getByToken(eeScToken_, superClusterEEHandle);

	edm::Handle<EcalRecHitCollection> recHitsEBHandle;
	e.getByToken(ebRecHitsToken_, recHitsEBHandle);
	edm::Handle<EcalRecHitCollection> recHitsEEHandle;
	e.getByToken(eeRecHitsToken_, recHitsEEHandle);


#ifdef DEBUG
	std::cout << "EE scCollection->size()" << superClusterEEHandle->size() << std::endl;
#endif

	// Get Electrons
	edm::Handle<reco::GsfElectronCollection> eleHandle;
	e.getByToken(electronToken_, eleHandle);
	//  const reco::GsfElectronCollection* electronCollection = eleHandle.product();

	reco::GsfElectronCoreRefProd rEleCore = e.getRefBeforePut<reco::GsfElectronCoreCollection>();
	edm::Ref<reco::GsfElectronCoreCollection>::key_type idxEleCore = 0;

#ifdef DEBUG
	std::cout << "[DEBUG" << "Electron collection size: " << eleHandle->size() << std::endl;
#endif

	if(superClusterEEHandle->size() + superClusterEBHandle->size() < eleHandle->size()) {
		edm::LogError("ALCARERECO") << "Reconstructed SCs < old electrons";
#ifdef DEBUG
		assert(superClusterEEHandle->size() + superClusterEBHandle->size() >= eleHandle->size());
#endif
	}
	for(reco::GsfElectronCollection::const_iterator eleIt = eleHandle->begin(); eleIt != eleHandle->end(); eleIt++) {
		float DeltaRMineleSCbarrel(0.15); //initial minDeltaR
		float DeltaRMineleSCendcap(0.15);
		const reco::SuperCluster* nearestSCbarrel = 0;
		const reco::SuperCluster* nearestSCendcap = 0;
		int iscRef = -1, iscRefendcap = -1;
		int iSC = 0;

		if(!eleIt->ecalDrivenSeed()) {
			edm::LogError("trackerDriven") << "skipping trackerDriven electrons";
			continue;
		}
		// first loop is on EB superClusters
		iSC = 0;
		for(reco::SuperClusterCollection::const_iterator scIt = superClusterEBHandle->begin();
		        scIt != superClusterEBHandle->end(); scIt++, iSC++) {

			double DeltaReleSC = sqrt(reco::deltaR2(eleIt->eta(), eleIt->phi(),
			                                        scIt->eta(), scIt->phi()));

			if(DeltaReleSC < DeltaRMineleSCbarrel) { //save the nearest SC
				DeltaRMineleSCbarrel = DeltaReleSC;
				nearestSCbarrel = &*scIt;
				iscRef = iSC;
			}
#ifdef DEBUG
			std::cout << "EB: " << scIt - superClusterEBHandle->begin() << " " << iSC << " " << iscRef
			          << "\t" << std::setprecision(4) << scIt->energy()
			          << " " << scIt->eta() << " " << scIt->phi()
			          << "\t--\t" << eleIt->energy() << " " << eleIt->eta() << " " << eleIt->phi()
			          << "\t" << DeltaRMineleSCbarrel
			          << std::endl;
#endif
		}

		// second loop is on EE superClusters
		iSC = 0;
		for(reco::SuperClusterCollection::const_iterator scIt = superClusterEEHandle->begin();
		        scIt != superClusterEEHandle->end(); scIt++, iSC++) {
#ifdef DEBUG
			std::cout << "EE: " << scIt - superClusterEEHandle->begin() << " " << iSC << " " << iscRef
			          << "\t" << std::setprecision(4) << scIt->energy()
			          << " " << scIt->eta() << " " << scIt->phi()
			          << "\t--\t " << eleIt->energy() << " " << eleIt->eta() << " " << eleIt->phi()
			          << "\t" << DeltaRMineleSCendcap
			          << std::endl;
#endif

			double DeltaReleSC = sqrt(reco::deltaR2(eleIt->eta(), eleIt->phi(),
			                                        scIt->eta(), scIt->phi()));

			if(DeltaReleSC < DeltaRMineleSCendcap) {
				DeltaRMineleSCendcap = DeltaReleSC;
				nearestSCendcap = &*scIt;
				iscRefendcap = iSC;
			}
		}
		////////////////////////
		//      if(eleIt->isEB()) assert(DeltaRMineleSCbarrel < DeltaRMineleSCendcap);
		//else assert(DeltaRMineleSCbarrel > DeltaRMineleSCendcap);
		if(eleIt->isEB() && DeltaRMineleSCbarrel > DeltaRMineleSCendcap) {
			edm::LogError("ElectronRecalibAssociator") << "EB electron, but nearest SC is in EE";;
			continue;
		}

		reco::SuperClusterRef scRef(reco::SuperClusterRef(superClusterEBHandle, iscRef));  // Reference to the new SC
		if( (eleIt->isEB() && nearestSCbarrel) || (!(eleIt->isEB()) && nearestSCendcap)) {
			pOutEleCore->push_back(*eleIt->core()); // clone the old core and add to the collection of new cores
			reco::GsfElectronCoreRef newEleCoreRef(rEleCore, idxEleCore++); // reference to the new electron core in the new collection
			reco::GsfElectronCore & newEleCore = pOutEleCore->back(); // pick the clone
			//newEleCore.setGsfTrack(eleIt->gsfTrack());           // set the gsf track (not needed since it is not changed)
			const EcalRecHitCollection *recHits =NULL;
			if(eleIt->isEB()){
				recHits = recHitsEBHandle.product();
				reco::SuperClusterRef scRef(reco::SuperClusterRef(superClusterEBHandle, iscRef));  // Reference to the new SC
				newEleCore.setParentSuperCluster(scRef);             // mustache
				newEleCore.setSuperCluster(scRef); // let's check this! if it is possible to recreate the pfSC
			} else {
				recHits = recHitsEEHandle.product();
				reco::SuperClusterRef scRef(reco::SuperClusterRef(superClusterEEHandle, iscRefendcap));  // Reference to the new SC
				newEleCore.setParentSuperCluster(scRef);             // mustache
				newEleCore.setSuperCluster(scRef); // let's check this! if it is possible to recreate the pfSC
			}

			pOutEle->push_back(reco::GsfElectron(*eleIt, newEleCoreRef));
			reco::GsfElectron& newEle = pOutEle->back();
			
			//-- first possibility: set the new p4SC using refined SC
			newEle.setP4(reco::GsfElectron::P4_FROM_SUPER_CLUSTER,
			             eleIt->p4(reco::GsfElectron::P4_FROM_SUPER_CLUSTER),
			             eleIt->p4Error(reco::GsfElectron::P4_FROM_SUPER_CLUSTER), false);  //*newEle.superCluster()->energy()/eleIt->superCluster()->energy());
			
			//-- second possibility: set the new p4SC using mustache SC
			//newEle.setP4(reco::GsfElectron::P4_FROM_SUPER_CLUSTER, eleIt->p4(reco::GsfElectron::P4_FROM_SUPER_CLUSTER)*newEle.parentSuperCluster()->energy()/eleIt->parentSuperCluster()->energy(), eleIt->p4Error(reco::GsfElectron::P4_FROM_SUPER_CLUSTER), false);
			
			//-- update the correctedEcalEnergy... it does not make too much sense for re-recoes
//			newEle.setCorrectedEcalEnergy(eleIt->ecalEnergy() * (scRef->energy() / eleIt->p4(reco::GsfElectron::P4_FROM_SUPER_CLUSTER).energy()));
//			newEle.setCorrectedEcalEnergyError(eleIt->ecalEnergyError() * (scRef->energy() / eleIt->ecalEnergy()));

			newEle.full5x5_setShowerShape(calculateShowerShape_full5x5(newEle.superCluster(), recHits));
		} else {
			edm::LogError("Failed SC association") << "No SC to be associated to the electron";
		}
	}



#ifdef DEBUG
	std::cout << "Filled new electrons  " << pOutEle->size() << std::endl;
	std::cout << "Filled new electronsCore  " << pOutEleCore->size() << std::endl;
	//  std::cout << "Filled new endcapSC  " << pOutNewEndcapSC->size() << std::endl;
#endif

	// put result into the Event

	e.put(pOutEle);
	e.put(pOutEleCore);

	//  e.put(pOutNewEndcapSC);

}


reco::GsfElectron::ShowerShape ElectronRecalibSuperClusterAssociator::calculateShowerShape_full5x5( const reco::SuperClusterRef & theClus, 
																		  const EcalRecHitCollection* recHits)
{
	reco::GsfElectron::ShowerShape showerShape;

	const reco::CaloCluster & seedCluster = *(theClus->seed()) ;

	const CaloTopology * topology = _theCaloTopology.product() ;
	const CaloGeometry * geometry = _theGeometry.product() ;

	std::vector<float> covariances = noZS::EcalClusterTools::covariances(seedCluster,recHits,topology,geometry) ;
	std::vector<float> localCovariances = noZS::EcalClusterTools::localCovariances(seedCluster,recHits,topology) ;
	showerShape.sigmaEtaEta = sqrt(covariances[0]) ;
	showerShape.sigmaIetaIeta = sqrt(localCovariances[0]) ;
	if (!edm::isNotFinite(localCovariances[2])) showerShape.sigmaIphiIphi = sqrt(localCovariances[2]) ;
	showerShape.e1x5 = noZS::EcalClusterTools::e1x5(seedCluster,recHits,topology)  ;
	showerShape.e2x5Max = noZS::EcalClusterTools::e2x5Max(seedCluster,recHits,topology)  ;
	showerShape.e5x5 = noZS::EcalClusterTools::e5x5(seedCluster,recHits,topology) ;
	showerShape.r9 = noZS::EcalClusterTools::e3x3(seedCluster,recHits,topology)/theClus->rawEnergy() ;
	const float see_by_spp = showerShape.sigmaIetaIeta*showerShape.sigmaIphiIphi;
	if(  see_by_spp > 0 ) {
		showerShape.sigmaIetaIphi = localCovariances[1] / see_by_spp;
	} else if ( localCovariances[1] > 0 ) {
		showerShape.sigmaIetaIphi = 1.f;
	} else {
		showerShape.sigmaIetaIphi = -1.f;
	}
	showerShape.eMax          = noZS::EcalClusterTools::eMax(seedCluster,recHits);
	showerShape.e2nd          = noZS::EcalClusterTools::e2nd(seedCluster,recHits);
	showerShape.eTop          = noZS::EcalClusterTools::eTop(seedCluster,recHits,topology);
	showerShape.eLeft         = noZS::EcalClusterTools::eLeft(seedCluster,recHits,topology);
	showerShape.eRight        = noZS::EcalClusterTools::eRight(seedCluster,recHits,topology);
	showerShape.eBottom       = noZS::EcalClusterTools::eBottom(seedCluster,recHits,topology);
	
	// showerShape.hcalDepth1OverEcal = generalData_->hcalHelper->hcalESumDepth1(*theClus)/theClus->energy() ;
    // showerShape.hcalDepth2OverEcal = generalData_->hcalHelper->hcalESumDepth2(*theClus)/theClus->energy() ;
    // showerShape.hcalTowersBehindClusters = generalData_->hcalHelper->hcalTowersBehindClusters(*theClus) ;
    // showerShape.hcalDepth1OverEcalBc = generalData_->hcalHelper->hcalESumDepth1BehindClusters(showerShape.hcalTowersBehindClusters)/showerShape.e5x5 ;
    // showerShape.hcalDepth2OverEcalBc = generalData_->hcalHelper->hcalESumDepth2BehindClusters(showerShape.hcalTowersBehindClusters)/showerShape.e5x5 ;

	return showerShape;
}
DEFINE_FWK_MODULE(ElectronRecalibSuperClusterAssociator);
