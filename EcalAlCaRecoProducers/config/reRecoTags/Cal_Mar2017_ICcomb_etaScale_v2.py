import FWCore.ParameterSet.Config as cms
# Candidate GT with most updated conditions for legacy rereco of 2016 data
# it uncludes:
# - laser
# - pedestal run based from laser sequence in collisions
# - pulse shapes multi IOV
# - timing
# validation slides in the last AlCaDB meeting of December 2016
###
# timebased pedestal tag from laser sequence
# alpha changed to 1.5 for chinese crystals in EB
# ES MIP calibration with MIP measurement in Run F valid since Run D
# ESEE Intercalib compensating for drift in Run G and Run H
# v1 has the PN correction on the wrong side. Now fixed
##
# IC same as Cal_Mar2017_ref_v2 for EB and including IOVs from phiSym in EE:
# Run2016B, CD, EF, G, H
# transport factors from pizero for Run B, C+D, G, H. I've not used the values for Run E+F
# same os IC_comb_v2 but with eta scale

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016LegacyRepro_Candidate_v2'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_timestamp_2016"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS"),
                 ),
        cms.PSet(
            record = cms.string('EcalLaserAlphasRcd'),
            tag = cms.string('EcalLaserAlphas_EB_1.52Russian_1.5Chinese'),
            connect = cms.string('frontier://FrontierPrep/CMS_CONDITIONS')
            ),
        cms.PSet(record = cms.string("ESIntercalibConstantsRcd"),
                 tag = cms.string("ESIntercalibConstants_Run1_Run2_V07_offline"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(record = cms.string("ESEEIntercalibConstantsRcd"),
                 tag = cms.string("ESEEIntercalibConstants_Legacy2016_v3"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
                 ),
        cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                 tag = cms.string("EcalIntercalibConstants_Cal_Mar2017_eopPNcorrection_comb_etaScale_v2"),
                 connect = cms.string('frontier://FrontierPrep/CMS_CONDITIONS')
                 ),
        cms.PSet(record = cms.string("EcalLinearCorrectionsRcd"),
                 tag = cms.string("EcalLinearCorrections_from2011_offline"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS"),
                  ),
        ),
    )
