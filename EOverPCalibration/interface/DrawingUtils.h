#ifndef DrawingUtils_h
#define DrawingUtils_h

#include <iostream>
#include <string>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TColor.h"



void SetLabelAndTitle(TGraphErrors* h);
void SetLabelAndTitle(TH1F* h);
void SetLabelAndTitle(TH2F* h2);



void DrawICMap(TH2F* h2, const std::string& fileName, const std::string& fileType, const bool& isEB = true);



void DrawSpreadHisto(TH1F* h, const std::string& fileName, const std::string& funcName, const std::string& fileType, const bool& isEB);

void DrawSpreadGraph(TGraphErrors* g, const std::string& fileName, const std::string& fileType, const bool& isEB,
                     TGraphErrors* g_stat);



void DrawPhiAvgICSpread(TH1F* h, const std::string& fileName, const std::string& fileType, const bool& isEB);

void DrawAvgICVsPhiGraph(TGraphErrors* g, const std::string& fileName, const std::string& fileType, const Color_t& color, const bool& isEB);

void DrawAvgICVsPhiFoldGraph(TGraphErrors* g, TGraphErrors* g2, const std::string& fileName, const std::string& fileType, const bool& isEB);



void DrawResidualGraph(TGraphErrors* g, const std::string& fileName, const std::string& fileType, const bool& isEB);

#endif
