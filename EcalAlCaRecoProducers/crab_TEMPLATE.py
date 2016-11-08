from CRABClient.UserUtilities import config
import FWCore.ParameterSet.Config as cms

config = config()

config.General.requestName = ''
config.General.workArea = 'crab_projects'
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles= [
    'ntuple.root'
    #'ntuple_numEvent1000.root'
]
config.JobType.psetName = 'python/alcaSkimming.py'
config.JobType.pyCfgParams = [
    'tagFile=config/reRecoTags/80X_dataRun2_Prompt_v12.py',
    'type=MINIAODNTUPLE',
    'doTreeOnly=1',
    'doTree=1',
    #'jsonFile=TestDontUse_13TeV_PromptReco_JSON_NoL1T.txt',
    #'jsonFile=RUN2016H-281613-282037_13TeV_PromptReco_JSON_NoL1T.txt',
    'jsonFile=RUN2016H-282092-284035_13TeV_PromptReco_JSON_NoL1T.txt',
    'isCrab=1',
    'isPrivate=0',
    'bunchSpacing=0']
config.JobType.inputFiles = [
    #'TestDontUse_13TeV_PromptReco_JSON_NoL1T.txt',
    #'RUN2016H-281613-282037_13TeV_PromptReco_JSON_NoL1T.txt'
    'RUN2016H-282092-284035_13TeV_PromptReco_JSON_NoL1T.txt'
]

config.JobType.allowUndistributedCMSSW = True
config.JobType.maxJobRuntimeMin = 2750
#config.JobType.sendPythonFolder = True

config.Data.inputDataset  = '/DoubleEG/Run2016H-PromptReco-v2/MINIAOD'
config.Data.inputDBS      = 'global'
config.Data.splitting     = 'FileBased' #'LumiBased'
config.Data.unitsPerJob   = 1 #30000
#config.Data.lumiMask      = 'TestDontUse_13TeV_PromptReco_JSON_NoL1T.txt'
config.Data.lumiMask      = 'RUN2016H-282092-284035_13TeV_PromptReco_JSON_NoL1T.txt'#'RUN2016H-281613-282037_13TeV_PromptReco_JSON_NoL1T.txt'
config.Data.runRange      = '282092-284035'#'281613-282037'
config.Data.outLFNDirBase = '/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/MINIAODNTUPLE/80X_dataRun2_Prompt_v12/DoubleEG-Run2016H-noSkim-Prompt_v2-miniAOD/282092-284035/'
config.Data.publication   = False

# GRID
config.Site.storageSite   =  'T2_CH_CERN'
#config.Site.whitelist     = ['T3_CH_CERN_CAF']
#config.Site.blacklist     = ['T1_US_FNAL','T2_UA_KIPT','T2_UK_London_Brunel','T2_CH_CSCS','T2_US_*']
#config.Site.ignoreGlobalBlacklist = True
