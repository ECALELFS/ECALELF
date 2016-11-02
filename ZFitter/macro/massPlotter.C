{
  //comparison plots: data+scale & MC+smear
  TString OutputPath="tmp/Plots/";
  TString mass="invMass_SC_corr";
  std::vector<TString> mcLabel_vec;
  //mcLabel_vec.push_back("amctnlo");
  //mcLabel_vec.push_back("Madgraph");
  mcLabel_vec.push_back("Madgraph + amctnlo");
  //eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose #questa seconda stringa la prendi da ZNtupleDumper/interface/eleIDMap.h
  //PlotDataMCs defined in macro/PlotDataMC.C

  //c = PlotDataMCs(data, MakeChainVector(signalA), mass, "(100,80,100)", "eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-EB", "", "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false);
  //c->SaveAs(OutputPath+mass+"_BB.png"); 
  //c->SaveAs(OutputPath+mass+"_BB.C"); 

  //Smearing and Scale applied with (same-as-before,true,true)
  //c = PlotDataMCs(data, MakeChainVector(signalA), mass, "(100,80,100)", "eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-EB", "", "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false, true, true);
  //c->SaveAs(OutputPath+mass+"_BB_withCorr.png"); 
  //c->SaveAs(OutputPath+mass+"_BB_withCorr.C"); 

  c = PlotDataMCs(data, MakeChainVector(signalA), mass, "(100,80,100)", "eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-EB", "", "DATA", mcLabel_vec, "Mass [GeV]","", true, true,false, false, false, true, true);
  c->SaveAs(OutputPath+mass+"_BB_withCorr_logy.png"); 
  c->SaveAs(OutputPath+mass+"_BB_withCorr_logy.C"); 


//
//  c = PlotDataMCs(data, MakeChainVector(signalA), mass, "(100,80,100)", "eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-EE", "", "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false);
//  c->SaveAs(OutputPath+mass+"_EE.png"); 
//  c->SaveAs(OutputPath+mass+"_EE.C"); 
//
//  //Smearing and Scale applied with (same-as-before,true,true)
  //c = PlotDataMCs(data, MakeChainVector(signalA), mass, "(100,80,100)", "eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-EE", "", "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false, true, true);
  //c->SaveAs(OutputPath+mass+"_EE_withCorr.png"); 
  //c->SaveAs(OutputPath+mass+"_EE_withCorr.C"); 

  //c = PlotDataMCs(data, MakeChainVector(signalA), mass, "(300,60,120)", "eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-EE", "", "DATA", mcLabel_vec, "Mass [GeV]","", true, true,false, false, false, true, true);
  //c->SaveAs(OutputPath+mass+"_EE_withCorr_larger_logy.png"); 
  //c->SaveAs(OutputPath+mass+"_EE_withCorr_larger_logy.C"); 

}
