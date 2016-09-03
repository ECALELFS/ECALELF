/// Zee Fit program

/**\file 
The aim of the program is to provide a common interface to all the Z
fitting algorithms reading and combining in the proper way the
configuration files.

\todo
   - remove commonCut from category name and add it in the ZFit_class in order to not repeate the cut
   - make alpha fitting more generic (look for alphaName)
   - Implement the iterative Et dependent scale corrections

*/
 #include <TGaxis.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>
#include <fstream> 
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <boost/program_options.hpp>

#include <TEventList.h>
#include <TChain.h>
#include <TStopwatch.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2F.h>
#include <TFriendElement.h>
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TLatex.h"


/// @cond SHOW
/// \code
#include "../interface/ZFit_class.hh"
#include "../interface/puWeights_class.hh"
#include "../interface/r9Weights_class.hh"
#include "../interface/ZPtWeights_class.hh"

#include "../interface/runDivide_class.hh"
#include "../interface/EnergyScaleCorrection_class.hh"
#include "../interface/addBranch_class.hh"

#include "../interface/RooSmearer.hh"

#include "../interface/nllProfile.hh"

#include "../../EOverPCalibration/interface/ntpleUtils.h"
#include "../../EOverPCalibration/interface/CalibrationUtils.h"
#include "../../EOverPCalibration/interface/FastCalibratorEB.h"
#include "../../EOverPCalibration/interface/FastCalibratorEE.h"

#include "../../EOverPCalibration/interface/TEndcapRings.h"
#include "../../EOverPCalibration/CommonTools/histoFunc.h"
//#include "treeReader.h"
//#include "setTDRStyle.h"
#include "../../EOverPCalibration/interface/ntpleUtils2.h"
#include "../../EOverPCalibration/interface/ConvoluteTemplate.h"
#include "../../EOverPCalibration/interface/stabilityUtils.h"
#include "../../EOverPCalibration/interface/geometryUtils.h"

/// \endcode
/// @endcond

#include <RooMinuit.h>
//#include <RooStats/UniformProposal.h>
//#include <RooStats/PdfProposal.h>
//#include <RooStats/SequentialProposal.h>
#include <RooGaussian.h>
#include <RooStats/MetropolisHastings.h>
#include "RooBinning.h"
//#include "../src/ShervinMinuit.cc"
//#include <RooStats/ProposalHelper.h>
#include <RooMultiVarGaussian.h>

#include "Math/Minimizer.h"

#include <TPRegexp.h>
#include <RooFormulaVar.h>
#include <RooPullVar.h>

#include <TMatrixDSym.h>

#include <TRandom3.h>
#include <queue>

#define profile

//#define DEBUG
#define smooth

//#include "../macro/loop.C" // a way to use compiled macros with ZFitter

//using namespace std;
using namespace RooStats;

///\endcond


/// map that associates the name of the tree and the pointer to the chain
typedef std::map< TString, TChain* > chain_map_t;
 
/** \brief map that associates the name of the tag to the chain_map_t
 *
 * the logic is better described in \ref validationConfig
 */
typedef std::map< TString, chain_map_t > tag_chain_map_t;


/** Function parsing the region files
 * \retval vector of strings, each string is the name of one region
 */
std::vector<TString> ReadRegionsFromFile(TString fileName){
  ifstream file(fileName);
  std::vector<TString> regions;
  TString region;

  while(file.peek()!=EOF && file.good()){
    if(file.peek()==10){ // 10 = \n
      file.get(); 
      continue;
    }

    if(file.peek() == 35){ // 35 = #
      file.ignore(1000,10); // ignore the rest of the line until \n
      continue;
    }

    file >> region;
    file.ignore(1000,10); // ignore the rest of the line until \n
#ifdef DEBUG
    std::cout << "[DEBUG] Reading region: " << region<< std::endl;
#endif
    regions.push_back(region);

  }
  return regions;
}


/**
 * This function reassociates the chains as friends of the "selected" tree.
 *
 * This function should be run when new chains or files are added to the tagChainMap.
 * 
 */
void UpdateFriends(tag_chain_map_t& tagChainMap, TString regionsFileNameTag){
//void UpdateFriends(tag_chain_map_t& tagChainMap){
  // loop over all the tags
  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
    // take the selected tree of that tag
    TChain *chain = (tag_chain_itr->second.find("selected"))->second;

    // loop over all the trees
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){

      if(chain_itr->first!="selected"){ //except the selected
	if(chain->GetFriend(chain_itr->first)==NULL){
	  std::cout << "[STATUS] Adding friend branch: " << chain_itr->first
		    << " to tag " << tag_chain_itr->first << std::endl;
	  chain->AddFriend(chain_itr->second);
	} // already added
      }

      if(chain->GetEntries()!= chain_itr->second->GetEntries()){
	std::cerr << "[ERROR] Not the same number of events: " << chain->GetEntries() << "\t" << chain_itr->second->GetEntries() << std::endl;
	exit(1);
      }
    }
  }
  return;
}

void Dump(tag_chain_map_t& tagChainMap, TString tag="s", Long64_t firstentry=0){
  (tagChainMap[tag])["selected"]->Scan("etaEle:R9Ele:energySCEle_regrCorrSemiParV5_pho/cosh(etaSCEle):smearerCat:catName","","col=5:4:5:3:50",5,firstentry);
}

/**
 * \param tagChainMap map of all the tags declared in the validation config file 
 * \param tag name of the new \b tag created by the function, all the existent tags with name starting with 
 b tag are merged in the new \b tag
 *
 * A new tagChain with name=tag is added to the tagChainMap. All the tagChains with tag starting with \b tag are merged
 * After the merging the friend list is updated by \ref UpdateFriends
 */
void MergeSamples(tag_chain_map_t& tagChainMap, TString regionsFileNameTag, TString tag="s"){
  
  std::pair<TString, chain_map_t > pair_tmp_tag(tag,chain_map_t()); // make_pair not work with scram b
  tagChainMap.insert(pair_tmp_tag);
  
  //loop over all the tags
  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
    
    // consider tags matching the tag input parameter
    if(tag_chain_itr->first.CompareTo(tag)==0 || !tag_chain_itr->first.Contains(tag)) continue; //do it for each sample
    
    // loop over all the trees
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){
      TString chainName = chain_itr->first;
      if(tagChainMap.count(chainName)==0){ // create the new chain if does not exist for tag
	std::pair<TString, TChain* > pair_tmp(chainName, new TChain(chainName));
	(tagChainMap[tag]).insert(pair_tmp);
	(tagChainMap[tag])[chainName]->SetTitle(tag);
      }
      (tagChainMap[tag])[chainName]->Add(chain_itr->second);
      std::cout << tag << "\t" << tag_chain_itr->first << "\t" << chainName <<  "\t" << chain_itr->second << "\t" << chain_itr->second->GetTitle() << std::endl;
      
    }
  }
  UpdateFriends(tagChainMap, regionsFileNameTag);
  return;
}


/**
 * \brief Function returning the name of the energy branch given the name of the invariant mass
 */
std::string energyBranchNameFromInvMassName(std::string invMass_var){

  std::string energyBranchName="";
  if(invMass_var=="invMass_SC_regrCorr_ele") energyBranchName = "energySCEle_regrCorr_ele";
  else if(invMass_var=="invMass_SC_regrCorr_pho") energyBranchName = "energySCEle_regrCorr_pho";
  else if(invMass_var=="invMass_regrCorr_fra") energyBranchName = "energyEle_regrCorr_fra";
  else if(invMass_var=="invMass_regrCorr_egamma") energyBranchName = "energyEle_regrCorr_egamma";
  else if(invMass_var=="invMass_SC") energyBranchName = "energySCEle";
  else if(invMass_var=="invMass_SC_must") energyBranchName = "energySCEle_must";
  else if(invMass_var=="invMass_SC_must_regrCorr_ele") energyBranchName = "energySCEle_must_regrCorr_ele";
  else if(invMass_var=="invMass_rawSC") energyBranchName = "rawEnergySCEle";
  else if(invMass_var=="invMass_rawSC_esSC") energyBranchName = "rawEnergySCEle+esEnergySCEle";
  else if(invMass_var=="invMass_SC_corr") energyBranchName = "energySCEle_corr";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV4_ele") energyBranchName = "energySCEle_regrCorrSemiParV4_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV4_pho") energyBranchName = "energySCEle_regrCorrSemiParV4_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV5_ele") energyBranchName = "energySCEle_regrCorrSemiParV5_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV5_pho") energyBranchName = "energySCEle_regrCorrSemiParV5_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV6_ele") energyBranchName = "energySCEle_regrCorrSemiParV6_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV6_pho") energyBranchName = "energySCEle_regrCorrSemiParV6_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV7_ele") energyBranchName = "energySCEle_regrCorrSemiParV7_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV7_pho") energyBranchName = "energySCEle_regrCorrSemiParV7_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV8_ele") energyBranchName = "energySCEle_regrCorrSemiParV8_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV8_pho") energyBranchName = "energySCEle_regrCorrSemiParV8_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiPar7TeVtrainV6_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV6_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiPar7TeVtrainV6_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV6_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiPar7TeVtrainV7_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV7_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiPar7TeVtrainV7_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV7_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiPar7TeVtrainV8_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV8_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiPar7TeVtrainV8_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV8_pho";

  else {
    std::cerr << "Energy branch name not define for invariant mass branch: " << invMass_var << std::endl;
    exit(1);
  }
  return energyBranchName;
}



TString energyBranchNameFromInvMassName(TString invMass_var){
  return TString( (energyBranchNameFromInvMassName(std::string(invMass_var))).c_str());
}

int main(int argc, char **argv) {
  TStopwatch myClock;
  TStopwatch globalClock;
  globalClock.Start();

  puWeights_class puWeights;
  std::cout << "============================== Z General Fitter" << std::endl;

  //------------------------------------------------------------
  // parsing input options to the program
  using namespace boost;
  namespace po = boost::program_options;
  unsigned int nEvents_runDivide=100000;
  std::string chainFileListName;
  std::string regionsFileName;
  std::string runRangesFileName;
  std::string dataPUFileName, mcPUFileName;
  std::vector<TString> dataPUFileNameVec, mcPUFileNameVec;
  std::string r9WeightFile;
  std::string ZPtWeightFile;
  std::string initFileName;
  //  bool savePUweightTree;
  std::string imgFormat="eps", outDirFitResMC="test/MC/fitres", outDirFitResData="test/dato/fitres", outDirImgMC="test/MC/img", outDirImgData="test/dato/img", outDirTable="test/dato/table", selection;
  TString eleID="";
  //std::vector<std::string> signalFiles, bkgFiles, dataFiles;
  std::string commonCut;
  std::string corrEleFile, corrEleType;
  std::string smearEleFile, smearEleType;
  double smearingCBAlpha=1, smearingCBPower=5;
  std::string invMass_var;
  float invMass_min=0, invMass_max=0, invMass_binWidth=0.250;
  int fit_type_value=1;
  int signal_type_value=0;
  unsigned long long int nToys = 0;
  float constTermToy=0;
  unsigned long long int nEventsPerToy = 0;
  unsigned int nIter =0;
  unsigned int nEventsMinDiag=0;
  unsigned int nEventsMinOffDiag=0;
  unsigned int nSmearToy=1;
 
  int pdfSystWeightIndex=-1;
  std::string minimType;
  std::vector<std::string> branchList;





//options for E/p
  std::string jsonFileName;
  std::string miscalibMap;
  bool isMiscalib;
  bool applyPcorr;
  bool applyEcorr;
  bool isSaveEPDistribution;
  bool isMCTruth;
  bool isEPselection;
  bool isPtCut;
  float PtMin;
  bool isfbrem;
  float fbremMax;
  bool isR9selection;
  float R9Min;
  float EPMin;
  int smoothCut;
  int miscalibMethod;
  std::string inputMomentumScale;
  std::string inputEnergyScale;
  std::string typeEB;
  std::string typeEE;
  std::string outputPath;
  std::string outputFile;
  int numberOfEvents;
  int useZ, useW;
  int useRawEnergy;
  //  int isBarrel;
  int splitStat;
  int nLoops;
  bool isDeadTriggerTower;
  std::string inputFileDeadXtal;
  std::string EBEE;
  std::string EBEEpu;
  int evtsPerPoint;
  
  int useRegression;
  int targetTypeEB, targetTypeEE;
  float R9cutEB, R9cutEE;
  int energyTypeEB, energyTypeEE;
  float yMIN;
  float yMAX;
  std::string dayMin;
  std::string dayMax;
  std::string dayZOOM;
  std::string LUMI;
  //------------------------------ setting option categories
  po::options_description desc("Main options");
  po::options_description outputOption("Output options");
  po::options_description inputOption("Input options");
  po::options_description fitterOption("Z fitter options");
  po::options_description smearerOption("Z smearer options");
  po::options_description toyOption("toyMC options");
  po::options_description EoverPOption("EoverP options");
  po::options_description momentumCorrectionEB("run the momentum calibration for barrel");
  po::options_description momentumCorrectionEE("run the momentum calibration for endcap");
  po::options_description laserMonitoringEPOption("laser monitoring with E/p options");
  po::options_description laserMonitoringEPvsPUOption("laser monitoring with E/p versus Pile Up options");



  //po::options_description cmd_line_options;
  //cmd_line_options.add(desc).add(fitOption).add(smearOption);

  //------------------------------ adding options' description
  desc.add_options()
    ("help,h","Help message")
    ("loop","")
    
    ("runDivide", "execute the run division")
    ("nEvents_runDivide", po::value<unsigned int>(&nEvents_runDivide)->default_value(100000), "Minimum number of events in a run range")

    //
    ("dataPU", po::value< string >(&dataPUFileName), "")
    ("mcPU", po::value<string>(&mcPUFileName),"")
    ("noPU", "")
    ("savePUTreeWeight","")
    //
    ("corrEleFile", po::value<string>(&corrEleFile),"File with energy scale corrections")
    ("corrEleType", po::value<string>(&corrEleType),"Correction type/step")
    ("saveCorrEleTree", "")

    ("smearEleFile", po::value<string>(&smearEleFile),"File with energy smearings")
    ("smearEleType", po::value<string>(&smearEleType),"Correction type/step")
    ("smearingCBAlpha", po::value<double>(&smearingCBAlpha),"Correction type/step")
    ("smearingCBPower", po::value<double>(&smearingCBPower),"Correction type/step")
    //
    ("r9WeightFile", po::value<string>(&r9WeightFile),"File with r9 photon-electron weights")
    ("useR9weight", "use r9 photon-electron weights")
    ("saveR9TreeWeight", "")
    ("ZPtWeightFile", po::value<string>(&ZPtWeightFile),"File with ZPt weights")
    ("useZPtweight", "use ZPt weights")
    ("useFSRweight", "activate the FSR weight in MC")
    ("useWEAKweight", "activate the WEAK interference weight in MC")
    ("saveRootMacro","")
    //
    ("selection", po::value<string>(&selection)->default_value("loose25nsRun2"),"")
    ("commonCut", po::value<string>(&commonCut)->default_value("Et_25"),"")
    ("invMass_var", po::value<string>(&invMass_var)->default_value("invMass_SC_must"),"")
    ("invMass_min", po::value<float>(&invMass_min)->default_value(65.),"")
    ("invMass_max", po::value<float>(&invMass_max)->default_value(115.),"")
    ("invMass_binWidth", po::value<float>(&invMass_binWidth)->default_value(0.25),"Smearing binning")
    ("isOddMC", "Activate if use only odd events in MC")
    ("isOddData", "Activate if use only odd events in data")    
    //
    ("readDirect","") //read correction directly from config file instead of passing as a command line arg
    //("addPtBranches", "")  //add new pt branches ( 3 by default, fra, ele, pho)
    ("addBranch", po::value< std::vector<string> >(&branchList), "")
    ("saveAddBranchTree","")
    //    ("signal,s", po::value< std::vector <string> >(&signalFiles), "Signal file (can be called multiple times putting the files in a chain")
    //    ("bkg,b", po::value< std::vector <string> >(&bkgFiles), "Bkg file (can be called multiple times putting the files in a chain")
    //    ("data,d", po::value< std::vector <string> >(&dataFiles), "Data file (can be called multiple times putting the files in a chain")
    //("_ZFit_class", "call the Z fitter")
    //;
    //
    //  po::options_description fitOption("Z Fit options");
    //  po::options_description smearOption("Z smearing options");
    //  smearOption.add_options()
    ;
  fitterOption.add_options()
    ("fit_type_value", po::value<int>(&fit_type_value)->default_value(1),"0=floating tails, 1=fixed tails")
    ("signal_type_value", po::value<int>(&signal_type_value)->default_value(0),"0=BW+CB, 1=Cruijff")
    ("forceNewFit", "refit MC also if fit exists")
    ("updateOnly",  "do not fit data if fit exists")
    ;;
  smearerOption.add_options()
    ("smearerFit",  "call the smearing")
    ("smearerType", po::value<string>(&minimType)->default_value("profile"), "minimization algo")
    ("onlyDiagonal","if want to use only diagonal categories")
    ("autoBin", "")
    ("autoNsmear", "")
    ("smearscan", "")
    ("isDataSmeared","")
    ("plotOnly", "active if you don't want to do the smearing")
    ("profileOnly", "")
    ("numIter", po::value<unsigned int>(&nIter)->default_value(300), "number of MCMC steps")
    ("nEventsMinDiag", po::value<unsigned int>(&nEventsMinDiag)->default_value(1000), "min num events in diagonal categories")
    ("nEventsMinOffDiag", po::value<unsigned int>(&nEventsMinOffDiag)->default_value(2000), "min num events in off-diagonal categories")
    ("onlyScale",    "fix the smearing to constant")
    ("constTermFix", "constTerm not depending on Et")
    ("alphaGoldFix", "alphaTerm for gold electrons fixed to the low eta region")
    ("smearingEt", "alpha term depend on sqrt(Et) and not on sqrt(E)")
    ("nSmearToy", po::value<unsigned int>(&nSmearToy)->default_value(0), "")
    ("pdfSystWeightIndex", po::value<int>(&pdfSystWeightIndex)->default_value(-1), "Index of the weight to be used")
    ;
  inputOption.add_options()
    ("chainFileList,f", po::value< string >(&chainFileListName), "Configuration file with input file list")
    ("regionsFile", po::value< string >(&regionsFileName), "Configuration file with regions")
    ("runRangesFile", po::value< string >(&runRangesFileName), "Configuration file with run ranges")
    ("initFile", po::value< string >(&initFileName), "Configuration file with init values of fit model")
    ;
  outputOption.add_options()
    ("imgFormat", po::value<string>(&imgFormat)->default_value("eps"),"")
    ("outDirFitResMC", po::value<string>(&outDirFitResMC),"")
    ("outDirFitResData", po::value<string>(&outDirFitResData),"")
    ("outDirImgMC", po::value<string>(&outDirImgMC),"")
    ("outDirImgData", po::value<string>(&outDirImgData),"")
    //("outDirTable", po::value<string>(&outDirTable),"")
    ;
  toyOption.add_options()
    ("runToy","")
    ("nToys", po::value<unsigned long long int>(&nToys)->default_value(1000),"")
    ("constTermToy", po::value<float>(&constTermToy)->default_value(0.01),"")
    ("eventsPerToy", po::value<unsigned long long int>(&nEventsPerToy)->default_value(0),"=0: all events")
    ;
  momentumCorrectionEE.add_options()
    ("momentumCorrectionEE",  "run the momentum calibration for endcap")
    ("targetTypeEE", po::value<int>(&targetTypeEE)->default_value(1),"compute corrections on: 1=pTk, 2=energy, 3=E/p")
    ("R9cutEE", po::value<float>(&R9cutEE)->default_value(0.),"apply R9 cut")
    ("energyTypeEE", po::value<int>(&energyTypeEE)->default_value(0),"0=rawEnergy, 1=scEnergy")
    ;
  momentumCorrectionEB.add_options()
    ("momentumCorrectionEB",  "run the momentum calibration for barrel")
    ("targetTypeEB", po::value<int>(&targetTypeEB)->default_value(1),"compute corrections on: 1=pTk, 2=energy, 3=E/p")
    ("R9cutEB", po::value<float>(&R9cutEB)->default_value(0.),"apply R9 cut")
    ("energyTypeEB", po::value<int>(&energyTypeEB)->default_value(0),"0=rawEnergy, 1=scEnergy")
    ;
  laserMonitoringEPOption.add_options()
    ("laserMonitoringEP", "call the laser monitoring with E/p")
    ("yMIN", po::value<float>(&yMIN)->default_value(0.65),"y min")
    ("yMAX", po::value<float>(&yMAX)->default_value(1.05),"y max")
    ("EBEE", po::value<string>(&EBEE)->default_value("EB"),"barrel or endcap")
    ("evtsPerPoint", po::value<int>(&evtsPerPoint)->default_value(1000),"events per point")
    ("useRegression", po::value<int>(&useRegression)->default_value(0),"use regression")
    ("dayMin", po::value<string>(&dayMin)->default_value("1-7-2015"),"day min")
    ("dayMax", po::value<string>(&dayMax)->default_value("15-11-2015"),"day max")
    ("dayZOOM", po::value<string>(&dayZOOM)->default_value("10-8-2015"),"day ZOOM")
    ("LUMI", po::value<string>(&LUMI)->default_value("1.9"),"LUMI")
    ;
   laserMonitoringEPvsPUOption.add_options()
    ("laserMonitoringEPvsPU", "call the laser monitoring with E/p")
     //    ("EBEEpu", po::value<string>(&EBEEpu)->default_value("EB"),"barrel or endcap")
     //("evtsPerPointpu", po::value<int>(&evtsPerPoint)->default_value(1000),"events per point")
     //    ("useRegression", po::value<int>(&useRegression)->default_value(0),"use regression")
     //    ("dayMin", po::value<string>(&dayMin)->default_value("1-6-2015"),"day min")
     //    ("dayMax", po::value<string>(&dayMax)->default_value("1-10-2015"),"day max")
     //    ("dayZOOM", po::value<string>(&dayZOOM)->default_value("10-8-2015"),"day ZOOM")
    ;
  EoverPOption.add_options()
    ("EOverPCalib",  "call the E/p calibration")
    //  ("isBarrel", po::value<int>(&isBarrel)->default_value(1),"1=barrel, 0=endcap")
    ("doEB", "do EoP IC calibration for EB")
    ("doEE", "do EoP IC calibration for EE")
    ("jsonFileName", po::value<string>(&jsonFileName)->default_value("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-258750_13TeV_PromptReco_Collisions15_25ns_JSON.txt"), "jsonFileName")
    ("isMiscalib", po::value<bool>(&isMiscalib)->default_value(false),"apply the initial miscalibration")
    ("miscalibMethod", po::value<int>(&miscalibMethod)->default_value(1),"miscalibration method")  
    ("miscalibMap", po::value<string>(&miscalibMap)->default_value("/gwteray/users/brianza/scalibMap2.txt"), "map for the miscalibration")
    ("isSaveEPDistribution", po::value<bool>(&isSaveEPDistribution)->default_value(false),"save E/P distribution")  
    ("isMCTruth", po::value<bool>(&isMCTruth)->default_value(false),"option for MC") 
    ("isEPselection", po::value<bool>(&isEPselection)->default_value(false),"apply E/p selection")             
    ("isPtCut", po::value<bool>(&isPtCut)->default_value(false),"apply Pt cut")  
    ("PtMin", po::value<float>(&PtMin)->default_value(0.),"treshold for the Pt cut")  
    ("isfbrem", po::value<bool>(&isfbrem)->default_value(false),"apply fbrem cut")      
    ("fbremMax", po::value<float>(&fbremMax)->default_value(100.),"fbrem treshold")  
    ("isR9selection", po::value<bool>(&isR9selection)->default_value(false),"apply R9 selection")  
    ("R9Min", po::value<float>(&R9Min)->default_value(-1.),"R9 treshold")  
    ("applyPcorr", po::value<bool>(&applyPcorr)->default_value(true),"apply momentum correction")
    ("inputMomentumScale", po::value<string>(&inputMomentumScale)->default_value("/afs/cern.ch/user/l/lbrianza/work/public/EoP_additionalFiles/MomentumCalibration2015_eta1_eta1.root"),"input momentum scale")  
    ("applyEcorr", po::value<bool>(&applyEcorr)->default_value(false),"apply energy correction")
    ("inputEnergyScale", po::value<string>(&inputEnergyScale)->default_value("/afs/cern.ch/user/l/lbrianza/work/public/EoP_additionalFiles/momentumCalibration2015_EB_scE.root"),"input energy scale")  
    ("typeEB", po::value<string>(&typeEB)->default_value("eta1"),"")  
    ("typeEE", po::value<string>(&typeEE)->default_value("eta1"),"")  
    ("outputPath", po::value<string>(&outputPath)->default_value("output/output_runD/"),"output dir for E/P calibration")  
    ("outputFile", po::value<string>(&outputFile)->default_value("FastCalibrator_Oct2015_runD"),"output file for E/P calibration")         
    ("numberOfEvents", po::value<int>(&numberOfEvents)->default_value(-1),"number of events (-1=all)") 
    ("useRawEnergy", po::value<int>(&useRawEnergy)->default_value(0),"use raw energy") 
    ("useZ", po::value<int>(&useZ)->default_value(1),"use Z events") 
    ("useW", po::value<int>(&useW)->default_value(1),"use W events") 
    ("splitStat", po::value<int>(&splitStat)->default_value(1),"split statistic")             
    ("nLoops", po::value<int>(&nLoops)->default_value(20),"number of iteration of the L3 algorithm") 
    ("isDeadTriggerTower", po::value<bool>(&isDeadTriggerTower)->default_value(false),"") 
    ("inputFileDeadXtal", po::value<string>(&inputFileDeadXtal)->default_value("NULL"),"") 
    ("EPMin", po::value<float>(&EPMin)->default_value(100.),"E/p window")  
    ("smoothCut", po::value<int>(&smoothCut)->default_value(0),"apply smooth cut on the E/p window")  
    ;
     
  desc.add(inputOption);
  desc.add(outputOption);
  desc.add(fitterOption);
  desc.add(smearerOption);
  desc.add(toyOption);
  desc.add(momentumCorrectionEE);
  desc.add(momentumCorrectionEB);
  desc.add(EoverPOption);
  desc.add(laserMonitoringEPOption);
  desc.add(laserMonitoringEPvsPUOption);
  
  po::variables_map vm;
  //
  // po::store(po::parse_command_line(argc, argv, smearOption), vm);
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    


  //------------------------------ checking options
  if(!vm.count("invMass_binWidth") && !vm.count("smearerFit")){
    std::cout << "[INFO] Bin Width=0.5" << std::endl;
    invMass_binWidth=0.5;
  }

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  if(vm.count("useZPtweight") && !vm.count("pdfSystWeightIndex")){
    std::cerr << "[ERROR] Asked for ZPt weights but no pdfSystWeightIndex indicated" << std::endl;
    exit(1);
  }

  TString energyBranchName=energyBranchNameFromInvMassName(invMass_var).c_str();

  if(!vm.count("chainFileList") && !vm.count("runToy")){
    std::cerr << "[ERROR] Missing mandatory option \"chainFile\"" << std::endl;
    return 1;
  }


  if(!vm.count("regionsFile") && 
     !vm.count("runDivide") && !vm.count("savePUTreeWeight") && 
     !vm.count("saveR9TreeWeight") && !vm.count("saveCorrEleTree") && !vm.count("EOverPCalib") && !vm.count("momentumCorrectionEE") && !vm.count("momentumCorrectionEB") && !vm.count("laserMonitoringEP") && !vm.count("laserMonitoringEPvsPU") 
     //&& !vm.count("saveRootMacro")
     ){
    std::cerr << "[ERROR] Missing mandatory option \"regionsFile\"" << std::endl;
    return 1;
  }
  //  if(!vm.count("runRangesFile")){
  //    std::cerr << "[ERROR] Missing mandatory option \"runRangesFile\"" << std::endl;
  //    return 1;
  //  }

  if(vm.count("dataPU") != vm.count("mcPU")){
    std::cerr << "[ERROR] One PU distribution (data or mc) are not provided" << std::endl;
    return 1;
  }


  //if((vm.count("corrEleType") != vm.count("corrEleFile"))&&(vm.count("corrEleType") != vm.count("readDirect")) ){
  //std::cout << "[ERROR] Either provide correction file name, or provide read direct option" << std::endl;
  //return 1;
  //}

  if( vm.count("corrEleType") && vm.count("corrEleFile") && vm.count("readDirect") ){
    std::cout << "[ERROR] Either provide correction file name, or provide read direct option - choose only one" << std::endl;
    return 1;
  }

  //============================== Check output folders
  bool checkDirectories=true;
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResMC)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("momentumCorrectionEE") && !vm.count("momentumCorrectionEB")){
    std::cerr << "[ERROR] Directory " << outDirFitResMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResData)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("momentumCorrectionEE") && !vm.count("momentumCorrectionEB")){
    std::cerr << "[ERROR] Directory " << outDirFitResData << " not found" << std::endl;
  }
  checkDirectories=checkDirectories &&   !system("[ -d "+TString(outDirImgMC)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("momentumCorrectionEE") && !vm.count("momentumCorrectionEB")){
     std::cerr << "[ERROR] Directory " << outDirImgMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirImgData)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("momentumCorrectionEE") && !vm.count("momentumCorrectionEB")){
    std::cerr << "[ERROR] Directory " << outDirImgData << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResMC)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("laserMonitoringEP") && !vm.count("laserMonitoringEPvsPU")){
    std::cerr << "[ERROR] Directory " << outDirFitResMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResData)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("laserMonitoringEP") && !vm.count("laserMonitoringEPvsPU")){
    std::cerr << "[ERROR] Directory " << outDirFitResData << " not found" << std::endl;
  }
  checkDirectories=checkDirectories &&   !system("[ -d "+TString(outDirImgMC)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("laserMonitoringEP") && !vm.count("laserMonitoringEPvsPU")){
     std::cerr << "[ERROR] Directory " << outDirImgMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirImgData)+" ]");
  if(!checkDirectories && !vm.count("EOverPCalib") && !vm.count("laserMonitoringEP") && !vm.count("laserMonitoringEPvsPU")){
    std::cerr << "[ERROR] Directory " << outDirImgData << " not found" << std::endl;
  }
  //   checkDirectories=checkDirectories && !system("[ -d "+TString(outDirTable)+" ]");
  //   if(!checkDirectories){
  //     std::cerr << "[ERROR] Directory " << outDirTable << " not found" << std::endl;
  //   }
  if(!checkDirectories 
     && !vm.count("runDivide") 
     && !vm.count("savePUTreeWeight") 
     && !vm.count("saveCorrEleTree") 
     && !vm.count("saveR9TreeWeight") 
     && !vm.count("saveRootMacro") 
     && !vm.count("EOverPCalib") 
     && !vm.count("momentumCorrectionEE") 
     && !vm.count("momentumCorrectionEB")
     && !vm.count("laserMonitoringEP")
     && !vm.count("laserMonitoringEPvsPU")
     ) return 1;

  if(!dataPUFileName.empty()) dataPUFileNameVec.push_back(dataPUFileName.c_str());
  if(!mcPUFileName.empty()) mcPUFileNameVec.push_back(mcPUFileName.c_str());
  //============================== Reading the config file with the list of chains
  tag_chain_map_t tagChainMap;
  TString tag, chainName, fileName;
  
  TString chainFileListTag=chainFileListName; 
  chainFileListTag.Remove(0,chainFileListTag.Last('/')+1);
  chainFileListTag.ReplaceAll(".dat","");

  TString regionsFileNameTag=regionsFileName;
  regionsFileNameTag.Remove(0,regionsFileNameTag.Last('/')+1);
  regionsFileNameTag.ReplaceAll(".dat","");
  
  std::ifstream chainFileList(chainFileListName.c_str());
  while(chainFileList >> tag, chainFileList.good()){
    if(tag.Contains('#')){
      chainFileList.ignore(1000,'\n');
      continue;
    }
    chainFileList >> chainName >> fileName;
    if(chainName.Contains("Hist")){
      // use these value only if not provided by the command line
      if(tag.Contains("d") && dataPUFileName.empty()) dataPUFileNameVec.push_back(fileName);
      else if(tag.Contains("s") && mcPUFileName.empty()) mcPUFileNameVec.push_back(fileName);
      else std::cerr << "[ERROR] in configuration file Hist not recognized" << std::endl;
      continue;
    }

    // discard file with energy corrections different from the specified type
    if(chainName.Contains("scaleEle")){
      if(chainName!="scaleEle_"+corrEleType) continue;
    }

    // discard file with energy smearings different from the specified type
    if(chainName.Contains("smearEle")){
      if(chainName!="smearEle_"+smearEleType) continue;
    }

    // discard file with categories for "smearingMethod" different from the region file name
    if(chainName.Contains("smearerCat")){
      if(chainName!="smearerCat_"+regionsFileNameTag) continue;
    }
    
    if(!tagChainMap.count(tag)){
#ifdef DEBUG
      std::cout << "[DEBUG] Create new tag map for tag: " << tag << std::endl;
#endif
      std::pair<TString, chain_map_t > pair_tmp(tag,chain_map_t()); // make_pair not work with scram b
      tagChainMap.insert(pair_tmp);
    }
    
    if(!tagChainMap[tag].count(chainName)){
      std::pair<TString, TChain* > pair_tmp(chainName, new TChain(chainName));
      (tagChainMap[tag]).insert(pair_tmp);
      (tagChainMap[tag])[chainName]->SetTitle(tag);
    }
    std::cout <<"Adding file: " << tag << "\t" << chainName << "\t" << fileName<< std::endl;
    if((tagChainMap[tag])[chainName]->Add(fileName,-1)==0) exit(1);
#ifdef DEBUG
    std::cout << "[DEBUG] " << tag << "\t" << chainName << "\t" << fileName << "\t" << (tagChainMap[tag])[chainName]->GetEntries() << std::endl;
#endif

  }

  
  //init chains and print 
  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
#ifdef DEBUG
    std::cout << tag_chain_itr->first << std::endl;
#endif
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){
      //#ifdef DEBUG
      std::cout << " - " << chain_itr->first << "\t" << chain_itr->second->GetName() << "\t" << chain_itr->second->GetTitle() << "\t" << chain_itr->second->GetEntries() << std::endl;
      //#endif
      chain_itr->second->GetEntries();
    }
  }


  ///------------------------------ to obtain run ranges
  if(vm.count("runDivide")){
    runDivide_class runDivider;
    std::vector<TString> v=runDivider.Divide((tagChainMap["d"])["selected"], "data/runRanges/runRangeLimits.dat", nEvents_runDivide);
    runDivider.PrintRunRangeEvents();
    std::vector<TString> runRanges;
    if(runRangesFileName!="") runRanges = ReadRegionsFromFile(runRangesFileName);
    for(std::vector<TString>::const_iterator itr = runRanges.begin();
	itr != runRanges.end();
	itr++){
      std::cout << *itr << "\t" << "-1" << "\t" << runDivider.GetRunRangeTime(*itr) << std::endl;
    }

    return 0;
  }



  TString r(regionsFileName.c_str());
  r.Remove(0,r.Last('/')+1); r.ReplaceAll(".dat","");

  std::vector<TString> regions = ReadRegionsFromFile(regionsFileName);
  std::vector<TString> runRanges = ReadRegionsFromFile(runRangesFileName);
  std::vector<TString> categories;
  for(std::vector<TString>::const_iterator region_itr = regions.begin();
      region_itr != regions.end();
      region_itr++){
    if(runRanges.size()>0){
      for(std::vector<TString>::const_iterator runRange_itr = runRanges.begin();
	  runRange_itr!=runRanges.end();
	  runRange_itr++){
	TString token1,token2; 
  //Ssiz_t ss=0;
	//runRange_itr->Tokenize(token1,ss,"-");
	//ss=runRange_itr->First('-');
	//runRange_itr->Tokenize(token2,ss,"-");
	TObjArray *tx = runRange_itr->Tokenize("-");
	token1 = ((TObjString *)(tx->At(0)))->String();
	token2 = ((TObjString *)(tx->At(1)))->String();
	categories.push_back((*region_itr)+"-runNumber_"+token1+"_"+token2+"-"+commonCut.c_str());
      }
    }else categories.push_back((*region_itr)+"-"+commonCut.c_str());
  }

  

  ///------------------------------ to obtain r9weights
  if(vm.count("saveR9TreeWeight") && !vm.count("r9WeightFile")){
    std::cerr << "[ERROR] No r9WeightFile specified" << std::endl;
    return 1;
  }
  if(vm.count("r9WeightFile")){
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "[STATUS] Getting r9Weights from file: " << r9WeightFile << std::endl;
    r9Weights_class r9Weights;
    r9Weights.ReadFromFile(r9WeightFile);

    TString treeName="r9Weight";

    // mc // save it in a file and reload it as a chain to be safe against the reference directory for the tree
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("d")==0 || tag_chain_itr->first.CompareTo("s")==0) continue; 
      if(tag_chain_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
     
      TString filename="tmp/r9Weight_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      std::cout << "[STATUS] Saving r9Weights tree to root file:" << filename << std::endl;
      
      TFile f(filename,"recreate");
      if(!f.IsOpen() || f.IsZombie()){
	std::cerr << "[ERROR] File for r9Weights: " << filename << " not opened" << std::endl;
	exit(1);
      }
      TTree *corrTree = r9Weights.GetTreeWeight(ch);
      f.cd();
      corrTree->Write();
      std::cout << "[INFO] Data      entries: " << ch->GetEntries() << std::endl;
      std::cout << "       r9Weights entries: " << corrTree->GetEntries() << std::endl;
      delete corrTree;
	
      f.Write();
      f.Close();
      std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
      chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
      chain_itr->second->SetTitle(tag_chain_itr->first);
      chain_itr->second->Add(filename);
    
    } // end of data samples loop
  } // end of r9Weight 
    

  if(vm.count("saveR9TreeWeight")) return 0;

  ///------------------------------ to obtain ZPt weights
  if(vm.count("ZPtWeightFile")){
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "[STATUS] Getting ZPtWeights from file: " << ZPtWeightFile << std::endl;
    UpdateFriends(tagChainMap, regionsFileNameTag);
    ZPtWeights_class ZPtWeights;
    ZPtWeights.ReadFromFile(ZPtWeightFile);

    TString treeName="ZPtWeight";

    // mc // save it in a file and reload it as a chain to be safe against the reference directory for the tree
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.Contains("d")) continue; /// \todo ZPtWeight only on MC! because from PdfWeights, to make it more general
      if(tag_chain_itr->first.CompareTo("d")==0 || tag_chain_itr->first.CompareTo("s")==0) continue; 
      if(tag_chain_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
     
      TString filename="tmp/ZPtWeight_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      std::cout << "[STATUS] Saving r9Weights tree to root file:" << filename << std::endl;
      
      TFile f(filename,"recreate");
      if(!f.IsOpen() || f.IsZombie()){
	std::cerr << "[ERROR] File for ZPtWeights: " << filename << " not opened" << std::endl;
	exit(1);
      }
      TTree *corrTree = ZPtWeights.GetTreeWeight(ch, "ZPt_"+energyBranchName);
      f.cd();
      corrTree->Write();
      std::cout << "[INFO] Data      entries: " << ch->GetEntries() << std::endl;
      std::cout << "       ZPtWeights entries: " << corrTree->GetEntries() << std::endl;
      delete corrTree;
	
      f.Write();
      f.Close();
      std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
      chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
      chain_itr->second->SetTitle(tag_chain_itr->first);
      chain_itr->second->Add(filename);
    
    } // end of data samples loop
  } // end of r9Weight 

    
  //==============================

  //  if(vm.count("dataPU")==0 && (tagChainMap["s"]).count("pileupHist")==0 && (tagChainMap["s"]).count("pileup")==0){

  if(vm.count("noPU")==0 && !vm.count("runToy") && !vm.count("correctionMomentum") && !vm.count("laserMonitoringEP")  && !vm.count("laserMonitoringEPvsPU") && 
     !vm.count("EOverPCalib")){
    if(dataPUFileNameVec.empty() && (tagChainMap.count("s")!=0) && (tagChainMap["s"]).count("pileup")==0){
      std::cerr << "[ERROR] Nor pileup mc tree configured in chain list file either dataPU histograms are not provided" << std::endl;
      return 1;
    }else if( !vm.count("runToy") && (vm.count("dataPU")!=0 || (!dataPUFileNameVec.empty() && ((tagChainMap.count("s")==0) || (tagChainMap["s"]).count("pileup")==0)))){
      std::cout << "[STATUS] Creating pileup weighting tree and saving it" << std::endl;
      for(unsigned int i=0; i < mcPUFileNameVec.size(); i++){
	TString mcPUFileName_=mcPUFileNameVec[i];
	TString dataPUFileName_=dataPUFileNameVec[i];
 	TString runMin_ = "";
	if(!mcPUFileName_.Contains("nPU")){
// 	if(mcPUFileName_.Index(".runMin_")!=-1){
 	  runMin_=mcPUFileName_;
 	  runMin_.Remove(0,runMin_.Last('/')+1);
 	  runMin_.Remove(runMin_.First('-'));
// 	  runMin_.Remove(runMin_.First('.'));
// 	  runMin_.ReplaceAll("runMin_","");
 	}
	int runMin = runMin_.Sizeof()>1 ? runMin_.Atoi() : 1;
	std::cout << "********* runMin = " << runMin << "\t" << runMin_ << std::endl;
	puWeights.ReadFromFiles(mcPUFileName_.Data(),dataPUFileName_.Data(), runMin);
      }

      // for each mc sample create a tree with the per-event-weight
      // but exclude the chain "s" since it's supposed to be created mergin alle the s-type samples
      for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	  tag_chain_itr!=tagChainMap.end();
	  tag_chain_itr++){
	if(tag_chain_itr->first.CompareTo("s")==0 || !tag_chain_itr->first.Contains("s")) continue;
	TChain *ch = (tag_chain_itr->second.find("selected"))->second;
	if((tag_chain_itr->second.count("pileup"))) continue;
	TString treeName="pileup";
	TString filename="tmp/mcPUtree"+tag_chain_itr->first+".root";
	TFile f(filename,"recreate");
	if(f.IsOpen()){
	  f.cd();

	  TTree *puTree = puWeights.GetTreeWeight(ch,true);
	  puTree->SetName(treeName);
	  puTree->Write();
	  delete puTree;
	  f.Write();
	  f.Close();
	  std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	  chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
 	  chain_itr->second->SetTitle(tag_chain_itr->first);
	  chain_itr->second->Add(filename);
	}
      }
    }
  }

  //read corrections directly from file
  if (vm.count("corrEleType") && corrEleFile!=""){
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "[STATUS] Getting energy scale corrections from file: " << corrEleFile << std::endl;
    TString treeName="scaleEle_"+corrEleType;
    EnergyScaleCorrection_class eScaler(corrEleFile);

    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("d")==0 || !tag_chain_itr->first.Contains("d")) continue; //only data
      if(tag_chain_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
     
      TString filename="tmp/scaleEle_"+corrEleType+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      std::cout << "[STATUS] Saving electron scale corrections to root file:" << filename << std::endl;

	TFile f(filename,"recreate");
	if(!f.IsOpen() || f.IsZombie()){
	  std::cerr << "[ERROR] File for scale corrections: " << filename << " not opened" << std::endl;
	  exit(1);
	}
	TTree *corrTree = eScaler.GetCorrTree(ch, "runNumber", "R9Eleprime");
	corrTree->SetName(TString("scaleEle_")+corrEleType.c_str());
	corrTree->SetTitle(corrEleType.c_str());
	f.cd();
	corrTree->Write();
	std::cout << "[INFO] Data entries: "    << ch->GetEntries() << std::endl;
	std::cout << "       corrEle entries: " << corrTree->GetEntries() << std::endl;
	delete corrTree;
	
	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
	chain_itr->second->SetTitle(tag_chain_itr->first);
	chain_itr->second->Add(filename);
    
    } // end of data samples loop
  } // end of corrEle loop


  //read corrections directly from file
  if (vm.count("smearEleType")){
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "[STATUS] Getting energy smearings from file: " << smearEleFile << std::endl;
    TString treeName="smearEle_"+smearEleType;
    EnergyScaleCorrection_class eScaler("", smearEleFile);
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("s")==0 || !tag_chain_itr->first.Contains("s")) continue; //only data
      if(tag_chain_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
 
      TString filename="tmp/smearEle_"+smearEleType+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      std::cout << "[STATUS] Saving electron smearings to root file:" << filename << std::endl;
      
      TFile f(filename,"recreate");
      if(!f.IsOpen() || f.IsZombie()){
	std::cerr << "[ERROR] File for scale corrections: " << filename << " not opened" << std::endl;
	exit(1);
      }
      if(TString(smearEleType).Contains("CB")){
	eScaler.SetSmearingType(1);
	eScaler.SetSmearingCBAlpha(smearingCBAlpha);
      }
      
      TTree *corrTree = eScaler.GetSmearTree(ch, true, energyBranchName );
      f.cd();
      corrTree->SetName(TString("smearEle_")+smearEleType.c_str());
      corrTree->SetTitle(smearEleType.c_str());
      corrTree->Write();
      std::cout << "[INFO] Data entries: "    << ch->GetEntries() << std::endl;
      std::cout << "       smearEle entries: " << corrTree->GetEntries() << std::endl;
      delete corrTree;

      f.Write();
      f.Close();
      std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
      chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
      chain_itr->second->SetTitle(tag_chain_itr->first);
      chain_itr->second->Add(filename);
    }
    // \todo need the data part in case of needs
  }


  addBranch_class newBrancher;
  newBrancher._commonCut=commonCut.c_str();
  newBrancher._regionList=categories;

  for( std::vector<string>::const_iterator branch_itr = branchList.begin();
       branch_itr != branchList.end();
       branch_itr++){
    UpdateFriends(tagChainMap, regionsFileNameTag);
    
    TString treeName=*branch_itr;
    TString t;
    if(treeName=="smearerCat_s"){
      treeName.ReplaceAll("_s","");
      t="s";
    } 
    if(treeName=="smearerCat_d"){
      treeName.ReplaceAll("_d","");
      t="d";
    }
    TString branchName=treeName;
    std::cout << "#### --> " << treeName << "\t" << t << "\t" << *branch_itr <<std::endl;
    if(branchName=="smearerCat") treeName+="_"+regionsFileNameTag;

    if(treeName.Contains("invMassSigma")){
      newBrancher.scaler= new EnergyScaleCorrection_class("", smearEleFile);
    }

    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if((tag_chain_itr->first.CompareTo("s")==0 || tag_chain_itr->first.CompareTo("d")==0)) continue; //only data
      if(tag_chain_itr->second.count(treeName)!=0) continue; //skip if already present
      if(t!="" && !tag_chain_itr->first.Contains(t)) continue;
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;

      //data
      std::cout <<"[STATUS] Adding branch " << branchName << " to " << tag_chain_itr->first <<std::endl;
      TString filename="tmp/"+treeName+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";

      TTree *newTree = newBrancher.AddBranch(ch,treeName, branchName,true,tag_chain_itr->first.Contains("s"));
      if(newTree==NULL){
	std::cerr << "[ERROR] New tree for branch " << treeName << " is NULL" << std::endl;
	return 1;
      }

      TFile f(filename,"recreate");
      if (!f.IsOpen()){
	std::cerr << "[ERROR] File for branch " << branchName << " not created" << std::endl;
	return 1;
      }
      f.cd();
      newTree->SetTitle(tag_chain_itr->first);
      newTree->Write();
      delete newTree;
      //f.Write();
      f.Close();
      std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
      chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
      chain_itr->second->SetTitle(tag_chain_itr->first);
      chain_itr->second->Add(filename);
    } //end of sample loop
  } //end of branches loop

 

  //(tagChainMap["s"])["selected"]->GetEntries();
  UpdateFriends(tagChainMap, regionsFileNameTag);

  //create tag "s" if not present (due to multiple mc samples)
  if(!tagChainMap.count("s")){
    //#ifdef DEBUG
    std::cout << "==============================" << std::endl;
    std::cout << "==============================" << std::endl;
    MergeSamples(tagChainMap, regionsFileNameTag, "s");
    MergeSamples(tagChainMap, regionsFileNameTag, "d");
  }
  
//   Dump(tagChainMap, "s",0);
//   Dump(tagChainMap, "s",(tagChainMap["s1"])["selected"]->GetEntries());
//   Dump(tagChainMap, "s",(tagChainMap["s2"])["selected"]->GetEntries());
//   exit(0);
  if(vm.count("saveRootMacro")){
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	  chain_itr!=tag_chain_itr->second.end();
	  chain_itr++){
	chain_itr->second->SaveAs("tmp/"+tag_chain_itr->first+"_"+chain_itr->first+"_chain.root");
	//chain_itr->second->SavePrimitive(std::cout); // not implemented
      }
    }
  }

  if(
     vm.count("saveRootMacro") 
     || vm.count("saveCorrEleTree")
     || vm.count("saveAddBranchTree")
     || vm.count("savePUTreeWeight")
     || vm.count("runDivide") 
     || vm.count("saveCorrEleTree") 
     || vm.count("saveR9TreeWeight") 
     ) return 0;

  eleID+=selection.c_str();
  eleID.ReplaceAll("_","");

  //------------------------------ RooSmearer
  RooArgSet args;
  std::vector<RooArgSet> args_vec;

  for(std::vector<TString>::const_iterator region_itr = categories.begin();
      region_itr != categories.end();
      region_itr++){
    RooRealVar *scale_ = new RooRealVar("scale_"+*region_itr, "scale_"+*region_itr,1.0, 0.95,1.05,"GeV"); //0.9,1.1,"GeV")
    scale_->setError(0.005); // 1%
    //    scale_->setConstant();
    //    scale_->setBinning(RooBinning(
    args.add(*scale_);

    TString varName=*region_itr;
    TPRegexp reg("Et_[0-9]*_[0-9]*");
    reg.Substitute(varName,"");
    TPRegexp reg2("energySC_[0-9]*_[0-9]*");
    reg2.Substitute(varName,"");
    varName.ReplaceAll("--","-");
    if(varName.First("-")==0) varName.Remove(0,1);

    //RooRealVar *const_term_ = new RooRealVar("constTerm_"+*region_itr, "constTerm_"+*region_itr, 0.01, 0.0005, 0.05);
    RooAbsReal *const_term_=NULL;
    RooRealVar *const_term_v = args.getSize() ==0 ? NULL : (RooRealVar *) args.find("constTerm_"+varName);
    if(const_term_v==NULL){
      if(vm.count("constTermFix")==0) const_term_v = new RooRealVar("constTerm_"+*region_itr, "constTerm_"+varName,0.00, 0.000,0.05); 
      else const_term_v = new RooRealVar("constTerm_"+varName, "constTerm_"+varName,0.00, 0.000,0.02);
      const_term_v->setError(0.03); // 1%
      //const_term_v->setConstant(true);
      args.add(*const_term_v);
    } 
    if((reg.MatchB(*region_itr) || reg2.MatchB(*region_itr) )&& vm.count("constTermFix")==1){
      const_term_ = new RooFormulaVar("constTerm_"+*region_itr, "constTerm_"+varName,"@0", *const_term_v);
      const_term_v->setConstant(false);
    } else const_term_ = const_term_v;



    RooAbsReal *alpha_=NULL;
    RooRealVar *alpha_v = args.getSize() ==0 ? NULL : (RooRealVar *) args.find("alpha_"+varName);
    if(alpha_v==NULL){
      alpha_v = new RooRealVar("alpha_"+varName, "alpha_"+varName,0.0, 0.,0.20);
      alpha_v->setError(0.01);
      alpha_v->setConstant(true);
      //alpha_v->Print();
      if(!vm.count("alphaGoldFix") || !region_itr->Contains("absEta_1_1.4442-gold")){
	args.add(*alpha_v);
      }
    } 
    if(reg.MatchB(*region_itr) && vm.count("constTermFix")==1){
      if(vm.count("alphaGoldFix") && region_itr->Contains("absEta_1_1.4442-gold")){
	std::cout << "[STATUS] Fixing alpha term to low eta region " << *region_itr << std::endl;
	std::cerr << "[STATUS] Fixing alpha term to low eta region " << *region_itr << std::endl;
	TString lowRegionVarName=varName; lowRegionVarName.ReplaceAll("absEta_1_1.4442","absEta_0_1");
	alpha_v = (RooRealVar *)args.find("alpha_"+lowRegionVarName);
	alpha_ = new RooFormulaVar("alpha_"+*region_itr, "alpha_"+lowRegionVarName,"@0", *alpha_v);
      } else {
	alpha_ = new RooFormulaVar("alpha_"+*region_itr, "alpha_"+varName,"@0", *alpha_v);
      }
      alpha_v->setConstant(false);
    } else alpha_ = alpha_v;
    
    args_vec.push_back(RooArgSet(*scale_, *alpha_, *const_term_));
  }

  if(vm.count("onlyScale")){
    TIterator *it1=NULL; 
    it1 = args.createIterator();
    for(RooRealVar *var = (RooRealVar *) it1->Next(); var!=NULL;
	var = (RooRealVar *) it1->Next()){
      TString name(var->GetName());
      if(name.Contains("scale")) continue;
      var->setConstant(true);
    }
  }

  args.sort(kFALSE);
  if(vm.count("smearerFit")){
    std::cout << "------------------------------ smearer parameters" << std::endl;
    args.writeToStream(std::cout, kFALSE);
  }

  TRandom3 g(0);
  Long64_t randomInt=g.Integer(1000000);
  TString filename="tmp/tmpFile-"; filename+=randomInt;filename+=".root";
  TFile *tmpFile = new TFile(filename,"recreate");
  tmpFile->cd();
  RooSmearer smearer("smearer",(tagChainMap["d"])["selected"], (tagChainMap["s"])["selected"], NULL, 
		     categories,
		     args_vec, args, energyBranchName);
  smearer._isDataSmeared=vm.count("isDataSmeared");
  if(vm.count("runToy")) smearer.SetPuWeight(false);
  smearer.SetOnlyDiagonal(vm.count("onlyDiagonal"));
  smearer._autoBin=vm.count("autoBin");
  smearer._autoNsmear=vm.count("autoNsmear");
  smearer.smearscan=vm.count("smearscan");
  //smearer.nEventsMinDiag = nEventsMinDiag;
  smearer._deactive_minEventsOffDiag = nEventsMinOffDiag;
  smearer.SetSmearingEt(vm.count("smearingEt"));
  smearer.SetR9Weight(vm.count("useR9weight"));
  smearer.SetPdfSystWeight(pdfSystWeightIndex);
  smearer.SetZPtWeight(vm.count("useZPtweight"));
  smearer.SetFsrWeight(vm.count("useFSRweight"));
  smearer.SetWeakWeight(vm.count("useWEAKweight"));

  if(nSmearToy>0) smearer._nSmearToy = nSmearToy;


  //------------------------------ Take the list of branches needed for the defined categories
  ElectronCategory_class cutter;
  cutter.energyBranchName=energyBranchName;
  std::set<TString> activeBranchList;
  for(std::vector<TString>::const_iterator region_itr = categories.begin();
      region_itr != categories.end();
      region_itr++){
    std::set<TString> tmpList = cutter.GetBranchNameNtuple(*region_itr);
    activeBranchList.insert(tmpList.begin(),tmpList.end());
    // add also the friend branches!
  }

  if(vm.count("loop")){
//     TFile *file = new TFile("evList.root","read");

//     Loop((tagChainMap["s1"])["selected"],file);
    return 0;
  }
  //------------------------------ ZFit_class declare and set the options
  TChain *data = NULL;
  TChain *mc = NULL;
  if(!vm.count("smearerFit")){
    data= (tagChainMap["d"])["selected"];
    mc  = (tagChainMap["s"])["selected"];
  }


    
//------------------------------ LASER MONITORING WITH E/P  ------------------------------------------------------
 
  if(vm.count("laserMonitoringEP")) {	  

        float timeLapse = 24.; // in hours
    //    int t1 = 1267401600;   //  1 Mar 2010
    //int t2 = 1325289600;   // 31 Dec 2011 
    //int  t1 = 1400000000;
    //int  t2 = 1600000000;

	// float yMIN = 0.65;
	//float yMAX = 1.10;


  // Set style options
  setTDRStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptStat(1110); 
  gStyle->SetOptFit(1); 
  
  // Set fitting options
  TVirtualFitter::SetDefaultFitter("Fumili2");
  
  
  
  //-----------------
  // Input parameters
 
  
  std::cout << "\n***************************************************************************************************************************" << std::endl;
 
  //  std::string dayMin = "";
  //std::string dayMax = "";
  std::string dayMinLabel = "";
  std::string dayMaxLabel = "";
  std::string dayZOOMLabel ="";
  float absEtaMin = -1.;
  float absEtaMax = -1.;
  int IetaMin = -1;
  int IetaMax = -1;
  int IphiMin = -1;
  int IphiMax = -1;

  


  int t1 = dateToInt(dayMin);
  int t2 = dateToInt(dayMax);
  int t3 = dateToInt(dayZOOM);
  
  /*
  if(argc >= 5)
  {
    dayMin = std::string(argv[4])+" "+std::string(argv[5])+" "+std::string(argv[6]);
    dayMax = std::string(argv[7])+" "+std::string(argv[8])+" "+std::string(argv[9]);
    dayMinLabel = std::string(argv[4])+"_"+std::string(argv[5])+"_"+std::string(argv[6]);
    dayMaxLabel = std::string(argv[7])+"_"+std::string(argv[8])+"_"+std::string(argv[9]);
    
  }
  if(argc >= 11)
  {
    yMIN = atof(argv[10]);
    yMAX = atof(argv[11]);
  }
  if(argc >= 13)
  {
    absEtaMin = atof(argv[12]);
    absEtaMax = atof(argv[13]);
  }
  if(argc >= 15)
  {
    IetaMin = atoi(argv[14]);
    IetaMax = atoi(argv[15]);
    IphiMin = atoi(argv[16]);
    IphiMax = atoi(argv[17]);
  }
  */
  
  std::cout << "EBEE: "          << EBEE          << std::endl;
  std::cout << "evtsPerPoint: "  << evtsPerPoint  << std::endl;
  std::cout << "useRegression: " << useRegression << std::endl;
  std::cout << "dayMin: "        << dayMin        << std::endl;
  std::cout << "dayZOOM: "       << dayZOOM       << std::endl;
  std::cout << "dayMax: "        << dayMax        << std::endl;
  std::cout << "yMin: "          << yMIN          << std::endl;
  std::cout << "yMax: "          << yMAX          << std::endl;
  std::cout << "absEtaMin: "     << absEtaMin     << std::endl;
  std::cout << "absEtaMax: "     << absEtaMax     << std::endl;
  std::cout << "IetaMin: "       << IetaMin       << std::endl;
  std::cout << "IetaMax: "       << IetaMax       << std::endl;
  std::cout << "IphiMin: "       << IphiMin       << std::endl;
  std::cout << "IphiMax: "       << IphiMax       << std::endl;
  std::cout << "t1: "            << t1            << std::endl;
  std::cout << "t2: "            << t2            << std::endl;
  std::cout << "t3"              << t3            << std::endl;
  
  std::string dayZOOM = "";
  std::string dayMin = "";
  std::string dayMax = "";
  
   
  //-------------------
  // Define in/outfiles
  
  std::string folderName = std::string(EBEE) + "_" + dayMinLabel + "_" + dayMaxLabel;
  if( (absEtaMin != -1.) && (absEtaMax != -1.) )
  {
    char absEtaBuffer[50];
    sprintf(absEtaBuffer,"_%.2f-%.2f",absEtaMin,absEtaMax);
    folderName += std::string(absEtaBuffer);
  } 
  
  if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) )
  {
    char absEtaBuffer[50];
    sprintf(absEtaBuffer,"_Ieta_%d-%d_Iphi_%d-%d",IetaMin,IetaMax,IphiMin,IphiMax);
    folderName += std::string(absEtaBuffer);
  } 
  
  gSystem->mkdir(folderName.c_str());
  TFile* o = new TFile((folderName+"/"+folderName+"_histos.root").c_str(),"RECREATE");
  
  
  
  // Get trees
  std::cout << std::endl;
 
  /* 
  TChain* ntu_DA = new TChain("simpleNtupleEoverP/SimpleNtupleEoverP");
  FillChain(ntu_DA,"inputDATA.txt");
  std::cout << "     DATA: " << std::setw(8) << ntu_DA->GetEntries() << " entries" << std::endl;
  
  TChain* ntu_MC = new TChain("simpleNtupleEoverP/SimpleNtupleEoverP");
  FillChain(ntu_MC,"inputMC.txt");
  std::cout << "REFERENCE: " << std::setw(8) << ntu_MC->GetEntries() << " entries" << std::endl;
  */  

  if (data->GetEntries() == 0 || mc->GetEntries() == 0 )
  {
    std::cout << "Error: At least one file is empty" << std::endl; 
    return -1;
  }
  
  
  
  // Set branch addresses
  int runNumber;
  int runTime;
  int nPU;
  float avgLCSCEle[3],  etaSCEle[3], phiSCEle[3], energySCEle[3], esEnergySCEle[3], pAtVtxGsfEle[3], energySCEle_corr[3];
  int seedXSCEle[3], seedYSCEle[3];//, seedZside;
  //  float seedLaserAlphaSCEle1;

  data->SetBranchStatus("*",0);
  data->SetBranchStatus("runNumber",1);  
  data->SetBranchStatus("runTime",1);
  data->SetBranchStatus("nPU",1);
  data->SetBranchStatus("avgLCSCEle",1);
  //  data->SetBranchStatus("seedLaserAlphaSCEle1",1);
  //  data->SetBranchStatus("ele1_EOverP",1);
  data->SetBranchStatus("etaSCEle",1);
  data->SetBranchStatus("phiSCEle",1);
  data->SetBranchStatus("energySCEle_must",1);
  data->SetBranchStatus("energySCEle_corr",1);
  data->SetBranchStatus("esEnergySCEle",1);
  data->SetBranchStatus("pAtVtxGsfEle",1);
  data->SetBranchStatus("seedXSCEle",1);
  data->SetBranchStatus("seedYSCEle",1);
  //  data->SetBranchStatus("ele1_seedZside",1);
    
  data->SetBranchAddress("runNumber", &runNumber);  
  data->SetBranchAddress("runTime", &runTime);
  data->SetBranchAddress("nPU", &nPU);
  data->SetBranchAddress("avgLCSCEle", &avgLCSCEle[0]);
  //data->SetBranchAddress("seedLaserAlphaSCEle1", &seedLaserAlphaSCEle1);
  //  data->SetBranchAddress("ele1_EOverP", &EoP);
  data->SetBranchAddress("etaSCEle", &etaSCEle);
  data->SetBranchAddress("phiSCEle", &phiSCEle);
  if( useRegression < 1 )
    data->SetBranchAddress("energySCEle_must", &energySCEle);
  else
    data->SetBranchAddress("energySCEle_corr", &energySCEle_corr);
  //  data->SetBranchAddress("energySCEle_corr", &energySCEle);
  data->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  data->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  data->SetBranchAddress("seedXSCEle", &seedXSCEle);
  data->SetBranchAddress("seedYSCEle", &seedYSCEle);
  //  data->SetBranchAddress("ele1_seedZside", &seedZside);


  mc->SetBranchStatus("*",0);
  mc->SetBranchStatus("runNumber",1);  
  mc->SetBranchStatus("runTime",1);
  mc->SetBranchStatus("nPU",1);
  mc->SetBranchStatus("avgLCSCEle",1);
  //  mc->SetBranchStatus("seedLaserAlphaSCEle1",1);
  //  mc->SetBranchStatus("ele1_EOverP",1);
  mc->SetBranchStatus("etaSCEle",1);
  mc->SetBranchStatus("phiSCEle",1);
  mc->SetBranchStatus("energySCEle_must",1);
  mc->SetBranchStatus("energySCEle_corr",1);
  mc->SetBranchStatus("esEnergySCEle",1);
  mc->SetBranchStatus("pAtVtxGsfEle",1);
  mc->SetBranchStatus("seedXSCEle",1);
  mc->SetBranchStatus("seedYSCEle",1);
  //  mc->SetBranchStatus("ele1_seedZside",1);
    
  mc->SetBranchAddress("runNumber", &runNumber);  
  mc->SetBranchAddress("runTime", &runTime);
  mc->SetBranchAddress("nPU", &nPU);
  mc->SetBranchAddress("avgLCSCEle", &avgLCSCEle[0]);
  //mc->SetBranchAddress("seedLaserAlphaSCEle1", &seedLaserAlphaSCEle1);
  //  mc->SetBranchAddress("ele1_EOverP", &EoP);
  mc->SetBranchAddress("etaSCEle", &etaSCEle);
  mc->SetBranchAddress("phiSCEle", &phiSCEle);
  if( useRegression < 1 )
    mc->SetBranchAddress("energySCEle_must", &energySCEle);
  else
    mc->SetBranchAddress("energySCEle_corr", &energySCEle_corr);
  //  mc->SetBranchAddress("energySCEle_corr", &energySCEle);
  mc->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  mc->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  mc->SetBranchAddress("seedXSCEle", &seedXSCEle);
  mc->SetBranchAddress("seedYSCEle", &seedYSCEle);
  //  mc->SetBranchAddress("ele1_seedZside", &seedZside);
  
  
  
  
  
  
  //--------------------------------------------------------
  // Define PU correction (to be used if useRegression == 0)
  
  // corr = p0 + p1 * nPU
  float p0_EB;
  float p1_EB;
  float p0_EE;
  float p1_EE;
  
  if( useRegression == 0 )
  {
    //2012 EB
    p0_EB = 0.9991;
    p1_EB = 0.0001635;
    //2012 EE
    p0_EE = 0.9968;
    p1_EE = 0.001046;
  }
  else
  {
    //2012 EB
    p0_EB = 1.001;
    p1_EB = -0.000143;
    //2012 EE
    p0_EE = 1.00327;
    p1_EE = -0.000432;
  }
  
  float p0 = -1.;
  float p1 = -1.;
  
  if( strcmp(EBEE.c_str(),"EB") == 0 )
  {
    p0 = p0_EB;
    p1 = p1_EB;
  }
  else
  {
    p0 = p0_EE;
    p1 = p1_EE;
  }
  
  //2015
  p0=1.;
  p1=0.;
  
  
  
  
  //---------------------------------
  // Build the reference distribution
  
  std::cout << std::endl;
  std::cout << "***** Build reference for " << EBEE << " *****" << std::endl;
  
  TH1F* h_template = new TH1F("template", "", 2000, 0., 5.);
  
  for(int ientry = 0; ientry < mc->GetEntries(); ++ientry)
  {
    if( (ientry%10000 == 0) ) std::cout << "reading MC entry " << ientry <<"/"<<mc->GetEntries()<< "\r" << std::endl;//std::flush;
    mc->GetEntry(ientry);
    
    // selections
    if( (strcmp(EBEE.c_str(),"EB") == 0) && (fabs(etaSCEle[0]) > 1.479) )                    continue; // barrel
    if( (strcmp(EBEE.c_str(),"EB_0_1") == 0) && (fabs(etaSCEle[0]) > 1.0)) continue; // inner barrel
    if( (strcmp(EBEE.c_str(),"EB_1_1479") == 0) && (fabs(etaSCEle[0]) < 1.0 || fabs(etaSCEle[0]) > 1.479) )                     continue; // barrel
    if( (strcmp(EBEE.c_str(),"EE") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_1479_2") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_2_25") == 0) && (fabs(etaSCEle[0]) < 2.0 || fabs(etaSCEle[0])>2.5) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEp") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_1479_2") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_25") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_225") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_225_25") == 0) && (etaSCEle[0] < 2.25 || etaSCEle[0]>2.25) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEm") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_1479_2") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_25") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_225") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_225_25") == 0) && (etaSCEle[0] > -2.25 || etaSCEle[0]<-2.5) ) continue; // endcap

    if( (absEtaMin != -1.) && (absEtaMax != -1.) )
    {
      if( (fabs(etaSCEle[0]) < absEtaMin) || (fabs(etaSCEle[0]) > absEtaMax) ) continue;
    }
    
    if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) )
    {
      if( (seedXSCEle[0] < IetaMin) || (seedXSCEle[0] > IetaMax) ) continue;
      if( (seedYSCEle[0] < IphiMin) || (seedYSCEle[0] > IphiMax) ) continue;
    }
    
    // PU correction
    float PUCorr = (p0 + p1*nPU);
    //std::cout << "p0: " << p0  << "   p1: " << p1 << "   nPU: " << nPU << std::endl;
    
    // fill the template histogram
    h_template -> Fill( (energySCEle[0]-esEnergySCEle[0])/(pAtVtxGsfEle[0]-esEnergySCEle[0]) / (avgLCSCEle[0]/pow(avgLCSCEle[0],0.9)) / PUCorr );
  }
  
  std::cout << "Reference built for " << EBEE << " - " << h_template->GetEntries() << " events" << std::endl;
  
  
  
  
  
  
  //---------------------
  // Loop and sort events
  
  std::cout << std::endl;
  std::cout << "***** Sort events and define bins *****" << std::endl;
  
  int nEntries = data -> GetEntriesFast(); 
  int nSavePts = 0; 
  std::vector<bool> isSavedEntries(nEntries);
  std::vector<Sorter> sortedEntries;
  std::vector<int> timeStampFirst;

  for(int ientry = 0; ientry < nEntries; ++ientry)
  {
    data -> GetEntry(ientry);
    isSavedEntries.at(ientry) = false;

    if( (ientry%10000 == 0) ) std::cout << "reading data entry " << ientry << "\r" << std::endl;//std::flush;
    
    // selections
    if( (strcmp(EBEE.c_str(),"EB") == 0) && (fabs(etaSCEle[0]) > 1.479) )                    continue; // barrel
    if( (strcmp(EBEE.c_str(),"EB_0_1") == 0) && (fabs(etaSCEle[0]) > 1.0)) continue; // inner barrel
    if( (strcmp(EBEE.c_str(),"EB_1_1479") == 0) && (fabs(etaSCEle[0]) < 1.0 || fabs(etaSCEle[0]) > 1.479) )                     continue; // barrel
    if( (strcmp(EBEE.c_str(),"EE") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_1479_2") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_2_25") == 0) && (fabs(etaSCEle[0]) < 2.0 || fabs(etaSCEle[0])>2.5) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEp") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_1479_2") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_25") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_225") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_225_25") == 0) && (etaSCEle[0] < 2.25 || etaSCEle[0]>2.25) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEm") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_1479_2") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_25") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_225") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_225_25") == 0) && (etaSCEle[0] > -2.25 || etaSCEle[0]<-2.5) ) continue; // endcap
    
    if( (absEtaMin != -1.) && (absEtaMax != -1.) )
    {
      if( (fabs(etaSCEle[0]) < absEtaMin) || (fabs(etaSCEle[0]) > absEtaMax) ) continue;
    }
    
    if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) )
    {
      if( (seedXSCEle[0] < IetaMin) || (seedXSCEle[0] > IetaMax) ) continue;
      if( (seedYSCEle[0] < IphiMin) || (seedYSCEle[0] > IphiMax) ) continue;
    }
    
    if( runTime < t1 ) continue;
    if( runTime > t2 ) continue;
    
    if( avgLCSCEle[0] <= 0. ) continue;
    
    isSavedEntries.at(ientry) = true;
    
    
    // fill sorter
    Sorter dummy;
    dummy.time = runTime;
    dummy.entry = ientry;
    sortedEntries.push_back(dummy);
    
    ++nSavePts;
  }
  
  // sort events
  std::sort(sortedEntries.begin(),sortedEntries.end(),Sorter());
  std::cout << "Data sorted in " << EBEE << " - " << nSavePts << " events" << std::endl;
  
  std::map<int,int> antiMap;
  for(unsigned int iSaved = 0; iSaved < sortedEntries.size(); ++iSaved)
    antiMap[sortedEntries.at(iSaved).entry] = iSaved;
  
  
  //---------------------
  // Loop and define bins
  
  // "wide" bins - find events with time separation bigger than 1 day
  int nWideBins = 1;
  std::vector<int> wideBinEntryMax;
  int timeStampOld = -1;

  //  TEventList* evlist=new TEventList("events");
  //  data->Draw(">>events","","goff");
  //  TEventList* evlist = (TEventList*) gDirectory->Get("events");
    
  wideBinEntryMax.push_back(0);  
  for(int iSaved = 0; iSaved < nSavePts; ++iSaved)
  {
    if( iSaved%10000 == 0 ) std::cout << "reading saved entry " << iSaved << "\r" << std::endl;//std::flush;
    data->GetEntry(sortedEntries[iSaved].entry);  
    //    data->GetEntry(evlist->GetEntry(sortedEntries[iSaved].entry));
    
    if( iSaved == 0 )
    {
      timeStampOld = runTime;
      continue;
    }
    
    if( (runTime-timeStampOld)/3600. > timeLapse )
    {
      ++nWideBins;
      wideBinEntryMax.push_back(iSaved-1);
    }
    
    timeStampOld = runTime;
  }
  std::cout << std::endl;
  wideBinEntryMax.push_back(nSavePts);
  
  
  // bins with approximatively evtsPerPoint events per bin
    int nBins = 0;
  std::vector<int> binEntryMax;
  
  binEntryMax.push_back(0);
  for(int wideBin = 0; wideBin < nWideBins; ++wideBin)
  {
    int nTempBins = std::max(1,int( (wideBinEntryMax.at(wideBin+1)-wideBinEntryMax.at(wideBin))/evtsPerPoint ));
    int nTempBinEntries = int( (wideBinEntryMax.at(wideBin+1)-wideBinEntryMax.at(wideBin))/nTempBins );
    
    for(int tempBin = 0; tempBin < nTempBins; ++tempBin)
    {
      ++nBins;
      if( tempBin < nTempBins - 1 )
        binEntryMax.push_back( wideBinEntryMax.at(wideBin) + (tempBin+1)*nTempBinEntries );
      else
        binEntryMax.push_back( wideBinEntryMax.at(wideBin+1) );
    }
  }
  
  std::cout << "nBins = " << nBins << std::endl;
  //for(int bin = 0; bin < nBins; ++bin)
  //  std::cout << "bin: " << bin
  //            << "   entry min: " << setw(6) << binEntryMax.at(bin)
  //            << "   entry max: " << setw(6) << binEntryMax.at(bin+1)
  //            << "   events: "    << setw(6) << binEntryMax.at(bin+1)-binEntryMax.at(bin)
  //            << std::endl;
  
  
  
  
  
  
  //---------------------
  // histogram definition
  
  TH1F* h_scOccupancy_eta  = new TH1F("h_scOccupancy_eta","", 298, -2.6, 2.6);
  TH1F* h_scOccupancy_phi  = new TH1F("h_scOccupancy_phi","", 363, -3.1765, 3.159);
  SetHistoStyle(h_scOccupancy_eta);
  SetHistoStyle(h_scOccupancy_phi);
  
  TH2F* h_seedOccupancy_EB  = new TH2F("h_seedOccupancy_EB","",  171, -86., 85., 361,   0.,361.);
  TH2F* h_seedOccupancy_EEp = new TH2F("h_seedOccupancy_EEp","", 101,   0.,101., 100,   0.,101.);
  TH2F* h_seedOccupancy_EEm = new TH2F("h_seedOccupancy_EEm","", 101,   0.,101., 100,   0.,101.);
  SetHistoStyle(h_seedOccupancy_EB);
  SetHistoStyle(h_seedOccupancy_EEp);
  SetHistoStyle(h_seedOccupancy_EEm);
  
  TH1F* h_EoP_spread = new TH1F("h_EoP_spread","",100,yMIN,yMAX);
  TH1F* h_EoC_spread = new TH1F("h_EoC_spread","",100,yMIN,yMAX);
  TH1F* h_EoP_spread_run = new TH1F("h_EoP_spread_run","",100,yMIN,yMAX);
  TH1F* h_EoC_spread_run = new TH1F("h_EoC_spread_run","",100,yMIN,yMAX);
  SetHistoStyle(h_EoP_spread,"EoP");
  SetHistoStyle(h_EoC_spread,"EoC");
  SetHistoStyle(h_EoP_spread_run,"EoP");
  SetHistoStyle(h_EoC_spread_run,"EoC");
  
  TH1F* h_EoP_chi2 = new TH1F("h_EoP_chi2","",50,0.,5.);
  TH1F* h_EoC_chi2 = new TH1F("h_EoC_chi2","",50,0.,5.);
  SetHistoStyle(h_EoP_chi2,"EoP");
  SetHistoStyle(h_EoC_chi2,"EoC");  
  
  TH1F** h_EoP = new TH1F*[nBins];
  TH1F** h_EoC = new TH1F*[nBins];
  TH1F** h_Las = new TH1F*[nBins];
  TH1F** h_Tsp = new TH1F*[nBins];
  TH1F** h_Cvl = new TH1F*[nBins];
  
  for(int i = 0; i < nBins; ++i)
  {
    char histoName[80];
    
    sprintf(histoName, "EoP_%d", i);
    h_EoP[i] = new TH1F(histoName, histoName, 2000, 0., 5.);
    SetHistoStyle(h_EoP[i],"EoP");
    
    sprintf(histoName, "EoC_%d", i);
    h_EoC[i] = new TH1F(histoName, histoName, 2000, 0., 5.);
    SetHistoStyle(h_EoC[i],"EoC");
    
    sprintf(histoName, "Las_%d", i);
    h_Las[i] = new TH1F(histoName, histoName, 500, 0.5, 1.5);
    
    sprintf(histoName, "Tsp_%d", i);
    h_Tsp[i] = new TH1F(histoName, histoName, 500, 0.5, 1.5);
   
  }
  
  
  // function definition
    TF1** f_EoP = new TF1*[nBins];
    TF1** f_EoC = new TF1*[nBins];
  
  
  // graphs definition
  TGraphAsymmErrors* g_fit   = new TGraphAsymmErrors();
  TGraphAsymmErrors* g_c_fit = new TGraphAsymmErrors();
  
  TGraphAsymmErrors* g_fit_run   = new TGraphAsymmErrors();
  TGraphAsymmErrors* g_c_fit_run = new TGraphAsymmErrors();  
  
  TGraph* g_las = new TGraph();
  
  TGraphErrors* g_LT = new TGraphErrors();
  
  g_fit->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_fit->GetXaxis()->SetTimeDisplay(1);
  g_c_fit->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_c_fit->GetXaxis()->SetTimeDisplay(1);
  g_las->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_las->GetXaxis()->SetTimeDisplay(1);
  g_LT->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_LT->GetXaxis()->SetTimeDisplay(1);
    
  
  
  
  
  
  //------------------------------------
  // loop on the saved and sorted events
  
  std::cout << std::endl;
  std::cout << "***** Fill and fit histograms *****" << std::endl;

  std::vector<int> Entries(nBins);  
  std::vector<double> AveTime(nBins);
  std::vector<int> MinTime(nBins);
  std::vector<int> MaxTime(nBins);
  std::vector<double> AveRun(nBins);    
  std::vector<int> MinRun(nBins);
  std::vector<int> MaxRun(nBins);
  std::vector<double> AveLT(nBins);
  std::vector<double> AveLT2(nBins);
    
  int iSaved = -1;
  for(int ientry = 0; ientry < nEntries; ++ientry)
  {
    if( (ientry%100000 == 0) ) std::cout << "reading entry " << ientry << "\r" << std::endl;//std::flush;
    
    if( isSavedEntries.at(ientry) == false ) continue;
    
    ++iSaved;
    
    int iSaved = antiMap[ientry];
    int bin = -1;
    for(bin = 0; bin < nBins; ++bin)
      if( iSaved >= binEntryMax.at(bin) && iSaved < binEntryMax.at(bin+1) )
	break;
    
    //std::cout << "bin = " << bin << "   iSaved = "<< iSaved << std::endl;
    data->GetEntry(ientry);
    
    
    
    Entries[bin] += 1;
    
    if( iSaved == binEntryMax.at(bin)+1 )   MinTime[bin] = runTime;
    if( iSaved == binEntryMax.at(bin+1)-1 ) MaxTime[bin] = runTime;
    AveTime[bin] += runTime;
    
    if( iSaved == binEntryMax.at(bin)+1 )   MinRun[bin] = runNumber;
    if( iSaved == binEntryMax.at(bin+1)-1 ) MaxRun[bin] = runNumber;
    AveRun[bin] += runNumber;
    
    //    float LT = (-1. / seedLaserAlphaSCEle1 * log(avgLCSCEle[0]));
    float LT = 1.;
    AveLT[bin] += LT;
    AveLT2[bin] += LT*LT;
    
    // PU correction
    float PUCorr = (p0 + p1*nPU);

    // fill the histograms
    (h_EoP[bin]) -> Fill( (energySCEle[0]-esEnergySCEle[0])/(pAtVtxGsfEle[0]-esEnergySCEle[0]) / pow(avgLCSCEle[0],0.9) / PUCorr);
    (h_EoC[bin]) -> Fill( (energySCEle[0]-esEnergySCEle[0])/(pAtVtxGsfEle[0]-esEnergySCEle[0]) / (avgLCSCEle[0]/pow(avgLCSCEle[0],0.9)) / PUCorr );
    
			  (h_Las[bin]) -> Fill(pow(avgLCSCEle[0],0.9));
			  (h_Tsp[bin]) -> Fill(1./pow(avgLCSCEle[0],0.9));
    
    h_scOccupancy_eta -> Fill(etaSCEle[0]);
    h_scOccupancy_phi -> Fill(phiSCEle[0]);
    if(fabs(etaSCEle[0])<1.449)
      h_seedOccupancy_EB -> Fill(seedXSCEle[0],seedYSCEle[0]);
    else if(etaSCEle[0]>1.449)
      h_seedOccupancy_EEp -> Fill(seedXSCEle[0],seedYSCEle[0]);
    else if(etaSCEle[0]<-1.449)
      h_seedOccupancy_EEm -> Fill(seedXSCEle[0],seedYSCEle[0]);
  }
  
  for(int bin = 0; bin < nBins; ++bin)
  {
    AveTime[bin] = 1. * AveTime[bin] / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    AveRun[bin]  = 1. * AveRun[bin]  / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    AveLT[bin]   = 1. * AveLT[bin]   / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    AveLT2[bin]  = 1. * AveLT2[bin]  / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    //std::cout << date << " " << AveTime[i] << " " << MinTime[i] << " " << MaxTime[i] << std::endl;
  }
  
  
  
  
  
  
  int rebin = 2;
  if( strcmp(EBEE.c_str(),"EE") == 0 ) rebin *= 2;
  
  h_template -> Rebin(rebin);
  
  
  
  float EoP_scale = 0.;
  float EoP_err = 0.;
  int   EoP_nActiveBins = 0;
  
  float EoC_scale = 0.;
  float EoC_err = 0.;
  int   EoC_nActiveBins = 0;
  
  float LCInv_scale = 0;
  
  std::vector<int> validBins;
  for(int i = 0; i < nBins; ++i)
  {
    bool isValid = true;
    
    h_EoP[i] -> Rebin(rebin);
    h_EoC[i] -> Rebin(rebin);
    
    
    
    //------------------------------------
    // Fill the graph for uncorrected data
    
    // define the fitting function
    // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
    
    //o -> cd();
    char convolutionName[50];
    sprintf(convolutionName,"h_convolution_%d",i);
    //h_Cvl[i] = ConvoluteTemplate(std::string(convolutionName),h_template,h_Las[i],32768,-5.,5.);
    h_Cvl[i] = MellinConvolution(std::string(convolutionName),h_template,h_Tsp[i]);
    
    histoFunc* templateHistoFunc = new histoFunc(h_template);
    histoFunc* templateConvolutedHistoFunc = new histoFunc(h_Cvl[i]);
    char funcName[50];

    sprintf(funcName,"f_EoP_%d",i);
    
    if( strcmp(EBEE.c_str(),"EB") == 0 )
      f_EoP[i] = new TF1(funcName, templateConvolutedHistoFunc, 0.8*(h_Tsp[i]->GetMean()), 1.4*(h_Tsp[i]->GetMean()), 3, "histoFunc");
    else
      f_EoP[i] = new TF1(funcName, templateConvolutedHistoFunc, 0.75*(h_Tsp[i]->GetMean()), 1.5*(h_Tsp[i]->GetMean()), 3, "histoFunc");
    
    f_EoP[i] -> SetParName(0,"Norm"); 
    f_EoP[i] -> SetParName(1,"Scale factor"); 
    f_EoP[i] -> SetLineWidth(1); 
    f_EoP[i] -> SetNpx(10000);
    
    double xNorm = h_EoP[i]->GetEntries()/h_template->GetEntries() *
      h_EoP[i]->GetBinWidth(1)/h_template->GetBinWidth(1); 
    
    f_EoP[i] -> FixParameter(0, xNorm);
    f_EoP[i] -> SetParameter(1, 1.);
    f_EoP[i] -> FixParameter(2, 0.);
    f_EoP[i] -> SetLineColor(kRed+2); 
    
    int fStatus = 0;
    int nTrials = 0;
    TFitResultPtr rp;
   
    rp = h_EoP[i] -> Fit(funcName, "ERLS+");
    while( (fStatus != 0) && (nTrials < 10) )
    {
      rp = h_EoP[i] -> Fit(funcName, "ERLS+");
      fStatus = rp;
      if(fStatus == 0) break;
      ++nTrials;
      }
    
    // fill the graph
    double eee = f_EoP[i]->GetParError(1);
    //float k    = f_EoP[i]->GetParameter(1);
    float k    = f_EoP[i]->GetParameter(1) / h_Tsp[i]->GetMean(); //needed when using mellin's convolution 
    
    /*
    std::cout << i <<"--nocorr---- "<< 1./k << std::endl;
    std::cout <<" condizione 1: " << h_EoP[i]->GetEntries() << "  fStatus: " << fStatus << " eee: " << eee << "con eee che ci piace essere maggiore di : " <<  0.05*h_template->GetRMS()/sqrt(evtsPerPoint) << std::endl ;
    getchar();
    */
    
    
    //    if( (h_EoP[i]->GetEntries() > 3) && (fStatus == 0) && (eee > 0.05*h_template->GetRMS()/sqrt(evtsPerPoint)) )
    if( (h_EoP[i]->GetEntries() > 500) && (fStatus == 0) )
    {
      float date = (float)AveTime[i];
      float dLow = (float)(AveTime[i]-MinTime[i]); 
      float dHig = (float)(MaxTime[i]-AveTime[i]);
      float run = (float)AveRun[i];
      float rLow = (float)(AveRun[i]-MinRun[i]); 
      float rHig = (float)(MaxRun[i]-AveRun[i]);
      
      g_fit -> SetPoint(i,  date , 1./k);
      g_fit -> SetPointError(i, dLow , dHig, eee/k/k, eee/k/k);
      
      g_fit_run -> SetPoint(i,  run , 1./k);
      g_fit_run -> SetPointError(i, rLow , rHig, eee/k/k, eee/k/k);
      
      std::cout <<"************-------------------*****************" <<std::endl;
      

      h_EoP_chi2 -> Fill(f_EoP[i]->GetChisquare()/f_EoP[i]->GetNDF());
      
      EoP_scale += 1./k;
      EoP_err += eee/k/k;
      ++EoP_nActiveBins;
    }
    else
    {
      std::cout << "Fitting uncorrected time bin: " << i << "   Fail status: " << fStatus << "   sigma: " << eee << std::endl;
      isValid = false;
    }  
    
    //----------------------------------
    // Fill the graph for corrected data
    
    // define the fitting function
    // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )

    sprintf(funcName,"f_EoC_%d",i);
    if( strcmp(EBEE.c_str(),"EB") == 0 )
      f_EoC[i] = new TF1(funcName, templateHistoFunc, 0.8, 1.4, 3, "histoFunc");
    else
      f_EoC[i] = new TF1(funcName, templateHistoFunc, 0.75, 1.5, 3, "histoFunc");
    f_EoC[i] -> SetParName(0,"Norm"); 
    f_EoC[i] -> SetParName(1,"Scale factor"); 
    f_EoC[i] -> SetLineWidth(1); 
    f_EoC[i] -> SetNpx(10000);
    
    xNorm = h_EoC[i]->GetEntries()/h_template->GetEntries() *
            h_EoC[i]->GetBinWidth(1)/h_template->GetBinWidth(1); 

    f_EoC[i] -> FixParameter(0, xNorm);
    f_EoC[i] -> SetParameter(1, 0.99);
    f_EoC[i] -> FixParameter(2, 0.);
    f_EoC[i] -> SetLineColor(kGreen+2); 
    
    
    rp = h_EoC[i] -> Fit(funcName, "ERLS+");
    fStatus = rp;
    nTrials = 0;
    while( (fStatus != 0) && (nTrials < 10) )
      {
	rp = h_EoC[i] -> Fit(funcName, "ERLS+");
	fStatus = rp;
	if(fStatus == 0) break;
	++nTrials;
      }

    
    // fill the graph
    k   = f_EoC[i]->GetParameter(1);
    eee = f_EoC[i]->GetParError(1); 
    
    /* std::cout << i <<"--corr---- "<< 1./k << std::endl;
    std::cout <<" condizione 1: " << h_EoP[i]->GetEntries() << "  fStatus: " << fStatus << " eee: " << eee << "con eee che ci piace essere maggiore di : " <<  0.05*h_template->GetRMS()/sqrt(evtsPerPoint) << std::endl ;
    getchar();
    */  


    if( (h_EoC[i]->GetEntries() > 500) && (fStatus == 0) )
    //  if( (h_EoC[i]->GetEntries() > 10) && (fStatus == 0) && (eee > 0.05*h_template->GetRMS()/sqrt(evtsPerPoint)) )
      {
      float date = (float)AveTime[i]; 
      float dLow = (float)(AveTime[i]-MinTime[i]); 
      float dHig = (float)(MaxTime[i]-AveTime[i]);
      float run = (float)AveRun[i];
      float rLow = (float)(AveRun[i]-MinRun[i]); 
      float rHig = (float)(MaxRun[i]-AveRun[i]);
      
      g_c_fit -> SetPoint(i,  date , 1./k);
      g_c_fit -> SetPointError(i, dLow , dHig , eee/k/k, eee/k/k);
      
      g_c_fit_run -> SetPoint(i,  run , 1./k);
      g_c_fit_run -> SetPointError(i, rLow , rHig, eee/k/k, eee/k/k);
      std::cout <<"************-------------------*****************" <<std::endl;
            

      h_EoC_chi2 -> Fill(f_EoC[i]->GetChisquare()/f_EoP[i]->GetNDF());
      
      EoC_scale += 1./k;
      EoC_err += eee/k/k;
      ++EoC_nActiveBins;
    }
    else
    {
      std::cout << "Fitting corrected time bin: " << i << "   Fail status: " << fStatus << "   sigma: " << eee << std::endl;
      isValid = false;
    }
    
    if( isValid == true ) validBins.push_back(i);
  }
  
  EoP_scale /= EoP_nActiveBins;
  EoP_err   /= EoP_nActiveBins;
  
  EoC_scale /= EoC_nActiveBins;
  EoC_err   /= EoC_nActiveBins;
  
  
  
  
  
  
  //----------------------------------------
  // Fill the graph for avg laser correction
  
  //fede
  for(unsigned int itr = 0; itr < validBins.size(); ++itr)
  {  
    int i = validBins.at(itr);
    g_las -> SetPoint(itr, (float)AveTime[i], h_Tsp[i]->GetMean());
    g_LT  -> SetPoint(itr, (float)AveTime[i], AveLT[i] );
    g_LT  -> SetPointError(itr, 0., sqrt(AveLT2[i]-AveLT[i]*AveLT[i]) / sqrt(Entries[i]) );
        
    LCInv_scale += h_Tsp[i]->GetMean();
  }  
  
  LCInv_scale /= validBins.size();
  
  
  
  
  
  
  //---------------
  // Rescale graphs
  
  float yscale = 1.;
  //float yscale = 1./EoC_scale;
  
  for(unsigned int itr = 0; itr < validBins.size(); ++itr)
  {
    double x,y; 
    g_fit -> GetPoint(itr,x,y); 
    g_fit -> SetPoint(itr,x,y*yscale);
    if ( (x > t1) && (x < t2) ) h_EoP_spread -> Fill(y*yscale);
    
    g_c_fit -> GetPoint(itr,x,y); 
    g_c_fit -> SetPoint(itr,x,y*yscale);
    if ( (x > t1) && (x < t2) ) h_EoC_spread -> Fill(y*yscale);
    
    g_fit_run -> GetPoint(itr,x,y); 
    g_fit_run -> SetPoint(itr,x,y*yscale); 
    if ( (x > t1) && (x < t2) ) h_EoP_spread_run -> Fill(y*yscale);
    
    g_c_fit_run -> GetPoint(itr,x,y); 
    g_c_fit_run -> SetPoint(itr,x,y*yscale);
    if ( (x > t1) && (x < t2) ) h_EoC_spread_run -> Fill(y*yscale);
    
    g_las -> GetPoint(itr,x,y);
    g_las -> SetPoint(itr,x,y*yscale*EoP_scale/LCInv_scale);
  }
  TF1 EoC_pol0("EoC_pol0","pol0",t1,t2);
  EoC_pol0.SetLineColor(kGreen+2);
  EoC_pol0.SetLineWidth(2);
  EoC_pol0.SetLineStyle(2);
  g_c_fit -> Fit("EoC_pol0","QNR");
  
  
  
  
  
  
  
  
  //----------------------------
  // Print out global quantities
  
  std::cout << std::endl;
  std::cout << "***** Mean scales and errors *****" << std::endl; 
  std::cout << std::fixed;
  std::cout << std::setprecision(4);
  std::cout << "Mean EoP scale: "  << std::setw(6) << EoP_scale   << "   mean EoP error: " << std::setw(8) << EoP_err << std::endl;
  std::cout << "Mean EoC scale: "  << std::setw(6) << EoC_scale   << "   mean EoC error: " << std::setw(8) << EoC_err << std::endl;
  std::cout << "Mean 1/LC scale: " << std::setw(6) << LCInv_scale << std::endl;
  
  
  
  
  
  
  //-----------------
  // Occupancy plots
  //-----------------
  
  TCanvas* c_scOccupancy = new TCanvas("c_scOccupancy","SC occupancy",0,0,1000,500);
  c_scOccupancy -> Divide(2,1);
  
  c_scOccupancy -> cd(1);
  h_scOccupancy_eta -> GetXaxis() -> SetTitle("sc #eta");
  h_scOccupancy_eta -> GetYaxis() -> SetTitle("events");
  h_scOccupancy_eta -> Draw();
  
  c_scOccupancy -> cd(2);
  h_scOccupancy_phi -> GetXaxis() -> SetTitle("sc #phi");
  h_scOccupancy_phi -> GetYaxis() -> SetTitle("events");
  h_scOccupancy_phi -> Draw();
  
  TCanvas* c_seedOccupancy = new TCanvas("c_seedOccupancy","seed occupancy",0,0,1500,500);
  c_seedOccupancy -> Divide(3,1);
  
  c_seedOccupancy -> cd(1);
  h_seedOccupancy_EB -> GetXaxis() -> SetTitle("seed i#eta");
  h_seedOccupancy_EB -> GetYaxis() -> SetTitle("seed i#phi");
  h_seedOccupancy_EB -> Draw("COLZ");
  
  c_seedOccupancy -> cd(2);
  h_seedOccupancy_EEp -> GetXaxis() -> SetTitle("seed ix");
  h_seedOccupancy_EEp -> GetYaxis() -> SetTitle("seed iy");
  h_seedOccupancy_EEp -> Draw("COLZ");
  
  c_seedOccupancy -> cd(3);
  h_seedOccupancy_EEm -> GetXaxis() -> SetTitle("seed ix");
  h_seedOccupancy_EEm -> GetYaxis() -> SetTitle("seed iy");
  h_seedOccupancy_EEm -> Draw("COLZ");
  
  
  
  //-----------
  // Chi2 plots
  //-----------
  
  TCanvas* c_chi2 = new TCanvas("c_chi2","fit chi2",0,0,500,500);
  c_chi2 -> cd();
  
  h_EoC_chi2 -> GetXaxis() -> SetTitle("#chi^{2}/N_{dof}");
  h_EoC_chi2 -> Draw("");
  gPad -> Update();
    
  TPaveStats* s_EoC = new TPaveStats;
  s_EoC = (TPaveStats*)(h_EoC_chi2->GetListOfFunctions()->FindObject("stats"));
  s_EoC -> SetStatFormat("1.4g");
  s_EoC -> SetTextColor(kGreen+2);
  s_EoC->SetY1NDC(0.59);
  s_EoC->SetY2NDC(0.79);
  s_EoC -> Draw("sames");
  gPad -> Update();
  
  h_EoP_chi2 -> GetXaxis() -> SetTitle("#chi^{2}/N_{dof}");
  h_EoP_chi2 -> Draw("sames");
  gPad -> Update();
  
  TPaveStats* s_EoP = new TPaveStats;
  s_EoP = (TPaveStats*)(h_EoP_chi2->GetListOfFunctions()->FindObject("stats"));
  s_EoP -> SetStatFormat("1.4g");
  s_EoP -> SetTextColor(kRed+2);
  s_EoP->SetY1NDC(0.79);
  s_EoP->SetY2NDC(0.99);
  s_EoP -> Draw("sames");
  gPad -> Update();
  
  //ciao
  //-------------------
  // RMS vs Num evts -BARREL
  //-------------------
  double x[13]={2.,4.,6.,8.,10.,12.,14.,16.,18.,20.,22.,24.,30.};
  double y[13]={0.001049,0.001114,0.0009367,0.0008480,0.0007669,0.0007892,0.0006699,0.0006473,0.0006235,0.0005903,0.0005815,0.0005459,0.0005506};
  
  TCanvas* RMSeb = new TCanvas("plot", "plot",0,0,500,500);
  TGraph* gRMSeb = new TGraph(13,x,y);
  
  gRMSeb->Draw("APC");
  gRMSeb -> SetMarkerColor(38);
  gRMSeb -> SetLineColor(38);
  gRMSeb->GetXaxis()->SetTitle("Number of Events - Barrel"); 
  gRMSeb->GetYaxis()->SetTitle("RMS"); 
  
  RMSeb -> Print((folderName+"/"+folderName+"_RMSeb"+".png").c_str(),"png");
  RMSeb -> Print((folderName+"/"+folderName+"_RMSeb"+".pdf").c_str(),"pdf");
  
  //-------------------
  // RMS vs Num evts -ENDCAP
  //-------------------
  
  double xx[11]={2.,4.,6.,8.,10.,12.,14.,16.,18.,20.,22.};
  double yy[11]={0.007234,0.005759,0.004174,0.004255,0.003833,0.004037,0.003912,0.004251,0.003598,0.004067,0.004138};
  
  TCanvas* RMSee = new TCanvas("plot", "plot",0,0,500,500);
  TGraph* gRMSee = new TGraph(11,xx,yy);
  
  gRMSee->Draw("APC");
  gRMSee -> SetMarkerColor(38);
  gRMSee -> SetLineColor(38);
  gRMSee->GetXaxis()->SetTitle("Number of Events - Endcap"); 
  gRMSee->GetYaxis()->SetTitle("RMS"); 
    
  
  RMSee -> Print((folderName+"/"+folderName+"_RMSee"+".png").c_str(),"png");
  RMSee -> Print((folderName+"/"+folderName+"_RMSee"+".pdf").c_str(),"pdf");
  
  
  //ciao
  //-------------------
  // histos 
  //-------------------
 
  
  /*      
  for ( int i = 0; i < nBins; ++i)
    {
      
      TCanvas* histoEoP = new TCanvas("histo","histo",0,0,500,500);
      histoEoP -> cd();
      
      h_EoP[i] -> Draw();
      f_EoP[i] -> SetLineWidth(2);
      f_EoP[i] -> SetLineColor(4);
      f_EoP[i] -> Draw("same");	
      // histoEoP -> Update();
      
      histoEoP -> Print((folderName+"/"+folderName+"_histoEoP"+std::to_string(i)+".png").c_str(),"png");
      histoEoP -> Print((folderName+"/"+folderName+"_histoEoP"+std::to_string(i)+".pdf").c_str(),"pdf");
      
      
      TCanvas* histoEoC = new TCanvas("histo","histo",0,0,500,500);
      histoEoC -> cd();
      
      h_EoC[i] -> Draw();
      f_EoC[i] -> SetLineWidth(2);
      f_EoC[i] -> SetLineColor(4);
      f_EoC[i] -> Draw("same");
      //histoEoC -> Update();
      
      histoEoC -> Print((folderName+"/"+folderName+"_histoEoC"+to_string(i)+".png").c_str(),"png");    
      histoEoC -> Print((folderName+"/"+folderName+"_histoEoC"+to_string(i)+".pdf").c_str(),"pdf");    
      
    }
  */  

  //-------------------
  // Final Plot vs date
  //-------------------
  
  TCanvas* cplot = new TCanvas("cplot", "history plot vs date",100,100,1000,500);
  cplot->cd();

  TPad *cLeft  = new TPad("pad_0","pad_0",0.00,0.00,0.75,1.00);
  TPad *cRight = new TPad("pad_1","pad_1",0.75,0.00,1.00,1.00);

  cLeft->SetLeftMargin(0.15); 
  cLeft->SetRightMargin(0.025); 
  cRight->SetLeftMargin(0.025); 

  cLeft->Draw();
  cRight->Draw();

  float tYoffset = 1.0; 
  float labSize = 0.05;
  float labSize2 = 0.06;

  cLeft->cd(); 
  
  cLeft->SetGridx();
  cLeft->SetGridy();
  
  TH1F *hPad = (TH1F*)gPad->DrawFrame(t1,0.9,t2,1.05);
    
  //hPad->GetXaxis()->SetLimits(t3,t2);
  hPad->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  hPad->GetXaxis()->SetTimeDisplay(1);
  hPad->GetXaxis() -> SetRangeUser(MinTime[0]-43200,MaxTime[nBins-1]+43200);
  hPad->GetXaxis()->SetTitle("date (day/month)");
  //ciao
  //hPad->GetXaxis()->SetLabelSize(0.025);
  
  if( strcmp(EBEE.c_str(),"EB") == 0 )
    hPad->GetYaxis()->SetTitle("Relative E/p scale"); 
  else
    hPad->GetYaxis()->SetTitle("Relative E/p scale"); 
  hPad->GetYaxis()->SetTitleOffset(tYoffset);
  hPad->GetXaxis()->SetTitleOffset(0.8);
  hPad->GetXaxis()->SetLabelSize(0.03);
  hPad->GetXaxis()->SetTitleSize(labSize2);
  hPad->GetYaxis()->SetLabelSize(labSize);
  hPad->GetYaxis()->SetTitleSize(labSize2);
  hPad -> SetMinimum(yMIN);
  hPad -> SetMaximum(yMAX);
  
  // draw history plot
  g_fit -> SetMarkerStyle(24);
  g_fit -> SetMarkerSize(0.7);
  g_fit -> SetMarkerColor(kRed+2);
  g_fit -> SetLineColor(kRed+2);
  g_fit -> Draw("P");
  g_c_fit -> SetMarkerStyle(20);
  g_c_fit -> SetMarkerColor(kGreen+2);
  g_c_fit -> SetLineColor(kGreen+2);
  g_c_fit -> SetMarkerSize(0.7);
  g_c_fit -> Draw("EP,same");
  g_las -> SetLineColor(kAzure-2);
  g_las -> SetLineWidth(2);
  //  g_las -> Draw("L,same");
  
  TLegend* legend = new TLegend(0.60,0.78,0.90,0.94);
  legend -> SetLineColor(kWhite);
  legend -> SetLineWidth(0);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(0);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.04);
  legend -> AddEntry(g_c_fit,"with LM correction","PL");
  legend -> AddEntry(g_fit,  "without LM correction","PL");
  //  legend -> AddEntry(g_las,  "1 / LM correction","L");
  legend -> Draw("same");
  
  char latexBuffer[250];
  
  sprintf(latexBuffer,"CMS 2016 Preliminary");
  TLatex* latex = new TLatex(0.18,0.89,latexBuffer);  
  latex -> SetNDC();
  latex -> SetTextFont(62);
  latex -> SetTextSize(0.05);
  latex -> Draw("same");
  
  //sprintf(latexBuffer,"#sqrt{s} = 8 TeV   L = 3.95 fb^{-1}");
  sprintf(latexBuffer,"#sqrt{s} = 13 TeV, L =%s fb^{-1} ", LUMI.c_str());

  // sprintf(latexBuffer, LUMI.c_str());
  TLatex* latex2 = new TLatex(0.18,0.84,latexBuffer);  
  latex2 -> SetNDC();
  latex2 -> SetTextFont(42);
  latex2 -> SetTextSize(0.05);
  latex2 -> Draw("same");
  
  if( strcmp(EBEE.c_str(),"EB") == 0 || strcmp(EBEE.c_str(),"EB_0_1") == 0 || strcmp(EBEE.c_str(),"EB_1_1479") == 0)
    sprintf(latexBuffer,"ECAL Barrel");
  else
    sprintf(latexBuffer,"ECAL Endcap");
  TLatex* latex3 = new TLatex(0.18,0.19,latexBuffer);
  latex3 -> SetNDC();
  latex3 -> SetTextFont(42);
  latex3 -> SetTextSize(0.05);
  latex3 -> Draw("same");
  
  //sprintf(latexBuffer,"%.2E events",1.*nSavePts);
  //TLatex* latex4 = new TLatex(0.18,0.24,latexBuffer);  
  //latex4 -> SetNDC();
  //latex4 -> SetTextFont(42);
  //latex4 -> SetTextSize(0.04);
  //latex4 -> Draw("same");
  //
  //sprintf(latexBuffer,"%d events/bin - %d bins",evtsPerPoint,nBins);
  //TLatex* latex5 = new TLatex(0.18,0.19,latexBuffer);  
  //latex5 -> SetNDC();
  //latex5 -> SetTextFont(42);
  //latex5 -> SetTextSize(0.04);
  //latex5 -> Draw("same");
  
  
  cRight -> cd();
  
  TPaveStats* s_EoP_spread = new TPaveStats();
  TPaveStats* s_EoC_spread = new TPaveStats();
  
  
  h_EoC_spread -> SetFillStyle(3001);
  h_EoC_spread -> SetFillColor(kGreen+2);
  h_EoC_spread->GetYaxis()->SetLabelSize(0.09);
  h_EoC_spread->GetYaxis()->SetLabelOffset(-0.03);
  h_EoC_spread->GetYaxis()->SetTitleSize(0.08);
  h_EoC_spread->GetYaxis()->SetNdivisions(505);
  h_EoC_spread->GetXaxis()->SetLabelOffset(1000);
  
  h_EoC_spread -> Draw("hbar");
  gPad -> Update();
  
  s_EoC_spread = (TPaveStats*)(h_EoC_spread->GetListOfFunctions()->FindObject("stats"));
  s_EoC_spread -> SetStatFormat("1.4g");
  s_EoC_spread->SetX1NDC(0.06); //new x start position
  s_EoC_spread->SetX2NDC(0.71); //new x end position
  s_EoC_spread->SetY1NDC(0.43); //new x start position
  s_EoC_spread->SetY2NDC(0.34); //new x end position
  s_EoC_spread -> SetOptStat(1100);
  s_EoC_spread ->SetTextColor(kGreen+2);
  s_EoC_spread ->SetTextSize(0.08);
  s_EoC_spread -> Draw("sames");
  
  
  h_EoP_spread -> SetFillStyle(3001);
  h_EoP_spread -> SetFillColor(kRed+2);
  h_EoP_spread -> Draw("hbarsames");
  gPad -> Update();
  s_EoP_spread = (TPaveStats*)(h_EoP_spread->GetListOfFunctions()->FindObject("stats"));
  s_EoP_spread -> SetStatFormat("1.4g");
  s_EoP_spread->SetX1NDC(0.06); //new x start position
  s_EoP_spread->SetX2NDC(0.71); //new x end position
  s_EoP_spread->SetY1NDC(0.33); //new x start position
  s_EoP_spread->SetY2NDC(0.24); //new x end position
  s_EoP_spread ->SetOptStat(1100);
  s_EoP_spread ->SetTextColor(kRed+2);
  s_EoP_spread ->SetTextSize(0.08);
  s_EoP_spread -> Draw("sames");
  
  /*
  h_EoP_spread -> SetFillStyle(3001);
  h_EoP_spread -> SetFillColor(kRed+2);
  h_EoP_spread -> Draw("hbarsame");
  gPad -> Update();
  */
  
  
  
  //------------------
  // Final plot vs run
  //------------------
    
  TCanvas* cplot_run = new TCanvas("cplot_run", "history plot vs run",100,100,1000,500);
  cplot_run->cd();
  
  cLeft  = new TPad("pad_0_run","pad_0_run",0.00,0.00,0.75,1.00);
  cRight = new TPad("pad_1_run","pad_1_run",0.75,0.00,1.00,1.00);

  cLeft->SetLeftMargin(0.15); 
  cLeft->SetRightMargin(0.025); 
  cRight->SetLeftMargin(0.025); 

  cLeft->Draw();
  cRight->Draw();

  tYoffset = 1.5; 
  labSize = 0.04;
  labSize2 = 0.07;

  cLeft->cd(); 

  cLeft->SetGridx();
  cLeft->SetGridy();
  
  hPad = (TH1F*)gPad->DrawFrame(MinRun[0]-1000,0.9,MaxRun[nBins-1]+1000,1.05);
  hPad->GetXaxis()->SetTitle("run");
  if( strcmp(EBEE.c_str(),"EB") == 0 )
    hPad->GetYaxis()->SetTitle("Relative E/p scale"); 
  else
    hPad->GetYaxis()->SetTitle("Relative E/p scale"); 
  hPad->GetYaxis()->SetTitleOffset(tYoffset);
  hPad->GetYaxis()->SetTitleOffset(0.8);
  hPad->GetXaxis()->SetLabelSize(labSize);
  hPad->GetXaxis()->SetTitleSize(labSize);
  hPad->GetYaxis()->SetLabelSize(labSize);
  hPad->GetYaxis()->SetTitleSize(labSize);
  hPad -> SetMinimum(yMIN);
  hPad -> SetMaximum(yMAX);
  
  // draw history plot
  g_fit_run -> SetMarkerStyle(20);
  g_fit_run -> SetMarkerSize(0.7);
  g_fit_run -> SetMarkerColor(kRed+2);
  g_fit_run -> SetLineColor(kRed+2);
  g_fit_run -> Draw("P");
  g_c_fit_run -> SetMarkerStyle(20);
  g_c_fit_run -> SetMarkerColor(kGreen+2);
  g_c_fit_run -> SetLineColor(kGreen+2);
  g_c_fit_run -> SetMarkerSize(0.7);
  g_c_fit_run -> Draw("P,same");
  
  
  cRight -> cd();
  
  s_EoP_spread = new TPaveStats();
  s_EoC_spread = new TPaveStats();
  
  
  h_EoC_spread_run -> SetFillStyle(3001);
  h_EoC_spread_run -> SetFillColor(kGreen+2);
  h_EoC_spread_run->GetYaxis()->SetLabelSize(labSize2);
  h_EoC_spread_run->GetYaxis()->SetTitleSize(labSize2);
  h_EoC_spread_run->GetYaxis()->SetNdivisions(505);
  h_EoC_spread_run->GetYaxis()->SetLabelOffset(-0.02);
  h_EoC_spread_run->GetXaxis()->SetLabelOffset(1000);

  h_EoC_spread_run -> Draw("hbar");
  gPad -> Update();
  
  s_EoC_spread = (TPaveStats*)(h_EoC_spread_run->GetListOfFunctions()->FindObject("stats"));
  s_EoC_spread ->SetTextColor(kGreen+2);
  s_EoC_spread ->SetTextSize(0.06);
  s_EoC_spread->SetX1NDC(0.49); //new x start position
  s_EoC_spread->SetX2NDC(0.99); //new x end position
  s_EoC_spread->SetY1NDC(0.475); //new x start position
  s_EoC_spread->SetY2NDC(0.590); //new x end position
  s_EoC_spread -> SetOptStat(1100);
  s_EoC_spread -> Draw("sames");

  h_EoP_spread_run -> SetFillStyle(3001);
  h_EoP_spread_run -> SetFillColor(kRed+2);
  h_EoP_spread_run -> Draw("hbarsames");
  gPad -> Update();
  
  s_EoP_spread = (TPaveStats*)(h_EoP_spread_run->GetListOfFunctions()->FindObject("stats"));
  s_EoP_spread->SetX1NDC(0.49); //new x start position
  s_EoP_spread->SetX2NDC(0.99); //new x end position
  s_EoP_spread->SetY1NDC(0.350); //new x start position
  s_EoP_spread->SetY2NDC(0.475); //new x end position
  s_EoP_spread ->SetOptStat(1100);
  s_EoP_spread ->SetTextColor(kRed+2);
  s_EoP_spread ->SetTextSize(0.06);
  s_EoP_spread -> Draw("sames");
  
  
  
  
  c_chi2 -> Print((folderName+"/"+folderName+"_fitChi2.png").c_str(),"png");
  c_scOccupancy -> Print((folderName+"/"+folderName+"_scOccupancy.png").c_str(),"png");
  c_seedOccupancy -> Print((folderName+"/"+folderName+"_seedOccupancy.png").c_str(),"png");
  cplot -> Print((folderName+"/"+folderName+"_history_vsTime.png").c_str(),"png");
  cplot_run -> Print((folderName+"/"+folderName+"_history_vsRun.png").c_str(),"png");
    
  c_chi2 -> Print((folderName+"/"+folderName+"_fitChi2.pdf").c_str(),"pdf");
  c_scOccupancy -> Print((folderName+"/"+folderName+"_scOccupancy.pdf").c_str(),"pdf");
  c_seedOccupancy -> Print((folderName+"/"+folderName+"_seedOccupancy.pdf").c_str(),"pdf");
  cplot -> Print((folderName+"/"+folderName+"_history_vsTime.pdf").c_str(),"pdf");
  cplot_run -> Print((folderName+"/"+folderName+"_history_vsRun.pdf").c_str(),"pdf");
  
  cplot -> SaveAs((folderName+"/"+folderName+"_history_vsTime.C").c_str());
  cplot_run -> SaveAs((folderName+"/"+folderName+"_history_vsRun.C").c_str());

  cplot -> SaveAs((folderName+"/"+folderName+"_history_vsTime.root").c_str());
  cplot_run -> SaveAs((folderName+"/"+folderName+"_history_vsRun.root").c_str());

  
  
   o -> cd();
        
   h_template -> Write();

   h_scOccupancy_eta   -> Write();
   h_scOccupancy_phi   -> Write();
   h_seedOccupancy_EB  -> Write(); 
   h_seedOccupancy_EEp -> Write();
   h_seedOccupancy_EEm -> Write();

   g_fit   -> Write("g_fit");
   g_c_fit -> Write("g_c_fit");
   g_fit_run   -> Write("g_fit_run");
   g_c_fit_run -> Write("g_c_fit_run");
   g_las -> Write("g_las");
   g_LT -> Write("g_LT");
   
   h_EoP_chi2 -> Write();
   h_EoC_chi2 -> Write();
   
   //ciao
   
   for(int i = 0; i < nBins; ++i)
     {
       gStyle->SetOptFit(1111);
       
       h_EoP[i] -> Write();
       h_EoC[i] -> Write();
       f_EoP[i] -> Write();
       f_EoC[i] -> Write();
       //  h_Tsp[i] -> Write();
       //
       //  h_Cvl[i] -> Write();
     }
   
   o -> Close();
   
  return 0;
  }
  //------------------------------ LASER MONITORING WITH E/P versus PILE UP  ------------------------------------------------------
 
  if(vm.count("laserMonitoringEPvsPU")) {	  


    //float yMIN = 0.95;
    //float yMAX = 1.05;


  // Set style options
  setTDRStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptStat(1110); 
  gStyle->SetOptFit(1); 
  
  // Set fitting options
  TVirtualFitter::SetDefaultFitter("Fumili2");
  
  
  
  //-----------------
  // Input parameters
 
  
  std::cout << "\n***************************************************************************************************************************" << std::endl;
 
  std::string dayMinLabel = "";
  std::string dayMaxLabel = "";
  std::string dayZOOMLabel ="";
  float absEtaMin = -1.;
  float absEtaMax = -1.;
  int IetaMin = -1;
  int IetaMax = -1;
  int IphiMin = -1;
  int IphiMax = -1;

  

  
  int t1 = 0;
  int t2 = 60;
  
  std::cout << "EBEE: "        << EBEE          << std::endl;
  std::cout << "evtsPerPoint: "  << evtsPerPoint  << std::endl;
  std::cout << "useRegression: " << useRegression << std::endl;
  std::cout << "dayMin: "        << dayMin        << std::endl;
  std::cout << "dayZOOM: "       << dayZOOM       << std::endl;
  std::cout << "dayMax: "        << dayMax        << std::endl;
  std::cout << "yMin: "          << yMIN          << std::endl;
  std::cout << "yMax: "          << yMAX          << std::endl;
  std::cout << "absEtaMin: "     << absEtaMin     << std::endl;
  std::cout << "absEtaMax: "     << absEtaMax     << std::endl;
  std::cout << "IetaMin: "       << IetaMin       << std::endl;
  std::cout << "IetaMax: "       << IetaMax       << std::endl;
  std::cout << "IphiMin: "       << IphiMin       << std::endl;
  std::cout << "IphiMax: "       << IphiMax       << std::endl;
  std::cout << "t1: "            << t1            << std::endl;
  std::cout << "t2: "            << t2            << std::endl;

  
  std::string dayZOOM = "";
  std::string dayMin = "";
  std::string dayMax = "";
  
   
  //-------------------
  // Define in/outfiles
  
  std::string folderName = std::string(EBEE) + "_" + dayMinLabel + "_" + dayMaxLabel;
  if( (absEtaMin != -1.) && (absEtaMax != -1.) )
  {
    char absEtaBuffer[50];
    sprintf(absEtaBuffer,"_%.2f-%.2f",absEtaMin,absEtaMax);
    folderName += std::string(absEtaBuffer);
  } 
  
  if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) )
  {
    char absEtaBuffer[50];
    sprintf(absEtaBuffer,"_Ieta_%d-%d_Iphi_%d-%d",IetaMin,IetaMax,IphiMin,IphiMax);
    folderName += std::string(absEtaBuffer);
  } 
  
  gSystem->mkdir(folderName.c_str());
  TFile* o = new TFile((folderName+"/"+folderName+"_histos.root").c_str(),"RECREATE");
  
  
  
  // Get trees
  std::cout << std::endl;
 

  if (data->GetEntries() == 0 || mc->GetEntries() == 0 )
  {
    std::cout << "Error: At least one file is empty" << std::endl; 
    return -1;
  }
  
  
  
  // Set branch addresses
  int runNumber;
  int nPV;
  int nPU;
  float avgLCSCEle[3], seedLaserAlphaSCEle1, etaSCEle[3], phiSCEle[3], energySCEle[3], esEnergySCEle[3], pAtVtxGsfEle[3], energySCEle_corr[3];
  int seedXSCEle[3], seedYSCEle[3];//, seedZside;
  
  data->SetBranchStatus("*",0);
  data->SetBranchStatus("runNumber",1);  
  data->SetBranchStatus("nPV",1);
  data->SetBranchStatus("nPU",1);
  data->SetBranchStatus("avgLCSCEle",1);
  data->SetBranchStatus("seedLaserAlphaSCEle1",1);
  //  data->SetBranchStatus("ele1_EOverP",1);
  data->SetBranchStatus("etaSCEle",1);
  data->SetBranchStatus("phiSCEle",1);
  data->SetBranchStatus("energySCEle_must",1);
  data->SetBranchStatus("energySCEle_corr",1);
  data->SetBranchStatus("esEnergySCEle",1);
  data->SetBranchStatus("pAtVtxGsfEle",1);
  data->SetBranchStatus("seedXSCEle",1);
  data->SetBranchStatus("seedYSCEle",1);
  //  data->SetBranchStatus("ele1_seedZside",1);
    
  data->SetBranchAddress("runNumber", &runNumber);  
  data->SetBranchAddress("nPV", &nPV);
  data->SetBranchAddress("nPU", &nPU);
  data->SetBranchAddress("avgLCSCEle", &avgLCSCEle[0]);
  data->SetBranchAddress("seedLaserAlphaSCEle1", &seedLaserAlphaSCEle1);
  //  data->SetBranchAddress("ele1_EOverP", &EoP);
  data->SetBranchAddress("etaSCEle", &etaSCEle);
  data->SetBranchAddress("phiSCEle", &phiSCEle);
  if( useRegression < 1 )
    data->SetBranchAddress("energySCEle_must", &energySCEle);
  else
    data->SetBranchAddress("energySCEle_corr", &energySCEle_corr);
  //  data->SetBranchAddress("energySCEle", &energySCEle);
  data->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  data->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  data->SetBranchAddress("seedXSCEle", &seedXSCEle);
  data->SetBranchAddress("seedYSCEle", &seedYSCEle);
  //  data->SetBranchAddress("ele1_seedZside", &seedZside);


  mc->SetBranchStatus("*",0);
  mc->SetBranchStatus("runNumber",1);  
  mc->SetBranchStatus("nPV",1);
  mc->SetBranchStatus("nPU",1);
  mc->SetBranchStatus("avgLCSCEle",1);
  mc->SetBranchStatus("seedLaserAlphaSCEle1",1);
  //  mc->SetBranchStatus("ele1_EOverP",1);
  mc->SetBranchStatus("etaSCEle",1);
  mc->SetBranchStatus("phiSCEle",1);
  mc->SetBranchStatus("energySCEle_must",1);
  mc->SetBranchStatus("energySCEle_corr",1);
  mc->SetBranchStatus("esEnergySCEle",1);
  mc->SetBranchStatus("pAtVtxGsfEle",1);
  mc->SetBranchStatus("seedXSCEle",1);
  mc->SetBranchStatus("seedYSCEle",1);
  //  mc->SetBranchStatus("ele1_seedZside",1);
    
  mc->SetBranchAddress("runNumber", &runNumber);  
  mc->SetBranchAddress("nPV", &nPV);
  mc->SetBranchAddress("nPU", &nPU);
  mc->SetBranchAddress("avgLCSCEle", &avgLCSCEle[0]);
  mc->SetBranchAddress("seedLaserAlphaSCEle1", &seedLaserAlphaSCEle1);
  //  mc->SetBranchAddress("ele1_EOverP", &EoP);
  mc->SetBranchAddress("etaSCEle", &etaSCEle);
  mc->SetBranchAddress("phiSCEle", &phiSCEle);
  if( useRegression < 1 )
    mc->SetBranchAddress("energySCEle_must", &energySCEle);
  else
    mc->SetBranchAddress("energySCEle_corr", &energySCEle_corr);
  //  mc->SetBranchAddress("energySCEle", &energySCEle);
  mc->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  mc->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  mc->SetBranchAddress("seedXSCEle", &seedXSCEle);
  mc->SetBranchAddress("seedYSCEle", &seedYSCEle);
  //  mc->SetBranchAddress("ele1_seedZside", &seedZside);
  
  
  
  
  
  
  //--------------------------------------------------------
  // Define PU correction (to be used if useRegression == 0)
  
  // corr = p0 + p1 * nPU
  float p0_EB;
  float p1_EB;
  float p0_EE;
  float p1_EE;
  
  if( useRegression == 0 )
  {
    //2012 EB
    p0_EB = 0.9991;
    p1_EB = 0.0001635;
    //2012 EE
    p0_EE = 0.9968;
    p1_EE = 0.001046;
  }
  else
  {
    //2012 EB
    p0_EB = 1.001;
    p1_EB = -0.000143;
    //2012 EE
    p0_EE = 1.00327;
    p1_EE = -0.000432;
  }
  
  float p0 = -1.;
  float p1 = -1.;
  
  if( strcmp(EBEE.c_str(),"EB") == 0 )
  {
    p0 = p0_EB;
    p1 = p1_EB;
  }
  else
  {
    p0 = p0_EE;
    p1 = p1_EE;
  }
  
  //2015
  p0=1.;
  p1=0.;
  
  
  
  
  //---------------------------------
  // Build the reference distribution
  
  std::cout << std::endl;
  std::cout << "***** Build reference for " << EBEE << " *****" << std::endl;
  
  TH1F* h_template = new TH1F("template", "", 2000, 0., 5.);
  
  for(int ientry = 0; ientry < mc->GetEntries(); ++ientry)
  {
    if( (ientry%100000 == 0) ) std::cout << "reading MC entry " << ientry << "\r" << std::endl;//std::flush;
    mc->GetEntry(ientry);
    
    // selections
    if( (strcmp(EBEE.c_str(),"EB") == 0) && (fabs(etaSCEle[0]) > 1.479) )                    continue; // barrel
    if( (strcmp(EBEE.c_str(),"EB_0_1") == 0) && (fabs(etaSCEle[0]) > 1.0)) continue; // inner barrel
    if( (strcmp(EBEE.c_str(),"EB_1_1479") == 0) && (fabs(etaSCEle[0]) < 1.0 || fabs(etaSCEle[0]) > 1.479) )                     continue; // barrel
    if( (strcmp(EBEE.c_str(),"EE") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_1479_2") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_2_25") == 0) && (fabs(etaSCEle[0]) < 2.0 || fabs(etaSCEle[0])>2.5) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEp") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_1479_2") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_25") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_225") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_225_25") == 0) && (etaSCEle[0] < 2.25 || etaSCEle[0]>2.25) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEm") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_1479_2") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_25") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_225") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_225_25") == 0) && (etaSCEle[0] > -2.25 || etaSCEle[0]<-2.5) ) continue; // endcap

    if( (absEtaMin != -1.) && (absEtaMax != -1.) )
    {
      if( (fabs(etaSCEle[0]) < absEtaMin) || (fabs(etaSCEle[0]) > absEtaMax) ) continue;
    }
    
    if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) )
    {
      if( (seedXSCEle[0] < IetaMin) || (seedXSCEle[0] > IetaMax) ) continue;
      if( (seedYSCEle[0] < IphiMin) || (seedYSCEle[0] > IphiMax) ) continue;
    }
    
    // PU correction
    float PUCorr = (p0 + p1*nPU);
    //std::cout << "p0: " << p0  << "   p1: " << p1 << "   nPU: " << nPU << std::endl;
    
    // fill the template histogram
    h_template -> Fill( (energySCEle[0]-esEnergySCEle[0])/(pAtVtxGsfEle[0]-esEnergySCEle[0]) / PUCorr );
  }
  
  std::cout << "Reference built for " << EBEE << " - " << h_template->GetEntries() << " events" << std::endl;
  
  
  
  
  
  
  //---------------------
  // Loop and sort events
  
  std::cout << std::endl;
  std::cout << "***** Sort events and define bins *****" << std::endl;
  
  int nEntries = data -> GetEntriesFast(); 
  int nSavePts = 0; 
  std::vector<bool> isSavedEntries(nEntries);
  std::vector<Sorter> sortedEntries;
  std::vector<int> timeStampFirst;
  
  for(int ientry = 0; ientry < nEntries; ++ientry)
  {
    data -> GetEntry(ientry);
    isSavedEntries.at(ientry) = false;
    
    // selections
    if( (strcmp(EBEE.c_str(),"EB") == 0) && (fabs(etaSCEle[0]) > 1.479) )                    continue; // barrel
    if( (strcmp(EBEE.c_str(),"EB_0_1") == 0) && (fabs(etaSCEle[0]) > 1.0)) continue; // inner barrel
    if( (strcmp(EBEE.c_str(),"EB_1_1479") == 0) && (fabs(etaSCEle[0]) < 1.0 || fabs(etaSCEle[0]) > 1.479) )                     continue; // barrel
    if( (strcmp(EBEE.c_str(),"EE") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_1479_2") == 0) && (fabs(etaSCEle[0]) < 1.479 || fabs(etaSCEle[0])>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EE_2_25") == 0) && (fabs(etaSCEle[0]) < 2.0 || fabs(etaSCEle[0])>2.5) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEp") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_1479_2") == 0) && (etaSCEle[0] < 1.479 || etaSCEle[0]>2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_25") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_2_225") == 0) && (etaSCEle[0] < 2.0 || etaSCEle[0]>2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEp_225_25") == 0) && (etaSCEle[0] < 2.25 || etaSCEle[0]>2.25) ) continue; // endcap

    if( (strcmp(EBEE.c_str(),"EEm") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.5 ) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_1479_2") == 0) && (etaSCEle[0] > -1.479 || etaSCEle[0]<-2.0) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_25") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.5) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_2_225") == 0) && (etaSCEle[0] > -2.0 || etaSCEle[0]<-2.25) ) continue; // endcap
    if( (strcmp(EBEE.c_str(),"EEm_225_25") == 0) && (etaSCEle[0] > -2.25 || etaSCEle[0]<-2.5) ) continue; // endcap
    
    if( (absEtaMin != -1.) && (absEtaMax != -1.) )
    {
      if( (fabs(etaSCEle[0]) < absEtaMin) || (fabs(etaSCEle[0]) > absEtaMax) ) continue;
    }
    
    if( (IetaMin != -1.) && (IetaMax != -1.) && (IphiMin != -1.) && (IphiMax != -1.) )
    {
      if( (seedXSCEle[0] < IetaMin) || (seedXSCEle[0] > IetaMax) ) continue;
      if( (seedYSCEle[0] < IphiMin) || (seedYSCEle[0] > IphiMax) ) continue;
    }
    
    if( nPV < t1 ) continue;
    if( nPV > t2 ) continue;
    
    if( avgLCSCEle[0] <= 0. ) continue;
    
    isSavedEntries.at(ientry) = true;
    
    
    // fill sorter
    Sorter dummy;
    dummy.time = nPV;
    dummy.entry = ientry;
    sortedEntries.push_back(dummy);
    
    ++nSavePts;
  }
  
  // sort events
  std::sort(sortedEntries.begin(),sortedEntries.end(),Sorter());
  std::cout << "Data sorted in " << EBEE << " - " << nSavePts << " events" << std::endl;
  
  std::map<int,int> antiMap;
  for(unsigned int iSaved = 0; iSaved < sortedEntries.size(); ++iSaved)
    antiMap[sortedEntries.at(iSaved).entry] = iSaved;
  
  
  //---------------------
  // Loop and define bins
  
  // "wide" bins - find events with time separation bigger than 1 day
  int nWideBins = 1;
  std::vector<int> wideBinEntryMax;
  //int timeStampOld = -1;
  
  wideBinEntryMax.push_back(0);  
  
  for(int iSaved = 0; iSaved < nSavePts; ++iSaved)
  {
    /* if( iSaved%100000 == 0 ) std::cout << "reading saved entry " << iSaved << "\r" << std::flush;
    data->GetEntry(sortedEntries[iSaved].entry);  
    
    if( iSaved == 0 )
    {
      timeStampOld = nPV;
      continue;
    }
    
    if( (nPV-timeStampOld)/3600. > timeLapse )
    {
      ++nWideBins;
      wideBinEntryMax.push_back(iSaved-1);
    }
    

    timeStampOld = nPV;
    */
  }

  
  std::cout << std::endl;
  wideBinEntryMax.push_back(nSavePts);
  
  // bins with approximatively evtsPerPoint events per bin
  int nBins = 0;
  std::vector<int> binEntryMax;
  
  binEntryMax.push_back(0);
  for(int wideBin = 0; wideBin < nWideBins; ++wideBin)
  {
    int nTempBins = std::max(1,int( (wideBinEntryMax.at(wideBin+1)-wideBinEntryMax.at(wideBin))/evtsPerPoint ));
    int nTempBinEntries = int( (wideBinEntryMax.at(wideBin+1)-wideBinEntryMax.at(wideBin))/nTempBins );
    
    for(int tempBin = 0; tempBin < nTempBins; ++tempBin)
    {
      ++nBins;
      if( tempBin < nTempBins - 1 )
        binEntryMax.push_back( wideBinEntryMax.at(wideBin) + (tempBin+1)*nTempBinEntries );
      else
        binEntryMax.push_back( wideBinEntryMax.at(wideBin+1) );
    }
  }
  
  //  std::cout << "nBins = " << nBins << std::endl;
  //for(int bin = 0; bin < nBins; ++bin)
  //  std::cout << "bin: " << bin
  //            << "   entry min: " << setw(6) << binEntryMax.at(bin)
  //            << "   entry max: " << setw(6) << binEntryMax.at(bin+1)
  //            << "   events: "    << setw(6) << binEntryMax.at(bin+1)-binEntryMax.at(bin)
  //            << std::endl;
  
  
  
  
  
  
  //---------------------
  // histogram definition
  
  TH1F* h_scOccupancy_eta  = new TH1F("h_scOccupancy_eta","", 298, -2.6, 2.6);
  TH1F* h_scOccupancy_phi  = new TH1F("h_scOccupancy_phi","", 363, -3.1765, 3.159);
  SetHistoStyle(h_scOccupancy_eta);
  SetHistoStyle(h_scOccupancy_phi);
  
  TH2F* h_seedOccupancy_EB  = new TH2F("h_seedOccupancy_EB","",  171, -86., 85., 361,   0.,361.);
  TH2F* h_seedOccupancy_EEp = new TH2F("h_seedOccupancy_EEp","", 101,   0.,101., 100,   0.,101.);
  TH2F* h_seedOccupancy_EEm = new TH2F("h_seedOccupancy_EEm","", 101,   0.,101., 100,   0.,101.);
  SetHistoStyle(h_seedOccupancy_EB);
  SetHistoStyle(h_seedOccupancy_EEp);
  SetHistoStyle(h_seedOccupancy_EEm);
  
  TH1F* h_EoP_spread = new TH1F("h_EoP_spread","",100,yMIN,yMAX);
  TH1F* h_EoC_spread = new TH1F("h_EoC_spread","",100,yMIN,yMAX);
  TH1F* h_EoP_spread_run = new TH1F("h_EoP_spread_run","",100,yMIN,yMAX);
  TH1F* h_EoC_spread_run = new TH1F("h_EoC_spread_run","",100,yMIN,yMAX);
  SetHistoStyle(h_EoP_spread,"EoP");
  SetHistoStyle(h_EoC_spread,"EoC");
  SetHistoStyle(h_EoP_spread_run,"EoP");
  SetHistoStyle(h_EoC_spread_run,"EoC");
  
  TH1F* h_EoP_chi2 = new TH1F("h_EoP_chi2","",50,0.,5.);
  TH1F* h_EoC_chi2 = new TH1F("h_EoC_chi2","",50,0.,5.);
  SetHistoStyle(h_EoP_chi2,"EoP");
  SetHistoStyle(h_EoC_chi2,"EoC");  
  
  TH1F** h_EoP = new TH1F*[nBins];
  TH1F** h_EoC = new TH1F*[nBins];
  TH1F** h_Las = new TH1F*[nBins];
  TH1F** h_Tsp = new TH1F*[nBins];
  TH1F** h_Cvl = new TH1F*[nBins];
  
  for(int i = 0; i < nBins; ++i)
  {
    char histoName[80];
    
    sprintf(histoName, "EoP_%d", i);
    h_EoP[i] = new TH1F(histoName, histoName, 2000, 0., 5.);
    SetHistoStyle(h_EoP[i],"EoP");
    
    sprintf(histoName, "EoC_%d", i);
    h_EoC[i] = new TH1F(histoName, histoName, 2000, 0., 5.);
    SetHistoStyle(h_EoC[i],"EoC");
    
    sprintf(histoName, "Las_%d", i);
    h_Las[i] = new TH1F(histoName, histoName, 500, 0.5, 1.5);
    
    sprintf(histoName, "Tsp_%d", i);
    h_Tsp[i] = new TH1F(histoName, histoName, 500, 0.5, 1.5);
   
  }
  
  
  // function definition
    TF1** f_EoP = new TF1*[nBins];
    TF1** f_EoC = new TF1*[nBins];
  
  
  // graphs definition
  TGraphAsymmErrors* g_fit   = new TGraphAsymmErrors();
  TGraphAsymmErrors* g_c_fit = new TGraphAsymmErrors();
  
  TGraphAsymmErrors* g_fit_run   = new TGraphAsymmErrors();
  TGraphAsymmErrors* g_c_fit_run = new TGraphAsymmErrors();  
  
  TGraph* g_las = new TGraph();
  
  TGraphErrors* g_LT = new TGraphErrors();
  
  g_fit->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_fit->GetXaxis()->SetTimeDisplay(1);
  g_c_fit->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_c_fit->GetXaxis()->SetTimeDisplay(1);
  g_las->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_las->GetXaxis()->SetTimeDisplay(1);
  g_LT->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  g_LT->GetXaxis()->SetTimeDisplay(1);
    
  
  
  
  
  
  //------------------------------------
  // loop on the saved and sorted events
  
  std::cout << std::endl;
  std::cout << "***** Fill and fit histograms *****" << std::endl;

  std::vector<int> Entries(nBins);  
  std::vector<double> AveTime(nBins);
  std::vector<int> MinTime(nBins);
  std::vector<int> MaxTime(nBins);
  std::vector<double> AveRun(nBins);    
  std::vector<int> MinRun(nBins);
  std::vector<int> MaxRun(nBins);
  std::vector<double> AveLT(nBins);
  std::vector<double> AveLT2(nBins);
    
  int iSaved = -1;
  for(int ientry = 0; ientry < nEntries; ++ientry)
  {
    if( (ientry%100000 == 0) ) std::cout << "reading entry " << ientry << "\r" << std::endl;//std::flush;
    
    if( isSavedEntries.at(ientry) == false ) continue;
    
    ++iSaved;
    
    int iSaved = antiMap[ientry];
    int bin = -1;
    for(bin = 0; bin < nBins; ++bin)
      if( iSaved >= binEntryMax.at(bin) && iSaved < binEntryMax.at(bin+1) )
	break;
    
    //std::cout << "bin = " << bin << "   iSaved = "<< iSaved << std::endl;
    data->GetEntry(ientry);
    
    
    
    Entries[bin] += 1;
    
    if( iSaved == binEntryMax.at(bin)+1 )   MinTime[bin] = nPV;
    if( iSaved == binEntryMax.at(bin+1)-1 ) MaxTime[bin] = nPV;
    AveTime[bin] += nPV;
    
    if( iSaved == binEntryMax.at(bin)+1 )   MinRun[bin] = runNumber;
    if( iSaved == binEntryMax.at(bin+1)-1 ) MaxRun[bin] = runNumber;
    AveRun[bin] += runNumber;
    
    float LT = (-1. / seedLaserAlphaSCEle1 * log(avgLCSCEle[0]));
    AveLT[bin] += LT;
    AveLT2[bin] += LT*LT;
    
    // PU correction
    float PUCorr = (p0 + p1*nPU);
    
    // fill the histograms
    (h_EoP[bin]) -> Fill( (energySCEle[0]-esEnergySCEle[0])/(pAtVtxGsfEle[0]-esEnergySCEle[0]) / avgLCSCEle[0] / PUCorr);
    (h_EoC[bin]) -> Fill( (energySCEle[0]-esEnergySCEle[0])/(pAtVtxGsfEle[0]-esEnergySCEle[0]) / PUCorr );
    
    (h_Las[bin]) -> Fill(avgLCSCEle[0]);
    (h_Tsp[bin]) -> Fill(1./avgLCSCEle[0]);
    
    h_scOccupancy_eta -> Fill(etaSCEle[0]);
    h_scOccupancy_phi -> Fill(phiSCEle[0]);
    if(fabs(etaSCEle[0])<1.449)
      h_seedOccupancy_EB -> Fill(seedXSCEle[0],seedYSCEle[0]);
    else if(etaSCEle[0]>1.449)
      h_seedOccupancy_EEp -> Fill(seedXSCEle[0],seedYSCEle[0]);
    else if(etaSCEle[0]<-1.449)
      h_seedOccupancy_EEm -> Fill(seedXSCEle[0],seedYSCEle[0]);
  }
  
  for(int bin = 0; bin < nBins; ++bin)
  {
    AveTime[bin] = 1. * AveTime[bin] / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    AveRun[bin]  = 1. * AveRun[bin]  / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    AveLT[bin]   = 1. * AveLT[bin]   / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    AveLT2[bin]  = 1. * AveLT2[bin]  / (binEntryMax.at(bin+1)-binEntryMax.at(bin));
    //std::cout << date << " " << AveTime[i] << " " << MinTime[i] << " " << MaxTime[i] << std::endl;
  }
  
  
  
  
  
  
  int rebin = 2;
  if( strcmp(EBEE.c_str(),"EE") == 0 ) rebin *= 2;
  
  h_template -> Rebin(rebin);
  
  
  
  float EoP_scale = 0.;
  float EoP_err = 0.;
  int   EoP_nActiveBins = 0;
  
  float EoC_scale = 0.;
  float EoC_err = 0.;
  int   EoC_nActiveBins = 0;
  
  float LCInv_scale = 0;
  
  std::vector<int> validBins;
  for(int i = 0; i < nBins; ++i)
  {
    bool isValid = true;
    
    h_EoP[i] -> Rebin(rebin);
    h_EoC[i] -> Rebin(rebin);
    
    
    
    //------------------------------------
    // Fill the graph for uncorrected data
    
    // define the fitting function
    // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
    
    //o -> cd();
    char convolutionName[50];
    sprintf(convolutionName,"h_convolution_%d",i);
    //h_Cvl[i] = ConvoluteTemplate(std::string(convolutionName),h_template,h_Las[i],32768,-5.,5.);
    h_Cvl[i] = MellinConvolution(std::string(convolutionName),h_template,h_Tsp[i]);
    
    histoFunc* templateHistoFunc = new histoFunc(h_template);
    histoFunc* templateConvolutedHistoFunc = new histoFunc(h_Cvl[i]);
    char funcName[50];

    sprintf(funcName,"f_EoP_%d",i);
    
    if( strcmp(EBEE.c_str(),"EB") == 0 )
      f_EoP[i] = new TF1(funcName, templateConvolutedHistoFunc, 0.8*(h_Tsp[i]->GetMean()), 1.4*(h_Tsp[i]->GetMean()), 3, "histoFunc");
    else
      f_EoP[i] = new TF1(funcName, templateConvolutedHistoFunc, 0.75*(h_Tsp[i]->GetMean()), 1.5*(h_Tsp[i]->GetMean()), 3, "histoFunc");
    
    f_EoP[i] -> SetParName(0,"Norm"); 
    f_EoP[i] -> SetParName(1,"Scale factor"); 
    f_EoP[i] -> SetLineWidth(1); 
    f_EoP[i] -> SetNpx(10000);
    
    double xNorm = h_EoP[i]->GetEntries()/h_template->GetEntries() *
      h_EoP[i]->GetBinWidth(1)/h_template->GetBinWidth(1); 
    
    f_EoP[i] -> FixParameter(0, xNorm);
    f_EoP[i] -> SetParameter(1, 1.);
    f_EoP[i] -> FixParameter(2, 0.);
    f_EoP[i] -> SetLineColor(kRed+2); 
    
    int fStatus = 0;
    int nTrials = 0;
    TFitResultPtr rp;
   
    rp = h_EoP[i] -> Fit(funcName, "ERLS+");
    while( (fStatus != 0) && (nTrials < 10) )
    {
      rp = h_EoP[i] -> Fit(funcName, "ERLS+");
      fStatus = rp;
      if(fStatus == 0) break;
      ++nTrials;
      }
    
    // fill the graph
    double eee = f_EoP[i]->GetParError(1);
    //float k    = f_EoP[i]->GetParameter(1);
    float k    = f_EoP[i]->GetParameter(1) / h_Tsp[i]->GetMean(); //needed when using mellin's convolution 
    //    std::cout << "noCORR" <<  std::endl;
    //    std::cout << "eee: " << f_EoP[i]->GetParError(1) << "k: " << f_EoP[i]->GetParameter(1) / h_Tsp[i]->GetMean() << std::endl;

    /*
    std::cout << i <<"--nocorr---- "<< 1./k << std::endl;
    std::cout <<" condizione 1: " << h_EoP[i]->GetEntries() << "  fStatus: " << fStatus << " eee: " << eee << "con eee che ci piace essere maggiore di : " <<  0.05*h_template->GetRMS()/sqrt(evtsPerPoint) << std::endl ;
    getchar();
    */
    //    if( (h_EoP[i]->GetEntries() > 3) && (fStatus == 0) && (eee > 0.05*h_template->GetRMS()/sqrt(evtsPerPoint)) )

    if( (h_EoP[i]->GetEntries() > 500) && (fStatus == 0) )
    {
      float date = (float)AveTime[i];
      float dLow = (float)(AveTime[i]-MinTime[i]); 
      float dHig = (float)(MaxTime[i]-AveTime[i]);
      float run = (float)AveRun[i];
      float rLow = (float)(AveRun[i]-MinRun[i]); 
      float rHig = (float)(MaxRun[i]-AveRun[i]);
      g_fit -> SetPoint(i,  date , 1./k);
      g_fit -> SetPointError(i, dLow , dHig, eee/k/k, eee/k/k);
      g_fit_run -> SetPoint(i,  run , 1./k);
      g_fit_run -> SetPointError(i, rLow , rHig, eee/k/k, eee/k/k);
      
      std::cout <<"************-------------------*****************" <<std::endl;
      

      h_EoP_chi2 -> Fill(f_EoP[i]->GetChisquare()/f_EoP[i]->GetNDF());
      
      EoP_scale += 1./k;
      EoP_err += eee/k/k;
      ++EoP_nActiveBins;
    }
    else
    {
      std::cout << "Fitting uncorrected time bin: " << i << "   Fail status: " << fStatus << "   sigma: " << eee << std::endl;
      isValid = false;
    }  
    
    //----------------------------------
    // Fill the graph for corrected data
    
    // define the fitting function
    // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )

    sprintf(funcName,"f_EoC_%d",i);
    if( strcmp(EBEE.c_str(),"EB") == 0 )
      f_EoC[i] = new TF1(funcName, templateHistoFunc, 0.8, 1.4, 3, "histoFunc");
    else
      f_EoC[i] = new TF1(funcName, templateHistoFunc, 0.75, 1.5, 3, "histoFunc");
    f_EoC[i] -> SetParName(0,"Norm"); 
    f_EoC[i] -> SetParName(1,"Scale factor"); 
    f_EoC[i] -> SetLineWidth(1); 
    f_EoC[i] -> SetNpx(10000);
    
    xNorm = h_EoC[i]->GetEntries()/h_template->GetEntries() *
            h_EoC[i]->GetBinWidth(1)/h_template->GetBinWidth(1); 

    f_EoC[i] -> FixParameter(0, xNorm);
    f_EoC[i] -> SetParameter(1, 0.99);
    f_EoC[i] -> FixParameter(2, 0.);
    f_EoC[i] -> SetLineColor(kGreen+2); 
    
    
    rp = h_EoC[i] -> Fit(funcName, "ERLS+");
    fStatus = rp;
    nTrials = 0;
    while( (fStatus != 0) && (nTrials < 10) )
      {
	rp = h_EoC[i] -> Fit(funcName, "ERLS+");
	fStatus = rp;
	if(fStatus == 0) break;
	++nTrials;
      }

    
    // fill the graph
    k   = f_EoC[i]->GetParameter(1);
    eee = f_EoC[i]->GetParError(1); 
    //std::cout << "CORR" <<  std::endl;
    //std::cout << "eee: " << f_EoP[i]->GetParError(1) << "k: " << f_EoP[i]->GetParameter(1) / h_Tsp[i]->GetMean() << std::endl;    
    //getchar();
    /* std::cout << i <<"--corr---- "<< 1./k << std::endl;
    std::cout <<" condizione 1: " << h_EoP[i]->GetEntries() << "  fStatus: " << fStatus << " eee: " << eee << "con eee che ci piace essere maggiore di : " <<  0.05*h_template->GetRMS()/sqrt(evtsPerPoint) << std::endl ;
    getchar();
    */  


    if( (h_EoC[i]->GetEntries() > 500) && (fStatus == 0) )
    //  if( (h_EoC[i]->GetEntries() > 10) && (fStatus == 0) && (eee > 0.05*h_template->GetRMS()/sqrt(evtsPerPoint)) )
      {
      float date = (float)AveTime[i]; 
      float dLow = (float)(AveTime[i]-MinTime[i]); 
      float dHig = (float)(MaxTime[i]-AveTime[i]);
      float run = (float)AveRun[i];
      float rLow = (float)(AveRun[i]-MinRun[i]); 
      float rHig = (float)(MaxRun[i]-AveRun[i]);
            
      g_c_fit -> SetPoint(i,  date , 1./k);
      g_c_fit -> SetPointError(i, dLow , dHig , eee/k/k, eee/k/k);
      
      g_c_fit_run -> SetPoint(i,  run , 1./k);
      g_c_fit_run -> SetPointError(i, rLow , rHig, eee/k/k, eee/k/k);
      std::cout <<"************-------------------*****************" <<std::endl;
            

      h_EoC_chi2 -> Fill(f_EoC[i]->GetChisquare()/f_EoP[i]->GetNDF());
      
      EoC_scale += 1./k;
      EoC_err += eee/k/k;
      ++EoC_nActiveBins;
      }
    else
      {
	std::cout << "Fitting corrected time bin: " << i << "   Fail status: " << fStatus << "   sigma: " << eee << std::endl;
	isValid = false;
      }
    
    if( isValid == true ) validBins.push_back(i);
  }
  
  EoP_scale /= EoP_nActiveBins;
  EoP_err   /= EoP_nActiveBins;
  
  EoC_scale /= EoC_nActiveBins;
  EoC_err   /= EoC_nActiveBins;
  
  
  
  
  
  
  //----------------------------------------
  // Fill the graph for avg laser correction
  
  
  for(unsigned int itr = 0; itr < validBins.size(); ++itr)
  {  
    //float k0   = f_EoP[0]->GetParameter(1) / h_Tsp[0]->GetMean(); 
    int i = validBins.at(itr);
    //    g_las -> SetPoint(itr, (float)AveTime[i], (h_Tsp[i]->GetMean())+((1/k0)-(h_Tsp[0]->GetMean())) );
    g_las -> SetPoint(itr, (float)AveTime[i], h_Tsp[i]->GetMean() );


      //g_las -> SetPointffa(itr, (float)AveTime[i], h_Tsp[i]->GetMean());
    g_LT  -> SetPoint(itr, (float)AveTime[i], AveLT[i] );
    g_LT  -> SetPointError(itr, 0., sqrt(AveLT2[i]-AveLT[i]*AveLT[i]) / sqrt(Entries[i]) );
        
    LCInv_scale += h_Tsp[i]->GetMean();
  }  
  
  LCInv_scale /= validBins.size();
  
  
  
  
  
  
  //---------------
  // Rescale graphs
  
  float yscale = 1.;
  //float yscale = 1./EoC_scale;
  
  for(unsigned int itr = 0; itr < validBins.size(); ++itr)
  {
    double x,y; 
    g_fit -> GetPoint(itr,x,y); 
    g_fit -> SetPoint(itr,x,y*yscale);
    if ( (x > t1) && (x < t2) ) h_EoP_spread -> Fill(y*yscale);
    
    g_c_fit -> GetPoint(itr,x,y); 
    g_c_fit -> SetPoint(itr,x,y*yscale);
    if ( (x > t1) && (x < t2) ) h_EoC_spread -> Fill(y*yscale);
    
    g_fit_run -> GetPoint(itr,x,y); 
    g_fit_run -> SetPoint(itr,x,y*yscale); 
    if ( (x > t1) && (x < t2) ) h_EoP_spread_run -> Fill(y*yscale);
    
    g_c_fit_run -> GetPoint(itr,x,y); 
    g_c_fit_run -> SetPoint(itr,x,y*yscale);
    if ( (x > t1) && (x < t2) ) h_EoC_spread_run -> Fill(y*yscale);
    
    g_las -> GetPoint(itr,x,y);
    g_las -> SetPoint(itr,x,y*yscale*EoP_scale/LCInv_scale);
  }
  //ciao
  TF1 EoC_pol0("EoC_pol0","pol0",t1,t2);
  EoC_pol0.SetLineColor(kGreen+2);
  EoC_pol0.SetLineWidth(2);
  EoC_pol0.SetLineStyle(2);
  g_c_fit -> Fit("EoC_pol0","QNR");
  
  
  
  
  
  
  
  
  //----------------------------
  // Print out global quantities
  
  std::cout << std::endl;
  std::cout << "***** Mean scales and errors *****" << std::endl; 
  std::cout << std::fixed;
  std::cout << std::setprecision(4);
  std::cout << "Mean EoP scale: "  << std::setw(6) << EoP_scale   << "   mean EoP error: " << std::setw(8) << EoP_err << std::endl;
  std::cout << "Mean EoC scale: "  << std::setw(6) << EoC_scale   << "   mean EoC error: " << std::setw(8) << EoC_err << std::endl;
  std::cout << "Mean 1/LC scale: " << std::setw(6) << LCInv_scale << std::endl;
  
  
  
  
  

  //-------------------
  // Final Plot vs Vertex
  //-------------------
  
  TCanvas* cplot = new TCanvas("cplot", "history plot vs Vertex",100,100,1000,500);
  cplot->cd();

  TPad *cLeft  = new TPad("pad_0","pad_0",0.00,0.00,0.75,1.00);
  TPad *cRight = new TPad("pad_1","pad_1",0.75,0.00,1.00,1.00);

  cLeft->SetLeftMargin(0.15); 
  cLeft->SetRightMargin(0.025); 
  cRight->SetLeftMargin(0.025); 

  cLeft->Draw();
  cRight->Draw();

  float tYoffset = 1.0; 
  float labSize = 0.05;
  float labSize2 = 0.06;

  cLeft->cd(); 
  
  cLeft->SetGridx();
  cLeft->SetGridy();
  
  TH1F *hPad = (TH1F*)gPad->DrawFrame(t1,0.9,t2,1.05);
    
  hPad->GetXaxis()->SetLimits(0,46);
  //hPad->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  //hPad->GetXaxis()->SetTimeDisplay(1);
  //hPad->GetXaxis() -> SetRangeUser(MinTime[0]-43200,MaxTime[nBins-1]+43200);
  hPad->GetXaxis()->SetTitle(" Number of Vertices");
  hPad->GetXaxis()->SetTitleOffset(0.8);
  
  //ciao
  
  
  if( strcmp(EBEE.c_str(),"EB") == 0 )
    hPad->GetYaxis()->SetTitle("Relative E/p scale"); 
  else
    hPad->GetYaxis()->SetTitle("Relative E/p scale"); 
  hPad->GetYaxis()->SetTitleOffset(tYoffset);
  hPad->GetXaxis()->SetLabelSize(labSize);
  hPad->GetXaxis()->SetTitleSize(labSize2);
  hPad->GetYaxis()->SetLabelSize(labSize);
  hPad->GetYaxis()->SetTitleSize(labSize2);
  hPad -> SetMinimum(yMIN);
  hPad -> SetMaximum(yMAX);
  
  // draw history plot
  g_fit -> SetMarkerStyle(24);
  g_fit -> SetMarkerSize(0.7);
  g_fit -> SetMarkerColor(kRed+2);
  g_fit -> SetLineColor(kRed+2);
  //g_fit -> Draw("P");
  g_c_fit -> SetMarkerStyle(20);
  g_c_fit -> SetMarkerColor(kGreen+2);
  g_c_fit -> SetLineColor(kGreen+2);
  g_c_fit -> SetMarkerSize(0.7);
  g_c_fit -> Draw("EP");
  g_las -> SetLineColor(kAzure-2);
  g_las -> SetLineWidth(2);
  //g_las -> Draw("L,same");
  
  TLegend* legend = new TLegend(0.60,0.78,0.90,0.94);
  legend -> SetLineColor(kWhite);
  legend -> SetLineWidth(0);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(0);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.04);
  legend -> AddEntry(g_c_fit,"with LM correction","PL");
  //legend -> AddEntry(g_fit,  "without LM correction","PL");
  //legend -> AddEntry(g_las,  "1 / LM correction","L");
  legend -> Draw("same");
  
  char latexBuffer[250];
  
  sprintf(latexBuffer,"CMS 2015 Preliminary");
  TLatex* latex = new TLatex(0.18,0.89,latexBuffer);  
  latex -> SetNDC();
  latex -> SetTextFont(62);
  latex -> SetTextSize(0.05);
  latex -> Draw("same");
  
  //sprintf(latexBuffer,"#sqrt{s} = 8 TeV   L = 3.95 fb^{-1}");
  sprintf(latexBuffer,"#sqrt{s} = 13 TeV");
  TLatex* latex2 = new TLatex(0.18,0.84,latexBuffer);  
  latex2 -> SetNDC();
  latex2 -> SetTextFont(42);
  latex2 -> SetTextSize(0.05);
  latex2 -> Draw("same");
  
  if( strcmp(EBEE.c_str(),"EB") == 0 || strcmp(EBEE.c_str(),"EB_0_1") == 0 || strcmp(EBEE.c_str(),"EB_1_1479") == 0)
    sprintf(latexBuffer,"ECAL Barrel");
  else
    sprintf(latexBuffer,"ECAL Endcap");
  TLatex* latex3 = new TLatex(0.18,0.19,latexBuffer);
  latex3 -> SetNDC();
  latex3 -> SetTextFont(42);
  latex3 -> SetTextSize(0.05);
  latex3 -> Draw("same");
  
  //sprintf(latexBuffer,"%.2E events",1.*nSavePts);
  //TLatex* latex4 = new TLatex(0.18,0.24,latexBuffer);  
  //latex4 -> SetNDC();
  //latex4 -> SetTextFont(42);
  //latex4 -> SetTextSize(0.04);
  //latex4 -> Draw("same");
  //
  //sprintf(latexBuffer,"%d events/bin - %d bins",evtsPerPoint,nBins);
  //TLatex* latex5 = new TLatex(0.18,0.19,latexBuffer);  
  //latex5 -> SetNDC();
  //latex5 -> SetTextFont(42);
  //latex5 -> SetTextSize(0.04);
  //latex5 -> Draw("same");
  
  
  cRight -> cd();
  
  TPaveStats* s_EoP_spread = new TPaveStats();
  TPaveStats* s_EoC_spread = new TPaveStats();
  
  
  h_EoC_spread -> SetFillStyle(3001);
  h_EoC_spread -> SetFillColor(kGreen+2);
  h_EoC_spread->GetYaxis()->SetLabelSize(0.09);
  h_EoC_spread->GetYaxis()->SetLabelOffset(-0.03);
  h_EoC_spread->GetYaxis()->SetTitleSize(0.08);
  h_EoC_spread->GetYaxis()->SetNdivisions(505);
  h_EoC_spread->GetXaxis()->SetLabelOffset(1000);
  
  h_EoC_spread -> Draw("hbar");
  gPad -> Update();
  
  s_EoC_spread = (TPaveStats*)(h_EoC_spread->GetListOfFunctions()->FindObject("stats"));
  s_EoC_spread -> SetStatFormat("1.4g");
  s_EoC_spread->SetX1NDC(0.06); //new x start position
  s_EoC_spread->SetX2NDC(0.71); //new x end position
  s_EoC_spread->SetY1NDC(0.43); //new x start position
  s_EoC_spread->SetY2NDC(0.34); //new x end position
  s_EoC_spread -> SetOptStat(1100);
  s_EoC_spread ->SetTextColor(kGreen+2);
  s_EoC_spread ->SetTextSize(0.08);
  s_EoC_spread -> Draw("sames");
  
  
  h_EoP_spread -> SetFillStyle(3001);
  h_EoP_spread -> SetFillColor(kRed+2);
  h_EoP_spread -> Draw("hbarsames");
  gPad -> Update();
  s_EoP_spread = (TPaveStats*)(h_EoP_spread->GetListOfFunctions()->FindObject("stats"));
  s_EoP_spread -> SetStatFormat("1.4g");
  s_EoP_spread->SetX1NDC(0.06); //new x start position
  s_EoP_spread->SetX2NDC(0.71); //new x end position
  s_EoP_spread->SetY1NDC(0.33); //new x start position
  s_EoP_spread->SetY2NDC(0.24); //new x end position
  s_EoP_spread ->SetOptStat(1100);
  s_EoP_spread ->SetTextColor(kRed+2);
  s_EoP_spread ->SetTextSize(0.08);
  s_EoP_spread -> Draw("sames");
  
  /*
  h_EoP_spread -> SetFillStyle(3001);
  h_EoP_spread -> SetFillColor(kRed+2);
  h_EoP_spread -> Draw("hbarsame");
  gPad -> Update();
  */
  
      
  
  
  
  cplot -> Print((folderName+"/"+folderName+"_history_vsVertex.png").c_str(),"png");
      
  cplot -> Print((folderName+"/"+folderName+"_history_vsVertex.pdf").c_str(),"pdf");
   
  cplot -> SaveAs((folderName+"/"+folderName+"_history_vsVertex.C").c_str());
  
  
  
   o -> cd();
        
   h_template -> Write();

   h_scOccupancy_eta   -> Write();
   h_scOccupancy_phi   -> Write();
   h_seedOccupancy_EB  -> Write(); 
   h_seedOccupancy_EEp -> Write();
   h_seedOccupancy_EEm -> Write();

   g_fit   -> Write("g_fit");
   g_c_fit -> Write("g_c_fit");
   g_fit_run   -> Write("g_fit_run");
   g_c_fit_run -> Write("g_c_fit_run");
   g_las -> Write("g_las");
   g_LT -> Write("g_LT");
   
   h_EoP_chi2 -> Write();
   h_EoC_chi2 -> Write();
   
   //ciao
   
   for(int i = 0; i < nBins; ++i)
     {
       gStyle->SetOptFit(1111);
       
       h_EoP[i] -> Write();
       h_EoC[i] -> Write();
       f_EoP[i] -> Write();
       f_EoC[i] -> Write();
       //  h_Tsp[i] -> Write();
       //
       //  h_Cvl[i] -> Write();
     }
   
   o -> Close();
   
   return 0;
  }
  ///////////--------------------------- E/P calibration ----------------------------------------------------------------------


  /////////////////////////////Momentum correction BARREL

//////////////////////////////////Momentum correction
  if(vm.count("momentumCorrectionEB")) {	
///////// produce P corrections
  
  std::string typeEB = "eta1";
  int  nRegionsEB = 1;
  int  nPhiBinsEB = 360;
  int  nPhiBinsTempEB = 1;
  int  nEtaBinsEB = 1;
  int  nEtaBinsTempEB = 1;
  int  rebinEB = 20;
  std::string outputFile = "momentumCalibration2015_EB.root";
  int nEntriesMC = -1;
  int nEntriesData = -1;
  
  //  int nRegionsEB = GetNRegionsEB(typeEB);

  std::cout<<"REGIONI: "<<nRegionsEB<<std::endl;

  cout <<" Basic Configuration " <<endl;
  cout <<" nRegionsEB = "<<nRegionsEB<<endl;
  cout <<" nPhiBinsEB = "<<nPhiBinsEB<<endl;
  cout <<" nPhiBinsTempEB = "<<nPhiBinsTempEB<<endl;
  cout <<" nEtaBinsEB = "<<nEtaBinsEB<<endl;
  cout <<" nEtaBinsTempEB = "<<nEtaBinsTempEB<<endl;
  cout <<" rebinEB = "<<rebinEB<<endl;
  
  cout << "Momentum correction "<< endl;
  
    
  //---- variables for selection
  float etaMax  = 1.445;
  float etaMin = 0;

  //  nEtaBinsEB=5;

  //histos to get the bin in phi given the electron phi
  TH1F* hPhiBinEB = new TH1F("hphiEB","hphiEB",nPhiBinsEB, -1.*TMath::Pi(),1.*TMath::Pi());
  TH1F* hEtaBinEB = new TH1F("hetaEB","hetaEB",nEtaBinsEB, 0,1.445);
  
  //----- NTUPLES--------------------
  /*
  TChain *ntu_DA = new TChain(TreeName.c_str());  
  if(!FillChain(*ntu_DA, infileDATA.c_str())) return 1;

  TChain *ntu_MC = new TChain(TreeName.c_str());  
  if(!FillChain(*ntu_MC, infileMC.c_str())) return 1;
  */
  
  std::cout << "     DATA: " << data->GetEntries() << " entries in Data sample" << std::endl;

  // Set branch addresses
  float etaSCEle[3], phiSCEle[3], etaEle[3], phiEle[3], esEnergySCEle[3], pAtVtxGsfEle[3], energySCEle[3], R9Ele[3];
  int seedXSCEle[3], seedYSCEle[3], chargeEle[3];//, seedZside;

  int debug=0;

  std::cout<<"debug: "<<debug<<std::endl; debug++;
  
  data->SetBranchStatus("*",0);
  data->SetBranchStatus("etaSCEle",1);
  data->SetBranchStatus("phiSCEle",1);
  data->SetBranchStatus("etaEle",1);
  data->SetBranchStatus("phiEle",1);
  data->SetBranchStatus("rawEnergySCEle",1);
  data->SetBranchStatus("energySCEle",1);
  data->SetBranchStatus("esEnergySCEle",1);
  data->SetBranchStatus("pAtVtxGsfEle",1);
  data->SetBranchStatus("seedXSCEle",1);
  data->SetBranchStatus("seedYSCEle",1);
  data->SetBranchStatus("chargeEle",1);
  data->SetBranchStatus("R9Ele",1);
    
  data->SetBranchAddress("etaSCEle", &etaSCEle);
  data->SetBranchAddress("phiSCEle", &phiSCEle);
  data->SetBranchAddress("etaEle", &etaEle);
  data->SetBranchAddress("phiEle", &phiEle);
  if (energyTypeEB==0)
    data->SetBranchAddress("rawEnergySCEle", &energySCEle);
  else
    data->SetBranchAddress("energySCEle", &energySCEle);
  data->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  data->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  //data->SetBranchAddress("energySCEle", &pAtVtxGsfEle);
  data->SetBranchAddress("seedXSCEle", &seedXSCEle);
  data->SetBranchAddress("seedYSCEle", &seedYSCEle);
  data->SetBranchAddress("chargeEle", &chargeEle);
  data->SetBranchAddress("R9Ele", &R9Ele);

  std::cout<<"debug: "<<debug<<std::endl; debug++;


  mc->SetBranchStatus("*",0);
  mc->SetBranchStatus("etaSCEle",1);
  mc->SetBranchStatus("phiSCEle",1);
  mc->SetBranchStatus("etaEle",1);
  mc->SetBranchStatus("phiEle",1);
  mc->SetBranchStatus("rawEnergySCEle",1);
  mc->SetBranchStatus("energySCEle",1);
  mc->SetBranchStatus("esEnergySCEle",1);
  mc->SetBranchStatus("pAtVtxGsfEle",1);
  mc->SetBranchStatus("seedXSCEle",1);
  mc->SetBranchStatus("seedYSCEle",1);
  mc->SetBranchStatus("chargeEle",1);
  mc->SetBranchStatus("R9Ele",1);
    
  mc->SetBranchAddress("etaSCEle", &etaSCEle);
  mc->SetBranchAddress("phiSCEle", &phiSCEle);
  mc->SetBranchAddress("etaEle", &etaEle);
  mc->SetBranchAddress("phiEle", &phiEle);
  if (energyTypeEB==0)
    mc->SetBranchAddress("rawEnergySCEle", &energySCEle);
  else
    mc->SetBranchAddress("energySCEle", &energySCEle);
  mc->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  mc->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  //mc->SetBranchAddress("energySCEle", &pAtVtxGsfEle);
  mc->SetBranchAddress("seedXSCEle", &seedXSCEle);
  mc->SetBranchAddress("seedYSCEle", &seedYSCEle);
  mc->SetBranchAddress("chargeEle", &chargeEle);
  mc->SetBranchAddress("R9Ele", &R9Ele);

  std::cout<<"debug: "<<debug<<std::endl; debug++;

  // histogram definition in EB and fit functions                                                                      
  //  std::vector<std::vector<TH1F*> > h_Phi_EB(nPhiBinsEB); // used to map iEta (as defined for Barrel and Endcap geom) into eta          
  //  std::vector<std::vector<TH1F*> > h_Eta_EB(nEtaBinsEB); // used to map iEta (as defined for Barrel and Endcap geom) into eta          
  TH1F* h_pData_EB[nPhiBinsEB][nEtaBinsEB][nRegionsEB];
  //  std::vector<std::vector<std::vector<TH1F*> > > h_pData_EB(nPhiBinsEB);
  TF1* f_pData_EB[nPhiBinsEB][nEtaBinsEB][nRegionsEB];
  
  TH1F* histoPull_EB[nEtaBinsEB][nRegionsEB];
  std::cout<<"debug: "<<debug<<std::endl; debug++;

  float maximum=500.;
  if(targetTypeEB==3) maximum=5.;
  int chargeType=1; //1=electrons, -1=positrons

  float targetVariable=0.;

  //  nRegionsEB=2; //EB- and EB+
  //  std::vector<TH1F* > vect1(nEtaBinsEB);

  // Initializate histos in EB
  std::cout << ">>> Initialize EB histos" << std::endl;
    //    std::vector<std::vector<TH1F*> >tempVect(nEtaBinsEB);
  for (int k=0; k<nEtaBinsEB; ++k)
    {
      TString histoName;
      TH1F* temp;
      //      std::cout<<i<<" "<<k<<" "<<j<<std::endl;
      for(int j = 0; j < nRegionsEB; ++j)
	{
	  
	  for(int i = 0; i < nPhiBinsEB; ++i)
	    {
	      
	      histoName= Form("EB_pData_%d_%d_%d", i,k,j);
	      temp = new TH1F (histoName, histoName, 50, 0., maximum);
	      temp->Sumw2();
	      temp->SetFillColor(kGreen+2);
	      temp->SetLineColor(kGreen+2);
	      temp->SetFillStyle(3004);
	      h_pData_EB[i][k][j] = temp;
	      //      (tempVect.at(k)).push_back(temp);
	      
	      //      histoName=Form("EB_Phi_%d_%d_%d", i,k,j);
	      //      temp = new TH1F(histoName, histoName, 360, 0., 360.); 
	      //      (h_Phi_EB.at(i)).push_back(temp); 
	    }
	  
	  //    std::cout<<"qui?"<<std::endl;
	  histoName=Form("histoPull_%d_%d", k,j);
	  temp = new TH1F(histoName, histoName, 100, -10, 10); 
	  histoPull_EB[k][j]=temp;
	  //      (h_Eta_EB.at(k)).push_back(temp); 
	  //    std::cout<<"qui2?"<<std::endl;
	}
      //    (h_pData_EB).push_back(tempVect);
      
    }


 // Template in EB
  //  std::vector<std::vector<TH1F*> > h_template_EB(nPhiBinsTempEB);
  TH1F* h_template_EB[nPhiBinsTempEB][nEtaBinsTempEB][nRegionsEB];
    
  std::cout << ">>> Initialize EB template" << std::endl;
  for(int mod = 0; mod < nPhiBinsTempEB; ++mod)
  {
  for(int k = 0; k < nEtaBinsEB; ++k)
  {
    for(int j = 0; j < nRegionsEB; ++j)
    {
      TString histoName;
      histoName=Form("EB_template_%d_%d_%d",mod,k,j);
      TH1F* temp;
	temp = new TH1F(histoName,"",50,0.,maximum);
      h_template_EB[mod][k][j] = temp;
      //      std::cout<<"mah: "<<mod<<" "<<j<<std::endl;
    }
  }  
  }  
 
  TH1F** h_phi_data_EB = new TH1F*[nRegionsEB];
  TH1F** h_eta_data_EB = new TH1F*[nRegionsEB];
  TH1F** h_phi_mc_EB = new TH1F*[nRegionsEB];

  for(int index = 0; index < nRegionsEB; ++index)
  {
    TString name;
    name=Form("EB_h_phi_data_%d",index);
    h_phi_data_EB[index] = new TH1F(name,"h_phi_data",100,-TMath::Pi(),TMath::Pi());
    name=Form("EB_h_phi_mc_%d",index);
    h_phi_mc_EB[index] =  new TH1F(name,"h_phi_mc",100,-TMath::Pi(),TMath::Pi());
    name=Form("EB_h_eta_data_%d",index);
    h_eta_data_EB[index] = new TH1F(name,"h_eta_data",100,0,1.445);
  }
  
  
  
  // fill MC templates
  
  std::vector<int> refIdEB;
  refIdEB.assign(nPhiBinsEB,0);
  
  for(int iphi = 0; iphi < nPhiBinsEB; ++iphi)
  {
    float phi = hPhiBinEB->GetBinCenter(iphi+1);
    
    phi = 2.*TMath::Pi() + phi + TMath::Pi()*10./180.;
    phi -= int(phi/2./TMath::Pi()) * 2.*TMath::Pi();
    
    int modPhi = int(phi/(2.*TMath::Pi()/nPhiBinsTempEB));
    if( modPhi == nPhiBinsTempEB ) modPhi = 0;
    refIdEB.at(iphi) = modPhi;
    //    std::cout<<iphi<<" "<<modPhi<<std::endl;
  }
    
 //**************************** loop on MC
  
  std::cout << "first loop: fill template histo" << endl; 
  
  //  for(int entry = 0; entry < mc->GetEntries(); ++entry)
  if (nEntriesMC<0) nEntriesMC = mc->GetEntries();
  for(int entry = 0; entry < nEntriesMC; ++entry)
  {
    if( entry%10000 == 0 ) 
      std::cout << "reading saved entry " << entry << "\r" << std::flush;
    
    mc->GetEntry(entry);
    //    std::cout<<fabs(scEta)<<" "<<fabs(scEta2)<<" "<<scEt<<" "<<scEt2<<std::endl;
    
    //    if( isW == 1 )               continue;
    for (int iEle=0; iEle<2; iEle++) {

      if( fabs(etaEle[iEle]) > etaMax )  continue;
      if( fabs(etaEle[iEle]) < etaMin) continue;
      if( fabs(chargeEle[iEle]) !=chargeType) continue;
      if( R9Ele[iEle] < R9cutEB) continue;
    
      float ww = 1.;
      int index=0;

      //   std::cout<<ele1_iz<<std::endl;
      // MC - ENDCAP - ele1
      if (fabs(etaEle[iEle])>etaMin && fabs(etaEle[iEle])<etaMax)
	{
	  	  // fill MC templates
	  int modPhi = int ((phiEle[iEle]+TMath::Pi())*360./(2.*TMath::Pi()));
	    //	  int modPhi = int (iphi/(360./nPhiBinsTempEB));
	  if( modPhi == nPhiBinsTempEB ) modPhi = 0;
	  
	  int EtabinEB = hEtaBinEB->FindBin(fabs(etaEle[iEle])) - 1;
	  if( EtabinEB == nEtaBinsEB ) EtabinEB = 0;
	  
	  //      int regionId =  templIndexEB(typeEB,etaEle[iEle]1,charge2,R92);
	  //      if(regionId == -1) continue;
	  
	  if (targetTypeEB==1)
	    targetVariable=pAtVtxGsfEle[iEle];
	  else if (targetTypeEB==2)
	    targetVariable=energySCEle[iEle];
	  else
	    targetVariable=energySCEle[iEle]/(pAtVtxGsfEle[iEle]);

	  //	  if (modPhi>=180) h_pData_EB[modPhi-180][EtabinEB][0] -> Fill((targetVariable),ww);  // if you want to sum the two dees
	  h_template_EB[modPhi][EtabinEB][index] ->  Fill((targetVariable),ww);
	  
	  // fill MC histos in eta bins
	  int PhibinEB = hPhiBinEB->FindBin(phiEle[iEle]) - 1;
	  if(PhibinEB==nPhiBinsEB) PhibinEB = 0;
	  
	  //      std::cout<<"MC: fill with "<<energySCEle[iEle]/(pAtVtxGsfEle[iEle]-esEnergySCEle[iEle])<<" "<<ww<<std::endl;
	  //      (h_pMC_EB.at(PhibinEB)).at(index) -> Fill(pAtVtxGsfEle[iEle],ww);  // This is MC
	  h_phi_mc_EB[index]->Fill(phiEle[iEle],ww);
	}
    }

  }
  


  
  //**************************** loop on DATA
  
  std::cout << "Loop in Data events " << endl; 

  /*  for (int i=0; i<nPhiBinsEB; ++i) {
    for (int k=0; k<nEtaBinsEB; ++k) {
      for (int j=0; j<nRegionsEB; ++j) {
	std::cout<<i<<" "<<j<<" "<<k<<std::endl;
	((h_pData_EB.at(i)).at(k)).at(j) -> Fill(0);  // This is DATA
      }
    }
  }
  */
  //  for(int entry = 0; entry < data->GetEntries(); ++entry)
  if (nEntriesData<0) nEntriesData = data->GetEntries();
  for(int entry = 0; entry < nEntriesData; ++entry)
  {
    if( entry%10000 == 0 ) 
      std::cout << "reading saved entry " << entry << "\r" << std::flush;
      
    data->GetEntry(entry);

    for (int iEle=0; iEle<2; iEle++) {
    //    if( isW == 1 )               continue;
      if( fabs(etaEle[iEle]) > etaMax )  continue;
      if( fabs(etaEle[iEle]) < etaMin) continue;
      if( fabs(chargeEle[iEle]) != chargeType) continue;
      if( R9Ele[iEle] < R9cutEB) continue;

      float ww = 1.;
      int index=0;

      //    int iz=0;
      // DATA - ENDCAP - ele1
      if (fabs(etaEle[iEle])>etaMin && fabs(etaEle[iEle])<etaMax)
	{
	  
	  int PhibinEB = int ((phiEle[iEle]+TMath::Pi())*360./(2.*TMath::Pi()));
	    //	  int PhibinEB = hPhiBinEB->FindBin(phiEle[iEle]) - 1;
	  if( PhibinEB == nPhiBinsEB ) PhibinEB = 0;
	  
	  int EtabinEB = hEtaBinEB->FindBin(fabs(etaEle[iEle])) - 1;
	  if( EtabinEB == nEtaBinsEB ) EtabinEB = 0;
	  
	  //      int regionId = templIndexEB(typeEB,etaEle[iEle],charge,R9);
	  //      if( regionId == -1 ) continue;
	  if (targetTypeEB==1)
	    targetVariable=pAtVtxGsfEle[iEle];
	  else if (targetTypeEB==2)
	    targetVariable=energySCEle[iEle];
	  else
	    targetVariable=energySCEle[iEle]/(pAtVtxGsfEle[iEle]);				      
	  
	  //	  if (PhibinEB>=180) h_pData_EB[PhibinEB-180][EtabinEB][0] -> Fill((targetVariable),ww);  // if you want to sum the two dees
	  h_pData_EB[PhibinEB][EtabinEB][index] -> Fill((targetVariable),ww);  // This is DATA
	  //      (h_Phi_EB.at(PhibinEB)).at(index) -> Fill(phiEle[iEle]); 

	  h_phi_data_EB[index] -> Fill(phiEle[iEle]);
	  h_eta_data_EB[index] -> Fill(fabs(etaEle[iEle]));

	}
    }				          
  }
  
  std::cout << "End loop: Analyze events " << endl; 
  
  
  
  
  
  
  //----------------
  // Initializations
  
  // initialize TGraphs
  TFile* o = new TFile((outputFile+"_"+typeEB+".root").c_str(),"RECREATE");
    
  TGraphErrors* g_pData_EB[nEtaBinsEB][nRegionsEB];// = new TGraphErrors**[nEtaBinsEB][nRegionsEB];
  TGraphErrors* g_pAbs_EB[nEtaBinsEB][nRegionsEB];// = new TGraphErrors**[nEtaBinsEB][nRegionsEB];

  for (int a=0; a<nEtaBinsEB; ++a)
    {
  for(int j = 0; j < nRegionsEB; ++j)
  {
    g_pData_EB[a][j]= new TGraphErrors();
    g_pAbs_EB[a][j]= new TGraphErrors();
  }
    } 
  
  // initialize template functions  
  //  std::vector<std::vector<histoFunc*> > templateHistoFuncEB(nPhiBinsTempEB);
  histoFunc* templateHistoFuncEB[nPhiBinsTempEB][nEtaBinsEB][nRegionsEB];

  for(int mod = 0; mod < nPhiBinsTempEB; ++mod)
  {
  for(int k = 0; k < nEtaBinsEB; ++k)
  {
    for(int j = 0; j < nRegionsEB; ++j)
    {
      //      h_template_EB[mod][k][j] -> Rebin(rebinEB);
      templateHistoFuncEB[mod][k][j] = new histoFunc(h_template_EB[mod][k][j]);
    }
  }
  }

  //-------------------
  // Template Fit in EB
  
  if( typeEB != "none" )
  {
    float pVector[nPhiBinsEB][nEtaBinsEB][2];
    float pVectorErr[nPhiBinsEB][nEtaBinsEB][2];

    for(int i = 0; i < nPhiBinsEB; ++i)
    {
    for(int k = 0; k < nEtaBinsEB; ++k)
    {
      for(int j = 0; j < nRegionsEB; ++j)
      {
	float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(i);
        
	//        (h_pMC_EB.at(i)).at(j) -> Rebin(rebinEB);
	//        h_pData_EB[i][k][j] -> Rebin(rebinEB);    
        
        
        // define the fitting function
        // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
        
        char funcName[50];        
        
        sprintf(funcName,"f_pData_%d_%d_%d_Ref_%d_%d_%d_EB",i,k,j,refIdEB.at(i),k,j);
	TF1* temp;
	temp = new TF1(funcName, templateHistoFuncEB[refIdEB.at(i)][k][j], 0., maximum, 3, "histoFunc");
        f_pData_EB[i][k][j] =  temp;
        
        f_pData_EB[i][k][j] -> SetParName(0,"Norm"); 
        f_pData_EB[i][k][j] -> SetParName(1,"Scale factor"); 
        
        f_pData_EB[i][k][j] -> SetLineWidth(1); 
        f_pData_EB[i][k][j] -> SetLineColor(kGreen+2); 
        f_pData_EB[i][k][j] -> SetNpx(10000);
        
	//	f_pData_EB[i][k][j] -> SetParameter(0, xNorm);
	f_pData_EB[i][k][j] -> SetParameter(0, 1.);
	f_pData_EB[i][k][j] -> SetParameter(1, 1);

	float shift=0.5;
	if (targetTypeEB!=3)
	  shift=3.;
	f_pData_EB[i][k][j] -> SetParameter(2, shift);
                               
        std::cout << "***** Fitting DATA EB " << flPhi << " (" << i << "," << j << "):   ";

	TFitResultPtr rp;
	int fStatus; 

        for(int trial = 0; trial < 300; ++trial)
        {
          rp = h_pData_EB[i][k][j] -> Fit(funcName, "QR+");
          fStatus = rp;

	  //          if( fStatus !=4 && f_pData_EB[i][k][j]->GetParError(1) != 0. && f_pData_EB[i][k][j] -> GetMaximumX(0.,500.)>30. )
          if( fStatus !=4 && f_pData_EB[i][k][j]->GetParError(1) != 0. && f_pData_EB[i][k][j] -> GetMaximumX(0.,maximum)>0. )
          {
            std::cout << "fit OK    ";
            
            double coeff = f_pData_EB[i][k][j]->GetParameter(1);
	    //            double eee = f_pData_EB[i][k][j]->GetParError(1);
	    pVector[i][k][j] = coeff;

            break;
          }
          else if( trial %5 == 0 )
          {
	    pVector[i][k][j]=-1;
	    //	    std::cout<<" BAD FIT. Make another attempt with different parameters.. "<<std::endl;
	    if (targetTypeEB!=3)
	      //	      shift-=5;
	      shift-=2.5;
	    else
	      shift-=0.05;
	    f_pData_EB[i][k][j] -> SetParameter(2, shift);
	    if (shift==-25)  shift+=50.;
	    if (shift==-0.6) shift+=1.07;
	    //	    trial = 0;
	    //	    getchar();
          }
        }

	//	std::cout<<"media del bin "<<i<<" : "<<f_pData_EB[i][k][j] -> GetMaximumX(0.,maximum)<<std::endl;
	//(f_pData_EB.at(0)).at(0)->GetParameter(2)*(f_pData_EB.at(0)).at(0)->GetParameter(1)+(h_template_EB.at(0)).at(j)->GetMean()<<std::endl;

	//	if( fStatus !=4 && f_pData_EB[i][k][j]->GetParError(1) != 0. && f_pData_EB[i][k][j] -> GetMaximumX(0.,500.)>30. ) {
	if( fStatus !=4 && f_pData_EB[i][k][j]->GetParError(1) != 0. && f_pData_EB[i][k][j] -> GetMaximumX(0.,maximum)>0. ) {
	  pVector[i][k][j] = f_pData_EB[i][k][j] -> GetMaximumX(0.,maximum);
	  pVectorErr[i][k][j] = (h_pData_EB[i][k][j] -> GetRMS())/sqrt(h_pData_EB[i][k][j] -> GetEntries());
	}
	else {
	  std::cout<<"BAD FIT!!!"<<std::endl;
	  pVector[i][k][j] = -1;  //if fit fails
	  pVectorErr[i][k][j] = 0;
	}
      }
    
  }
      
      std::cout << std::endl;
    }

	///////
    float pMean[nEtaBinsEB][nRegionsEB];
    float pMeanErr2[nEtaBinsEB][nRegionsEB];

    for(int jc = 0; jc < nRegionsEB; ++jc)
      {	
	float sum=0.;
	float sumErr2=0.;
	int n=0;
	for (int a=0; a<nEtaBinsEB; a++)
	  {
	sum=0.;
	sumErr2=0.;
	n=0;

	for (int c=0; c<nPhiBinsEB; c++)
	  {
	    if (pVector[c][a][jc]==-1) continue;
	    sum+=pVector[c][a][jc];
	    sumErr2+=(1/(pVectorErr[c][a][jc]*pVectorErr[c][a][jc]));
	    n++;  
	  }
	pMean[a][jc] = sum/(float)n;
	pMeanErr2[a][jc] = sqrt(1/sumErr2);
	std::cout<<"pMEan: "<<pMean[a][jc]<<std::endl;
	std::cout<<"pMeanErr2: "<<pMeanErr2[a][jc]<<std::endl;

	  }
      }


    for(int jc = 0; jc < nRegionsEB; ++jc)
      {	
	for (int a=0; a<nEtaBinsEB; a++)
	  {
	for (int c=0; c<nPhiBinsEB; c++)
	  {
	    float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(c);
	    int xVar=c*(int(360/nPhiBinsEB));

	    if (pVector[c][a][jc]==-1) {
	      //	      pVector[c][a][jc]=pMean[a][jc]; //if fit has failed, fill with the mean
	      pVector[c][a][jc]=pVector[c+1][a][jc]; //if fit has failed, fill with the mean
	      std::cout<<"be careful!! ("<<c<<","<<a<<","<<jc<<") has a bad value! Fill with the value from the previous point"<<std::endl;
	    }

	    if ( (pVector[c][a][jc]/pMean[a][jc])>1.15) {
	      if ( (pVector[c-1][a][jc]/pMean[a][jc])>1.15 ) {
		g_pData_EB[a][jc] -> SetPoint(c,flPhi,pVector[c+1][a][jc]/pMean[a][jc]);
		std::cout<<"caso A -iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c+1][a][jc]/pMean[a][jc]<<std::endl;
	      }
	      else {
		g_pData_EB[a][jc] -> SetPoint(c,flPhi,pVector[c-1][a][jc]/pMean[a][jc]);
		std::cout<<"caso B -iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c-1][a][jc]/pMean[a][jc]<<std::endl;
	      }
	    }
	    else if ( (pVector[c][a][jc]/pMean[a][jc])<0.85 ) {
	      if ( (pVector[c-1][a][jc]/pMean[a][jc])<0.85 ) {
		g_pData_EB[a][jc] -> SetPoint(c,flPhi,pVector[c+1][a][jc]/pMean[a][jc]);
		std::cout<<"caso C-iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c+3][a][jc]/pMean[a][jc]<<std::endl;
		std::cout<<"dentro"<<std::endl;
	      }
	      else {
		g_pData_EB[a][jc] -> SetPoint(c,flPhi,pVector[c-1][a][jc]/pMean[a][jc]);
		std::cout<<"caso D-iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c-1][a][jc]/pMean[a][jc]<<std::endl;
		std::cout<<"dentro"<<std::endl;
	      }
	    }
	    else	   {
	      g_pData_EB[a][jc] -> SetPoint(c,flPhi,pVector[c][a][jc]/pMean[a][jc]);
	      std::cout<<"caso E-iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<std::endl;
	    }
	    
	    //	    int xVar=c*(int(360/nPhiBinsEB))-90;
	    //if (xVar<0)	      xVar+=180;
	    //else if (xVar>180) xVar-=180; 
	    //	    if (xVar>=90) continue;
	    //if (xVar<0)  xVar+=180;
	    
	    //	    g_pData_EB[a][jc] -> SetPoint(c,xVar,pVector[c][a][jc]/pMean[a][jc]);
	    //	    g_pData_EB[a][jc] -> SetPoint(c,flPhi,pVector[c][a][jc]/pMean[a][jc]);	    
	    g_pAbs_EB[a][jc] -> SetPoint(c,xVar,pVector[c][a][jc]);
	    //g_pAbs_EB[a][jc] -> SetPoint(c,c*(int(360/nPhiBinsEB)),pVector[c][a][jc]);
	    histoPull_EB[a][jc] -> Fill((pVector[c][a][jc]-pMean[a][jc])/pVectorErr[c][a][jc]);
            
	    //	    float err=(pVectorErr[c][a][jc]/pMean[a][jc])*(pVectorErr[c][a][jc]/pMean[a][jc])+(pVector[c][a][jc]/(pMean[a][jc]*pMean[a][jc])*(pMeanErr2[a][jc]*pMeanErr2[a][jc]))*(pVector[c][a][jc]/(pMean[a][jc]*pMean[a][jc])*(pMeanErr2[a][jc]*pMeanErr2[a][jc]));
	    //	    float err=(pVectorErr[c][a][jc]/pMean[a][jc]);
	    //	    float err=0.;

	    //	    g_pData_EB[a][jc] -> SetPointError(c,0,err);
	    g_pData_EB[a][jc] -> SetPointError(c,0,0);
	    g_pAbs_EB[a][jc] -> SetPointError(c,0,pVectorErr[c][a][jc]);
	    //	    g_pAbs_EB[a][jc] -> SetPointError(c,0,0.);
	    //	    std::cout<<flPhi<<" "<<pVector[c][a][jc]/pMean[a][jc]<<" "<<err<<std::endl;
	  }
	  }
      }  
  ////////        


  }
  else
  {
    for(int i = 0; i < nPhiBinsEB; ++i)
    {  
      for (int a=0; a<nEtaBinsEB; ++a)
	{
      for(int j = 0; j < nRegionsEB; ++j)
      {
        float flPhi = hPhiBinEB->GetXaxis()->GetBinCenter(i+1);
        g_pData_EB[a][j] -> SetPoint(i, flPhi, 1.);
      }
	}
    }
  }
  
  
  
  std::cout<<"output"<<std::endl;
  
  
  //-------
  // Output
   
  o -> cd();
  
  for (int a=0; a<nEtaBinsEB; ++a)
    {  
  for(int j = 0; j < nRegionsEB; ++j)
  {
    TString Name;
    //Name = Form("g_pMC_EB_%d",j);
    //if(g_pMC_EB[j]->GetN()!=0) g_pMC_EB[j] -> Write(Name);
    Name = Form("g_pData_EB_%d_%d",a,j);
    if(g_pData_EB[a][j]->GetN()!=0) g_pData_EB[a][j] -> Write(Name);
    Name = Form("g_pAbs_EB_%d_%d",a,j);
    if(g_pAbs_EB[a][j]->GetN()!=0) g_pAbs_EB[a][j] -> Write(Name);
    //Name = Form("g_Rat_EB_%d",j);
    //if(g_Rat_EB[j]->GetN()!=0) g_Rat_EB[j] -> Write(Name);
  }
    }
    
  for(int j =0; j< nRegionsEB; ++j)
  {
    if( h_phi_data_EB[j] -> GetEntries() !=0 ) h_phi_data_EB[j] -> Write();
  }
  
  h_template_EB[0][0][0] -> Write();
  //  h_template_EB[0][0][1] -> Write();
  //  h_template_EB[0][1][0] -> Write();
  //  h_template_EB[0][1][1] -> Write();
  //  h_template_EB[0][2][0] -> Write();
  //  h_template_EB[0][2][1] -> Write();
  //  h_template_EB[0][4][0] -> Write();
  //  h_template_EB[0][4][1] -> Write();

  for (int k=0; k<nEtaBinsEB; ++k)
    {
      for(int j = 0; j < nRegionsEB; ++j)
	{
	  for(int i = 0; i < nPhiBinsEB; ++i)
	    {  
	      h_pData_EB[i][k][j] -> Write();
	    }	  
	}
    }

  for (int k=0; k<nEtaBinsEB; ++k)
    {
      for(int j = 0; j < nRegionsEB; ++j)
	{
	  histoPull_EB[k][j]->Write();
	}
    }

  o -> Close();
  
  
  
  return 0;
}


//////////////////////////////////Momentum correction
  if(vm.count("momentumCorrectionEE")) {	
///////// produce P corrections
  
  std::string typeEE = "eta1";
  int  nRegionsEE = 3;
  int  nPhiBinsEE = 360;
  int  nPhiBinsTempEE = 1;
  int  nEtaBinsEE = 1;
  int  nEtaBinsTempEE = 1;
  int  rebinEE = 20;
  std::string outputFile = "momentumCalibration2015_EE.root";
  int nEntriesMC = -1;
  int nEntriesData = -1;
  
  //  int nRegionsEE = GetNRegionsEE(typeEE);

  std::cout<<"REGIONI: "<<nRegionsEE<<std::endl;

  cout <<" Basic Configuration " <<endl;
  cout <<" nRegionsEE = "<<nRegionsEE<<endl;
  cout <<" nPhiBinsEE = "<<nPhiBinsEE<<endl;
  cout <<" nPhiBinsTempEE = "<<nPhiBinsTempEE<<endl;
  cout <<" nEtaBinsEE = "<<nEtaBinsEE<<endl;
  cout <<" nEtaBinsTempEE = "<<nEtaBinsTempEE<<endl;
  cout <<" rebinEE = "<<rebinEE<<endl;
  
  cout << "Momentum correction "<< endl;
  
    
  //---- variables for selection
  float etaMax  = 2.5;
  float etaMin = 1.479;

  //  nEtaBinsEE=5;

  //histos to get the bin in phi given the electron phi
  TH1F* hPhiBinEE = new TH1F("hphiEE","hphiEE",nPhiBinsEE, -1.*TMath::Pi(),1.*TMath::Pi());
  TH1F* hEtaBinEE = new TH1F("hetaEE","hetaEE",nEtaBinsEE, 1.4,2.5);
  
  //----- NTUPLES--------------------
  /*
  TChain *ntu_DA = new TChain(TreeName.c_str());  
  if(!FillChain(*ntu_DA, infileDATA.c_str())) return 1;

  TChain *ntu_MC = new TChain(TreeName.c_str());  
  if(!FillChain(*ntu_MC, infileMC.c_str())) return 1;
  */
  
  std::cout << "     DATA: " << data->GetEntries() << " entries in Data sample" << std::endl;

  // Set branch addresses
  float etaSCEle[3], phiSCEle[3], etaEle[3], phiEle[3], esEnergySCEle[3], pAtVtxGsfEle[3], energySCEle[3], R9Ele[3];
  int seedXSCEle[3], seedYSCEle[3], chargeEle[3];//, seedZside;

  int debug=0;

  std::cout<<"debug: "<<debug<<std::endl; debug++;
  
  data->SetBranchStatus("*",0);
  data->SetBranchStatus("etaSCEle",1);
  data->SetBranchStatus("phiSCEle",1);
  data->SetBranchStatus("etaEle",1);
  data->SetBranchStatus("phiEle",1);
  data->SetBranchStatus("rawEnergySCEle",1);
  data->SetBranchStatus("energySCEle",1);
  data->SetBranchStatus("esEnergySCEle",1);
  data->SetBranchStatus("pAtVtxGsfEle",1);
  data->SetBranchStatus("seedXSCEle",1);
  data->SetBranchStatus("seedYSCEle",1);
  data->SetBranchStatus("chargeEle",1);
  data->SetBranchStatus("R9Ele",1);
    
  data->SetBranchAddress("etaSCEle", &etaSCEle);
  data->SetBranchAddress("phiSCEle", &phiSCEle);
  data->SetBranchAddress("etaEle", &etaEle);
  data->SetBranchAddress("phiEle", &phiEle);
  if (energyTypeEE==0)
    data->SetBranchAddress("rawEnergySCEle", &energySCEle);
  else
    data->SetBranchAddress("energySCEle", &energySCEle);
  data->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  data->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  //data->SetBranchAddress("energySCEle", &pAtVtxGsfEle);
  data->SetBranchAddress("seedXSCEle", &seedXSCEle);
  data->SetBranchAddress("seedYSCEle", &seedYSCEle);
  data->SetBranchAddress("chargeEle", &chargeEle);
  data->SetBranchAddress("R9Ele", &R9Ele);

  std::cout<<"debug: "<<debug<<std::endl; debug++;


  mc->SetBranchStatus("*",0);
  mc->SetBranchStatus("etaSCEle",1);
  mc->SetBranchStatus("phiSCEle",1);
  mc->SetBranchStatus("etaEle",1);
  mc->SetBranchStatus("phiEle",1);
  mc->SetBranchStatus("rawEnergySCEle",1);
  mc->SetBranchStatus("energySCEle",1);
  mc->SetBranchStatus("esEnergySCEle",1);
  mc->SetBranchStatus("pAtVtxGsfEle",1);
  mc->SetBranchStatus("seedXSCEle",1);
  mc->SetBranchStatus("seedYSCEle",1);
  mc->SetBranchStatus("chargeEle",1);
  mc->SetBranchStatus("R9Ele",1);
    
  mc->SetBranchAddress("etaSCEle", &etaSCEle);
  mc->SetBranchAddress("phiSCEle", &phiSCEle);
  mc->SetBranchAddress("etaEle", &etaEle);
  mc->SetBranchAddress("phiEle", &phiEle);
  if (energyTypeEE==0)
    mc->SetBranchAddress("rawEnergySCEle", &energySCEle);
  else
    mc->SetBranchAddress("energySCEle", &energySCEle);
  mc->SetBranchAddress("esEnergySCEle", &esEnergySCEle);
  mc->SetBranchAddress("pAtVtxGsfEle", &pAtVtxGsfEle);
  //mc->SetBranchAddress("energySCEle", &pAtVtxGsfEle);
  mc->SetBranchAddress("seedXSCEle", &seedXSCEle);
  mc->SetBranchAddress("seedYSCEle", &seedYSCEle);
  mc->SetBranchAddress("chargeEle", &chargeEle);
  mc->SetBranchAddress("R9Ele", &R9Ele);

  std::cout<<"debug: "<<debug<<std::endl; debug++;

  // histogram definition in EE and fit functions                                                                      
  //  std::vector<std::vector<TH1F*> > h_Phi_EE(nPhiBinsEE); // used to map iEta (as defined for Barrel and Endcap geom) into eta          
  //  std::vector<std::vector<TH1F*> > h_Eta_EE(nEtaBinsEE); // used to map iEta (as defined for Barrel and Endcap geom) into eta          
  TH1F* h_pData_EE[nPhiBinsEE][nEtaBinsEE][nRegionsEE];
  //  std::vector<std::vector<std::vector<TH1F*> > > h_pData_EE(nPhiBinsEE);
  TF1* f_pData_EE[nPhiBinsEE][nEtaBinsEE][nRegionsEE];
  
  TH1F* histoPull_EE[nEtaBinsEE][nRegionsEE];
  std::cout<<"debug: "<<debug<<std::endl; debug++;

  float maximum=500.;
  if(targetTypeEE==3) maximum=5.;
  int chargeType=1; //1=electrons, -1=positrons

  float targetVariable=0.;

  //  nRegionsEE=2; //EE- and EE+
  //  std::vector<TH1F* > vect1(nEtaBinsEE);

  // Initializate histos in EE
  std::cout << ">>> Initialize EE histos" << std::endl;
    //    std::vector<std::vector<TH1F*> >tempVect(nEtaBinsEE);
  for (int k=0; k<nEtaBinsEE; ++k)
    {
      TString histoName;
      TH1F* temp;
      //      std::cout<<i<<" "<<k<<" "<<j<<std::endl;
      for(int j = 0; j < nRegionsEE; ++j)
	{
	  
	  for(int i = 0; i < nPhiBinsEE; ++i)
	    {
	      
	      histoName= Form("EE_pData_%d_%d_%d", i,k,j);
	      temp = new TH1F (histoName, histoName, 50, 0., maximum);
	      temp->Sumw2();
	      temp->SetFillColor(kGreen+2);
	      temp->SetLineColor(kGreen+2);
	      temp->SetFillStyle(3004);
	      h_pData_EE[i][k][j] = temp;
	      //      (tempVect.at(k)).push_back(temp);
	      
	      //      histoName=Form("EE_Phi_%d_%d_%d", i,k,j);
	      //      temp = new TH1F(histoName, histoName, 360, 0., 360.); 
	      //      (h_Phi_EE.at(i)).push_back(temp); 
	    }
	  
	  //    std::cout<<"qui?"<<std::endl;
	  histoName=Form("histoPull_%d_%d", k,j);
	  temp = new TH1F(histoName, histoName, 100, -10, 10); 
	  histoPull_EE[k][j]=temp;
	  //      (h_Eta_EE.at(k)).push_back(temp); 
	  //    std::cout<<"qui2?"<<std::endl;
	}
      //    (h_pData_EE).push_back(tempVect);
      
    }


 // Template in EE
  //  std::vector<std::vector<TH1F*> > h_template_EE(nPhiBinsTempEE);
  TH1F* h_template_EE[nPhiBinsTempEE][nEtaBinsTempEE][nRegionsEE];
    
  std::cout << ">>> Initialize EE template" << std::endl;
  for(int mod = 0; mod < nPhiBinsTempEE; ++mod)
  {
  for(int k = 0; k < nEtaBinsEE; ++k)
  {
    for(int j = 0; j < nRegionsEE; ++j)
    {
      TString histoName;
      histoName=Form("EE_template_%d_%d_%d",mod,k,j);
      TH1F* temp;
	temp = new TH1F(histoName,"",50,0.,maximum);
      h_template_EE[mod][k][j] = temp;
      //      std::cout<<"mah: "<<mod<<" "<<j<<std::endl;
    }
  }  
  }  
 
  TH1F** h_phi_data_EE = new TH1F*[nRegionsEE];
  TH1F** h_eta_data_EE = new TH1F*[nRegionsEE];
  TH1F** h_phi_mc_EE = new TH1F*[nRegionsEE];

  for(int index = 0; index < nRegionsEE; ++index)
  {
    TString name;
    name=Form("EE_h_phi_data_%d",index);
    h_phi_data_EE[index] = new TH1F(name,"h_phi_data",100,-TMath::Pi(),TMath::Pi());
    name=Form("EE_h_phi_mc_%d",index);
    h_phi_mc_EE[index] =  new TH1F(name,"h_phi_mc",100,-TMath::Pi(),TMath::Pi());
    name=Form("EE_h_eta_data_%d",index);
    h_eta_data_EE[index] = new TH1F(name,"h_eta_data",100,1.479,2.5);
  }
  
  
  // Initialize endcap geometry
  TEndcapRings *eRings = new TEndcapRings(); 
  
  // Map for conversion (ix,iy) into Eta for EE
  TH2F * mapConversionEEp = new TH2F ("mapConversionEEp","mapConversionEEp",101,1,101,101,1,101);
  TH2F * mapConversionEEm = new TH2F ("mapConversionEEm","mapConversionEEm",101,1,101,101,1,101);
  
  for(int ix =0; ix<mapConversionEEp->GetNbinsX(); ix++)
    for(int iy =0; iy<mapConversionEEp->GetNbinsY(); iy++)
    {
      mapConversionEEp->SetBinContent(ix+1,iy+1,0);
      mapConversionEEm->SetBinContent(ix+1,iy+1,0);
    }
  
  
  // fill MC templates
  
  std::vector<int> refIdEE;
  refIdEE.assign(nPhiBinsEE,0);
  
  for(int iphi = 0; iphi < nPhiBinsEE; ++iphi)
  {
    float phi = hPhiBinEE->GetBinCenter(iphi+1);
    
    phi = 2.*TMath::Pi() + phi + TMath::Pi()*10./180.;
    phi -= int(phi/2./TMath::Pi()) * 2.*TMath::Pi();
    
    int modPhi = int(phi/(2.*TMath::Pi()/nPhiBinsTempEE));
    if( modPhi == nPhiBinsTempEE ) modPhi = 0;
    refIdEE.at(iphi) = modPhi;
    //    std::cout<<iphi<<" "<<modPhi<<std::endl;
  }
    
 //**************************** loop on MC
  
  std::cout << "first loop: fill template histo" << endl; 
  
  //  for(int entry = 0; entry < mc->GetEntries(); ++entry)
  if (nEntriesMC<0) nEntriesMC = mc->GetEntries();
  for(int entry = 0; entry < nEntriesMC; ++entry)
  {
    if( entry%10000 == 0 ) 
      std::cout << "reading saved entry " << entry << "\r" << std::flush;
    
    mc->GetEntry(entry);
    //    std::cout<<fabs(scEta)<<" "<<fabs(scEta2)<<" "<<scEt<<" "<<scEt2<<std::endl;
    
    //    if( isW == 1 )               continue;
    for (int iEle=0; iEle<2; iEle++) {

      if( fabs(etaEle[iEle]) > etaMax )  continue;
      if( fabs(etaEle[iEle]) < etaMin) continue;
      if( fabs(chargeEle[iEle]) !=chargeType) continue;
      if( R9Ele[iEle] < R9cutEE) continue;
    
      float ww = 1.;
      int index=0;

      int iz=0;
      //   std::cout<<ele1_iz<<std::endl;
      // MC - ENDCAP - ele1
      if (fabs(etaEle[iEle])>etaMin && fabs(etaEle[iEle])<etaMax)
	{
	  if( etaEle[iEle]>0 ) { mapConversionEEp -> SetBinContent(seedXSCEle[iEle],seedYSCEle[iEle],etaEle[iEle]); iz=1; index=0; }
	  if( etaEle[iEle]<0 ) {mapConversionEEm -> SetBinContent(seedXSCEle[iEle],seedYSCEle[iEle],etaEle[iEle]); iz=-1; index=1; }
	  
	  int iphi = eRings->GetEndcapIphi(seedXSCEle[iEle],seedYSCEle[iEle],iz);
	  
	  // fill MC templates
	  int modPhi = int (iphi/(360./nPhiBinsTempEE));
	  if( modPhi == nPhiBinsTempEE ) modPhi = 0;
	  
	  int EtabinEE = hEtaBinEE->FindBin(fabs(etaEle[iEle])) - 1;
	  if( EtabinEE == nEtaBinsEE ) EtabinEE = 0;
	  
	  //      int regionId =  templIndexEE(typeEE,etaEle[iEle]1,charge2,R92);
	  //      if(regionId == -1) continue;
	  
	  if (targetTypeEE==1)
	    targetVariable=pAtVtxGsfEle[iEle];
	  else if (targetTypeEE==2)
	    targetVariable=energySCEle[iEle];
	  else
	    targetVariable=energySCEle[iEle]/(pAtVtxGsfEle[iEle]-esEnergySCEle[iEle]);

	  //	  if (index==1)	    h_template_EE[modPhi][EtabinEE][0] ->  Fill((targetVariable),ww); //if you want to sum EE+ and EE-
	  //	  if (modPhi>=180) h_pData_EE[modPhi-180][EtabinEE][0] -> Fill((targetVariable),ww);  // if you want to sum the two dees
	  h_template_EE[modPhi][EtabinEE][index] ->  Fill((targetVariable),ww);
	  h_template_EE[modPhi][EtabinEE][2] ->  Fill((targetVariable),ww); //this is the sum of EE+ and EE-
	  
	  // fill MC histos in eta bins
	  int PhibinEE = hPhiBinEE->FindBin(phiEle[iEle]) - 1;
	  if(PhibinEE==nPhiBinsEE) PhibinEE = 0;
	  
	  //      std::cout<<"MC: fill with "<<energySCEle[iEle]/(pAtVtxGsfEle[iEle]-esEnergySCEle[iEle])<<" "<<ww<<std::endl;
	  //      (h_pMC_EE.at(PhibinEE)).at(index) -> Fill(pAtVtxGsfEle[iEle],ww);  // This is MC
	  h_phi_mc_EE[index]->Fill(phiEle[iEle],ww);
	  h_phi_mc_EE[2]->Fill(phiEle[iEle],ww);
	}
    }

  }
  


  
  //**************************** loop on DATA
  
  std::cout << "Loop in Data events " << endl; 

  /*  for (int i=0; i<nPhiBinsEE; ++i) {
    for (int k=0; k<nEtaBinsEE; ++k) {
      for (int j=0; j<nRegionsEE; ++j) {
	std::cout<<i<<" "<<j<<" "<<k<<std::endl;
	((h_pData_EE.at(i)).at(k)).at(j) -> Fill(0);  // This is DATA
      }
    }
  }
  */
  //  for(int entry = 0; entry < data->GetEntries(); ++entry)
  if (nEntriesData<0) nEntriesData = data->GetEntries();
  for(int entry = 0; entry < nEntriesData; ++entry)
  {
    if( entry%10000 == 0 ) 
      std::cout << "reading saved entry " << entry << "\r" << std::flush;
      
    data->GetEntry(entry);

    for (int iEle=0; iEle<2; iEle++) {
    //    if( isW == 1 )               continue;
      if( fabs(etaEle[iEle]) > etaMax )  continue;
      if( fabs(etaEle[iEle]) < etaMin) continue;
      if( fabs(chargeEle[iEle]) != chargeType) continue;
      if( R9Ele[iEle] < R9cutEE) continue;

      float ww = 1.;
      int index=0;

      //    int iz=0;
      // DATA - ENDCAP - ele1
      if (fabs(etaEle[iEle])>etaMin && fabs(etaEle[iEle])<etaMax)
	{
	  if( etaEle[iEle]>0 ) { mapConversionEEp -> SetBinContent(seedXSCEle[iEle],seedYSCEle[iEle],etaEle[iEle]); index=0; }
	  if( etaEle[iEle]<0 ) { mapConversionEEm -> SetBinContent(seedXSCEle[iEle],seedYSCEle[iEle],etaEle[iEle]); index=1; }
	  
	  int PhibinEE = hPhiBinEE->FindBin(phiEle[iEle]) - 1;
	  if( PhibinEE == nPhiBinsEE ) PhibinEE = 0;
	  
	  int EtabinEE = hEtaBinEE->FindBin(fabs(etaEle[iEle])) - 1;
	  if( EtabinEE == nEtaBinsEE ) EtabinEE = 0;
	  
	  //      int regionId = templIndexEE(typeEE,etaEle[iEle],charge,R9);
	  //      if( regionId == -1 ) continue;
	  if (targetTypeEE==1)
	    targetVariable=pAtVtxGsfEle[iEle];
	  else if (targetTypeEE==2)
	    targetVariable=energySCEle[iEle];
	  else
	    targetVariable=energySCEle[iEle]/(pAtVtxGsfEle[iEle]-esEnergySCEle[iEle]);				      
	  
	  //	  if (PhibinEE>=180) h_pData_EE[PhibinEE-180][EtabinEE][0] -> Fill((targetVariable),ww);  // if you want to sum the two dees
	  h_pData_EE[PhibinEE][EtabinEE][index] -> Fill((targetVariable),ww);  // This is DATA
	  //      (h_Phi_EE.at(PhibinEE)).at(index) -> Fill(phiEle[iEle]); 
	  h_pData_EE[PhibinEE][EtabinEE][2] -> Fill((targetVariable),ww);  // if you want to sum EE+ and EE-

	  h_phi_data_EE[index] -> Fill(phiEle[iEle]);
	  h_eta_data_EE[index] -> Fill(fabs(etaEle[iEle]));
	  h_phi_data_EE[2] -> Fill(phiEle[iEle]);
	  h_eta_data_EE[2] -> Fill(fabs(etaEle[iEle]));

	}
    }				          
  }
  
  std::cout << "End loop: Analyze events " << endl; 
  
  
  
  
  
  
  //----------------
  // Initializations
  
  // initialize TGraphs
  TFile* o = new TFile((outputFile+"_"+typeEE+".root").c_str(),"RECREATE");
    
  TGraphErrors* g_pData_EE[nEtaBinsEE][nRegionsEE];// = new TGraphErrors**[nEtaBinsEE][nRegionsEE];
  TGraphErrors* g_pAbs_EE[nEtaBinsEE][nRegionsEE];// = new TGraphErrors**[nEtaBinsEE][nRegionsEE];

  for (int a=0; a<nEtaBinsEE; ++a)
    {
  for(int j = 0; j < nRegionsEE; ++j)
  {
    g_pData_EE[a][j]= new TGraphErrors();
    g_pAbs_EE[a][j]= new TGraphErrors();
  }
    } 
  
  // initialize template functions  
  //  std::vector<std::vector<histoFunc*> > templateHistoFuncEE(nPhiBinsTempEE);
  histoFunc* templateHistoFuncEE[nPhiBinsTempEE][nEtaBinsEE][nRegionsEE];

  for(int mod = 0; mod < nPhiBinsTempEE; ++mod)
  {
  for(int k = 0; k < nEtaBinsEE; ++k)
  {
    for(int j = 0; j < nRegionsEE; ++j)
    {
      //      h_template_EE[mod][k][j] -> Rebin(rebinEE);
      templateHistoFuncEE[mod][k][j] = new histoFunc(h_template_EE[mod][k][j]);
    }
  }
  }

  //-------------------
  // Template Fit in EE
  
  if( typeEE != "none" )
  {
    float pVector[nPhiBinsEE][nEtaBinsEE][nRegionsEE];
    float pVectorErr[nPhiBinsEE][nEtaBinsEE][nRegionsEE];

    for(int i = 0; i < nPhiBinsEE; ++i)
    {
    for(int k = 0; k < nEtaBinsEE; ++k)
    {
      for(int j = 0; j < nRegionsEE; ++j)
      {
	float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(i);
        
	//        (h_pMC_EE.at(i)).at(j) -> Rebin(rebinEE);
	//        h_pData_EE[i][k][j] -> Rebin(rebinEE);    
        
        
        // define the fitting function
        // N.B. [0] * ( [1] * f( [1]*(x-[2]) ) )
        
        char funcName[50];        
        
        sprintf(funcName,"f_pData_%d_%d_%d_Ref_%d_%d_%d_EE",i,k,j,refIdEE.at(i),k,j);
	TF1* temp;
	temp = new TF1(funcName, templateHistoFuncEE[refIdEE.at(i)][k][j], 0., maximum, 3, "histoFunc");
        f_pData_EE[i][k][j] =  temp;
        
        f_pData_EE[i][k][j] -> SetParName(0,"Norm"); 
        f_pData_EE[i][k][j] -> SetParName(1,"Scale factor"); 
        
        f_pData_EE[i][k][j] -> SetLineWidth(1); 
        f_pData_EE[i][k][j] -> SetLineColor(kGreen+2); 
        f_pData_EE[i][k][j] -> SetNpx(10000);
        
	//	f_pData_EE[i][k][j] -> SetParameter(0, xNorm);
	f_pData_EE[i][k][j] -> SetParameter(0, 1.);
	f_pData_EE[i][k][j] -> SetParameter(1, 1);

	float shift=0.5;
	if (targetTypeEE!=3)
	  shift=3.;
	f_pData_EE[i][k][j] -> SetParameter(2, shift);
                               
        std::cout << "***** Fitting DATA EE " << flPhi << " (" << i << "," << j << "):   ";

	TFitResultPtr rp;
	int fStatus; 

        for(int trial = 0; trial < 300; ++trial)
        {
          rp = h_pData_EE[i][k][j] -> Fit(funcName, "QR+");
          fStatus = rp;

	  //          if( fStatus !=4 && f_pData_EE[i][k][j]->GetParError(1) != 0. && f_pData_EE[i][k][j] -> GetMaximumX(0.,500.)>30. )
          if( fStatus !=4 && f_pData_EE[i][k][j]->GetParError(1) != 0. && f_pData_EE[i][k][j] -> GetMaximumX(0.,maximum)>0. )
          {
            std::cout << "fit OK    ";
            
            double coeff = f_pData_EE[i][k][j]->GetParameter(1);
	    //            double eee = f_pData_EE[i][k][j]->GetParError(1);
	    pVector[i][k][j] = coeff;

            break;
          }
          else if( trial %5 == 0 )
          {
	    pVector[i][k][j]=-1;
	    //	    std::cout<<" BAD FIT. Make another attempt with different parameters.. "<<std::endl;
	    if (targetTypeEE!=3)
	      //	      shift-=5;
	      shift-=2.5;
	    else
	      shift-=0.05;
	    f_pData_EE[i][k][j] -> SetParameter(2, shift);
	    if (shift==-25)  shift+=50.;
	    if (shift==-0.6) shift+=1.07;
	    //	    trial = 0;
	    //	    getchar();
          }
        }

	//	std::cout<<"media del bin "<<i<<" : "<<f_pData_EE[i][k][j] -> GetMaximumX(0.,maximum)<<std::endl;
	//(f_pData_EE.at(0)).at(0)->GetParameter(2)*(f_pData_EE.at(0)).at(0)->GetParameter(1)+(h_template_EE.at(0)).at(j)->GetMean()<<std::endl;

	//	if( fStatus !=4 && f_pData_EE[i][k][j]->GetParError(1) != 0. && f_pData_EE[i][k][j] -> GetMaximumX(0.,500.)>30. ) {
	if( fStatus !=4 && f_pData_EE[i][k][j]->GetParError(1) != 0. && f_pData_EE[i][k][j] -> GetMaximumX(0.,maximum)>0. ) {
	  pVector[i][k][j] = f_pData_EE[i][k][j] -> GetMaximumX(0.,maximum);
	  pVectorErr[i][k][j] = (h_pData_EE[i][k][j] -> GetRMS())/sqrt(h_pData_EE[i][k][j] -> GetEntries());
	}
	else {
	  std::cout<<"BAD FIT!!!"<<std::endl;
	  pVector[i][k][j] = -1;  //if fit fails
	  pVectorErr[i][k][j] = 0;
	}
      }
    
  }
      
      std::cout << std::endl;
    }

	///////
    float pMean[nEtaBinsEE][nRegionsEE];
    float pMeanErr2[nEtaBinsEE][nRegionsEE];

    for(int jc = 0; jc < nRegionsEE; ++jc)
      {	
	float sum=0.;
	float sumErr2=0.;
	int n=0;
	for (int a=0; a<nEtaBinsEE; a++)
	  {
	sum=0.;
	sumErr2=0.;
	n=0;

	for (int c=0; c<nPhiBinsEE; c++)
	  {
	    if (pVector[c][a][jc]==-1) continue;
	    sum+=pVector[c][a][jc];
	    sumErr2+=(1/(pVectorErr[c][a][jc]*pVectorErr[c][a][jc]));
	    n++;  
	  }
	pMean[a][jc] = sum/(float)n;
	pMeanErr2[a][jc] = sqrt(1/sumErr2);
	std::cout<<"pMEan: "<<pMean[a][jc]<<std::endl;
	std::cout<<"pMeanErr2: "<<pMeanErr2[a][jc]<<std::endl;

	  }
      }


    for(int jc = 0; jc < nRegionsEE; ++jc)
      {	
	for (int a=0; a<nEtaBinsEE; a++)
	  {
	for (int c=0; c<nPhiBinsEE; c++)
	  {
	    float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(c);
	    int xVar=c*(int(360/nPhiBinsEE));

	    if (pVector[c][a][jc]==-1) {
	      //	      pVector[c][a][jc]=pMean[a][jc]; //if fit has failed, fill with the mean
	      pVector[c][a][jc]=pVector[c+1][a][jc]; //if fit has failed, fill with the mean
	      std::cout<<"be careful!! ("<<c<<","<<a<<","<<jc<<") has a bad value! Fill with the value from the previous point"<<std::endl;
	    }

	    if ( (pVector[c][a][jc]/pMean[a][jc])>1.15) {
	      if ( (pVector[c-1][a][jc]/pMean[a][jc])>1.15 ) {
		g_pData_EE[a][jc] -> SetPoint(c,flPhi,pVector[c+1][a][jc]/pMean[a][jc]);
		std::cout<<"caso A -iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c+1][a][jc]/pMean[a][jc]<<std::endl;
	      }
	      else {
		g_pData_EE[a][jc] -> SetPoint(c,flPhi,pVector[c-1][a][jc]/pMean[a][jc]);
		std::cout<<"caso B -iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c-1][a][jc]/pMean[a][jc]<<std::endl;
	      }
	    }
	    else if ( (pVector[c][a][jc]/pMean[a][jc])<0.85 ) {
	      if ( (pVector[c-1][a][jc]/pMean[a][jc])<0.85 ) {
		g_pData_EE[a][jc] -> SetPoint(c,flPhi,pVector[c+1][a][jc]/pMean[a][jc]);
		std::cout<<"caso C-iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c+3][a][jc]/pMean[a][jc]<<std::endl;
		std::cout<<"dentro"<<std::endl;
	      }
	      else {
		g_pData_EE[a][jc] -> SetPoint(c,flPhi,pVector[c-1][a][jc]/pMean[a][jc]);
		std::cout<<"caso D-iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<" corr: "<<pVector[c-1][a][jc]/pMean[a][jc]<<std::endl;
		std::cout<<"dentro"<<std::endl;
	      }
	    }
	    else	   {
	      g_pData_EE[a][jc] -> SetPoint(c,flPhi,pVector[c][a][jc]/pMean[a][jc]);
	      std::cout<<"caso E-iphi: "<<xVar<<" phi: "<<flPhi<<" side: "<<jc<<" value: "<<pVector[c][a][jc]/pMean[a][jc]<<" abs: "<<pVector[c][a][jc]<<std::endl;
	    }
	    
	    //	    int xVar=c*(int(360/nPhiBinsEE))-90;
	    //if (xVar<0)	      xVar+=180;
	    //else if (xVar>180) xVar-=180; 
	    //	    if (xVar>=90) continue;
	    //if (xVar<0)  xVar+=180;
	    
	    //	    g_pData_EE[a][jc] -> SetPoint(c,xVar,pVector[c][a][jc]/pMean[a][jc]);
	    //	    g_pData_EE[a][jc] -> SetPoint(c,flPhi,pVector[c][a][jc]/pMean[a][jc]);	    
	    g_pAbs_EE[a][jc] -> SetPoint(c,xVar,pVector[c][a][jc]);
	    //g_pAbs_EE[a][jc] -> SetPoint(c,c*(int(360/nPhiBinsEE)),pVector[c][a][jc]);
	    histoPull_EE[a][jc] -> Fill((pVector[c][a][jc]-pMean[a][jc])/pVectorErr[c][a][jc]);
            
	    //	    float err=(pVectorErr[c][a][jc]/pMean[a][jc])*(pVectorErr[c][a][jc]/pMean[a][jc])+(pVector[c][a][jc]/(pMean[a][jc]*pMean[a][jc])*(pMeanErr2[a][jc]*pMeanErr2[a][jc]))*(pVector[c][a][jc]/(pMean[a][jc]*pMean[a][jc])*(pMeanErr2[a][jc]*pMeanErr2[a][jc]));
	    //	    float err=(pVectorErr[c][a][jc]/pMean[a][jc]);
	    //	    float err=0.;

	    //	    g_pData_EE[a][jc] -> SetPointError(c,0,err);
	    g_pData_EE[a][jc] -> SetPointError(c,0,0);
	    g_pAbs_EE[a][jc] -> SetPointError(c,0,pVectorErr[c][a][jc]);
	    //	    g_pAbs_EE[a][jc] -> SetPointError(c,0,0.);
	    //	    std::cout<<flPhi<<" "<<pVector[c][a][jc]/pMean[a][jc]<<" "<<err<<std::endl;
	  }
	  }
      }  
  ////////        


  }
  else
  {
    for(int i = 0; i < nPhiBinsEE; ++i)
    {  
      for (int a=0; a<nEtaBinsEE; ++a)
	{
      for(int j = 0; j < nRegionsEE; ++j)
      {
        float flPhi = hPhiBinEE->GetXaxis()->GetBinCenter(i+1);
        g_pData_EE[a][j] -> SetPoint(i, flPhi, 1.);
      }
	}
    }
  }
  
  
  
  
  
  
  //-------
  // Output
   
  o -> cd();
  
  for (int a=0; a<nEtaBinsEE; ++a)
    {  
  for(int j = 0; j < nRegionsEE; ++j)
  {
    TString Name;
    //Name = Form("g_pMC_EE_%d",j);
    //if(g_pMC_EE[j]->GetN()!=0) g_pMC_EE[j] -> Write(Name);
    Name = Form("g_pData_EE_%d_%d",a,j);
    if(g_pData_EE[a][j]->GetN()!=0) g_pData_EE[a][j] -> Write(Name);
    Name = Form("g_pAbs_EE_%d_%d",a,j);
    if(g_pAbs_EE[a][j]->GetN()!=0) g_pAbs_EE[a][j] -> Write(Name);
    //Name = Form("g_Rat_EE_%d",j);
    //if(g_Rat_EE[j]->GetN()!=0) g_Rat_EE[j] -> Write(Name);
  }
    }
    
  for(int j =0; j< nRegionsEE; ++j)
  {
    if( h_phi_data_EE[j] -> GetEntries() !=0 ) h_phi_data_EE[j] -> Write();
  }
  
  mapConversionEEp -> Write();
  mapConversionEEm -> Write();

  h_template_EE[0][0][0] -> Write();
  h_template_EE[0][0][1] -> Write();
  h_template_EE[0][0][2] -> Write();
  //  h_template_EE[0][1][0] -> Write();
  //  h_template_EE[0][1][1] -> Write();
  //  h_template_EE[0][2][0] -> Write();
  //  h_template_EE[0][2][1] -> Write();
  //  h_template_EE[0][4][0] -> Write();
  //  h_template_EE[0][4][1] -> Write();

  for (int k=0; k<nEtaBinsEE; ++k)
    {
      for(int j = 0; j < nRegionsEE; ++j)
	{
	  for(int i = 0; i < nPhiBinsEE; ++i)
	    {  
	      h_pData_EE[i][k][j] -> Write();
	    }	  
	}
    }

  for (int k=0; k<nEtaBinsEE; ++k)
    {
      for(int j = 0; j < nRegionsEE; ++j)
	{
	  histoPull_EE[k][j]->Write();
	}
    }

  o -> Close();
  
  
  
  return 0;
}

///////////////////////LUCA

  if(vm.count("EOverPCalib") && vm.count("doEB")) {	

    std::cout<<"---- START E/P CALIBRATION: BARREL ----"<<std::endl;
  /// open ntupla of data or MC
 // TChain * tree = new TChain (inputTree.c_str());
 // FillChain(*tree,inputList);
   int nRegionsEB = GetNRegionsEB(typeEB);
 
   std::map<int, std::vector<std::pair<int, int> > > jsonMap;
   jsonMap = readJSONFile(jsonFileName); 
   //   std::cout<<"JSON file: "<<jsonFileName<<std::endl;

   std::cout<<"Output Directory: "<<outputPath<<std::endl;
   system(("mkdir -p "+outputPath).c_str());
  
   /// open calibration momentum graph
   TFile* momentumscale = new TFile((inputMomentumScale.c_str()));//+"_"+typeEB+"_"+typeEE+".root").c_str());
   std::vector<TGraphErrors*> g_EoC_EB;
  
   //   for(int i = 0; i < nRegionsEB; ++i){
   //    TString Name = Form("g_pData_EB_0_%d",i);
    TString Name = Form("g_EoC_EB_0");
    g_EoC_EB.push_back( (TGraphErrors*)(momentumscale->Get(Name)) );
    //   }

   std::cout<<"momentum calibration file correctly opened"<<std::endl;

   /// open calibration energy graph
   TFile* energyscale = new TFile((inputEnergyScale.c_str()));//+"_"+typeEB+"_"+typeEE+".root").c_str());
   std::vector<TGraphErrors*> g_EoE_EB;
  
   for(int i = 0; i < nRegionsEB; ++i){
    TString Name = Form("g_pData_EB_0_%d",i);
    g_EoE_EB.push_back( (TGraphErrors*)(energyscale->Get(Name)) );
   }

   std::cout<<"energy calibration file correctly opened"<<std::endl;

  ///Use the whole sample statistics if numberOfEvents < 0
   if ( numberOfEvents < 0 ) numberOfEvents = data->GetEntries(); 

   std::cout<<"number of events: "<<numberOfEvents<<std::endl;

  /// run in normal mode: full statistics
   if ( splitStat == 0 ) {
   
    TString name ;
    TString name_tmp;

    if(isMiscalib == true && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_R9_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_Fbrem_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true ) 
     name_tmp = Form ("%s_WZ_PT_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_EP_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  ) 
     name_tmp =Form ("%s_WZ_noEP_miscalib_EB",outputFile.c_str());

    else if(isMiscalib == false && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_R9_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_Fbrem_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true ) 
     name_tmp = Form ("%s_WZ_PT_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_EP_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  ) 
     name_tmp =Form ("%s_WZ_noEP_EB",outputFile.c_str());

    else if(isMiscalib == true && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_W_R9_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false ) 
     name_tmp = Form ("%s_W_Fbrem_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true ) 
     name_tmp = Form ("%s_W_PT_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_W_EP_miscalib_EB",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  ) 
     name_tmp =Form ("%s_W_noEP_miscalib_EB",outputFile.c_str());

    else if(isMiscalib == false && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_WZ_R9_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false ) 
     name_tmp = Form ("%s_W_Fbrem_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true ) 
     name_tmp = Form ("%s_W_PT_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false ) 
     name_tmp = Form ("%s_W_EP_EB",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  ) 
     name_tmp =Form ("%s_W_noEP_EB",outputFile.c_str());
    else { std::cout<<" Option not considered --> exit "<<std::endl; return -1 ;}

    name = Form("%s%s.root",outputPath.c_str(),name_tmp.Data());
    TFile *outputName = new TFile(name,"RECREATE");
    
    TString outEPDistribution = "Weight_"+name;
    
    TString DeadXtal = Form("%s",inputFileDeadXtal.c_str());    

    std::cout<<"start calibration. "<<std::endl;

    if(isSaveEPDistribution == true){
      FastCalibratorEB analyzer(data, g_EoC_EB, g_EoE_EB, typeEB, outEPDistribution);
      analyzer.bookHistos(nLoops);
      analyzer.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
      analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, applyPcorr, applyEcorr, useRawEnergy, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
      analyzer.saveHistos(outputName);
    }
    else{
      std::cout<<"create FastCalibratorEB object "<<std::endl;
      FastCalibratorEB analyzer(data, g_EoC_EB, g_EoE_EB, typeEB);
      std::cout<<"book histos "<<std::endl;
      analyzer.bookHistos(nLoops);
      std::cout<<"acquire dead xtal "<<std::endl;
      analyzer.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
      std::cout<<"start loop "<<std::endl;
      analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, applyPcorr, applyEcorr, useRawEnergy, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
      analyzer.saveHistos(outputName);
    }
    
   }
  
   /// run in even-odd mode: half statistics
   else if ( splitStat == 1 ) {

    
    /// Prepare the outputs
    TString name;
    TString name2;

    if(isMiscalib == true && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_R9_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_R9_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_EP_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_EP_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==true && isPtCut==false){ 
      name  = Form ("%s_WZ_Fbrem_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_Fbrem_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_WZ_PT_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_PT_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_noEP_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_noEP_miscalib_EB_odd.root", outputFile.c_str());
    }



    else if(isMiscalib == false && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_R9_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_R9_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_EP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_EP_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_WZ_Fbrem_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_Fbrem_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_WZ_PT_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_PT_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_noEP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_noEP_EB_odd.root", outputFile.c_str());
    }
    

    else if(isMiscalib == true && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_R9_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_R9_miscalib_EB_odd.root", outputFile.c_str());
    }
    
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_EP_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_EP_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_W_Fbrem_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_Fbrem_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_W_PT_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_PT_miscalib_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_noEP_miscalib_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_noEP_miscalib_EB_odd.root", outputFile.c_str());
    }

    else if(isMiscalib == false && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_R9_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_R9_EB_odd.root", outputFile.c_str());
    }

    
    else if(isMiscalib == false && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_EP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_EP_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
       name  = Form ("%s_W_EP_EB_even.root",outputFile.c_str());
       name2 = Form ("%s_W_EP_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_W_Fbrem_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_Fbrem_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_W_PT_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_PT_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_noEP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_noEP_EB_odd.root", outputFile.c_str());
    }
    else { std::cout<<" Option not considered --> exit "<<std::endl; return -1 ;}

    TFile *outputName1 = new TFile(outputPath+name,"RECREATE");
    TFile *outputName2 = new TFile(outputPath+name2,"RECREATE");

    TString DeadXtal = Form("%s",inputFileDeadXtal.c_str());

    /// Run on odd
    FastCalibratorEB analyzer_even(data, g_EoC_EB, g_EoE_EB, typeEB);
    analyzer_even.bookHistos(nLoops);
    analyzer_even.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
    analyzer_even.Loop(numberOfEvents, useZ, useW, splitStat, nLoops,applyPcorr, applyEcorr, useRawEnergy, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
    analyzer_even.saveHistos(outputName1);
  
    /// Run on even
    FastCalibratorEB analyzer_odd(data, g_EoC_EB, g_EoE_EB, typeEB);
    analyzer_odd.bookHistos(nLoops);
    analyzer_odd.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
    analyzer_odd.Loop(numberOfEvents, useZ, useW, splitStat*(-1), nLoops,applyPcorr, applyEcorr, useRawEnergy, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
    analyzer_odd.saveHistos(outputName2);
    
   }	

   return 0;
//E/P calibration done!
  }


  if(vm.count("EOverPCalib") && vm.count("doEE")){
///////// E/P calibration

    std::cout<<"---- START E/P CALIBRATION: ENDCAP ----"<<std::endl;
  /// open ntupla of data or MC
 // TChain * tree = new TChain (inputTree.c_str());
 // FillChain(*tree,inputList);
   int nRegionsEE = GetNRegionsEE(typeEE);
 
   std::map<int, std::vector<std::pair<int, int> > > jsonMap;
   jsonMap = readJSONFile(jsonFileName); 
   std::cout<<"JSON file: "<<jsonFileName<<std::endl;

   std::cout<<"Output Directory: "<<outputPath<<std::endl;
   system(("mkdir -p "+outputPath).c_str());

  /// open calibration momentum graph
   TFile* f4 = new TFile((inputMomentumScale.c_str()));//+"_"+typeEB+"_"+typeEE+".root").c_str());
  std::vector<TGraphErrors*> g_EoC_EE;

  //  for(int i = 0; i < nRegionsEE; ++i){
    //    TString Name = Form("g_pData_EE_0_%d",i);
    TString Name = Form("g_EoC_EE_0");
    g_EoC_EE.push_back( (TGraphErrors*)(f4->Get(Name)) );
    //  }

   /// open calibration energy graph
   TFile* f5 = new TFile((inputEnergyScale.c_str()));//+"_"+typeEB+"_"+typeEE+".root").c_str());
   std::vector<TGraphErrors*> g_EoE_EE;
  
   for(int i = 0; i < nRegionsEE; ++i){
    TString Name = Form("g_pData_EE_0_%d",i);
    g_EoE_EE.push_back( (TGraphErrors*)(f5->Get(Name)) );
   }
  
  ///Use the whole sample statistics if numberOfEvents < 0
  if ( numberOfEvents < 0 ) numberOfEvents = data->GetEntries(); 
  

  /// run in normal mode: full statistics
  if ( splitStat == 0 ) {
   
    TString name ;
    TString name_tmp;

    if(isMiscalib == true && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_WZ_R9_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false )
      name_tmp = Form ("%s_WZ_Fbrem_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true )
      name_tmp = Form ("%s_WZ_PT_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_WZ_EP_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 1 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  )
      name_tmp =Form ("%s_WZ_noEP_miscalib_EE",outputFile.c_str());

    else if(isMiscalib == false && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_WZ_R9_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false )
      name_tmp = Form ("%s_WZ_Fbrem_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true )
      name_tmp = Form ("%s_WZ_PT_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_WZ_EP_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 1 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  )
      name_tmp =Form ("%s_WZ_noEP_EE",outputFile.c_str());

    

    else if(isMiscalib == true && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_W_R9_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false )
      name_tmp = Form ("%s_W_Fbrem_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true )
      name_tmp = Form ("%s_W_PT_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_W_EP_miscalib_EE",outputFile.c_str());
    else if(isMiscalib == true && useZ == 0 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  )
      name_tmp =Form ("%s_W_noEP_miscalib_EE",outputFile.c_str());

    else if(isMiscalib == false && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_WZ_R9_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isR9selection ==false && isEPselection == false && isfbrem == true && isPtCut ==false )
      name_tmp = Form ("%s_W_Fbrem_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isR9selection ==true && isEPselection == false && isfbrem == false && isPtCut ==true )
      name_tmp = Form ("%s_W_PT_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isR9selection ==true && isEPselection == true && isfbrem == false && isPtCut ==false )
      name_tmp = Form ("%s_W_EP_EE",outputFile.c_str());
    else if(isMiscalib == false && useZ == 0 && isEPselection ==false && isR9selection==false && isPtCut ==false && isfbrem ==false  )
      name_tmp =Form ("%s_W_noEP_EE",outputFile.c_str());
    else { std::cout<<" Option not considered --> exit "<<std::endl; return -1 ;}

    name = Form("%s%s.root",outputPath.c_str(),name_tmp.Data());
    TFile *f1 = new TFile(name,"RECREATE");

    TString outEPDistribution = "Weight_"+name;

    TString DeadXtal = Form("%s",inputFileDeadXtal.c_str());    

    
    if(isSaveEPDistribution == true){
      FastCalibratorEE analyzer(data, g_EoC_EE, g_EoE_EE, typeEE, outEPDistribution);
      analyzer.bookHistos(nLoops);
      analyzer.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
      analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, applyPcorr, applyEcorr, useRawEnergy, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
      analyzer.saveHistos(f1);
    }
    else{
      FastCalibratorEE analyzer(data, g_EoC_EE, g_EoE_EE, typeEE);
      analyzer.bookHistos(nLoops);
      analyzer.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);  
      analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, applyPcorr, applyEcorr, useRawEnergy,isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
      analyzer.saveHistos(f1);
    }
   
  }

  /// run in even-odd mode: half statistics
  else if ( splitStat == 1 ) {
    
    /// Prepare the outputs
    TString name;
    TString name2;

    if(isMiscalib == true && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_R9_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_R9_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_EP_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_EP_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_WZ_Fbrem_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_Fbrem_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_WZ_PT_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_PT_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_noEP_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_noEP_miscalib_EE_odd.root", outputFile.c_str());
    }


    else if(isMiscalib == false && useZ == 1 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_R9_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_R9_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_EP_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_EP_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_WZ_Fbrem_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_Fbrem_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_WZ_PT_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_PT_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_WZ_noEP_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_WZ_noEP_EE_odd.root", outputFile.c_str());
    }


    else if(isMiscalib == true && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_R9_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_W_R9_miscalib_EE_odd.root", outputFile.c_str());
    }

    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_EP_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_W_EP_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_W_Fbrem_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_W_Fbrem_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_W_PT_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_W_PT_miscalib_EE_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_noEP_miscalib_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_W_noEP_miscalib_EE_odd.root", outputFile.c_str());
    }

    else if(isMiscalib == false && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_R9_EE_even.root",outputFile.c_str());
      name2 = Form ("%s_W_R9_EE_odd.root", outputFile.c_str());
    }


    else if(isMiscalib == false && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_EP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_EP_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==true && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_EP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_EP_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==true && isPtCut==false){
      name  = Form ("%s_W_Fbrem_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_Fbrem_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==false && isfbrem ==false && isPtCut==true){
      name  = Form ("%s_W_PT_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_PT_EB_odd.root", outputFile.c_str());
    }
    else if(isMiscalib == false && useZ == 0 && isR9selection==true && isEPselection==false && isfbrem ==false && isPtCut==false){
      name  = Form ("%s_W_noEP_EB_even.root",outputFile.c_str());
      name2 = Form ("%s_W_noEP_EB_odd.root", outputFile.c_str());
    }
    else { std::cout<<" Option not considered --> exit "<<std::endl; return -1 ;}

    TFile *outputName1 = new TFile(outputPath+name,"RECREATE");
    TFile *outputName2 = new TFile(outputPath+name2,"RECREATE");

    TString DeadXtal = Form("%s",inputFileDeadXtal.c_str());
     
    /// Run on odd
    FastCalibratorEE analyzer_even(data, g_EoC_EE, g_EoE_EE, typeEE);
    analyzer_even.bookHistos(nLoops);
    analyzer_even.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
    analyzer_even.Loop(numberOfEvents, useZ, useW, splitStat, nLoops,applyPcorr, applyEcorr,  useRawEnergy,isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
    analyzer_even.saveHistos(outputName1);
  
    /// Run on even
    FastCalibratorEE analyzer_odd(data, g_EoC_EE, g_EoE_EE, typeEE);
    analyzer_odd.bookHistos(nLoops);
    analyzer_odd.AcquireDeadXtal(DeadXtal,isDeadTriggerTower);
    analyzer_odd.Loop(numberOfEvents, useZ, useW, splitStat*(-1), nLoops,applyPcorr, applyEcorr,  useRawEnergy,isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,R9Min,EPMin,smoothCut,isfbrem,fbremMax,isPtCut,PtMin,isMCTruth,jsonMap, miscalibMethod, miscalibMap);
    analyzer_odd.saveHistos(outputName2);
    
  }

   return 0;
//E/P calibration done!
  }


  ZFit_class fitter( data, mc, NULL, 
		     invMass_var.c_str(), invMass_min, invMass_max, invMass_binWidth); 

  fitter._oddMC=vm.count("isOddMC");
  fitter._oddData=vm.count("isOddData");

  if(vm.count("r9WeightFile")){
    // if the data are weighted need to use the unbinned likelihood fit to have the correct errors
    fitter._isDataUnbinned=true;
    fitter._isDataSumW2=true;
  }

  fitter._forceNewFit=vm.count("forceNewFit");
  //  fitter._initFitMC=true;
  fitter.SetFitType(fit_type_value);
  fitter._updateOnly=vm.count("updateOnly");

  fitter.imgFormat=imgFormat;
  fitter.outDirFitResMC=outDirFitResMC;
  fitter.outDirFitResData=outDirFitResData;
  fitter.outDirImgMC=outDirImgMC;
  fitter.outDirImgData=outDirImgData;

  // check folder existance
  fitter.SetPDF_model(signal_type_value,0); // (0,0) convolution, CB no_bkg
  //fitter.SetPDF_model(1,0); // cruijff, no_bkg

  if(!vm.count("smearerFit")){ 

    fitter.Import(commonCut.c_str(), eleID, activeBranchList);
    for(std::vector<TString>::const_iterator category_itr=categories.begin();
	category_itr != categories.end();
	category_itr++){
      myClock.Start();
#ifdef DEBUG
      std::cout << "[DEBUG] category: " << *category_itr << std::endl;
#endif
      if (vm.count("runToy")) { 
	cout << "number of toys: "<<nToys<<endl;
	fitter.SetInitFileMC(outDirFitResMC+"/"+*category_itr+".txt");
	fitter.FitToy(*category_itr, nToys, nEventsPerToy); 
      }	
      else	fitter.Fit(*category_itr);
      myClock.Stop();
      myClock.Print();
    }
  }

  myClock.Reset();
  if(vm.count("smearerFit")){

	smearer.SetHistBinning(80,100,invMass_binWidth); // to do before Init
	if(vm.count("runToy")){
	  smearer.SetPuWeight(false);

	  smearer.SetToyScale(1, constTermToy);
	  if(vm.count("initFile")) smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"), true,initFileName.c_str());
	  //	  if(vm.count("initFile")) smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"), false,initFileName.c_str());
	  else smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"));
	  std::cout << "[DEBUG] " << constTermToy << std::endl;
	} else{
	  if(vm.count("initFile")){
	    std::cout << "[INFO] Reading init file: " << initFileName << std::endl;
	    args.readFromFile(initFileName.c_str());
	  }
	  args.writeToStream(std::cout, kFALSE);
	  smearer.Init(commonCut.c_str(), eleID);
	}
	myClock.Start();
	smearer.evaluate();
	myClock.Stop();
	myClock.Print();
	smearer.DumpNLL();

	//if(vm.count("plotOnly")) //smearer.SetNSmear(10);
	RooMinuit m(smearer);
	if(! vm.count("plotOnly") && ! vm.count("profileOnly")){
	  //m.setStrategy(0);
	  //m.setVerbose();

	  //m.hesse(); // one turn of hess to guess the covariance matrix
	  //RooFitResult *fitres = m.save();
	  ShervinMinuit sh(100, smearer);//(RooArgSet&)fitres->floatParsFinal());
	  sh.SetMinuit(m);
	  //ph.SetVariables();
	  //ph.SetCovMatrix(fitres->covarianceMatrix());
	  //ph.SetUpdateProposalParameters(kTRUE); // auto-create mean vars and add mappings
	  //ph.SetCacheSize(100);
	  //ProposalFunction* pf = ph.GetProposalFunction();
	  
	  if(minimType=="migrad"){
	  
	    //fitres = 
	    m.fit("");
	    //m.migrad();
	    //m.hesse();
	  } else if(minimType=="profile"){
	    MinimizationProfile(smearer, args, nIter);
	    args.writeToStream(std::cout, kFALSE);
	  } else if(minimType=="MCMC"){
	    MinimizationProfile(smearer, args, nIter, true);
	    args.writeToStream(std::cout, kFALSE);
	  } else if(minimType=="sampling"){
	    RooArgList 	 argList_(args);
	    TIterator 	*it_ = argList_.createIterator();
	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	      if (var->isConstant() || !var->isLValue()) continue;
	      TString  name(var->GetName());
	      if(name.Contains("scale")) var->setConstant();
	    }
	    
	    RooStats::ProposalHelper ph;
	    ph.SetVariables(args);
	    ph.SetCovMatrix(*(smearer._markov.GetAsDataSet()->covarianceMatrix(args)));
	    ph.SetUniformFraction(0.05);
	    ph.SetWidthRangeDivisor(2);

	    RooStats::MetropolisHastings MCMC(smearer, args, *(ph.GetProposalFunction()), nIter);
	    MCMC.SetNumBurnInSteps(10);
	    MCMC.SetSign(RooStats::MetropolisHastings::kNegative);
	    MCMC.SetType(RooStats::MetropolisHastings::kLog);
	    //MCMC.SetProposalFunction(sh);
	    RooStats::MarkovChain *mcChain = (MCMC.ConstructChain());
	    mcChain->SaveAs("tmp/newChain.root");
	    delete mcChain;

	  }
	}

	std::cout << "[INFO] Minimization time: \t";
	myClock.Stop();
	myClock.Print();
	if(!vm.count("profileOnly") && !vm.count("plotOnly")){
	  args.writeToFile(outDirFitResData+"/params-"+r+"-"+TString(commonCut.c_str())+".txt");
	  smearer._markov.SaveAs((outDirFitResData+"/markov-"+r+"-"+TString(commonCut.c_str())+".root"));
	}

	//RooDataSet *dSet = smearer.GetMarkovChainAsDataSet();
	//dSet->SaveAs("tmp/dataset.root");
	//save the smeared histograms

	for(std::vector<ZeeCategory>::iterator itr= smearer.ZeeCategories.begin();
		itr != smearer.ZeeCategories.end();
		itr++){
	  smearer.GetSmearedHisto(*itr, true, false);
	  smearer.GetSmearedHisto(*itr, true, true);
	  smearer.GetSmearedHisto(*itr, false, smearer._isDataSmeared);
	} 

	if(vm.count("plotOnly") || !vm.count("profileOnly")){
	  TFile *f = new TFile(outDirFitResData+"/histos-"+r+"-"+TString(commonCut.c_str())+".root", "recreate");
	  f->Print();
	  f->cd();
	  for(std::vector<ZeeCategory>::iterator itr= smearer.ZeeCategories.begin();
	      itr != smearer.ZeeCategories.end();
	      itr++){
	    //if(!itr->active) continue;
	    TH1F *MC = smearer.GetSmearedHisto(*itr, true, false);
	    TH1F *smearMC = smearer.GetSmearedHisto(*itr, true, true);
	    TH1F *data = smearer.GetSmearedHisto(*itr, false, smearer._isDataSmeared);
	  
	    MC->Write();
	    smearMC->Write();
	    data->Write();
	    f->Write();    
	  } 
	  f->Close();
	}

	if(vm.count("profileOnly") || !vm.count("plotOnly")){
	  //if(vm.count("profileOnly") && !vm.count("runToy")) smearer.SetNSmear(10);

	  std::cout <<"==================PROFILE=================="<<endl;
	  //if(!vm.count("constTermFix")) smearer.SetNSmear(0,20);
	  //create profiles
	  TString outFile=outDirFitResData.c_str();
	  outFile+="/outProfile-";
	  outFile+=r+"-"+TString(commonCut.c_str())+".root";
	  TFile *fOutProfile = new TFile(outFile,"recreate");
	  //test/dato/fitres/Hgg_Et_v7/0.03//outProfile-data/regions/test.dat-Et_25-EB.root

	  for (int ivar=0;ivar<args.getSize();++ivar)
	    {
	      RooArgList argList(args);
	      RooRealVar* var=(RooRealVar*)(argList.at(ivar));
	      if (var->isConstant())
		continue;

	      TString name(var->GetName());
	      //if(name.Contains("scale")) continue;
	      //if(name.Contains("absEta_1_1.4442-gold") && vm.count("alphaGoldFix")) continue;
	      // special part for alpha fitting 
	      double min=0.;
	      TString  alphaName=name; alphaName.ReplaceAll("constTerm","alpha");
	      RooRealVar *var2= name.Contains("constTerm") ? (RooRealVar *)argList.find(alphaName): NULL;
	      if(var2!=NULL && name.Contains("constTerm") && var2->isConstant()==false){
		TString name2 = name;
		
		double rho=0, Emean=0;
		smearer.SetDataSet(name,TString(var->GetName())+TString(var2->GetName()));
		if(vm.count("constTermFix")) MinProfile2D(var, var2, smearer, -1, 0., min, rho, Emean, false);
		smearer.dataset->Write();

		// rho profile
		name2.ReplaceAll("constTerm", "rho");
		smearer.SetDataSet(name2,"rho");
		Double_t v1=var->getVal();
		Double_t v2=var2->getVal();
		var2->setVal(0);
		var->setVal(rho);
 		TGraph *profil = NULL;
		profil = GetProfile(var, smearer,0);
		var2->setVal(v2);
		var->setVal(v1);
 		TString n="profileChi2_"+name2+"_"; n+=randomInt;
		profil->SetName(n);  
		TCanvas c("c_"+name);
		profil->Draw("AP*");
		fOutProfile->cd();
		profil->Write();
		delete profil;
		smearer.dataset->Write();
		
		
 		// phi profile
 		name2.ReplaceAll("rho", "phi");
		smearer.SetDataSet(name2,"phi");
		profil = GetProfile(var, var2, smearer, true, rho, Emean);
		n="profileChi2_"+name2+"_"; n+=randomInt;
		profil->SetName(n);  
		profil->Draw("AP*");
		fOutProfile->cd();
		profil->Write();
		delete profil;
		smearer.dataset->Write();

		// rho profile with fixed phi!=pi/2
		name2.ReplaceAll("phi", "rho_phi4");
		smearer.SetDataSet(name2,"rho_phi4");
		profil = GetProfile(var, var2, smearer, true, 0, Emean, 0.785);
		n="profileChi2_"+name2+"_"; n+=randomInt;
		profil->SetName(n);  
		profil->Draw("AP*");
		fOutProfile->cd();
		profil->Write();
		delete profil;
		smearer.dataset->Write();

// 		// rho profile with fixed phi!=pi/2
// 		name2.ReplaceAll("rho_phi4", "rho_phi6");
// 		smearer.SetDataSet(name2,"rho_phi6");
// 		profil = GetProfile(var, var2, smearer, true, 0, Emean, 0.524);
// 		n="profileChi2_"+name2+"_"; n+=randomInt;
// 		profil->SetName(n);  
// 		profil->Draw("AP*");
// 		fOutProfile->cd();
// 		profil->Write();
// 		delete profil;
// 		smearer.dataset->Write();

// 		// rho profile with fixed phi!=pi/2
// 		name2.ReplaceAll("rho_phi6", "rho_phi3");
// 		smearer.SetDataSet(name2,"rho_phi3");
// 		profil = GetProfile(var, var2, smearer, true, 0, Emean, 1.05);
// 		n="profileChi2_"+name2+"_"; n+=randomInt;
// 		profil->SetName(n);  
// 		profil->Draw("AP*");
// 		fOutProfile->cd();
// 		profil->Write();
// 		delete profil;
// 		smearer.dataset->Write();


	      }

	      //if(!name.Contains("scale")) continue;
	      std::cout << "Doing " << name << "\t" << var->getVal() << std::endl;
	      smearer.SetDataSet(name,name);
	      //	      if(vm.count("runToy")){
	      //		MinProfile(var, smearer, -1, 0., min, false);
	      //}
	      TGraph *profil = GetProfile(var, smearer,0);
	      TString n="profileChi2_"+name+"_"; n+=randomInt;
	      profil->SetName(n);  
	      TCanvas c("c_"+name);
	      profil->Draw("AP*");
	      fOutProfile->cd();
	      profil->Write();
	      std::cout << "Saved profile for " << name << std::endl;
	      smearer.dataset->Write();
	      delete profil;
	    }
	  std::cout << "Cloning args" << std::endl;
	  //	  RooArgSet *mu = (RooArgSet *)args.snapshot();
	  //std::cout << "creating roomultivargaussian" << std::endl;
	  //RooMultiVarGaussian g("multi","",args, *mu, *(smearer.GetMarkovChainAsDataSet()->covarianceMatrix()));
	  //std::cout << "created" << std::endl;

	  // 	  RooDataSet *dataset = g.generate(args,10);
	  // 	  for(Long64_t iGen=0; iGen<1; iGen++){  
	  // 	    RooArgList argList_(* dataset->get(iGen));
	  // 	    TIterator *it_ = argList_.createIterator();
	  // 	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	  // 	      if (!var->isLValue()) continue;
	  // 	      TString  name(var->GetName());
	  // 	      //std::cout << name << var->getVal() << std::endl;
	  // 	      var->Print();
	  // 	      //args.setRealValue(name,var->getVal());
// 	    }
// 	    //smearer.evaluate();
// 	  }
	  fOutProfile->Close();
	  
	}
  }
  tmpFile->Close();
  globalClock.Stop();
  std::cout << "[INFO] Total elapsed time: "; globalClock.Print(); 
  return 0;
}


//  LocalWords:  etaSCEle
