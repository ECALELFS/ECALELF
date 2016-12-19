import FWCore.ParameterSet.Config as cms

def MCPuDumper(process):
    process.load("Calibration.EcalAlCaRecoProducers.PUDumper_cfi")
    process.PUDumperSeq = cms.Sequence()
    process.TFileService = cms.Service(
        "TFileService",
        fileName = cms.string("PUDumper.root")
        )
    process.PUDumperSeq *= process.PUDumper

    process.pudumperpath = cms.Path(process.PUDumperSeq)
    process.schedule.append(process.pudumperpath)
    return process

