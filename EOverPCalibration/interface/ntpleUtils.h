#ifndef ntpleUtils_h
#define ntpleUtils_h

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>
#include <iomanip>

#include "TFile.h"
#include "TH1F.h"
#include "TChain.h"
#include "TVector3.h"
//#include "Math/Vector3D.h"
//#include "Math/Vector4D.h"
//#include "Math/Boost.h"


/** get the number of events from a list of files */
std::map<int, int> GetTotalEvents(const std::string& histoName, const std::string& inputFileList);
TH1F* GetTotalHisto(const std::string& histoName, const std::string& inputFileList);
std::map<int, std::string> GetBinLabels(const std::string& histoName, const std::string& inputFileList);

/** fill a chain from a list of files */
bool FillChain(TChain& chain, const std::string& inputFileList);


/** compute delta phi */
double deltaPhi (const double& phi1, const double& phi2);

/** compute delta eta */
double deltaEta (const double& eta1, const double& eta2);

/** compute delta R */
double deltaR (const double& eta1, const double& phi1,
               const double& eta2, const double& phi2);



/** print 4-vector */
//void Print4V(const ROOT::Math::XYZTVector& p);

#endif
