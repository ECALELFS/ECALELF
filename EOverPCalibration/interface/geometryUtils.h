#ifndef geometryUtils_h
#define geometryUtils_h

//ECAL GEOMETRY

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>
#include "TSicCrystals.h"

/// EE Geometry ///
class TEndcapRegions
{
private:
	int iEndcapRing[100][100][3];
	int iLMR[100][100][2];
public:
	TEndcapRegions();
	virtual ~TEndcapRegions();
	int GetNRegions(const std::string& , const int );
	int GetRegionId(const int, const int, const int, const std::string&);
	int GetEndcapRing(const int, const int, const int, const int);
	int GetEndcapIeta(const int, const int, const int, const std::string&, const int);

};

/// EB Geometry
class TBarrelRegions
{

private:
	std::map<int, int> LmrEtaMap;
	int iBarrelRings[171][360];
	int iLMR[171][360];
	int iTT[171][360];
	int iSM[171][360];

public:
	TBarrelRegions();
	virtual ~TBarrelRegions();
	int GetNRegions(const std::string& type);
	int GetNRegionsIeta(const std::string& type);
	int GetRegionId(const int, const int, const std::string&);
	int GetRegionIdIeta(const int& regionId, const std::string& type);


};



#endif
