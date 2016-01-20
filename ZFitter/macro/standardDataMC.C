{
  int i=0;
  bool	puReweight=true;

  TCanvas *c[200];

  TString dataLabel="data";

  std::vector<TString> mcLabel_vec;
  mcLabel_vec.push_back("madgraph");
  //  mcLabel_vec.push_back("Powheg");
  //  mcLabel_vec.push_back("Sherpa");

  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(60,0,60)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "nVtx ", "",outputPath,"nPV",false,puReweight,true,false)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "invMass_SC", "(30,60,120)", "EB-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "invMass_SC", "",outputPath,"invMass_SC",false,puReweight,true,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "EE-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "ES energy fraction: ES/(rawSC+ES)", "",outputPath,"esEnergyFraction",true,puReweight,true,true)); i++;

//  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES})", "",outputPath,"energyCorrections_EB",false,puReweight,true,false)); i++;
//  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "",outputPath,"energyCorrections_EE",false,puReweight,true,false)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(60,0,60)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "nVtx ", "",outputPath,"nPV-no-pureweight",false,false,true,false)); i++;
//  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(60,0,60)", "EE", "", dataLabel,mcLabel_vec, "nVtx", "",outputPath,"nPV-noreweight-ee",false,false,true,false)); i++;
 // c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(60,0,60)", "EB", "", dataLabel,mcLabel_vec, "nVtx", "",outputPath,"nPV-noreweight-eb",false,false,true,false)); i++;
//  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(60,0,60)", "EE", "", dataLabel,mcLabel_vec, "nVtx", "",outputPath,"nPV-ee",false,true,true,false)); i++;
//  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(60,0,60)", "EB", "", dataLabel,mcLabel_vec, "nVtx", "",outputPath,"nPV--eb",false,true,true,false)); i++;
 
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rho", "(60,-1,29)", "eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "#rho ", "",outputPath,"rho",false,puReweight,true,false)); i++;
  // kinematic variables
 //c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rawESEnergyPlane1SCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"rawESEnergyPlane1SCEle",false,puReweight,true,true)); i++;
 //c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rawESEnergyPlane2SCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"rawESEnergyPlane2SCEle",false,puReweight,true,true)); i++;
 /*
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane1SCEle", "(100,0,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane1SCEle-noPUreweight",false,false,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane1SCEle", "(100,0,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane1SCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane2SCEle", "(100,0,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane2SCEle-noPUreweight",false,false,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane2SCEle", "(100,0,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane2SCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane1SCEle", "(100,1,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane1SCEle-skip1stbin",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane1SCEle", "(100,1,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane1SCEle-noPUreweight-skip1stbin",false,false,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane2SCEle", "(100,1,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane2SCEle-skip1stbin",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergyPlane2SCEle", "(100,1,50)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergyPlane2SCEle-noPUreweight-skip1stbin",false,false,true,true)); i++;
*/
/*
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle",false,puReweight,true,true)); i++;
*/


//c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "EE-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle_EE",false,puReweight,true,true)); i++;
// c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "energySCEle", "(100,0,200)", "EB-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"energySCEle_EB",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rawEnergySCEle", "(100,0,200)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"rawEnergySCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rawEnergySCEle", "(100,0,200)", "EE-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"rawEnergySCEle_EE",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "rawEnergySCEle", "(100,0,200)", "EB-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"rawEnergySCEle_EB",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergySCEle", "(100,2,30)", "EE-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergySCEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "esEnergySCEle", "(100,0,30)", "EE-eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "energy SC [GeV] ", "",outputPath,"esEnergySCEle-noCut",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "etaEle", "(100,-2.5,2.5)", "eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "#eta ", "",outputPath,"etaEle",false,puReweight,true,true)); i++;
 c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "phiEle", "(100,-3.14,3.14)", "eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "#phi ", "",outputPath,"phiEle",false,puReweight,true,true)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EB-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB",false,puReweight,true,true)); i++;
//  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EB-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EB-log",true,puReweight,true,true)); i++;
  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EE-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE",false,puReweight,true,true)); i++;
 // c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "R9Ele", "(100,0.3,1.4)", "EE-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "R_{9} ", "",outputPath,"R9Ele_EE-log",true,puReweight,true,true)); i++;

}
