#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TChain.h>
#include <TString.h>
#include <TCut.h>
#include <iostream>
#include <TH1F.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TPaveText.h>
#include "src/ElectronCategory_class.cc"
#include <iostream>

TCut GetCut(TString category, int indexEle=0){
  ElectronCategory_class cutter;
  return cutter.GetCut(category, false,indexEle);
}



TCanvas *PlotDataMC2D(TChain *data, TChain *mc, TString branchname, TString binning, 
		      TCut selection, 
		      TString dataLabel, TString mcLabel, 
		      TString xLabel, TString yLabel, 
		      int type=2, TString opt="colz", bool usePU=true, bool smear=false, bool scale=false){
  //type == 0: data only
  //type == 1: MC only
  //type == 2: data/MC
   
  TCanvas *c = new TCanvas("c","");

  if(branchname=="map"){
    branchname="seedXSCEle:seedYSCEle";
    binning="(360,1,361,171,-85,86)";
    yLabel="iEta";
    xLabel="iPhi";
    c->SetGridx();
  }
  data->Draw(branchname+">>data_hist"+binning, selection,opt);
  if(usePU)  mc->Draw(branchname+">>mc_hist"+binning, selection *"puWeight",opt);
  else  mc->Draw(branchname+">>mc_hist"+binning, selection,opt);

  c->Clear();
  
  TH2F *d = (TH2F *) gROOT->FindObject("data_hist");
  TH2F *s = (TH2F *) gROOT->FindObject("mc_hist");

  if(type==0) d->Draw(opt);
  else if(type==1) s->Draw(opt);
  else if(type==3){
    s->Scale(d->Integral()/s->Integral());
    d->Divide(s);
    d->Draw(opt);
  }

  d->GetXaxis()->SetTitle(xLabel);
  s->GetXaxis()->SetTitle(xLabel);
  d->GetYaxis()->SetTitle(yLabel);
  s->GetYaxis()->SetTitle(yLabel);
  c->SetRightMargin(0.2);
  return c;
  
}

TCanvas *PlotDataMC2D(TChain *data, TChain *mc, 
		      TString branchname, TString binning, 
		      TString category, TString selection, 
		      TString dataLabel, TString mcLabel, 
		      TString xLabel, TString yLabel, 
		      int type=2, TString opt="colz",
		      bool usePU=true, bool smear=false, bool scale=false){
  
  TCut sel ="";
  if(category.Sizeof()>1) sel= GetCut(category, 0);
  sel+=selection;
  return PlotDataMC2D(data, mc, branchname, binning, sel, dataLabel, mcLabel, xLabel, yLabel, type, opt,usePU, smear, scale);
}  


TCanvas *PlotDataMC(TChain *data, TChain *mc, TString branchname, TString binning, 
		    TString category, TString selection, 
		    TString dataLabel, TString mcLabel, TString xLabel, TString yLabelUnit, 
		    bool logy=false, bool usePU=true, bool smear=false, bool scale=false){
  
  TString yLabel; 
  
  TCanvas *c = new TCanvas("c","");
  TString branchNameData=branchname;
  TString branchNameMC=branchname;

  ElectronCategory_class cutter;
  TCut selection_data="";
  if(category.Sizeof()>1) selection_data = cutter.GetCut(category, false,0);
  selection_data+=selection;
  TCut selection_MC="";
  if(category.Sizeof()>1) selection_MC = cutter.GetCut(category, false,0);
  selection_MC+=selection;

  if(smear){
    branchNameMC.ReplaceAll("invMass_SC_regrCorr_pho ","(invMass_SC_regrCorr_pho*sqrt(smearEle[0]*smearEle[1]))");
    branchNameMC.ReplaceAll("energySCEle_regrCorr_pho ","(energySCEle_regrCorr_pho*smearEle) ");
    branchNameMC.ReplaceAll("energySCEle_regrCorr_pho[0]","(energySCEle_regrCorr_pho[0]*smearEle[0])");
    branchNameMC.ReplaceAll("energySCEle_regrCorr_pho[1]","(energySCEle_regrCorr_pho[1]*smearEle[1])");

  }
  if(scale){
    branchNameData.ReplaceAll("invMass_SC_regrCorr_pho ","(invMass_SC_regrCorr_pho*sqrt(corrEle[0]*corrEle[1]))");
    branchNameData.ReplaceAll("energySCEle_regrCorr_pho ","(energySCEle_regrCorr_pho*corrEle)");
    branchNameData.ReplaceAll("energySCEle_regrCorr_pho[0]","(energySCEle_regrCorr_pho[0]*corrEle[0])");
    branchNameData.ReplaceAll("energySCEle_regrCorr_pho[1]","(energySCEle_regrCorr_pho[1]*corrEle[1])");
  }
  //std::cout << branchNameData << "\t" << branchNameMC << std::endl;


  if(branchname=="HLT"){
    TH1F *hlt_data = new TH1F("data_hist","",500,0,500);
    TH1F *hlt_mc = new TH1F("mc_hist","",500,0,500);
    
    
    std::vector< std::string > *HLTNames = new std::vector<std::string>; ///< List of HLT names
    std::vector<Bool_t> *HLTResults = new std::vector<Bool_t>;      ///< 0=fail, 1=fire
    
    data->SetBranchAddress("HLTNames", &HLTNames);
    data->SetBranchAddress("HLTResults", &HLTResults);
    
    data->GetEntries();
    for(Long64_t jentry=0; jentry < data->GetEntriesFast(); jentry++){
      data->GetEntry(jentry);
      std::vector<Bool_t>::const_iterator hltRes_itr=HLTResults->begin();
      for(std::vector<std::string>::const_iterator hlt_itr=HLTNames->begin();
	  hlt_itr!=HLTNames->end();
	  hlt_itr++,hltRes_itr++){
	std::cout << *hlt_itr << std::endl;
	hlt_data->Fill(hlt_itr-HLTNames->begin(),*hltRes_itr);
      }
    }
    data->ResetBranchAddresses();

    mc->SetBranchAddress("HLTNames", &HLTNames);
    mc->SetBranchAddress("HLTResults", &HLTResults);
    
    mc->GetEntries();
    for(Long64_t jentry=0; jentry < mc->GetEntriesFast(); jentry++){
      mc->GetEntry(jentry);
      std::vector<Bool_t>::const_iterator hltRes_itr=HLTResults->begin();
      for(std::vector<std::string>::const_iterator hlt_itr=HLTNames->begin();
	  hlt_itr!=HLTNames->end();
	  hlt_itr++,hltRes_itr++){
	std::cout << *hlt_itr << std::endl;
	hlt_mc->Fill(hlt_itr-HLTNames->begin(),*hltRes_itr);
      }
    }

    for(std::vector<std::string>::const_iterator hlt_itr=HLTNames->begin();
	hlt_itr!=HLTNames->end();
	hlt_itr++){
      int index=hlt_itr-HLTNames->begin()+1;
      if(hlt_mc->GetBinContent(index)>0)
      hlt_mc->GetXaxis()->SetBinLabel(index,(*hlt_itr).c_str());
    }
  } else {
    data->Draw(branchNameData+">>data_hist"+binning, selection_data);
    if(mc!=NULL){
      if(usePU)  mc->Draw(branchNameMC+">>mc_hist"+binning, selection_MC *"puWeight");
    else  mc->Draw(branchNameMC+">>mc_hist"+binning, selection_MC);
    }
  }

  c->Clear();
  TH1F *d = (TH1F *) gROOT->FindObject("data_hist");
  TH1F *s = (TH1F *) gROOT->FindObject("mc_hist");
  if(s==NULL) s=d;
  d->SaveAs("tmp/d_hist.root");
  s->SaveAs("tmp/s_hist.root");

  yLabel.Form("Events /(%.2f %s)", s->GetBinWidth(2), yLabelUnit.Data());
  float max = 1.1 * std::max(
			     d->GetMaximum(),///d->Integral(),
			     s->GetMaximum() ///s->Integral()
			     );
  max=1.1*d->GetMaximum();
  std::cout << "max = " << max << std::endl;
  std::cout << "nEvents data: " << d->Integral() << "\t" << d->GetEntries() << std::endl;
  std::cout << "nEvents signal: " << s->Integral() << "\t" << s->GetEntries() << std::endl;
  if(logy){
    max*=10;
    d->GetYaxis()->SetRangeUser(0.1,max);
    s->GetYaxis()->SetRangeUser(0.1,max);
    c->SetLogy();
  } else {
    d->GetYaxis()->SetRangeUser(0,max);
    s->GetYaxis()->SetRangeUser(0,max);
  }
  s->GetYaxis()->SetTitle(yLabel);
  s->GetXaxis()->SetTitle(xLabel);
  d->GetYaxis()->SetTitle(yLabel);
  d->GetXaxis()->SetTitle(xLabel);


  d->SetMarkerStyle(20);
  d->SetMarkerSize(1);
  if(d != s){
    s->SetMarkerStyle(20);
    s->SetMarkerSize(1);
    s->SetFillStyle(3001);
    s->SetFillColor(kRed);
  }

  TH1F* s_norm = (TH1F *) (s->DrawNormalized("hist", d->Integral()));
  //TH1F* d_norm = s_norm;
  //if(d!=s) d_norm = (TH1F *) (d->DrawNormalized("p same", d->Integral()));
  if(d!=s) d->Draw("p same");

  if(logy){
    //d_norm->GetYaxis()->SetRangeUser(0.1,max);
    s_norm->GetYaxis()->SetRangeUser(0.1,max);
    c->SetLogy();
  } else {
    //d_norm->GetYaxis()->SetRangeUser(0,max);  
    s_norm->GetYaxis()->SetRangeUser(0,max);  
  }
  std::cout << "Variable  & Data & Simulation \\" << std::endl;
  std::cout << "Mean      & " << d->GetMean() << " " << d->GetMeanError() 
	    << " & " << s_norm->GetMean() <<  " " << s_norm->GetMeanError() << " \\" << std::endl;
  std::cout << "Std. dev. & " << d->GetRMS() << " " << d->GetRMSError() 
	    << " & " << s_norm->GetRMS() << " " << s_norm->GetRMSError() << " \\" << std::endl;
  std::cout << "\\hline" << std::endl;
  std::cout << "$\\Chi^2$ " <<  d->Chi2Test(s_norm, "UW CHI2/NDF NORM") << std::endl;
  

  TLegend *leg = new TLegend(0.7,0.8,1,1);
  if(dataLabel !="") leg->AddEntry(d,dataLabel,"p");
  if(mcLabel   !="") leg->AddEntry(s,mcLabel, "lf");
  leg->SetBorderSize(1);
  leg->SetFillColor(0);
  leg->SetTextSize(0.05);
  if(dataLabel !="" && mcLabel !="") leg->Draw();
  //c->GetListOfPrimitives()->Add(leg,"");

  TPaveText *pv = new TPaveText(0.2,0.95,0.7,1,"NDC");
  pv->AddText("CMS Preliminary 2012");
  pv->SetFillColor(0);
  pv->SetBorderSize(0);
  pv->Draw();


  return c;

}


#ifdef shervin

TCanvas *PlotHLTDataMC(TChain *data, TChain *mc, TString branchname, TString binning, 
		    TString category, TString selection, 
		    TString dataLabel, TString mcLabel, TString xLabel, TString yLabelUnit, 
		    bool logy=false, bool usePU=true, bool smear=false, bool scale=false){


  TH1F *hlt_data("hlt_data","",500,0,500);
  TH1F *hlt_mc("hlt_mc","",500,0,500);
  

  std::vector< std::string > HLTNames[1]; ///< List of HLT names
  std::vector<Bool_t> HLTResults[1];      ///< 0=fail, 1=fire

  data->SetBranchAddress("HLTNames", HLTNames);
  data->SetBranchAddress("HLTResults", HLTResults);

  data->GetEntries();
  for(Long64_t jentry=0; jentry < data->GetEntriesFast(); jentry++){
    data->GetEntry(jentry);
    std::vector<Bool_t>::const_iterator hltRes_itr=HLTResults[0].begin();
    for(std::vector<std::string>::const_iterator hlt_itr=HLTNames[0].begin();
	hlt_itr!=HLTNames[0].end();
	hlt_itr++,hltRes_itr++){
      hlt_data->Fill(hlt_itr-HLTNames[0].begin(),*hltRes_itr);
    }
  }

  for(std::vector<std::string>::const_iterator hlt_itr=HLTNames[0].begin();
	hlt_itr!=HLTNames[0].end();
	hlt_itr++,hltRes_itr++){
    hlt_data->GetXaxis()->SetBinLabel(hlt_itr-HLTNames[0].begin()+1,(*hlt_itr).c_str());
  }
  
  TCanvas *c = new TCanvas("c","");
  c->cd();
  hlt_data->Draw();
  return c;
}


#endif
