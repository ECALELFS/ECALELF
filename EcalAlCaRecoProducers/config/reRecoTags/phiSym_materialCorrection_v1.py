import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

# rereco with tag of prompt + IC derived on 2012C data with material corrections on top of EcalIntercalibConstants_2012ABCD_offline
# it should give a resolution worse than the prompt tag because the etaScale is not updated and any systematic from the phiSym is in (not ratios of phiSym IOVs)

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v2'),
                               toGet = cms.VPSet(
                                 cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                          tag = cms.string("Run2015ABC_ICphiSym_2012C_v1"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/phiSym_materialCorrection/Run2015ABC_ICphiSym_2012C_v1.db"),
                                          ),
                                 cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                                          tag = cms.string("EcalPulseShapes_v01_offline"),
                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
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


# RerecoGlobalTag.toGet.append(
#     cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
#              )
#     )

# RerecoGlobalTag = cms.ESSource("PoolDBESSource",
#                                CondDBSetup,
#                                connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
#                                globaltag = cms.string('74X_dataRun2_Prompt_v2'),
#                                toGet = cms.VPSet(
#                                  cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
#                                           tag = cms.string("Run2015ABC_ICphiSym_2012C_v1"),
#                                           connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/phiSym_materialCorrection/Run2015ABC_ICphiSym_2012C_v1.db"),
#                                           ),
#                                  cms.PSet(record = cms.string("EcalPulseShapesRcd"),
#                                           tag = cms.string("EcalPulseShapes_v01_offline"),
#                                           connect = cms.untracked.string("frontier://FrontierProd/CMS_CONDITIONS"),
#                                           ),
#                                  )
# )
