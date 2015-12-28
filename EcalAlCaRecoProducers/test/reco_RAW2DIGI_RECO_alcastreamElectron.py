# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: reco -s RAW2DIGI,RECO -n 100 --filein=/store/data/Run2012D/SingleElectron/RAW/v1/000/208/307/0085A34B-BD3A-E211-B6E9-003048D2BC4C.root --data --conditions=auto:run2_data --nThreads=4 --dirout=./
import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/data/Run2015D/AlCaElectron/RAW/v1/000/256/673/00000/5AD13995-DF5C-E511-8DDB-02163E013516.root'),
#    fileNames = cms.untracked.vstring('root://xrootd.unl.edu//store/data/Run2015C/AlCaElectron/RAW/v1/000/254/313/00000/88935D49-C842-E511-BA1F-02163E014330.root'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reco nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('./reco_RAW2DIGI_RECO.root'),
    outputCommands = process.RECOSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

### some fix for the stream                                                                                                                                       
process.csctfDigis.producer       = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.dttfDigis.DTTF_FED_Source = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.ecalDigis.InputLabel      = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.ecalPreshowerDigis.sourceTag = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.castorDigis.InputLabel    = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.gctDigis.inputLabel       = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.gtDigis.DaqGtInputTag     = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.gtEvmDigis.EvmGtInputTag  = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.hcalDigis.InputLabel      = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.muonCSCDigis.InputObjects = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.muonDTDigis.inputLabel    = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.muonRPCDigis.InputLabel   = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.scalersRawToDigi.scalersInputTag = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.siPixelDigis.InputLabel   = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

process.siStripDigis.ProductLabel = cms.InputTag("hltSelectedElectronFEDListProducerGsf:StreamElectronRawFed")

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.RECOSIMoutput_step)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(4)
process.options.numberOfStreams=cms.untracked.uint32(0)


