{
	int i = 0;

	TCanvas *c[200];

	//TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
	//TString outputPath="test/dato/moriond2013/WP80_PU/img/";
	//TString mc1Label="pythia";
	TString mc1Label = "madgraph old";
	TString mc2Label = "v4";

	std::vector<TString> mcLabel_vec; // create a vector of labels for the MC samples
	mcLabel_vec.push_back("amcatnlo old");
	mcLabel_vec.push_back("madgraph new");
	mcLabel_vec.push_back("amcatnlo new");
	//mcLabel_vec.push_back("Powheg");
	//mcLabel_vec.push_back("Sherpa");

	//c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "invMass_SC", "(30,60,120)", "EB", "", mc1Label,mcLabel_vec, "invMass_SC", "",outputPath,"invMass_SC",false,false,true,false)); i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "EB", "", mc1Label, mcLabel_vec, "ES energy fraction: ES/(rawSC+ES)", "", outputPath, "esEnergyFraction", false, false, true, false));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES})", "", outputPath, "energyCorrections_EB", false, false, true, false));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", outputPath, "energyCorrections_EE", false, false, true, false));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "nPV", "(30,0,30)", "eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "nVtx ", "", outputPath, "nPV", false, false, true, false));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "rho", "(60,-1,29)", "eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "#rho ", "", outputPath, "rho", false, false, true, false));
	i++;
	// kinematic variables
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "rawEnergySCEle", "(100,0,200)", "eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "RAW energy SC [GeV] ", "", outputPath, "rawEnergySCEle", false, false, true, true));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "energySCEle", "(100,0,200)", "eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "energy SC [GeV] ", "", outputPath, "energySCEle", false, false, true, true));
	i++;

	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "etaEle", "(100,-2.5,2.5)", "eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "#eta ", "", outputPath, "etaEle", false, false, true, true));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "phiEle", "(100,-3.14,3.14)", "eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "#phi ", "", outputPath, "phiEle", false, false, true, true));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "R9Ele", "(100,0.3,1.4)", "EB-eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "R_{9} ", "", outputPath, "R9Ele_EB", false, false, true, true));
	i++;
//  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "R9Ele", "(100,0.3,1.4)", "EB-eleID_medium50nsRun2-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB-log",true,false,true,true)); i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "R9Ele", "(100,0.3,1.4)", "EE-eleID_medium50nsRun2-Et_25", "", mc1Label, mcLabel_vec, "R_{9} ", "", outputPath, "R9Ele_EE", false, false, true, true));
	i++;
//  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "R9Ele", "(100,0.3,1.4)", "EE-eleID_medium50nsRun2-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE-log",true,false,true,true)); i++;
//  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "R9Ele", "(100,0.8,1.02)", "EB-eleID_medium50nsRun2-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB-zoom",false,false,true,true)); i++;
//  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "R9Ele", "(100,0.8,1.02)", "EB-eleID_medium50nsRun2-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB-log-zoom",true,false,true,true)); i++;
//  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "R9Ele", "(100,0.8,1.02)", "EE-eleID_medium50nsRun2-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE-zoom",false,false,true,true)); i++;
//  c[i] = new TCanvas (PlotDataMCs(signalA,MakeChainVector(signalB,signalC,signalD), "R9Ele", "(100,0.8,1.02)", "EE-eleID_medium50nsRun2-Et_25", "", mc1Label,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE-log-zoom",true,false,true,true)); i++;

	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_medium25nsRun2-Et_25", "", mc1Label, mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES})", "", outputPath, "energyCorrections_EB", false, false, true, false));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_medium25nsRun2-Et_25", "", mc1Label, mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", outputPath, "energyCorrections_EE", false, false, true, false));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "esEnergySCEle", "(100,2,30)", "EE-eleID_medium25nsRun2", "", mc1Label, mcLabel_vec, "energy SC [GeV] ", "", outputPath, "esEnergySCEle", false, false, true, true));
	i++;
	c[i] = new TCanvas (PlotDataMCs(signalA, MakeChainVector(signalB, signalC, signalD), "esEnergySCEle", "(100,0,30)", "EE-eleID_medium25nsRun2", "", mc1Label, mcLabel_vec, "energy SC [GeV] ", "", outputPath, "esEnergySCEle-noCut", false, false, true, true));
	i++;


}
