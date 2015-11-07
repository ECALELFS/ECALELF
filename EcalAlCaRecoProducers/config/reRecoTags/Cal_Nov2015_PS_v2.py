import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
# new covariances  from lone bunch data in 3 IOVs
# more details to be added here

#The new covariances from data are looser than the previous ones, so the resolution seems 
#to lower a bit (even if it should be a bit more robust against input variations). 
#If you confirm this with your validation, then we would not update them, but only the pulse templates. 


RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_Prompt_v2'),
                               toGet = cms.VPSet(
                                 cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                          tag = cms.string("EcalIntercalibConstants_2012ABCD_offline"),
                                          connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL"),
                                          ),
#                                 cms.PSet(record = cms.string("EcalPulseShapesRcd"),
#                                          tag = cms.string("EcalPulseShapes_data"),
#                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/pulseShapes/Run2015-EOY/ecaltemplates_popcon_weekly.db"),
#                                          ),
                                cms.PSet(record = cms.string("EcalPulseCovariancesRcd"),
                                         tag = cms.string("EcalPulseCovariances_data"),
                                         connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/pulseShapes/Run2015-EOY/ecalcovariances_popcon_weekly.db"),
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
