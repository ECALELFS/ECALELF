#include "CommonTools/CandAlgos/interface/ModifyObjectValueBase.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

namespace
{
const edm::InputTag empty_tag;
}

#include <unordered_map>

class EleIDModifierFromValueMaps : public ModifyObjectValueBase
{
public:
	typedef edm::EDGetTokenT<edm::ValueMap<float> > ValMapFloatToken;
	typedef std::unordered_map<std::string, ValMapFloatToken> ValueMaps;
	typedef std::unordered_map<std::string, edm::InputTag> ValueMapsTags;
	struct electron_config {
		edm::InputTag electron_src;
		edm::EDGetTokenT<edm::View<pat::Electron> > tok_electron_src;
		ValueMapsTags valuemaps;
		ValueMaps tok_valuemaps;
	};

	EleIDModifierFromValueMaps(const edm::ParameterSet& conf);

	void setEvent(const edm::Event&) override final;
	void setEventContent(const edm::EventSetup&) override final;
	void setConsumes(edm::ConsumesCollector&) override final;

	void modifyObject(pat::Electron&) const override final;

private:
	electron_config e_conf;
	std::unordered_map<unsigned, edm::Ptr<reco::GsfElectron> > eles_by_oop; // indexed by original object ptr
	std::unordered_map<unsigned, edm::Handle<edm::ValueMap<float> > > ele_vmaps;
	mutable unsigned ele_idx; // hack here until we figure out why some slimmedPhotons don't have original object ptrs
};

DEFINE_EDM_PLUGIN(ModifyObjectValueFactory,
                  EleIDModifierFromValueMaps,
                  "EleIDModifierFromValueMaps");

EleIDModifierFromValueMaps::
EleIDModifierFromValueMaps(const edm::ParameterSet& conf) :
	ModifyObjectValueBase(conf)
{
	constexpr char electronSrc[] =  "electronSrc";

	if( conf.exists("electron_config") ) {
		const edm::ParameterSet& electrons = conf.getParameter<edm::ParameterSet>("electron_config");
		if( electrons.exists(electronSrc) ) e_conf.electron_src = electrons.getParameter<edm::InputTag>(electronSrc);
		const std::vector<std::string> parameters = electrons.getParameterNames();
		for( const std::string& name : parameters ) {
			if( std::string(electronSrc) == name ) continue;
			if( electrons.existsAs<edm::InputTag>(name) ) {
				e_conf.valuemaps[name] = electrons.getParameter<edm::InputTag>(name);
			}
		}
	}
	ele_idx = 0;
}

namespace
{
inline void get_product(const edm::Event& evt,
                        const edm::EDGetTokenT<edm::ValueMap<float> >& tok,
                        std::unordered_map<unsigned, edm::Handle<edm::ValueMap<float> > >& map)
{
	evt.getByToken(tok, map[tok.index()]);
}
}

void EleIDModifierFromValueMaps::
setEvent(const edm::Event& evt)
{
	eles_by_oop.clear();
	ele_vmaps.clear();

	ele_idx = 0;

	if( !e_conf.tok_electron_src.isUninitialized() ) {
		edm::Handle<edm::View<pat::Electron> > eles;
		evt.getByToken(e_conf.tok_electron_src, eles);

		for( unsigned i = 0; i < eles->size(); ++i ) {
			edm::Ptr<pat::Electron> ptr = eles->ptrAt(i);
			eles_by_oop[i] = ptr;
		}
	}

	for( auto itr = e_conf.tok_valuemaps.begin(); itr != e_conf.tok_valuemaps.end(); ++itr ) {
		get_product(evt, itr->second, ele_vmaps);
	}

}

void EleIDModifierFromValueMaps::setEventContent(const edm::EventSetup& evs)
{
}

namespace
{
template<typename T, typename U, typename V>
inline void make_consumes(T& tag, U& tok, V& sume)
{
	if( !(empty_tag == tag) ) tok = sume.template consumes<edm::ValueMap<float> >(tag);
}
}

void EleIDModifierFromValueMaps::setConsumes(edm::ConsumesCollector& sumes)
{
	//setup electrons
	if( !(empty_tag == e_conf.electron_src) ) e_conf.tok_electron_src = sumes.consumes<edm::View<pat::Electron> >(e_conf.electron_src);

	for( auto itr = e_conf.valuemaps.begin(); itr != e_conf.valuemaps.end(); ++itr ) {
		make_consumes(itr->second, e_conf.tok_valuemaps[itr->first], sumes);
	}

}

namespace
{
template<typename T, typename U, typename V>
inline void assignValue(const T& ptr, const U& tok, const V& map, float& value)
{
	if( !tok.isUninitialized() ) value = map.find(tok.index())->second->get(ptr.id(), ptr.key());
}
}

void EleIDModifierFromValueMaps::modifyObject(pat::Electron& ele) const
{
	// we encounter two cases here, either we are running AOD -> MINIAOD
	// and the value maps are to the reducedEG object, can use original object ptr
	// or we are running MINIAOD->MINIAOD and we need to fetch the pat objects to reference
	edm::Ptr<reco::Candidate> ptr(ele.originalObjectRef());
	if( !e_conf.tok_electron_src.isUninitialized() ) {
		auto key = eles_by_oop.find(ele_idx);
		if( key != eles_by_oop.end() ) {
			ptr = key->second;
		} else {
			throw cms::Exception("BadElectronKey")
			        << "Original object pointer with key = " << ele.originalObjectRef().key()
			        << " not found in cache!";
		}
	}

	auto eleIDs = ele.electronIDs();

	//now we go through and modify the objects using the valuemaps we read in
	for( auto itr = e_conf.tok_valuemaps.begin(); itr != e_conf.tok_valuemaps.end(); ++itr ) {
		float value(0.0);
		assignValue(ptr, itr->second, ele_vmaps, value);
		if( !ele.isElectronIDAvailable(itr->first) ) {
			pat::Electron::IdPair id_pair(itr->first, value);
			eleIDs.push_back(id_pair);
		} else {
			throw cms::Exception("ValueNameAlreadyExists")
			        << "Trying to add new UserFloat = " << itr->first
			        << " failed because it already exists!";
		}
	}
	ele.setElectronIDs(eleIDs);
	++ele_idx;
}

