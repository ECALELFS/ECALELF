# this module produces standard patElectrons
# #------------------------------
from PhysicsTools.PatAlgos.mcMatchLayer0.electronMatch_cfi import *
from PhysicsTools.PatAlgos.producersLayer1.electronProducer_cfi import *

patElectrons.electronSource = cms.InputTag("gedGsfElectrons")
patElectrons.useParticleFlow  =  cms.bool( False )
patElectrons.addElectronID = cms.bool(False)
patElectrons.addGenMatch = cms.bool(True)
patElectrons.pvSrc = cms.InputTag("offlinePrimaryVertices")
#patElectrons.reducedBarrelRecHitCollection = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
#patElectrons.reducedEndcapRecHitCollection = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
patElectrons.reducedBarrelRecHitCollection = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
patElectrons.reducedEndcapRecHitCollection = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")
patElectrons.embedPFCandidate     = cms.bool(True)  ## embed in AOD externally stored particle flow candidate
patElectrons.embedTrack           = cms.bool(False) ## embed in AOD externally stored track (note: gsf electrons don't have a track), make it point to reducedtrack collection


# #------------------------------ trigger information producer
from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi import *
#patTrigger

# #------------------------------ trigger matching
#from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi.py import *
PatElectronTriggerMatchHLTEle_Ele20SC4Mass50v7 = cms.EDProducer(
    "PATTriggerMatcherDRDPtLessByR"                 # match by DeltaR only, best match by DeltaR
    , src     = cms.InputTag( "patElectrons" )
    , matched = cms.InputTag( "patTrigger" )          # default producer label as defined in PhysicsTools/PatAlgos/python/triggerLayer1/triggerProducer_cfi.py
    , matchedCuts = cms.string( 'path( "HLT_Ele20_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC4_Mass50_v7" )' )
    , maxDPtRel = cms.double( 0.5 )
    , maxDeltaR = cms.double( 0.5 )
    , resolveAmbiguities    = cms.bool( True )        # only one match per trigger object
    , resolveByMatchQuality = cms.bool( True )        # take best match found per reco object: by DeltaR here (s. above)
    )


# #------------------------------ trigger matching embedder
# PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatchEmbedder_cfi.py
# Embedding in electrons
PatElectronsTriggerMatch = cms.EDProducer(
      "PATTriggerMatchElectronEmbedder"
      , src     = cms.InputTag( "PatElectrons" )
      , matches = cms.VInputTag(
    'PatElectronTriggerMatchHLTEle_Ele20SC4Mass50v7'
    )
      )

# #------------------------------ trigger event producer
# PhysicsTools/PatAlgos/python/triggerLayer1/triggerEventProducer_cfi.py
# patTriggerEvent
from PhysicsTools.PatAlgos.triggerLayer1.triggerEventProducer_cfi import *
patTriggerEvent.patTriggerMatches = cms.VInputTag( 'PatElectronsTriggerMatch' )

