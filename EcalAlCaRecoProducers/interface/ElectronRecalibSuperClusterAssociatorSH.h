#ifndef ElectronRecalibSuperClusterAssociatorSH_h
#define ElectronRecalibSuperClusterAssociatorSH_h

//
// Package:         RecoEgamma/EgammaElectronProducers
// Class:           ElectronRecalibSuperClusterAssociatorSH
//
// Description:


#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>

class PixelMatchElectronAlgo;

class ElectronRecalibSuperClusterAssociatorSH : public edm::EDProducer
{
public:

	explicit ElectronRecalibSuperClusterAssociatorSH(const edm::ParameterSet& conf);

	virtual ~ElectronRecalibSuperClusterAssociatorSH();

	virtual void produce(edm::Event& e, const edm::EventSetup& c);

private:

	std::string scProducer_;
	std::string scCollection_;

	std::string scIslandProducer_;
	std::string scIslandCollection_;

	edm::InputTag electronSrc_;

};
#endif
