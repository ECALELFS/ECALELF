import FWCore.ParameterSet.Config as cms

#from CondCore.DBCommon.CondDBSetup_cfi import * #==> deprecated

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
                               globaltag = cms.string('80X_dataRun2_Prompt_v8'),
                               #toGet = cms.VPSet( ),   # hook to override or add single payloads
                               toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                 tag = cms.string("EcalPulseShapes_data"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/pulseShapes/Cal_Jun2016/tags/sqlite/ecaltemplates_popcon_data_Run2016B_since_271983.db")
                 ),
        cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
                 tag = cms.string("ESIntercalibConstants_Run1_Run2_V05_offline"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ES_MIP/Cal_Jun2016/tags/sqlite/sqlite_esMIPs_offline_DBv2.db")
                 ),
        cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                 tag = cms.string("EcalIntercalibConstants_2016B_v1"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Jun2016/pizero/tags/sqlite/EcalIntercalibConstants_2016B_v1.db")
                 )
        #  ,cms.PSet(record = cms.string("EcalADCToGeVConstantRcd"),
        #          tag = cms.string("EcalADCToGeVConstant_v10_offline"),
        #          connect = cms.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
        #         )
        # ,cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
        #           tag = cms.string("EcalLaserAPDPNRatios_2011fit_noVPT_nolim_online"),
        #           tag = cms.string("EcalLaserAPDPNRatios_test_20110625"),
        #           tag = cms.string("EcalLaserAPDPNRatios_2011V3_online"),
        #           tag = cms.string("EcalLaserAPDPNRatios_2011mixfit_online"),
        #          connect = cms.string("frontier://FrontierPrep/CMS_COND_ECAL")
        #          )
        # , cms.PSet(
        #     record = cms.string('EcalLaserAlphasRcd'),
        #     tag = cms.string('EcalLaserAlphas_test_prompt'),
        #     connect = cms.string('frontier://FrontierProd/CMS_COND_31X_ECAL')
        #    )
        ),
                               )
