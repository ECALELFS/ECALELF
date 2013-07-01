#include "calibration/ALCARAW_RECO/interface/PUDumper.h"



//! ctor
PUDumper::PUDumper(const edm::ParameterSet& iConfig)
{
  MCPileupTag_ = iConfig.getParameter<edm::InputTag>("MCPileupTag");
  
  // create TTree
  edm::Service<TFileService> fs;
  PUTree_ = fs -> make<TTree>("pileup","pileup");
  
  PUTree_ -> Branch("runNumber",&runNumber_,"runNumber/I");
  PUTree_ -> Branch("BX",       &BX_,              "BX/I");
  PUTree_ -> Branch("nPUtrue",  &nPUtrue_,    "nPUtrue/I");
  PUTree_ -> Branch("nPUobs",   &nPUobs_,      "nPUobs/I");
}

// ----------------------------------------------------------------



//! dtor
PUDumper::~PUDumper()
{}

// ----------------------------------------------------------------



//! loop over the reco particles and count leptons
void PUDumper::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // get the PU collection
  edm::Handle<std::vector<PileupSummaryInfo> > PupInfo;
  iEvent.getByLabel(MCPileupTag_,PupInfo);
  
  
  // loop on BX
  std::vector<PileupSummaryInfo>::const_iterator PVI;
  for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI)
  {
    runNumber_ = iEvent.id().run();
    BX_        = PVI -> getBunchCrossing();
    nPUtrue_   = PVI -> getTrueNumInteractions();
    nPUobs_    = PVI -> getPU_NumInteractions();
    
    std::cout << "PUDumper::runNumber: " << runNumber_
              << "   BX: "      << BX_
              << "   nPUtrue: " << nPUtrue_
              << "   nPUobs: "  << nPUobs_
              << std::endl;
    
    PUTree_ -> Fill();
  }    
}

DEFINE_FWK_MODULE(PUDumper);
