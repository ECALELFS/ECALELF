{
  //TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
  //TString outputPath="test/dato/moriond2013/WP80_PU/img/";
  TString mc1Label="standard Madgraph";
  TString mc2Label="runDep Madgraph";

  // regression comparison
    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EB.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Photon (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EB.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Egamma) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EB.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EE.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Photon (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EE.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Egamma) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EE.eps");
  delete c;


  // cluster corrections and ES energy fraction
  c = PlotDataMC(data, signal, "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "absEta_1.7_2.5-eleID_7-Et_25","", mc1Label,mc2Label, "ES energy fraction: ES/(rawSC+ES) ", "", false, false); 
  c->SaveAs(outputPath+"/esEnergyFraction.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, false); 
  c->SaveAs(outputPath+"/energyCorrections_EB.eps");
  delete c;

  c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, false); 
  c->SaveAs(outputPath+"/energyCorrections_EE.eps");
  delete c;
  
  // pileup
  c = PlotDataMC(data, signal, "nPV", "(30,0,30)", "eleID_7-Et_25", "", mc1Label,mc2Label, "nVtx", "", false, false); 
  c->SaveAs(outputPath+"nPV.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "rho", "(60,-1,29)", "eleID_7-Et_25", "", mc1Label,mc2Label, "#rho", "", false, false); 
  c->SaveAs(outputPath+"rho.eps");
  delete c;

  // kinematic variables
  c = PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "eleID_7", "", mc1Label,mc2Label, "energy SC [GeV]", "", false, false); 
  c->SaveAs(outputPath+"energySCEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "etaEle", "(100,-2.5,2.5)", "eleID_7-Et_25", "", mc1Label,mc2Label, "#eta", "", false, false); 
  c->SaveAs(outputPath+"etaEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "phiEle", "(100,-5,5)", "eleID_7-Et_25", "", mc1Label,mc2Label, "#phi", "", false, false); 
  c->SaveAs(outputPath+"phiEle.eps");
  delete c;

  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "", false, false); 
  c->SaveAs(outputPath+"R9Ele_EB.eps");
  delete c;

  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "",true,  false); 
  c->SaveAs(outputPath+"R9Ele_EB-log.eps");
  delete c;

  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "", false, false); 
  c->SaveAs(outputPath+"R9Ele_EE.eps");
  delete c; 
  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "",true, false); 
  c->SaveAs(outputPath+"R9Ele_EE-log.eps");

  // Checking fit selection
}
  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25","", mc1Label,mc2Label, "Reference Region map", "",1, false, false);
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

}

  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(171,-85,86,361,0,361)", "EBRefReg-gold-eleID_7-Et_25","", mc1Label,mc2Label, "Reference Region map", "",1, false, false);
  c->SaveAs(outputPath+"EBRefRegMap.eps");
  delete c;
  
  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25","", mc1Label,mc2Label, "Reference Region map", "",1, false, false);
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

  //PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "", mc1Label, "DY Summer 12", "energy SC [GeV]", ""); 
  //PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)<1.5", mc1Label, "DY Summer 12", "R9", ""); 
  //PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)>1.5", mc1Label, "DY Summer 12", "R9", ""); 
// PlotDataMC(data, signal, "R9Ele:etaEle", "(100,-3,3,100,0,1.1)", "", mc1Label, "DY Summer 12", "#eta", "R9"); 
}

{
  //TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
  //TString outputPath="test/dato/moriond2013/WP80_PU/img/";
  TString mc1Label="standard Madgraph";
  TString mc2Label="runDep Madgraph";

  // regression comparison
    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EB.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Photon (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EB.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Egamma) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EB.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCEle_stdSC-EE.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Photon (Hgg) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrSCPho_stdSC-EE.eps");
  delete c;

    c = PlotDataMC(data, signal, "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Electron (Egamma) regression/std. SC -1 ", "", false, false); 
  c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EE.eps");
  delete c;


  // cluster corrections and ES energy fraction
  c = PlotDataMC(data, signal, "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "absEta_1.7_2.5-eleID_7-Et_25","", mc1Label,mc2Label, "ES energy fraction: ES/(rawSC+ES) ", "", false, false); 
  c->SaveAs(outputPath+"/esEnergyFraction.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_7-Et_25","", mc1Label,mc2Label, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, false); 
  c->SaveAs(outputPath+"/energyCorrections_EB.eps");
  delete c;

  c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_7-Et_25","", mc1Label,mc2Label, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, false); 
  c->SaveAs(outputPath+"/energyCorrections_EE.eps");
  delete c;
  
  // pileup
  c = PlotDataMC(data, signal, "nPV", "(30,0,30)", "eleID_7-Et_25", "", mc1Label,mc2Label, "nVtx", "", false, false); 
  c->SaveAs(outputPath+"nPV.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "rho", "(60,-1,29)", "eleID_7-Et_25", "", mc1Label,mc2Label, "#rho", "", false, false); 
  c->SaveAs(outputPath+"rho.eps");
  delete c;

  // kinematic variables
  c = PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "eleID_7", "", mc1Label,mc2Label, "energy SC [GeV]", "", false, false); 
  c->SaveAs(outputPath+"energySCEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "etaEle", "(100,-2.5,2.5)", "eleID_7-Et_25", "", mc1Label,mc2Label, "#eta", "", false, false); 
  c->SaveAs(outputPath+"etaEle.eps");
  delete c;
  
  c = PlotDataMC(data, signal, "phiEle", "(100,-5,5)", "eleID_7-Et_25", "", mc1Label,mc2Label, "#phi", "", false, false); 
  c->SaveAs(outputPath+"phiEle.eps");
  delete c;

  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "", false, false); 
  c->SaveAs(outputPath+"R9Ele_EB.eps");
  delete c;

  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "",true,  false); 
  c->SaveAs(outputPath+"R9Ele_EB-log.eps");
  delete c;

  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "", false, false); 
  c->SaveAs(outputPath+"R9Ele_EE.eps");
  delete c; 
  c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EE-eleID_7-Et_25", "", mc1Label,mc2Label, "R_{9}", "",true, false); 
  c->SaveAs(outputPath+"R9Ele_EE-log.eps");

  // Checking fit selection
}
  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25","", mc1Label,mc2Label, "Reference Region map", "",1, false, false);
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

}

  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(171,-85,86,361,0,361)", "EBRefReg-gold-eleID_7-Et_25","", mc1Label,mc2Label, "Reference Region map", "",1, false, false);
  c->SaveAs(outputPath+"EBRefRegMap.eps");
  delete c;
  
  c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25","", mc1Label,mc2Label, "Reference Region map", "",1, false, false);
  c->SaveAs(outputPath+"EERefRegMap.eps");
  delete c;

  //PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "", mc1Label, "DY Summer 12", "energy SC [GeV]", ""); 
  //PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)<1.5", mc1Label, "DY Summer 12", "R9", ""); 
  //PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)>1.5", mc1Label, "DY Summer 12", "R9", ""); 
// PlotDataMC(data, signal, "R9Ele:etaEle", "(100,-3,3,100,0,1.1)", "", mc1Label, "DY Summer 12", "#eta", "R9"); 
}

