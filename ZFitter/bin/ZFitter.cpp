#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>

#include <TChain.h>
#include <TStopwatch.h>
#include <TF1.h>
#include <TH2F.h>
#include <TFriendElement.h>

#include "../interface/ZFit_class.hh"
#include "../interface/puWeights_class.hh"
#include "../interface/r9Weights_class.hh"

#include "../interface/runDivide_class.hh"
#include "../interface/EnergyScaleCorrection_class.h"
#include "../interface/addBranch_class.hh"

#include "../interface/RooSmearer.hh"
#include <RooMinuit.h>
#include <RooStats/UniformProposal.h>
#include <RooStats/PdfProposal.h>
//#include <RooStats/SequentialProposal.h>
#include <RooGaussian.h>
#include <RooStats/MetropolisHastings.h>
#include "RooBinning.h"
//#include "../src/ShervinMinuit.cc"
#include <RooStats/ProposalHelper.h>
#include <RooMultiVarGaussian.h>

#include "Math/Minimizer.h"
//#include "../interface/RooMinimizer.hh"

#include <TPRegexp.h>
#include <RooFormulaVar.h>
#include <RooPullVar.h>

#include <TMatrixDSym.h>

#include <TRandom3.h>
#include <queue>
#define profile
#define PROFILE_NBINS 200

//#define DEBUG
#define smooth
#include "../src/nllProfile.cc"
using namespace std;
using namespace RooStats;

/**
   \todo
   - remove commonCut from category name and add it in the ZFit_class in order to not repeate the cut
   - make alpha fitting more generic (look for alphaName)
*/

typedef std::map< TString, TChain* > chain_map_t;
typedef std::map< TString, chain_map_t > tag_chain_map_t;

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


void UpdateFriends(tag_chain_map_t& tagChainMap){

  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
    TChain *chain = (tag_chain_itr->second.find("selected"))->second;
    //    std::cout << chain->GetName() << std::endl;
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){

      if(chain_itr->first!="selected" &&
	 chain->GetFriend(chain_itr->first)==NULL){
	std::cout << "[STATUS] Adding friend branch: " << chain_itr->first
		  << " to tag " << tag_chain_itr->first << std::endl;
	chain->AddFriend(chain_itr->second);
      }
      chain_itr->second->GetEntries();
    }
  }
  return;
}

 

int main(int argc, char **argv) {
  TStopwatch myClock;
  TStopwatch globalClock;
  globalClock.Start();

  puWeights_class puWeights;
  std::cout << "============================== Z General Fitter" << std::endl;

  using namespace boost;
  namespace po = boost::program_options;
  unsigned int nEvents_runDivide=100000;
  std::string chainFileListName;
  std::string regionsFileName;
  std::string runRangesFileName;
  std::string dataPUFileName, mcPUFileName;
  std::vector<TString> dataPUFileNameVec, mcPUFileNameVec;
  std::string r9WeightFile;
  std::string initFileName;
  //  bool savePUweightTree;
  std::string imgFormat="eps", outDirFitResMC="test/MC/fitres", outDirFitResData="test/dato/fitres", outDirImgMC="test/MC/img", outDirImgData="test/dato/img", outDirTable="test/dato/table", selection;
  TString eleID="";
  //std::vector<std::string> signalFiles, bkgFiles, dataFiles;
  std::string commonCut;
  std::string corrEleFile, corrEleType;
  std::string smearEleFile, smearEleType;
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
  std::string minimType;
  std::vector<std::string> branchList;

  //  std::vector<std::string> signalFriends,
  //  bool _ZFit_class, _RooSmearer;

  po::options_description desc("Main options");
  po::options_description outputOption("Output options");
  po::options_description inputOption("Input options");
  po::options_description fitterOption("Z fitter options");
  po::options_description smearerOption("Z smearer options");
  po::options_description toyOption("toyMC options");

  //po::options_description cmd_line_options;
  //cmd_line_options.add(desc).add(fitOption).add(smearOption);

  desc.add_options()
    ("help,h","Help message")

    ("runDivide", "execute the run division")

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
    //
    ("r9WeightFile", po::value<string>(&r9WeightFile),"File with r9 photon-electron weights")
    ("saveR9TreeWeight", "")
    ("saveRootMacro","")
    //
    ("selection", po::value<string>(&selection)->default_value("loose"),"")
    ("commonCut", po::value<string>(&commonCut)->default_value("Et_25-trigger-noPF"),"")
    ("invMass_var", po::value<string>(&invMass_var)->default_value("invMass_SC_regrCorr_ele"),"")
    ("invMass_min", po::value<float>(&invMass_min)->default_value(65.),"")
    ("invMass_max", po::value<float>(&invMass_max)->default_value(115.),"")
    ("invMass_binWidth", po::value<float>(&invMass_binWidth)->default_value(0.25),"Smearing binning")
    ("isOddMC", "Activate if use only odd events in MC")
    ("isOddData", "Activate if use only odd events in data")    
    //
    ("readDirect","") //read correction directly from config file instead of passing as a command line arg
    ("addPtBranches", "")  //add new pt branches ( 3 by default, fra, ele, pho)
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
    ("constTermFix", "constTerm not depending on Et")
    ("smearingEt", "alpha term depend on sqrt(Et) and not on sqrt(E)")
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
    
  desc.add(inputOption);
  desc.add(outputOption);
  desc.add(fitterOption);
  desc.add(smearerOption);
  desc.add(toyOption);

  po::variables_map vm;
  //
  // po::store(po::parse_command_line(argc, argv, smearOption), vm);
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    



  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  TString energyBranchName="";
  if(invMass_var=="invMass_SC_regrCorr_ele") energyBranchName = "energySCEle_regrCorr_ele";
  else if(invMass_var=="invMass_SC_regrCorr_pho") energyBranchName = "energySCEle_regrCorr_pho";
  else if(invMass_var=="invMass_regrCorr_fra") energyBranchName = "energyEle_regrCorr_fra";
  else if(invMass_var=="invMass_regrCorr_egamma") energyBranchName = "energyEle_regrCorr_egamma";
  else if(invMass_var=="invMass_SC") energyBranchName = "energySCEle";
  else if(invMass_var=="invMass_SC_corr") energyBranchName = "energySCEle_corr";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV4_ele") energyBranchName = "energySCEle_regrCorrSemiParV4_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV4_pho") energyBranchName = "energySCEle_regrCorrSemiParV4_pho";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV5_ele") energyBranchName = "energySCEle_regrCorrSemiParV5_ele";
  else if(invMass_var=="invMass_SC_regrCorrSemiParV5_pho") energyBranchName = "energySCEle_regrCorrSemiParV5_pho";
  else {
    std::cerr << "Energy branch name not define for invariant mass branch: " << invMass_var << std::endl;
    exit(1);
  }

  if(!vm.count("chainFileList") && !vm.count("runToy")){
    std::cerr << "[ERROR] Missing mandatory option \"chainFile\"" << std::endl;
    return 1;
  }


  if(!vm.count("regionsFile") && 
     !vm.count("runDivide") && !vm.count("savePUTreeWeight") && 
     !vm.count("saveR9TreeWeight") && !vm.count("saveCorrEleTree") &&
     !vm.count("saveRootMacro")
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
  if(!checkDirectories){
    std::cerr << "[ERROR] Directory " << outDirFitResMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirFitResData)+" ]");
  if(!checkDirectories){
    std::cerr << "[ERROR] Directory " << outDirFitResData << " not found" << std::endl;
  }
  checkDirectories=checkDirectories &&   !system("[ -d "+TString(outDirImgMC)+" ]");
  if(!checkDirectories){
     std::cerr << "[ERROR] Directory " << outDirImgMC << " not found" << std::endl;
  }
  checkDirectories=checkDirectories && !system("[ -d "+TString(outDirImgData)+" ]");
  if(!checkDirectories){
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
     ) return 1;

  if(!dataPUFileName.empty()) dataPUFileNameVec.push_back(dataPUFileName.c_str());
  if(!mcPUFileName.empty()) mcPUFileNameVec.push_back(mcPUFileName.c_str());
  //============================== Reading the config file with the list of chains
  tag_chain_map_t tagChainMap;
  TString tag, chainName, fileName;
  
  TString chainFileListTag=chainFileListName; 
  chainFileListTag.Remove(0,chainFileListTag.Last('/')+1);
  chainFileListTag.ReplaceAll(".dat","");

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
    //else if (chainName.Contains("corr")){
    //if (tag.Contains("d") && corrEleFile.empty()) corrEleFile=fileName;
    //continue;
    //}

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
    std::cout << "[DEBUG] " << tag << "\t" << chainName << "\t" << fileName << std::endl;
#endif
  }

  
  //init chains
  for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
      tag_chain_itr!=tagChainMap.end();
      tag_chain_itr++){
#ifdef DEBUG
    std::cout << tag_chain_itr->first << std::endl;
#endif
    for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){
#ifdef DEBUG
      std::cout << " - " << chain_itr->first << "\t" << chain_itr->second->GetName() << "\t" << chain_itr->second->GetTitle() << std::endl;
#endif
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
	Ssiz_t ss=0;
	runRange_itr->Tokenize(token1,ss,"-");
	ss=runRange_itr->First('-');
	runRange_itr->Tokenize(token2,ss,"-");
	categories.push_back((*region_itr)+"-runNumber_"+token1+"_"+token2+"-"+commonCut.c_str());
      }
    }else categories.push_back((*region_itr)+"-"+commonCut.c_str());
  }

  

  ///------------------------------ to obtain r9weights
  r9Weights_class r9Weights;
  if(vm.count("saveR9TreeWeight") && !vm.count("r9WeightFile")){
    std::cerr << "[ERROR] No r9WeightFile specified" << std::endl;
    return 1;
  }
  if(vm.count("r9WeightFile")){
    r9Weights.ReadFromFile(r9WeightFile);
    
    // mc // save it in a file and reload it as a chain to be safe against the reference directory for the tree
    
    TFile r9File_mc("tmp/r9mcTree.root","recreate");
    if(r9File_mc.IsOpen()){
      r9File_mc.cd();
      TTree *r9Tree = r9Weights.GetTreeWeight((tagChainMap["s"])["selected"]);
      r9Tree->Write();
      delete r9Tree;
      r9File_mc.Write();
      r9File_mc.Close();
      std::pair<TString, TChain* > pair_tmp("r9Weight", new TChain("r9Weight"));
      (tagChainMap["s"]).insert(pair_tmp);
      (tagChainMap["s"])["r9Weight"]->SetTitle("s");
      (tagChainMap["s"])["r9Weight"]->Add("tmp/r9mcTree.root");
    } else {
      std::cerr << "[ERROR] " << std::endl;
      return 1;
    }
    

    // data
    TFile r9File_data("tmp/r9dataTree.root","recreate");
    if(r9File_data.IsOpen()){
      r9File_data.cd();
      TTree *r9Tree = r9Weights.GetTreeWeight((tagChainMap["d"])["selected"]);
      r9Tree->Write();
      delete r9Tree;
      r9File_data.Write();
      r9File_data.Close();
      std::pair<TString, TChain* > pair_tmp("r9Weight", new TChain("r9Weight"));
      (tagChainMap["d"]).insert(pair_tmp);
      (tagChainMap["d"])["r9Weight"]->SetTitle("d");
      (tagChainMap["d"])["r9Weight"]->Add("tmp/r9dataTree.root");
    }
  }
  if(vm.count("saveR9TreeWeight")) return 0;
//   if((tagChainMap["s"]).count("r9Weight")){
//     std::cout << "[STATUS] Adding r9Weight chain to signal chain as friend chain" << std::endl;
//   }
//   if((tagChainMap["d"]).count("r9Weight")){
//     std::cout << "[STATUS] Adding r9Weight chain to data chain as friend chain" << std::endl;
//     //	if((tagChainMap["d"])["selected"]->AddFriend((tagChainMap["d"])["r9Weight"])==NULL) return 2;
// #ifdef DEBUG
//     std::cout << "[DEBUG] r9Weight branch address " << (tagChainMap["d"])["selected"]->GetBranch("r9Weight") << std::endl;
// #endif
//   }


  //==============================

  //  if(vm.count("dataPU")==0 && (tagChainMap["s"]).count("pileupHist")==0 && (tagChainMap["s"]).count("pileup")==0){

  if(vm.count("noPU")==0 && !vm.count("runToy")){
    if(dataPUFileNameVec.empty() && (tagChainMap.count("s")!=0) && (tagChainMap["s"]).count("pileup")==0){
      std::cerr << "[ERROR] Nor pileup mc tree configured in chain list file either dataPU histograms are not provided" << std::endl;
      return 1;
    }else if( vm.count("dataPU")!=0 || (!dataPUFileNameVec.empty() && ((tagChainMap.count("s")==0) || (tagChainMap["s"]).count("pileup")==0))){
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
	TString treeName="pileup";
	TString fileame="tmp/mcPUtree"+tag_chain_itr->first+".root";
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
  if (vm.count("corrEleType")){
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "[STATUS] Getting energy scale corrections from file: " << corrEleFile << std::endl;
    TString treeName="scaleEle_"+corrEleType;
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("d")==0 || !tag_chain_itr->first.Contains("d")) continue; //only data
      if(tag_chaint_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
     
      TString filename="tmp/scaleEle_"+corrEleType+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      EnergyScaleCorrection_class eScaler(corrEleFile,corrEleType);
      std::cout << "[STATUS] Saving electron scale corrections to root file:" << filename << std::endl;

	TFile f(filename,"recreate");
	if(!f.IsOpen() || f.IsZombie()){
	  std::cerr << "[ERROR] File for scale corrections: " << filename << " not opened" << std::endl;
	  exit(1);
	}
	TTree *corrTree = eScaler.GetCorrTree(ch);
	//corrTree->SetName("scaleEle");
	//corrTree->SetTitle(corrEleType.c_str());
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
  if (vm.count("corrEleType")){
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "[STATUS] Getting energy scale corrections from file: " << corrEleFile << std::endl;
    TString treeName="scaleEle_"+corrEleType;
    EnergyScaleCorrection_class eScaler(corrEleFile,corrEleType);

    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("d")==0 || !tag_chain_itr->first.Contains("d")) continue; //only data
      if(tag_chaint_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
     
      TString filename="tmp/scaleEle_"+corrEleType+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      std::cout << "[STATUS] Saving electron scale corrections to root file:" << filename << std::endl;

	TFile f(filename,"recreate");
	if(!f.IsOpen() || f.IsZombie()){
	  std::cerr << "[ERROR] File for scale corrections: " << filename << " not opened" << std::endl;
	  exit(1);
	}
	TTree *corrTree = eScaler.GetCorrTree(ch);
	//corrTree->SetName("scaleEle");
	//corrTree->SetTitle(corrEleType.c_str());
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
    EnergyScaleCorrection_class eScaler("","", smearEleFile,smearEleType);
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("s")==0 || !tag_chain_itr->first.Contains("s")) continue; //only data
      if(tag_chaint_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;
 
      TString filename="tmp/smearEle_"+corrEleType+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";
      std::cout << "[STATUS] Saving electron smearings to root file:" << filename << std::endl;
      
      TFile f(filename,"recreate");
      if(!f.IsOpen() || f.IsZombie()){
	std::cerr << "[ERROR] File for scale corrections: " << filename << " not opened" << std::endl;
	exit(1);
      }

      TTree *corrTree = eScaler.GetSmearTree((tagChainMap["s"])["selected"], true, energyBranchName );
      f.cd();
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
    UpdateFriends(tagChainMap);
    TString treeName=*branch_itr;
    if(treeName.Contains("invMassSigma")){
      newBrancher.scaler= new EnergyScaleCorrection_class("","", smearEleFile,smearEleType);
    }

    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
      if(tag_chain_itr->first.CompareTo("s")==0 || tag_chain_itr->first.CompareTo("d")==0) continue; //only data
      if(tag_chaint_itr->second.count(treeName)!=0) continue; //skip if already present
      TChain *ch = (tag_chain_itr->second.find("selected"))->second;

      //data
      std::cout <<"[STATUS] Adding branch " << treeName << " to " << tag_chain_itr->first <<std::endl;
      TString filename="tmp/"+treeName+"_"+tag_chain_itr->first+"-"+chainFileListTag+".root";

      TFile f(filename,"recreate");
      if (!f.IsOpen()){
	std::cerr << "[ERROR] File for branch " << treeName << " not created" << std::endl;
	return 1;
      }
      TTree *newTree = newBrancher.AddBranch(ch,treeName+tag_chain_itr->first, treeName);

      if(newTree==NULL){
	std::cerr << "[ERROR] New tree for branch " << treeName << " is NULL" << std::endl;
	return 1;
      }

      f.cd();
      newTree->Write();	
      delete newTree;
      f.Write();
      f.Close();
      std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
      chain_map_t::iterator chain_itr= ((tagChainMap[tag_chain_itr->first]).insert(pair_tmp)).first;
      chain_itr->second->SetTitle(tag_chain_itr->first);
      chain_itr->second->Add(filename);
    } //end of sample loop
  } //end of branches loop

  if (vm.count("addPtBranches")){
    TString treeName="AddedCorrPtBranches";
    //data
    if((tagChainMap["d"]).count(treeName)==0){
      std::cout <<"[STATUS] Adding corrected Pt branches to data..."<<std::endl;
      addBranch_class ptBr;
      TFile f(TString("tmp/AddedCorrPtBranches_data.root"),"recreate");
      if (f.IsOpen()){
	TTree *ptBranches = ptBr.AddBranch_Pt((tagChainMap["d"])["selected"],treeName);
	f.cd();
	ptBranches->Write();	
	delete ptBranches;
	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["d"]).insert(pair_tmp);
	(tagChainMap["d"])[treeName]->SetTitle("d");
	if((tagChainMap["d"])[treeName]->Add(TString("tmp/AddedCorrPtBranches_data.root"))<=0) return 1;
      }
    }
    //	if( (tagChainMap["d"])["selected"]->AddFriend((tagChainMap["d"])[treeName])==NULL) return 2;

    //mc
    if((tagChainMap["s"]).count(treeName)==0){
      std::cout <<"[STATUS] Adding corrected Pt branches to mc signal..."<<std::endl;
      addBranch_class ptBr;
      TFile f(TString("tmp/AddedCorrPtBranches_signal.root"),"recreate");
      if (f.IsOpen()){
	TTree *ptBranches = ptBr.AddBranch_Pt((tagChainMap["s"])["selected"],treeName);
	f.cd();
	ptBranches->Write();	
	delete ptBranches;
	f.Write();
	f.Close();
	std::pair<TString, TChain* > pair_tmp(treeName, new TChain(treeName));
	(tagChainMap["s"]).insert(pair_tmp);
	(tagChainMap["s"])[treeName]->SetTitle("s");
	if((tagChainMap["s"])[treeName]->Add(TString("tmp/AddedCorrPtBranches_signal.root"))<=0) return 1;
      }
    }
    //if( (tagChainMap["s"])["selected"]->AddFriend((tagChainMap["s"])[treeName])==NULL) return 2;
  }


  //(tagChainMap["s"])["selected"]->GetEntries();
  UpdateFriends(tagChainMap);

  //create tag "s" if not present (due to multiple mc samples)
  tag="s"; chainName="selected";
  if(!tagChainMap.count(tag)){
    //#ifdef DEBUG
    std::cout << "==============================" << std::endl;
    std::cout << "==============================" << std::endl;

    std::cout << "[DEBUG] Create new tag map for tag: " << tag << std::endl;
    //#endif
    std::pair<TString, chain_map_t > pair_tmp_tag(tag,chain_map_t()); // make_pair not work with scram b
    tagChainMap.insert(pair_tmp_tag);
    
    std::pair<TString, TChain* > pair_tmp(chainName, new TChain(chainName));
    (tagChainMap[tag]).insert(pair_tmp);
    (tagChainMap[tag])[chainName]->SetTitle(tag);
    
    for(tag_chain_map_t::const_iterator tag_chain_itr=tagChainMap.begin();
	tag_chain_itr!=tagChainMap.end();
	tag_chain_itr++){
#ifdef DEBUG
      std::cout << tag_chain_itr->first << std::endl;
#endif
      if(tag_chain_itr->first.CompareTo("s")==0 || !tag_chain_itr->first.Contains("s")) continue;
      
      for(chain_map_t::const_iterator chain_itr=tag_chain_itr->second.begin();
	chain_itr!=tag_chain_itr->second.end();
	chain_itr++){
	chain_itr->second->GetEntries();
	std::cout <<"Adding chain to s: " << chain_itr->first << "\t" << chain_itr->second->GetName() << "\t" << chain_itr->second->GetTitle() << std::endl;
	if((tagChainMap[tag])[chainName]->Add(chain_itr->second)==0) exit(1);
	
	TList *friendList= chain_itr->second->GetListOfFriends();
	if(friendList!=NULL){
	  friendList->Print();
	TIter friend_itr(friendList);
	for(TFriendElement *friendElement = (TFriendElement*) friend_itr.Next(); 
	    friendElement != NULL; friendElement = (TFriendElement*) friend_itr.Next()){
	  std::cout << "[STATUS] Adding friend " << friendElement->GetTreeName() << std::endl;
	  (tagChainMap[tag])[chainName]->AddFriend(friendElement->GetTree());
	}
	}
      }
    }
  }
  
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
      if(vm.count("constTermFix")==0) const_term_v = new RooRealVar("constTerm_"+*region_itr, "constTerm_"+varName,0.01, 0.0005,0.05); 
      else const_term_v = new RooRealVar("constTerm_"+varName, "constTerm_"+varName,0.01, 0.0005,0.02);
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
      args.add(*alpha_v);
    } 
    if(reg.MatchB(*region_itr) && vm.count("constTermFix")==1){
      alpha_ = new RooFormulaVar("alpha_"+*region_itr, "alpha_"+varName,"@0", *alpha_v);
      alpha_v->setConstant(false);
    } else alpha_ = alpha_v;
    
    args_vec.push_back(RooArgSet(*scale_, *alpha_, *const_term_));
  }

  args.sort(kFALSE);
  if(vm.count("smearerFit")){
    std::cout << "------------------------------ smearer parameters" << std::endl;
    args.writeToStream(std::cout, kFALSE);
  }

  TRandom3 g(0);
  TString filename="tmp/tmpFile-"; filename+=g.Integer(1000000);filename+=".root";
  TFile *tmpFile = new TFile(filename,"recreate");
  tmpFile->cd();
  RooSmearer smearer("smearer",(tagChainMap["d"])["selected"], (tagChainMap["s"])["selected"], NULL, 
		     categories,
		     args_vec, args, energyBranchName);
  smearer._isDataSmeared=vm.count("isDataSmeared");
  smearer.SetOnlyDiagonal(vm.count("onlyDiagonal"));
  smearer._autoBin=vm.count("autoBin");
  smearer._autoNsmear=vm.count("autoNsmear");
  smearer.smearscan=vm.count("smearscan");
  //smearer.nEventsMinDiag = nEventsMinDiag;
  smearer._deactive_minEventsOffDiag = nEventsMinOffDiag;
  smearer.SetSmearingEt(vm.count("smearingEt"));

  //------------------------------ Take the list of branches needed for the defined categories
  ElectronCategory_class cutter;
  std::set<TString> activeBranchList;
  for(std::vector<TString>::const_iterator region_itr = categories.begin();
      region_itr != categories.end();
      region_itr++){
    std::set<TString> tmpList = cutter.GetBranchNameNtuple(*region_itr);
    activeBranchList.insert(tmpList.begin(),tmpList.end());
    // add also the friend branches!
  }

  //------------------------------ ZFit_class declare and set the options
  TChain *data = NULL;
  TChain *mc = NULL;
  if(!vm.count("smearerFit")){
    data= (tagChainMap["d"])["selected"];
    mc  = (tagChainMap["s"])["selected"];
  }
  ZFit_class fitter( data, mc, NULL, 
		    invMass_var.c_str(), invMass_min, invMass_max); 

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
	  smearer.SetToyScale(1, constTermToy);
	  if(vm.count("initFile")) smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"), true,initFileName.c_str());
	  else smearer.Init(commonCut.c_str(), eleID, nEventsPerToy, vm.count("runToy"));
	  std::cout << "[DEBUG] " << constTermToy << std::endl;
	} else{
	  if(vm.count("initFile")) args.readFromFile(initFileName.c_str());
	  args.writeToStream(std::cout, kFALSE);
	  smearer.Init(commonCut.c_str(), eleID);
	}

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
	  } else if(minimType=="shervinDNA"){
	    RooArgList 	 argList_(args);
	    TIterator 	*it_ = argList_.createIterator();
	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	      if (var->isConstant() || !var->isLValue()) continue;
	      TString  name(var->GetName());
	      if(!name.Contains("scale")) var->setConstant();
	    }
	    MinimizationProfile(smearer, args, nIter);	
	    delete it_;
	    it_ = argList_.createIterator();
	    for(RooRealVar *var = (RooRealVar*)it_->Next(); var != NULL; var = (RooRealVar*)it_->Next()){
	      if (!var->isLValue()) continue;
	      TString  name(var->GetName());
	      if(!name.Contains("scale")) var->setConstant(kFALSE);
	      else var->setConstant();
	    } 
	      
	      
	    args.writeToStream(std::cout, kFALSE);
	    RooMinimizer m(smearer);
	    m._theFitter->Config().SetMinimizer("Genetic","Genetic");
	    m.setEps(100); // default tolerance
	    m._theFitter->Config().MinimizerOptions().SetTolerance(2e-5);
	    // default max number of calls
	    m._theFitter->Config().MinimizerOptions().SetMaxIterations(50);
 	    m._theFitter->Config().MinimizerOptions().SetMaxFunctionCalls(10);
	    //m.setVerbose();
	    m.setPrintLevel(4);
	    m.minimize("Genetic","Genetic");

	  } else if(minimType=="genetic"){
	    RooMinimizer m(smearer);
	    //m.setMinimizerType("genetic");
	    m._theFitter->Config().SetMinimizer("Genetic","Genetic");
	    m.setEps(100); // default tolerance
	    m._theFitter->Config().MinimizerOptions().SetTolerance(2e-5);
	    // default max number of calls
	    m._theFitter->Config().MinimizerOptions().SetMaxIterations(10);
 	    m._theFitter->Config().MinimizerOptions().SetMaxFunctionCalls(10);
	    //	    m.setErrorLevel(0.1);
	    //m.setVerbose();
	    m.setPrintLevel(4);
	    m.minimize("Genetic","Genetic");
	    
	    //fitres = m.save();
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
	if(!vm.count("plotOnly")){
	  Int_t oldMarkovSize=smearer._markov.Size();
	  //smearer.SetNSmear(10);

	  std::cout <<"==================PROFILE=================="<<endl;
	  //	  smearer.SetNSmear(0,10);
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

	      // special part for alpha fitting 
	      double min=0.;
	      TString  alphaName=name; alphaName.ReplaceAll("constTerm","alpha");
	      RooRealVar *var2= name.Contains("constTerm") ? (RooRealVar *)argList.find(alphaName): NULL;
	      if(var2!=NULL && name.Contains("constTerm")){ // && var2->isConstant()==false){
		smearer.SetDataSet(name,TString(var->GetName())+TString(var2->GetName()));
		MinProfile2D(var, var2, smearer, -1, 0., min, false);
		//MinMCMC2D(var, var2, smearer, 1, 0., min, 1200, false);
		//MinMCMC2D(var, var2, smearer, 2, 0., min, 800, false);
		//MinMCMC2D(var, var2, smearer, 3, 0., min, 100, false);
		smearer.dataset->Write();
		TString of=outFile; of.ReplaceAll(".root",name+".root");
		smearer.dataset->SaveAs(of);
	      }
	      //if(!name.Contains("scale")) continue;
	      std::cout << "Doing " << name << "\t" << var->getVal() << std::endl;
	      
	      TGraph *profil = GetProfile(var, smearer,0);
	      TString n="profileChi2_"+name;
	      profil->SetName(n);  
	      TCanvas c("c_"+name);
	      profil->Draw("AP*");
	      fOutProfile->cd();
	      profil->Write();
	      std::cout << "Saved profile for " << name << std::endl;
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
	  RooStats::MarkovChain mchain;
	  mchain.AddWithBurnIn(smearer._markov, oldMarkovSize);
	  mchain.Write();
	  fOutProfile->Close();
	  
	}
  }
  tmpFile->Close();
  globalClock.Stop();
  std::cout << "[INFO] Total elapsed time: "; globalClock.Print(); 
  return 0;
}

