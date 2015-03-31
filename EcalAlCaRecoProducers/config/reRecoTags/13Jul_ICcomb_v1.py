import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

#### Please fill with comments
#
# combination of PhiSym + PiZero + Ele 2012 
# the weighted average of the errors estimated with the differential smearing
# on top of 13Jul rereco GT
#
#
#

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('FT_R_53_V6::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
             tag = cms.string('EcalIntercalibConstants_V20121025_2012CombWithDSMethodErrors'),
             connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
             )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )

