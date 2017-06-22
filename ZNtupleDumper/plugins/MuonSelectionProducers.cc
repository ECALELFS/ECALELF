// -*- C++ -*-
//
// Package:    EleSelectionProducers
// Class:      MuonSelectionProducers
//
/**\class MuonSelectionProducers MuonSelectionProducers.cc Calibration/EleSelectionProducers/src/MuonSelectionProducers.cc

 */
//
// Original Author:  Luca Brianza
//
// $Id$
//
//

//#define DEBUG
// system include files

#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "PhysicsTools/SelectorUtils/interface/strbitset.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "Calibration/ZNtupleDumper/interface/SimpleCutBasedMuonIDSelectionFunctor.h"

#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"

//
// class declaration
//

class MuonSelectionProducers : public edm::EDProducer
{

	//  typedef pat::strbitset SelectionValue_t;
	typedef float SelectionValue_t;
	typedef edm::ValueMap<SelectionValue_t> SelectionMap;
	typedef std::vector< edm::Handle< edm::ValueMap<double> > > IsoDepositValsHandles_t;

public:
	explicit MuonSelectionProducers(const edm::ParameterSet&);
	~MuonSelectionProducers();

	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
	virtual void beginJob() ;
	virtual void produce(edm::Event&, const edm::EventSetup&);
	virtual void endJob() ;

	virtual void beginRun(edm::Run&, edm::EventSetup const&);
	virtual void endRun(edm::Run&, edm::EventSetup const&);
	virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
	virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

	// ----------member data ---------------------------
private:
	edm::Handle<std::vector<reco::Muon> > muonsHandle;
	edm::Handle<reco::ConversionCollection> conversionsHandle;
	edm::Handle<reco::BeamSpot> bsHandle;
	edm::Handle<reco::VertexCollection> vertexHandle;
	edm::Handle< edm::ValueMap<double> > chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle;
	edm::Handle<double> rhoHandle;


	/// input tag for muons
	edm::InputTag muonsTAG;
	// conversions
	edm::InputTag conversionsProducerTAG;
	edm::InputTag BeamSpotTAG;
	edm::InputTag VertexTAG;
	// isolation
	edm::InputTag chIsoValsTAG, emIsoValsTAG, nhIsoValsTAG;
	/// input rho
	edm::InputTag rhoTAG;


	SimpleCutBasedMuonIDSelectionFunctor fiducial_selector;
	SimpleCutBasedMuonIDSelectionFunctor loose_selector;
	SimpleCutBasedMuonIDSelectionFunctor soft_selector;
	SimpleCutBasedMuonIDSelectionFunctor tight_selector;
	SimpleCutBasedMuonIDSelectionFunctor highPT_selector;

};


MuonSelectionProducers::MuonSelectionProducers(const edm::ParameterSet& iConfig):
	muonsTAG(iConfig.getParameter<edm::InputTag>("muonCollection")),
	conversionsProducerTAG(iConfig.getParameter<edm::InputTag>("conversionCollection")),
	BeamSpotTAG(iConfig.getParameter<edm::InputTag>("BeamSpotCollection")),
	VertexTAG(iConfig.getParameter<edm::InputTag>("vertexCollection")),
	chIsoValsTAG(iConfig.getParameter<edm::InputTag>("chIsoVals")),
	emIsoValsTAG(iConfig.getParameter<edm::InputTag>("emIsoVals")),
	nhIsoValsTAG(iConfig.getParameter<edm::InputTag>("nhIsoVals")),
	rhoTAG(iConfig.getParameter<edm::InputTag>("rhoFastJet")),

	fiducial_selector("fiducial", muonsHandle, conversionsHandle, bsHandle, vertexHandle,
	                  chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
	loose_selector("loose", muonsHandle, conversionsHandle, bsHandle, vertexHandle,
	               chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
	soft_selector("soft", muonsHandle, conversionsHandle, bsHandle, vertexHandle,
	              chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
	tight_selector("tight", muonsHandle, conversionsHandle, bsHandle, vertexHandle,
	               chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle),
	highPT_selector("highPT", muonsHandle, conversionsHandle, bsHandle, vertexHandle,
	                chIsoValsHandle, emIsoValsHandle, nhIsoValsHandle, rhoHandle)
{
	//register your products
	/* Examples
	   produces<ExampleData2>();

	   //if do put with a label
	   produces<ExampleData2>("label");

	   //if you want to put into the Run
	   produces<ExampleData2,InRun>();
	*/
	produces< SelectionMap >("fiducial");
	produces< SelectionMap >("loose");
	produces< SelectionMap >("soft");
	produces< SelectionMap >("tight");
	produces< SelectionMap >("highPT");

	//now do what ever other initialization is needed

}


MuonSelectionProducers::~MuonSelectionProducers()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void MuonSelectionProducers::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;

	std::vector<SelectionValue_t>  fiducial_vec;
	std::unique_ptr<SelectionMap> fiducialMap(new SelectionMap());
	std::vector<SelectionValue_t>  loose_vec;
	std::vector<SelectionValue_t>  soft_vec;
	std::vector<SelectionValue_t>  tight_vec;
	std::vector<SelectionValue_t>  highPT_vec;
	std::unique_ptr<SelectionMap> looseMap(new SelectionMap());
	std::unique_ptr<SelectionMap> softMap(new SelectionMap());
	std::unique_ptr<SelectionMap> tightMap(new SelectionMap());
	std::unique_ptr<SelectionMap> highPTMap(new SelectionMap());

	//------------------------------ MUON
	iEvent.getByLabel(muonsTAG, muonsHandle);
	//if(!muonsHandle.isValid()){
	//  std::cerr << "[ERROR] muon collection not found" << std::endl;
	//  return;
	//}

	//------------------------------ CONVERSIONS
	iEvent.getByLabel(conversionsProducerTAG, conversionsHandle);
	//   std::cout << conversionsHandle.isValid() << std::endl;
	iEvent.getByLabel(BeamSpotTAG, bsHandle);
	iEvent.getByLabel(VertexTAG, vertexHandle);
	//------------------------------ RHO
	iEvent.getByLabel(rhoTAG, rhoHandle);

	//------------------------------ ISO DEPOSITS
#ifdef CMSSW_7_2_X
#else
	iEvent.getByLabel(chIsoValsTAG, chIsoValsHandle);
	if(!chIsoValsHandle.isValid()) {
		chIsoValsTAG = edm::InputTag(chIsoValsTAG.label().substr(0, chIsoValsTAG.label().find("PFIso", chIsoValsTAG.label().size() - 6)) + "Gsf", chIsoValsTAG.instance(), chIsoValsTAG.process());
		emIsoValsTAG = edm::InputTag(emIsoValsTAG.label().substr(0, emIsoValsTAG.label().find("PFIso", emIsoValsTAG.label().size() - 6)) + "Gsf", emIsoValsTAG.instance(), emIsoValsTAG.process());
		nhIsoValsTAG = edm::InputTag(nhIsoValsTAG.label().substr(0, nhIsoValsTAG.label().find("PFIso", nhIsoValsTAG.label().size() - 6)) + "Gsf", nhIsoValsTAG.instance(), nhIsoValsTAG.process());

		iEvent.getByLabel(chIsoValsTAG, chIsoValsHandle);
	}
	iEvent.getByLabel(emIsoValsTAG, emIsoValsHandle);
	iEvent.getByLabel(nhIsoValsTAG, nhIsoValsHandle);
#endif

#ifdef DEBUG
	std::cout << "[DEBUG] Starting loop over muons" << std::endl;
#endif
	for(reco::MuonCollection::const_iterator ele_itr = (muonsHandle)->begin();
	        ele_itr != (muonsHandle)->end(); ele_itr++) {
		const reco::MuonRef eleRef(muonsHandle, ele_itr - muonsHandle->begin());

		// the new tree has one event per each muon
		pat::strbitset fiducial_ret;
		pat::strbitset loose_ret;
		pat::strbitset soft_ret;
		pat::strbitset tight_ret;
		pat::strbitset highPT_ret;

		fiducial_selector(eleRef, fiducial_ret);
		fiducial_vec.push_back(fiducial_selector.result());


		loose_selector(eleRef, loose_ret);
		loose_vec.push_back(loose_selector.result());
		soft_selector(eleRef, soft_ret);
		soft_vec.push_back(soft_selector.result());
		tight_selector(eleRef, tight_ret);
		tight_vec.push_back(tight_selector.result());
		highPT_selector(eleRef, highPT_ret);
		highPT_vec.push_back(highPT_selector.result());


		if(((bool)tight_selector.result())) {
			if(!(bool) loose_selector.result() || !(bool) loose_selector.result()) {
				edm::LogError("Incoherent selection") << "passing tight but not loose";
				exit (1);
			}
		}


	}

	//prepare product
	// declare the filler of the ValueMap
	SelectionMap::Filler fiducial_filler(*fiducialMap);
	SelectionMap::Filler loose_filler(*looseMap);
	SelectionMap::Filler soft_filler(*softMap);
	SelectionMap::Filler tight_filler(*tightMap);
	SelectionMap::Filler highPT_filler(*highPTMap);

	//fill and insert valuemap
	fiducial_filler.insert(muonsHandle, fiducial_vec.begin(), fiducial_vec.end());
	loose_filler.insert(muonsHandle, loose_vec.begin(), loose_vec.end());
	soft_filler.insert(muonsHandle, soft_vec.begin(), soft_vec.end());
	tight_filler.insert(muonsHandle, tight_vec.begin(), tight_vec.end());
	highPT_filler.insert(muonsHandle, highPT_vec.begin(), highPT_vec.end());


	fiducial_filler.fill();
	loose_filler.fill();
	soft_filler.fill();
	tight_filler.fill();
	highPT_filler.fill();

	//------------------------------
	// put the ValueMap in the event
	iEvent.put(std::move(fiducialMap), "fiducial");
	iEvent.put(std::move(looseMap), "loose");
	iEvent.put(std::move(softMap), "soft");
	iEvent.put(std::move(tightMap), "tight");
	iEvent.put(std::move(highPTMap), "highPT");

}

// ------------ method called once each job just before starting event loop  ------------
void
MuonSelectionProducers::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
MuonSelectionProducers::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
void
MuonSelectionProducers::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void
MuonSelectionProducers::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void
MuonSelectionProducers::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void
MuonSelectionProducers::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MuonSelectionProducers::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	//   desc.add<edm::InputTag>("muonCollection","Muons");
	//   desc.add<edm::InputTag>("rhoFastJet","kt6PFJetsForRhoCorrection:rho");
	//   desc.add<edm::InputTag>("vertexCollection","offlinePrimaryVertices");
	//   desc.add<edm::InputTag>("conversionCollection","allConversions");
	//   desc.add<edm::InputTag>("BeamSpotCollection","offlineBeamSpot");
	//   desc.add<edm::InputTag>("chIsoVals","elPFIsoValueCharged03PFIdPFIso");
	//   desc.add<edm::InputTag>("emIsoVals","elPFIsoValueGamma03PFIdPFIso");
	//   desc.add<edm::InputTag>("nhIsoVals","elPFIsoValueNeutral03PFIdPFIso");
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonSelectionProducers);
