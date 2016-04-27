#ifndef SmearingImporter_hh
#define SmearingImporter_hh

#include <iostream>

#include <TChain.h>
#include <TString.h>
#include <TMath.h>
#include <TEntryList.h>

#define NSMEARTOYLIM 31

#include "ZeeEvent.hh"
#include "ElectronCategory_class.hh"
// prende le TChain


// definisce i branch da disabilitare



// prende la lista delle regioni e fa una lista di tagli indicizzati secondo l'ordine delle regioni


// applica i tagli in cascata, se nessun evento passa i tagli l'evento e' buttato, altrimenti viene importato
// e gli viene assegnato l'indice del taglio/regione

class SmearingImporter
{
	typedef zee_events_t event_cache_t;
	typedef std::vector<event_cache_t> regions_cache_t;
public:
	// constructor
	inline SmearingImporter() {};
	SmearingImporter(std::vector<TString> regionList, TString energyBranchName, TString commonCut = "");


	inline void SetRegionList(std::vector<TString> regionList)
	{
		_regionList = regionList;
	};
	regions_cache_t GetCache(TChain *_chain, bool isMC, bool odd, Long64_t nEvents = 0, bool isToy = false, bool externToy = true);
	regions_cache_t GetCacheToy(Long64_t nEvents, bool isMC);

	inline void SetPuWeight(bool value)
	{
		_usePUweight = value;
	};
	inline void SetMCWeight(bool value)
	{
		_useMCweight = value;
	};
	inline void SetR9Weight(bool value)
	{
		_useR9weight = value;
	};
	inline void SetPtWeight(bool value)
	{
		_usePtweight = value;
	};
	inline void SetZPtWeight(bool value)
	{
		_useZPtweight = value;
	};
	inline void SetFsrWeight(bool value)
	{
		_useFSRweight = value;
	};
	inline void SetWeakWeight(bool value)
	{
		_useWEAKweight = value;
	};
	inline void SetExcludeByWeight(bool value)
	{
		_excludeByWeight = value;
	};
	inline void SetOnlyDiagonal(bool value)
	{
		_onlyDiagonal = value;
	};
	inline void SetEleID(TString value)
	{
		_eleID = value;
	};
	inline void SetCommonCut(TString cut)
	{
		_commonCut = cut;
	};
	inline void SetSmearingEt(bool value)
	{
		_isSmearingEt = value;
	};
	inline void SetPdfSystWeight(int value)
	{
		_pdfWeightIndex = value;
	};

	std::vector<TString> _regionList;
	float _scaleToy, _constTermToy;

private:
	//  regions_cache_t _cache; // one entry per region
	//  TChain *_chain;

	TString _energyBranchName;
	TString _commonCut;
	TString _eleID;
	bool _isMC;
	bool _odd;
	bool _usePUweight;
	bool _useMCweight;
	bool _useR9weight;
	bool _usePtweight;
	bool _useZPtweight;
	bool _useFSRweight;
	bool _useWEAKweight;
	bool _excludeByWeight;
	bool _onlyDiagonal;
	bool _isSmearingEt;
	int  _pdfWeightIndex;

	ElectronCategory_class cutter;

	void Import(TTree *chain, regions_cache_t& cache, TString oddString, bool isMC, Long64_t nEvents = 0, bool isToy = false, bool externToy = true);
	void ImportToy(Long64_t nEvents, event_cache_t& eventCache, bool isMC);

};


#endif

