#ifndef PUWEIGHTS_HH
#define PUWEIGHTS_HH

#include <iostream>
#include <map>

#include <TFile.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TKey.h>
#include <TTree.h>
#include <TChain.h>

#define MAX_PU_REWEIGHT 59
using namespace std;



class puWeights_class
{

public:
	puWeights_class(void);
	~puWeights_class(void);

	void ReadFromFile(std::string filename);
	void ReadFromFiles(std::string mcPUFile, std::string dataPUFile, int runMin = 1);

	TTree *GetTreeWeight(TChain *tree, bool fastLoop = true, TString nPUbranchName = "nPU");

	// made with only on-time PU
	double GetWeight(int nPU, int runNumber = 1);

private:
	//std::map<const int,double> PUweights;
	typedef std::map<int, double> PUweights_t; ///< map of (nPU,weight) for a given run range: weights map
	typedef std::map<int, PUweights_t>  PUWeightsRunDepMap_t; ///< map of (runMin,weights map)
	PUWeightsRunDepMap_t PUWeightsRunDepMap;  ///< map of the weights
	PUWeightsRunDepMap_t::const_iterator PUweights_itr; ///< iterator to the weights of the last run range
	unsigned int warningCounter;
};



#endif

