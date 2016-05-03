#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>

class TMomentumCalibration
{
private:
	TGraphErrors *gdata;
	TGraphErrors *gmc;
public:
	TMomentumCalibration(std::string);
	~TMomentumCalibration();
	double GetMomentumCalibration(int, int);
	// ClassDef(TMomentumCalibration,1);
};

// default constructor, reading the corrections from file
TMomentumCalibration::TMomentumCalibration(std::string fname)
{
	TFile *fP = TFile::Open(fname.c_str(), "READ");
	std::cout << "Reading momentum calibration from :" <<  fname.c_str()  << std::endl;
	gdata   = (TGraphErrors*)fP->Get("g_EoC");
	gmc     = (TGraphErrors*)fP->Get("g_EoP");
	return;
}

TMomentumCalibration::~TMomentumCalibration()
{
	return;
}

double TMomentumCalibration::GetMomentumCalibration(int ieta, int isData)
{
	double scale;
	// NB: i graps contengono la scala di 1/p.
	if ( isData) scale = gdata -> Eval(ieta);
	if (!isData) scale = gmc   -> Eval(ieta);
	if (scale == 0) scale = 1.;
	return (scale);
}
