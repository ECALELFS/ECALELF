import FWCore.ParameterSet.Config as cms

elPFIsoValueCharged03PFIdRecalib = cms.EDProducer('ValueMapTraslator',
                                                  inputCollection = cms.InputTag('elPFIsoValueCharged03PFId'),
                                                  outputCollection = cms.untracked.string(''),
                                                  referenceCollection = cms.InputTag('electronRecalibSCAssociator')
                                                  )

