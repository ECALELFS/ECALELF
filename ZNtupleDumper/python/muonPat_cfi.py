# this module produces standard patMuons
# #------------------------------
from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import *
from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cfi import *

patMuons.muonSource = cms.InputTag("muons")
