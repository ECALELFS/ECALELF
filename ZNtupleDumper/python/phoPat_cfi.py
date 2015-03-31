# this module produces standard patPhotons
# #------------------------------
from PhysicsTools.PatAlgos.mcMatchLayer0.photonMatch_cfi import *
from PhysicsTools.PatAlgos.producersLayer1.photonProducer_cfi import *

patPhotons.photonSource = cms.InputTag("gedPhotons")
patPhotons.addPhotonID = cms.bool(False)
patPhotons.addGenMatch = cms.bool(True)
patPhotons.reducedBarrelRecHitCollection = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
patPhotons.reducedEndcapRecHitCollection = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")

