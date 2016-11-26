import FWCore.ParameterSet.Config as cms

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_mcRun2_asymptotic_2016_miniAODv2'),
    )

