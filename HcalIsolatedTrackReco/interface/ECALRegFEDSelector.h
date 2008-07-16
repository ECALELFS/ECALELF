#ifndef Calibration_ECALRegFEDSelector_h
#define Calibration_ECALRegFEDSelector_h

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"

#include "DataFormats/EcalDetId/interface/EcalDetIdCollections.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalEtaPhiRegion.h"

#include "DataFormats/EcalRawData/interface/EcalListOfFEDS.h"

class ECALRegFEDSelector : public edm::EDProducer {
 public:
  ECALRegFEDSelector(const edm::ParameterSet&);
  ~ECALRegFEDSelector();
  const EcalElectronicsMapping* ec_mapping;

  edm::InputTag seedLabel_;
  double delta_;
  bool fedSaved[1200];
  
  edm::InputTag rawInLabel_;
  
 private:
  virtual void beginJob(const edm::EventSetup&) ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
};

#endif

