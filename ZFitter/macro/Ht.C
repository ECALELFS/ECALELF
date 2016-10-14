{
  TH2F *Et_hist_data_4=new TH2F("Et_hist_data_4","Et_hist_data_4",200,0,500,200,0,500);
  TH2F *Et_hist_MC_4=new TH2F("Et_hist_MC_4","Et_hist_MC_4",200,0,500,200,0,500);
  Et_hist_data_4->GetXaxis()->SetTitle("E_{T}[1] [GeV]");
  Et_hist_data_4->GetYaxis()->SetTitle("E_{T}[0] [GeV]");

  //#include <fstream> 
  std::ofstream ofs ("Cat_histos/test.txt", std::ofstream::out);
  ofs << "lorem ipsum";
  ofs.close();

//  data->Draw("energySCEle[0]/cosh(etaSCEle[0]) + energySCEle[1]/cosh(etaSCEle[1]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_data_4","smearerCat[0]==4 && invMass_SC> 80 && invMass_SC<100 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
//  signalA->Draw("energySCEle[0]/cosh(etaSCEle[0]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_MC_4","smearerCat[0]==4 && invMass_SC> 80 && invMass_SC<100 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
//  Et_hist_data_4->SaveAs("Cat_histos/Et_hist_smearerCat_4_random_data.root");
//  Et_hist_MC_4->SaveAs("Cat_histos/Et_hist_smearerCat_4_random_MC.root");
//
//  TH2F *Et_hist_data_4_all=new TH2F("Et_hist_data_4_all","Et_hist_data_4_all",200,0,500,200,0,500);
//  TH2F *Et_hist_MC_4_all  =new TH2F("Et_hist_MC_4_all","Et_hist_MC_4_all",200,0,500,200,0,500);
//  data->Draw("energySCEle[0]/cosh(etaSCEle[0]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_data_4_all","smearerCat[0]==4 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
//  signalA->Draw("energySCEle[0]/cosh(etaSCEle[0]): energySCEle[1]/cosh(etaSCEle[1])>>Et_hist_MC_4_all","smearerCat[0]==4 && (eleID[0] && 131072) &&(eleID[1] & 131072)&&(energySCEle_pho_regrCorr[0]/cosh(etaSCEle[0]) >= 20)&&(energySCEle_pho_regrCorr[1]/cosh(etaSCEle[1]) >= 20)&&recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1");
//  Et_hist_data_4->SaveAs("Cat_histos/Et_hist_smearerCat_4_random_data_all.root");
//  Et_hist_MC_4->SaveAs("Cat_histos/Et_hist_smearerCat_4_random_MC_all.root");

}
