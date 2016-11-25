#ifndef ZPTWEIGHTS_HH
#define ZPTWEIGHTS_HH

#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <map>
#include <math.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TKey.h>
#include <TTree.h>
#include <TChain.h>

using namespace std;


class ZPtWeights_class
{
//------------------------------
// All the weights are saved in this kind of map, where the key is the low edge of the bin,
// and the content (the float) is the bin content of the histogram: the weight
// There is one map of each histogram (for each electron category)
	typedef std::map<double, float> ZPtweight_map_t; // this map corresponds to one histogram
	typedef std::map<TString , ZPtweight_map_t> ZPtweights_map_t; // map of the histograms

public:
	ZPtWeights_class(void);
	~ZPtWeights_class(void);

	void ReadFromFile(std::string filename);

	TTree *GetTreeWeight(TChain *tree,  TString ZPtBranchName, bool fastLoop = true);

	double GetPtWeight(double ZPt_, int pdfWeightIndex); //all array of two elements (the two electrons)
private:
	TFile *f_ZPt;

	std::map<TString , ZPtweight_map_t> ZPtweights;
	//map<int,double> ZPtweights;
	std::map<TString, TH1F *> ZPtHistMap;

	unsigned int warningCounter;
};



#endif

