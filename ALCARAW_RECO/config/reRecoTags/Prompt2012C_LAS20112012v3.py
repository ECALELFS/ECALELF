import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_P_V41::All'),
                               toGet = cms.VPSet(
                               cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                                          tag = cms.string("EcalLaserAPDPNRatios_data_20120814_2011-2012_v3"),
                                         connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
                                         )
                               ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
