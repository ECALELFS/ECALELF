import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_P_V32::All'),
                               toGet = cms.VPSet(
                                 cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                                     tag = cms.string("EcalLaserAPDPNRatios_data_20120516_190380_193761_P_p1_tbc"),
                                     connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
                                     )
                                 ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
