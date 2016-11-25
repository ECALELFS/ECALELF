{
  TString OutputPath="~/scratch1/www/RUN2_ECAL_Calibration/Ntuple_validation/";
  std::vector<TString> mcLabel_vec;
  mcLabel_vec.push_back("amctnlo");
  mcLabel_vec.push_back("Madgraph");
  TString cut="eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose-noPF-Et_25";
  TString BB="-EB";
  TString EE="-EE";
  ElectronCategory_class cutter;
  //(region,isMC,nEle (0 is both, 1 or 2),iscorr) 
  TString B0=cutter.GetCut("EB",0,1,0).GetTitle();
  TString B1=cutter.GetCut("EB",0,2,0).GetTitle();
  TString E0=cutter.GetCut("EE",0,1,0).GetTitle();
  TString E1=cutter.GetCut("EE",0,2,0).GetTitle();
  TString BE=B0+"&&"+E1+"||"+B1+"&&"+E0;

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass", "(100,80,100)", cut+BB, "", "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_BB.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass", "(100,80,100)", cut+EE, "", "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_EE.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass", "(100,80,100)", cut, BE, "DATA", mcLabel_vec, "Mass [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_BE.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC", "(100,80,100)", cut+BB, "", "DATA", mcLabel_vec, "Mass_SC [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_SC_BB.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC", "(100,80,100)", cut+EE, "", "DATA", mcLabel_vec, "Mass_SC [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_SC_EE.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC", "(100,80,100)", cut, BE, "DATA", mcLabel_vec, "Mass_SC [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_SC_BE.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_corr", "(100,80,100)", cut+BB, "", "DATA", mcLabel_vec, "Mass_SC_corr [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_SC_corr_BB.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_corr", "(100,80,100)", cut+EE, "", "DATA", mcLabel_vec, "Mass_SC_corr [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_SC_corr_EE.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_corr", "(100,80,100)", cut, BE, "DATA", mcLabel_vec, "Mass_SC_corr [GeV]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"invMass_SC_corr_BE.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "R9Ele[0]", "(100,0,1)", cut, B0, "DATA", mcLabel_vec, "R9Ele[0]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"R9_ele0_B.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "R9Ele[1]", "(100,0,1)", cut, B1, "DATA", mcLabel_vec, "R9Ele[1]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"R9_ele1_B.png"); 


  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "etaEle[0]", "(100,-1.5,1.5)", cut, B0, "DATA", mcLabel_vec, "etaEle[0]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"eta_ele0_B.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "etaEle[1]", "(100,-1.5,1.5)", cut, B1, "DATA", mcLabel_vec, "etaEle[1]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"eta_ele1_B.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "etaEle[0]", "(200,-2.5,2.5)", cut, E0, "DATA", mcLabel_vec, "etaEle[0]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"eta_ele0_E.png"); 

  c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "etaEle[1]", "(200,-2.5,2.5)", cut, E1, "DATA", mcLabel_vec, "etaEle[1]","", false, true,false, false, false);
  c->SaveAs(OutputPath+"eta_ele1_E.png"); 


}
