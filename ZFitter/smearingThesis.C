{
  std::vector<TString> mcLabel_vec;
  mcLabel_vec.clear();
  mcLabel_vec.push_back("Simulation");
  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_20_25", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_20_25.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_20_25.C");
  delete c;

  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_25_30", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_25_30.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_25_30.C");
  delete c;

  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_30_35", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_30_35.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_30_35.C");
  delete c;



  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_35_40", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_35_40.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_35_40.C");
  delete c;


  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_40_45", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_40_45.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_40_45.C");
  delete c;


  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_45_50", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_45_50.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_45_50.C");
  delete c;


  c = PlotDataMCs(signalA, MakeChainVector(signalA), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB-Et_50_55", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true); 
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_50_55.eps");
  c->SaveAs(outputPath+"invMass_SC_regrCorrSemiParV5_ele-EB-Et_50_55.C");
  delete c;

}
