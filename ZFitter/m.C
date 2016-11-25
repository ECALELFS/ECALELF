{
  TString optSmooth="k3a";
  TString toyDir="test/dato/fitres/Hgg_Et-toys/scaleStep2smearing_9/factorizedSherpaFixed_DataSeedFixed_cmscaf1nd/";
  //0.005-0.00  0.005-0.01  0.005-0.15  0.01-0.00  0.01-0.01  0.01-0.15  0.02-0.00  0.02-0.01  0.02-0.15
  gROOT->ProcessLine(".include /afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.32.03-cms16/include");
  gROOT->ProcessLine(".L macro/mcmcDraw.C+");
 
  std::vector<int> intSmoothVec;
  intSmoothVec.push_back(0);
  intSmoothVec.push_back(3);
  intSmoothVec.push_back(10);

  std::vector<TString> toySubDirVec;
  toySubDirVec.push_back("0.01-0.00/1/");
  //  toySubDirVec.push_back("0.000-0.07/50/");
  //  toySubDirVec.push_back("0.007-0.05/50/");

  for(std::vector<TString>::const_iterator toySubDir_itr = toySubDirVec.begin();
      toySubDir_itr!= toySubDirVec.end(); toySubDir_itr++){
    for(int nSmooth=0; nSmooth<=10; nSmooth++){
      if(nSmooth!=0 && nSmooth!=10) continue;
    //    for(std::vector<int>::const_iterator nSmooth_itr = intSmoothVec.begin();
    //	nSmooth_itr != intSmoothVec.end(); nSmooth_itr++){
      //int nSmooth=*nSmooth_itr;
      TString toySubDir=*toySubDir_itr;
      std::cout << toySubDir << "\t" << nSmooth << std::endl;
      TString outDir=toyDir+toySubDir+"img/"+optSmooth+"/nSmooth_"; outDir+=nSmooth; outDir+="/";
      system(("mkdir -p "+outDir).Data());
      //  TTree *toys = ToyTree("test/dato/fitres/Hgg_Et-toys/old/scaleStep2smearing_5/0.01-0.00/C/","outProfile-scaleStep2smearing_5-Et_25-trigger-noPF-EB.root","scan");
      TTree *toys = ToyTree(toyDir+toySubDir,"outProfile-scaleStep2smearing_9-Et_20-trigger-noPF-EB.root", optSmooth,nSmooth);
      //toys->Scan("catName:catIndex:constTermTrue:alphaTrue:constTerm:alpha","","col=50");
      DrawToyTree(toys, outDir);
      delete toys;
    }
  }
  
}
