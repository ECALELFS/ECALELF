import FWCore.ParameterSet.Config as cms

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016LegacyRepro_Candidate_v2'),
    toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('EcalLaserAlphasRcd'),
            tag = cms.string('EcalLaserAlphas_EFlow_2sigma'),
            connect = cms.string('frontier://FrontierPrep/CMS_CONDITIONS')
            ),
        ),
    )
