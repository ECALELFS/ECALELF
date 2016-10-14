from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferOutputs = True
config.General.requestName = 'AlCaElectron_alcareco_RUN2015D'
config.General.workArea = 'AlCaElectron_alcareco_RUN2015D'
config.section_('JobType')
config.JobType.psetName = 'reco_ALCA.py'
config.JobType.pluginName = 'Analysis'
#config.JobType.pyCfgParams = ['global_tag=74X_dataRun2_Prompt_v1', 'MC=False', 'isCrab=True']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ['PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt','PHYS14_25_V2_All_L2Relative_AK8PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt','PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt','PHYS14_25_V2_All_L2Relative_AK4PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt' ]
config.section_('Data')
config.Data.inputDataset = '/AlCaElectron/lbrianza-crab_reco_stream_runD-00000000000000000000000000000000/USER'
config.Data.unitsPerJob = 1
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt'
config.JobType.maxMemoryMB = 2500    # 2.5 GB       
config.JobType.maxJobRuntimeMin = 1400
#config.Data.lumiMask = 'Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
config.Data.inputDBS = 'phys03' #'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet'
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/AlCaElectron_alcareco_RUN2015D/'
config.Data.useParent = True
config.Data.publication = True
#config.Data.ignoreLocality = True
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.whitelist = ['T2_US_Caltech', 'T2_US_Florida', 'T2_US_MIT', 'T2_US_Nebraska', 'T2_US_Purdue', 'T2_US_UCSD', 'T2_US_Vanderbilt']
