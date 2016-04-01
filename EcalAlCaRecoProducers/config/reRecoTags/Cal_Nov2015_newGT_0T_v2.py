import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

#candidate used for 0T rereco without updating ECAL conditions
# + scaled IC with Bon/Boff corrections and scaled ADCtoGeV
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('74X_dataRun2_2015EOY_Candidate_2016_01_17_21_50_54'),
                               toGet = cms.VPSet(
                                 cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                                          tag = cms.string("EcalIntercalibConstants_2015_Boff"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Nov2015/combinations/tags/db/EcalIntercalibConstants_2015_Boff.db"),
                                          ),
                                 cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
                                          tag = cms.string("EcalADCToGeVConstant_2015_Boff"),
                                          connect = cms.untracked.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Nov2015/ADCtoGeV/tags/db/EcalADCToGeVConstant_2015_Boff.db"),
                                          ),

                                 )
                               )
