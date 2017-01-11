import FWCore.ParameterSet.Config as cms
# Candidate GT with most updated conditions for legacy rereco of 2016 data
# it uncludes:
# - laser
# - pedestal run based from laser sequence in collisions
# - pulse shapes multi IOV
# - timing
# validation slides in the last AlCaDB meeting of December 2016
# timebased pedestal tag from laser sequence

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016LegacyRepro_Candidate_v2'),
    toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_timestamp_2016"),
                 connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS"),
#                 timetype = cms.string('timestamp'),
                 ),
        ),
    )
