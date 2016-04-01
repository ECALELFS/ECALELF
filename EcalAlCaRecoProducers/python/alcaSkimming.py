import FWCore.ParameterSet.Config as cms
import os, sys, imp, re
import FWCore.ParameterSet.VarParsing as VarParsing
import subprocess
import copy

from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
from Calibration.EcalAlCaRecoProducers.customRereco import EcalRecal 


############################################################
### SETUP OPTIONS
options = VarParsing.VarParsing('standard')
options.register('isCrab',
                 1, # default Value = true
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,          # string, int, or float
                 "change files path in case of local test: isCrab=0 if you are running it locally with cmsRun")
options.register ('type',
                  "ALCARAW",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "type of operations: ALCARAW, ALCARERECO, ALCARECO, ALCARECOSIM, MINIAODNTUPLE, SKIMEFFTEST")
options.register ('isPrivate',
                  0, #default value = false
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "tells if this is privately produced")
options.register ('tagFile',
                  "",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "path of the file with the reReco tags")
options.register('skim',
                 "", 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "type of skim: ZSkim, WSkim, ZHLTSkim, partGun, ZmmgSkim, EleSkim (at least one electron), ''")
options.register('jsonFile',
                 "",
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "path and name of the json file")
options.register('doTree',
                 0, #default value False
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,          # string, int, or float
                 "doTree=0: no tree; 1: standard tree; 2: onlyExtraTree; 3: standard+extra; 4:only eleID; 5:eleID+standard; 6: eleID+extra; 7: standard+extra+eleID")
options.register('doTreeOnly',
                 0, #default value False
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,          # string, int, or float
                 "bool: doTreeOnly=1 true, doTreeOnly=0 false")
options.register('pdfSyst',
                 0, #default value False
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,          # string, int, or float
                 "bool: pdfSyst=1 true, pdfSyst=0 false")
options.register('bunchSpacing',
                 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "50=50ns, 25=25ns,0=multifit auto,-1=weights")

### setup any defaults you want
options.output="alcaSkimALCARAW.root"
options.secondaryOutput="ntuple.root"
options.files= ""
options.maxEvents = -1 # -1 means all events
### get and parse the command line arguments
options.parseArguments()

print options

############################################################
# Use the options

# Do you want to filter events?
HLTFilter = False
ZSkim = False
WSkim = False
ZmmgSkim = False

if(options.skim=="ZSkim"):
    ZSkim=True
elif(options.skim=="WSkim"):
    WSkim=True
elif(options.skim=="ZHLTSkim"):
    HLTFilter=True
elif(options.skim=="ZmmgSkim"):
    ZmmgSkim=True
else:
    if(options.type=="ALCARAW"):
        print "[ERROR] no skim selected"
        sys.exit(-1)
    

MC = False  # please specify it if starting from AOD
if(options.type == "ALCARAW"):
    processName = 'ALCASKIM'
elif(options.type == "ALCARERECO"):
    processName = 'ALCARERECO'
elif(options.type == "ALCARECOSIM"):
    processName = 'ALCARECO'
    MC = True
elif(options.type == "ALCARECO"):
    processName = 'ALCARECO'
    MC = False
elif(options.type == 'SKIMEFFTEST'):
    processName = 'SKIMEFFTEST'
    MC = True
elif(options.type == "MINIAODNTUPLE" ):
    processName = "MINIAODNTUPLE"
else:
    print "[ERROR] wrong type defined"
    sys.exit(-1)
    
doTreeOnly=False
if(options.doTree>0 and options.doTreeOnly==1):
    print "doTreeOnly"
    doTreeOnly=True
    processName = processName+'DUMP'
    

#    _____  __             _             _         _
#   / ____|/ _|           | |           | |       | |
#   | |    | |_ __ _   ___| |_ __ _ _ __| |_ ___  | |__   ___ _ __ ___
#   | |    |  _/ _` | / __| __/ _` | '__| __/ __| | '_ \ / _ \ '__/ _ \
#   | |____| || (_| | \__ \ || (_| | |  | |_\__ \ | | | |  __/ | |  __/
#    \_____|_| \__, | |___/\__\__,_|_|   \__|___/ |_| |_|\___|_|  \___|
#               __/ |
#              |___/


CMSSW_VERSION=os.getenv("CMSSW_VERSION")
CMSSW_BASE=os.getenv("CMSSW_BASE")

process = cms.Process(processName)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

# import of ALCARECO sequences
process.load('Calibration.EcalAlCaRecoProducers.ALCARECOEcalCalIsolElectron_cff') # reduction of recHits
process.load('Calibration.EcalAlCaRecoProducers.ALCARECOEcalCalIsolElectron_Output_cff')
process.load('Calibration.EcalAlCaRecoProducers.ALCARECOEcalUncalIsolElectron_cff') # reduction of recHits
process.load('Calibration.EcalAlCaRecoProducers.ALCARECOEcalUncalIsolElectron_Output_cff')

# this module provides:
# process.seqALCARECOEcalRecalElectron 
process.load('Calibration.EcalAlCaRecoProducers.ALCARECOEcalRecalIsolElectron_cff')
process.load('Calibration.EcalAlCaRecoProducers.ALCARECOEcalRecalIsolElectron_Output_cff')
from RecoLocalCalo.EcalRecProducers.ecalLocalCustom import *

process.load("Calibration.EcalAlCaRecoProducers.PUDumper_cfi")

# Tree production
process.load('Calibration.ZNtupleDumper.ntupledumper_cff')

# ntuple
# added by Shervin for ES recHits (saved as in AOD): large window 15x3 (strip x row)
process.load('RecoEcal.EgammaClusterProducers.interestingDetIdCollectionProducer_cfi')

# pdfSystematics
process.load('Calibration.EcalAlCaRecoProducers.pdfSystematics_cff')

process.MessageLogger.cerr = cms.untracked.PSet(
    optionalPSet = cms.untracked.bool(True),
    INFO = cms.untracked.PSet(
    limit = cms.untracked.int32(0)
    ),
    noTimeStamps = cms.untracked.bool(False),
    FwkReport = cms.untracked.PSet(
    optionalPSet = cms.untracked.bool(True),
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(10000000)
    ),
    default = cms.untracked.PSet(
    limit = cms.untracked.int32(10000000)
    ),
    Root_NoDictionary = cms.untracked.PSet(
                 optionalPSet = cms.untracked.bool(True),
                 limit = cms.untracked.int32(0)
                 ),
    FwkJob = cms.untracked.PSet(
    optionalPSet = cms.untracked.bool(True),
    limit = cms.untracked.int32(0)
    ),
    FwkSummary = cms.untracked.PSet(
    optionalPSet = cms.untracked.bool(True),
    reportEvery = cms.untracked.int32(1),
    limit = cms.untracked.int32(10000000)
    ),
    threshold = cms.untracked.string('INFO')
    )

if(options.isCrab==0):
    process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.files),
                            secondaryFileNames = cms.untracked.vstring(options.secondaryFiles),
                            skipEvents=cms.untracked.uint32(0),
                            inputCommands = cms.untracked.vstring( [ 'keep *', 
                                                                     'drop *_correctedHybridSuperClusters_*_RECO',
#                                                                     'drop *_correctedMulti5x5SuperClustersWithPreshower_*_RECO',
                                                                     ]
                                                                   ),
                             dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
                            )


process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

# Other statements
#

if(len(options.tagFile)>0):
    execfile(options.tagFile) # load the GT 
    process.GlobalTag = RerecoGlobalTag 
else:
    if(options.type=="ALCARERECO" and not doTreeOnly):
        print "******************************"
        print "[ERROR] no file with tags specified, but rereco requested"
        sys.exit(1)
        
    if(re.match("CMSSW_4_2_.*",CMSSW_VERSION)):
        if (MC):
            print "[INFO] Using GT START42_V17::All"
            process.GlobalTag.globaltag = 'START42_V17::All'
        else:
            print "[INFO] Using GT FT_R_42_V24::All" #GR_P_V22::All"
            process.GlobalTag.globaltag = 'FT_R_42_V24::All' #'GR_P_V22::All' #GR_R_42_V21B::All' # rereco30Nov
    elif(re.match("CMSSW_4_4_.*", CMSSW_VERSION)):
        if (MC):
            print "[INFO] Using GT START44_V13::All"
            process.GlobalTag.globaltag = 'START44_V13::All'
        else:
            print "[INFO] Using GT GR_R_44_V15C::All"
            #process.GlobalTag.globaltag = 'GR_R_44_V12::All'
            process.GlobalTag.globaltag = 'GR_R_44_V15C::All'
    elif(re.match("CMSSW_5_2_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START52_V16::All"
            process.GlobalTag.globaltag = 'START52_V16::All'
        else:
            process.GlobalTag.globaltag = 'GR_P_V32::All' # 5_2_0 Prompt
            #            process.GlobalTag.globaltag = 'GR_R_52_V7::All' # 5_2_0
    elif(re.match("CMSSW_5_3_11_patch3",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START53_LV4::All"
            process.GlobalTag.globaltag = 'START53_V7C::All'
#            process.GlobalTag.globaltag = 'START53_LV4::All'
        else:
            process.GlobalTag.globaltag = 'FT_R_53_V21::All' #22Jan rereco
    elif(re.match("CMSSW_5_3_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START53_V7N::All"
            process.GlobalTag.globaltag = 'START53_V7N::All' # run dep MC
            #            print "[INFO] Using GT START53_V7G::All"
            #            process.GlobalTag.globaltag = 'START53_V7G::All' # suggested for analysis std. MC
        else:
            print "[INFO] Using GT FT_R_53_V21N::All"
            process.GlobalTag.globaltag = 'FT_R_53_V21::All' #GR_P_V42B::All' # 5_3_3 Prompt
            #process.GlobalTag.globaltag = 'FT_R_53_LV3::All' #21Jun rereco 53X 2011 data
            #process.GlobalTag.globaltag = 'GR_R_53_V9F::All' # GT for 53 rereco (2011)
            if(options.files==""):
                process.source.fileNames=[ 'root://cms-xrd-global.cern.ch//store/data/Run2012A/DoubleElectron/AOD/22Jan2013-v1/20000/003EC246-5E67-E211-B103-00259059642E.root' ]
    elif(re.match("CMSSW_6_1_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START61_V11::All"
            process.GlobalTag.globaltag = 'START61_V11::All'
        else:
            process.GlobalTag.globaltag = 'GR_P_V42B::All' # 5_3_3 Prompt
    elif(re.match("CMSSW_7_0_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT POSTLS162_V5::All"
            process.GlobalTag.globaltag = 'POSTLS162_V5::All'
        else:
            process.GlobalTag.globaltag = 'GR_R_62_V3::All'
            if(options.files==""):
                process.source.fileNames=[ 'root://cms-xrd-global.cern.ch//store/data/Run2012D/DoubleElectron/AOD/15Apr2014-v1/00000/0EA11D35-0CD5-E311-862E-0025905A6070.root' ]
    elif(re.match("CMSSW_7_5_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT auto:run2_data"
            process.GlobalTag.globaltag = 'auto:run2_data'
        else:
            process.GlobalTag.globaltag = 'auto:run2_data'
            if(options.files==""):
                process.source.fileNames=[ 'root://cms-xrd-global.cern.ch//store/data/Run2012D/DoubleElectron/AOD/15Apr2014-v1/00000/0EA11D35-0CD5-E311-862E-0025905A6070.root' ]
    else:
        print "[ERROR]::Global Tag not set for CMSSW_VERSION: ", CMSSW_VERSION
        sys.exit(1)

if(re.match("CMSSW_7_.*",CMSSW_VERSION)):
    myEleCollection =  cms.InputTag("gedGsfElectrons")
else:
    myEleCollection =  cms.InputTag("gsfElectrons")
if(options.type=="MINIAODNTUPLE" ):
    myEleCollection= cms.InputTag("slimmedElectrons")

#Define the sequences
#
# particle flow isolation
#
process.pfIsoEgamma = cms.Sequence()
if(not re.match("CMSSW_7_.*_.*",CMSSW_VERSION)):
    if((options.type=='ALCARECO' or options.type=='ALCARECOSIM' or options.type=='ALCARAW') and not re.match("CMSSW_7_.*_.*",CMSSW_VERSION)):
        from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso, setupPFMuonIso
        process.eleIsoSequence = setupPFElectronIso(process, 'gsfElectrons', 'PFIso')
        process.pfIsoEgamma *= (process.pfParticleSelectionSequence + process.eleIsoSequence)
    elif((options.type=='ALCARECO' or options.type=='ALCARECOSIM' or options.type=='ALCARAW') and re.match("CMSSW_7_.*_.*",CMSSW_VERSION)):
        process.pfisoALCARECO = cms.Sequence() # remove any modules

###############################/
# Event filter sequence: process.filterSeq
# sanbox sequence: process.seqALCARECOEcalUncalElectron + process.alcarecoElectronTracksReducerSeq
# sandbox rereco sequence: process.sandboxRerecoSeq
# alcareco event reduction: process.alcarecoSeq
#
    

################################# FILTERING EVENTS
process.PUDumperSeq = cms.Sequence()
#process.load('Calibration.EcalAlCaRecoProducers.trackerDrivenFinder_cff')
if(MC):
    # PUDumper
    process.TFileService = cms.Service(
        "TFileService",
        fileName = cms.string("PUDumper.root")
        )
    process.PUDumperSeq *= process.PUDumper
    
if(re.match("CMSSW_5_.*", CMSSW_VERSION)):
    process.load('Calibration.EcalAlCaRecoProducers.WZElectronSkims53X_cff')
else:
    process.load('Calibration.EcalAlCaRecoProducers.WZElectronSkims_cff')

process.load('DPGAnalysis.Skims.ZmmgSkim_cff')

process.MinMuonNumberFilter = cms.EDFilter("CandViewCountFilter",
                                          src = cms.InputTag("muons"),
                                          minNumber = cms.uint32(2))
process.MinPhoNumberFilter = cms.EDFilter("CandViewCountFilter",
                                          src = cms.InputTag("gedPhotons"),
                                          minNumber = cms.uint32(1))
process.MinEleNumberFilter = cms.EDFilter("CandViewCountFilter",
                                          src = myEleCollection,
                                          minNumber = cms.uint32(1))
if (ZmmgSkim==True):
    process.filterSeq = cms.Sequence(process.MinMuonNumberFilter * process.MinPhoNumberFilter)
else:
    process.filterSeq = cms.Sequence(process.MinEleNumberFilter)

if (HLTFilter):
    from HLTrigger.HLTfilters.hltHighLevel_cfi import *
    process.ZEEHltFilter = copy.deepcopy(hltHighLevel)
    process.ZEEHltFilter.throw = cms.bool(False)
    process.ZEEHltFilter.HLTPaths = [ "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_*"]
    process.filterSeq *= process.ZEEHltFilter

from HLTrigger.HLTfilters.hltHighLevel_cfi import *
process.NtupleFilter = copy.deepcopy(hltHighLevel)
process.NtupleFilter.throw = cms.bool(False)
process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalUncalZElectron',   'pathALCARECOEcalUncalWElectron',
                                  'pathALCARECOEcalCalZElectron',     'pathALCARECOEcalCalWElectron',
                                  'pathALCARECOEcalUncalZSCElectron', 'pathALCARECOEcalCalZSCElectron',
                                  'pathALCARECOEcalUncalSingleElectron', 'pathALCARECOEcalCalSingleElectron', ## in case of no skim
                                 ]

if(options.isPrivate == 0):
    process.NtupleFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","RECO")
else:
    process.NtupleFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","ALCARECO")


process.NtupleFilterSeq = cms.Sequence()
if(ZSkim):
    process.NtupleFilterSeq = cms.Sequence(process.WZFilter)
  # process.NtupleFilterSeq= cms.Sequence(process.NtupleFilter)
    process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalCalZElectron', 'pathALCARECOEcalUncalZElectron',
                                      'pathALCARECOEcalCalZSCElectron', 'pathALCARECOEcalUncalZSCElectron',
                                      ]
elif(WSkim):
    process.NtupleFilterSeq = cms.Sequence(process.WZFilter)
    #    process.NtupleFilterSeq= cms.Sequence(process.NtupleFilter)
    process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalCalWElectron', 'pathALCARECOEcalUncalWElectron' ]
elif(ZmmgSkim):
    process.NtupleFilterSeq = cms.Sequence(process.ZmmgSkimSeq)
    process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalCalZmmgPhoton', 'pathALCARECOEcalUncalZmmgPhoton' ]

elif(options.skim=="no" or options.skim=="NO" or options.skim=="none" or options.skim=="NONE"):
    process.NtupleFilterSeq = cms.Sequence()
    process.NtupleFilter.HLTPaths = []


if(options.skim=="partGun"):
    process.zNtupleDumper.isPartGun = cms.bool(True)

###############################
# ECAL Recalibration
###############################
#============================== TO BE CHECKED FOR PRESHOWER
process.load("RecoEcal.EgammaClusterProducers.reducedRecHitsSequence_cff")
#process.reducedEcalRecHitsES.scEtThreshold = cms.double(0.)

#process.reducedEcalRecHitsES.EcalRecHitCollectionES = cms.InputTag('ecalPreshowerRecHit','EcalRecHitsES')
#process.reducedEcalRecHitsES.noFlag = cms.bool(True)
#process.reducedEcalRecHitsES.OutputLabel_ES = cms.string('alCaRecHitsES')

#==============================

    
    
try:
    EcalTrivialConditionRetriever
except NameError:
    #print "well, it WASN'T defined after all!"
    process.trivialCond = cms.Sequence()
else:
    print "** TrivialConditionRetriver defined"
    process.trivialCond = cms.Sequence( EcalTrivialConditionRetriever )

if(re.match("CMSSW_7_.*", CMSSW_VERSION)):
    process.alcarerecoSeq=cms.Sequence( process.trivialCond * process.seqALCARECOEcalRecalElectron * process.reducedEcalRecHitsES)
else:
    process.alcarerecoSeq=cms.Sequence( process.trivialCond * process.sandboxRerecoSeq * (process.ALCARECOEcalCalElectronECALSeq + process.reducedEcalRecHitsES))



process.rhoFastJetSeq = cms.Sequence()
if((not options.type=="ALCARERECO") and re.match("CMSSW_5_.*", CMSSW_VERSION)):
    process.rhoFastJetSeq = cms.Sequence(process.kt6PFJetsForRhoCorrection) 

if (options.skim=="ZmmgSkim"):
    process.patSequence=cms.Sequence( (process.muonSelectionProducers * process.phoSelectionProducers) * process.patMuons * process.patPhotons )
    process.patSequenceMC=cms.Sequence( process.muonMatch * process.photonMatch * (process.muonSelectionProducers * process.phoSelectionProducers ) * process.patMuons * process.patPhotons )

if(options.type!="MINIAODNTUPLE"):
    if(MC):
        process.ntupleSeq = cms.Sequence(process.jsonFilter * process.patSequenceMC)
    else:
        process.ntupleSeq = cms.Sequence(process.jsonFilter * process.patSequence)
else:
    process.load('PhysicsTools.PatAlgos.slimming.MiniAODfromMiniAOD_cff')
    process.ntupleSeq = cms.Sequence(process.jsonFilter *   process.EIsequence)
    
if(options.doTree==2 or options.doTree==4 or options.doTree==6 or options.doTree==8):
    process.zNtupleDumper.doStandardTree = cms.bool(False)
if(options.doTree==2 or options.doTree==3 or options.doTree==6 or options.doTree==7 or options.doTree==10 or options.doTree==11 or options.doTree==14 or options.doTree==15): # it's a bit mask
    process.zNtupleDumper.doExtraCalibTree=cms.bool(True)
if(options.doTree==4 or options.doTree==5 or options.doTree==6 or options.doTree==7 or options.doTree==12 or options.doTree==13 or options.doTree==14 or options.doTree==15): # it's a bit mask
    process.zNtupleDumper.doEleIDTree=cms.bool(True)

if(MC and options.pdfSyst==1):
    process.pdfWeightsSeq = cms.Sequence(process.pdfWeights + process.weakWeight + process.fsrWeight)

    process.zNtupleDumper.pdfWeightCollections = cms.VInputTag(cms.InputTag('pdfWeights:cteq66'), cms.InputTag("pdfWeights:MRST2006nnlo"), cms.InputTag('pdfWeights:NNPDF10'))
else:
    process.pdfWeightsSeq = cms.Sequence()



############################################################
# OUTPUT MODULES
##############################
fileName = cms.untracked.string(options.output)

process.outputALCARAW = cms.OutputModule("PoolOutputModule",
                                         # after 5 GB split the file
                                         maxSize = cms.untracked.int32(5120000),
                                         outputCommands = process.OutALCARECOEcalUncalElectron.outputCommands,
                                         #fileName = fileName,
                                         fileName = cms.untracked.string('alcaraw.root'),
                                         SelectEvents = process.OutALCARECOEcalUncalElectron.SelectEvents,
                                         dataset = cms.untracked.PSet(
                                             filterName = cms.untracked.string(''),
                                             dataTier = cms.untracked.string('ALCARECO')
                                         )
                                     )

process.outputALCARECO = cms.OutputModule("PoolOutputModule",
                                          # after 5 GB split the file
                                          maxSize = cms.untracked.int32(5120000),
                                          outputCommands = process.OutALCARECOEcalCalElectron.outputCommands,
                                          fileName = cms.untracked.string('alcareco.root'),
                                          SelectEvents = process.OutALCARECOEcalCalElectron.SelectEvents,
                                          dataset = cms.untracked.PSet(
                                              filterName = cms.untracked.string(''),
                                              dataTier = cms.untracked.string('ALCARECO')
                                          )
                                      )

process.zNtupleDumper.SelectEvents = process.NtupleFilter.HLTPaths

process.outputALCARERECO = cms.OutputModule("PoolOutputModule",
                                            # after 5 GB split the file
                                            maxSize = cms.untracked.int32(5120000),
                                            outputCommands = process.OutALCARECOEcalCalElectron.outputCommands,
                                            fileName = cms.untracked.string('EcalRecalElectron.root'),
                                            SelectEvents = cms.untracked.PSet(
                                                SelectEvents = cms.vstring('pathALCARERECOEcalCalElectron')
                                            ),
                                            dataset = cms.untracked.PSet(
                                                filterName = cms.untracked.string(''),
                                                dataTier = cms.untracked.string('ALCARECO')
                                            )
                                        )

process.outputRECO = cms.OutputModule("PoolOutputModule",
                                      # after 5 GB split the file
                                      maxSize = cms.untracked.int32(5120000),
                                      outputCommands = cms.untracked.vstring('keep *'),
                                      fileName = cms.untracked.string('RECO.root'),
                                      SelectEvents = process.OutALCARECOEcalCalElectron.SelectEvents,
                                      dataset = cms.untracked.PSet(
                                          filterName = cms.untracked.string(''),
                                          dataTier = cms.untracked.string('RECO')
                                      )
                                  )

#print "OUTPUTCOMMANDS"
#print process.outputALCARECO.outputCommands
# if(options.pdfSyst==1):
#     process.TFileService = cms.Service("TFileService",
#                                        fileName = cms.string("ntupleExtra.root"),
#                                        closeFileFast = cms.untracked.bool(True)
#                                        )
 


##############################################################
# Setting Path
################################
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
#process.endjob_step*=process.outputRECO
# Skim check paths to measure efficiency and purity of the skims
# reco efficiencies are not taken into account
# eff = N_skim/N_gen | after reco requirements and gen filter


# ALCARAW
if (not re.match("CMSSW_7_.*",CMSSW_VERSION)):
    uncalibRecHitSeq = cms.Sequence( (ecalDigis + ecalPreshowerDigis) * ecalUncalibRecHitSequence)  #containing the new local reco for 72X

    process.pathALCARECOEcalUncalSingleElectron = cms.Path(process.PUDumperSeq * process.filterSeq *
                                                       process.pfIsoEgamma *
                                                       (process.ALCARECOEcalCalElectronNonECALSeq +
                                                        uncalibRecHitSeq ))
    process.pathALCARECOEcalUncalZElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.preFilterSeq *
                                                   process.pfIsoEgamma *
                                                   (process.ALCARECOEcalCalElectronNonECALSeq +
                                                    uncalibRecHitSeq ))
    process.pathALCARECOEcalUncalZSCElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.preFilterSeq *
                                                     process.pfIsoEgamma *
                                                     ~process.ZeeFilter * process.ZSCFilter *
                                                     (process.ALCARECOEcalCalElectronNonECALSeq +
                                                      uncalibRecHitSeq ))
    process.pathALCARECOEcalUncalWElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.preFilterSeq *
                                                   process.pfIsoEgamma *
                                                   ~process.ZeeFilter * ~process.ZSCFilter * process.WenuFilter *
                                                   (process.ALCARECOEcalCalElectronNonECALSeq +
                                                    uncalibRecHitSeq ))
    process.pathALCARECOEcalUncalZmmgPhoton = cms.Path( process.PUDumperSeq * process.filterSeq * process.FilterMuSeq * process.ZmmgSkimSeq *
                                                   process.pfIsoEgamma *
                                                   ~process.ZeeFilter * ~process.ZSCFilter * ~process.WenuFilter *
                                                   (process.ALCARECOEcalCalElectronNonECALSeq +
                                                    uncalibRecHitSeq ))

else:

    process.pathALCARECOEcalUncalSingleElectron = cms.Path(process.PUDumperSeq * process.filterSeq *
                                                       process.pfIsoEgamma *
                                                           process.seqALCARECOEcalUncalElectron 
                                                           )
    process.pathALCARECOEcalUncalZElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.preFilterSeq *
                                                       process.pfIsoEgamma *
                                                       process.seqALCARECOEcalUncalZElectron )
    process.pathALCARECOEcalUncalZSCElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.preFilterSeq *
                                                         process.pfIsoEgamma *
                                                         ~process.ZeeFilter * process.ZSCFilter *
                                                         process.seqALCARECOEcalUncalZSCElectron 
                                                         )
    process.pathALCARECOEcalUncalWElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.preFilterSeq *
                                                       process.pfIsoEgamma *
                                                       ~process.ZeeFilter * ~process.ZSCFilter * process.WenuFilter *
                                                       process.seqALCARECOEcalUncalWElectron 
                                                       )
    process.pathALCARECOEcalUncalZmmgPhoton = cms.Path( process.PUDumperSeq *
                                                   process.filterSeq * process.FilterMuSeq * process.ZmmgSkimSeq * 
                                                   ~process.ZeeFilter * ~process.ZSCFilter * ~process.WenuFilter *
                                                   process.pfIsoEgamma *
                                                   process.seqALCARECOEcalUncalElectron ) #* process.hltReporter)


# ALCARERECO
process.pathALCARERECOEcalCalElectron = cms.Path(process.alcarerecoSeq)

if(options.doTree>0):
    process.pathALCARERECOEcalCalElectron+=cms.Sequence( process.pdfWeightsSeq * process.ntupleSeq)

# ALCARECO
process.pathALCARECOEcalCalSingleElectron = cms.Path(process.PUDumperSeq * process.filterSeq *
                                                     process.pfIsoEgamma *
                                                     process.ALCARECOEcalCalElectronSeq)
process.pathALCARECOEcalCalZElectron = cms.Path( process.PUDumperSeq * process.filterSeq * 
                                                 process.ZeeSkimFilterSeq *                                                 
                                                 process.pfIsoEgamma *
                                                 process.ALCARECOEcalCalElectronSeq)
process.pathALCARECOEcalCalWElectron = cms.Path( process.PUDumperSeq * process.filterSeq *
                                                 process.WenuSkimFilterSeq *
                                                 process.pfIsoEgamma *
                                                 process.ALCARECOEcalCalElectronSeq)
process.pathALCARECOEcalCalZSCElectron = cms.Path( process.PUDumperSeq *
                                                   process.filterSeq *
                                                   process.ZSCSkimFilterSeq *
#                                                   process.ZSCHltFilter *
                                                   process.pfIsoEgamma *
                                                   process.ALCARECOEcalCalElectronSeq ) #* process.hltReporter)
process.pathALCARECOEcalCalZmmgPhoton = cms.Path( process.PUDumperSeq *
                                                   process.filterSeq * process.FilterMuSeq *  
                                                  process.ZmmgSkimSeq *
                                                   ~process.ZeeFilter * ~process.ZSCFilter * ~process.WenuFilter *
                                                   process.pfIsoEgamma *
                                                   process.seqALCARECOEcalCalPhoton ) #* process.hltReporter)

if (options.skim=="ZmmgSkim"):
    process.NtuplePath = cms.Path(process.filterSeq * process.FilterMuSeq *  process.NtupleFilterSeq 
                                  #                              * process.pfIsoEgamma 
                                  #                              * process.ALCARECOEcalCalElectronSeq 
                              * process.pdfWeightsSeq * process.ntupleSeq)
else:
    process.NtuplePath = cms.Path(process.filterSeq * process.preFilterSeq *  process.NtupleFilterSeq 
                                  #                              * process.pfIsoEgamma 
                                  #                              * process.ALCARECOEcalCalElectronSeq 
                              * process.pdfWeightsSeq * process.ntupleSeq)

process.NtupleEndPath = cms.EndPath( process.zNtupleDumper)


if(not doTreeOnly):
    process.ALCARECOoutput_step = cms.EndPath(process.outputALCARECO )
    if(options.type=="ALCARERECO"):
        process.ALCARERECOoutput_step = cms.EndPath(process.outputALCARERECO)

    if(options.type=="ALCARAW"):
        process.ALCARAWoutput_step = cms.EndPath(process.outputALCARAW)
            


############### JSON Filter
if(options.doTree>0):
    if((options.doTree>0 and options.doTreeOnly==0)):
        # or (options.type=='ALCARECOSIM' and len(options.jsonFile)>0) ):
        print "[INFO] Using json file"
        process.jsonFilter.jsonFileName = cms.string(options.jsonFile)
    else:
        if(len(options.jsonFile)>0):
            print "[INFO] Using json file"
            if(re.match("CMSSW_5_.*_.*",CMSSW_VERSION) or re.match("CMSSW_6_.*_.*",CMSSW_VERSION) or re.match("CMSSW_7_.*_.*",CMSSW_VERSION) ):
                # from CMSSW 5.0.0
                import FWCore.PythonUtilities.LumiList as LumiList
                process.source.lumisToProcess = LumiList.LumiList(filename = options.jsonFile).getVLuminosityBlockRange()
            else:
                # from CMSSW 3.8.0
            #import FWCore.ParameterSet.Config as cms
                import PhysicsTools.PythonAnalysis.LumiList as LumiList
                myLumis = LumiList.LumiList(filename = options.jsonFile).getCMSSWString().split(',')
                process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
                process.source.lumisToProcess.extend(myLumis)


############################################################
# Schedule definition
##############################
if(options.skim=='WSkim'):
    process.outputALCARAW.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalUncalWElectron')
        )
    process.outputALCARECO.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalCalWElectron')
        )
elif(options.skim=='ZSkim'):
    process.outputALCARAW.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalUncalZElectron', 'pathALCARECOEcalUncalZSCElectron')
        )
    process.outputALCARECO.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalCalZElectron', 'pathALCARECOEcalCalZSCElectron')
        )
elif(options.skim=='ZmmgSkim'):
    process.outputALCARAW.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalUncalZmmgPhoton')
        )
    process.outputALCARECO.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalCalZmmgPhoton')
        )
else:
    #if(options.skim=="" or options.skim=="none" or options.skim=="no" or options.skim=="partGun"):
    process.outputALCARAW.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalUncalSingleElectron')
        )
    process.outputALCARECO.SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalCalSingleElectron')
        )

if(options.type=='ALCARAW'):
    process.schedule = cms.Schedule(
        #process.raw2digi_step,process.L1Reco_step,
        #process.reconstruction_step,process.endjob_step, 
        process.pathALCARECOEcalUncalZElectron, process.pathALCARECOEcalUncalWElectron,
        process.pathALCARECOEcalUncalZSCElectron,
        process.pathALCARECOEcalUncalZmmgPhoton,
        process.ALCARAWoutput_step,
        process.pathALCARECOEcalCalZElectron,  process.pathALCARECOEcalCalWElectron,
        process.pathALCARECOEcalCalZSCElectron,
        process.pathALCARECOEcalCalZmmgPhoton,
        process.ALCARECOoutput_step, process.NtuplePath
    ) # fix the output modules
    

elif(options.type=='ALCARERECO'):
    if(doTreeOnly):
        process.NtuplePath = cms.Path(process.pdfWeightsSeq * process.ntupleSeq)
        process.schedule = cms.Schedule(process.NtuplePath, process.NtupleEndPath)
    else:
        if(options.doTree>0):
            process.pathALCARERECOEcalCalElectron += process.zNtupleDumper
        process.schedule = cms.Schedule(process.pathALCARERECOEcalCalElectron, process.ALCARERECOoutput_step)
elif(options.type=='ALCARECO' or options.type=='ALCARECOSIM'):
    if(doTreeOnly):
        process.NtuplePath = cms.Path(process.pdfWeightsSeq * process.ntupleSeq)
        process.schedule = cms.Schedule(process.NtuplePath, process.NtupleEndPath)
        process.zNtupleDumper.WZSkimResultsCollection = cms.InputTag('TriggerResults::ALCARECO')
    else:
        if(options.doTree==0):
            process.schedule = cms.Schedule(process.pathALCARECOEcalCalZElectron,  process.pathALCARECOEcalCalWElectron,
                                            process.pathALCARECOEcalCalZSCElectron, process.pathALCARECOEcalCalZmmgPhoton,
                                            process.ALCARECOoutput_step
                                            ) # fix the output modules
        else:
            process.schedule = cms.Schedule(process.pathALCARECOEcalCalZElectron,  process.pathALCARECOEcalCalWElectron,
                                            process.pathALCARECOEcalCalZSCElectron, process.pathALCARECOEcalCalZmmgPhoton,
                                            #process.ALCARECOoutput_step,
                                            process.NtuplePath, process.NtupleEndPath
                                            ) # fix the output modules
        if(options.skim=="" or options.skim=="ZHLTSkim" or options.skim=="partGun"):
            process.schedule += cms.Schedule(process.pathALCARECOEcalCalSingleElectron)
elif(options.type=='SKIMEFFTEST'):
    process.schedule = cms.Schedule(process.pathWElectronSkimGen, process.pathZSCElectronSkimGen, process.pathZElectronSkimGen,
                                    process.pathWElectronSkim, process.pathZSCElectronSkim, process.pathZElectronSkim,
                                    process.pathWElectronGen, process.pathZSCElectronGen, process.pathZElectronGen,
                                    )
elif(options.type=='MINIAODNTUPLE'):
    process.schedule = cms.Schedule(process.NtuplePath, process.NtupleEndPath)

process.zNtupleDumper.foutName=options.secondaryOutput
# this includes the sequence: patSequence
# patSequence=cms.Sequence( (eleSelectionProducers  + eleNewEnergiesProducer ) * patElectrons)

if(options.isCrab==1):
    pathPrefix=""
else:
    pathPrefix=CMSSW_BASE+'/' #./src/Calibration/EleNewEnergiesProducer' #CMSSW_BASE+'/src/Calibration/EleNewEnergiesProducer/'
    print "[INFO] Running locally: pathPrefix="+pathPrefix


if(re.match("CMSSW_5_.*", CMSSW_VERSION)):
    process.load('Calibration.EcalAlCaRecoProducers.valuemaptraslator_cfi')
    process.sandboxRerecoSeq*=process.elPFIsoValueCharged03PFIdRecalib
    process.sandboxRerecoSeq*=process.elPFIsoValueGamma03PFIdRecalib
    process.sandboxRerecoSeq*=process.elPFIsoValueNeutral03PFIdRecalib


############################################################
# Setting collection names
##############################
process.selectedECALElectrons.src = myEleCollection
if(re.match("CMSSW_5_.*", CMSSW_VERSION)):
    process.PassingVeryLooseId.src = myEleCollection
    process.PassingMediumId.src = myEleCollection
    process.PassingTightId.src = myEleCollection

    process.elPFIsoValueCharged03PFIdRecalib.oldreferenceCollection = myEleCollection
    process.elPFIsoValueGamma03PFIdRecalib.oldreferenceCollection = myEleCollection
    process.elPFIsoValueNeutral03PFIdRecalib.oldreferenceCollection = myEleCollection
    process.elPFIsoDepositChargedGsf.src = myEleCollection
    process.elPFIsoDepositGammaGsf.src = myEleCollection
    process.elPFIsoDepositChargedGsf.src = myEleCollection

else:
    process.PassingVetoId.src = myEleCollection


process.patElectrons.electronSource = myEleCollection
process.eleSelectionProducers.electronCollection = myEleCollection
process.electronMatch.src = myEleCollection
if (options.skim=="ZmmgSkim"):
    process.alCaIsolatedElectrons.photonLabel = cms.InputTag("gedPhotons")

process.alcaElectronTracksReducer.electronLabel = myEleCollection

#process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
#process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
if(options.type!="MINIAODNTUPLE"):
    process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
    process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")
else:
    process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("reducedEgamma", "reducedEBRecHits")
    process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("reducedEgamma", "reducedEERecHits")
    process.eleNewEnergiesProducer.recHitCollectionES = cms.InputTag("reducedEgamma", "reducedESRecHits")
    process.zNtupleDumper.useIDforPresel = cms.bool(False)
    process.zNtupleDumper.rhoFastJet = cms.InputTag("fixedGridRhoFastjetAll")
    process.zNtupleDumper.pileupInfo = cms.InputTag("slimmedAddPileupInfo")
    process.zNtupleDumper.vertexCollection = cms.InputTag('offlineSlimmedPrimaryVertices')
    process.zNtupleDumper.electronCollection = cms.InputTag('slimmedElectrons')
    process.zNtupleDumper.EESuperClusterCollection = cms.InputTag("reducedEgamma", "reducedSuperClusters")
    process.zNtupleDumper.WZSkimResultsCollection = cms.InputTag('')
    process.zNtupleDumper.SelectEvents = cms.vstring('')
    process.zNtupleDumper.eleID_loose = cms.string("cutBasedElectronID-Spring15-25ns-V1-standalone-loose")
    process.zNtupleDumper.eleID_medium = cms.string("cutBasedElectronID-Spring15-25ns-V1-standalone-medium")
    process.zNtupleDumper.eleID_tight = cms.string("cutBasedElectronID-Spring15-25ns-V1-standalone-tight")

process.eleNewEnergiesProducer.electronCollection = myEleCollection

if(options.type=="ALCARERECO"):
    recalibElectronSrc = cms.InputTag("electronRecalibSCAssociator") #now done by EcalRecal(process)
    process = EcalRecal(process)
    process.eleSelectionProducers.electronCollection   = recalibElectronSrc
    process.patElectrons.electronSource                = recalibElectronSrc
    process.eleSelectionProducers.chIsoVals = cms.InputTag('elPFIsoValueCharged03PFIdRecalib')
    process.eleSelectionProducers.emIsoVals = cms.InputTag('elPFIsoValueGamma03PFIdRecalib')
    process.eleSelectionProducers.nhIsoVals = cms.InputTag('elPFIsoValueNeutral03PFIdRecalib')
    process.eleNewEnergiesProducer.electronCollection = recalibElectronSrc
    
    process.outputALCARECO.outputCommands += process.OutALCARECOEcalRecalElectron.outputCommands
    process.outputALCARECO.fileName=cms.untracked.string('EcalRecalElectron.root')
    process.MinEleNumberFilter.src = recalibElectronSrc
    process.zNtupleDumper.WZSkimResultsCollection = cms.InputTag('TriggerResults::RECO') ## how and why and where is it used?
    process.eleNewEnergiesProducer.electronCollection = recalibElectronSrc

    if(options.bunchSpacing==25):
        print "bunchSpacing", options.bunchSpacing
        #        configureEcalLocal25ns(process)
        process.ecalMultiFitUncalibRecHit.algoPSet.activeBXs = cms.vint32(-5,-4,-3,-2,-1,0,1,2,3,4)
        process.ecalMultiFitUncalibRecHit.algoPSet.useLumiInfoRunHeader = cms.bool(False)
    elif(options.bunchSpacing==50):
        #        configureEcalLocal50ns(process)
        process.ecalMultiFitUncalibRecHit.algoPSet.activeBXs = cms.vint32(-4,-2,0,2,4)
        process.ecalMultiFitUncalibRecHit.algoPSet.useLumiInfoRunHeader = cms.bool(False)
    elif(options.bunchSpacing==0):
        # auto defined by the bunchSpacingProducer
        process.ecalMultiFitUncalibRecHit.algoPSet.useLumiInfoRunHeader = cms.bool(True)
    elif(options.bunchSpacing==-1):
        process.ecalLocalRecoSequence.replace(process.ecalMultiFitUncalibRecHit, process.ecalGlobalUncalibRecHit)
        process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalGlobalUncalibRecHit","EcalUncalibRecHitsEE")
        process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalGlobalUncalibRecHit","EcalUncalibRecHitsEB")
    else:
        print "[ERROR] only bunchSpacing of 50 and 25 are implemented"
        exit(1)
        
process.patElectrons.reducedBarrelRecHitCollection = process.eleNewEnergiesProducer.recHitCollectionEB
process.patElectrons.reducedEndcapRecHitCollection = process.eleNewEnergiesProducer.recHitCollectionEE
process.zNtupleDumper.recHitCollectionEB = process.eleNewEnergiesProducer.recHitCollectionEB
process.zNtupleDumper.recHitCollectionEE = process.eleNewEnergiesProducer.recHitCollectionEE

if(options.type=="ALCARECOSIM"):
    process.zNtupleDumper.recHitCollectionES = cms.InputTag("reducedEcalRecHitsES")
#process.zNtupleDumper.recHitCollectionES = cms.InputTag("reducedEcalRecHitsES")
############################
## Dump the output Python ##
############################
processDumpFile = open('processDump.py', 'w')
print >> processDumpFile, process.dumpPython()
