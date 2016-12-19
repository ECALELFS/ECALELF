import FWCore.ParameterSet.Config as cms
# Official GT for september re-reco
# pedestal tags produced by Jean for pedestal studies:
# 6a9a2818932fce79d8222768ba4f2ad3f60f894c payload is used (first Bon pedestal run of 2016, Apr)

# I have prepared the tag with 15 IOVs for the pulse shapes to be used by the multifit reconstruction covering the 2016 data-taking period to be used for the legacy re-reco.
# This is an update with 4 IOVs wr t the last ones (never used in the prompt-reco), documented with validation in this HN [1].
# These 4 additional IOVs cover Run2016H era. The timing was stable, so there is no need to have more granular IOVs, but still I divided the period in 4 with chunks of ~2-3 fb-1 of
# integrated luminosity each.

# The technical validation is done (links attached to  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02  2016-12-02 13:  EcalCondObjectContainer< 

# This should be testd with the ususal Z->ee re-reco with ECALELF.
# EcalPulseShape> 05:  be642b296faf51dea835b486c165a99a591e7d9d45.25536013:  Ecalcondobjectcontainer< 
#   283863 Ecalpulseshape> 05:  73ddb35e1efdb47f6e4de25f8b8c2d5add02008a44.21377713:  Ecalcondobjectcontainer< 
#   283171 Ecalpulseshape> 05:  bb3edcea2498ad48fb19a02f4e8b562df2d0f2e543.32989713:  Ecalcondobjectcontainer< 
#   282408 Ecalpulseshape> 05:  038d2080b1c556a5970928414a4a8764d184581842.42242713:  Ecalcondobjectcontainer< 
#   281010 Ecalpulseshape> 05:  a27b40ddaa99fe34908f2ec93dcf3b4bc738f4ba41.53864613:  Ecalcondobjectcontainer< 
#   279717 Ecalpulseshape> 05:  8e32fed534ea69b089f65d15c56c9e085871bd8540.08371913:  Ecalcondobjectcontainer< 
#   279474 Ecalpulseshape> 05:  207f05d40a0d0b2be2e274960866bf05a983402538.87385213:  Ecalcondobjectcontainer< 
#   278977 Ecalpulseshape> 05:  7df43ecd92a555fa12c41bbe4b657728bf8e3f1437.88257413:  Ecalcondobjectcontainer< 
#   278346 Ecalpulseshape> 05:  64d745a1fecf331c20207ff45560a4211997bfa936.81906613:  Ecalcondobjectcontainer< 
#   277932 Ecalpulseshape> 05:  5c2a3a69454cdf3df32d84782af23d1b6591776435.30114713:  Ecalcondobjectcontainer< 
#   276315 Ecalpulseshape> 05:  3bc574c9a7eddce037b5ac33b75e51254139a7b234.93146913:  Ecalcondobjectcontainer< 
#   275757 Ecalpulseshape> 05:  8d2e3964c3e5d01a26796ebf0ffc385d556fbc5134.56762413:  Ecalcondobjectcontainer< 
#   274958 Ecalpulseshape> 05:  e8fb4d5d50ccdb22c4947df681c6b3c9db092efb33.06022213:  Ecalcondobjectcontainer< 
#   274080 Ecalpulseshape> 05:  4ccd6df17d7702a2154d9aefcb40d51caa3f7dd532.71961113:  Ecalcondobjectcontainer< 
#   272818 Ecalpulseshape> 05:  28a707a2368e0e2512167dce02194cc206a4860a32.351461Each  Iov 
#   271983 Below)  --------------------------------------- And  ----------------------------------------Seem  --------------------------Fine [ 
# -----------2]
#   The  Sqlite  File  Containing  All  The  Iovs  Is  This  One:

#/afs/cern.ch/user/e/emanuele/w/public/ecal/pulseshapes_db/ecaltemplates_popcon_2016_legacy_calib.db

#   And  Payload The  Content  Is  The  Following:

#   Since:  Insertion TimeRun 

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v4'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_test_0_0"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                 tag = cms.string("EcalPulseShapes_2016_LEGACY_v1"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS"),
                 ),
        ),
    )
