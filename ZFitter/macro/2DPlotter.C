{
  //La macro che disegna e' in ZFitter/macro/PlotDataMC.C
  TString OutputPath="tmp/Plots/";
  std::vector<TString> mcLabel_vec;

  mcLabel_vec.push_back("Madgraph");
  //mcLabel_vec.push_back("Powheg");
  //mcLabel_vec.push_back("Sherpa");

  //gStyle->SetPalette(1);  

  ElectronCategory_class el;
  //TCut ElectronCategory_class::GetCut(TString region, bool isMC, int nEle, bool corrEle){

  TString pt_Ratio="(energySCEle_regrCorrSemiParV5_ele[0]/cosh(etaSCEle[0]) > energySCEle_regrCorrSemiParV5_ele[1]/cosh(etaSCEle[1]))*(energySCEle_regrCorrSemiParV5_ele[0]*cosh(etaSCEle[1]))/(cosh(etaSCEle[0])*energySCEle_regrCorrSemiParV5_ele[1]) + (energySCEle_regrCorrSemiParV5_ele[0]/cosh(etaSCEle[0]) < energySCEle_regrCorrSemiParV5_ele[1]/cosh(etaSCEle[1]))*(energySCEle_regrCorrSemiParV5_ele[1]*cosh(etaSCEle[0]))/(cosh(etaSCEle[1])*energySCEle_regrCorrSemiParV5_ele[0])";
  TString pt2_Sum="sqrt(pow(energySCEle_regrCorrSemiParV5_ele[0]/cosh(etaSCEle[0]),2) + pow(energySCEle_regrCorrSemiParV5_ele[1]/cosh(etaSCEle[1]) ,2))";

  /*PlotDataMC2D(TChain *data, TChain *mc, TString branchname, TString binning,
	       TString category, TString selection,
	       TString dataLabel, TString mcLabel,
	       TString xLabel, TString yLabel,
	       int type=2, TString opt="colz", bool usePU=true, bool smear=false, bool scale=false, bool single_ele=false,int ele_index=0,TString name_root="name_root")*/

  //1D plot
  //c = PlotDataMCs(data, MakeChainVector(signalA),pt2_Sum, "", "DATA", mcLabel_vec, "pt2Sum [GeV]","", false, true,false, true, true);
  //c->SaveAs(OutputPath+"pt2Sum.png");
  //c = PlotDataMCs(data, MakeChainVector(signalA),pt_Ratio, "(30,0.5,3)", "eleID_loose-trigger-noPF-EB", "", "DATA", mcLabel_vec, "ptRatio","", false, true,false, true, true);
  //c->SaveAs(OutputPath+"ptRatio.png");

  /***********///2D plot for correlation

  //global correlation
  c = PlotDataMC2D(data,signalA,pt2_Sum+":"+pt_Ratio,"(30,0.5,2,200,0,200)","eleID_loose-trigger-noPF-ZPt_0_10","","data_label","mc_label","ptRatio","pt2Sum",0);
  c->SaveAs(OutputPath+"Correlation_pt2Sum_ptRatio.png");
  //BB correlation
  c = PlotDataMC2D(data,signalA,pt2_Sum+":"+pt_Ratio,"(30,0.5,2,200,0,200)","eleID_loose-trigger-noPF-ZPt_0_10-EB","","data_label","mc_label","ptRatio","pt2Sum",0);
  c->SaveAs(OutputPath+"Correlation_pt2Sum_ptRatio_BB.png");
  //EE correlation
  c = PlotDataMC2D(data,signalA,pt2_Sum+":"+pt_Ratio,"(30,0.5,2,200,0,200)","eleID_loose-trigger-noPF-ZPt_0_10-EE","","data_label","mc_label","ptRatio","pt2Sum",0);
  c->SaveAs(OutputPath+"Correlation_pt2Sum_ptRatio_EE.png");
  //BE correlation
  TString BE="((abs(etaEle[0]) < 1.4442)&&(abs(etaEle[1]) > 1.566 && abs(etaEle[1]) < 2.5))||((abs(etaEle[1]) < 1.4442)&&(abs(etaEle[0]) > 1.566 && abs(etaEle[0]) < 2.5))";
  cout<<BE<<endl;
  c = PlotDataMC2D(data,signalA,pt2_Sum+":"+pt_Ratio,"(30,0.5,2,200,0,200)","eleID_loose-trigger-noPF-ZPt_0_10",BE,"data_label","mc_label","ptRatio","pt2Sum",0);
  c->SaveAs(OutputPath+"Correlation_pt2Sum_ptRatio_BE.png");


}

//If you need to do a profile
//c = Plot2D_profileX_my(data,"energySCEle[0]:etaSCEle[0]","(10,-2,2,70,130,200)","","colz","eta SC Ele [0]","energy SC Ele [0]"); //che deficiente il tizio di root => y:x e poi x,xmin,xmax, y, ymin,ymax 
