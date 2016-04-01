#ifndef __eleIDMap__
#define __eleIDMap__

#include<iostream>
#include<map>

class eleIDMap {

 public:
  std::map<std::string,UInt_t> eleIDmap;

  eleIDMap() {

    eleIDmap["fiducial"]          =0x0001;
    eleIDmap["loose"]             =0x0002;
    eleIDmap["medium"]            =0x0004;
    eleIDmap["tight"]             =0x0008;
    eleIDmap["WP90PU"]            =0x0010;

    eleIDmap["WP80PU"]            =0x0020;
    eleIDmap["WP70PU"]            =0x0040;
    eleIDmap["loose25nsRun2"]     =0x0080;
    eleIDmap["medium25nsRun2"]    =0x0100;
    eleIDmap["tight25nsRun2"]     =0x0200;
    eleIDmap["loose50nsRun2"]     =0x0400;
    eleIDmap["medium50nsRun2"]    =0x0800;
    eleIDmap["tight50nsRun2"]     =0x1000;

	// 0T ids
	eleIDmap["medium25nsRun2Boff"] = 0x2000;

	//official eleIDs
	eleIDmap["cutBasedElectronID-Spring15-25ns-V1-standalone-veto"]   = 0x10000;
	eleIDmap["cutBasedElectronID-Spring15-25ns-V1-standalone-loose"]  = 0x20000;
	eleIDmap["cutBasedElectronID-Spring15-25ns-V1-standalone-medium"] = 0x40000;
	eleIDmap["cutBasedElectronID-Spring15-25ns-V1-standalone-tight"]  = 0x80000;

	eleIDmap["cutBasedElectronID-Spring15-50ns-V1-standalone-veto"]   = 0x100000;
	eleIDmap["cutBasedElectronID-Spring15-50ns-V1-standalone-loose"]  = 0x200000;
	eleIDmap["cutBasedElectronID-Spring15-50ns-V1-standalone-medium"] = 0x400000;
	eleIDmap["cutBasedElectronID-Spring15-50ns-V1-standalone-tight"]  = 0x800000;

//'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-loose' 'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium' 'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight' 'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-veto' 'cutBasedElectronID-Spring15-25ns-V1-standalone-loose' 'cutBasedElectronID-Spring15-25ns-V1-standalone-medium' 'cutBasedElectronID-Spring15-25ns-V1-standalone-tight' 'cutBasedElectronID-Spring15-25ns-V1-standalone-veto' 'cutBasedElectronID-Spring15-50ns-V1-standalone-loose' 'cutBasedElectronID-Spring15-50ns-V1-standalone-medium' 'cutBasedElectronID-Spring15-50ns-V1-standalone-tight' 'cutBasedElectronID-Spring15-50ns-V1-standalone-veto' 'eidLoose' 'eidRobustHighEnergy' 'eidRobustLoose' 'eidRobustTight' 'eidTight' 'heepElectronID-HEEPV60' 'mvaEleID-Spring15-25ns-nonTrig-V1-wp80' 'mvaEleID-Spring15-25ns-nonTrig-V1-wp90''cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-loose' 'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium' 'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight' 'cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-veto' 'cutBasedElectronID-Spring15-25ns-V1-standalone-loose' 'cutBasedElectronID-Spring15-25ns-V1-standalone-medium' 'cutBasedElectronID-Spring15-25ns-V1-standalone-tight' 'cutBasedElectronID-Spring15-25ns-V1-standalone-veto' 'cutBasedElectronID-Spring15-50ns-V1-standalone-loose' 'cutBasedElectronID-Spring15-50ns-V1-standalone-medium' 'cutBasedElectronID-Spring15-50ns-V1-standalone-tight' 'cutBasedElectronID-Spring15-50ns-V1-standalone-veto' 'eidLoose' 'eidRobustHighEnergy' 'eidRobustLoose' 'eidRobustTight' 'eidTight' 'heepElectronID-HEEPV60' 'mvaEleID-Spring15-25ns-nonTrig-V1-wp80' 'mvaEleID-Spring15-25ns-nonTrig-V1-wp90'
  }

};

#endif

