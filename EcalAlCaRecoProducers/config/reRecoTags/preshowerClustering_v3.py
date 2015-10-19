import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

# this tag has the previous ESAlignment, prompt of Run2015AB as in _v2
# this has also the ECAL alignment in prompt for Run2015AB
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
                                 cms.PSet(record = cms.string("EBAlignmentRcd"), # only Bon!
                                          tag = cms.string("EBAlignment_IOV184519"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALtagValidation/EBAlignment_IOV184519.db"),
                                          ),
                                 cms.PSet(record = cms.string("EEAlignmentRcd"), # only Bon!
                                          tag = cms.string("EEAlignment_IOV184519"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALtagValidation/EEAlignment_IOV184519.db"),
                                          ),
                                 cms.PSet(record = cms.string("ESAlignmentRcd"), # only Bon!
                                          tag = cms.string("ESAlignment_IOV184519"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALtagValidation/ESAlignment_IOV184519.db"),
                                          ),
                               )
)
