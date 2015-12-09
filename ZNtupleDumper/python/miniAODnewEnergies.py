import FWCore.ParameterSet.Config as cms

electron_energy_modifications = cms.VPSet(
    cms.PSet( modifierName    = cms.string('EGExtraInfoModifierFromFloatValueMaps'),
              electron_config = cms.PSet( 
            electronSrc = cms.InputTag("slimmedElectrons","","@skipCurrentProcess"),
            energySCEleMust = cms.InputTag("eleNewEnergiesProducer","energySCEleMust"),
            energySCEleMustVar = cms.InputTag("eleNewEnergiesProducer","energySCEleMustVar"),
            energySCElePho = cms.InputTag("eleNewEnergiesProducer","energySCElePho"),
            energySCElePhoVar = cms.InputTag("eleNewEnergiesProducer","energySCElePhoVar")
            ),
              photon_config   = cms.PSet( )
              )
    )
