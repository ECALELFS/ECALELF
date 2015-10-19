import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
#conddb list 74X_dataRun2_Prompt_v2 | grep ESAlign
#conddb list ESAlignment_measured_v01_express 
#conddb_import -f frontier://FrontierProd/CMS_CONDITIONS -i ESAlignment_measured_v01_express -c sqlite:ESAlignment_IOV184519.db -t ESAlignment_IOV184519 -b 184519 -e 253983

# this tag has the previous ESAlignment, prompt of Run2015AB
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v2'),
                               toGet = cms.VPSet(
                                 cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                          tag = cms.string("EcalIntercalibConstants_2012ABCD_offline"),
                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL"),
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
                                          tag = cms.string("ESAlignment_IOV184519"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALtagValidation/ESAlignment_IOV184519.db"),
                                          ),
                               )
)
