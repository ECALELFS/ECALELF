#ifndef r9WEIGHTS_HH
#define r9WEIGHTS_HH

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


class r9Weights_class
{
//------------------------------
// All the weights are saved in this kind of map, where the key is the low edge of the bin,
// and the content (the float) is the bin content of the histogram: the weight
// There is one map of each histogram (for each electron category)
	typedef std::map<double, float> r9weight_map_t; // this map corresponds to one histogram
	typedef std::map<TString , r9weight_map_t> r9weights_map_t; // map of the histograms

public:
	r9Weights_class(void);
	~r9Weights_class(void);

	void ReadFromFile(std::string filename);

	TTree *GetTreeWeight(TChain *tree, bool fastLoop = true,
	                     TString etaElebranchName = "etaEle", TString R9ElebranchName = "R9Ele",
	                     TString ptElebranchName = "PtEle");

	double GetWeight(double etaEle_, double R9Ele_);
	double GetPtWeight(double PtEle_);
private:
	TFile *f_r9;

	std::map<TString , r9weight_map_t> r9weights;
	//map<int,double> r9weights;
	std::map<TString, TH1F *> r9HistMap;

	unsigned int warningCounter;
};



#endif

