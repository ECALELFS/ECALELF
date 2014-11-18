//--------------------------------------------------------------------------------------------------
// $Id $
//
// EGEnergyCorrector_fra
//
// Helper Class for applying regression-based energy corrections with optimized BDT implementation
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef EGAMMATOOLS_EGEnergyCorrector_fra_H
#define EGAMMATOOLS_EGEnergyCorrector_fra_H
    
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "RecoEgamma/EgammaTools/interface/EcalClusterLocal.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

class GBRForest;
//class EcalClusterLazyTools;

class EGEnergyCorrector_fra {
  public:
    EGEnergyCorrector_fra();
    ~EGEnergyCorrector_fra(); 

    void Initialize(const edm::EventSetup &iSetup, std::string regweights, bool weightsFromDB=false, bool ptSplit=false);
    Bool_t IsInitialized() const { return fIsInitialized; }
    
    std::pair<double,double> CorrectedEnergyWithErrorTracker(const reco::GsfElectron &e, const reco::VertexCollection& vtxcol, double rho, EcalClusterLazyTools &clustertools, const edm::EventSetup &es, bool ptSplit);
    
  protected:
    const GBRForest *fReadereb;
    const GBRForest *fReadereb_lowPt;
    const GBRForest *fReadereb_highPt;
    const GBRForest *fReaderebvariance;
    const GBRForest *fReaderee;
    const GBRForest *fReaderee_lowPt;
    const GBRForest *fReaderee_highPt;
    const GBRForest *fReadereevariance;      

    std::vector<std::string> *varnameseb;
    std::vector<std::string> *varnamesee;
    

    Bool_t fIsInitialized;
    Bool_t fOwnsForests;
    Float_t *fVals;
    Bool_t _GetCorrections;
    EcalClusterLocal _ecalLocal;
    
    };


#endif
