import FWCore.ParameterSet.Config as cms

LTmin=5
LTmax=75

###MC check efficiency
genEleFromZnoAccept = cms.EDFilter("CandViewSelector",
                              src = cms.InputTag("prunedGenParticles"),
                              cut = cms.string("(pdgId == 11 || pdgId == -11)  && (mother(0).pdgId == 23 || mother(0).pdgId == 2)"),
                              )

minGenEleFromZNumberFilter = cms.EDFilter("CandViewCountFilter",
                                  src = cms.InputTag("genEleFromZnoAccept"),
                                  minNumber = cms.uint32(2)
                                  )

ZLTFiltered = cms.EDProducer("CandViewShallowCloneCombiner",
                                 decay = cms.string("genEleFromZnoAccept genEleFromZnoAccept"),
                                 checkCharge = cms.bool(False),
                                 cut = cms.string("50 < mass && "+ str(LTmin) + " < daughter(0).pt+daughter(1).pt < "+str(LTmax)),
                                 )

ltFilter = cms.EDFilter("CandViewCountFilter",
                        src = cms.InputTag("ZLTFiltered"),
                        minNumber = cms.uint32(1)
                        )
stopFilter = cms.EDFilter("CandViewCountFilter",
                        src = cms.InputTag("ZLTFiltered"),
                        minNumber = cms.uint32(10)
                        )

ZLTFiltered5lt75 = ZLTFiltered.clone(
    cut = cms.string("50 < mass &&  daughter(0).pt+daughter(1).pt < "+str(65))
#    cut = cms.string("50 < mass && "+ str(5) + " < daughter(0).pt+daughter(1).pt < "+str(75))
    )
ZLTFiltered75lt80 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(75) + " < daughter(0).pt+daughter(1).pt < "+str(80))
    )
ZLTFiltered80lt85 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(80) + " < daughter(0).pt+daughter(1).pt < "+str(85))
    )
ZLTFiltered85lt90 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(85) + " < daughter(0).pt+daughter(1).pt < "+str(90))
    )
ZLTFiltered90lt95 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(90) + " < daughter(0).pt+daughter(1).pt < "+str(95))
    )
ZLTFiltered95lt100 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(95) + " < daughter(0).pt+daughter(1).pt < "+str(100))
    )
ZLTFiltered100lt200 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(100) + " < daughter(0).pt+daughter(1).pt < "+str(200))
    )
ZLTFiltered200lt400 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(200) + " < daughter(0).pt+daughter(1).pt < "+str(400))
    )
ZLTFiltered400lt800 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(400) + " < daughter(0).pt+daughter(1).pt < "+str(800))
    )
ZLTFiltered800lt2000 = ZLTFiltered.clone(
    cut = cms.string("50 < mass && "+ str(800) + " < daughter(0).pt+daughter(1).pt < "+str(2000))
    )

ltFilter5lt75    = ltFilter.clone(src  = cms.InputTag('ZLTFiltered5lt75'))
ltFilter75lt80   = ltFilter.clone(src  = cms.InputTag('ZLTFiltered75lt80'))
ltFilter80lt85   = ltFilter.clone(src  = cms.InputTag('ZLTFiltered80lt85'))
ltFilter85lt90   = ltFilter.clone(src  = cms.InputTag('ZLTFiltered85lt90'))
ltFilter90lt95   = ltFilter.clone(src  = cms.InputTag('ZLTFiltered90lt95'))
ltFilter95lt100  = ltFilter.clone(src  = cms.InputTag('ZLTFiltered95lt100'))
ltFilter100lt200 = ltFilter.clone(src  = cms.InputTag('ZLTFiltered100lt200'))
ltFilter200lt400 = ltFilter.clone(src  = cms.InputTag('ZLTFiltered200lt400'))
ltFilter400lt800 = ltFilter.clone(src  = cms.InputTag('ZLTFiltered400lt800'))
ltFilter800lt2000 = ltFilter.clone(src = cms.InputTag('ZLTFiltered800lt2000'))



############################################################
# Sequences
##############################
ltPreFilterSeq = cms.Sequence(genEleFromZnoAccept * minGenEleFromZNumberFilter)

