import FWCore.ParameterSet.Config as cms
# Candidate GT with most updated conditions for legacy rereco of 2016 data

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016Repro_OnlyForVerification'),
    )
