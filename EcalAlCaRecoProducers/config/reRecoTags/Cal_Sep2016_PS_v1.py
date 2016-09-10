import FWCore.ParameterSet.Config as cms
# this rereco has 51 IOVs in the pedestal tag produced by Jean:
# created a special Ecal Pedestals tag with all the payloads extracted from MiniDAQ runs with their IOV corresponding to these runs (to be
# fully correct, we also put the Boff payload for July 1st to 2nd).
#   Tha tag EcalPedestals_2016 with 59 IOVs (first one : 1 with Boff payload) has been uploaded in the production DB.
# this rereco has also the laser tag with fixed TP PN corrections, the tags has  Still  Some  Problems.

#  This  Rereco  Has  Also  The  Payload Pulse  Shapes  Provided  By  Emanuele:
#Since: Run   Insertion Time              Payload                                   Object Type                              
#-----------  --------------------------  ----------------------------------------  ---------------------------------------  
#271983       2016-09-09 10:55:01.683878  28a707a2368e0e2512167dce02194cc206a4860a  EcalCondObjectContainer<EcalPulseShape>  
#272818       2016-09-09 10:55:02.008839  4ccd6df17d7702a2154d9aefcb40d51caa3f7dd5  EcalCondObjectContainer<EcalPulseShape>  
#274080       2016-09-09 10:55:02.325229  e8fb4d5d50ccdb22c4947df681c6b3c9db092efb  EcalCondObjectContainer<EcalPulseShape>  
#274958       2016-09-09 10:55:03.474915  8d2e3964c3e5d01a26796ebf0ffc385d556fbc51  EcalCondObjectContainer<EcalPulseShape>  
#275757       2016-09-09 10:55:03.795796  3bc574c9a7eddce037b5ac33b75e51254139a7b2  EcalCondObjectContainer<EcalPulseShape>  
#276315       2016-09-09 10:55:04.069530  5c2a3a69454cdf3df32d84782af23d1b65917764  EcalCondObjectContainer<EcalPulseShape>  
#277932       2016-09-09 10:55:05.545168  64d745a1fecf331c20207ff45560a4211997bfa9  EcalCondObjectContainer<EcalPulseShape>  
#278346       2016-09-09 10:55:07.029847  7df43ecd92a555fa12c41bbe4b657728bf8e3f14  EcalCondObjectContainer<EcalPulseShape>  
#278977       2016-09-09 10:55:08.146323  207f05d40a0d0b2be2e274960866bf05a9834025  EcalCondObjectContainer<EcalPulseShape>  
#279474       2016-09-09 10:55:09.765139  8e32fed534ea69b089f65d15c56c9e085871bd85  EcalCondObjectContainer<EcalPulseShape>  
#279717       2016-09-09 10:55:10.968237  a27b40ddaa99fe34908f2ec93dcf3b4bc738f4ba  EcalCondObjectContainer<EcalPulseShape>  


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
                  tag = cms.string("EcalPulseShapes_data"),
                  connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/pulseShapes/Cal_Sep2016/tags/sqlite/ecaltemplates_popcon_2016_calib.db")
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
                 tag = cms.string("EcalADCToGeVConstant_2016_Bon"),
                 connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/RunII-IC/Cal_Jun2016/EcalADCToGeVConstant_2016_Bon.db")
                 ),
        cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                 tag = cms.string("EcalLaserAPDPNRatios_TestPulsetest"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS")
                 ),
        # , cms.PSet(
        #     record = cms.string('EcalLaserAlphasRcd'),
        #     tag = cms.string('EcalLaserAlphas_test_prompt'),
        #     connect = cms.string('frontier://FrontierProd/CMS_COND_31X_ECAL')
        #    )
        ),
    )
