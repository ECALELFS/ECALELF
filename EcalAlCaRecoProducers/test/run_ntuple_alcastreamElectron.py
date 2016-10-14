from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferOutputs = True
config.General.requestName = 'ntuple_AlCaElectron_RUN2015D'
config.General.workArea = 'ntuple_AlCaElectron_RUN2015D'
config.section_('JobType')
config.JobType.psetName = 'EcalAlCaRecoProducers/python/alcaSkimming.py'
#config.JobType.psetName = 'processDump.py'
config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles= ['ntuple.root','extraCalibTree.root']
config.JobType.pyCfgParams = ['isCrab=1', 'skim=WSkim', 'maxEvents=-1', 'type=ALCARECO', 'tagFile=EcalAlCaRecoProducers/config/reRecoTags/test75x.py','doTreeOnly=1', 'electronStream=1','doTree=3']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ['PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt','PHYS14_25_V2_All_L2Relative_AK8PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt','PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt','PHYS14_25_V2_All_L2Relative_AK4PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt' ]
config.section_('Data')
#config.Data.inputDataset = '/AlCaElectron/lbrianza-crab_alcareco_stream_runC-e468ed1ee444b924f5c474c9b5bffcd2/USER'
config.Data.inputDataset = '/AlCaElectron/lbrianza-crab_AlCaElectron_alcareco_RUN2015D-c2f08da413a109f58849306beef921b4/USER'
config.Data.unitsPerJob = 10
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt'
config.Data.inputDBS = 'phys03' #'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet'
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntuple_AlCaElectron_RUN2015D/'
#config.Data.useParent = True
#config.Data.publication = True
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
