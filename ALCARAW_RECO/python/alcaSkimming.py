import FWCore.ParameterSet.Config as cms
import os, sys, imp, re
import FWCore.ParameterSet.VarParsing as VarParsing
import subprocess

#sys.path(".")
#
#    _____             __ _                        _   _
#   / ____|           / _(_)                      | | (_)
#   | |     ___  _ __ | |_ _  __ _ _   _ _ __ __ _| |_ _  ___  _ __
#   | |    / _ \| '_ \|  _| |/ _` | | | | '__/ _` | __| |/ _ \| '_ \
#   | |___| (_) | | | | | | | (_| | |_| | | | (_| | |_| | (_) | | | |
#    \_____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__|_|\___/|_| |_|
#                             __/ |
#                            |___/

### setup 'ALCARAW'  options
options = VarParsing.VarParsing('standard')
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
                 "type of skim: ZSkim, WSkim, fromWSkim (from USER format), EleSkim (at least one electron), ''")
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
options.register('isSingleEle',
                 0, #default value False
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,          # string, int, or float
                 "sSingleEle=0: false")
options.register('doTreeOnly',
                 1, #default value False
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.int,          # string, int, or float
                 "bool: doTreeOnly=1 true, doTreeOnly=0 false")
                 
### setup any defaults you want
#options.outputFile = '/uscms/home/cplager/nobackup/outputFiles/try_3.root'
options.output="alcaSkimALCARAW.root"
options.secondaryOutput="ntuple.root"
#options.files= "file:///tmp/shervin/RAW-RECO.root"
options.files= "root://eoscms//eos/cms/store/group/alca_ecalcalib/ALCARAW/RAW-RECO_533.root"
#options.files= "file:///tmp/shervin/MC-AODSIM.root"
#'file1.root', 'file2.root'
options.maxEvents = -1 # -1 means all events
### get and parse the command line arguments
options.parseArguments()

print options
# Use the options

# Do you want to filter events? 
HLTFilter = False
ZSkim = False
WSkim = False

if(options.skim=="ZSkim"):
    ZSkim=True
elif(options.skim=="WSkim"):
    WSkim=True
elif(options.skim=="fromWSkim"):
    print "[INFO] producing from WSkim files (USER format)"
    WSkim=False
else:
    if(options.type=="ALCARAW"):
        print "[ERROR] no skim selected"
#        sys.exit(-1)


doTreeOnly=False
if(options.doTree>0 and options.doTreeOnly==1):
    doTreeOnly=True

MC = False  # please specify it if starting from AOD
if(options.type == "ALCARAW"):
    processName = 'ALCASKIM'
#    ZSkim = True
#    WSkim = True
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
    


if(doTreeOnly):
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
#process.prescaler = cms.EDFilter("Prescaler",
#                                    prescaleFactor = cms.int32(prescale),
#                                    prescaleOffset = cms.int32(0)
#                                    )
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
#process.load('Configuration.StandardSequences.AlCaRecoStreams_cff')
process.load('Calibration.ALCARAW_RECO.ALCARECOEcalCalIsolElectron_cff')

process.load('Configuration.EventContent.EventContent_cff')

# added by Shervin for ES recHits (saved as in AOD): large window 15x3 (strip x row)
process.load('RecoEcal.EgammaClusterProducers.interestingDetIdCollectionProducer_cfi')



#process.MessageLogger.cerr.FwkReport.reportEvery = 500
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
    elif(re.match("CMSSW_5_3_3.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START53_V7A::All"
            process.GlobalTag.globaltag = 'START53_V7A::All'
        else:
            #process.GlobalTag.globaltag = 'GR_P_V41::All' # 5_3_3_patch1 Prompt
            process.GlobalTag.globaltag = 'GR_R_53_V9F::All' # GT for 53 rereco (2011)
    elif(re.match("CMSSW_5_3_7.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START53_V24::All"
            process.GlobalTag.globaltag = 'START53_V24::All'
        else:
            process.GlobalTag.globaltag = 'FT_R_53_V21::All' #22Jan rereco
    elif(re.match("CMSSW_5_3_9_patch3",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START53_V7N::All"
            process.GlobalTag.globaltag = 'START53_V7N::All'
        else:
            process.GlobalTag.globaltag = 'FT_R_53_V21::All' #22Jan rereco
    elif(re.match("CMSSW_5_3_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START53_V7A::All"
            process.GlobalTag.globaltag = 'START53_V7A::All'
        else:
            process.GlobalTag.globaltag = 'GR_P_V42B::All' # 5_3_3 Prompt
    elif(re.match("CMSSW_6_1_.*",CMSSW_VERSION)):
        if(MC):
            print "[INFO] Using GT START61_V11::All"
            process.GlobalTag.globaltag = 'START61_V11::All'
        else:
            process.GlobalTag.globaltag = 'GR_P_V42B::All' # 5_3_3 Prompt

    else:
        print "[ERROR]::Global Tag not set for CMSSW_VERSION: ", CMSSW_VERSION
    

###############################
# Event filter sequence: process.filterSeq
# sanbox sequence: process.sandboxSeq + process.alcarecoElectronTracksReducerSeq
# sandbox rereco sequence: process.sandboxRerecoSeq
# alcareco event reduction: process.alcarecoSeq
#
    

################################# FILTERING EVENTS
process.filterSeq = cms.Sequence()
#process.load('Calibration.ALCARAW_RECO.trackerDrivenFinder_cff')
if(options.type == "ALCARECOSIM" and (options.doTree==0 or (options.doTree==1 and options.doTreeOnly==0))):
    # PUDumper
    process.load("Calibration.ALCARAW_RECO.PUDumper_cfi")
    process.TFileService = cms.Service(
        "TFileService",
        fileName = cms.string("PUDumper.root")
        )
    process.filterSeq *= process.PUDumper
    

if (ZSkim):
    process.load('Calibration.ALCARAW_RECO.ZElectronSkimSandbox_cff')
    process.filterSeq *= process.ZeeFilterSeq
elif (WSkim):
    process.load("DPGAnalysis.Skims.WElectronSkim_cff")
    process.filterSeq *= process.elecMetSeq
elif(options.skim=="EleSkim"):
    process.MinEleNumberFilter = cms.EDFilter("CandViewCountFilter",
                                              src = cms.InputTag("gsfElectrons"),
                                              minNumber = cms.uint32(1)
                                              )
    process.filterSeq *= process.MinEleNumberFilter
                

if (HLTFilter):
    import copy
    from HLTrigger.HLTfilters.hltHighLevel_cfi import *
    process.ZEEHltFilter = copy.deepcopy(hltHighLevel)
    process.ZEEHltFilter.throw = cms.bool(False)
    process.ZEEHltFilter.HLTPaths = ["HLT_Ele*"]
    process.filterSeq *= process.ZEEHltFilter



#
# particle flow isolation
#

process.pfiso = cms.Sequence()

###############################
# ECAL Recalibration
###############################

if (options.type=="ALCARAW"):
    process.raw2digi_step = cms.Path(process.RawToDigi)
    process.L1Reco_step = cms.Path(process.L1Reco)
    process.reconstruction_step = cms.Path(process.reconstruction)
    process.endjob_step = cms.EndPath(process.endOfProcess)

    process.load('calibration.ALCARAW_RECO.sandboxSeq_cff')

    # this module provides:
    #process.sandboxSeq  = uncalibRecHitSeq
else:
    # I want to reduce the recHit collections to save space
    process.load('Calibration.ALCARAW_RECO.alCaIsolatedElectrons_cfi')
    #============================== TO BE CHECKED FOR PRESHOWER
    process.load("RecoEcal.EgammaClusterProducers.reducedRecHitsSequence_cff")
    process.reducedEcalRecHitsES.scEtThreshold = cms.double(0.)

    process.reducedEcalRecHitsES.EcalRecHitCollectionES = cms.InputTag('ecalPreshowerRecHit','EcalRecHitsES')
    process.reducedEcalRecHitsES.noFlag = cms.bool(True)
    process.reducedEcalRecHitsES.OutputLabel_ES = cms.string('alCaRecHitsES')

    if(not options.type=="ALCARERECO"):  # in ALCARECO production starting from AOD or RECO the ES recHits are reduced
        process.alcarecoSeq = cms.Sequence(process.alCaIsolatedElectrons) # + process.alcaElectronTracksReducer)
    else:
        process.alcarecoSeq = cms.Sequence(process.alCaIsolatedElectrons + process.reducedEcalRecHitsES)

    #==============================
    
if(options.type=="ALCARERECO"):
    try:
        EcalTrivialConditionRetriever
    except NameError:
        print "well, it WASN'T defined after all!"
        process.trivialCond = cms.Sequence()
    else:
        print "sure, it was defined."
        process.trivialCond = cms.Sequence( EcalTrivialConditionRetriever )
        
    process.load('Calibration.ALCARAW_RECO.sandboxRerecoSeq_cff')
    # this module provides:
    # process.electronRecoSeq
    # process.electronClusteringSeq # with ele-SC reassociation
    # process.sandboxRerecoSeq = (electronRecoSeq * electronClusteringSeq)
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
    if(re.match("CMSSW_6_.*", CMSSW_VERSION)):
        process.alcarerecoSeq=cms.Sequence( process.trivialCond * process.sandboxPFRerecoSeq * process.alcarecoSeq)
    else:
        process.alcarerecoSeq=cms.Sequence( process.trivialCond * process.sandboxRerecoSeq * process.alcarecoSeq)

if(options.type=="ALCARECO"):
    from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso, setupPFMuonIso
    process.eleIsoSequence = setupPFElectronIso(process, 'gsfElectrons')
    process.pfiso = cms.Sequence(process.pfParticleSelectionSequence + process.eleIsoSequence)
    
if((not options.type=="ALCARERECO") ):
    process.load('RecoJets.Configuration.RecoPFJets_cff')
    process.kt6PFJetsForRhoCorrection = process.kt6PFJets.clone(doRhoFastjet = True)
    process.kt6PFJetsForRhoCorrection.Rho_EtaMax = cms.double(2.5)


    if(options.skim!="fromWSkim"):
        process.rhoFastJetSeq = cms.Sequence(process.kt6PFJetsForRhoCorrection * process.pfiso) 
    else:
        process.rhoFastJetSeq = cms.Sequence(process.pfiso)


###########################
# Tree production
###########################
if(options.doTree>0):
    process.load('Calibration.ZNtupleDumper.patSequence_cff')
    process.load("Calibration.ZNtupleDumper.zntupledumper_cfi")
    process.load("Calibration.JsonFilter.jsonFilter_cfi")
    process.zNtupleDumper.recHitCollectionEB = process.patElectrons.reducedBarrelRecHitCollection.value()
    process.zNtupleDumper.recHitCollectionEE = process.patElectrons.reducedEndcapRecHitCollection.value()
    if(options.isSingleEle==1):
	process.zNtupleDumper.isWenu = cms.bool(True)
    if(MC):
        process.ntupleSeq = cms.Sequence(process.jsonFilter * process.patSequenceMC * process.zNtupleDumper)
    else:
        process.ntupleSeq = cms.Sequence(process.jsonFilter * process.patSequence * process.zNtupleDumper)

    if(options.doTree==2 or options.doTree==4 or options.doTree==6 or options.doTree==8):
        process.zNtupleDumper.doStandardTree = cms.bool(False)
    if(options.doTree==2 or options.doTree==3 or options.doTree==6 or options.doTree==7 or options.doTree==10 or options.doTree==11 or options.doTree==14 or options.doTree==15): # it's a bit mask
        process.zNtupleDumper.doExtraCalibTree=cms.bool(True)
    if(options.doTree==4 or options.doTree==5 or options.doTree==6 or options.doTree==7 or options.doTree==12 or options.doTree==13 or options.doTree==14 or options.doTree==15): # it's a bit mask
        process.zNtupleDumper.doEleIDTree=cms.bool(True)
    process.zNtupleDumper.foutName=options.secondaryOutput
    # this includes the sequence: patSequence
    # patSequence=cms.Sequence( (eleSelectionProducers  + eleNewEnergiesProducer ) * patElectrons)

    pathPrefix=CMSSW_BASE+"/"
    process.eleNewEnergiesProducer.regrPhoFile=pathPrefix+process.eleNewEnergiesProducer.regrPhoFile.value()
    process.eleNewEnergiesProducer.regrEleFile=pathPrefix+process.eleNewEnergiesProducer.regrEleFile.value()
    process.eleNewEnergiesProducer.regrEleFile_fra=pathPrefix+process.eleNewEnergiesProducer.regrEleFile_fra.value()
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

################################
# Setting Path
################################
if(options.type=="ALCARAW"):
    process.ZPath = cms.Path( process.filterSeq *
                              (process.rhoFastJetSeq + process.alcarecoElectronTracksReducerSeq +
                               process.sandboxSeq ))
elif(options.type=="ALCARERECO"):
    if(options.doTree>0):
        process.eleRegressionEnergy.inputElectronsTag = cms.InputTag('electronRecalibSCAssociator')
        process.eleSelectionProducers.electronCollection   = 'electronRecalibSCAssociator'
        process.eleNewEnergiesProducer.electronCollection  = 'electronRecalibSCAssociator'
        process.patElectrons.electronSource                = 'electronRecalibSCAssociator'

        if(doTreeOnly):
            process.NtuplePath = cms.Path(process.ntupleSeq)
        else:
            process.ZPath = cms.Path(process.alcarerecoSeq)
            process.NtuplePath = cms.Path(process.alcarerecoSeq * process.ntupleSeq)
    else:
        process.ZPath = cms.Path(process.alcarerecoSeq)
        
elif(options.type == "ALCARECO"):

    if(options.doTree>0):
        if(doTreeOnly):
            process.NtuplePath = cms.Path(process.filterSeq * process.ntupleSeq)
        else:
            process.ZPath = cms.Path( process.filterSeq * ( process.rhoFastJetSeq + process.alcarecoSeq ))
            process.NtuplePath = cms.Path(process.filterSeq *(process.rhoFastJetSeq+process.alcarecoSeq) * process.ntupleSeq)
    else:
        process.ZPath = cms.Path( process.filterSeq * ( process.rhoFastJetSeq + process.alcarecoSeq ))
elif(options.type == "ALCARECOSIM"):
    print "Alcarecosim"
    if(options.doTree>0):
        if(doTreeOnly):
            print "Only ntuple path"
            process.NtuplePath = cms.Path(process.ntupleSeq)
        else:
            process.ZPath = cms.Path( process.filterSeq * ( process.rhoFastJetSeq + process.alcarecoSeq ))
            process.NtuplePath = cms.Path( process.filterSeq * ( process.rhoFastJetSeq + process.alcarecoSeq )
                                           *process.ntupleSeq)

    else:
        process.ZPath = cms.Path( process.filterSeq * ( process.rhoFastJetSeq + process.alcarecoSeq ))


process.load('Calibration.ALCARAW_RECO.ALCARECOEcalCalIsolElectron_Output_cff')

if(options.type=="ALCARAW"):
    from Calibration.ALCARAW_RECO.sandboxOutput_cff import *
    process.OutALCARECOEcalCalElectron.outputCommands +=  sandboxOutputCommands
    
if(options.type == "ALCARERECO"):
    from Calibration.ALCARAW_RECO.sandboxRerecoOutput_cff import *
    process.OutALCARECOEcalCalElectron.outputCommands += sandboxRerecoOutputCommands 

fileName = cms.untracked.string(options.output)

process.output = cms.OutputModule("PoolOutputModule",
                                  # after 5 GB split the file
                                  maxSize = cms.untracked.int32(5120000),
                                  outputCommands = process.OutALCARECOEcalCalElectron.outputCommands,
                                  fileName = fileName,
                                  SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('ZPath')),
                                  dataset = cms.untracked.PSet(
    filterName = cms.untracked.string(''),
    dataTier = cms.untracked.string('ALCARECO')
    )
                                  )

#print "OUTPUTCOMMANDS"
#print process.output.outputCommands

if(not doTreeOnly): 
    process.ALCARECOoutput_step = cms.EndPath(process.output)

debug=False
if(debug):
    process.DEBUGoutput = cms.OutputModule("PoolOutputModule",
                                           # after 5 GB split the file
                                           maxSize = cms.untracked.int32(5120000),
                                           outputCommands = cms.untracked.vstring([ 'keep *_*_*_*' ]),
                                           fileName = cms.untracked.string('/tmp/shervin/patDUMP.root'),
                                           #                                  SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('ZPath, NtuplePath')),
                                           dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('ALCARECO')
        )
                                           )
    
    process.DEBUGoutput_step = cms.EndPath(process.DEBUGoutput)
    
#process.schedule = cms.Schedule(process.zFilterPath,process.ALCARECOoutput_step)

if(options.doTree>0):
    #process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEB")
    #process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alCaRecHitsEE")
    process.eleNewEnergiesProducer.recHitCollectionEB = cms.InputTag("alCaIsolatedElectrons", "alcaBarrelHits")
    process.eleNewEnergiesProducer.recHitCollectionEE = cms.InputTag("alCaIsolatedElectrons", "alcaEndcapHits")
    process.patElectrons.reducedBarrelRecHitCollection = process.eleNewEnergiesProducer.recHitCollectionEB
    process.patElectrons.reducedEndcapRecHitCollection = process.eleNewEnergiesProducer.recHitCollectionEE
    process.zNtupleDumper.recHitCollectionEB = process.eleNewEnergiesProducer.recHitCollectionEB
    process.zNtupleDumper.recHitCollectionEE = process.eleNewEnergiesProducer.recHitCollectionEE
    process.eleRegressionEnergy.recHitCollectionEB = process.eleNewEnergiesProducer.recHitCollectionEB.value()
    process.eleRegressionEnergy.recHitCollectionEE = process.eleNewEnergiesProducer.recHitCollectionEE.value()

    print process.eleNewEnergiesProducer.recHitCollectionEB
    

if(options.doTree>0 and options.doTreeOnly==0):
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


# Schedule definition
if(options.type=='ALCARAW'):
    process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.endjob_step, process.ZPath, process.ALCARECOoutput_step)
else:
    process.schedule = cms.Schedule(process.ZPath, process.ALCARECOoutput_step)
