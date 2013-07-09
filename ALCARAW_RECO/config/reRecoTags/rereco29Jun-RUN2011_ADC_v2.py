import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *
###
### Comments
### Best 2011 Tag until now: 29 Jun 2012
### It is based on 16Jan rereco (GT)
### new IC with phisym + piZero + electron + 2010
### fixed scale (but to be checked)
### static alpha 1.16
###
###
RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_R_42_V24::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
             tag = cms.string("EcalIntercalibConstants_V20120613_PhiSymmetryPiZeroElectron2010V3Combination_EtaScaleAllR9"),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
             ),
    cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             tag = cms.string("EcalLaserAPDPNRatios_data_20120131_158851_183320"),
             connect = cms.untracked.string("frontier://PromptProd/CMS_COND_42X_ECAL_LAS")
             ),
    cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
             tag = cms.string('EcalLaserAlphas_EB_sic1_btcp152_EE_sic1_btcp116'),
             connect = cms.untracked.string('frontier://FrontierInt/CMS_COND_ECAL')
             ),
    cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
              tag = cms.string("EcalADCToGeVConstant_Bon_EB98EE98"),
              connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
              )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )
