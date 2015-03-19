#include <iostream>
#include <fstream>
#include <TString.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TMath.h>
#include <TH1F.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TH1.h>
#include <TList.h>
#include <THashList.h>
#include <TStyle.h>
#include <TLine.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
//#define DEBUG
#define lightLabels
enum plotType { onlyData=0, onlyDataBis, dataMC, dataMCs, dataOverMC, dataOverMCfit, onlyMCs};

TH1F *stabilityHist(TGraphErrors *g_data, double& y_err_mean){
  //------------------------------
  Double_t *Y=g_data->GetY();
  Double_t *EY = g_data->GetEY();

  TH1 *obj = (TH1*) g_data->GetYaxis()->GetParent();
  TH1F *hist_corr = new TH1F("hist_"+TString(g_data->GetName()),"",
			     g_data->GetN(), obj->GetMinimum(), obj->GetMaximum());
  hist_corr->GetXaxis()->SetLabelSize(0);
  hist_corr->GetYaxis()->SetLabelSize(0);
  for(int i =0; i < g_data->GetN(); i++){
    hist_corr -> Fill(Y[i]);
    y_err_mean+=EY[i];
  }
  y_err_mean/=g_data->GetN();
  return hist_corr;
}

TGraphErrors *columns_vs_var(TString filename, TString region_, int column, double& rMin, double& rMax, bool updateRange=false){

  TString region;
  TString xVar, rangeMin, rangeMax,numEvents;
  double deltaM_data, err_deltaM_data;
  double deltaM_MC, err_deltaM_MC;
  double deltaG, err_deltaG;
  bool isDeltaG=false;
  std::vector<TString> xLabels;
  double rMin_=10, rMax_=-10;
  TGraphErrors deltaM_data_graph(100), deltaM_MC_graph(100), deltaG_graph(100);
  TH1F deltaM_data_hist("deltaM_data_hist", "#Delta m [GeV/c^{2}]",
			1000, -5, 5);

  int i_point=0;


  //  std::cout << "------------------------------" << std::endl;
  //  std::cout << "[STATUS] Starting with deltaM stability for region: " << region_ << "\t" << "column=" << column <<std::endl;
  std::ifstream f_in(filename);
  if(f_in.bad()){
    std::cerr << "[ERROR] File " << filename << " not found or not readable" << std::endl;
    return NULL;
  }
  

  //  while(f_in.peek()!=EOF && f_in.good()){
  while(f_in.peek()!=EOF && f_in.good()){
    if(f_in.peek()==10){ // 10 = \n
      f_in.get();
      continue;
    }

    if(f_in.peek() == 35){ // 35 = #
      std::cout << "[DEBUG] Ignore line" << std::endl;
      f_in.ignore(1000,10); // ignore the rest of the line until \n
      continue;
    }
    
    f_in >> region >> xVar >> rangeMin >> rangeMax >> numEvents 
	 >> deltaM_data >> err_deltaM_data
	 >> deltaM_MC >> err_deltaM_MC;

    //std::cout << region << "\t" << xVar << "\t" << rangeMin << "\t" << rangeMax << "\t" << deltaM_data << "\t" << deltaM_MC << "\t" << err_deltaM_MC << "\t" << f_in.peek() << std::endl;

    if(f_in.peek()!=10){ // 10 = \n
      isDeltaG=true;
      f_in >> deltaG >> err_deltaG;
      //std::cout << region << "\t" << xVar << "\t" << rangeMin << "\t" << rangeMax << "\t" << deltaM_data << "\t" << deltaM_MC << "\t" << err_deltaM_MC << "\t" << deltaG << "\t" << err_deltaG << "\t" << f_in.peek() << std::endl;

    }
    if(column!=2){
      if(deltaM_data< rMin_) rMin_=deltaM_data;
      if(deltaM_MC< rMin_) rMin_=deltaM_MC;
      if(deltaM_data >  rMax_) rMax_=deltaM_data;
      if(deltaM_MC > rMax_) rMax_=deltaM_MC;
    }
    if(region_=="" || region.CompareTo(region_)==0){
      deltaM_data_hist.Fill(deltaM_data);
      if(xVar.CompareTo("runNumber")==0){
	deltaM_data_graph.SetPoint(i_point, i_point, deltaM_data);
	deltaM_data_graph.SetPointError(i_point, 0, err_deltaM_data);
	
	deltaM_MC_graph.SetPoint(i_point, i_point, deltaM_MC);
	deltaM_MC_graph.SetPointError(i_point, 0, err_deltaM_MC);
	
	deltaG_graph.SetPoint(i_point, i_point, deltaG);
	deltaG_graph.SetPointError(i_point, 0, err_deltaG);

	
      } else {       // because it's not a stability:
	float i_point_x = (rangeMax.Atof() + rangeMin.Atof())/2.;
	float i_point_ex = (rangeMax.Atof() - rangeMin.Atof())/2.;
	deltaM_data_graph.SetPoint(i_point, i_point_x, deltaM_data);
	deltaM_data_graph.SetPointError(i_point, i_point_ex, err_deltaM_data);
	
	//      deltaM_data_hist.Fill(deltaM_MC);
	deltaM_MC_graph.SetPoint(i_point, i_point_x, deltaM_MC);
	deltaM_MC_graph.SetPointError(i_point, i_point_ex, err_deltaM_MC);
	
	deltaG_graph.SetPoint(i_point, i_point_x, deltaG);
	deltaG_graph.SetPointError(i_point, i_point_ex, err_deltaG);
	
      }
#ifndef lightLabels
      xLabels.push_back(rangeMin+"-"+rangeMax);
#else
      if(i_point%3==0) xLabels.push_back(rangeMin);
      else xLabels.push_back("");
#endif
      //      deltaM_data_graph.GetXaxis()->SetBinLabel(i_point, rangeMin+"-"+rangeMax);
      i_point++;
      if(i_point>99){
	std::cerr << "[ERROR] maximum number of points reached" << std::endl;
	return NULL;
      }
      
    } //else std::cout << region << std::endl;
    
    
  }
  f_in.close();


  if(i_point==0){
    std::cerr << "[WARNING] No data found for region " << region << " in file " << filename << std::endl;
    return NULL;
  } else std::cout << i_point << std::endl;


  deltaM_data_graph.Set(i_point);
  deltaM_MC_graph.Set(i_point);
  deltaG_graph.Set(i_point);

  deltaM_data_graph.GetYaxis()->SetTitle(deltaM_data_hist.GetTitle());
  
  TGraphErrors *g_out;
  if (column==1) g_out = new TGraphErrors(deltaM_MC_graph);
  else if(column==0) g_out = new TGraphErrors(deltaM_data_graph);
  else g_out = new TGraphErrors(deltaG_graph);
  
  TH1F *hist_g=  g_out->GetHistogram();
  hist_g->Draw();
  TAxis *axis = hist_g->GetXaxis();
  if(column<2){
  for(int i =0; i < (int)xLabels.size() ; i++){
    int i_bin= axis->FindBin(deltaM_data_graph.GetX()[i]);
    axis->SetBinLabel(i_bin,xLabels[i]); //
  }
  }
  g_out->SetHistogram(hist_g);
  if(updateRange){
    //    std:: cout << rMin_ << "\t" << rMax_ << std::endl;
    if(rMin_<rMin) rMin=rMin_-(rMax_-rMin_)*0.05;
    if(rMax_>rMax) rMax=rMax_+0.1;
  }
  g_out->Draw("AP");
  return g_out;


}



void SetLegendStyle(TLegend *legend){
  //legend->SetTextFont(22); // 132
  legend->SetTextSize(0.12); // l'ho preso mettendo i punti con l'editor e poi ho ricavato il valore con il metodo GetTextSize
  //legend->SetTextSize(0.06); // l'ho preso mettendo i punti con l'editor e poi ho ricavato il valore con il metodo GetTextSize
  legend->SetBorderSize(0);
  legend->SetFillColor(0); // colore di riempimento bianco
  legend->SetMargin(0.2);  // percentuale della larghezza del simbolo
}

TCanvas *var_Stability(TString filename_corr, TString region, double rMin=3, double rMax=2, bool color=false, int column=1, TString xTitle="", TString yTitle="", TString filename_unCorr="", TString f_corr_legend="Data", TString f_uncorr_legend="no LC Data"){
  
  // column<0: same as >0 but without numbers
  // column=0: only data
  // column=1: data and MC
  // column=2: data/MC
  // column=3: data, data_uncorr, data/data_uncorr     -> to be fixed
  // column=4: data, MC, data_uncorr, data/data_uncorr -> to be fixed
  // column=5: data, MC and data/MC (corr & uncorr)
  // column=51: data, MC and data/MC (corr & uncorr) -> with data/MC linear fit
  // column=6: data/MC (corr & uncorr)
  // column=61: data/MC (corr & uncorr) -> with linear fit
  // column=-10: ECAL paper
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  if(column==-10){
    f_corr_legend="with laser monitoring corrections";
    f_uncorr_legend="without laser monitoring corrections";
  }
  float labelScale=1;
  if(column>3) labelScale=2;

  if(!yTitle.Sizeof()>1){
    if(filename_corr.Contains("bias")) yTitle="#Delta m [GeV/c^{2}]";
    else if(filename_corr.Contains("sigmaCBover")) yTitle="#sigma_{CB}/peak_{CB} [%]";
    else if(filename_corr.Contains("sigma")) yTitle="#sigma_{CB} [GeV/c^{2}]";
  }

  if(!xTitle.Sizeof()>1){    
    if(filename_corr.Contains("nPV"))     xTitle = "Number of reconstructed vertexes";
    if(filename_corr.Contains("runNumber")) xTitle="runNumber";
    if(filename_corr.Contains("unixtime")) xTitle="Time";
    if(filename_corr.Contains("abs_eta")) xTitle="|#eta|";
  }
  TString yTitle2;
  if (yTitle.Contains("#Delta")){
    yTitle2 = "#Delta P [%]";
  }
  else if (yTitle.Contains("#sigma")&&yTitle.Contains("peak")){
    yTitle2 = "#Add.Smearing [%]";
  }


  TCanvas *c;
  if(column<0)  c = new TCanvas("c", region);
  else  c = new TCanvas("c", region, 900,500);
  c->cd();

  TGraphErrors *g_data = columns_vs_var(filename_corr, region, 0,rMin, rMax);
  TGraphErrors *g_MC = columns_vs_var(filename_corr, region, 1,rMin, rMax);
  TGraphErrors *g_dataMC=NULL, *g_data_unCorr=NULL, *g_dataMC_unCorr=NULL;

  if(filename_unCorr.Sizeof()>1) g_data_unCorr = columns_vs_var(filename_unCorr, region, 0, rMin, rMax);
  if(column ==2 || column==5 || column==3 || abs(column)==6 || column==-5 || column==-3 
     || fabs(column/10) == 5 ||  fabs(column/10) == 6){
    g_dataMC = columns_vs_var(filename_corr, region, 2,rMin, rMax);
    if(filename_unCorr.Sizeof()>1) g_dataMC_unCorr = columns_vs_var(filename_unCorr, region, 2, rMin, rMax);
  }
  
  //------------------------------ style
  g_data -> SetMarkerStyle(20);
  g_data->SetMarkerSize(1.2);
  //g_data->SetMarkerSize(3);
  g_data -> SetMarkerColor(kBlack);
  
  if(color && column ==-10) g_data -> SetMarkerColor(kGreen+2);
  g_data -> SetFillColor(0);
  g_data -> SetDrawOption("P");

  if(g_data_unCorr!=NULL){
    g_data_unCorr->SetMarkerStyle(22);
    g_data_unCorr->SetMarkerSize(1.2);
    g_data_unCorr -> SetMarkerColor(kGray+2);
    if(color) g_data_unCorr->SetMarkerColor(kBlue);
    if(color && column==-10) g_data_unCorr->SetMarkerColor(kRed+2);
    g_data_unCorr -> SetFillColor(0);
  }

  g_MC -> SetMarkerStyle(0);
  //g_MC -> SetMarkerSize(0);
  g_MC -> SetFillColor(kGray);
  if(color){
    g_MC -> SetMarkerColor(kRed+2);
    g_MC -> SetFillColor(kRed+2);
  }
  g_MC -> SetLineWidth(0);
  g_MC -> SetLineColor(1);
  //g_MC -> SetDrawOption("");
  
  TMultiGraph *g_multi = new TMultiGraph();
  if(column>0 )   g_multi->Add(g_MC,"L3");
  g_multi->Add(g_data,"P");

  if(g_data_unCorr!=NULL) g_multi->Add(g_data_unCorr);

  c->Clear();
  g_multi->Draw("A");

  g_multi->GetYaxis()->SetTitle(yTitle);
  g_multi->GetXaxis()->SetTitle(xTitle);
  g_multi->GetXaxis()->SetLabelSize(0.05 * labelScale);
#ifdef lightLabels
  g_multi->GetXaxis()->SetLabelSize(g_multi->GetXaxis()->GetLabelSize()*1.5);
#endif
  g_multi->GetXaxis()->SetNdivisions(5);

  if(xTitle.Contains("Time") || xTitle.Contains("date")){
    g_multi->GetXaxis()->SetTimeDisplay(true);
    g_multi->GetXaxis()->SetTimeOffset(0);
    g_multi->GetXaxis()->SetLabelSize(0.05 * labelScale);
    g_multi->GetXaxis()->SetTimeFormat("%d/%m");
  }

  //  g_multi->GetXaxis()->LabelsOption("v");

  g_multi->GetXaxis()->SetTitleSize(g_multi->GetXaxis()->GetTitleSize()*labelScale);
  g_multi->GetXaxis()->SetTitleOffset(g_multi->GetXaxis()->GetTitleOffset()/labelScale);

  g_multi->GetYaxis()->SetTitleSize(g_multi->GetYaxis()->GetTitleSize()*labelScale);
  g_multi->GetYaxis()->SetTitleOffset(g_multi->GetYaxis()->GetTitleOffset()/labelScale);
  g_multi->GetYaxis()->SetLabelSize(g_multi->GetYaxis()->GetLabelSize()*labelScale);

  // automatic setting of the range
  if( rMax == rMin || rMax < rMin){
    g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin(), g_multi->GetYaxis()->GetXmax() *1.1);
  } else {
    if(rMin > g_multi->GetYaxis()->GetXmin()) rMin=g_multi->GetYaxis()->GetXmin();
    if(rMax < g_multi->GetYaxis()->GetXmax()) rMax=g_multi->GetYaxis()->GetXmax();
    g_multi->GetYaxis()->SetRangeUser(rMin,rMax);
  }
  TPad *pad1=NULL; 
  TPad *pad2=NULL; 
  TPad *pad3=NULL;
  TPad *pad4=NULL;

  if(column>2){
    pad1= new TPad("pad1","", 0,0.4,0.75,1);
    pad2= new TPad("pad2","", 0.75,0.4,1,1);
    pad3= new TPad("pad3","", 0,0.,0.75,0.4);
    pad4= new TPad("pad4","", 0.75,0.,1,0.4);
  } else if(column<0)    pad1= new TPad("pad1","", 0,0.,0.95,0.95);
  else{
    pad1= new TPad("pad1","", 0,0.,0.75,1);
    pad2= new TPad("pad2","", 0.75,0.,1,1);
  }
  c->Clear();
  if(column>=0) pad2->Draw();
  pad1->Draw();
  if(column>2){
    pad3->Draw();
    pad4->Draw();
    pad3->SetRightMargin(0.01);
    pad3->SetGridy();
  }
  pad1->cd();

  if(xTitle.Contains("runNumber")){
    TAxis *axis = g_multi->GetXaxis();
    TAxis *axis_data = g_data->GetXaxis();
    for(int i_bin =0; i_bin < (int) axis_data->GetNbins()-4; i_bin++){
      //    int i_bin= axis->FindBin(i);
      axis->SetBinLabel(i_bin+5, axis_data->GetBinLabel(i_bin)); //
    }
    pad1->SetBottomMargin(0.28);
    if(column>=0)pad2->SetBottomMargin(0.28);
    axis->SetTitleOffset(-0.3);
    if(rMin >= rMax){
      if(g_multi->GetYaxis()->GetXmin() > 0)
	g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin()*0.9,g_multi->GetYaxis()->GetXmax());
      else
	g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin()*1.2,g_multi->GetYaxis()->GetXmax());
    }
  } else {
    pad1->SetGridx();
    //    if(pad3!=NULL) pad3->SetGridx();
  }

  pad1->SetRightMargin(0.01);
  pad1->SetGridy();
  
  if(column!=2){
    g_multi->Draw("A");
    g_multi->Draw("Psame");
    g_data->GetXaxis()->Draw();
  }

  //------------------------------
  Double_t *Y=NULL, *Y_uncorr=NULL, *EY = NULL, *EY_uncorr = NULL;
  double y_err_mean=0,y_err_uncorr_mean=0;

  Y = g_data->GetY();
  EY = g_data->GetEY();
  if( g_data_unCorr!=NULL){ 
    Y_uncorr = g_data_unCorr->GetY();
    EY_uncorr = g_data_unCorr->GetEY();
  }

  TH1 *obj = (TH1*) g_multi->GetYaxis()->GetParent();
  TH1F *hist_corr = new TH1F("hist_corr","hist_corr",   
			     g_data->GetN(), obj->GetMinimum(), obj->GetMaximum());
  TH1F *hist_uncorr = new TH1F("hist_uncorr","hist_uncorr",   
			       g_data->GetN(), obj->GetMinimum(), obj->GetMaximum());
  hist_corr->GetXaxis()->SetLabelSize(0);
  hist_corr->GetYaxis()->SetLabelSize(0);
  for(int i =0; i < g_data->GetN(); i++){
    hist_corr -> Fill(Y[i]);
    y_err_mean+=EY[i];

    if(Y_uncorr!=NULL){
      hist_uncorr -> Fill(Y_uncorr[i]);
      y_err_uncorr_mean+=EY_uncorr[i];
    }
  }
  y_err_mean/=g_data->GetN();
  y_err_uncorr_mean/=g_data->GetN();

  if(column>=0) pad2->cd();
  hist_corr->SetFillColor(g_data->GetMarkerColor());
  //  hist_uncorr->SetFillColor(g_data_unCorr->GetMarkerColor());

  //  hist_corr->SetFillStyle(3001);
  hist_corr->SetFillStyle(1001);
  hist_uncorr->SetFillStyle(3002);

  if(column >=0 && column<2 && g_data_unCorr==NULL){
    hist_corr->Draw("hbar");
    hist_uncorr->Draw("hbar same");
  }


  TLegend *legend;
  TPaveText *pt;
  if(column == -2){
    if(region.CompareTo("EB")==0)
      legend = new TLegend(0.35,0.2,0.7,0.4);
    else
      legend = new TLegend(0.35,0.15,0.9,0.4);
    pt = new TPaveText(0.15,0.73,0.6,0.94,"NDC");
    pt -> SetFillColor(0);
    pt -> SetFillStyle(0);
  } else 
  if(column == -3){
    legend = new TLegend(0.35,0.35,0.95,0.55);
    pt = new TPaveText(0.15,0.73,0.6,0.94,"NDC");
    pt -> SetFillColor(0);
    pt -> SetFillStyle(0);
  }else    if(column <0){
    if(region.CompareTo("EB")==0)
      legend = new TLegend(0.35,0.35,0.9,0.6);
    else
      legend = new TLegend(0.35,0.15,0.9,0.4);
    pt = new TPaveText(0.15,0.73,0.6,0.94,"NDC");
    pt -> SetFillColor(0);
    pt -> SetFillStyle(0);
  }else        if(column >=0 && column<2 && g_data_unCorr==NULL){
    legend = new TLegend(0,0.,1,0.10);
    pt = new TPaveText(0,0.11,1,0.28,"NDC");
  } else {
    if(g_data_unCorr==NULL){
      legend = new TLegend(0,0.1,1,0.4);
      pt = new TPaveText(0,0.4,1,1,"NDC");
    } else {
      legend = new TLegend(0,0.,1,0.3);
      pt = new TPaveText(0,0.3,1,0.9,"NDC");
    }     
  }
    

  TGraphErrors *g_data_leg = (TGraphErrors *)g_data->Clone("leg_g_data");
  TGraphErrors *g_data_unCorr_leg =NULL;

  if(g_data_unCorr!=NULL){
    g_data_unCorr_leg = (TGraphErrors *)g_data_unCorr->Clone("leg_g_data_unCorr");
    g_data_unCorr_leg->SetMarkerSize(3);
    legend->AddEntry(g_data_unCorr_leg, f_uncorr_legend,"p");
  }
  g_data_leg->SetMarkerSize(3);
  legend->AddEntry(g_data_leg, f_corr_legend,"p");


  SetLegendStyle(legend);
  if(column==-10) legend->SetTextSize(0.04);
  if(column<0) legend->SetTextSize(0.05);

  legend->SetEntrySeparation(0.01);


  //  pt->SetLabel("Data");
  pt->SetTextSize(0.1);
  pt->SetFillColor(kWhite);
  pt->SetFillStyle(0);
  pt->SetBorderSize(0);
  char out_char[300];
  //if(column!=1) pt->AddLine();
  pt->AddText(f_corr_legend);
  sprintf(out_char,"Mean = %.2f", hist_corr->GetMean());
  pt->AddText(out_char);

  sprintf(out_char,"Std. dev. = %.2f", hist_corr->GetRMS());
  pt->AddText(out_char);

  sprintf(out_char,"Mean Error = %.2f", y_err_mean);
  pt->AddText(out_char);

  if(hist_uncorr->GetEntries()>0){
    pt->AddText("");
    pt->AddLine();
    pt->AddText(f_uncorr_legend);
    sprintf(out_char,"Mean = %.2f", hist_uncorr->GetMean());
    pt->AddText(out_char);
    
    sprintf(out_char,"Std. dev. = %.2f", hist_uncorr->GetRMS());
    pt->AddText(out_char);
    
    sprintf(out_char,"Mean Error = %.2f", y_err_uncorr_mean);
    pt->AddText(out_char);
  }

  if(xTitle.Contains("runNumber")){
    pt->SetY1NDC(0.11);
    pt->SetY2NDC(0.28);
  }

  if(column!=2){
    if(column<0 ){
      pt->Clear();
      pt->AddText("CMS Preliminary 2012");
      pt->AddText("#sqrt{s} = 8TeV, L = 19.6 fb^{-1}");
      if(region.Contains("EB")) pt->AddText("ECAL Barrel");
      if(region.Contains("EE")) pt->AddText("ECAL Endcap");
      pt->Draw("NDC");
      pt->SetTextSize(0.045);
      pt->SetTextAlign(11);
      pt->SetFillStyle(1001);
    } else pt->Draw("NDC");
  }


  if(column>0)  legend->AddEntry(g_MC, "Simulation", "lf");
  if(column!=-1 && column!=2) legend->Draw("NDC");


  TLine *line = new TLine(0,0,1,0);
  line->SetLineWidth(2);
  //line->Draw();

  if(abs(column)>=2){
    if(column>2) pad3->cd();
    else pad1->cd();
    
    g_dataMC->SetTitle(0);     
    if(column!=2){
      g_dataMC->GetXaxis()->SetLabelSize(0);
    } else {
      pad1->SetGridy();
      g_dataMC->GetXaxis()->SetTitle(xTitle);
      g_dataMC->GetXaxis()->SetTitleSize(g_multi->GetXaxis()->GetTitleSize()*labelScale);
      g_dataMC->GetXaxis()->SetLabelSize(g_multi->GetXaxis()->GetLabelSize()*labelScale);
    }     
    g_dataMC->GetYaxis()->SetTitleSize(g_dataMC->GetYaxis()->GetTitleSize()*labelScale*1.3); 
    g_dataMC->GetYaxis()->SetTitleOffset(g_dataMC->GetYaxis()->GetTitleOffset()/labelScale/1.3); 
    g_dataMC->GetYaxis()->SetLabelSize(g_dataMC->GetYaxis()->GetLabelSize()*labelScale*1.3);
    //    g_dataMC->GetXaxis()->SetRangeUser(g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin(), g_multi->GetYaxis()->GetXmax() *1.1);

    g_dataMC->SetMarkerStyle(22);
    g_dataMC->SetMarkerSize(1.5);

    g_dataMC->GetYaxis()->SetTitle(yTitle2);
    g_dataMC->Draw("AP");
    if(g_dataMC_unCorr!=NULL)     g_dataMC_unCorr->Draw("Psame");

    //------------------------------
    Double_t *Y1=NULL, *Y1_uncorr=NULL, *EY1 = NULL, *EY1_uncorr = NULL;
    double y1_err_mean=0,y1_err_uncorr_mean=0;

    Y1 = g_dataMC->GetY();
    EY1 = g_dataMC->GetEY();
    if( g_dataMC_unCorr!=NULL){ 
      Y1_uncorr = g_dataMC_unCorr->GetY();
      EY1_uncorr = g_dataMC_unCorr->GetEY();
    }

    TH1 *obj1 = (TH1*) g_dataMC->GetYaxis()->GetParent();
    TH1F *hist_corr1 = new TH1F("hist_corr1","hist_corr1",   
				g_dataMC->GetN(), obj1->GetMinimum(), obj1->GetMaximum());
    TH1F *hist_uncorr1 = new TH1F("hist_uncorr1","hist_uncorr1",   
				  g_dataMC->GetN(), obj1->GetMinimum(), obj1->GetMaximum());
    for(int i =0; i < g_dataMC->GetN(); i++){
      hist_corr1 -> Fill(Y1[i]);
      y1_err_mean+=EY1[i];

      if(Y1_uncorr!=NULL){
	hist_uncorr1 -> Fill(Y1_uncorr[i]);
	y1_err_uncorr_mean+=EY1_uncorr[i];
      }
    }
    y1_err_mean/=g_dataMC->GetN();
    y1_err_uncorr_mean/=g_dataMC->GetN();

    if(column>2) pad4->cd();
    else pad2->cd();

    TLegend *legend1;
    TPaveText *pt1;
    if(column<2 && g_dataMC_unCorr==NULL){
      legend1 = new TLegend(0,0.,1,0.10);
      pt1 = new TPaveText(0,0.11,1,0.28,"NDC");
    } else {
      if(g_dataMC_unCorr==NULL){
	legend1 = new TLegend(0,0.,1,0.4);
	pt1 = new TPaveText(0,0.4,1,1,"NDC");
      } else {
	legend1 = new TLegend(0,0.,1,0.3);
	pt1 = new TPaveText(0,0.3,1,0.9,"NDC");
      }     
    }    

    TGraphErrors *g_dataMC_leg = (TGraphErrors *)g_dataMC->Clone("leg_g_dataMC");
    TGraphErrors *g_dataMC_unCorr_leg =NULL;

    g_dataMC_leg->SetMarkerSize(3);


    if(g_dataMC_unCorr!=NULL){
      legend1->AddEntry(g_dataMC_leg, "#DeltaP "+f_corr_legend,"p");
      g_dataMC_unCorr_leg = (TGraphErrors *)g_dataMC_unCorr->Clone("leg_g_dataMC_unCorr");
      g_dataMC_unCorr_leg->SetMarkerSize(3);
      legend1->AddEntry(g_dataMC_unCorr_leg, "#DeltaP "+f_uncorr_legend,"p");
    } else     legend1->AddEntry(g_dataMC_leg, "#DeltaP ","p");

    SetLegendStyle(legend1);
    //  legend->SetTextSize(0.12);
    legend1->SetEntrySeparation(0.01);

    //  pt1->SetLabel("Data");
    pt1->SetTextSize(0.1);
    pt1->SetFillColor(kWhite);
    pt1->SetBorderSize(0);
    //    char out_char[300];
    //pt1->AddLine();
    pt1->AddText(f_corr_legend);
    sprintf(out_char,"Mean = %.2f", hist_corr1->GetMean());
    pt1->AddText(out_char);

    sprintf(out_char,"Std. dev. = %.2f", hist_corr1->GetRMS());
    pt1->AddText(out_char);

    sprintf(out_char,"Mean Error = %.2f", y1_err_mean);
    pt1->AddText(out_char);

    if(hist_uncorr1->GetEntries()>0){
      pt1->AddText("");
      pt1->AddLine();
      pt1->AddText(f_uncorr_legend);
      sprintf(out_char,"Mean = %.2f", hist_uncorr1->GetMean());
      pt1->AddText(out_char);
    
      sprintf(out_char,"Std. dev. = %.2f", hist_uncorr1->GetRMS());
      pt1->AddText(out_char);
    
      sprintf(out_char,"Mean Error = %.2f", y1_err_uncorr_mean);
      pt1->AddText(out_char);
    }

    if(fabs(column/10) >0 && abs(column)%10==1){
      TFitResultPtr ptr = g_dataMC->Fit("pol1","s");
      TFitResult *fitres  = ptr.Get();
      pt1->AddText("");
      pt1->AddLine();
      sprintf(out_char,"Chi2/ndf = %.2f", fitres->Chi2()/fitres->Ndf());
      pt1->AddText(out_char);
      sprintf(out_char,"slope = %.2f #pm %.2f", fitres->GetParams()[1], fitres->GetErrors()[1]);
      pt1->AddText(out_char);
      sprintf(out_char,"inter. = %.2f #pm %.2f", fitres->GetParams()[0], fitres->GetErrors()[0]);
      pt1->AddText(out_char);
    }
    
    if(xTitle.Contains("runNumber")){
      pt1->SetY1NDC(0.11);
      pt1->SetY2NDC(0.28);
    }

    pt1->Draw("NDC");
    legend1->Draw("NDC");
  }

  return c;

}


TCanvas *var_Stability(std::vector<TString> filenameList, std::vector<TString> legendList,TString region, double rMin=3, double rMax=2, bool color=false, int column=1, TString xTitle="", TString yTitle="" ){
  
  
  // column<0: same as >0 but without numbers
  // column=0: only data
  // column=1: only data
  // column=2: data and MC (only 1 MC)
  // column=3: data and MC (all MCs)
  // column=4: data/MC
  // column=41: data/MC with linear fit
  // column=51: data, MC and data/MC with data/MC linear fit
  // column=6: only MCs
  // column=61: 
  // column=-10: ECAL paper
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  std::cout << "REGION=" << region << std::endl;
  float labelScale=1;
  //  if(column>3) labelScale=2;

  TString yTitle2;
  if (yTitle.Contains("#Delta")){
    yTitle2 = "#Delta P [%]";
  }
  else if (yTitle.Contains("#sigma")&&yTitle.Contains("peak")){
    yTitle2 = "#Add.Smearing [%]";
  }

  TString lumi="";
  TCanvas *c;
  TLegend *legend=NULL;
  TPaveText *pt=NULL;
  TPaveText *pave = new TPaveText(0.182,0.96,0.48,0.99, "ndc");
  // 	pv->DeleteText();
  pave->SetFillColor(0);
  pave->SetTextAlign(12);
  pave->SetBorderSize(0);
  pave->SetTextSize(0.04);
  pave->AddText("CMS Preliminary   #sqrt{s}=8 TeV");
  //if(lumi.Sizeof()>1)  pave->AddText("#sqrt{s}="+energy+"   L="+lumi+" fb^{-1}");
  //else
  //pave->AddText("#sqrt{s}=8 TeV");

  if(column<0)  c = new TCanvas("c", region);
  else  c = new TCanvas("c", region, 1000,500);
  c->cd();

  bool doStatistics= (column>10 && abs(column)%10==1);
  bool doFit = (column > 10 && abs(column)%10==2);
  if(column>10){
    column-=(column%10);
    column/=10;
  }
  if(column<0)  legend = new TLegend(0.16,0.7,0.42,0.93);
  else if(column>=0){
    if(doStatistics){
      //legend = new TLegend(0,0.6,1,1);
      legend = new TLegend(0,0.6,1,1);
      pt = new TPaveText(0.,0.,1,0.6,"NDC");
      pt -> SetFillStyle(0);
      pt->SetTextSize(0.1);
      pt->SetFillColor(kWhite);
      pt->SetBorderSize(0);
    } else{
      //legend = new TLegend(0,0.5,1,1);
      legend = new TLegend(0,0.2,1,0.8);
      pt=NULL;
    }
  }
  
  SetLegendStyle(legend);
  if(column<0){
    legend->SetTextSize(0.04); 
  } else {
    //legend->SetTextSize(0.06); 
    legend->SetTextSize(0.07); 
  }
  TMultiGraph *g_multi = new TMultiGraph();
  bool updateRange=(rMin>=rMax);
  for(std::vector<TString>::const_iterator filename_itr= filenameList.begin();
      filename_itr != filenameList.end();
      filename_itr++){
    int index=filename_itr-filenameList.begin();
    TGraphErrors *g_data = columns_vs_var(*filename_itr, region, 0,rMin, rMax, updateRange&&abs(column)!=dataOverMC);
    TGraphErrors *g_MC   = columns_vs_var(*filename_itr, region, 1,rMin, rMax, updateRange&&abs(column)!=dataOverMC);
    TGraphErrors *g_dataMC=columns_vs_var(*filename_itr, region, 2,rMin, rMax, updateRange&&abs(column)==dataOverMC);
    g_dataMC->SaveAs("tmp/g_dataMC.root");

    if(updateRange){
      rMin-=(rMax-rMin)*0.1;
    }

    if(g_data==NULL || g_MC==NULL || g_dataMC==NULL) return c;

    g_data->SetName(region);
    //------------------------------ style
    g_data -> SetMarkerStyle(20+index);
    g_data->SetMarkerSize(1.5);
    //g_data->SetMarkerSize(3);
    g_data -> SetMarkerColor(kBlack+index);
    g_data -> SetFillColor(0);
    g_data -> SetDrawOption("P");

    g_dataMC -> SetMarkerStyle(20+index);
    g_dataMC->SetMarkerSize(1.2);
    //g_dataMC->SetMarkerSize(3);
    g_dataMC -> SetMarkerColor(kBlack+index);
    g_dataMC -> SetFillColor(0);
    g_dataMC -> SetDrawOption("P");

    g_MC -> SetMarkerStyle(0);
    //g_MC -> SetMarkerSize(0);
    g_MC -> SetFillColor(kGray);
    g_MC -> SetMarkerColor(kRed+2+index);
    g_MC -> SetFillColor(kRed+2+index);
    if(abs(column)==dataMCs){
      g_MC -> SetFillStyle(1001); //+index);
      g_MC -> SetFillColor(g_data->GetMarkerColor());
      g_MC -> SetMarkerColor(g_data->GetMarkerColor()); //kBlack+index);
      g_MC -> SetMarkerStyle(20+index);
    }else if(abs(column)==onlyMCs){
      g_MC -> SetFillStyle(0); //3003+index);
      g_MC -> SetFillColor(kBlack+index);
      g_MC -> SetMarkerColor(kBlack+index);
      g_MC -> SetMarkerStyle(20+index);
    }
    g_MC -> SetLineWidth(0);
    g_MC -> SetLineColor(1);

    if(doFit && abs(column)==onlyMCs){
      TFitResultPtr ptr = g_MC->Fit("pol1","s");
      TFitResult *fitres  = ptr.Get();
      char out_char[100];
//       pt1->AddText("");
//       pt1->AddLine();
      std::cout << "----" << std::endl;
      sprintf(out_char,"Chi2/ndf = %.2f", fitres->Chi2()/fitres->Ndf());
      std::cout << out_char << std::endl;
//       pt1->AddText(out_char);
      sprintf(out_char,"slope = %.2f #pm %.2f", fitres->GetParams()[1], fitres->GetErrors()[1]);
//       pt1->AddText(out_char);
      std::cout << out_char << std::endl;
      sprintf(out_char,"inter. = %.2f #pm %.2f", fitres->GetParams()[0], fitres->GetErrors()[0]);
//       pt1->AddText(out_char);
      std::cout << out_char << std::endl;
    }

    if(abs(column)!=onlyData && abs(column)!=dataOverMC && abs(column)!=onlyDataBis && abs(column)!=dataMC || (abs(column)==dataMC && index==0)){
    std::cout << doStatistics << "\t" << doFit << std::endl;


      TString plotOption="L3";
      if(abs(column)==onlyMCs) plotOption="P";
      g_multi->Add(g_MC,plotOption);
      if (abs(column)==dataMC) legend->AddEntry(g_MC,"Simulation", plotOption);
      else{
	if(legendList[index]!="") 	legend->AddEntry(g_MC,TString("MC: ")+legendList[index], plotOption+"f");
	else legend->AddEntry(g_MC,TString("Simulation"), plotOption+"f");
      }
    }
    if(abs(column)!=dataOverMC){
      if(pt!=NULL){
	double y_mean_error=0;
	TH1F *hist = (abs(column)!=onlyMCs) ? stabilityHist(g_data, y_mean_error) : stabilityHist(g_MC, y_mean_error);
	pt->AddText("");
	pt->AddLine();
	std::cout << "pre" << std::endl;
	pt->AddText(legendList[index]);
	std::cout << "post" << std::endl;

	//sprintf(out_char,"Mean = %.2f", hist_corr->GetMean());
	pt->AddText(TString::Format("Mean = %.2f", hist->GetMean()));
	pt->AddText(TString::Format("Std. dev. = %.2f", hist->GetRMS()));
	pt->AddText(TString::Format("Mean Error = %.2f", y_mean_error));
      }

      if(abs(column)!=onlyMCs){
	legend->AddEntry(g_data,"Data: "+legendList[index], "P");
	g_multi->Add(g_data,"P");
      }
    }else{
      legend->AddEntry(g_dataMC,legendList[index], "P");
      g_multi->Add(g_dataMC,"P");
    }
    
    if(xTitle.Contains("runNumber")){
      //std::cout << "ciao" << "\t" << index << std::endl;
      g_multi->Draw("A");
      TAxis *axis = g_multi->GetXaxis();
      TAxis *axis_data = g_data->GetXaxis();
      for(int i_bin =0; i_bin < (int) axis_data->GetNbins()-4; i_bin++){
	axis->SetBinLabel(i_bin+5, axis_data->GetBinLabel(i_bin)); //
      }
    }
    std::cout << doStatistics << "\t" << doFit << std::endl;


  }
    std::cout << doStatistics << "\t" << doFit << std::endl;


  c->Clear();
  g_multi->Draw("A");

  //########## graph settings
  g_multi->GetYaxis()->SetTitle(yTitle);
  if(abs(column)==dataOverMC && yTitle.Contains("#Delta m")){
    g_multi->GetYaxis()->SetTitle("#Delta P [\%]");
  }
  g_multi->GetXaxis()->SetTitle(xTitle);
  g_multi->GetXaxis()->SetLabelSize(0.05 * labelScale);
#ifdef lightLabels
  g_multi->GetXaxis()->SetLabelSize(g_multi->GetXaxis()->GetLabelSize()*1.5);
#endif
  g_multi->GetXaxis()->SetNdivisions(5);
  if(xTitle.Contains("Time") || xTitle.Contains("date")){
    g_multi->GetXaxis()->SetTimeDisplay(true);
    g_multi->GetXaxis()->SetTimeOffset(0);
    g_multi->GetXaxis()->SetLabelSize(0.05 * labelScale);
    g_multi->GetXaxis()->SetTimeFormat("%d/%m");
  }

  g_multi->GetXaxis()->SetTitleSize(g_multi->GetXaxis()->GetTitleSize()*labelScale);
  g_multi->GetXaxis()->SetTitleOffset(g_multi->GetXaxis()->GetTitleOffset()/labelScale);

  g_multi->GetYaxis()->SetTitleSize(g_multi->GetYaxis()->GetTitleSize()*labelScale);
  g_multi->GetYaxis()->SetTitleOffset(g_multi->GetYaxis()->GetTitleOffset()/labelScale*1.1);
  g_multi->GetYaxis()->SetLabelSize(g_multi->GetYaxis()->GetLabelSize()*labelScale);

  if( rMax == rMin || rMax < rMin){
    std::cout << g_multi->GetYaxis()->GetXmin() << "\t" <<  g_multi->GetYaxis()->GetXmax() *1.1 << std::endl;
    g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin(), g_multi->GetYaxis()->GetXmax() *1.1);
  } else {
    if(rMin > g_multi->GetYaxis()->GetXmin()) rMin=g_multi->GetYaxis()->GetXmin();
    if(rMax < g_multi->GetYaxis()->GetXmax()) rMax=g_multi->GetYaxis()->GetXmax();
    g_multi->GetYaxis()->SetRangeUser(rMin,rMax);
  }


  // automatic setting of the range
  TPad *pad1=NULL; 
  TPad *pad2=NULL; 
  TPad *pad3=NULL;
  TPad *pad4=NULL;

  if(column>5 && column!=onlyMCs){
    pad1= new TPad("pad1","", 0,0.4,0.75,1);
    pad2= new TPad("pad2","", 0.75,0.4,1,1);
    pad3= new TPad("pad3","", 0,0.,0.75,0.4);
    pad4= new TPad("pad4","", 0.75,0.,1,0.4);
  } else if(column<0)    pad1= new TPad("pad1","", 0,0.,0.95,0.95);
  else{
    pad1= new TPad("pad1","", 0,0.,0.75,1);
    pad2= new TPad("pad2","", 0.75,0.,1,1);
  }
  c->Clear();
  if(column>=0) pad2->Draw();
  pad1->Draw();
  if(column>5 && column!=onlyMCs){
    pad3->Draw();
    pad4->Draw();
    pad3->SetRightMargin(0.01);
    pad3->SetGridy();
  }
  pad1->cd();

  g_multi->Draw("A");
  if(xTitle.Contains("runNumber")){
    TAxis *axis = g_multi->GetXaxis();
    pad1->SetBottomMargin(0.28);
//     if(column>=0)pad2->SetBottomMargin(0.28);
    axis->SetTitleOffset(-0.3);
//     if(rMin >= rMax){
//       if(g_multi->GetYaxis()->GetXmin() > 0)
// 	g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin()*0.9,g_multi->GetYaxis()->GetXmax());
//       else
// 	g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin()*1.2,g_multi->GetYaxis()->GetXmax());
//     }
  } else {
    pad1->SetGridx();
  }

  pad1->SetRightMargin(0.01);
  pad1->SetGridy();
  
  if(column>0){
    pad2->cd();
  }

  if(legendList[0]!="") legend->Draw();
  if(pt!=NULL) pt->Draw();
  
  pad1->cd();
  pave->Draw();
  //    g_multi->Draw("Psame");
  //    g_data->GetXaxis()->Draw();
  //  }



  return c;
}

#ifdef shervin




  //------------------------------
  Double_t *Y=NULL, *Y_uncorr=NULL, *EY = NULL, *EY_uncorr = NULL;
  double y_err_mean=0,y_err_uncorr_mean=0;

  Y = g_data->GetY();
  EY = g_data->GetEY();
  if( g_data_unCorr!=NULL){ 
    Y_uncorr = g_data_unCorr->GetY();
    EY_uncorr = g_data_unCorr->GetEY();
  }

  TH1 *obj = (TH1*) g_multi->GetYaxis()->GetParent();
  TH1F *hist_corr = new TH1F("hist_corr","hist_corr",   
			     g_data->GetN(), obj->GetMinimum(), obj->GetMaximum());
  TH1F *hist_uncorr = new TH1F("hist_uncorr","hist_uncorr",   
			       g_data->GetN(), obj->GetMinimum(), obj->GetMaximum());
  hist_corr->GetXaxis()->SetLabelSize(0);
  hist_corr->GetYaxis()->SetLabelSize(0);
  for(int i =0; i < g_data->GetN(); i++){
    hist_corr -> Fill(Y[i]);
    y_err_mean+=EY[i];

    if(Y_uncorr!=NULL){
      hist_uncorr -> Fill(Y_uncorr[i]);
      y_err_uncorr_mean+=EY_uncorr[i];
    }
  }
  y_err_mean/=g_data->GetN();
  y_err_uncorr_mean/=g_data->GetN();

  if(column>=0) pad2->cd();
  hist_corr->SetFillColor(g_data->GetMarkerColor());
  //  hist_uncorr->SetFillColor(g_data_unCorr->GetMarkerColor());

  //  hist_corr->SetFillStyle(3001);
  hist_corr->SetFillStyle(1001);
  hist_uncorr->SetFillStyle(3002);

  if(column >=0 && column<2 && g_data_unCorr==NULL){
    hist_corr->Draw("hbar");
    hist_uncorr->Draw("hbar same");
  }


  if(column == -2){
    if(region.CompareTo("EB")==0)
      legend = new TLegend(0.35,0.2,0.7,0.4);
    else
      legend = new TLegend(0.35,0.15,0.9,0.4);
    pt = new TPaveText(0.15,0.73,0.6,0.94,"NDC");
    pt -> SetFillColor(0);
    pt -> SetFillStyle(0);
  } else 
  if(column == -3){
    legend = new TLegend(0.35,0.35,0.95,0.55);
    pt = new TPaveText(0.15,0.73,0.6,0.94,"NDC");
    pt -> SetFillColor(0);
    pt -> SetFillStyle(0);
  }else    if(column <0){
    if(region.CompareTo("EB")==0)
      legend = new TLegend(0.35,0.35,0.9,0.6);
    else
      legend = new TLegend(0.35,0.15,0.9,0.4);
    pt = new TPaveText(0.15,0.73,0.6,0.94,"NDC");
    pt -> SetFillColor(0);
    pt -> SetFillStyle(0);
  }else        if(column >=0 && column<2 && g_data_unCorr==NULL){
    legend = new TLegend(0,0.,1,0.10);
    pt = new TPaveText(0,0.11,1,0.28,"NDC");
  } else {
    if(g_data_unCorr==NULL){
      legend = new TLegend(0,0.1,1,0.4);
      pt = new TPaveText(0,0.4,1,1,"NDC");
    } else {
      legend = new TLegend(0,0.,1,0.3);
      pt = new TPaveText(0,0.3,1,0.9,"NDC");
    }     
  }
    

  TGraphErrors *g_data_leg = (TGraphErrors *)g_data->Clone("leg_g_data");
  TGraphErrors *g_data_unCorr_leg =NULL;

  if(g_data_unCorr!=NULL){
    g_data_unCorr_leg = (TGraphErrors *)g_data_unCorr->Clone("leg_g_data_unCorr");
    g_data_unCorr_leg->SetMarkerSize(3);
    legend->AddEntry(g_data_unCorr_leg, f_uncorr_legend,"p");
  }
  g_data_leg->SetMarkerSize(3);
  legend->AddEntry(g_data_leg, f_corr_legend,"p");


  SetLegendStyle(legend);
  if(column==-10) legend->SetTextSize(0.04);
  if(column<0) legend->SetTextSize(0.05);
  legend->SetEntrySeparation(0.01);


  //  pt->SetLabel("Data");

  if(hist_uncorr->GetEntries()>0){
    pt->AddText("");
    pt->AddLine();
    pt->AddText(f_uncorr_legend);
    sprintf(out_char,"Mean = %.2f", hist_uncorr->GetMean());
    pt->AddText(out_char);
    
    sprintf(out_char,"Std. dev. = %.2f", hist_uncorr->GetRMS());
    pt->AddText(out_char);
    
    sprintf(out_char,"Mean Error = %.2f", y_err_uncorr_mean);
    pt->AddText(out_char);
  }

  if(xTitle.Contains("runNumber")){
    pt->SetY1NDC(0.11);
    pt->SetY2NDC(0.28);
  }

  if(column!=2){
    if(column<0 ){
      pt->Clear();
      pt->AddText("CMS Preliminary 2012");
      pt->AddText("#sqrt{s} = 8TeV, L = 19.6 fb^{-1}");
      if(region.Contains("EB")) pt->AddText("ECAL Barrel");
      if(region.Contains("EE")) pt->AddText("ECAL Endcap");
      pt->Draw("NDC");
      pt->SetTextSize(0.045);
      pt->SetTextAlign(11);
      pt->SetFillStyle(1001);
    } else pt->Draw("NDC");
  }


  if(column>0)  legend->AddEntry(g_MC, "Simulation", "lf");
  if(column!=-1 && column!=2) legend->Draw("NDC");


  TLine *line = new TLine(0,0,1,0);
  line->SetLineWidth(2);
  //line->Draw();

  if(abs(column)>=2){
    if(column>2) pad3->cd();
    else pad1->cd();
    
    g_dataMC->SetTitle(0);     
    if(column!=2){
      g_dataMC->GetXaxis()->SetLabelSize(0);
    } else {
      pad1->SetGridy();
      g_dataMC->GetXaxis()->SetTitle(xTitle);
      g_dataMC->GetXaxis()->SetTitleSize(g_multi->GetXaxis()->GetTitleSize()*labelScale);
      g_dataMC->GetXaxis()->SetLabelSize(g_multi->GetXaxis()->GetLabelSize()*labelScale);
    }     
    g_dataMC->GetYaxis()->SetTitleSize(g_dataMC->GetYaxis()->GetTitleSize()*labelScale*1.3); 
    g_dataMC->GetYaxis()->SetTitleOffset(g_dataMC->GetYaxis()->GetTitleOffset()/labelScale/1.3); 
    g_dataMC->GetYaxis()->SetLabelSize(g_dataMC->GetYaxis()->GetLabelSize()*labelScale*1.3);
    //    g_dataMC->GetXaxis()->SetRangeUser(g_multi->GetYaxis()->SetRangeUser(g_multi->GetYaxis()->GetXmin(), g_multi->GetYaxis()->GetXmax() *1.1);

    g_dataMC->SetMarkerStyle(22);
    g_dataMC->SetMarkerSize(1.5);

    g_dataMC->GetYaxis()->SetTitle(yTitle2);
    g_dataMC->Draw("AP");
    if(g_dataMC_unCorr!=NULL)     g_dataMC_unCorr->Draw("Psame");

    //------------------------------
    Double_t *Y1=NULL, *Y1_uncorr=NULL, *EY1 = NULL, *EY1_uncorr = NULL;
    double y1_err_mean=0,y1_err_uncorr_mean=0;

    Y1 = g_dataMC->GetY();
    EY1 = g_dataMC->GetEY();
    if( g_dataMC_unCorr!=NULL){ 
      Y1_uncorr = g_dataMC_unCorr->GetY();
      EY1_uncorr = g_dataMC_unCorr->GetEY();
    }

    TH1 *obj1 = (TH1*) g_dataMC->GetYaxis()->GetParent();
    TH1F *hist_corr1 = new TH1F("hist_corr1","hist_corr1",   
				g_dataMC->GetN(), obj1->GetMinimum(), obj1->GetMaximum());
    TH1F *hist_uncorr1 = new TH1F("hist_uncorr1","hist_uncorr1",   
				  g_dataMC->GetN(), obj1->GetMinimum(), obj1->GetMaximum());
    for(int i =0; i < g_dataMC->GetN(); i++){
      hist_corr1 -> Fill(Y1[i]);
      y1_err_mean+=EY1[i];

      if(Y1_uncorr!=NULL){
	hist_uncorr1 -> Fill(Y1_uncorr[i]);
	y1_err_uncorr_mean+=EY1_uncorr[i];
      }
    }
    y1_err_mean/=g_dataMC->GetN();
    y1_err_uncorr_mean/=g_dataMC->GetN();

    if(column>2) pad4->cd();
    else pad2->cd();

    TLegend *legend1;
    TPaveText *pt1;
    if(column<2 && g_dataMC_unCorr==NULL){
      legend1 = new TLegend(0,0.,1,0.10);
      pt1 = new TPaveText(0,0.11,1,0.28,"NDC");
    } else {
      if(g_dataMC_unCorr==NULL){
	legend1 = new TLegend(0,0.,1,0.4);
	pt1 = new TPaveText(0,0.4,1,1,"NDC");
      } else {
	legend1 = new TLegend(0,0.,1,0.3);
	pt1 = new TPaveText(0,0.3,1,0.9,"NDC");
      }     
    }    

    TGraphErrors *g_dataMC_leg = (TGraphErrors *)g_dataMC->Clone("leg_g_dataMC");
    TGraphErrors *g_dataMC_unCorr_leg =NULL;

    g_dataMC_leg->SetMarkerSize(3);


    if(g_dataMC_unCorr!=NULL){
      legend1->AddEntry(g_dataMC_leg, "#DeltaP "+f_corr_legend,"p");
      g_dataMC_unCorr_leg = (TGraphErrors *)g_dataMC_unCorr->Clone("leg_g_dataMC_unCorr");
      g_dataMC_unCorr_leg->SetMarkerSize(3);
      legend1->AddEntry(g_dataMC_unCorr_leg, "#DeltaP "+f_uncorr_legend,"p");
    } else     legend1->AddEntry(g_dataMC_leg, "#DeltaP ","p");

    SetLegendStyle(legend1);
    //  legend->SetTextSize(0.12);
    legend1->SetEntrySeparation(0.01);

    //  pt1->SetLabel("Data");
    pt1->SetTextSize(0.1);
    pt1->SetFillColor(kWhite);
    pt1->SetBorderSize(0);
    //    char out_char[300];
    //pt1->AddLine();
    pt1->AddText(f_corr_legend);
    sprintf(out_char,"Mean = %.2f", hist_corr1->GetMean());
    pt1->AddText(out_char);

    sprintf(out_char,"Std. dev. = %.2f", hist_corr1->GetRMS());
    pt1->AddText(out_char);

    sprintf(out_char,"Mean Error = %.2f", y1_err_mean);
    pt1->AddText(out_char);

    if(hist_uncorr1->GetEntries()>0){
      pt1->AddText("");
      pt1->AddLine();
      pt1->AddText(f_uncorr_legend);
      sprintf(out_char,"Mean = %.2f", hist_uncorr1->GetMean());
      pt1->AddText(out_char);
    
      sprintf(out_char,"Std. dev. = %.2f", hist_uncorr1->GetRMS());
      pt1->AddText(out_char);
    
      sprintf(out_char,"Mean Error = %.2f", y1_err_uncorr_mean);
      pt1->AddText(out_char);
    }

    if(fabs(column/10) >0 && abs(column)%10==1){
      TFitResultPtr ptr = g_dataMC->Fit("pol1","s");
      TFitResult *fitres  = ptr.Get();
      pt1->AddText("");
      pt1->AddLine();
      sprintf(out_char,"Chi2/ndf = %.2f", fitres->Chi2()/fitres->Ndf());
      pt1->AddText(out_char);
      sprintf(out_char,"slope = %.2f #pm %.2f", fitres->GetParams()[1], fitres->GetErrors()[1]);
      pt1->AddText(out_char);
      sprintf(out_char,"inter. = %.2f #pm %.2f", fitres->GetParams()[0], fitres->GetErrors()[0]);
      pt1->AddText(out_char);
    }
    
    if(xTitle.Contains("runNumber")){
      pt1->SetY1NDC(0.11);
      pt1->SetY2NDC(0.28);
    }

    pt1->Draw("NDC");
    legend1->Draw("NDC");
  }

  return c;

}
#endif
