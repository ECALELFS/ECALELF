{
  int i=0;

  TCanvas *c[200];

  TString dataLabel="data";

  std::vector<TString> mcLabel_vec;
  mcLabel_vec.push_back("MC");
  //  mcLabel_vec.push_back("Powheg");
  //  mcLabel_vec.push_back("Sherpa");

  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "invMass_SC", "(30,60,120)", "EB", "", dataLabel,mcLabel_vec, "invMass_SC", "",outputPath,"invMass_SC",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "EB", "", dataLabel,mcLabel_vec, "ES energy fraction: ES/(rawSC+ES)", "",outputPath,"esEnergyFraction",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_7-Et_25", "", dataLabel,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES})", "",outputPath,"energyCorrections_EB",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_7-Et_25", "", dataLabel,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "",outputPath,"energyCorrections_EE",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(30,0,30)", "eleID_7-Et_25", "", dataLabel,mcLabel_vec, "nVtx ", "",outputPath,"nPV",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rho", "(60,-1,29)", "eleID_7-Et_25", "", dataLabel,mcLabel_vec, "#rho ", "",outputPath,"rho",false,false)); i++;
  // kinematic variables
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "eleID_7", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "etaEle", "(100,-2.5,2.5)", "eleID_7-Et_25", "", dataLabel,mcLabel_vec, "#eta ", "",outputPath,"etaEle",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "phiEle", "(100,-3.14,3.14)", "eleID_7-Et_25", "", dataLabel,mcLabel_vec, "#phi ", "",outputPath,"phiEle",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EB-eleID_7-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EB-eleID_7-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB-log",true,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EE-eleID_7-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EE-eleID_7-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE-log",true,false)); i++;

/*

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
    }

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

*/
}
