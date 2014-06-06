import FWCore.ParameterSet.Config as cms
import os, sys, imp, re
import FWCore.ParameterSet.VarParsing as VarParsing
import subprocess
import copy

from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet

myEleCollection =  cms.InputTag("gedGsfElectrons")
#sys.path(".")

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
                  "type of operations: ALCARAW, ALCARERECO, ALCARECO, ALCARECOSIM, AOD, RECO")
options.register ('tagFile',
                  "",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "path of the file with the reReco tags")
options.register('skim',
                 "", 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "type of skim: ZSkim, WSkim, partGun, EleSkim (at least one electron), ''")
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
                 
### setup any defaults you want
options.output="alcaSkimALCARAW.root"
options.secondaryOutput="ntuple.root"
options.files= "root://eoscms//eos/cms/store/group/alca_ecalcalib/ALCARAW/RAW-RECO_533.root"
options.maxEvents = -1 # -1 means all events
### get and parse the command line arguments
options.parseArguments()

print options

############################################################
# Use the options

# Do you want to filter events? 
ZSkim = False
WSkim = False

if(options.skim=="ZSkim"):
    ZSkim=True
elif(options.skim=="WSkim"):
    WSkim=True
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
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.EventContent.EventContent_cff')

# import of ALCARECO sequences
process.load('Calibration.ALCARAW_RECO.ALCARECOEcalCalIsolElectron_Output_cff')
process.load('Calibration.ALCARAW_RECO.ALCARECOEcalUncalIsolElectron_Output_cff')
from Calibration.ALCARAW_RECO.sandboxRerecoOutput_cff import *

#process.load('Configuration.StandardSequences.AlCaRecoStreams_cff') # this is for official ALCARAW ALCARECO production
process.load('Calibration.ALCARAW_RECO.ALCARECOEcalCalIsolElectron_cff') # reduction of recHits
process.load("Calibration.ALCARAW_RECO.PUDumper_cfi")
process.load('Calibration.ALCARAW_RECO.ALCARECOEcalUncalIsolElectron_cff') # ALCARAW
# this module provides:
#process.seqALCARECOEcalUncalElectron  = uncalibRecHitSeq
process.load('Calibration.ALCARAW_RECO.sandboxRerecoSeq_cff')    # ALCARERECO
# this module provides:
# process.electronRecoSeq
# process.electronClusteringSeq # with ele-SC reassociation
# process.sandboxRerecoSeq = (electronRecoSeq * electronClusteringSeq)

# Tree production
process.load('Calibration.ZNtupleDumper.ntupledumper_cff')

# ntuple
# added by Shervin for ES recHits (saved as in AOD): large window 15x3 (strip x row)
process.load('RecoEcal.EgammaClusterProducers.interestingDetIdCollectionProducer_cfi')

# pdfSystematics
process.load('Calibration.ALCARAW_RECO.pdfSystematics_cff')

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
                            secondaryFileNames = cms.untracked.vstring(options.secondaryFiles)
                            )

# try to drop as much as possible to reduce the running time
# process.source.inputCommands = cms.untracked.vstring("keep *",
#                                                      "drop recoPFTaus*_*_*_*", 
#                                                      "drop recoPFTauDiscriminator*_*_*_*",
#                                                      "drop *_tevMuons*_*_*",
# #                                                     "drop *muon*_*_*_*",
# #                                                     "keep *Electron*_*_*_",
# #                                                     "keep *electron*_*_*_*"
#                                                      )

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

# Other statements
#

CMSSW_VERSION=os.getenv("CMSSW_VERSION")
CMSSW_BASE=os.getenv("CMSSW_BASE")
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
            process.GlobalTag.globaltag = 'FT_R_53_V21::All' #GR_P_V42B::All' # 5_3_3 Prompt
            #process.GlobalTag.globaltag = 'FT_R_53_LV3::All' #21Jun rereco 53X 2011 data
            #process.GlobalTag.globaltag = 'GR_R_53_V9F::All' # GT for 53 rereco (2011)
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
    else:
        print "[ERROR]::Global Tag not set for CMSSW_VERSION: ", CMSSW_VERSION
        sys.exit(1)

#Define the sequences
#
# particle flow isolation
#
process.pfIsoEgamma = cms.Sequence()
if((options.type=='ALCARECO' or options.type=='ALCARECOSIM') and not re.match("CMSSW_7_.*_.*",CMSSW_VERSION)):
    from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso, setupPFMuonIso
    process.eleIsoSequence = setupPFElectronIso(process, 'gsfGsfElectrons', 'PFIso')
    process.pfIsoEgamma *= (process.pfParticleSelectionSequence + process.eleIsoSequence)
elif((options.type=='ALCARECO' or options.type=='ALCARECOSIM') and re.match("CMSSW_7_.*_.*",CMSSW_VERSION)):
    process.pfisoALCARECO = cms.Sequence() # remove any modules

###############################/
# Event filter sequence: process.filterSeq
# sanbox sequence: process.seqALCARECOEcalUncalElectron + process.alcarecoElectronTracksReducerSeq
# sandbox rereco sequence: process.sandboxRerecoSeq
# alcareco event reduction: process.alcarecoSeq
#
    

################################# FILTERING EVENTS
process.PUDumperSeq = cms.Sequence()
#process.load('Calibration.ALCARAW_RECO.trackerDrivenFinder_cff')
if(MC):
    # PUDumper
    process.TFileService = cms.Service(
        "TFileService",
        fileName = cms.string("PUDumper.root")
        )
    process.PUDumperSeq *= process.PUDumper
    
process.load('Calibration.ALCARAW_RECO.WZElectronSkims_cff')

process.MinEleNumberFilter = cms.EDFilter("CandViewCountFilter",
                                          src = myEleCollection,
                                          minNumber = cms.uint32(1)
                                          )
process.filterSeq = cms.Sequence(process.MinEleNumberFilter)


#from HLTrigger.HLTfilters.hltHighLevel_cfi import *
#process.NtupleFilter = copy.deepcopy(hltHighLevel)
#process.NtupleFilter.throw = cms.bool(False)
#process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalUncalZElectron',   'pathALCARECOEcalUncalWElectron',
#                                  'pathALCARECOEcalCalZElectron',     'pathALCARECOEcalCalWElectron',
#                                  'pathALCARECOEcalUncalZSCElectron', 'pathALCARECOEcalCalZSCElectron',
#                                  ]
#process.NtupleFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","ALCARECO")
#
#if(ZSkim):
#    process.NtupleFilterSeq= cms.Sequence(process.NtupleFilter)
#    process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalCalZElectron', 'pathALCARECOEcalUncalZElectron',
#                                      'pathALCARECOEcalCalZSCElectron', 'pathALCARECOEcalUncalZSCElectron',
#                                      ]
#elif(WSkim):
#    #cms.Sequence(~process.ZeeFilter * ~process.ZSCFilter * process.WenuFilter)
#    process.NtupleFilterSeq= cms.Sequence(process.NtupleFilter)
#    process.NtupleFilter.HLTPaths = [ 'pathALCARECOEcalCalWElectron', 'pathALCARECOEcalUncalWElectron' ]
#    process.zNtupleDumper.isWenu=cms.bool(True)
#else:
#    process.NtupleFilterSeq = cms.Sequence()
process.NtupleFilterSeq = cms.Sequence()

if(options.skim=="partGun"):
    process.zNtupleDumper.isPartGun = cms.bool(True)

###############################
# ECAL Recalibration
###############################
#============================== TO BE CHECKED FOR PRESHOWER
process.load("RecoEcal.EgammaClusterProducers.reducedRecHitsSequence_cff")
process.reducedEcalRecHitsES.scEtThreshold = cms.double(0.)

process.reducedEcalRecHitsES.EcalRecHitCollectionES = cms.InputTag('ecalPreshowerRecHit','EcalRecHitsES')
process.reducedEcalRecHitsES.noFlag = cms.bool(True)
process.reducedEcalRecHitsES.OutputLabel_ES = cms.string('alCaRecHitsES')

#==============================

    
    
try:
    EcalTrivialConditionRetriever
except NameError:
    #print "well, it WASN'T defined after all!"
    process.trivialCond = cms.Sequence()
else:
    print "** TrivialConditionRetriver defined"
    process.trivialCond = cms.Sequence( EcalTrivialConditionRetriever )


if(re.match("CMSSW_6_.*", CMSSW_VERSION)):
    process.alcarerecoSeq=cms.Sequence( process.trivialCond * process.sandboxPFRerecoSeq * (process.seqALCARECOEcalCalElectronRECO + process.reducedEcalRecHitsES))
else:
    process.alcarerecoSeq=cms.Sequence( process.trivialCond * process.sandboxRerecoSeq * (process.seqALCARECOEcalCalElectronRECO + process.reducedEcalRecHitsES))


process.rhoFastJetSeq = cms.Sequence()
if((not options.type=="ALCARERECO") ):
    process.rhoFastJetSeq = cms.Sequence(process.kt6PFJetsForRhoCorrection) 


if(MC):
    process.ntupleSeq = cms.Sequence(process.jsonFilter * process.patSequenceMC)
else:
    process.ntupleSeq = cms.Sequence(process.jsonFilter * process.patSequence)
    
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
    process.zNtupleDumper.doPdfSystTree = cms.bool(False)


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
                                          outputCommands = process.OutALCARECOEcalCalElectron_.outputCommands,
                                          fileName = cms.untracked.string('alcareco.root'),
                                          SelectEvents = process.OutALCARECOEcalCalElectron.SelectEvents,
                                          dataset = cms.untracked.PSet(
    filterName = cms.untracked.string(''),
    dataTier = cms.untracked.string('ALCARECO')
    )
                                          )
process.outputALCARERECO = cms.OutputModule("PoolOutputModule",
                                          # after 5 GB split the file
                                          maxSize = cms.untracked.int32(5120000),
                                          outputCommands = process.OutALCARECOEcalCalElectron_.outputCommands,
                                          fileName = cms.untracked.string('alcarereco.root'),
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
# ALCARAW
process.pathALCARECOEcalUncalSingleElectron = cms.Path(process.PUDumperSeq * process.filterSeq *
                                                   (process.ALCARECOEcalCalElectronPreSeq +
                                                    process.seqALCARECOEcalUncalElectron ))
process.pathALCARECOEcalUncalZElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.FilterSeq *
                                                   (process.ALCARECOEcalCalElectronPreSeq +
                                                    process.seqALCARECOEcalUncalElectron ))
process.pathALCARECOEcalUncalZSCElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.FilterSeq *
                                                     ~process.ZeeFilter * process.ZSCFilter *
                                                     (process.ALCARECOEcalCalElectronPreSeq +
                                                      process.seqALCARECOEcalUncalElectron ))
process.pathALCARECOEcalUncalWElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.FilterSeq *
                                                   ~process.ZeeFilter * ~process.ZSCFilter * process.WenuFilter *
                                                   (process.ALCARECOEcalCalElectronPreSeq +
                                                    process.seqALCARECOEcalUncalElectron ))

# ALCARERECO
process.pathALCARERECOEcalCalElectron = cms.Path(process.alcarerecoSeq)
# ALCARECO
process.pathALCARECOEcalCalSingleElectron = cms.Path(process.PUDumperSeq * process.filterSeq *
                                                     process.pfIsoEgamma *
                                                     process.seqALCARECOEcalCalElectron)
process.pathALCARECOEcalCalZElectron = cms.Path( process.PUDumperSeq * process.filterSeq * process.FilterSeq *
                                                 process.ZeeFilter *
                                                 process.pfIsoEgamma *
                                                 process.seqALCARECOEcalCalElectron)
process.pathALCARECOEcalCalWElectron = cms.Path( process.PUDumperSeq * process.filterSeq *
                                                 process.FilterSeq * 
                                                 ~process.ZeeFilter * ~process.ZSCFilter * process.WenuFilter *
                                                 process.pfIsoEgamma *
                                                 process.seqALCARECOEcalCalElectron)


process.pathALCARECOEcalCalZSCElectron = cms.Path( process.PUDumperSeq *
                                                   process.filterSeq * process.FilterSeq *
                                                   ~process.ZeeFilter * process.ZSCFilter * 
#                                                   process.ZSCHltFilter *
                                                   process.pfIsoEgamma *
                                                   process.seqALCARECOEcalCalElectron ) #* process.hltReporter)


process.NtuplePath = cms.Path(process.filterSeq * process.FilterSeq * process.WZFilter * process.NtupleFilterSeq 
                              * process.pdfWeightsSeq * process.ntupleSeq)
process.NtupleEndPath = cms.EndPath( process.zNtupleDumper)


if(not doTreeOnly):
    process.ALCARECOoutput_step = cms.EndPath(process.outputALCARECO )
    if(options.type=="ALCARERECO"):
        process.ALCARERECOoutput_step = cms.EndPath(process.outputALCARERECO)
    if(options.type=="ALCARAW"):
        process.ALCARAWoutput_step = cms.EndPath(process.outputALCARAW)
            

############################################################
# Setting collection names
##############################
#process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
#process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")
if(options.type=="ALCARERECO"):        
    process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalGlobalUncalibRecHit","EcalUncalibRecHitsEB")
    process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalGlobalUncalibRecHit","EcalUncalibRecHitsEE")

    process.correctedHybridSuperClusters.corectedSuperClusterCollection = 'recalibSC'
    process.correctedMulti5x5SuperClustersWithPreshower.corectedSuperClusterCollection = 'endcapRecalibSC'
    if(re.match("CMSSW_5_.*",CMSSW_VERSION) or re.match("CMSSW_6_.*", CMSSW_VERSION)):
        process.multi5x5PreshowerClusterShape.endcapSClusterProducer = "correctedMulti5x5SuperClustersWithPreshower:endcapRecalibSC"

    # in sandboxRereco
    process.reducedEcalRecHitsES.EndcapSuperClusterCollection= cms.InputTag('correctedMulti5x5SuperClustersWithPreshower','endcapRecalibSC',processName)

    process.alCaIsolatedElectrons.electronLabel = cms.InputTag("electronRecalibSCAssociator")
    process.alCaIsolatedElectrons.ebRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB")
    process.alCaIsolatedElectrons.eeRecHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE")

    process.eleRegressionEnergy.inputElectronsTag = cms.InputTag('electronRecalibSCAssociator')
    process.eleSelectionProducers.electronCollection   = 'electronRecalibSCAssociator'
    process.eleNewEnergiesProducer.electronCollection  = 'electronRecalibSCAssociator'
    process.patElectrons.electronSource                = 'electronRecalibSCAssociator'
    process.eleSelectionProducers.chIsoVals = cms.InputTag('elPFIsoValueCharged03PFIdRecalib')
    process.eleSelectionProducers.emIsoVals = cms.InputTag('elPFIsoValueGamma03PFIdRecalib')
    process.eleSelectionProducers.nhIsoVals = cms.InputTag('elPFIsoValueNeutral03PFIdRecalib')
    
    process.outputALCARECO.outputCommands += sandboxRerecoOutputCommands 
    process.outputALCARECO.fileName=cms.untracked.string('alcarereco.root')

process.patElectrons.reducedBarrelRecHitCollection = process.eleNewEnergiesProducer.recHitCollectionEB
process.patElectrons.reducedEndcapRecHitCollection = process.eleNewEnergiesProducer.recHitCollectionEE
process.zNtupleDumper.recHitCollectionEB = process.eleNewEnergiesProducer.recHitCollectionEB
process.zNtupleDumper.recHitCollectionEE = process.eleNewEnergiesProducer.recHitCollectionEE
process.eleRegressionEnergy.recHitCollectionEB = process.eleNewEnergiesProducer.recHitCollectionEB.value()
process.eleRegressionEnergy.recHitCollectionEE = process.eleNewEnergiesProducer.recHitCollectionEE.value()


############### JSON Filter
if((options.doTree>0 and options.doTreeOnly==0)):
    # or (options.type=='ALCARECOSIM' and len(options.jsonFile)>0) ):
    process.jsonFilter.jsonFileName = cms.string(options.jsonFile)
else:
    if(len(options.jsonFile)>0):
        if(re.match("CMSSW_5_.*_.*",CMSSW_VERSION) or re.match("CMSSW_6_.*_.*",CMSSW_VERSION)):
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
        process.ALCARAWoutput_step,
        process.pathALCARECOEcalCalZElectron,  process.pathALCARECOEcalCalWElectron,
        process.pathALCARECOEcalCalZSCElectron,
        process.ALCARECOoutput_step, process.NtuplePath) # fix the output modules
    

elif(options.type=='ALCARERECO'):
    if(doTreeOnly):
        process.schedule = cms.Schedule(process.NtuplePath, process.NtupleEndPath)
    else:
        process.schedule = cms.Schedule(process.pathALCARERECOEcalCalElectron, process.ALCARERECOoutput_step,
                                        process.NtuplePath, process.NtupleEndPath)
elif(options.type=='ALCARECO' or options.type=='ALCARECOSIM'):
    if(doTreeOnly):
        process.schedule = cms.Schedule(process.NtuplePath, process.NtupleEndPath)
                                        process.NtuplePath)
    else:
        if(options.doTree==1):
            process.schedule = cms.Schedule(process.pathALCARECOEcalCalZElectron,  process.pathALCARECOEcalCalWElectron,
                                            process.pathALCARECOEcalCalZSCElectron,
                                            process.ALCARECOoutput_step
                                            ) # fix the output modules
        else:
            process.schedule = cms.Schedule(process.pathALCARECOEcalCalZElectron,  process.pathALCARECOEcalCalWElectron,
                                            process.pathALCARECOEcalCalZSCElectron,
                                            process.ALCARECOoutput_step,  process.NtuplePath, process.NtupleEndPath
                                            ) # fix the output modules


process.zNtupleDumper.foutName=options.secondaryOutput
# this includes the sequence: patSequence
# patSequence=cms.Sequence( (eleSelectionProducers  + eleNewEnergiesProducer ) * patElectrons)

if(options.isCrab=='1'):
    pathPrefix=""
else:
    pathPrefix=CMSSW_BASE+'/' #./src/Calibration/EleNewEnergiesProducer' #CMSSW_BASE+'/src/Calibration/EleNewEnergiesProducer/'
    print "[INFO] Running locally: pathPrefix="+pathPrefix

process.eleNewEnergiesProducer.regrPhoFile=pathPrefix+process.eleNewEnergiesProducer.regrPhoFile.value()
process.eleNewEnergiesProducer.regrEleFile=pathPrefix+process.eleNewEnergiesProducer.regrEleFile.value()
process.eleNewEnergiesProducer.regrEleFile_fra=pathPrefix+process.eleNewEnergiesProducer.regrEleFile_fra.value()
# Now files are on CERN EOS, files accessed via xrootd
#process.eleNewEnergiesProducer.regrEleJoshV4_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV4_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV5_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV5_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV4_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV4_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV5_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV5_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV6_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV6_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV6_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV6_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV7_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV7_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV7_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV7_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV8_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV8_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV8_SemiParamFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV8_SemiParamFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV6_SemiParam7TeVtrainFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV6_SemiParam7TeVtrainFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV6_SemiParam7TeVtrainFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV6_SemiParam7TeVtrainFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV7_SemiParam7TeVtrainFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV7_SemiParam7TeVtrainFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV7_SemiParam7TeVtrainFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV7_SemiParam7TeVtrainFile.value()
#process.eleNewEnergiesProducer.regrEleJoshV8_SemiParam7TeVtrainFile = pathPrefix+process.eleNewEnergiesProducer.regrEleJoshV8_SemiParam7TeVtrainFile.value()
#process.eleNewEnergiesProducer.regrPhoJoshV8_SemiParam7TeVtrainFile = pathPrefix+process.eleNewEnergiesProducer.regrPhoJoshV8_SemiParam7TeVtrainFile.value()

#    process.eleRegressionEnergy.regressionInputFile = cms.string("EgammaAnalysis/ElectronTools/data/eleEnergyReg2012Weights_V1.root") #eleEnergyRegWeights_WithSubClusters_VApr15.root")
process.eleRegressionEnergy.energyRegressionType=cms.uint32(2)
if(re.match("CMSSW_4_4_.*", CMSSW_VERSION)):
    process.eleRegressionEnergy.regressionInputFile = cms.string("EgammaAnalysis/ElectronTools/data/eleEnergyReg2011Weights_V1.root")
if(re.match("CMSSW_4_2_.*", CMSSW_VERSION)):
    pathPrefix=CMSSW_BASE+'/src/Calibration/EleNewEnergiesProducer/'
    print '[INFO] Using v2 regression for CMSSW_4_2_X' 
    process.eleNewEnergiesProducer.regrPhoFile=cms.string(pathPrefix+'data/gbrv2ph.root')
    process.eleNewEnergiesProducer.regrEleFile=cms.string(pathPrefix+'data/gbrv2ele.root')
    process.eleNewEnergiesProducer.regrEleFile_fra=cms.string('nocorrections')
    #process.eleNewEnergiesProducer.regrEleFile_fra=cms.string(pathPrefix+'data/eleEnergyRegWeights_V1.root')


process.load('Calibration.ValueMapTraslator.valuemaptraslator_cfi')
process.sandboxRerecoSeq*=process.elPFIsoValueCharged03PFIdRecalib
process.sandboxRerecoSeq*=process.elPFIsoValueGamma03PFIdRecalib
process.sandboxRerecoSeq*=process.elPFIsoValueNeutral03PFIdRecalib



############################
## Dump the output Python ##
############################
processDumpFile = open('processDump.py', 'w')
print >> processDumpFile, process.dumpPython()

##########################################################
## Set correct electron definition for required methods ##
##########################################################
process.eleRegressionEnergy.inputElectronsTag = myEleCollection
process.patElectrons.electronSource = myEleCollection
process.eleSelectionProducers.electronCollection = myEleCollection
process.PassingHLT.InputProducer = myEleCollection
process.selectedElectrons.src = myEleCollection
process.eleNewEnergiesProducer.electronCollection = myEleCollection
process.alCaIsolatedElectrons.electronLabel = myEleCollection 
process.alcaElectronTracksReducer.electronLabel = myEleCollection
process.elPFIsoDepositGammaGsf.src = myEleCollection
process.elPFIsoValueCharged03PFIdRecalib.oldreferenceCollection = myEleCollection
process.elPFIsoValueGamma03PFIdRecalib.oldreferenceCollection = myEleCollection
process.elPFIsoValueNeutral03PFIdRecalib.oldreferenceCollection = myEleCollection
