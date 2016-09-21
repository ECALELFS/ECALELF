import FWCore.ParameterSet.Config as cms
# this rereco has also the laser tag with fixed TP PN corrections, the tags has still some problems.
# the limit at 0.05 is removed at set to 0.0001

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_Prompt_v11'),
    #toGet = cms.VPSet( ),   # hook to override or add single payloads
    toGet = cms.VPSet(
        # cms.PSet(record = cms.string("EcalPedestalsRcd"),
        #          tag = cms.string("EcalPedestals_2016"),
        #          connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        #          ),
        cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                 tag = cms.string("EcalPulseShapes_v03_offline"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        # cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
        #          tag = cms.string("ESIntercalibConstants_Run1_Run2_V05_offline"),
        #          connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ES_MIP/Cal_Jun2016/tags/sqlite/sqlite_esMIPs_offline_DBv2.db")
        #          ),
        # cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
        #          tag = cms.string("EcalIntercalibConstants_trans_2016vs2015_v1"),
        #          connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Jun2016/phiSym/tags/sqlite/EcalIntercalibConstants_trans_2016vs2015_v1.db")
        #                )
        cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
                 tag = cms.string("EcalADCToGeVConstant_2016_Bon_v2"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Sep2016/EcalADCToGeVConstant_2016_Bon_v2.db")
                 ),
        cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                 tag = cms.string("EcalLaserAPDPNRatios_offline_2016"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS")
                 ),
        # , cms.PSet(
        #     record = cms.string('EcalLaserAlphasRcd'),
        #     tag = cms.string('EcalLaserAlphas_test_prompt'),
        #     connect = cms.string('frontier://FrontierProd/CMS_COND_31X_ECAL')
        #    )
        ),
    )
