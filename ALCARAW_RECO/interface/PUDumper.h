#ifndef PUDumper_h
#define PUDumper_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "TTree.h"



class PUDumper : public edm::EDAnalyzer
{
 public:
  
  //! ctor
  explicit PUDumper(const edm::ParameterSet&);
  
  //! dtor 
  ~PUDumper();
  
  
  
 private:
  
  //! the actual analyze method 
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  
  
  
 private:
  
  edm::InputTag MCPileupTag_;
  
  TTree* PUTree_;
  
  int runNumber_;
  int BX_;
  int nPUtrue_;
  int nPUobs_;
};

#endif
