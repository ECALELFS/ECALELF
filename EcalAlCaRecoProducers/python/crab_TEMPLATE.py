from CRABClient.UserUtilities import config
import FWCore.ParameterSet.Config as cms

config = config()

config.General.requestName  = ''
config.General.workArea     = 'crab_projects'
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles= [
    'ntuple.root'
]
config.JobType.psetName = 'python/alcaSkimming.py'
config.JobType.pyCfgParams = [
    'tagFile={GT}',
    'type={TYPE}',
    'doTreeOnly=1',
    'doTree=1',
    'jsonFile={JSONFILE}',
    'isCrab=1',
    'isPrivate=0',
    'bunchSpacing=0']
config.JobType.inputFiles = [
    '{JSONFILE}'
]

config.JobType.allowUndistributedCMSSW = True
config.JobType.maxJobRuntimeMin = 2750

config.Data.inputDataset  = '{DATASET}'
config.Data.inputDBS      = 'global'
config.Data.splitting     = 'FileBased'#'LumiBased'
config.Data.unitsPerJob   = 1 #30000
config.Data.lumiMask      = '{JSONFILE}'
config.Data.runRange      = '{RUNRANGE}'#'281613-282037'
config.Data.outLFNDirBase = '{EOSDIR}'
# file don't need to be published
config.Data.publication   = False

# GRID
if {RUNONCAF}:
    config.Site.whitelist = ['T3_CH_CERN_CAF']
    config.Site.blacklist = ['T1_US_FNAL',
                             'T2_UA_KIPT',
                             'T2_UK_London_Brunel',
                             'T2_CH_CSCS','T2_US_*']
    config.Site.ignoreGlobalBlacklist = True
else:
    config.Site.storageSite   =  'T2_CH_CERN'

