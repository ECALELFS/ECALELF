import FWCore.ParameterSet.Config as cms

electron_energy_modifications = cms.VPSet(
    cms.PSet( modifierName    = cms.string('EGExtraInfoModifierFromFloatValueMaps'),
              electron_config = cms.PSet( 
            electronSrc = cms.InputTag("slimmedElectrons","","@skipCurrentProcess"),
            energySCEleMust = cms.InputTag("eleNewEnergiesProducer","energySCEleMust"),
            energySCEleMustVar = cms.InputTag("eleNewEnergiesProducer","energySCEleMustVar"),
            energySCElePho = cms.InputTag("eleNewEnergiesProducer","energySCElePho"),
            energySCElePhoVar = cms.InputTag("eleNewEnergiesProducer","energySCElePhoVar"),
            fiducial = cms.InputTag("eleSelectionProducers", "fiducial"),
            WP70PU      = cms.InputTag("eleSelectionProducers", "WP70PU"),
            WP80PU      = cms.InputTag("eleSelectionProducers", "WP80PU"),
            WP90PU      = cms.InputTag("eleSelectionProducers", "WP90PU"),
            loose       = cms.InputTag("eleSelectionProducers", "loose"),
            medium      = cms.InputTag("eleSelectionProducers", "medium"),
            tight      = cms.InputTag("eleSelectionProducers", "tight"),
            loose25nsRun2       = cms.InputTag("eleSelectionProducers", "loose25nsRun2"),
            medium25nsRun2       = cms.InputTag("eleSelectionProducers", "medium25nsRun2"),
            tight25nsRun2       = cms.InputTag("eleSelectionProducers", "tight25nsRun2"),
            loose50nsRun2       = cms.InputTag("eleSelectionProducers", "loose50nsRun2"),
            medium50nsRun2       = cms.InputTag("eleSelectionProducers", "medium50nsRun2"),
            tight50nsRun2       = cms.InputTag("eleSelectionProducers", "tight50nsRun2"),
            medium25nsRun2Boff = cms.InputTag("eleSelectionProducers", "medium25nsRun2Boff"),
            ),
              photon_config   = cms.PSet( )
              )
    )
