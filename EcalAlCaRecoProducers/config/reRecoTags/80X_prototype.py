import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('80X_mcRun2_asymptotic_2016_miniAODv2'),
                               )

##This does NOT work
#import FWCore.ParameterSet.Config as cms
#
#from CondCore.CondDB.CondDB_cfi import * 
#
#RerecoGlobalTag = cms.ESSource("PoolDBESSource",
#                               CondDB,
#                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
#                               globaltag = cms.string('80X_mcRun2_asymptotic_2016_miniAODv2_v0'),
#                               )


#Exception Message:
#python encountered the error: <type 'exceptions.ValueError'>
#Duplicate insert of member connect
#The original parameters are:
#cms.ESSource("PoolDBESSource",
#    DBParameters = cms.PSet(
#        authenticationPath = cms.untracked.string(''),
#        authenticationSystem = cms.untracked.int32(0),
#        messageLevel = cms.untracked.int32(0),
#        security = cms.untracked.string('')
#    ),
#    connect = cms.string('')
#)

