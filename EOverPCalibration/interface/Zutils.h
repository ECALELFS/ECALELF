#ifndef ntupleUtils_h
#define ntupleUtils_h

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


#include "TFile.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TH1F.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TMath.h"


#ifdef _MAKECINT_
#pragma link C++ class map<int,TH1F*>+;
#pragma link C++ class map<string,TH1F*>+;
#pragma link C++ class map<string,RooDataSet*>+;
#endif



/*** breit-wigner ***/
double breitWigner(double* x, double* par);

/*** crystall ball with low tail ***/
double crystalBallLow(double* x, double* par);

/*** double crystall ball ***/
double crystalBallLowHigh(double* x, double* par);

/*** breit-wigner convoluted with crystalBall ***/

double breitWigner_crystalBallLow(double* x, double* par, const int nPoints);


std::pair<double, double> breitWigner_crystalBallLowFWHM(TF1* bwcb, const double &min, const double &max, const double &Precision = 0.001, const int & maxCycle = 100000);

void MoveInterval(TF1* bwcb, const double & xCenter, std::pair<double, double> & Interval, const double & MAX);


/*** Method for binned and ubinned lineshape fit ***/

void BinnedFitZPeak(const std::string& category, const int& rebin, TH1F* h_mZ_DATA, TH1F* h_mZ_MC,
                    int nPoints, const double &min, const double &max, std::string energyType = "Reg");

void BinnedFitZPeak(const std::string& category, const int& rebin, TH1F* h_mZ_DATA,
                    int nPoints, const double &min, const double &max, std::string energyType = "Reg");


void SetParameterFunctionEE_EE(TF1* bw_cb, TRandom3 * rand);
void SetParameterFunctionEB_EE(TF1* bw_cb, TRandom3 * rand);
void SetParameterFunctionEB_EB(TF1* bw_cb, TRandom3 * rand);

#endif
