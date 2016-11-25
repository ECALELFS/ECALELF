import FWCore.ParameterSet.Config as cms
# this rereco has also the laser tag with fixed TP PN corrections, the tags has still some problems.
# the limit at 0.05 is removed at set to 0.0001

from CondCore.ESSources.CondDBESSource_cfi import * 
#CondDBConnection.connect = cms.string( 'frontier://FrontierProd/CMS_CONDITIONS' )
RerecoGlobalTag = GlobalTag.clone(
    globaltag = cms.string('80X_dataRun2_2016SeptRepro_v3'),
    )
