#include "TFile.h"
#include "TH1.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>


class TPileupReweighting
{
private:
	float w[100];
public:
	TPileupReweighting(std::string, std::string);
	~TPileupReweighting();
	double GetWeight(int);
	// ClassDef(TPileupReweighting,1);
};

// default constructor, reading the corrections from file
TPileupReweighting::TPileupReweighting(std::string fname, std::string hname)
{
	TFile weightsFile(fname.c_str(), "READ");
	TH1F* hweights = (TH1F*)weightsFile.Get(hname.c_str());
	for (int ibin = 1; ibin < hweights->GetNbinsX() + 1; ibin++) {
		w[ibin - 1] = hweights->GetBinContent(ibin); // bin 1 --> nvtx = 0
	}
	weightsFile.Close();
	return;
}

TPileupReweighting::~TPileupReweighting()
{
	return;
}

double TPileupReweighting::GetWeight(int npu)
{
	return (w[npu]);
}
