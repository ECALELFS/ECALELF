import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
###
### Comments
### TESTING THE IC!
### Using last laser tag available for 2011
### this laser tag has been used to derive the ICs
###
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_R_42_V24::All'),
                               toGet = cms.VPSet(cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                                          tag = cms.string("EcalIntercalibConstants_V20120429_PiZeroEtaSepOct2011_EnergyFlow3200TimeCorr"),
                                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                                                          ),
                                                 cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                                                          tag = cms.string("EcalLaserAPDPNRatios_data_20120131_158851_183320"),
                                                          connect = cms.untracked.string("frontier://PromptProd/CMS_COND_42X_ECAL_LAS")
                                                          ),
                                                 cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
                                                          tag = cms.string('EcalLaserAlphas_test_prompt'),
                                                          connect = cms.untracked.string('frontier://FrontierProd/CMS_COND_31X_ECAL')
                                                          )
                                                 ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
