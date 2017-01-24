import FWCore.ParameterSet.Config as cms
import os, sys, imp, re
import FWCore.ParameterSet.VarParsing as VarParsing
import subprocess
import copy

from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet


############################################################
### SETUP OPTIONS
options = VarParsing.VarParsing('standard')
options.register ('tagFile',
                  "",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "path of the file with the reReco tags")

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

processName = "MINIAODNTUPLE"

CMSSW_VERSION=os.getenv("CMSSW_VERSION")
CMSSW_BASE=os.getenv("CMSSW_BASE")

process = cms.Process(processName)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff') ## please double check
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#FrontierConditions_GlobalTag_cff
# import of ALCARECO sequences
process.load('Calibration.EcalAlCaRecoProducers.LTFilter_cff')

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
    limit = cms.untracked.int32(100)
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

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.files),
                            secondaryFileNames = cms.untracked.vstring(options.secondaryFiles),
                            skipEvents=cms.untracked.uint32(0),
#                            inputCommands = cms.untracked.vstring( [ 'keep *', 
#                                                                     'drop *_correctedHybridSuperClusters_*_RECO',
#                                                                     'drop *_correctedMulti5x5SuperClustersWithPreshower_*_RECO',
#                                                                     ]
#                                                                   ),
#                             dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
                            )


process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
#    SkipEvent = cms.untracked.vstring('ProductNotFound'),
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
        
#assuming a Run2 release
    if (MC):
        print "[INFO] Using GT auto:run2_mc"
        from Configuration.AlCa.GlobalTag import GlobalTag
        process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
    else:
        print "[INFO] Using GT auto:run2_data"
        from Configuration.AlCa.GlobalTag import GlobalTag
        process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
        if(options.files==""):
            process.source.fileNames=[ 'root://cms-xrd-global.cern.ch//store/data/Run2012D/DoubleElectron/AOD/15Apr2014-v1/00000/0EA11D35-0CD5-E311-862E-0025905A6070.root' ]
            



process.lt5lt75Path     = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered5lt75 *     process.ltFilter5lt75    )
process.lt75lt80Path    = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered75lt80 *    process.ltFilter75lt80   )
process.lt80lt85Path    = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered80lt85 *    process.ltFilter80lt85   )
process.lt85lt90Path    = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered85lt90 *    process.ltFilter85lt90   )
process.lt90lt95Path    = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered90lt95 *    process.ltFilter90lt95   )
process.lt95lt100Path   = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered95lt100 *   process.ltFilter95lt100  )
process.lt100lt200Path  = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered100lt200 *  process.ltFilter100lt200 )
process.lt200lt400Path  = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered200lt400 *  process.ltFilter200lt400 )
process.lt400lt800Path  = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered400lt800 *  process.ltFilter400lt800 )
process.lt800lt2000Path = cms.Path(process.ltPreFilterSeq * process.ZLTFiltered800lt2000 * process.ltFilter800lt2000 )





############################
## Dump the output Python ##
############################
#process.ecalRecHit.recoverEBIsolatedChannels = True
processDumpFile = open('processDump.py', 'w')
print >> processDumpFile, process.dumpPython()

#cmsRun test/ltFilter_cfg.py tagFile=$PWD/config/reRecoTags/80X_mcRun2_asymptotic_2016_miniAODv2.py maxEvents=-1  files=`cat miniaod.list`
# take the lines TrigReport and replace Executed with 
# visited events fro the LT filters ZLTFiltered95lt100
# put the output in num2.dat
#awk '(!/#/ && $5>0){N=$4;n=$5;eff=n/N; err=sqrt(eff*(1-eff)/N); print $8, eff, err, err/eff*100}' num2.dat |column -t

#             efficiency error     relative error   
# lt5lt75Path 0.46966 0.000904072 0.192495
# lt75lt80Path 0.0788372 0.000488166 0.619208
# lt80lt85Path 0.0961797 0.000534093 0.555307
# lt85lt90Path 0.131629 0.000612439 0.465275
# lt90lt95Path 0.125296 0.000599699 0.478625
# lt95lt100Path 0.0390527 0.000350921 0.898583
# lt100lt200Path 0.0572386 0.000420803 0.735174
# lt200lt400Path 0.00168667 7.43332e-05 4.40709
# lt400lt800Path 5.90664e-05 1.39217e-05 23.5695

