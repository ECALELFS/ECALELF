import FWCore.ParameterSet.Config as cms

HLTPath = "HLT_Ele*"
HLTProcessName = "HLT"

##    ____      __ _____ _           _                   
##   / ___|___ / _| ____| | ___  ___| |_ _ __ ___  _ __  
##  | |  _/ __| |_|  _| | |/ _ \/ __| __| '__/ _ \| '_ \ 
##  | |_| \__ \  _| |___| |  __/ (__| |_| | | (_) | | | |
##   \____|___/_| |_____|_|\___|\___|\__|_|  \___/|_| |_|
##  
#  GsfElectron ################ 

GsfMatchedPhotonCands = cms.EDProducer("ElectronMatchedCandidateProducer",
   src     = cms.InputTag("goodPhotons"),
   ReferenceElectronCollection = cms.untracked.InputTag("goodElectrons"),
   deltaR =  cms.untracked.double(0.3)
)

##    _____ _           _                     ___    _ 
##   | ____| | ___  ___| |_ _ __ ___  _ __   |_ _|__| |
##   |  _| | |/ _ \/ __| __| '__/ _ \| '_ \   | |/ _` |
##   | |___| |  __/ (__| |_| | | (_) | | | |  | | (_| |
##   |_____|_|\___|\___|\__|_|  \___/|_| |_| |___\__,_|
##   
# Electron ID  ######


                         
##    _____     _                         __  __       _       _     _             
##   |_   _| __(_) __ _  __ _  ___ _ __  |  \/  | __ _| |_ ___| |__ (_)_ __   __ _ 
##     | || '__| |/ _` |/ _` |/ _ \ '__| | |\/| |/ _` | __/ __| '_ \| | '_ \ / _` |
##     | || |  | | (_| | (_| |  __/ |    | |  | | (_| | || (__| | | | | | | | (_| |
##     |_||_|  |_|\__, |\__, |\___|_|    |_|  |_|\__,_|\__\___|_| |_|_|_| |_|\__, |
##                |___/ |___/                                                |___/ 
##   
# Trigger  ##################
PassingHLT = cms.EDProducer("trgMatchGsfElectronProducer",    
    InputProducer = cms.InputTag( 'gsfElectrons' ),                          
    hltTags = cms.untracked.string( HLTPath ),
    triggerEventTag = cms.untracked.InputTag("hltTriggerSummaryAOD","",HLTProcessName),
    triggerResultsTag = cms.untracked.InputTag("TriggerResults","",HLTProcessName)   
)

# WElecTagHLT = PassingHLT.clone(
#     InputProducer = cms.InputTag( "PassingWP80" )
#     )

# ele_sequence = cms.Sequence(
#     goodElectrons +
#     PassingWP80 +
#     WElecTagHLT
#     )


# import HLTrigger.HLTfilters.hltHighLevel_cfi
# WEnuHltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
#     throw = cms.bool(False),
#     HLTPaths = [HLTPath]
#     )

# elecMetSeq = cms.Sequence( WEnuHltFilter * ele_sequence * elecMetFilter )


selectedElectrons = cms.EDFilter("GsfElectronRefSelector",
                                 src = cms.InputTag( 'gsfElectrons' ),
                                 cut = cms.string(
    "(abs(superCluster.eta)<2.5) && (energy*sin(superClusterPosition.theta)> 15)")
                                         )
# This are the cuts at trigger level except ecalIso
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


############################################################
# Selectors
##############################
ZeeSelector =  cms.EDProducer("CandViewShallowCloneCombiner",
                              decay = cms.string("PassingVeryLooseId PassingVeryLooseId"),
                              checkCharge = cms.bool(False),
                              cut   = cms.string("mass > 40 && mass < 130")
                              )



#met, mt cuts for W selection
MT="sqrt(2*daughter(0).pt*daughter(1).pt*(1 - cos(daughter(0).phi - daughter(1).phi)))"
MET_CUT_MIN = 20.
W_ELECTRON_ET_CUT_MIN = 27.0
MT_CUT_MIN = 50.

WenuSelector = cms.EDProducer("CandViewShallowCloneCombiner",
    decay = cms.string("pfMet PassingVeryLooseId"), # charge coniugate states are implied
    checkCharge = cms.bool(False),                           
    cut   = cms.string(("daughter(0).pt > %f && daughter(1).pt > %f && "+MT+" > %f") % (MET_CUT_MIN, W_ELECTRON_ET_CUT_MIN, MT_CUT_MIN))
)


WZSelector = cms.EDProducer("CandMerger",
                            src = cms.VInputTag("WenuSelector", "ZeeSelector")
                            )

############################################################
# Filters
##############################
WenuFilter = cms.EDFilter("CandViewCountFilter",
                          src = cms.InputTag("WenuSelector"),
                          minNumber = cms.uint32(1)
                          )
# filter events with at least one Zee candidate as identified by the ZeeSelector
ZeeFilter = cms.EDFilter("CandViewCountFilter",
                         src = cms.InputTag("ZeeSelector"),
                         minNumber = cms.uint32(1)
                         )

WZFilter = cms.EDFilter("CandViewCountFilter",
                         src = cms.InputTag("WZSelector"),
                         minNumber = cms.uint32(1)
                         )

ZeeFilterSeq = cms.Sequence(eleSelSeq * ZeeSelector * ZeeFilter)    
WenuFilterSeq = cms.Sequence(eleSelSeq * WenuSelector * WenuFilter)
WZFilterSeq = cms.Sequence(eleSelSeq * WZFilter)
