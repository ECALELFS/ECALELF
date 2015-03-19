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

#include "EnergyScaleCorrection_class.h"
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

  correctionCategory_class c3(std::string("EB-absEta_1_1.4442"));
  correctionCategory_class c1(std::string("EB-absEta_0_1-bad" ));
  correctionCategory_class c2(std::string("EB-absEta_0_1-gold"));
  std::cout << (c1 < c2) << "\t" << (c1 < c3) << "\t" << (c2 < c3) << std::endl;
  std::cout << c1 << std::endl;
  std::cout << c2 << std::endl;
  std::cout << c3 << std::endl;

  correctionCategory_class p1(0, 0.8, 0.8, 50);
  correctionCategory_class p2(0, 0.81, 0.8, 50);

  std::cout << p1 << std::endl;
  std::cout << (p1 < c1) << std::endl;
  if(p1<c1) std::cout << "cioa" << std::endl;

  //  if(p1==p2) std::cout << "uguali 1" << std::endl;
  if(!(p1<p2) && !(p2<p1)) std::cout << "uguali 2" << std::endl;

  //  EnergyScaleCorrection_class corr("test/dato/22Jan2012-runDepMCAll_v2/loose/invMass_SC_regrCorrSemiParV5_pho/table/step7-invMass_SC_regrCorrSemiParV5_pho-loose-Et_20-trigger-noPF-HggRunEtaR9Et.dat","HggRunEtaR9");
  EnergyScaleCorrection_class corr("test/dato/22Jan2012-runDepMCAll_v2/loose/invMass_SC_regrCorrSemiParV5_pho/table/step1-invMass_SC_regrCorrSemiParV5_pho-loose-Et_20-trigger-noPF-HggRunEta.dat");

  std::cout << corr.getScaleOffset(206392,false,0.89,1.3, 50) << std::endl;
  std::cout << corr.getScaleOffset(205158,false, 0.926962, -0.246714, 59.38) << std::endl;
  std::cout << corr.getScaleOffset(205158,false, 0.926962, -0.246714, 19.38) << std::endl;
  correctionCategory_class category(205158, -0.246714,  0.926962, 59.38);
  std::cout << "##############################" << std::endl;
//   if(! (category < (--(correction_map_p->upper_bound(category)))->first))
//   std::cout << "[DEBUG] category(isEBEle, R9Ele) = " << category << "(" << isEBEle << ", " << R9Ele << ")" <<   correction_map_p->find(category)->second.first << std::endl;

  return 0;
}
  
		    
