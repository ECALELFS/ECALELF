{
  std::vector<TString> mcLabel_vec;
  mcLabel_vec.push_back("Madgraph");
  mcLabel_vec.push_back("Powheg");
  mcLabel_vec.push_back("Sherpa");


  // pileup
  c = PlotDataMC(data, signal, "nPV", "(30,0,30)", ""+commonCut, "", dataLabel+*l_itr, mcLabel, "nVtx", ""); 
  c->SaveAs(outputPath+"nPV.eps");
  delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "etaEle", "(100,-2.5,2.5)", "eleID_loose-trigger-noPF-Et_25", "", "", mcLabel_vec, "#eta", "", false, true, true,false,false,false); 
//    c->SaveAs(outputPath+"etaEle-Et_25.eps");
//    //c->SaveAs(outputPath+"etaEle-Et_25.png");
//    c->SaveAs(outputPath+"etaEle-Et_25.C");
//   delete c;


}







  TString dataLabel="22Jan ", mcLabel="Simulation";
  std::vector<TString> runPeriods, labels;
  
  //runPeriods.push_back("-runNumber_190456_193621"); labels.push_back("RUN2012A");
  //runPeriods.push_back("-runNumber_193834_196531"); labels.push_back("RUN2012B");
  runPeriods.push_back("-runNumber_190456_196531"); labels.push_back("RUN2012AB");
  runPeriods.push_back("-runNumber_198111_203002"); labels.push_back("RUN2012C");
  runPeriods.push_back("-runNumber_203756_208686"); labels.push_back("RUN2012D");
  runPeriods.push_back(""); labels.push_back("");

  std::vector<TString>::const_iterator l_itr=labels.begin();
  TString outPath=outputPath;
  for(std::vector<TString>::const_iterator p_itr = runPeriods.begin();
      p_itr != runPeriods.end();
      p_itr++,l_itr++){
    int index=p_itr-runPeriods.begin();

  TString commonCut="eleID_WP80PU-Et_25-noPF-trigger"+*p_itr;
  //TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
  //TString outputPath="test/dato/moriond2013/WP80_PU/img/";
  outputPath=outPath+"/"+commonCut+"/";
  
  system(("mkdir -p "+outputPath).Data());
  
  c = PlotDataMC(data, signal, "nHitsSCEle", "(100,0,100)", "EB-"+commonCut,"", dataLabel+*l_itr, mcLabel, "nHits SC", ""); 
  c->SaveAs(outputPath+"/nHitsSCEle-EB.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "nHitsSCEle", "(200,0,200)", "EB-"+commonCut,"", dataLabel+*l_itr, mcLabel, "nHits SC", "",true); 
  c->SaveAs(outputPath+"/nHitsSCEle-EB-log.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "nHitsSCEle", "(100,0,100)", "EE-"+commonCut,"", dataLabel+*l_itr, mcLabel, "nHits SC", ""); 
  c->SaveAs(outputPath+"/nHitsSCEle-EE.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "nHitsSCEle", "(200,0,200)", "EE-"+commonCut,"", dataLabel+*l_itr, mcLabel, "nHits SC", "",true ); 
  c->SaveAs(outputPath+"/nHitsSCEle-EE-log.eps");
  delete c;


  // regression comparison
  c = PlotDataMC(data, signal, "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.2,0.2)", "EB-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Electron (Hgg) regression/std. SC -1 ", ""); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EB.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.2,0.2)", "EB-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Photon (Hgg) regression/std. SC -1 ", ""); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EB.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.2,0.2)", "EB-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Electron (Egamma) regression/std. SC -1 ", ""); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EB.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.1,0.1)", "EE-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Electron (Hgg) regression/std. SC -1 ", ""); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EE.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.1,0.1)", "EE-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Photon (Hgg) regression/std. SC -1 ", ""); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EE.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.1,0.1)", "EE-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Electron (Egamma) regression/std. SC -1 ", ""); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EE.eps");
  delete c;
  
  
  // cluster corrections and ES energy fraction
  c = PlotDataMC(data, signal, "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "absEta_1.7_2.5-"+commonCut,"", dataLabel+*l_itr, mcLabel, "ES energy fraction: ES/(rawSC+ES) ", ""); 
  c->SaveAs(outputPath+"/esEnergyFraction.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", ""); 
  c->SaveAs(outputPath+"/energyCorrections_EB.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", ""); 
  c->SaveAs(outputPath+"/energyCorrections_EE.eps");
  delete c;
  
  // pileup
  c = PlotDataMC(data, signal, "nPV", "(30,0,30)", ""+commonCut, "", dataLabel+*l_itr, mcLabel, "nVtx", ""); 
  c->SaveAs(outputPath+"nPV.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "rho", "(60,-1,29)", ""+commonCut, "", dataLabel+*l_itr, mcLabel, "#rho", ""); 
  c->SaveAs(outputPath+"rho.eps");
  delete c;
  
  // kinematic variables
  c = PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "eleID_WP80PU", "", dataLabel+*l_itr, mcLabel, "energy SC [GeV]", ""); 
  c->SaveAs(outputPath+"energySCEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "etaEle", "(100,-2.5,2.5)", ""+commonCut, "", dataLabel+*l_itr, mcLabel, "#eta", ""); 
  c->SaveAs(outputPath+"etaEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "phiEle", "(100,-5,5)", ""+commonCut, "", dataLabel+*l_itr, mcLabel, "#phi", ""); 
  c->SaveAs(outputPath+"phiEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-"+commonCut, "", dataLabel+*l_itr, mcLabel, "R_{9}", ""); 
  c->SaveAs(outputPath+"R9Ele_EB.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-"+commonCut, "", dataLabel+*l_itr, mcLabel, "R_{9}", "",true); 
  c->SaveAs(outputPath+"R9Ele_EB-log.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EE-"+commonCut, "", dataLabel+*l_itr, mcLabel, "R_{9}", ""); 
  c->SaveAs(outputPath+"R9Ele_EE.eps");
  delete c; 
  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EE-"+commonCut, "", dataLabel+*l_itr, mcLabel, "R_{9}", "",true); 
  c->SaveAs(outputPath+"R9Ele_EE-log.eps");

  // Checking fit selection
}
}

  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Reference Region map", "",1);
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

}

  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(171,-85,86,361,0,361)", "EBRefReg-gold-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Reference Region map", "",1);
  c->SaveAs(outputPath+"EBRefRegMap.eps");
  delete c;
  
  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-"+commonCut,"", dataLabel+*l_itr, mcLabel, "Reference Region map", "",1);
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

  //PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "", dataLabel+*l_itr, "DY Summer 12", "energy SC [GeV]", ""); 
  //PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)<1.5", dataLabel+*l_itr, "DY Summer 12", "R9", ""); 
  //PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)>1.5", dataLabel+*l_itr, "DY Summer 12", "R9", ""); 
// PlotDataMC(data, signal, "R9Ele:etaEle", "(100,-3,3,100,0,1.1)", "", dataLabel+*l_itr, "DY Summer 12", "#eta", "R9"); 
}
}
