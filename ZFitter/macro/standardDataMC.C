{
  int i=0;
  bool	puReweight=true;

  TCanvas *c[200];

  TString dataLabel="Data new0T";

  std::vector<TString> mcLabel_vec;
  mcLabel_vec.push_back("MC 0T");
  //  mcLabel_vec.push_back("Powheg");
  //  mcLabel_vec.push_back("Sherpa");

//  c[i] =
  TCanvas *cc = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "nPV", "(40,0,40)", "eleID_medium25nsRun2", "", dataLabel,mcLabel_vec, "nVtx ", "",outputPath,"nPV",false,puReweight,true,false)); i++;


  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "invMass_SC_must_regrCorr_ele", "(120,60,120)", "EB-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "M_{ee} [GeV]", "",outputPath,"invMass_SC_must_regrCorr_ele-EB",false,puReweight,true,false)); i++;

  c[i] = new TCanvas (PlotDataMCs(data,MakeChainVector(signalA), "invMass_SC_must_regrCorr_ele", "(120,60,120)", "EE-eleID_medium25nsRun2-Et_25", "", dataLabel,mcLabel_vec, "M_{ee} [GeV]", "",outputPath,"invMass_SC_must_regrCorr_ele-EE",false,puReweight,true,false)); i++;

}
