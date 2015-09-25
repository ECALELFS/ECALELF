#ifndef stabilityUtils_h
#define stabilityUtils_h

#include "histoFunc.h"
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

#include <sstream>
#include <ctime>

#include "TH1F.h"
#include "TF1.h"

#include <iostream>
#include <string>

int dateToInt(const std::string& date);

void SetHistoStyle(TH1* h, const std::string& label = "");

TH1F* MellinConvolution(const std::string& name, TH1F* h_template, TH1F* h_Las);

#endif
