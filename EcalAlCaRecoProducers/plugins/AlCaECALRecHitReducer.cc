#include "Calibration/EcalAlCaRecoProducers/plugins/AlCaECALRecHitReducer.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/SiStripElectron.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/EcalAlgo/interface/EcalPreshowerGeometry.h"
#include "Geometry/CaloTopology/interface/EcalPreshowerTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
//#include "Geometry/CaloTopology/interface/EcalBarrelTopology.h"

//#define ALLrecHits
//#define DEBUG

//#define QUICK -> if commented loop over the recHits of the SC and add them to the list of recHits to be saved
//                 comment it if you want a faster module but be sure the window is large enough

AlCaECALRecHitReducer::AlCaECALRecHitReducer(const edm::ParameterSet& iConfig) 
{

  ebRecHitsLabel_ = iConfig.getParameter< edm::InputTag > ("ebRecHitsLabel");
  eeRecHitsLabel_ = iConfig.getParameter< edm::InputTag > ("eeRecHitsLabel");
  //  esRecHitsLabel_ = iConfig.getParameter< edm::InputTag > ("esRecHitsLabel");
  electronLabel_ = iConfig.getParameter< edm::InputTag > ("electronLabel");
  EESuperClusterCollection_ = iConfig.getParameter< edm::InputTag>("EESuperClusterCollection");
  minEta_highEtaSC_         = iConfig.getParameter< double >("minEta_highEtaSC");

  alcaBarrelHitsCollection_ = iConfig.getParameter<std::string>("alcaBarrelHitCollection");
  alcaEndcapHitsCollection_ = iConfig.getParameter<std::string>("alcaEndcapHitCollection");
  
  //  alcaPreshowerHitsCollection_ = iConfig.getParameter<std::string>("alcaPreshowerHitCollection");
  
  etaSize_ = iConfig.getParameter<int> ("etaSize");
  phiSize_ = iConfig.getParameter<int> ("phiSize");
  // FIXME: minimum size of etaSize_ and phiSize_
  if ( phiSize_ % 2 == 0 ||  etaSize_ % 2 == 0)
    edm::LogError("AlCaECALRecHitReducerError") << "Size of eta/phi should be odd numbers";
 
  //  esNstrips_  = iConfig.getParameter<int> ("esNstrips");
  //  esNcolumns_ = iConfig.getParameter<int> ("esNcolumns");
  
  //register your products
  produces< EBRecHitCollection > (alcaBarrelHitsCollection_) ;
  produces< EERecHitCollection > (alcaEndcapHitsCollection_) ;
  //  produces< ESRecHitCollection > (alcaPreshowerHitsCollection_) ;
}


AlCaECALRecHitReducer::~AlCaECALRecHitReducer()
{}


// ------------ method called to produce the data  ------------
void
AlCaECALRecHitReducer::produce (edm::Event& iEvent, 
                                const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  
  EcalRecHitCollection::const_iterator recHit_itr;

  // get the ECAL geometry:
  ESHandle<CaloGeometry> geoHandle;
  iSetup.get<CaloGeometryRecord>().get(geoHandle);

  edm::ESHandle<CaloTopology> theCaloTopology;
  iSetup.get<CaloTopologyRecord>().get(theCaloTopology);
  const CaloTopology *caloTopology = theCaloTopology.product();
  
  // Get GSFElectrons
  Handle<reco::GsfElectronCollection> pElectrons;
  iEvent.getByLabel(electronLabel_, pElectrons);
  //if (!pElectrons.isValid())  return ;
  
  const reco::GsfElectronCollection * electronCollection = pElectrons.product();
  
  // get RecHits
  Handle<EBRecHitCollection> barrelRecHitsHandle;
  iEvent.getByLabel(ebRecHitsLabel_,barrelRecHitsHandle);
//   if (!barrelRecHitsHandle.isValid()) {
//     edm::LogError ("reading") << ebRecHitsLabel_ << " not found" ; 
//     barrelIsFull = false ;
//   }
  
  const EBRecHitCollection *barrelHitsCollection = barrelRecHitsHandle.product () ;
  
  // get RecHits
  Handle<EERecHitCollection> endcapRecHitsHandle;    
  iEvent.getByLabel(eeRecHitsLabel_,endcapRecHitsHandle);
//   if (!endcapRecHitsHandle.isValid()) {
//     edm::LogError ("reading") << eeRecHitsLabel_ << " not found" ; 
//     endcapIsFull = false ;
//   }
  
  const EERecHitCollection *endcapHitsCollection = endcapRecHitsHandle.product () ;

  //  const EERecHitCollection * endcapHitsCollection = endcapRecHitsHandle.product();
  
  //   // get ES RecHits
  //   Handle<ESRecHitCollection> preshowerRecHitsHandle;
  //   bool preshowerIsFull = true ;
  
  //   iEvent.getByLabel(esRecHitsLabel_,preshowerRecHitsHandle);
  //   if (!preshowerRecHitsHandle.isValid()) {
  //     edm::LogError ("reading") << esRecHitsLabel_ << " not found" ; 
  //     preshowerIsFull = false ;
  //   }
  
  //   const ESRecHitCollection * preshowerHitsCollection = 0 ;
  //   if (preshowerIsFull)  
  //     preshowerHitsCollection = preshowerRecHitsHandle.product () ;

  //   // make a vector to store the used ES rechits:
  //   set<ESDetId> used_strips;
  //   used_strips.clear();

  Handle<reco::SuperClusterCollection> EESCHandle;
  iEvent.getByLabel(EESuperClusterCollection_, EESCHandle);
 
  //Create empty output collections
  std::auto_ptr< EBRecHitCollection > miniEBRecHitCollection (new EBRecHitCollection) ;
  std::auto_ptr< EERecHitCollection > miniEERecHitCollection (new EERecHitCollection) ;  
  //  std::auto_ptr< ESRecHitCollection > miniESRecHitCollection (new ESRecHitCollection) ;  

  std::set<DetId> reducedRecHit_EBmap;
  std::set<DetId> reducedRecHit_EEmap;

  
  for (reco::GsfElectronCollection::const_iterator eleIt=electronCollection->begin(); eleIt!=electronCollection->end(); eleIt++) {
    // barrel
    const reco::SuperCluster& sc = *(eleIt->superCluster()) ;

    if (eleIt->isEB()) {
      AddMiniRecHitCollection(sc, reducedRecHit_EBmap, caloTopology);
    } else { // endcap
      AddMiniRecHitCollection(sc, reducedRecHit_EEmap, caloTopology);
    } // end of endcap
  }


  //saving recHits for highEta SCs for highEta studies
  for(reco::SuperClusterCollection::const_iterator SC_iter = EESCHandle->begin();
      SC_iter!=EESCHandle->end();
      SC_iter++){
    if(fabs(SC_iter->eta()) < minEta_highEtaSC_) continue;
    AddMiniRecHitCollection(*SC_iter, reducedRecHit_EEmap, caloTopology);
  }

  //------------------------------ fill the alcareco reduced recHit collection
  for(std::set<DetId>::const_iterator itr = reducedRecHit_EBmap.begin();
      itr != reducedRecHit_EBmap.end(); itr++){
    if (barrelHitsCollection->find(*itr) != barrelHitsCollection->end())
      miniEBRecHitCollection->push_back(*(barrelHitsCollection->find(*itr)));
  }

  for(std::set<DetId>::const_iterator itr = reducedRecHit_EEmap.begin();
      itr != reducedRecHit_EEmap.end(); itr++){
    if (endcapHitsCollection->find(*itr) != endcapHitsCollection->end())
      miniEERecHitCollection->push_back(*(endcapHitsCollection->find(*itr)));
  }

  //--------------------------------------- Put selected information in the event
  iEvent.put( miniEBRecHitCollection,alcaBarrelHitsCollection_ );
  iEvent.put( miniEERecHitCollection,alcaEndcapHitsCollection_ );     
  //  iEvent.put( miniESRecHitCollection,alcaPreshowerHitsCollection_ );     
}

void AlCaECALRecHitReducer::AddMiniRecHitCollection(const reco::SuperCluster& sc,
						    std::set<DetId>& reducedRecHitMap,
						    const CaloTopology *caloTopology					    
						    ){
  DetId seed=(sc.seed()->seed());
  int phiSize=phiSize_, etaSize=etaSize_;
  if(seed.subdetId()!=EcalBarrel){ // if not EB, take a square window
    etaSize= std::max(phiSize_,etaSize_);
    phiSize=etaSize;
  }

  std::vector<DetId> recHit_window = caloTopology->getWindow(seed, phiSize, etaSize);
  for(unsigned int i =0; i < recHit_window.size(); i++){
    reducedRecHitMap.insert(recHit_window[i]);
  }

  const std::vector< std::pair<DetId, float> > & scHits = sc.hitsAndFractions();
  for(std::vector< std::pair<DetId, float> >::const_iterator scHit_itr = scHits.begin(); 
      scHit_itr != scHits.end(); scHit_itr++){
    // the map fills just one time (avoiding double insert of recHits)
    reducedRecHitMap.insert(scHit_itr->first);
  }


  return;
}


DEFINE_FWK_MODULE(AlCaECALRecHitReducer);

