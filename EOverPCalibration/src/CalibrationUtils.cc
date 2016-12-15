#include "../interface/CalibrationUtils.h"
#include <fstream>
#include <TF1.h>




void DrawCorr_EE(TH2F* h_scale_EEM, TH2F* h_scale_EEP,
                 TH2F* hcmap_EEM, TH2F* hcmap_EEP,
                 const std::vector< std::pair<int, int> >& TT_centre_EEM,
                 const std::vector< std::pair<int, int> >& TT_centre_EEP,
                 std::vector<std::vector<TGraphErrors*> >& correctionMomentum, TEndcapRings* eRings, bool skip , int nEtaBinsEE, float etaMin, float etaMax)
{
	std::map<int, TH2F*> h_scale_EE;
	std::map<int, TH2F*> hcmap_EE;

	h_scale_EE[0] = h_scale_EEM;
	h_scale_EE[1] = h_scale_EEP;

	hcmap_EE[0] = hcmap_EEM;
	hcmap_EE[1] = hcmap_EEP;



	std::map<int, std::vector<float> > sumIC;
	std::map<int, std::vector<int> > numIC;

	(sumIC[0]).assign(40, 0.);
	(sumIC[1]).assign(40, 0.);

	(numIC[0]).assign(40, 0);
	(numIC[1]).assign(40, 0);



	// mean over phi corrected skipping dead channel
	for(int k = 0; k < 2; ++k)
		for(int ix = 1; ix <= h_scale_EE[k] -> GetNbinsX(); ++ix)
			for(int iy = 1; iy <= h_scale_EE[k] -> GetNbinsY(); ++iy) {
				int ring = eRings->GetEndcapRing(ix, iy, k);
				if( ring == -1 ) continue;

				bool isGood = CheckxtalIC_EE(h_scale_EE[k], ix, iy, ring);
				bool isGoodTT;
				if( k == 0 ) isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEM);
				else         isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEP);

				if( isGoodTT && isGood ) {
					(sumIC[k]).at(ring) += h_scale_EE[k]->GetBinContent(ix, iy);
					(numIC[k]).at(ring) += 1;
				}
			}

	// normalize IC skipping bad channels and bad TTs
	for(int k = 0; k < 2; ++k)
		for(int ix = 1; ix <= h_scale_EE[k]->GetNbinsX(); ++ix)
			for(int iy = 1; iy <= h_scale_EE[k]->GetNbinsY(); ++iy) {
				int ring = eRings->GetEndcapRing(ix, iy, k);
				if( ring == -1 ) continue;

				if( !skip ) {
					if( ring > 33 ) {
						hcmap_EE[k] -> Fill(ix, iy, 0.);
						continue;
					} else {
						if( (numIC[k]).at(ring) != 0 && (sumIC[k]).at(ring) != 0 ) {
							int zside = 0;
							if (k == 0) zside = -1;
							if (k == 1) zside = 1;
							int iPhi = eRings->GetEndcapIphi(ix, iy, zside);
							float phi = ((float)iPhi * 2.*TMath::Pi() / 360. - TMath::Pi());
							//	      int iRing = 85 + eRings -> GetEndcapRing(ix,iy,zside);
							//float eta = eRings -> GetEtaFromIRing(iRing);

							//int etaBin = int((fabs(eta)-etaMin)/float((etaMax-etaMin)/nEtaBinsEE));
							//if (fabs(eta)<etaMin)  etaBin=0;
							//else if (fabs(eta)>etaMax)  etaBin=nEtaBinsEE-1;
							//      std::cout<<ix<<" "<<iy<<" "<<zside<<" "<<iRing<<" "<<fabs(eta)<<" "<<etaBin<<std::endl;

							hcmap_EE[k] -> Fill(ix, iy, correctionMomentum.at(0).at(k)->Eval(phi));
						}
					}
				}

				if( skip ) {
					bool isGood = CheckxtalIC_EE(h_scale_EE[k], ix, iy, ring);
					bool isGoodTT;

					if( k == 0 ) isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEM);
					else         isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEP);

					if( isGood && isGoodTT ) {
						if( ring > 33 ) {
							hcmap_EE[k] -> Fill(ix, iy, 0.);
							continue;
						} else {
							if( (numIC[k]).at(ring) != 0 && (sumIC[k]).at(ring) != 0 ) {
								int zside = 0;
								if (k == 0) zside = -1;
								if (k == 1) zside = 1;
								int iPhi = eRings->GetEndcapIphi(ix, iy, zside);
								float phi = ((float)iPhi * 2.*TMath::Pi() / 360. - TMath::Pi());
								/*
								int iRing = 85 + eRings -> GetEndcapRing(ix,iy,zside);
								float eta = eRings -> GetEtaFromIRing(iRing);

								int etaBin = int((fabs(eta)-etaMin)/float((etaMax-etaMin)/nEtaBinsEE));
								if (fabs(eta)<etaMin)  etaBin=0;
								else if (fabs(eta)>etaMax)  etaBin=nEtaBinsEE-1;
								//      std::cout<<ix<<" "<<iy<<" "<<zside<<" "<<iRing<<" "<<fabs(eta)<<" "<<etaBin<<std::endl;
								*/
								hcmap_EE[k] -> Fill(ix, iy, correctionMomentum.at(0).at(k)->Eval(phi));
								if (ix == 30 && iy == 32 && k == 1)
									std::cout << "corr: " << correctionMomentum.at(0).at(k)->Eval(phi) << std::endl;

							}
						}
					}
				}
			}

}





void DrawICCorr_EE(TH2F* h_scale_EEM, TH2F* h_scale_EEP,
                   TH2F* hcmap_EEM, TH2F* hcmap_EEP,
                   const std::vector< std::pair<int, int> >& TT_centre_EEM,
                   const std::vector< std::pair<int, int> >& TT_centre_EEP,
                   std::vector<std::vector<TGraphErrors*> > & correctionMomentum, TEndcapRings* eRings, bool skip , int nEtaBinsEE, float etaMin, float etaMax,
                   int shift)
{
	std::map<int, TH2F*> h_scale_EE;
	std::map<int, TH2F*> hcmap_EE;

	h_scale_EE[0] = h_scale_EEM;
	h_scale_EE[1] = h_scale_EEP;

	hcmap_EE[0] = hcmap_EEM;
	hcmap_EE[1] = hcmap_EEP;



	std::map<int, std::vector<float> > sumIC;
	std::map<int, std::vector<int> > numIC;

	(sumIC[0]).assign(40, 0.);
	(sumIC[1]).assign(40, 0.);

	(numIC[0]).assign(40, 0);
	(numIC[1]).assign(40, 0);

	// mean over phi corrected skipping dead channel
	for(int k = 0; k < 2; ++k)
		for(int ix = 1; ix <= h_scale_EE[k] -> GetNbinsX(); ++ix)
			for(int iy = 1; iy <= h_scale_EE[k] -> GetNbinsY(); ++iy) {
				int ring = eRings->GetEndcapRing(ix, iy, k);
				if( ring == -1 ) continue;

				bool isGood = CheckxtalIC_EE(h_scale_EE[k], ix, iy, ring);
				bool isGoodTT;
				if( k == 0 ) isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEM);
				else         isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEP);

				if( isGoodTT && isGood ) {
					int zside = 0;
					if (k == 0) zside = -1;
					if (k == 1) zside = 1;
					int iPhi = eRings->GetEndcapIphi(ix, iy, zside);
					if (iPhi + shift >= 360)  iPhi -= 360;
					/*
					int iRing = 85 + eRings -> GetEndcapRing(ix,iy,zside);
					float eta = eRings -> GetEtaFromIRing(iRing);

					int etaBin = int((fabs(eta)-etaMin)/float((etaMax-etaMin)/nEtaBinsEE));
					if (fabs(eta)<etaMin)  etaBin=0;
					else if (fabs(eta)>etaMax)  etaBin=nEtaBinsEE-1;
					//      std::cout<<ix<<" "<<iy<<" "<<zside<<" "<<iRing<<" "<<fabs(eta)<<" "<<etaBin<<std::endl;
					*/
					(sumIC[k]).at(ring) += h_scale_EE[k]->GetBinContent(ix, iy); ///correctionMomentum.at(etaBin).at(k)->Eval(iPhi+shift);
					(numIC[k]).at(ring) += 1;
				}
			}

	// normalize IC skipping bad channels and bad TTs
	for(int k = 0; k < 2; ++k)
		for(int ix = 1; ix <= h_scale_EE[k]->GetNbinsX(); ++ix)
			for(int iy = 1; iy <= h_scale_EE[k]->GetNbinsY(); ++iy) {
				int ring = eRings->GetEndcapRing(ix, iy, k);
				if( ring == -1 ) continue;

				if( !skip ) {
					if( ring > 33 ) {
						hcmap_EE[k] -> Fill(ix, iy, 0.);
						continue;
					} else {
						if( (numIC[k]).at(ring) != 0 && (sumIC[k]).at(ring) != 0 ) {
							int zside = 0;
							if (k == 0) zside = -1;
							if (k == 1) zside = 1;
							int iPhi = eRings->GetEndcapIphi(ix, iy, zside);
							float phi = ((float)iPhi * 2.*TMath::Pi() / 360. - TMath::Pi());
							if (iPhi + shift >= 360)  iPhi -= 360;
							/*
							int iRing = 85 + eRings -> GetEndcapRing(ix,iy,zside);
							float eta = eRings -> GetEtaFromIRing(iRing);

							int etaBin = int((fabs(eta)-etaMin)/float((etaMax-etaMin)/nEtaBinsEE));
							if (fabs(eta)<etaMin)  etaBin=0;
							else if (fabs(eta)>etaMax)  etaBin=nEtaBinsEE-1;
							//      std::cout<<ix<<" "<<iy<<" "<<zside<<" "<<iRing<<" "<<fabs(eta)<<" "<<etaBin<<std::endl;
							*/
							hcmap_EE[k] -> Fill(ix, iy, (h_scale_EE[k]->GetBinContent(ix, iy) / correctionMomentum.at(0).at(k)->Eval(phi)) / ((sumIC[k]).at(ring) / (numIC[k]).at(ring)));
						}
					}
				}

				if( skip ) {
					bool isGood = CheckxtalIC_EE(h_scale_EE[k], ix, iy, ring);
					bool isGoodTT;

					if( k == 0 ) isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEM);
					else         isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEP);

					if( isGood && isGoodTT ) {
						if( ring > 33 ) {
							hcmap_EE[k] -> Fill(ix, iy, 0.);
							continue;
						} else {
							if( (numIC[k]).at(ring) != 0 && (sumIC[k]).at(ring) != 0 ) {
								int zside = 0;
								if (k == 0) zside = -1;
								if (k == 1) zside = 1;
								int iPhi = eRings->GetEndcapIphi(ix, iy, zside);
								float phi = ((float)iPhi * 2.*TMath::Pi() / 360. - TMath::Pi());
								if (iPhi + shift >= 360)  iPhi -= 360;
								/*
								int iRing = 85 + eRings -> GetEndcapRing(ix,iy,zside);
								float eta = eRings -> GetEtaFromIRing(iRing);

								int etaBin = int((fabs(eta)-etaMin)/float((etaMax-etaMin)/nEtaBinsEE));
								if (fabs(eta)<etaMin)  etaBin=0;
								else if (fabs(eta)>etaMax)  etaBin=nEtaBinsEE-1;
								//      std::cout<<ix<<" "<<iy<<" "<<zside<<" "<<iRing<<" "<<fabs(eta)<<" "<<etaBin<<std::endl;
								*/
								hcmap_EE[k] -> Fill(ix, iy, (h_scale_EE[k]->GetBinContent(ix, iy) / correctionMomentum.at(0).at(k)->Eval(phi)) / ((sumIC[k]).at(ring) / (numIC[k]).at(ring)));
								if (ix == 30 && iy == 32 && k == 1)
									std::cout << "IC_corr: " << (h_scale_EE[k]->GetBinContent(ix, iy) / correctionMomentum.at(0).at(k)->Eval(phi)) / ((sumIC[k]).at(ring) / (numIC[k]).at(ring)) << std::endl;

								//	      std::cout<<ix<<" "<<iy<<" "<<h_scale_EE[k]->GetBinContent(ix,iy)<<" "<<correctionMomentum.at(etaBin).at(k)->Eval(iPhi+shift)<<std::endl;
							}
						}
					}
				}
			}

}



///////////////////////////////////////////////////////////////
// check if a xtal confines with a bad xtal or lies in a bad TT
///////////////////////////////////////////////////////////////

bool CheckxtalIC_EB(TH2F* h_scale_EB, int iPhi, int iEta)
{
	if( h_scale_EB->GetBinContent(iPhi, iEta) == 0 ) return false;
	if( h_scale_EB->GetBinContent(iPhi, iEta) > 5 ) return false;

	int bx = h_scale_EB->GetNbinsX();
	int by = h_scale_EB->GetNbinsY();

	if( (iPhi < bx && h_scale_EB->GetBinContent(iPhi + 1, iEta) == 0) ||
	        (iPhi > 1  && h_scale_EB->GetBinContent(iPhi - 1, iEta) == 0) ) return false;

	if( (iEta != 85 && iEta < by && h_scale_EB->GetBinContent(iPhi, iEta + 1) == 0) ||
	        (iEta != 87 && iEta > 1  && h_scale_EB->GetBinContent(iPhi, iEta - 1) == 0) ) return false;

	if( (iEta != 85 && iEta < by && iPhi < bx && h_scale_EB->GetBinContent(iPhi + 1, iEta + 1) == 0) ||
	        (iEta != 87 && iEta > 1  && iPhi > 1  && h_scale_EB->GetBinContent(iPhi - 1, iEta - 1) == 0) ) return false;

	if( (iEta != 87 && iEta > 1  && iPhi < bx && h_scale_EB->GetBinContent(iPhi + 1, iEta - 1) == 0) ||
	        (iEta != 85 && iEta < by && iPhi > 1  && h_scale_EB->GetBinContent(iPhi - 1, iEta + 1) == 0) ) return false;

	return true;
}

bool CheckxtalTT_EB(int iPhi, int iEta, const std::vector<std::pair<int, int> >& TT_centre)
{
	for(unsigned int k = 0; k < TT_centre.size(); ++k)
		if( (fabs(iPhi - TT_centre.at(k).second) < 5) && (fabs(iEta - 86 - TT_centre.at(k).first) < 5) ) return false;

	return true;
}



bool CheckxtalIC_EE(TH2F* h_scale_EE, int ix, int iy, int ir)
{
	if( h_scale_EE->GetBinContent(ix, iy) == 0 ) return false;
	if( h_scale_EE->GetBinContent(ix, iy) > 5 ) return false;

	int bx = h_scale_EE->GetNbinsX();
	int by = h_scale_EE->GetNbinsY();

	if( ( h_scale_EE->GetBinContent(ix + 1, iy) == 0 && (ir != 0 || ir < 33) && ix < bx) ||
	        ( h_scale_EE->GetBinContent(ix - 1, iy) == 0 && (ir != 0 || ir < 33) && ix > 1 ) ) return false;

	if( ( h_scale_EE->GetBinContent(ix, iy + 1) == 0 && (ir != 0 || ir < 33) && iy < by) ||
	        ( h_scale_EE->GetBinContent(ix, iy - 1) == 0 && (ir != 0 || ir < 33) && iy > 1) ) return false;

	if( ( h_scale_EE->GetBinContent(ix + 1, iy + 1) == 0 && (ir != 0 || ir < 33) && ix < bx && iy < by ) ||
	        ( h_scale_EE->GetBinContent(ix - 1, iy - 1) == 0 && (ir != 0 || ir < 33) && iy > 1  && ix > 1  ) ) return false;

	if( ( h_scale_EE->GetBinContent(ix + 1, iy - 1) == 0 && (ir != 0 || ir < 33) && iy > 1 && ix < bx) ||
	        ( h_scale_EE->GetBinContent(ix - 1, iy + 1) == 0 && (ir != 0 || ir < 33) && ix > 1 && iy < by) ) return false;

	return true;
}

bool CheckxtalTT_EE(int ix, int iy, int ir, const std::vector<std::pair<int, int> >& TT_centre)
{
	for( unsigned int k = 0; k < TT_centre.size(); k++)
		if( (fabs(ix - TT_centre.at(k).first) < 4) && (fabs(iy - TT_centre.at(k).second) < 4) ) return false;

	return true;
}






//-------------------------------------------------------------------------------------------------------------






///////////////////////////////////////////////////////////////////
// normalize the IC of each eta ring to the average IC of that ring
///////////////////////////////////////////////////////////////////

void NormalizeIC_EB(TH2F* h_scale_EB, TH2F* hcmap_EB, const std::vector< std::pair<int, int> > & TT_centre, bool skip)
{
	// mean over phi corrected skipping dead channel
	for(int iEta = 1; iEta <= h_scale_EB->GetNbinsY(); ++iEta) {
		float sumIC = 0.;
		int numIC = 0;

		for(int iPhi = 1; iPhi <= h_scale_EB->GetNbinsX() ; ++iPhi) {
			bool isGood = CheckxtalIC_EB(h_scale_EB, iPhi, iEta);
			bool isGoodTT = CheckxtalTT_EB(iPhi, iEta, TT_centre);

			if( isGood && isGoodTT ) {
				sumIC += h_scale_EB -> GetBinContent(iPhi, iEta);
				++numIC;
			}
		}

		// normalize IC skipping bad channels and bad TTs
		for(int iPhi = 1; iPhi <= h_scale_EB->GetNbinsX(); ++iPhi) {
			if( numIC == 0 || sumIC == 0 ) continue;

			if( !skip ) {
				hcmap_EB -> SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (sumIC / numIC));
				continue;
			} else {
				bool isGood = CheckxtalIC_EB(h_scale_EB, iPhi, iEta);
				bool isGoodTT = CheckxtalTT_EB(iPhi, iEta, TT_centre);
				if( !isGood || !isGoodTT ) continue;
				hcmap_EB -> SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (sumIC / numIC));
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void NormalizeIC_LMR_EB(TH2F* h_scale_EB, TH2F* hcmap_EB, const std::vector< std::pair<int, int> > & TT_centre, bool skip)
{
	TFile* inFile = TFile::Open("./src/drawRegions.root", "READ");
	TH2F* h2_EB_LMR = (TH2F*)(inFile -> Get("h2_EB_LMR"));

	float sumIC[325];
	float numIC[325];
	for(int ii = 0; ii < 325; ii++) {
		sumIC[ii] = 0;
		numIC[ii] = 0;
	}

	// mean over LMR corrected skipping dead channel
	for(int iEta = 1; iEta <= h_scale_EB->GetNbinsY(); ++iEta) {
		for(int iPhi = 1; iPhi <= h_scale_EB->GetNbinsX() ; ++iPhi) {

			bool isGood = CheckxtalIC_EB(h_scale_EB, iPhi, iEta);
			bool isGoodTT = CheckxtalTT_EB(iPhi, iEta, TT_centre);
			int regionId = h2_EB_LMR -> GetBinContent(iEta, iPhi);
			if( isGood && isGoodTT ) {
				sumIC[regionId] = sumIC[regionId] + h_scale_EB -> GetBinContent(iPhi, iEta);
				numIC[regionId] = numIC[regionId] + 1;
			}
		}
	}

	// normalize IC skipping bad channels and bad TTs
	for(int iEta = 1; iEta <= h_scale_EB->GetNbinsY(); ++iEta) {
		for(int iPhi = 1; iPhi <= h_scale_EB->GetNbinsX() ; ++iPhi) {
			int regionId = h2_EB_LMR -> GetBinContent(iPhi, iEta);
			if( numIC[regionId] == 0 || sumIC[regionId] == 0 ) continue;

			if( !skip ) {
				hcmap_EB -> SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (sumIC[regionId] / numIC[regionId]));
				continue;
			} else {
				bool isGood = CheckxtalIC_EB(h_scale_EB, iPhi, iEta);
				bool isGoodTT = CheckxtalTT_EB(iPhi, iEta, TT_centre);
				if( !isGood || !isGoodTT ) continue;
				hcmap_EB -> SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (sumIC[regionId] / numIC[regionId]));
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void NormalizeIC_SM_EB(TH2F* h_scale_EB, TH2F* hcmap_EB, const std::vector< std::pair<int, int> > & TT_centre, bool skip)
{
	TFile* inFile = TFile::Open("./src/drawRegions.root", "READ");
	TH2F* h2_EB_LMR = (TH2F*)(inFile -> Get("h2_EB_SM"));

	float sumIC[36];
	float numIC[36];
	for(int ii = 0; ii < 36; ii++) {
		sumIC[ii] = 0;
		numIC[ii] = 0;
	}

	// mean over LMR corrected skipping dead channel
	for(int iEta = 1; iEta <= h_scale_EB->GetNbinsY(); ++iEta) {
		for(int iPhi = 1; iPhi <= h_scale_EB->GetNbinsX() ; ++iPhi) {

			bool isGood = CheckxtalIC_EB(h_scale_EB, iPhi, iEta);
			bool isGoodTT = CheckxtalTT_EB(iPhi, iEta, TT_centre);
			int regionId = h2_EB_LMR -> GetBinContent(iEta, iPhi);
			if( isGood && isGoodTT ) {
				sumIC[regionId] = sumIC[regionId] + h_scale_EB -> GetBinContent(iPhi, iEta);
				numIC[regionId] = numIC[regionId] + 1;
			}
		}
	}

	// normalize IC skipping bad channels and bad TTs
	for(int iEta = 1; iEta <= h_scale_EB->GetNbinsY(); ++iEta) {
		for(int iPhi = 1; iPhi <= h_scale_EB->GetNbinsX() ; ++iPhi) {
			int regionId = h2_EB_LMR -> GetBinContent(iPhi, iEta);
			if( numIC[regionId] == 0 || sumIC[regionId] == 0 ) continue;

			if( !skip ) {
				hcmap_EB -> SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (sumIC[regionId] / numIC[regionId]));
				continue;
			} else {
				bool isGood = CheckxtalIC_EB(h_scale_EB, iPhi, iEta);
				bool isGoodTT = CheckxtalTT_EB(iPhi, iEta, TT_centre);
				if( !isGood || !isGoodTT ) continue;
				hcmap_EB -> SetBinContent(iPhi, iEta, h_scale_EB->GetBinContent(iPhi, iEta) / (sumIC[regionId] / numIC[regionId]));
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////
void NormalizeIC_EE(TH2F* h_scale_EEM, TH2F* h_scale_EEP,
                    TH2F* hcmap_EEM, TH2F* hcmap_EEP,
                    const std::vector< std::pair<int, int> >& TT_centre_EEM,
                    const std::vector< std::pair<int, int> >& TT_centre_EEP,
                    TEndcapRings* eRings, bool skip)
{
	std::map<int, TH2F*> h_scale_EE;
	std::map<int, TH2F*> hcmap_EE;

	h_scale_EE[0] = h_scale_EEM;
	h_scale_EE[1] = h_scale_EEP;

	hcmap_EE[0] = hcmap_EEM;
	hcmap_EE[1] = hcmap_EEP;



	std::map<int, std::vector<float> > sumIC;
	std::map<int, std::vector<int> > numIC;

	(sumIC[0]).assign(40, 0.);
	(sumIC[1]).assign(40, 0.);

	(numIC[0]).assign(40, 0);
	(numIC[1]).assign(40, 0);



	// mean over phi corrected skipping dead channel
	for(int k = 0; k < 2; ++k)
		for(int ix = 1; ix <= h_scale_EE[k] -> GetNbinsX(); ++ix)
			for(int iy = 1; iy <= h_scale_EE[k] -> GetNbinsY(); ++iy) {
				int ring = eRings->GetEndcapRing(ix, iy, k);
				if( ring == -1 ) continue;

				bool isGood = CheckxtalIC_EE(h_scale_EE[k], ix, iy, ring);
				bool isGoodTT;
				if( k == 0 ) isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEM);
				else         isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEP);

				if( isGoodTT && isGood ) {
					(sumIC[k]).at(ring) += h_scale_EE[k]->GetBinContent(ix, iy);
					(numIC[k]).at(ring) += 1;
				}
			}

	// normalize IC skipping bad channels and bad TTs
	for(int k = 0; k < 2; ++k)
		for(int ix = 1; ix <= h_scale_EE[k]->GetNbinsX(); ++ix)
			for(int iy = 1; iy <= h_scale_EE[k]->GetNbinsY(); ++iy) {
				int ring = eRings->GetEndcapRing(ix, iy, k);
				if( ring == -1 ) continue;

				if( !skip ) {
					if( ring > 33 ) {
						hcmap_EE[k] -> Fill(ix, iy, 0.);
						continue;
					} else {
						if( (numIC[k]).at(ring) != 0 && (sumIC[k]).at(ring) != 0 )
							hcmap_EE[k] -> Fill(ix, iy, h_scale_EE[k]->GetBinContent(ix, iy) / ((sumIC[k]).at(ring) / (numIC[k]).at(ring)));
					}
				}

				if( skip ) {
					bool isGood = CheckxtalIC_EE(h_scale_EE[k], ix, iy, ring);
					bool isGoodTT;

					if( k == 0 ) isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEM);
					else         isGoodTT = CheckxtalTT_EE(ix, iy, ring, TT_centre_EEP);

					if( isGood && isGoodTT ) {
						if( ring > 33 ) {
							hcmap_EE[k] -> Fill(ix, iy, 0.);
							continue;
						} else {
							if( (numIC[k]).at(ring) != 0 && (sumIC[k]).at(ring) != 0 )
								hcmap_EE[k] -> Fill(ix, iy, h_scale_EE[k]->GetBinContent(ix, iy) / ((sumIC[k]).at(ring) / (numIC[k]).at(ring)));
						}
					}
				}
			}

}







//-------------------------------------------------------------------------------------------------------------






///////////////////////////////////////////////////////////////////////
void BookSpreadHistos_EB(TH1F* h_spread, std::vector<TH1F*>& h_spread_vsEta, TGraphErrors* g_spread_vsEta, const int& etaRingWidth,
                         const std::string& name, const int& nBins_spread, const float& spreadMin, const float& spreadMax,
                         TH2F* hcmap, TH2F* hcmap2)
{
	char histoName[100];
	char funcName[100];

	std::ofstream RMSFile;
	if (hcmap2 == NULL) {
		RMSFile.open ("RMSFile.txt");
	}

	// define the number of eta rings
	int nEtaRings = 85 / etaRingWidth;
	if( 85 % etaRingWidth > 0 ) nEtaRings += 1;

	// initialize the histograms
	for(int etaRing = 0; etaRing < nEtaRings; ++etaRing) {
		int etaMin = 1 + etaRing * etaRingWidth;

		sprintf(histoName, "h_%s%02d", name.c_str(), etaMin);
		h_spread_vsEta.push_back( new TH1F(histoName, "", nBins_spread, spreadMin, spreadMax) );
	}


	// spread histos folding EB+ and EB-
	for(int jbin = 1; jbin <= hcmap->GetNbinsY(); ++jbin) {
		float etaRingMin = hcmap->GetYaxis()->GetBinLowEdge(jbin);
		int etaRing = int( (fabs(etaRingMin) - 1.) / etaRingWidth );
		if( etaRing == -1 ) continue;


		for(int ibin = 1; ibin <= hcmap->GetNbinsX(); ++ibin) {
			if( hcmap2 == NULL ) {
				float IC = hcmap->GetBinContent(ibin, jbin);
				if( IC > 0. && IC < 2. ) {
					h_spread -> Fill(IC);
					h_spread_vsEta.at(etaRing) -> Fill(IC);
				}
			} else {
				float IC1 = hcmap  -> GetBinContent(ibin, jbin);
				float IC2 = hcmap2 -> GetBinContent(ibin, jbin);
				if( IC1 > 0. && IC1 < 2. && IC2 > 0. && IC2 < 2. ) {
					h_spread -> Fill((IC1 - IC2) / (IC1 + IC2));
					h_spread_vsEta.at(etaRing) -> Fill((IC1 - IC2) / (IC1 + IC2));
					//  std::cout<<"eta: "<<etaRing<<" values: "<<IC1<<" "<<IC2<<" "<<(IC1-IC2)/(IC1+IC2)<<std::endl;
				}
			}
		}

	}


	// fit the global spread
	sprintf(funcName, "f_%s", name.c_str());
	TF1* fgaus = new TF1(funcName, "gaus", spreadMin, spreadMax);

	fgaus -> SetNpx(10000);
	if( hcmap2 == NULL ) fgaus -> SetLineColor(kBlue + 2);
	else                 fgaus -> SetLineColor(kRed + 2);

	float center = 0.5 * (spreadMin + spreadMax);
	fgaus -> SetParameter(1, h_spread->GetMean());
	fgaus -> SetParameter(2, h_spread->GetRMS());
	h_spread -> Fit(funcName, "QLS+", "", center - h_spread->GetRMS(), center + h_spread->GetRMS());


	// fill the TGraph
	g_spread_vsEta -> SetMarkerStyle(20);
	g_spread_vsEta -> SetMarkerSize(1);
	g_spread_vsEta -> GetYaxis() -> SetRangeUser(0., 0.05);
	if( hcmap2 == NULL ) g_spread_vsEta -> SetMarkerColor(kBlue + 2);
	else                 g_spread_vsEta -> SetMarkerColor(kRed + 2);

	for(int etaRing = 0; etaRing < nEtaRings; ++etaRing) {
		int etaMin = 1 + etaRing * etaRingWidth;

		sprintf(funcName, "f_%s%02d", name.c_str(), etaMin);
		fgaus = new TF1(funcName, "gaus", spreadMin, spreadMax);

		fgaus -> SetNpx(10000);
		if( hcmap2 == NULL ) fgaus -> SetLineColor(kBlue + 2);
		else                 fgaus -> SetLineColor(kRed + 2);

		fgaus -> SetParameter(1, h_spread_vsEta[etaRing]->GetMean());
		fgaus -> SetParameter(2, h_spread_vsEta[etaRing]->GetRMS());
		h_spread_vsEta[etaRing] -> Fit(funcName, "QLS+", "", center - 3.*h_spread_vsEta[etaRing]->GetRMS(), center + 3.*h_spread_vsEta[etaRing]->GetRMS());

		g_spread_vsEta -> SetPoint(etaRing, etaMin, fgaus->GetParameter(2));
		g_spread_vsEta -> SetPointError(etaRing, 0.5 * etaRingWidth, fgaus->GetParError(2));

		//   std::cout<<etaMin <<" "<<fgaus->GetParameter(2)<<std::endl;
		if (hcmap2 == NULL)    RMSFile << etaMin << " " << fgaus->GetParameter(2) << "\n";
	}
}






///////////////////////////////////////////////////////////////////////////////
void PhiProfile(TH1F* h_phiAvgICSpread, TGraphErrors* g_avgIC_vsPhi, const int& phiRegionWidth,
                TH2F* hcmap, TEndcapRings* eRings)
{
	// define the number of phi regions
	int nPhiRegions = 360 / phiRegionWidth;
	if( 360 % phiRegionWidth > 0 ) nPhiRegions += 1;

	std::vector<TH1F*> h_IC_vsPhi(nPhiRegions);
	for(int i = 0; i < nPhiRegions; ++i) {
		char histoName[50];
		sprintf(histoName, "h_IC_vsPhi%03d", i);
		h_IC_vsPhi.at(i) = new TH1F(histoName, "", 1000, 0., 2.);
	}


	for(int ibin = 1; ibin <= hcmap->GetNbinsX(); ++ibin)
		for(int jbin = 1; jbin <= hcmap->GetNbinsY(); ++jbin) {
			float IC = hcmap->GetBinContent(ibin, jbin);
			if( IC <= 0. || IC >= 2. ) continue;

			float phiRegionMin = hcmap->GetXaxis()->GetBinLowEdge(ibin);
			if( eRings != NULL ) phiRegionMin = eRings -> GetEndcapIphi(ibin, jbin, 1);

			int phiRegion = int( (fabs(phiRegionMin) - 1.) / phiRegionWidth );

			h_IC_vsPhi.at(phiRegion) -> Fill(IC);
		}


	for(int i = 0; i < nPhiRegions; ++i) {
		int phiMin = 1 + i * phiRegionWidth;

		h_phiAvgICSpread -> Fill(h_IC_vsPhi.at(i)->GetMean());

		g_avgIC_vsPhi -> SetPoint(i, phiMin, h_IC_vsPhi.at(i)->GetMean());
		g_avgIC_vsPhi -> SetPointError(i, 0.5 * phiRegionWidth, h_IC_vsPhi.at(i)->GetMeanError());
	}


	TF1* fgaus = new TF1("f_phiAvgICSpread", "gaus", 0., 2.);
	fgaus -> SetNpx(10000);
	fgaus -> SetLineColor(kBlack);

	fgaus -> SetParameter(1, h_phiAvgICSpread->GetMean());
	fgaus -> SetParameter(2, h_phiAvgICSpread->GetRMS());
	h_phiAvgICSpread -> Fit("f_phiAvgICSpread", "QLS+", "", 1. - 3.*h_phiAvgICSpread->GetRMS(), 1. + 3.*h_phiAvgICSpread->GetRMS());



	for(int i = 0; i < nPhiRegions; ++i) {
		delete h_IC_vsPhi.at(i);
	}
}



void PhiFoldProfile_EB(TGraphErrors* g_avgIC_vsPhiFold_EBM, TGraphErrors* g_avgIC_vsPhiFold_EBP, const int& phiRegionWidth,
                       TH2F* hcmap)
{
	// define the number of phi regions
	int nPhiRegions = 20 / phiRegionWidth;
	if( 20 % phiRegionWidth > 0 ) nPhiRegions += 1;

	std::vector<TH1F*> h_IC_vsPhiFold_EBM(nPhiRegions);
	std::vector<TH1F*> h_IC_vsPhiFold_EBP(nPhiRegions);
	for(int i = 0; i < nPhiRegions; ++i) {
		char histoName[50];
		sprintf(histoName, "h_IC_vsPhiFold_EBM_%03d", i);
		h_IC_vsPhiFold_EBM.at(i) = new TH1F(histoName, "", 1000, 0., 2.);
		sprintf(histoName, "h_IC_vsPhiFold_EBP_%03d", i);
		h_IC_vsPhiFold_EBP.at(i) = new TH1F(histoName, "", 1000, 0., 2.);
	}


	for(int ibin = 1; ibin <= hcmap->GetNbinsX(); ++ibin)
		for(int jbin = 1; jbin <= hcmap->GetNbinsY(); ++jbin) {
			float IC = hcmap->GetBinContent(ibin, jbin);
			if( IC <= 0. || IC >= 2. ) continue;

			float phiRegionMin = hcmap->GetXaxis()->GetBinLowEdge(ibin);
			int phiRegion = int( (fabs(phiRegionMin) - 1.) / phiRegionWidth ) % 20;

			float etaBinCenter = hcmap->GetYaxis()->GetBinCenter(jbin);

			if( etaBinCenter < 0. ) h_IC_vsPhiFold_EBM.at(phiRegion) -> Fill(IC);
			if( etaBinCenter > 0. ) h_IC_vsPhiFold_EBP.at(phiRegion) -> Fill(IC);
		}


	for(int i = 0; i < nPhiRegions; ++i) {
		int phiMin = 1 + i * phiRegionWidth;

		g_avgIC_vsPhiFold_EBM -> SetPoint(i, phiMin, h_IC_vsPhiFold_EBM.at(i)->GetMean());
		g_avgIC_vsPhiFold_EBM -> SetPointError(i, 0.5 * phiRegionWidth, h_IC_vsPhiFold_EBM.at(i)->GetMeanError());

		g_avgIC_vsPhiFold_EBP -> SetPoint(i, phiMin, h_IC_vsPhiFold_EBP.at(i)->GetMean());
		g_avgIC_vsPhiFold_EBP -> SetPointError(i, 0.5 * phiRegionWidth, h_IC_vsPhiFold_EBP.at(i)->GetMeanError());
	}
}

////////////////////////////////////////////////////////////////////////////
void ResidualSpread(TGraphErrors* g_stat, TGraphErrors* g_spread, TGraphErrors* g_residual)
{
	g_residual -> SetMarkerStyle(20);
	g_residual -> SetMarkerSize(1);
	g_residual -> SetMarkerColor(kGreen + 2);


	for(int i = 0; i < g_stat->GetN(); ++i) {
		double spread, espread;
		double stat, estat;
		double residual, eresidual;
		double x, ex;

		ex = g_stat-> GetErrorX(i);

		g_stat -> GetPoint(i, x, stat);
		estat = g_stat-> GetErrorY(i);

		g_spread -> GetPoint(i, x, spread);
		espread = g_spread -> GetErrorY(i);

		if( spread > stat ) {
			residual  = sqrt( pow(spread, 2) - pow(stat, 2) );
			eresidual = sqrt( pow(spread * espread, 2) + pow(stat * estat, 2) ) / residual;
		} else {
			residual = 0;
			eresidual = 0;
		}

		g_residual -> SetPoint(i, x, residual);
		g_residual -> SetPointError(i, ex, eresidual);
	}
}

//////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
void BookSpreadHistos_EE(std::map<int, TH1F*>& h_spread, std::map<int, std::vector<TH1F*> >& h_spread_vsEta, std::map<int, TGraphErrors*>& g_spread_vsEta,
                         TEndcapRings* eRings, const int& etaRingWidth,
                         const std::string& name, const int& nBins_spread, const float& spreadMin, const float& spreadMax,
                         std::map<int, TH2F*>& hcmap, std::map<int, TH2F*>& hcmap2)
{
	char histoName[100];
	char funcName[100];


	// define the number of eta rings
	int nEtaRings = 39 / etaRingWidth;
	if( 39 % etaRingWidth > 0 ) nEtaRings += 1;


	// initialize the histograms (EE-, all EE, EE+)
	for(int k = -1; k <= 1; ++k)
		for(int etaRing = 0; etaRing < nEtaRings; ++etaRing) {
			int etaMin = 0 + etaRing * etaRingWidth;

			if( k == -1 ) {
				sprintf(histoName, "h_%s%02d_EEM", name.c_str(), etaMin);
				h_spread_vsEta[k].push_back( new TH1F(histoName, "", nBins_spread, spreadMin, spreadMax) );
			}
			if( k == 0 ) {
				sprintf(histoName, "h_%s%02d_EE", name.c_str(), etaMin);
				h_spread_vsEta[k].push_back( new TH1F(histoName, "", nBins_spread, spreadMin, spreadMax) );
			}
			if( k == 1 ) {
				sprintf(histoName, "h_%s%02d_EEP", name.c_str(), etaMin);
				h_spread_vsEta[k].push_back( new TH1F(histoName, "", nBins_spread, spreadMin, spreadMax) );
			}
		}


	// spread histos
	for(int k = -1; k <= 1; ++k) {
		if( k == 0 ) continue;

		for(int ibin = 1; ibin <= hcmap[k]->GetNbinsX(); ++ibin)
			for(int jbin = 1; jbin <= hcmap[k]->GetNbinsY(); ++jbin) {
				int etaRing = eRings -> GetEndcapRing(ibin, jbin, k);
				if( etaRing == -1 ) continue;

				if( (hcmap2[-1] == NULL) && (hcmap2[1] == NULL) ) {
					float IC = hcmap[k]->GetBinContent(ibin, jbin);
					if( IC > 0. && IC < 2. ) {
						h_spread[k] -> Fill(IC);
						h_spread_vsEta[k].at(etaRing) -> Fill(IC);

						h_spread[0] -> Fill(IC);
						h_spread_vsEta[0].at(etaRing) -> Fill(IC);
					}
				}

				else {
					float IC1 = hcmap[k]  -> GetBinContent(ibin, jbin);
					float IC2 = hcmap2[k] -> GetBinContent(ibin, jbin);
					if( IC1 > 0. && IC1 < 2. && IC2 > 0. && IC2 < 2. ) {
						h_spread[k] -> Fill((IC1 - IC2) / (IC1 + IC2));
						h_spread_vsEta[k].at(etaRing) -> Fill((IC1 - IC2) / (IC1 + IC2));

						h_spread[0] -> Fill((IC1 - IC2) / (IC1 + IC2));
						h_spread_vsEta[0].at(etaRing) -> Fill((IC1 - IC2) / (IC1 + IC2));
					}
				}
			}
	}



	// fit the global spread
	for(int k = -1; k <= 1; ++k) {
		if( k == -1 ) sprintf(funcName, "f_%s_EEM", name.c_str());
		if( k == 0 )  sprintf(funcName, "f_%s_EE", name.c_str());
		if( k == +1 ) sprintf(funcName, "f_%s_EEP", name.c_str());

		TF1* fgaus = new TF1(funcName, "gaus", spreadMin, spreadMax);

		fgaus -> SetNpx(10000);
		if( (hcmap2[-1] == NULL) && (hcmap2[1] == NULL) ) fgaus -> SetLineColor(kBlue + 2);
		else                                              fgaus -> SetLineColor(kBlue + 2);

		float center = 0.5 * (spreadMin + spreadMax);
		fgaus -> SetParameter(1, h_spread[k]->GetMean());
		fgaus -> SetParameter(2, h_spread[k]->GetRMS());
		h_spread[k] -> Fit(funcName, "QLS+", "", center - h_spread[k]->GetRMS(), center + h_spread[k]->GetRMS());


		// fill the TGraph
		g_spread_vsEta[k] -> SetMarkerStyle(20);
		g_spread_vsEta[k] -> SetMarkerSize(1);
		g_spread_vsEta[k] -> GetYaxis() -> SetRangeUser(0., 0.05);
		if( (hcmap2[-1] == NULL) && (hcmap2[1] == NULL) )g_spread_vsEta[k] -> SetMarkerColor(kBlue + 2);
		else                                             g_spread_vsEta[k] -> SetMarkerColor(kRed + 2);

		for(int etaRing = 0; etaRing < nEtaRings; ++etaRing) {
			if( (h_spread_vsEta[k])[etaRing]->Integral() == 0 ) continue;

			int etaMin = etaRing * etaRingWidth;

			sprintf(funcName, "f_%s%02d", name.c_str(), etaMin);
			fgaus = new TF1(funcName, "gaus", spreadMin, spreadMax);

			fgaus -> SetNpx(10000);
			if( (hcmap2[-1] == NULL) && (hcmap2[1] == NULL) ) fgaus -> SetLineColor(kBlue + 2);
			else                                              fgaus -> SetLineColor(kRed + 2);

			fgaus -> SetParameter(1, (h_spread_vsEta[k])[etaRing]->GetMean());
			fgaus -> SetParameter(2, (h_spread_vsEta[k])[etaRing]->GetRMS());
			(h_spread_vsEta[k])[etaRing] -> Fit(funcName, "QLS+", "", center - 3.*(h_spread_vsEta[k])[etaRing]->GetRMS(), center + 3.*(h_spread_vsEta[k])[etaRing]->GetRMS());

			g_spread_vsEta[k] -> SetPoint(etaRing, etaMin, fgaus->GetParameter(2));
			g_spread_vsEta[k] -> SetPointError(etaRing, 0.5 * etaRingWidth, fgaus->GetParError(2));
		}
	}
}


///////////////////////////////////////////////////////////////////////
void PhiProfileEE(TGraphErrors *phiProjection, TGraphErrors **MomentumScale, TH2F* hcmap, TEndcapRings *eRings, const int & iz)
{
	std::vector<double> vectSum;
	std::vector<double> vectCounter;

	vectCounter.assign(MomentumScale[0]->GetN(), 0.);
	vectSum.assign(MomentumScale[0]->GetN(), 0.);

	for(int ix = 1; ix < hcmap->GetNbinsX() + 1; ix++)
		for(int iy = 1; iy < hcmap->GetNbinsY() + 1; iy++) {
			if(hcmap->GetBinContent(ix, iy) == 0) continue;
			int iPhi = int(eRings->GetEndcapIphi(ix, iy, iz) / (360. / MomentumScale[0]->GetN()));
			vectSum.at(iPhi) = vectSum.at(iPhi) + hcmap->GetBinContent(ix, iy);
			vectCounter.at(iPhi) = vectCounter.at(iPhi) + 1;
		}

	for(unsigned int i = 0; i < vectCounter.size(); i++)
		phiProjection -> SetPoint(i, int(i * (360. / MomentumScale[0]->GetN())), vectSum.at(i) / vectCounter.at(i));
}






//-------------------------------------------------------------------------------------------------------------






////////////////////////////
// define the list of bad TT
////////////////////////////

void InitializeDeadTT_EB(std::vector<std::pair<int, int> >& TT_centre)
{
	TT_centre.push_back(std::pair<int, int> (58, 49));
	TT_centre.push_back(std::pair<int, int> (53, 109));
	TT_centre.push_back(std::pair<int, int> (8, 114));
	TT_centre.push_back(std::pair<int, int> (83, 169));
	TT_centre.push_back(std::pair<int, int> (53, 174));
	TT_centre.push_back(std::pair<int, int> (63, 194));
	TT_centre.push_back(std::pair<int, int> (83, 224));
	TT_centre.push_back(std::pair<int, int> (73, 344));
	TT_centre.push_back(std::pair<int, int> (83, 358));
	TT_centre.push_back(std::pair<int, int> (-13, 18));
	TT_centre.push_back(std::pair<int, int> (-18, 23));
	TT_centre.push_back(std::pair<int, int> (-8, 53));
	TT_centre.push_back(std::pair<int, int> (-3, 63));
	TT_centre.push_back(std::pair<int, int> (-53, 128));
	TT_centre.push_back(std::pair<int, int> (-28, 168));
	TT_centre.push_back(std::pair<int, int> (-53, 183));
	TT_centre.push_back(std::pair<int, int> (-83, 193));
	TT_centre.push_back(std::pair<int, int> (-74, 218));
	TT_centre.push_back(std::pair<int, int> (-8, 223));
	TT_centre.push_back(std::pair<int, int> (-48, 273));
	TT_centre.push_back(std::pair<int, int> (-68, 303));
	TT_centre.push_back(std::pair<int, int> (-43, 328));
	TT_centre.push_back(std::pair<int, int> (-43, 243));
}

void InitializeDeadTT_EB2012(std::vector<std::pair<int, int> >& TT_centre)
{
	TT_centre.push_back(std::pair<int, int> (58, 49));
	TT_centre.push_back(std::pair<int, int> (53, 109));
	TT_centre.push_back(std::pair<int, int> (8, 114));
	TT_centre.push_back(std::pair<int, int> (83, 169));
	TT_centre.push_back(std::pair<int, int> (53, 174));
	TT_centre.push_back(std::pair<int, int> (63, 194));
	TT_centre.push_back(std::pair<int, int> (83, 224));
	TT_centre.push_back(std::pair<int, int> (73, 344));
	TT_centre.push_back(std::pair<int, int> (83, 358));
	TT_centre.push_back(std::pair<int, int> (-13, 18));
	TT_centre.push_back(std::pair<int, int> (-18, 23));
	TT_centre.push_back(std::pair<int, int> (-8, 53));
	TT_centre.push_back(std::pair<int, int> (-3, 63));
	TT_centre.push_back(std::pair<int, int> (-53, 128));
	TT_centre.push_back(std::pair<int, int> (-53, 183));
	TT_centre.push_back(std::pair<int, int> (-83, 193));
	TT_centre.push_back(std::pair<int, int> (-74, 218));
	TT_centre.push_back(std::pair<int, int> (-8, 223));
	TT_centre.push_back(std::pair<int, int> (-68, 303));
	TT_centre.push_back(std::pair<int, int> (-43, 328));
	TT_centre.push_back(std::pair<int, int> (-23, 167));
}

void InitializeDeadTTEEP(std::vector<std::pair<int, int> >& TT_centre)
{
	//  TT_centre.push_back(std::pair<int,int> (78,78));
	//  TT_centre.push_back(std::pair<int,int> (83,28));
	//  TT_centre.push_back(std::pair<int,int> (83,23));
	TT_centre.push_back(std::pair<int, int> (78, 78));
	TT_centre.push_back(std::pair<int, int> (83, 28));
	TT_centre.push_back(std::pair<int, int> (33, 18));
}

void InitializeDeadTTEEM(std::vector<std::pair<int, int> >& TT_centre)
{
	//  TT_centre.push_back(std::pair<int,int> (53,28));
	TT_centre.push_back(std::pair<int, int> (53, 28));
	TT_centre.push_back(std::pair<int, int> (28, 33));
	TT_centre.push_back(std::pair<int, int> (89, 80));
}

void InitializeDeadTTEEP2012(std::vector<std::pair<int, int> >& TT_centre)
{
	TT_centre.push_back(std::pair<int, int> (78, 78));
	TT_centre.push_back(std::pair<int, int> (83, 28));
	TT_centre.push_back(std::pair<int, int> (83, 23));
	TT_centre.push_back(std::pair<int, int> (91, 23));
}

void InitializeDeadTTEEM2012(std::vector<std::pair<int, int> >& TT_centre)
{
	TT_centre.push_back(std::pair<int, int> (53, 28));
	TT_centre.push_back(std::pair<int, int> (28, 33));
	TT_centre.push_back(std::pair<int, int> (89, 80));
}






int GetNRegionsEB(const std::string& type)
{
	if( type == "none"        ) return 1;

	if( type == "R9"          ) return 2;

	if( type == "charge"      ) return 2;

	if( type == "eta1"        ) return 1;
	if( type == "eta2"        ) return 2;
	if( type == "eta4"        ) return 4;

	if( type == "absEta2"     ) return 2;

	if( type == "eta2-charge" ) return 4;

	if( type == "absEta2-charge" ) return 4;

	return 0;
}

int templIndexEB(const std::string& type, const float& eta, const float& charge, const float& R9)
{
	if( type == "none" ) {
		return 0;
	}


	if( type == "R9" ) {
		if( R9  < 0.94 ) return 0;
		if( R9 >= 0.94 ) return 1;
	}


	if( type == "charge" ) {
		if( charge < 0. ) return 0;
		if( charge > 0. ) return 1;
	}


	if( type == "eta1" ) {
		return 0;
	}
	if( type == "eta2" ) {
		if( eta  < 0. ) return 0;
		if( eta >= 0. ) return 1;
	}
	if( type == "eta4" ) {
		if( (eta  < -1.)               ) return 0;
		if( (eta >= -1.) && (eta < 0.) ) return 1;
		if( (eta >=  0.) && (eta < 1.) ) return 2;
		if( (eta >=  1.)               ) return 3;
	}


	if( type == "absEta2" ) {
		if( fabs(eta)  < 1. ) return 0;
		if( fabs(eta) >= 1. ) return 1;
	}


	if( type == "eta2-charge" ) {
		if( eta  < 0. ) {
			if( charge < 0. ) return 0;
			if( charge > 0. ) return 1;
		}
		if( eta >= 0. ) {
			if( charge < 0. ) return 2;
			if( charge > 0. ) return 3;
		}
	}

	if( type == "absEta2-charge" ) {
		if( fabs(eta)  < 1. ) {
			if( charge < 0. ) return 0;
			if( charge > 0. ) return 1;
		}
		if( fabs(eta) >= 1. ) {
			if( charge < 0. ) return 2;
			if( charge > 0. ) return 3;
		}
	}

	return -1;
}



int GetNRegionsEE(const std::string& type)
{
	if( type == "none"        ) return 1;

	if( type == "R9"          ) return 2;

	if( type == "charge"      ) return 2;

	if( type == "eta1"        ) return 1;
	if( type == "eta2"        ) return 2;
	if( type == "eta4"        ) return 4;

	if( type == "absEta2"     ) return 2;

	if( type == "eta2-charge" ) return 4;

	if( type == "absEta2-charge" ) return 4;

	return 0;
}

int templIndexEE(const std::string& type, const float& eta, const float& charge, const float& R9)
{
	if( type == "none" ) {
		return 0;
	}


	if( type == "R9" ) {
		if( R9  < 0.94 ) return 0;
		if( R9 >= 0.94 ) return 1;
	}


	if( type == "charge" ) {
		if( charge < 0. ) return 0;
		if( charge > 0. ) return 1;
	}


	if( type == "eta1" ) {
		return 0;
	}
	if( type == "eta2" ) {
		if( eta  < 0. ) return 0;
		if( eta >= 0. ) return 1;
	}
	if( type == "eta4" ) {
		if( (eta  < -2.)                     ) return 0;
		if( (eta >= -2.)   && (eta < -1.479) ) return 1;
		if( (eta >= 1.479) && (eta < 2.)     ) return 2;
		if( (eta >= 2.)                      ) return 3;
	}


	if( type == "absEta2" ) {
		if( fabs(eta)  < 2. ) return 0;
		if( fabs(eta) >= 2. ) return 1;
	}


	if( type == "eta2-charge" ) {
		if( eta  < 0. ) {
			if( charge < 0. ) return 0;
			if( charge > 0. ) return 1;
		}
		if( eta >= 0. ) {
			if( charge < 0. ) return 2;
			if( charge > 0. ) return 3;
		}
	}


	if( type == "absEta2-charge" ) {
		if( fabs(eta)  < 2. ) {
			if( charge < 0. ) return 0;
			if( charge > 0. ) return 1;
		}
		if( fabs(eta) >= 2. ) {
			if( charge < 0. ) return 2;
			if( charge > 0. ) return 3;
		}
	}


	return -1;
}
