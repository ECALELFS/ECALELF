import FWCore.ParameterSet.Config as cms
from CondCore.DBCommon.CondDBSetup_cfi import *

#### Please fill with comments
# Basic tag combination for 2012 Moriond conditions 
# Laser tags is fixed: no time to improve them
# For further studies
# Cal_Dic2012_AlphaPiZero_v1
# Cal_Dic2012_AlphaEle_v1
# Cal_Dic2012_AlphaEFlow_v1
# Cal_Dic2012_AlphaComb_v1
# Cal_Dic2012_AlphaComb_v2
# Cal_Dic2012_AlphaComb_v3
# Cal_Dic2012_AlphaComb_v2_ICEle_v1
# Cal_Dic2012_AlphaComb_v2_ICPiZero_v1
# Cal_Dic2012_AlphaComb_v2_ICPhiSym_v1
# Cal_Dic2012_AlphaComb_v2_ICComb_v1
# Cal_Dic2012_AlphaComb_v2_ICComb_v2
# Cal_Dic2012_AlphaComb_v2_ICComb_v3
#### No improvement given by AlphaStudies
# Start with this for IC validation
# tag = cms.string('EcalIntercalibConstants_V20120620_piZPhiSEtaScale2012_IOV2_AlphaStudies'),
# this tag is the one present in Prompt2012C   
# this is the reference of any IC test for 2012C

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'),
                               globaltag = cms.string('GR_P_V42B::All'),
                               toGet = cms.VPSet(
    cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
             tag = cms.string('EcalADCToGeVConstant_Bon_V20111129'),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
             ),
    cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
             tag = cms.string('EcalLaserAPDPNRatios_20121020_447_p1_v2'),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
             ),
    cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
             tag = cms.string('EcalLaserAlphas_EB_sic1_btcp152_EE_sic1_btcp116'),
             connect = cms.untracked.string('frontier://FrontierInt/CMS_COND_ECAL')
              ),
    cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
             tag = cms.string('EcalIntercalibConstants_V20121215_NLT_MeanPizPhiABC_EleABC_HR9EtaScABC_PhisymCor2_EoPCorAvg1'),
             connect = cms.untracked.string("frontier://FrontierInt/CMS_COND_ECAL")
             )
    ),
                               BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
                               )


