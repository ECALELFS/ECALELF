import FWCore.ParameterSet.Config as cms

elPFIsoValueCharged03PFIdRecalib = cms.EDProducer('ValueMapTraslator',
                                                  inputCollection = cms.InputTag('elPFIsoValueCharged03PFIdGsf'),
                                                  outputCollection = cms.string(''),
                                                  referenceCollection = cms.InputTag('electronRecalibSCAssociator')
                                                  )

elPFIsoValueGamma03PFIdRecalib = cms.EDProducer('ValueMapTraslator',
                                                inputCollection = cms.InputTag('elPFIsoValueGamma03PFIdGsf'),
                                                outputCollection = cms.string(''),
                                                referenceCollection = cms.InputTag('electronRecalibSCAssociator')
                                                )

elPFIsoValueNeutral03PFIdRecalib = cms.EDProducer('ValueMapTraslator',
                                                inputCollection = cms.InputTag('elPFIsoValueNeutral03PFIdGsf'),
                                                outputCollection = cms.string(''),
                                                referenceCollection = cms.InputTag('electronRecalibSCAssociator')
                                                )
