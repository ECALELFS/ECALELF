// -*- C++ -*-
//
// Package:    JsonFilter
// Class:      JsonFilter
//
/**\class JsonFilter JsonFilter.cc Calibration/JsonFilter/src/JsonFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Shervin Nourbakhsh,32 4-C03,+41227672087,
//         Created:  Mon Jun  3 18:46:45 CEST 2013
// $Id$
//
//
//#define DEBUG
#include <TTree.h>

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>


class JsonFilter : public edm::EDFilter
{
public:
	explicit JsonFilter(const edm::ParameterSet&);
	~JsonFilter();

	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
	virtual void beginJob() ;
	virtual bool filter(edm::Event&, const edm::EventSetup&);
	virtual void endJob() ;

	virtual bool beginRun(edm::Run&, edm::EventSetup const&);
	virtual bool endRun(edm::Run&, edm::EventSetup const&);
	virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
	virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

	void readJsonFile(const std::string& inFileName);
	bool AcceptEventByRunAndLumiSection(const int& runId, const int& lumiId,
	                                    std::map<int, std::vector<std::pair<int, int> > >& jsonMap);
	// ----------member data ---------------------------
	std::string jsonFileName;
	std::map<int, std::vector<std::pair<int, int> > > jsonMap;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
JsonFilter::JsonFilter(const edm::ParameterSet& iConfig):
	jsonFileName(iConfig.getParameter<std::string>("jsonFileName"))
{
	//now do what ever initialization is needed
	// Json file map
	std::cout << "jsonFileName: " << jsonFileName << std::endl;
	if( jsonFileName != "none" && jsonFileName.size() > 1 ) {
		edm::LogInfo("ZNtupleDumper") << "Read json file >>>" << jsonFileName  << "<<<";
		readJsonFile(jsonFileName);
	}

}


JsonFilter::~JsonFilter()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool JsonFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;

	Int_t     	runNumber = iEvent.id().run();
	Int_t         lumiBlock =   iEvent.luminosityBlock();

	return AcceptEventByRunAndLumiSection( runNumber, lumiBlock,
	                                       jsonMap);

}

// ------------ method called once each job just before starting event loop  ------------
void
JsonFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
JsonFilter::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
bool
JsonFilter::beginRun(edm::Run&, edm::EventSetup const&)
{
	return true;
}

// ------------ method called when ending the processing of a run  ------------
bool
JsonFilter::endRun(edm::Run&, edm::EventSetup const&)
{
	return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool
JsonFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
	return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool
JsonFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
	return true;
}





void JsonFilter::readJsonFile(const std::string& inFileName)
{
	jsonMap.clear();
	std::ifstream inFile(inFileName.c_str(), std::ios::in);

	std::string line;
	while(!inFile.eof()) {
		std::string buffer;
		inFile >> buffer;
		line += buffer;
	}



	// loop on Json file
	for(std::string::const_iterator it = line.begin(); it < line.end(); ++it) {
		// find run number
		if( (*(it) == '"') && (*(it + 7) == '"') ) {
			std::string run(it + 1, it + 7);
			//std::cout << "found run " << run << std::endl;



			// find lumi sections
			std::vector<std::pair<int, int> > lumisections;
			for(std::string::const_iterator it2 = it + 10; it2 < line.end(); ++it2) {
				if( (*(it2) == ']') && (*(it2 - 1) == ']') ) break;
				if( *(it2) != '[' ) continue;

				std::string::const_iterator it_beg = it2;
				std::string::const_iterator it_mid;
				std::string::const_iterator it_end;

				for(std::string::const_iterator it3 = it_beg; it3 < line.end(); ++it3) {
					if( *(it3) == ',' ) it_mid = it3;
					if( *(it3) == ']' ) {
						it_end = it3;
						break;
					}
				}

				std::string lumi_beg(it_beg + 1, it_mid);
				std::string lumi_end(it_mid + 1, it_end);
#ifdef DEBUG
				std::cout << "[" << lumi_beg;
				std::cout << ",";
				std::cout << lumi_end << "]" << std::endl;
#endif
				std::pair<int, int> tempLS(atoi(lumi_beg.c_str()), atoi(lumi_end.c_str()));
				lumisections.push_back(tempLS);

				it2 = it_end;
			}

#ifdef DEBUG
			std::cout << run.c_str() << "\t" << atoi(run.c_str()) << "\t"  << std::endl;
#endif
			jsonMap[atoi(run.c_str())] = lumisections;

		} // find run number

	} // loop on Json file



	return;
}



bool JsonFilter::AcceptEventByRunAndLumiSection(const int& runId, const int& lumiId,
        std::map<int, std::vector<std::pair<int, int> > >& jsonMap)
{
	// select by runId
	if(jsonMap.size() == 0) return true;
	if( jsonMap.count(runId) == 0 ) return false;

#ifdef DEBUG
	std::cout << "runNumber = " << runId << std::endl;
#endif

	// select by lumiId
	std::vector<std::pair<int, int> > lumisections = jsonMap[runId];

	int skipEvent = true;
	for(unsigned int i = 0; i < lumisections.size(); ++i)
		if( (lumiId >= lumisections.at(i).first) &&
		        (lumiId <= lumisections.at(i).second) )
			skipEvent = false;

	if( skipEvent == true ) return false;


	return true;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
JsonFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.add<std::string>("jsonFileName", "");
	//desc.setUnknown();
	descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(JsonFilter);
