import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

# rereco of cycle Run2015B_WF2
# other infos

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v0'),
                               toGet = cms.VPSet(
                                     cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                              tag = cms.string("EcalIntercalibConstants_transport2012to2015_V7"),
                                              connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Run2015B_WF2/EcalIntercalibConstants_transport2012to2015_V7.db")
                                              )
                                     ),
                               )
