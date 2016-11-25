{
//  std::cout<<"scaling"<<std::endl;
//  TH1F *data_start=new TH1F("data_start","data_start",2000,0,2000);
//  TH1F *MC_start=new TH1F("MC_start","MC_start",2000,0,2000);
//  data->Draw("energySCEle[0]/cosh(etaSCEle[0])>>data_start","(eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
//  signalA->Draw("energySCEle[0]/cosh(etaSCEle[0])>>MC_start","(eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
//
//  double scale_factor=data_start->Integral()/MC_start->Integral();
//
//  double scale_factor_data=30/2.5;
//
//  MC_start->Scale(scale_factor);
//  MC_start->SetFillColor(kRed);
//  MC_start->SetLineColor(kRed);
//  MC_start->Draw("hist");
//  data_start->Draw("samep");
//  c1->SetLogx();
//  c1->SaveAs("Cat_histos/check_norm.png");
//  std::cout<<scale_factor<<" "<<scale_factor_data<<std::endl;

  //4 => lowEta highR9
  std::cout<<"Making histograms for 30 fb expected"<<std::endl;
  TH2F *Et_hist_MC=new TH2F("Et_hist_MC","Et_hist_MC",200,0,500,200,0,500);
  TH2F *Et_hist_MC_all=new TH2F("Et_hist_MC_all","Et_hist_MC_all",200,0,500,200,0,500);
  TH2F *Et_hist_data=new TH2F("Et_hist_data","Et_hist_data",200,0,500,200,0,500);
  signalA->Draw("energySCEle[0]/cosh(etaSCEle[0]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_MC","smearerCat[0]==4 && invMass_SC> 80 && invMass_SC<100 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
  signalA->Draw("energySCEle[0]/cosh(etaSCEle[0]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_MC_all","smearerCat[0]==4 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
  data->Draw("energySCEle[0]/cosh(etaSCEle[0]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_data","smearerCat[0]==4 && invMass_SC> 80 && invMass_SC<100 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
  //std::cout<<scale_factor*scale_factor_data<<std::endl;
  //Et_hist_MC->Scale(scale_factor*scale_factor_data);
  TH1F *mass_MC=new TH1F("mass_MC","mass_MC",2000,0,1000);
  data->Draw("invMass_SC>>mass_MC","smearerCat[0]==4 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
  Et_hist_MC->Scale(5.4);
  Et_hist_MC_all->Scale(5.4);
  Et_hist_MC->SaveAs("Cat_histos/Et_hist_30fb_lowEta_highR9.root");
  Et_hist_MC_all->SaveAs("Cat_histos/Et_hist_30fb_lowEta_highR9_all.root");
  Et_hist_data->SaveAs("Cat_histos/Et_hist_30fb_lowEta_highR9_data.root");
  mass_MC->SaveAs("Cat_histos/mass_MC.root");

}
