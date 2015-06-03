#ifndef runDivide_class_hh
#define runDivide_class_hh

#include <TChain.h>
#include <TFile.h>
#include <TString.h>

#include <iostream>
#include <fstream>

#include <map>
#include <set>


using namespace std;
//#define DEBUG


class runDivide_class{

public: 
  runDivide_class(void);
  ~runDivide_class(void);

  std::map<unsigned int,Long64_t> eventsRunMap;
  std::map<unsigned int,UInt_t> runMinTimeRunMap;
  std::map<unsigned int,UInt_t> runMaxTimeRunMap;

  void ReadRunRangeLimits(TString fileName);
  //  void LoadRunEventNumbers(TTree *tree, TString runNumber_branchName);
  void LoadRunEventNumbers(TTree *tree, TString runNumber_branchName="runNumber", TString runTime_branchName="runTime");
  
  void FillRunLimits(unsigned int nEvents_min=50000); 
  std::vector<TString> GetRunRanges(void);
  std::vector<std::pair<TString,Long64_t> > GetRunRangeEvents(void);

  std::vector<TString> Divide(TTree *tree, 
			      TString fileName, // limits
			      unsigned int nEvents_min=50000, 
			      TString runNumber_branchName="runNumber");

  void PrintMap(void);
  void PrintRunRangeEvents(void);

  TString GetRunRangeTime(TString runRange);
private:
  
  std::set<unsigned int> limits; // set of limits given by source file
  std::set<unsigned int> runLimits; // set of limits by minEvent number and limits
  //std::map<unsigned int, Long64_t> runLimits;
  void PrintRunRanges(void);


  
};


#endif
