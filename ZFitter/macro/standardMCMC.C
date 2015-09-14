{
  int i=0;

  TCanvas *c[200];

  //TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
  //TString outputPath="test/dato/moriond2013/WP80_PU/img/";
  TString mc1Label="Asympt50ns";
  TString mc2Label="StartupFlat10To50";

  std::vector<TString> mcLabel_vec; // create a vector of labels for the MC samples 
  mcLabel_vec.push_back("StartupFlat10To50"); 
  //mcLabel_vec.push_back("Powheg"); 
  //mcLabel_vec.push_back("Sherpa");

  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "invMass_SC", "(30,60,120)", "EB", "", mc1Label,mcLabel_vec, "invMass_SC", "",outputPath,"invMass_SC",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "EB", "", mc1Label,mcLabel_vec, "ES energy fraction: ES/(rawSC+ES)", "",outputPath,"esEnergyFraction",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES})", "",outputPath,"energyCorrections_EB",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "",outputPath,"energyCorrections_EE",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "nPV", "(30,0,30)", "eleID_7-Et_25", "", mc1Label,mcLabel_vec, "nVtx ", "",outputPath,"nPV",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "rho", "(60,-1,29)", "eleID_7-Et_25", "", mc1Label,mcLabel_vec, "#rho ", "",outputPath,"rho",false,false)); i++;
  // kinematic variables
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "energySCEle", "(100,0,200)", "eleID_7", "", mc1Label,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "etaEle", "(100,-2.5,2.5)", "eleID_7-Et_25", "", mc1Label,mcLabel_vec, "#eta ", "",outputPath,"etaEle",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "phiEle", "(100,-3.14,3.14)", "eleID_7-Et_25", "", mc1Label,mcLabel_vec, "#phi ", "",outputPath,"phiEle",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "R9Ele", "(100,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "R9Ele", "(100,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB-log",true,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "R9Ele", "(100,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE",false,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB), "R9Ele", "(100,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE-log",true,false)); i++;

/*
 
  c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9}", "",true,  false)); 
  c->SaveAs(outputPath+"R9Ele_EB-log.eps");
  delete c;

  c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "R9Ele", "(110,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9}", "", false, false)); 
  c->SaveAs(outputPath+"R9Ele_EE.eps");
  delete c; 
  c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "R9Ele", "(110,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mcLabel_vec, "R_{9}", "",true, false)); 
  c->SaveAs(outputPath+"R9Ele_EE-log.eps");

  // Checking fit selection
  c = new TCanvas( PlotDataMC2D(signalA, MakeChainVector(signalB), "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Reference Region map", "",1, false, false));
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;



  c = new TCanvas( PlotDataMC2D(signalA, MakeChainVector(signalB), "seedYSCEle:seedXSCEle", "(171,-85,86,361,0,361)", "EBRefReg-gold-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Reference Region map", "",1, false, false));
  c->SaveAs(outputPath+"EBRefRegMap.eps");
  delete c;
  
  c = new TCanvas( PlotDataMC2D(signalA, MakeChainVector(signalB), "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Reference Region map", "",1, false, false));
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

  //PlotDataMCs(signalA, MakeChainVector(signalB), "energySCEle", "(100,0,200)", "", mc1Label, "DY Summer 12", "energy SC [GeV]", ""); 
  //PlotDataMCs(signalA, MakeChainVector(signalB), "R9Ele", "(100,0,1.1)", "abs(etaEle)<1.5", mc1Label, "DY Summer 12", "R9", ""); 
  //PlotDataMCs(signalA, MakeChainVector(signalB), "R9Ele", "(100,0,1.1)", "abs(etaEle)>1.5", mc1Label, "DY Summer 12", "R9", ""); 
// PlotDataMCs(signalA, MakeChainVector(signalB), "R9Ele:etaEle", "(100,-3,3,100,0,1.1)", "", mc1Label, "DY Summer 12", "#eta", "R9"); 

  //TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
  //TString outputPath="test/dato/moriond2013/WP80_PU/img/";
  TString mc1Label="standard Madgraph";
  TString mcLabel_vec="runDep Madgraph";


    c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Electron (Egamma) regression/std. SC -1 ", "", false, false)); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EB.eps");
  delete c;

    c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Electron (Hgg) regression/std. SC -1 ", "", false, false)); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EE.eps");
  delete c;

    c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Photon (Hgg) regression/std. SC -1 ", "", false, false)); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EE.eps");
  delete c;

    c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Electron (Egamma) regression/std. SC -1 ", "", false, false)); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EE.eps");
  delete c;


  // cluster corrections and ES energy fraction
  c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "absEta_1.7_2.5-eleID_7-Et_25","", mc1Label,mcLabel_vec, "ES energy fraction: ES/(rawSC+ES) ", "", false, false)); 
  c->SaveAs(outputPath+"/esEnergyFraction.eps");
  delete c;
  
  c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, false)); 
  c->SaveAs(outputPath+"/energyCorrections_EB.eps");
  delete c;

  c = new TCanvas( PlotDataMCs(signalA, MakeChainVector(signalB), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_7-Et_25","", mc1Label,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, false)); 
  c->SaveAs(outputPath+"/energyCorrections_EE.eps");
  delete c;
  
*/

}
