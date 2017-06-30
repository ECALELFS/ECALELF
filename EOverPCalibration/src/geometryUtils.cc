#include "Calibration/EOverPCalibration/interface/geometryUtils.h"

// --- EE ---
// default constructor, reading the map from file
TEndcapRegions::TEndcapRegions()
{
	//initializing the matrix
	for(int ii = 0; ii < 100; ++ii)
		for(int jj = 0; jj < 100; ++jj) {
			iEndcapRing[ii][jj][2] = -1;///< \todoy to be corrected? it was -1
			iEndcapRing[ii][jj][0] = -1;
			iEndcapRing[ii][jj][+1] = -1;
			iLMR[ii][jj][0] = -1;
			iLMR[ii][jj][1] = -1;
		}
	FILE *fRing;
	fRing = fopen("eeregions.dat", "r");
	std::cout << "Inizializing endcap geometry from: eerings.dat" << std::endl;
	int ix, iy, iz, ir, lmr;
	while(fscanf(fRing, "(%d,%d,%d) %d %d\n", &ix, &iy, &iz, &lmr, &ir) != EOF ) {
		iEndcapRing[ix - 1][iy - 1][iz] = ir;
		iLMR[ix - 1][iy - 1][iz] = lmr;
	}
	return;
}

//dtor
TEndcapRegions::~TEndcapRegions()
{
	return;
}




//methods
int TEndcapRegions::GetNRegions(const std::string& type, const int nRings)
{
	if( type == "LMR" ) return 38;
	if( type == "RING" && nRings == 0 ) return 78;
	if( type == "RING" ) return 2 * nRings;

	return -1;
}

int TEndcapRegions::GetRegionId(const int ix, const int iy, const int iz, const std::string& type)
{
	int iSide = iz;
	if (iSide < 0) iSide = 0;
	if( type == "LMR" ) {
		return iLMR[ix - 1][iy - 1][iSide] + 19 * iSide;
	}
	if(type == "RING") {
		if(iSide == 0)
			return -(iEndcapRing[ix - 1][iy - 1][iz]);
		if(iSide > 0)
			return iEndcapRing[ix - 1][iy - 1][iz];
	}
	return -1;
}

int TEndcapRegions::GetEndcapRing(const int ix, const int iy, const int iz, const int nRings )
{
	int iSide = iz;
	float ringWidth = int(39. / nRings) + 1;
	if(nRings == 0) {
		if(iEndcapRing[ix - 1][iy - 1][iSide] == -1) return -999;
		else return iEndcapRing[ix - 1][iy - 1][iSide];
	} else {
		if(iEndcapRing[ix - 1][iy - 1][iSide] == -1) return -999;
		else {
			if(iSide < 0) return -int(((float)iEndcapRing[ix - 1][iy - 1][iSide] / (float)ringWidth));
			if(iSide > 0) return int(((float)iEndcapRing[ix - 1][iy - 1][iSide] / (float)ringWidth));
		}
	}
	return -1;
}

int TEndcapRegions::GetEndcapIeta(const int ix, const int iy, const int iz, const std::string& type, const int nRings)
{
	int iSide = iz;
	int iEtaOffset = 86 * iz;
	int iEta = iEtaOffset + iz * iEndcapRing[ix - 1][iy - 1][iSide];
	float ringWidth = int(39. / nRings);
	if(type == "RING") {
		if(iEndcapRing[ix - 1][iy - 1][iSide] == -1) return -999;
		else {
			if(iSide < 0) return -int(((float)iEndcapRing[ix - 1][iy - 1][iSide] / (float)ringWidth));
			if(iSide > 0) return int(((float)iEndcapRing[ix - 1][iy - 1][iSide] / (float)ringWidth));
		}
	}
	return iEta;
}




// --- EB ---
// default constructor (NEEDED FOR INPUT REGIONS FROM EXTERNAL FILE)
TBarrelRegions::TBarrelRegions()
{
	//initializing the matrix
	for(int ii = 0; ii < 171; ++ii)
		for(int kk = 0; kk < 360; ++kk) {
			iBarrelRings[ii][kk] = -1;
			iLMR[ii][kk] = -1;
			iSM[ii][kk] = -1;
			iTT[ii][kk] = -1;
		}

	FILE *fRegion;
	fRegion = fopen("ebregions.dat", "r");
	std::cout << "Inizializing barrel geometry from: ebregions.dat" << std::endl;

	int ieta, iphi, sm, tt, lmr;
	std::map<int, int> dummyMap;
	while(fscanf(fRegion, "(%d ,%d) %d %d %d\n", &ieta, &iphi, &sm, &tt, &lmr) != EOF ) {
		iBarrelRings[ieta + 85][iphi - 1] = ieta + 85;
		iLMR[ieta + 85][iphi - 1] = lmr;
		iSM[ieta + 85][iphi - 1] = sm;
		iTT[ieta + 85][iphi - 1] = tt;

		//inverse map containing the ietaMin in that region
		if(dummyMap.find(lmr) == dummyMap.end()) dummyMap[lmr] = abs(ieta);
		else if(abs(ieta) < dummyMap[lmr]) dummyMap[lmr] = abs(ieta);
	}

	int counter = 0;
	std::map<int, int> dummyMapEtaCount;
	for(std::map<int, int>::const_iterator itr = dummyMap.begin(); itr != dummyMap.end(); ++itr) {
		if(dummyMapEtaCount.find(itr->second) == dummyMapEtaCount.end()) {
			dummyMapEtaCount[itr->second] = counter;
			++counter;
		}
		LmrEtaMap[itr->first] = dummyMapEtaCount[itr->second];
	}


	return;
}

//dtor
TBarrelRegions::~TBarrelRegions()
{
	return;
}



//methods
int TBarrelRegions::GetNRegions(const std::string& type)
{
	if( type == "ALL" ) return 1;
	if( type == "SM" ) return 36;
	if( type == "LMR" ) return 324;
	if( type == "TT" ) return 2448;
	if( type == "RING" ) return 171;

	return -1;
}

int TBarrelRegions::GetRegionId(const int iEta, const int iPhi, const std::string& type)
{
	if( (iEta < -85) || (iEta == 0) || (iEta > 85) ) return -1;
	if( (iPhi < 1) || (iPhi > 360) ) return -1;

	if( type == "ALL" ) {
		return 0;
	}
	if( type == "SM" ) {
		return iSM[iEta + 85][iPhi - 1];
	}
	if( type == "LMR" ) {
		return iLMR[iEta + 85][iPhi - 1];
	}
	if( type == "TT" ) {
		return iTT[iEta + 85][iPhi - 1];
	}
	if( type == "RING" ) {
		return iBarrelRings[iEta + 85][iPhi - 1];
	}
	return -1;
}



// ---- geometry functions ----
int TBarrelRegions::GetNRegionsIeta(const std::string& type)
{
	if( type == "SM" ) return 36;
	if( type == "LMR" ) return 5;
	if( type == "TT" ) return 17;
	if( type == "RING" ) return 171;


	return -1;
}


int TBarrelRegions::GetRegionIdIeta(const int& regionId, const std::string& type)
{
	if( type == "LMR" ) {
		return ( LmrEtaMap[regionId] );
	}
	if( type == "SM" ) {
		return regionId;
	}
	if( type == "RING" ) {
		return regionId;
	}

	return -1;

	//--------------------------------

	// int nPhiRegions;
	// float regionEtaWidth;
	// float regionPhiWidth;

	// if( type == "SM" )
	//   {
	//     nPhiRegions = 18;
	//     regionEtaWidth = 85.;
	//     regionPhiWidth = 20.;
	//   }
	// if( type == "TT" )
	//   {
	//     nPhiRegions = 72;
	//     regionEtaWidth = 5.;
	//     regionPhiWidth = 5.;
	//   }

	// iPhi = regionPhiWidth/2. + regionPhiWidth * (regionId % nPhiRegions);
	// iEta = regionEtaWidth/2. + regionPhiWidth * (regionId / nPhiRegions);

	// iPhi += 1.;
	// if( iEta  < 75 ) iEta = -1. * (iEta+1.);
	// if( iEta >= 75 ) iEta -= 74.;
}


