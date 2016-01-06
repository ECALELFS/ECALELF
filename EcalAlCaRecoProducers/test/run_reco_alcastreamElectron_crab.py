from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferOutputs = True
config.General.requestName = 'reco_stream_runD'
config.General.workArea = 'reco_stream_runD'
config.section_('JobType')
config.JobType.psetName = 'EcalAlCaRecoProducers/test/reco_RAW2DIGI_RECO_alcastreamElectron.py'
config.JobType.pluginName = 'Analysis'
config.JobType.pyCfgParams = ['tagFile=/afs/cern.ch/user/l/lbrianza/work/public/EoPtest_december2015/ICset_multifit_20loop_2015/Cal_Dec2015_ICEoP_ref2.py']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ['PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt','PHYS14_25_V2_All_L2Relative_AK8PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt','PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt','PHYS14_25_V2_All_L2Relative_AK4PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt' ]
config.section_('Data')
#config.Data.inputDataset = '/ElectronStream/Run2015B-v1/RAW'
config.Data.inputDataset = '/AlCaElectron/Run2015D-v1/RAW'
config.Data.unitsPerJob = 30
#config.Data.lumiMask = 'json/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
config.Data.inputDBS = 'global' #'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet'
config.Data.splitting = 'LumiBased'
config.Data.outLFNDirBase = '/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/AlCaElectron-Run2015D-v1-RECO/'
config.Data.publication = True
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
config.Site.whitelist = ['T2_CH_CERN']
