#include "Calibration/EOverPCalibration/interface/GetHashedIndexEB.h"

int GetHashedIndexEB(int iEta, int iPhi, int Zside)
{
	int MAX_IETA = 85;
	int MAX_IPHI = 360;
	bool positiveZ = true;
	if ( Zside == -1 ) positiveZ = false;
	int absEta = iEta * Zside;

	return ((MAX_IETA + (positiveZ ? absEta - 1 : -absEta) ) * MAX_IPHI + iPhi - 1);
}

int GetIetaFromHashedIndex(int Index)
{
	int MAX_IETA = 85;
	int MAX_IPHI = 360;

	int iEta = Index / MAX_IPHI - MAX_IETA;
	if ( iEta >= 0 ) iEta++;
	return iEta;
}

int GetIphiFromHashedIndex(int Index)
{
	int MAX_IETA = 85;
	int MAX_IPHI = 360;

	int iEta = Index / MAX_IPHI - MAX_IETA;
	int iPhi = Index - (MAX_IETA + iEta) * MAX_IPHI + 1;
	return iPhi;
}
