{
	//TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
	//TString outputPath="test/dato/moriond2013/WP80_PU/img/";

	c = PlotDataMC2D(data, signal, "invMass_SC_regrCorr_pho:nPV", "(50,0,50,240,60,120)", "EB-Et_25", "", "Data 2011", "Simulation", "nVtx", "M_{ee}", 1, "profx");
	c->SaveAs(outputPath + "../../img/invMass_SC_regrCorr_pho_vs_nPV-EB-MC.eps");
	delete c;

	c = PlotDataMC2D(data, signal, "invMass_SC_regrCorr_pho:nPV", "(50,0,50,240,60,120)", "EB-Et_25", "", "Data 2011", "Simulation", "nVtx", "M_{ee}", 0, "profx");
	c->SaveAs(outputPath + "../../img/invMass_SC_regrCorr_pho_vs_nPV-EB-data.eps");
	delete c;

	c = PlotDataMC2D(data, signal, "invMass_SC_regrCorr_pho:nPV", "(50,0,50,240,60,120)", "EE-Et_25", "", "Data 2011", "Simulation", "nVtx", "M_{ee}", 1, "profx");
	c->SaveAs(outputPath + "../../img/invMass_SC_regrCorr_pho_vs_nPV-EB-MC.eps");
	delete c;

	c = PlotDataMC2D(data, signal, "invMass_SC_regrCorr_pho:nPV", "(50,0,50,240,60,120)", "EE-Et_25", "", "Data 2011", "Simulation", "nVtx", "M_{ee}", 0, "profx");
	c->SaveAs(outputPath + "../../img/invMass_SC_regrCorr_pho_vs_nPV-EB-data.eps");
	delete c;

	c = PlotDataMC2D(data, signal, "invMass_SC_regrCorr_pho:eventNumber", "(2000,0,2000000000,240,60,120)", "Et_25", "", "Data 2011", "Simulation", "eventNumber", "M_{ee}", 0, "profx");
	c->SaveAs(outputPath + "../../img/invMass_SC_regrCorr_pho_vs_eventNumber-data.eps");
	delete c;
}
c = PlotDataMC2D(data, signal, "nPV:eventNumber", "(2000,0,100000000,100,0,100)", "", "", "Data 2011", "Simulation", "eventNumber", "nVtx", 1, "profx");
c->SaveAs(outputPath + "../../img/nPV_vs_eventNumber-MC.eps");
delete c;

c = PlotDataMC2D(data, signal, "nPV:eventNumber", "(2000,0,2000000000,100,0,100)", "", "", "Data 2011", "Simulation", "eventNumber", "nVtx", 0, "profx");
c->SaveAs(outputPath + "../../img/nPV_vs_eventNumber-data.eps");
delete c;

c = PlotDataMC2D(data, signal, "nPV:runNumber", "(50,190000,209000,100,0,100)", "", "", "Data 2011", "Simulation", "runNumber", "nVtx", 0, "profx");
c->SaveAs(outputPath + "../../img/nPV_vs_runNumber-data.eps");
delete c;


c = PlotDataMC2D(data, signal, "nPV:lumiBlock", "(2000,0,2000,100,0,100)", "", "", "Data 2011", "Simulation", "Lumi section", "nVtx", 0, "profx");
c->SaveAs(outputPath + "../../img/nPV_vs_lumiBlock-data.eps");
delete c;

c = PlotDataMC2D(data, signal, "rho:nPV", "(100,0,100,100,0,100)", "", "", "Data 2011", "Simulation", "nVtx", "#rho", 0);
c->SaveAs(outputPath + "../../img/rho_vs_nPV-data.eps");
delete c;

c = PlotDataMC2D(data, signal, "rho:nPV", "(100,0,100,100,0,100)", "", "", "Data 2011", "Simulation", "nVtx", "#rho", 1);
c->SaveAs(outputPath + "../../img/rho_vs_nPV-MC.eps");
delete c;

c = PlotDataMC2D(data, signal, "rho:nPV", "(100,0,100,100,0,100)", "", "", "Data 2011", "Simulation", "nVtx", "#rho", 3);
c->SaveAs(outputPath + "../../img/rho_vs_nPV-dataMC.eps");
delete c;



}
// cluster corrections and ES energy fraction
c = PlotDataMC(data, signal, "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "absEta_1.7_2.5-eleID_7-Et_25", "", "Data 2011", "Simulation", "ES energy fraction: ES/(rawSC+ES) ", "");
c->SaveAs(outputPath + "esEnergyFraction.eps");
delete c;

c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_7-Et_25", "", "Data 2011", "Simulation", "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "");
c->SaveAs(outputPath + "energyCorrections_EB.eps");
delete c;

c = PlotDataMC(data, signal, "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_7-Et_25", "", "Data 2011", "Simulation", "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "");
c->SaveAs(outputPath + "energyCorrections_EE.eps");
delete c;

// pileup
c = PlotDataMC(data, signal, "nPV", "(30,0,30)", "eleID_7", "", "Data 2011", "Simulation", "nVtx", "");
c->SaveAs(outputPath + "nPV.eps");
delete c;

c = PlotDataMC(data, signal, "rho", "(60,-1,29)", "eleID_7", "", "Data 2011", "Simulation", "#rho", "");
c->SaveAs(outputPath + "rho.eps");
delete c;

// kinematic variables
c = PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "eleID_7", "", "Data 2011", "Simulation", "energy SC [GeV]", "");
c->SaveAs(outputPath + "energySCEle.eps");
delete c;

c = PlotDataMC(data, signal, "etaEle", "(100,-2.5,2.5)", "eleID_7", "", "Data 2011", "Simulation", "#eta", "");
c->SaveAs(outputPath + "etaEle.eps");
delete c;

c = PlotDataMC(data, signal, "phiEle", "(100,-5,5)", "eleID_7", "", "Data 2011", "Simulation", "#phi", "");
c->SaveAs(outputPath + "phiEle.eps");
delete c;

c = PlotDataMC(data, signal, "R9Ele", "(1100,0,1.1)", "EB-eleID_7", "", "Data 2011", "Simulation", "R_{9}", "");
c->SaveAs(outputPath + "R9Ele_EB.eps");
delete c;

c = PlotDataMC(data, signal, "R9Ele", "(1100,0,1.1)", "EE-eleID_7", "", "Data 2011", "Simulation", "R_{9}", "");
c->SaveAs(outputPath + "R9Ele_EE.eps");
delete c;

// Checking fit selection
}
c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25", "", "Data 2011", "Simulation", "Reference Region map", "", 1);
c->SaveAs(outputPath + "EERefRegMap.eps");
delete c;

}

c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(171,-85,86,361,0,361)", "EBRefReg-gold-eleID_7-Et_25", "", "Data 2011", "Simulation", "Reference Region map", "", 1);
c->SaveAs(outputPath + "EBRefRegMap.eps");
delete c;

c = PlotDataMC2D(data, signal, "seedYSCEle:seedXSCEle", "(100,0,100,100,0,100)", "EERefReg-gold-eleID_7-Et_25", "", "Data 2011", "Simulation", "Reference Region map", "", 1);
c->SaveAs(outputPath + "EERefRegMap.eps");
delete c;

//PlotDataMC(data, signal, "energySCEle", "(100,0,200)", "", "Data 2012", "DY Summer 12", "energy SC [GeV]", "");
//PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)<1.5", "Data 2012", "DY Summer 12", "R9", "");
//PlotDataMC(data, signal, "R9Ele", "(100,0,1.1)", "abs(etaEle)>1.5", "Data 2012", "DY Summer 12", "R9", "");
// PlotDataMC(data, signal, "R9Ele:etaEle", "(100,-3,3,100,0,1.1)", "", "Data 2012", "DY Summer 12", "#eta", "R9");
}

