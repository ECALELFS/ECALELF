#ifndef ConvoluteTemplate_h
#define ConvoluteTemplate_h

#include "histoFunc.h"

#include "TH1.h"
#include "TF1.h"
#include "TVirtualFFT.h"


TH1F* ConvoluteTemplate(const std::string& name, TH1F* h_template, TH1F* h_smearing,
                        int nPoints, double min, double max);

#endif
