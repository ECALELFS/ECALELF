#include <iostream>

//#include "ZFit_class.hh"

#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TChain.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <TCut.h>
#include <TString.h>
#include <set>
#include <boost/program_options.hpp>

int main(int argc, char **argv) {


  std::cout << "------------------------------ Declaring variables" << std::endl;
  //  chain_map_t signal_chain, data_chain, bkg_chain;
  using namespace boost;
  namespace po = boost::program_options;
  po::options_description desc("Main options");
  
  desc.add_options()
    ("help,h","Help message")
    ;


  //po::options_description cmd_line_options;
  //cmd_line_options.add(desc).add(fitOption).add(smearOption);
  po::variables_map vm;
  //
  // po::store(po::parse_command_line(argc, argv, smearOption), vm);
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  return 0;
}
  
		    
