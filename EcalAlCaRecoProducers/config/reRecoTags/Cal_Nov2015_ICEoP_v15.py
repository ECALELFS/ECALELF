import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
# Pulse shapes validated in Cal_Nov2015_PS_v1
# IC from EoP
# more details to be added here

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v2'),
                               toGet = cms.VPSet(
                                 cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                          tag = cms.string("EcalIntercalibConstants_Cal_Nov2015_EoP_v15"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Nov2015/eop/tags/db/EcalIntercalibConstants_Cal_Nov2015_EoP_v15.db"),
                                          ),
                                 cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                                          tag = cms.string("EcalPulseShapes_data"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/pulseShapes/Run2015-EOY/ecaltemplates_popcon_weekly.db"),
                                          ),
                                 cms.PSet(record = cms.string("EBAlignmentRcd"),
                                          tag = cms.string("EBAlignment_measured_v10_offline"),
                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
                                          ),
                                 cms.PSet(record = cms.string("EEAlignmentRcd"),
                                          tag = cms.string("EEAlignment_measured_v10_offline"),
                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
                                          ),
                                 cms.PSet(record = cms.string("ESAlignmentRcd"), # only Bon!
                                          tag = cms.string("ESAlignment_measured_v08_offline"),
                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
                                          ),
                               )
)
