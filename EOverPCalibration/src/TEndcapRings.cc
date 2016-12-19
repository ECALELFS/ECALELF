#include "../interface/TEndcapRings.h"



//-----
// ctor

TEndcapRings::TEndcapRings()
{
	// initialization
	for(int ix = 1; ix <= 100; ++ix)
		for(int iy = 1; iy <= 100; ++iy)
			for(int iz = 0; iz <= 1; ++iz)
				iEndcapRing[ix][iy][iz] = -1;

	FILE *fRing;
	fRing = fopen("/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/EOverPCalibration/CommonTools/eerings.dat", "r");
	std::cout << "Inizializing endcap geometry from: eerings.dat" << std::endl;
	int ix, iy, iz, ir;
	while(fscanf(fRing, "(%d,%d,%d) %d \n", &ix, &iy, &iz, &ir) != EOF ) {
		if( iz < 0 ) iz = 0;
		iEndcapRing[ix][iy][iz] = ir;
	}

	return;
}

//-----
// dtor

TEndcapRings::~TEndcapRings()
{
	return;
}



//--------
// methods

int TEndcapRings::GetEndcapRing(int ix, int iy, int iz)
{
	int iSide = iz;
	if( iSide < 0 ) iSide = 0;
	return iEndcapRing[ix][iy][iSide];
}

int TEndcapRings::GetEndcapIeta(int ix, int iy, int iz)
{
	int iSide = iz;
	if( iSide < 0 ) iSide = 0;
	int iEtaOffset = 86 * iz;
	int iEta = iEtaOffset + iz * iEndcapRing[ix][iy][iSide];

	return iEta;
}

int TEndcapRings::GetEndcapIphi(int ix, int iy, int iz)
{
	double iX = ix - 50.5;
	double iY = iy - 50.5;
	int iPhi = -1;

	if( iX > 0 && iY > 0 ) iPhi =  int(       10 + TMath::ATan(iY / iX) * 360. / (2.*TMath::Pi()) ) % 360 + 1;
	if( iX < 0 && iY > 0 ) iPhi =  int( 180 + 10 + TMath::ATan(iY / iX) * 360. / (2.*TMath::Pi()) ) % 360 + 1;
	if( iX < 0 && iY < 0 ) iPhi =  int( 180 + 10 + TMath::ATan(iY / iX) * 360. / (2.*TMath::Pi()) ) % 360 + 1;
	if( iX > 0 && iY < 0 ) iPhi =  int( 360 + 10 + TMath::ATan(iY / iX) * 360. / (2.*TMath::Pi()) ) % 360 + 1;

	return iPhi;
}
