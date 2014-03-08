#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TChain.h>
#include <TString.h>
#include <TCut.h>
#include <iostream>
#include <TH1F.h>
#include <TProfile.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStopwatch.h>
#include "src/ElectronCategory_class.cc"
#include <iostream>
#include <fstream>
#include "src/runDivide_class.cc"

TString GetRunRangeTime(TChain *chain, TString runRange){
  runDivide_class runDivide;
  runDivide.LoadRunEventNumbers(chain);
  return runDivide.GetRunRangeTime(runRange);
}

std::vector<TString> ReadRegionsFromFile(TString fileName){
  ifstream file(fileName);
  std::vector<TString> regions;
  TString region;

  while(file.peek()!=EOF && file.good()){
    if(file.peek()==10){ // 10 = \n
      file.get(); 
      continue;
    }

    if(file.peek() == 35){ // 35 = #
      file.ignore(1000,10); // ignore the rest of the line until \n
      continue;
    }

    file >> region;
    file.ignore(1000,10); // ignore the rest of the line until \n
#ifdef DEBUG
    std::cout << "[DEBUG] Reading region: " << region<< std::endl;
#endif
    regions.push_back(region);

  }
  return regions;
}


void DisableBranchNameNtuple(TChain *chain, TString region){
  ElectronCategory_class cutter;
  std::set<TString> tmpList = cutter.GetBranchNameNtuple(region);
  chain->SetBranchStatus("*",1);

  for(std::set<TString>::const_iterator branch_itr= tmpList.begin();
      branch_itr!=tmpList.end();
      branch_itr++){
    chain->SetBranchStatus(*branch_itr,1);
  }

}

TCut GetCut(TString category, int indexEle=0){
  ElectronCategory_class cutter;
  return cutter.GetCut(category, false,indexEle);
}

/*TCanvas *Plot2D_my(TChain *data, TChain *mc, TString branchname, TString binning, 
		      TCut selection, 
		      TString dataLabel, TString mcLabel, 
		      TString xLabel, TString yLabel, 
		      int type=2, TString opt="colz", bool usePU=true, bool smear=false, bool scale=false){*/
/*TCanvas *PlotDataMCs(TChain *data, std::vector<TChain *> mc_vec, TString branchname, TString binning,
                     TString category, TString selection,
                     TString dataLabel, std::vector<TString> mcLabel_vec, TString xLabel, TString yLabelUnit,
                     bool logy=false, bool usePU=true, bool ratio=true,bool smear=false, bool scale=false, bool useR9Weight=false){*/

TCanvas *Plot2D_profileX_my(TChain *data, TString branchname, TString binning,TString selection,TString opt,TString xLabel, TString yLabel){
  //type == 0: data only
  //type == 1: MC only
  //type == 2: data/MC
   
  TCanvas *c = new TCanvas("c","");
  data->Draw(branchname+">>data_hist"+binning,selection,opt);
  TH2F *d = (TH2F *) gROOT->FindObject("data_hist");

  TCanvas *c1 = new TCanvas("c1","");
  TProfile *prof = d->ProfileX("prof",1,-1,"s");
  prof->SetMarkerStyle(20);
  prof->SetMarkerSize(1);
  prof->Draw();
  prof->GetYaxis()->SetTitle(yLabel);
  prof->GetXaxis()->SetTitle(xLabel);

  return c1;
}

TCanvas *Plot2D_my(TChain *data, TString branchname, TString binning,TString selection,TString opt,TString xLabel, TString yLabel){
  //type == 0: data only
  //type == 1: MC only
  //type == 2: data/MC
   
  TCanvas *c = new TCanvas();
  c->SetRightMargin(0.2);
  //  data->Draw(branchname+">>data_hist"+binning, selection,opt);
  data->Draw(branchname+">>data_hist"+binning,selection,opt);
  TH2F *d = (TH2F *) gROOT->FindObject("data_hist");
  d->GetYaxis()->SetTitle(yLabel);
  d->GetXaxis()->SetTitle(xLabel);
  d->GetZaxis()->SetTitle("Events");
  d->GetZaxis()->SetNdivisions(510);
  d->Draw("colz");

  return c;
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
  TString weightVar;
  if(branchname.Contains("map")){
    weightVar=branchname.ReplaceAll("_map","").ReplaceAll("map","");
    branchname="seedXSCEle:seedYSCEle";
    binning="(361,-0.5,360.5,171,-85.5,85.5)";
    yLabel="iEta";
    xLabel="iPhi";
    c->SetGridx();
  }
  if(weightVar!="") selection+="*"+weightVar;
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
  if(category.Sizeof()>1) selection_data = cutter.GetCut(category, false,0,true);
  selection_data.Print();
  return NULL;
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
    branchNameData.ReplaceAll("energySCEle_regrCorr_ele ","(energySCEle_regrCorr_ele*corrEle)");
    branchNameData.ReplaceAll("energySCEle_regrCorr_ele[0]","(energySCEle_regrCorr_ele[0]*corrEle[0])");
    branchNameData.ReplaceAll("energySCEle_regrCorr_ele[1]","(energySCEle_regrCorr_ele[1]*corrEle[1])");

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
  //d->SaveAs("tmp/d_hist.root");
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
  

  TLegend *leg = new TLegend(0.6,0.8,1,1);
  if(dataLabel !="") leg->AddEntry(d,dataLabel,"p");
  if(mcLabel   !="") leg->AddEntry(s,mcLabel, "lf");
  leg->SetBorderSize(1);
  leg->SetFillColor(0);
  leg->SetTextSize(0.04);
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


TCanvas *PlotDataMCMC(TChain *data, TChain *mc, TChain *mc2,
		      TString branchname, TString binning, 
		      TString category, TString selection, 
		      TString dataLabel, TString mcLabel, TString mc2Label,
		      TString xLabel, TString yLabelUnit, 
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
  if(category.Sizeof()>1) selection_MC = cutter.GetCut(category, true,0);
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


  data->Draw(branchNameData+">>data_hist"+binning, selection_data);
  if(mc!=NULL){
    if(usePU)  mc->Draw(branchNameMC+">>mc_hist"+binning, selection_MC *"puWeight");
    else  mc->Draw(branchNameMC+">>mc_hist"+binning, selection_MC);
  }
  if(mc2!=NULL){
    if(usePU)  mc2->Draw(branchNameMC+">>mc2_hist"+binning, selection_data *"puWeight");
    else  mc2->Draw(branchNameMC+">>mc2_hist"+binning, selection_data);
  }
  

  c->Clear();
  TH1F *d = (TH1F *) gROOT->FindObject("data_hist");
  TH1F *s = (TH1F *) gROOT->FindObject("mc_hist");
  TH1F *s2 = (TH1F *) gROOT->FindObject("mc2_hist");
  if(s==NULL){
    std::cerr << "[WARNING] no mc" << std::endl;
    s=d;
  }
  if(s2==NULL){
    std::cerr << "[WARNING] no mc2" << std::endl;
    s2=s;
  }
  std::cout << s->GetEntries() << "\t" << s2->GetEntries() << "\t" << d->GetEntries() << std::endl;

  //d->SaveAs("tmp/d_hist.root");
  s->SaveAs("tmp/s_hist.root");
  s2->SaveAs("tmp/s2_hist.root");


  yLabel.Form("Events /(%.2f %s)", s->GetBinWidth(2), yLabelUnit.Data());
  float max = 1.1 * std::max(
			     d->GetMaximum(),///d->Integral(),
			     s->GetMaximum() ///s->Integral()
			     );
  max=1.1*d->GetMaximum();
  std::cout << "max = " << max << std::endl;
  std::cout << "nEvents data: " << d->Integral() << "\t" << d->GetEntries() << std::endl;
  std::cout << "nEvents signal: " << s->Integral() << "\t" << s->GetEntries() << std::endl;
  std::cout << "nEvents signal2: " << s2->Integral() << "\t" << s2->GetEntries() << std::endl;
  if(logy){
    max*=10;
    d->GetYaxis()->SetRangeUser(0.1,max);
    s->GetYaxis()->SetRangeUser(0.1,max);
    s2->GetYaxis()->SetRangeUser(0.1,max);
    c->SetLogy();
  } else {
    d->GetYaxis()->SetRangeUser(0,max);
    s->GetYaxis()->SetRangeUser(0,max);
    s2->GetYaxis()->SetRangeUser(0,max);
  }
  s->GetYaxis()->SetTitle(yLabel);
  s->GetXaxis()->SetTitle(xLabel);
  s2->GetYaxis()->SetTitle(yLabel);
  s2->GetXaxis()->SetTitle(xLabel);
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
  if(s2 != s){
    //s2->SetMarkerStyle(1);
    //s2->SetMarkerSize(0);
    //s->SetFillStyle(0);
    //s->SetFillColor(kB);
    s2->SetLineWidth(3);
    s2->SetLineColor(kBlack);
  }


  TH1F* s_norm = (TH1F *) (s->DrawNormalized("hist", d->Integral()));
  TH1F* s2_norm = (TH1F *) (s2->DrawNormalized("hist same", d->Integral()));
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
  std::cout << "Variable  & Data & Simulation & Simulation2 \\" << std::endl;
  std::cout << "Mean      & " << d->GetMean() << " " << d->GetMeanError() 
	    << " & " << s_norm->GetMean() <<  " " << s_norm->GetMeanError() 
	    << " & " << s2_norm->GetMean() <<  " " << s2_norm->GetMeanError() 
	    << " \\" << std::endl;
  std::cout << "Std. dev. & " << d->GetRMS() << " " << d->GetRMSError() 
	    << " & " << s_norm->GetRMS() << " " << s_norm->GetRMSError() 
	    << " & " << s2_norm->GetRMS() << " " << s2_norm->GetRMSError() 
	    << " \\" << std::endl;
  std::cout << "\\hline" << std::endl;
  std::cout << "$\\Chi^2$ " <<  d->Chi2Test(s_norm, "UW CHI2/NDF NORM") << std::endl;
  

  TLegend *leg = new TLegend(0.6,0.8,1,1);
  if(dataLabel !="") leg->AddEntry(d,dataLabel,"p");
  if(mcLabel   !="") leg->AddEntry(s,mcLabel, "lf");
  if(mc2Label   !="") leg->AddEntry(s2,mc2Label, "l");
  leg->SetBorderSize(1);
  leg->SetFillColor(0);
  leg->SetTextSize(0.04);
  if(dataLabel !="" && mcLabel !="") leg->Draw();
  //c->GetListOfPrimitives()->Add(leg,"");

  TPaveText *pv = new TPaveText(0.23,0.95,0.6,1,"NDC");
  pv->AddText("CMS Preliminary 2012");
  pv->SetFillColor(0);
  pv->SetBorderSize(0);
  pv->Draw();


  return c;

}



TCanvas *PlotDataMCs(TChain *data, std::vector<TChain *> mc_vec, TString branchname, TString binning, 
		     TString category,  TString selection, 
		     TString dataLabel, std::vector<TString> mcLabel_vec, TString xLabel, TString yLabelUnit, 
		     bool logy=false, bool usePU=true, bool ratio=true,bool smear=false, bool scale=false, bool useR9Weight=false, TString pdfIndex=""){
  TStopwatch watch;
  watch.Start();
  //gStyle->SetOptStat(11);//Giuseppe

  int nHist= mc_vec.size();
  int colors[4]={kRed,kGreen,kBlue,kCyan};
  int fillstyle[4]={0,0,0,0}; //3003,3004,3005,3006};
  if(nHist>4) return NULL;
  TString yLabel; 
  
  TCanvas *c = new TCanvas("c","");
  TPad * pad1 = new TPad("pad1", "pad1",0.01,0.13,0.75,1.);  
  TPad * pad2 = new TPad("pad2", "pad2",0.01,0.001,0.75,0.2);  
  TPad * pad3 = new TPad("pad3", "pad3",0.68,0.001,1.,0.2);

  pad1->SetRightMargin(0.1);
  pad1->SetLogy();
  pad1->Draw();
  pad1->cd();
  
  pad2->SetGrid();
  pad2->SetBottomMargin(0.4);
  pad2->SetRightMargin(0.1);
  pad2->Draw();
  pad2->cd();
  
  pad3->SetGrid();    
  //pad2->SetTopMargin(0.01);
  pad3->SetBottomMargin(0.4);
  pad3->SetRightMargin(0.1);
  pad3->Draw();
  pad3->cd();
    
  pad1->cd();

  TString branchNameData=branchname;
  TString branchNameMC=branchname;

  ElectronCategory_class cutter;
  // data->SetBranchStatus("*",0);
//   std::set<TString> branchList = cutter.GetBranchNameNtuple(category);
//   for(std::set<TString>::const_iterator itr = branchList.begin();
//       itr != branchList.end();
//       itr++){
//     std::cout << "[STATUS] Enabling branch: " << *itr << std::endl;
//     data->SetBranchStatus(*itr, 1);
//   }
//   data->SetBranchStatus("invMass_SC_regrCorrSemiParV4_ele", 1);
//   data->SetBranchStatus("invMass_SC_regrCorrSemiParV5_ele", 1);
  if(branchNameData.Contains("energySCEle_regrCorrSemiParV5_pho")) cutter.energyBranchName="energySCEle_regrCorrSemiParV5_pho";
  else if(branchNameData.Contains("energySCEle_regrCorrSemiParV5_ele")) cutter.energyBranchName="energySCEle_regrCorrSemiParV5_ele";

  TCut selection_data="";
  if(category.Sizeof()>1) selection_data = cutter.GetCut(category, false,0,scale);
  selection_data+=selection;
  TCut selection_MC="";
  if(category.Sizeof()>1) selection_MC = cutter.GetCut(category, false,0);
  selection_MC+=selection;

  if(smear){
    branchNameMC.ReplaceAll("invMass_SC_regrCorr_pho ","(invMass_SC_regrCorr_pho*sqrt(smearEle[0]*smearEle[1]))");
    branchNameMC.ReplaceAll("invMass_SC_regrCorrSemiParV5_pho","(invMass_SC_regrCorrSemiParV5_pho*sqrt(smearEle[0]*smearEle[1]))");
    branchNameMC.ReplaceAll("energySCEle_regrCorr_pho ","(energySCEle_regrCorr_pho*smearEle) ");
    branchNameMC.ReplaceAll("energySCEle_regrCorr_pho[0]","(energySCEle_regrCorr_pho[0]*smearEle[0])");
    branchNameMC.ReplaceAll("energySCEle_regrCorr_pho[1]","(energySCEle_regrCorr_pho[1]*smearEle[1])");
    branchNameMC.ReplaceAll("energySCEle_regrCorrSemiParV5_ele[0]","(energySCEle_regrCorrSemiParV5_ele[0]*smearEle[0])");
    branchNameMC.ReplaceAll("energySCEle_regrCorrSemiParV5_ele[1]","(energySCEle_regrCorrSemiParV5_ele[1]*smearEle[1])");
    if(!branchNameMC.Contains("smear")) branchNameMC.ReplaceAll("energySCEle_regrCorrSemiParV5_ele","(energySCEle_regrCorrSemiParV5_ele*smearEle)");

  }
  if(scale){
    std::cout << "Apply scale" << std::endl;
    branchNameData.ReplaceAll("invMass_SC_regrCorr_pho ","(invMass_SC_regrCorr_pho*sqrt(scaleEle[0]*scaleEle[1]))");
    branchNameData.ReplaceAll("invMass_SC_regrCorrSemiParV5_pho","(invMass_SC_regrCorrSemiParV5_pho*sqrt(scaleEle[0]*scaleEle[1]))");
    branchNameData.ReplaceAll("energySCEle_regrCorrSemiParV5_pho ","(energySCEle_regrCorrSemiParV5_pho*scaleEle)");
    branchNameData.ReplaceAll("energySCEle_regrCorrSemiParV5_pho[0]","(energySCEle_regrCorrSemiParV5_pho[0]*scaleEle[0])");
    branchNameData.ReplaceAll("energySCEle_regrCorrSemiParV5_pho[1]","(energySCEle_regrCorrSemiParV5_pho[1]*scaleEle[1])");
    branchNameData.ReplaceAll("energySCEle_regrCorrSemiParV5_ele[0]","(energySCEle_regrCorrSemiParV5_ele[0]*scaleEle[0])");
    branchNameData.ReplaceAll("energySCEle_regrCorrSemiParV5_ele[1]","(energySCEle_regrCorrSemiParV5_ele[1]*scaleEle[1])");
    if(!branchNameData.Contains("scale"))    branchNameData.ReplaceAll("energySCEle_regrCorrSemiParV5_ele","(energySCEle_regrCorrSemiParV5_ele*scaleEle)");
  }    

    std::cout << branchNameData << "\t" << branchNameMC << std::endl;
  //  return NULL;

  
  // Draw histograms
    data->Draw(branchNameData+">>data_hist"+binning, selection_data);
    if(nHist > 0){
      for(std::vector<TChain *>::const_iterator mc_itr = mc_vec.begin();
	  mc_itr != mc_vec.end();
	  mc_itr++){
	TChain *mc = *mc_itr;
// 	mc->SetBranchStatus("*",0);
// 	for(std::set<TString>::const_iterator itr = branchList.begin();
// 	    itr != branchList.end();
// 	    itr++){
// 	  //std::cout << "[STATUS] Enabling branch: " << *itr << std::endl;
// 	  mc->SetBranchStatus(*itr, 1);
// 	}
// 	mc->SetBranchStatus("invMass_SC_regrCorrSemiParV4_ele", 1);
// 	mc->SetBranchStatus("invMass_SC_regrCorrSemiParV5_ele", 1);
// 	mc->SetBranchStatus("puWeight",1);

	TString mcHistName; mcHistName+=mc_itr-mc_vec.begin(); mcHistName+="_hist";
	TString weights="mcGenWeight";
	if(pdfIndex!="") weights+="*(pdfWeights_cteq66["+pdfIndex+"]/pdfWeights_cteq66[0])";
	if(usePU) weights+="*puWeight";
	if(useR9Weight) weights+="*r9Weight";
	mc->Draw(branchNameMC+">>"+mcHistName+binning, selection_MC *weights.Data());

      }
    }


  c->Clear();
  TLegend *leg = new TLegend(0.65,0.8,1,1);
  leg->SetBorderSize(1);
  leg->SetFillColor(0);
  leg->SetTextSize(0.04);
//   if(dataLabel !="" && mcLabel !="") leg->Draw();
//   //c->GetListOfPrimitives()->Add(leg,"");


  TH1F *d = (TH1F *) gROOT->FindObject("data_hist");
  if(dataLabel !="") leg->AddEntry(d,dataLabel,"p");

  d->SetMarkerStyle(20);
  d->SetMarkerSize(1);

  if(d->GetEntries()==0 || d->Integral()==0){
    d=(TH1F *) gROOT->FindObject("0_hist");
    d->SetMarkerSize(0);
  }
  //d->SaveAs("tmp/d_hist.root");
  //s->SaveAs("tmp/s_hist.root");

  yLabel.Form("Events /(%.2f %s)", d->GetBinWidth(2), yLabelUnit.Data());
  
  float max = 0; //1.1 * std::max(
  max=1.2*d->GetMaximum();
  std::cout << "max = " << max << std::endl;
  std::cout << "nEvents data: " << d->Integral() << "\t" << d->GetEntries() << std::endl;
    

  d->GetYaxis()->SetTitle(yLabel);
  d->GetXaxis()->SetTitle(xLabel);
  if(logy){
    max*=10;
    d->GetYaxis()->SetRangeUser(0.1,max);
    c->SetLogy();
  } else {
      d->GetYaxis()->SetRangeUser(0,max);
  }

  for(int i=0; i < nHist; i++){
    TString mcHistName; mcHistName+=i; mcHistName+="_hist";
    TH1F *s = (TH1F *) gROOT->FindObject(mcHistName);
    if(s==NULL) continue;
    std::cout << "nEvents signal: " << s->Integral() << "\t" << s->GetEntries() << std::endl;
    if(d->Integral()==0 && s->Integral()==0){
      delete c;
      return NULL;
    }
    if(logy){
      s->GetYaxis()->SetRangeUser(0.1,max);
    } else {
      s->GetYaxis()->SetRangeUser(0,max);
    }
    s->GetYaxis()->SetTitle(yLabel);
    s->GetXaxis()->SetTitle(xLabel);

    s->SetMarkerStyle(20);
    s->SetMarkerSize(1);
    s->SetMarkerColor(colors[i]);
    s->SetFillStyle(fillstyle[i]);
    s->SetFillColor(colors[i]);
    s->SetLineColor(colors[i]);
    s->SetLineWidth(2);

    TH1F* s_norm = NULL;
    if(i==0) s_norm = (TH1F *) (s->DrawNormalized("hist", d->Integral()));
    else s_norm = (TH1F *) (s->DrawNormalized("hist same", d->Integral()));
    if(logy){
      //d_norm->GetYaxis()->SetRangeUser(0.1,max);
      s_norm->GetYaxis()->SetRangeUser(0.1,max);
    } else {
      //d_norm->GetYaxis()->SetRangeUser(0,max);  
      s_norm->GetYaxis()->SetRangeUser(0,max);  
    }

    if(mcLabel_vec[i] !="") leg->AddEntry(s,mcLabel_vec[i], "lf");

//     TH1F *sRatio = (TH1F *) s->Clone(mcHistName+"_ratio");
//     sRatio->Divide(d);
//     if(ratio){
//       pad2->cd();
//       if(i==0) sRatio->Draw();
//       else sRatio->Draw("same");
//     }
//     pad1->cd();
  }

  //TH1F* d_norm = s_norm;
  //if(d!=s) d_norm = (TH1F *) (d->DrawNormalized("p same", d->Integral()));
  if(nHist>0) d->Draw("p same");
  else d->Draw("p");

//   std::cout << "Variable  & Data & Simulation \\" << std::endl;
//   std::cout << "Mean      & " << d->GetMean() << " " << d->GetMeanError() 
// 	    << " & " << s_norm->GetMean() <<  " " << s_norm->GetMeanError() << " \\" << std::endl;
//   std::cout << "Std. dev. & " << d->GetRMS() << " " << d->GetRMSError() 
// 	    << " & " << s_norm->GetRMS() << " " << s_norm->GetRMSError() << " \\" << std::endl;
//   std::cout << "\\hline" << std::endl;
//   std::cout << "$\\Chi^2$ " <<  d->Chi2Test(s_norm, "UW CHI2/NDF NORM") << std::endl;
  
  if(mcLabel_vec.size()!=0) leg->Draw();

  TPaveText *pv = new TPaveText(0.25,0.95,0.65,1,"NDC");
  pv->AddText("CMS Preliminary 2012");
  pv->SetFillColor(0);
  pv->SetBorderSize(0);
  pv->Draw();

  watch.Stop();
  watch.Print();
  return c;

}

std::vector<TChain *> MakeChainVector(TChain *v1, TChain *v2, TChain *v3, TChain *v4){
  std::vector<TChain *> vec;
  vec.push_back(v1);
  vec.push_back(v2);
  vec.push_back(v3);
  vec.push_back(v4);
  return vec;
}

std::vector<TChain *> MakeChainVector(TChain *v1, TChain *v2, TChain *v3){
  std::vector<TChain *> vec;
  vec.push_back(v1);
  vec.push_back(v2);
  vec.push_back(v3);
  return vec;
}

std::vector<TChain *> MakeChainVector(TChain *v1, TChain *v2){
  std::vector<TChain *> vec;
  vec.push_back(v1);
  vec.push_back(v2);
  return vec;
}

std::vector<TChain *> MakeChainVector(TChain *v1){
  std::vector<TChain *> vec;
  vec.push_back(v1);
  return vec;
}


// TH2F *PlotDataMCs(TChain *data, TString branchname, 
// 		  TString category, TString selection, 
// 		  bool usePU=true, bool smear=false, bool scale=false){

//     ElectronCategory_class cutter;
//     TCut selection_data="";
//     if(category.Sizeof()>1) selection_data = cutter.GetCut(category, false,0);
//     selection_data+=selection;
    
//   if(smear){
//     branchNameMC.ReplaceAll("invMass_SC_regrCorr_pho ","(invMass_SC_regrCorr_pho*sqrt(smearEle[0]*smearEle[1]))");
//     branchNameMC.ReplaceAll("energySCEle_regrCorr_pho ","(energySCEle_regrCorr_pho*smearEle) ");
//     branchNameMC.ReplaceAll("energySCEle_regrCorr_pho[0]","(energySCEle_regrCorr_pho[0]*smearEle[0])");
//     branchNameMC.ReplaceAll("energySCEle_regrCorr_pho[1]","(energySCEle_regrCorr_pho[1]*smearEle[1])");

//   }
//   if(scale){
//     branchNameData.ReplaceAll("invMass_SC_regrCorr_pho ","(invMass_SC_regrCorr_pho*sqrt(corrEle[0]*corrEle[1]))");
//     branchNameData.ReplaceAll("energySCEle_regrCorr_pho ","(energySCEle_regrCorr_pho*corrEle)");
//     branchNameData.ReplaceAll("energySCEle_regrCorr_pho[0]","(energySCEle_regrCorr_pho[0]*corrEle[0])");
//     branchNameData.ReplaceAll("energySCEle_regrCorr_pho[1]","(energySCEle_regrCorr_pho[1]*corrEle[1])");
//   }
  
//   // Draw histograms
//   TString branchNameData=branchname;
//   data->Draw(branchNameData+">>eventlist", selection_data,"eventlist");
//   TEventList *evlist = (TEventList*) gROOT->FindObject("eventlist");
//   data->SetEventList(evlist);

//   TTreeFormula *selector_ele1 = new TTreeFormula("selector", branchNameData, data);
//   TTreeFormula *selector_ele2 = new TTreeFormula("selector", branchNameData.ReplaceAll("[0]","[1]"), data);

//   Long64_t entries = entryList->GetN();
//   std::cout << "___ ENTRIES: " << entries << std::endl; 
//   data->LoadTree(chain->GetEntryNumber(0));
//   Long64_t treenumber=-1;

//   branchname="seedXSCEle:seedYSCEle";
//   binning="(360,1,361,171,-85,86)";
// //   yLabel="iEta";
// //   xLabel="iPhi";
// //   c->SetGridx();
  
//   Float_t iPhi[2], iEta[2];
//   data->SetBranchAddress("seedYSCEle", iPhi);
//   data->SetBranchAddress("seedXSCEle", iEta);
//   double sum[360][171]=0, sum2[360][171]=0;
//   for(Long64_t jentry=0; jentry < entries; jentry++){
//     Long64_t entryNumber= data->GetEntryNumber(jentry);
//     data->GetEntry(entryNumber);
//     if (data->GetTreeNumber() != treenumber) {
//       treenumber = data->GetTreeNumber();
//       selector_ele1->UpdateFormulaLeaves();
//       selector_ele2->UpdateFormulaLeaves();
//     }
//     double value=selector_ele1->EvalInstance();
    
//     sum+=value;
//     sum2+=value*value;
//   }
  
// }
