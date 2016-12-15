#ifndef CalibrationUtils_h
#define CalibrationUtils_h

#include <TGraphErrors.h>
#include <TH2F.h>
#include "../interface/TEndcapRings.h"


void DrawCorr_EE(TH2F* h_scale_EEM, TH2F* h_scale_EEP, TH2F* hcmap_EEM, TH2F* hcmap_EEP, const std::vector< std::pair<int, int> > & TT_centre_EEM, const  std::vector< std::pair<int, int> > & TT_centre_EEP, std::vector<std::vector<TGraphErrors*> > & correctionMomentum, TEndcapRings *eRings, bool skip = true, int nEtaBinsEE = 5, float etaMin = 1.4, float etaMax = 2.5);

void DrawICCorr_EE(TH2F* h_scale_EEM, TH2F* h_scale_EEP, TH2F* hcmap_EEM, TH2F* hcmap_EEP, const std::vector< std::pair<int, int> > & TT_centre_EEM, const  std::vector< std::pair<int, int> > & TT_centre_EEP, std::vector<std::vector<TGraphErrors*> > & correctionMomentum, TEndcapRings *eRings, bool skip = true, int nEtaBinsEE = 5, float etaMin = 1.4, float etaMax = 2.5, int shift = 0);
//############## ECAL BARREL ####################

/// Check if the crystal is near to a dead one
bool CheckxtalIC_EB (TH2F* h_scale_EB, int iPhi, int iEta );

/// Check if the crystal is near to a dead TT
bool CheckxtalTT_EB (int iPhi, int iEta, const std::vector<std::pair<int, int> >& TT_centre );

/// Initialize TT dead map
void InitializeDeadTT_EB(std::vector<std::pair<int, int> >& TT_centre);

void InitializeDeadTT_EB2012(std::vector<std::pair<int, int> >& TT_centre);


/// Normalize IC vs ring EB
void NormalizeIC_EB(TH2F* h_scale_EB, TH2F* hcmap, const std::vector< std::pair<int, int> > & TT_centre, bool skip = true);

/// Normalize IC vs LMR EB
void NormalizeIC_LMR_EB(TH2F* h_scale_EB, TH2F* hcmap, const std::vector< std::pair<int, int> > & TT_centre, bool skip = true);

/// Normalize IC vs SM EB
void NormalizeIC_SM_EB(TH2F* h_scale_EB, TH2F* hcmap, const std::vector< std::pair<int, int> > & TT_centre, bool skip = true);

/// Book spread Histos
void BookSpreadHistos_EB(TH1F* h_spread, std::vector<TH1F*>& h_spread_vsEtaFold, TGraphErrors* g_spread_vsEtaFold, const int& etaRingWidth,
                         const std::string& name, const int& nBins_spread, const float& spreadMin, const float& spreadMax,
                         TH2F* hcmap, TH2F* hcmap2 = NULL);

/// Phi Projection EB
void PhiProfile(TH1F* h_phiAvgICSpread, TGraphErrors* g_avgIC_vsPhi, const int& phiRegionWidth,
                TH2F* hcmap, TEndcapRings* eRings = NULL);

void PhiFoldProfile_EB(TGraphErrors* g_avgIC_vsPhiFold_EBM, TGraphErrors* g_avgIC_vsPhiFold_EBP, const int& phiRegionWidth,
                       TH2F* hcmap);

/// Residual Spread
void ResidualSpread (TGraphErrors *statprecision, TGraphErrors *Spread, TGraphErrors *Residual);

//################# ECAL ENDCAPS #####################

/// check if the xtal is near to a dead one
bool CheckxtalIC_EE(TH2F* h_scale_EE, int ix, int iy, int ir);

/// check if the xtal is neat to a dead TT
bool CheckxtalTT_EE(int ix, int iy, int ir, const std::vector<std::pair<int, int> >& TT_centre );

/// Map dead TT EE+
void InitializeDeadTTEEP(std::vector<std::pair<int, int> >& TT_centre);

void InitializeDeadTTEEP2012(std::vector<std::pair<int, int> >& TT_centre);

/// Map dead TT EE-
void InitializeDeadTTEEM(std::vector<std::pair<int, int> >& TT_centre);

void InitializeDeadTTEEM2012(std::vector<std::pair<int, int> >& TT_centre);

/// Normalize in function of ring
void NormalizeIC_EE(TH2F* h_scale_EEM, TH2F* h_scale_EEP, TH2F* hcmap_EEM, TH2F* hcmap_EEP, const std::vector< std::pair<int, int> > & TT_centre_EEM, const  std::vector< std::pair<int, int> > & TT_centre_EEP, TEndcapRings *eRings, bool skip = true);

/// Book spread  Histos
void BookSpreadHistos_EE(std::map<int, TH1F*>& h_spread, std::map<int, std::vector<TH1F*> >& h_spread_vsEtaFold, std::map<int, TGraphErrors*>& g_spread_vsEtaFold,
                         TEndcapRings* eRings, const int& etaRingWidth,
                         const std::string& name, const int& nBins_spread, const float& spreadMin, const float& spreadMax,
                         std::map<int, TH2F*>& hcmap, std::map<int, TH2F*>& hcmap2);

/// Book spread stat Histos
void BookSpreadStatHistos_EE(TH2F** hcmap2, TH2F** hcmap3, TH1F ***hstatprecision, TH1F **hstatprecisionAll,  TEndcapRings *eRings);

/// Phi Projection EB
void PhiProfileEE(TGraphErrors *phiProjection, TGraphErrors **MomentumScale, TH2F* hcmap, TEndcapRings *eRings, const int & iz);




int GetNRegionsEB(const std::string& type);
int templIndexEB(const std::string& type, const float& eta, const float& charge, const float& R9);

int GetNRegionsEE(const std::string& type);
int templIndexEE(const std::string& type, const float& eta, const float& charge, const float& R9);

#endif
