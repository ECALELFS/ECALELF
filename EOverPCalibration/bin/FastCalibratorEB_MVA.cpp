
#include "FastCalibratorEB_MVA.h"
#include <iostream>
#include <fstream>
#include <stdlib.h> 
#include <iomanip>
#include "ConfigParser.h"
#include "ntpleUtils.h"

/// Code used for Run IC procedure on EB/

int main (int argc, char ** argv) 
{
    
  ///Check if all nedeed arguments to parse are there
  if(argc != 2)
  {
    std::cerr << ">>>>> FastCalibrator::usage: " << argv[0] << " configFileName" << std::endl ;
    return 1;
  }
    
  /// Parse the config file
  parseConfigFile (argv[1]) ;
 
//   std::string inputFile       = gConfigParser -> readStringOption("Input::inputFile");
  std::string inputList       = gConfigParser -> readStringOption("Input::inputList");

  std::string inputTree       = gConfigParser -> readStringOption("Input::inputTree");

  std::string inputFileDeadXtal ="NULL" ;
  try {
        inputFileDeadXtal = gConfigParser -> readStringOption("Input::inputFileDeadXtal");
   }
   catch ( char const* exceptionString ){
   std::cerr << " exception = " << exceptionString << std::endl;

   }
  bool isMiscalib = gConfigParser -> readBoolOption("Input::isMiscalib");
  bool isSaveEPDistribution = gConfigParser -> readBoolOption("Input::isSaveEPDistribution");
  bool isEPselection = gConfigParser -> readBoolOption("Input::isEPselection");
  bool isR9selection = gConfigParser -> readBoolOption("Input::isR9selection");
  bool isMCTruth = gConfigParser -> readBoolOption("Input::isMCTruth");

  std::string outputFile      = gConfigParser -> readStringOption("Output::outputFile");

 
  
  int numberOfEvents       = gConfigParser -> readIntOption("Options::numberOfEvents");
  int useZ                 = gConfigParser -> readIntOption("Options::useZ");
  int useW                 = gConfigParser -> readIntOption("Options::useW");
  int splitStat            = gConfigParser -> readIntOption("Options::splitStat");
  int nLoops               = gConfigParser -> readIntOption("Options::nLoops");
  
  /// open ntupla of data or MC
  TChain * albero = new TChain (inputTree.c_str());
  FillChain(*albero,inputList); 
  
  ///Use the whole sample statistics if numberOfEvents < 0
  if ( numberOfEvents < 0 ) numberOfEvents = albero->GetEntries(); 
  

  /// run in normal mode: full statistics
  if ( splitStat == 0 ) {
   
    TString name ;
    TString name_tmp;
    if(isMiscalib == true && useZ == 1 && isR9selection ==true ) name_tmp = Form ("%s_Z_R9_miscalib",outputFile.c_str());
    if(isMiscalib == true && useZ == 1 && isEPselection ==true ) name_tmp = Form ("%s_Z_EP_miscalib",outputFile.c_str());
    if(isMiscalib == true && useZ == 1 && isEPselection ==false && isR9selection==false ) name_tmp =Form ("%s_Z_noEP_miscalib",outputFile.c_str());
    
    if(isMiscalib == false && useZ == 1 && isR9selection ==true ) name_tmp = Form ("%s_Z_R9",outputFile.c_str());
    if(isMiscalib == false && useZ == 1 && isEPselection ==true ) name_tmp = Form ("%s_Z_EP",outputFile.c_str());
    if(isMiscalib == false && useZ == 1 && isEPselection ==false && isR9selection==false ) name_tmp =Form ("%s_Z_noEP",outputFile.c_str());
    

    if(isMiscalib == true && useZ == 0 && isR9selection ==true ) name_tmp = Form ("%s_R9_miscalib",outputFile.c_str());
    if(isMiscalib == true && useZ == 0 && isEPselection ==true ) name_tmp = Form ("%s_EP_miscalib",outputFile.c_str());
    if(isMiscalib == true && useZ == 0 && isEPselection ==false && isR9selection==false ) name_tmp =Form ("%s_noEP_miscalib",outputFile.c_str());
    
    
    if(isMiscalib == false && useZ == 0 && isR9selection ==true ) name_tmp = Form ("%s_R9",outputFile.c_str());
    if(isMiscalib == false && useZ == 0 && isEPselection ==true ) name_tmp = Form ("%s_EP",outputFile.c_str());
    if(isMiscalib == false && useZ == 0 && isEPselection ==false && isR9selection==false ) name_tmp =Form ("%s_noEP",outputFile.c_str());
         
    name = Form("%s.root",name_tmp.Data());
    TFile *f1 = new TFile(name,"RECREATE");

    TString outEPDistribution = "Weight_"+name;
    
    TString DeadXtal = Form("%s",inputFileDeadXtal.c_str());    

    if(isSaveEPDistribution == true)
    {
     FastCalibratorEB_MVA analyzer(albero,outEPDistribution);
     analyzer.bookHistos(nLoops);
     analyzer.AcquireDeadXtal(DeadXtal);
     analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,isMCTruth);
     analyzer.saveHistos(f1);
    }
    else
    {
     FastCalibratorEB_MVA analyzer(albero);
     analyzer.bookHistos(nLoops);
     analyzer.AcquireDeadXtal(DeadXtal);
     analyzer.Loop(numberOfEvents, useZ, useW, splitStat, nLoops, isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,isMCTruth);
     analyzer.saveHistos(f1);
    }
   
  }

  /// run in even-odd mode: half statistics
  else if ( splitStat == 1 ) {
    
    /// Prepare the outputs
    std::string evenFile = "Even_" + outputFile;
    std::string oddFile = "Odd_" + outputFile;
    TString name;
    TString name2;
    
    if(isMiscalib == true && useZ == 1 && isR9selection==true)
    { name = Form ("%s_Z_R9_miscalib.root",evenFile.c_str());
      name2 = Form ("%s_Z_R9_miscalib.root",oddFile.c_str());
    }
    
    if(isMiscalib == true && useZ == 1 && isEPselection==true)
    { name = Form ("%s_Z_EP_miscalib.root",evenFile.c_str());
      name2 = Form ("%s_Z_EP_miscalib.root",oddFile.c_str());
    }
    if(isMiscalib == true && useZ == 1 && isR9selection==false && isEPselection==false)
    { name = Form ("%s_Z_noEP_miscalib.root",evenFile.c_str());
      name2 = Form ("%s_Z_noEP_miscalib.root",oddFile.c_str());
    }



    if(isMiscalib == false && useZ == 1 && isR9selection==true)
    { name = Form ("%s_Z_R9.root",evenFile.c_str());
      name2 = Form ("%s_Z_R9.root",oddFile.c_str());
    }
    
    if(isMiscalib == false && useZ == 1 && isEPselection==true)
    { name = Form ("%s_Z_EP.root",evenFile.c_str());
      name2 = Form ("%s_Z_EP.root",oddFile.c_str());
    }
    if(isMiscalib == false && useZ == 1 && isR9selection==false && isEPselection==false)
    { name = Form ("%s_Z_noEP.root",evenFile.c_str());
      name2 = Form ("%s_Z_noEP.root",oddFile.c_str());
    }
    

    if(isMiscalib == true && useZ == 0 && isR9selection==true)
    { name = Form ("%s_R9_miscalib.root",evenFile.c_str());
      name2 = Form ("%s_R9_miscalib.root",oddFile.c_str());
    }
    
    if(isMiscalib == true && useZ == 0 && isEPselection==true)
    { name = Form ("%s_EP_miscalib.root",evenFile.c_str());
      name2 = Form ("%s_EP_miscalib.root",oddFile.c_str());
    }
    if(isMiscalib == true && useZ == 0 && isR9selection==false && isEPselection==false)
    { name = Form ("%s_noEP_miscalib.root",evenFile.c_str());
      name2 = Form ("%s_noEP_miscalib.root",oddFile.c_str());
    }

    
    if(isMiscalib == false && useZ == 0 && isR9selection==true)
    { name = Form ("%s_R9.root",evenFile.c_str());
      name2 = Form ("%s_R9.root",oddFile.c_str());
    }
    
    if(isMiscalib == false && useZ == 0 && isEPselection==true)
    { name = Form ("%s_EP.root",evenFile.c_str());
      name2 = Form ("%s_EP.root",oddFile.c_str());
    }
    if(isMiscalib == false && useZ == 0 && isR9selection==false && isEPselection==false)
    { name = Form ("%s_noEP.root",evenFile.c_str());
      name2 = Form ("%s_noEP.root",oddFile.c_str());
    }

    TFile *f1 = new TFile(name,"RECREATE");
    TFile *f2 = new TFile(name2,"RECREATE");

    TString DeadXtal = Form("%s",inputFileDeadXtal.c_str());
     
    /// Run on odd
    FastCalibratorEB_MVA analyzer_even(albero);
    analyzer_even.bookHistos(nLoops);
    analyzer_even.AcquireDeadXtal(DeadXtal);
    analyzer_even.Loop(numberOfEvents, useZ, useW, splitStat, nLoops,isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,isMCTruth);
    analyzer_even.saveHistos(f1);
  
    /// Run on even
    FastCalibratorEB_MVA analyzer_odd(albero);
    analyzer_odd.bookHistos(nLoops);
    analyzer_odd.AcquireDeadXtal(DeadXtal);
    analyzer_odd.Loop(numberOfEvents, useZ, useW, splitStat*(-1), nLoops,isMiscalib,isSaveEPDistribution,isEPselection,isR9selection,isMCTruth);
    analyzer_odd.saveHistos(f2);
    
  }

  delete albero;
  return 0;
}
