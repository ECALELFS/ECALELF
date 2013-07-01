import FWCore.ParameterSet.Config as cms


#met, mt cuts (for Wenu selection)
#W_ELECTRON_ET_CUT_MIN = 27.0
#MET_CUT_MIN = 20.
#MT_CUT_MIN = 50.


selectedElectrons = cms.EDFilter("GsfElectronRefSelector",
                                 src = cms.InputTag( 'gsfElectrons' ),
                                 cut = cms.string(
    "(abs(superCluster.eta)<2.5) && (energy*sin(superClusterPosition.theta)> 15)")
                                         )

PassingVeryLooseId = selectedElectrons.clone(
    cut = cms.string(
    selectedElectrons.cut.value() +
    " && (gsfTrack.trackerExpectedHitsInner.numberOfHits<=1)" #wrt std WP90 allowing 1 numberOfMissingExpectedHits
    " && ((isEB"
    " && ( dr03TkSumPt/p4.Pt <0.2 "#&& dr03EcalRecHitSumEt/p4.Pt < 0.3
    " && dr03HcalTowerSumEt/p4.Pt  < 0.2 )"
    " && (sigmaIetaIeta<0.01)"
    " && ( -0.15<deltaPhiSuperClusterTrackAtVtx<0.15 )"
    " && ( -0.007<deltaEtaSuperClusterTrackAtVtx<0.007 )"
    " && (hadronicOverEm<0.12)"
    ")"
    " || (isEE"
    " && ( dr03TkSumPt/p4.Pt <0.2"
    #&& dr03EcalRecHitSumEt/p4.Pt < 0.3
    " && dr03HcalTowerSumEt/p4.Pt  < 0.2 )"
    " && (sigmaIetaIeta<0.03)"
    " && ( -0.10<deltaPhiSuperClusterTrackAtVtx<0.10 )"
    " && ( -0.009<deltaEtaSuperClusterTrackAtVtx<0.009 )"
    " && (hadronicOverEm<0.10) "
    "))"
    )
    )


eleSelSeq = cms.Sequence(
    PassingVeryLooseId 
    )



ZeeSelector =  cms.EDProducer("CandViewShallowCloneCombiner",
                              decay = cms.string("PassingVeryLooseId PassingVeryLooseId"),
                              checkCharge = cms.bool(False),
                              cut   = cms.string("mass > 50")
                              )

# filter events with at least one Zee candidate as identified by the ZeeSelector
ZeeFilter = cms.EDFilter("CandViewCountFilter",
                         src = cms.InputTag("ZeeSelector"),
                         minNumber = cms.uint32(1)
                         )

ZeeFilterSeq = cms.Sequence(eleSelSeq * ZeeSelector * ZeeFilter)    

