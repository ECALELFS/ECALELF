
//
// Macro to produce ECAL single electron calibration plots
//

void DrawCalibrationPlotsEB( Char_t* infile1 = "/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EB/WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_noEP.root",
			    Char_t* infile2 = "/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EB/Even_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_noEP.root",
			    Char_t* infile3 = "/data1/rgerosa/L3_Weight/PromptSkim_Single_Double_Electron_recoFlag/EB/Odd_WZAnalysis_PromptSkim_W-DoubleElectron_FT_R_42_V21B_Z_noEP.root",
			    int evalStat = 0,
			    Char_t* fileType = "png", 
			    Char_t* dirName = ".")
{

  bool  printPlots = false;

  // by TT
  int nbins = 500;

  // by xtal
  //int nbins = 500;

  // Set style options
  gROOT->Reset();
  gROOT->SetStyle("Plain");

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptStat(1110); 
  gStyle->SetOptFit(0); 
  gStyle->SetFitFormat("6.3g"); 
  gStyle->SetPalette(1); 
 
  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.05);
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.05);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetLabelSize(0.05);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(1.1);
  gROOT->ForceStyle();

  if ( !infile1 ) {
    cout << " No input file specified !" << endl;
    return;
  }


  if ( evalStat && (!infile2 || !infile3 )){
    cout << " No input files to evaluate statistical precision specified !" << endl;
    return;
  }

  cout << "Making calibration plots for: " << infile1 << endl;
  
  TFile *f = new TFile(infile1);
  TH2F *h_scale_EB = (TH2F*)f->Get("h_scale_EB");
  TH2F *hcmap = (TH2F*) h_scale_EB->Clone("hcmap");
  hcmap -> Reset("ICEMS");

  // Mean over phi 

  for (int iEta = 1 ; iEta < h_scale_EB->GetNbinsY()+1 ; iEta ++)
  {
   float SumIC = 0;
   int numIC = 0;
   
   for(int iPhi = 1 ; iPhi < h_scale_EB->GetNbinsX()+1 ; iPhi++)
   {
    if( h_scale_EB->GetBinContent(iPhi,iEta) !=0)
    {
      SumIC = SumIC + h_scale_EB->GetBinContent(iPhi,iEta);
      numIC ++ ;
    }
   }
   
   for (int iPhi = 1; iPhi< h_scale_EB->GetNbinsX()+1  ; iPhi++)
   {
    if(numIC!=0 && SumIC!=0)
    hcmap->SetBinContent(iPhi,iEta,h_scale_EB->GetBinContent(iPhi,iEta)/(SumIC/numIC));
   }
  }
  

  
  //-----------------------------------------------------------------
  //--- Build the precision vs ieta plot starting from the TH2F of IC
  //-----------------------------------------------------------------
  TH1F *hoccall = new TH1F("hoccall", "hoccall", 1000,0.,1000.);
  for (int jbin = 1; jbin < h_occupancy-> GetNbinsY()+1; jbin++){
    for (int ibin = 1; ibin < h_occupancy-> GetNbinsX()+1; ibin++){
      float ic = h_occupancy->GetBinContent(ibin,jbin);
      	hoccall->Fill(ic);
    }
  }

  TH1F *hspreadall = new TH1F("hspreadall", "hspreadall", 800,0.,2.);
  for (int jbin = 1; jbin < hcmap-> GetNbinsY()+1; jbin++){
    for (int ibin = 1; ibin < hcmap-> GetNbinsX()+1; ibin++){
      float ic = hcmap->GetBinContent(ibin,jbin);
      if (ic>0 && ic<2 && ic !=1)    {
	hspreadall->Fill(ic);
      }
    }
  }

  
  TH1F *hspread[172];
  char hname[100];
  char htitle[100];
  
  for (int jbin = 1; jbin < hcmap-> GetNbinsY()+1; jbin++){
    //float etaring = hcmap-> GetYaxis()->GetBinLowEdge(jbin);
    float etaring = hcmap-> GetYaxis()->GetBinCenter(jbin);
    sprintf(hname,"hspread_ring_ieta%02d",etaring);
    hspread[jbin-1]= new TH1F(hname, hname, nbins/2,0.5,1.5);
    for (int ibin = 1; ibin < hcmap-> GetNbinsX()+1; ibin++){
      float ic = hcmap->GetBinContent(ibin,jbin);
      if (ic>0 && ic<2 && ic!=1)    {
	hspread[jbin-1]->Fill(ic);
      }
    }
  }
  
  TGraphErrors *sigma_vs_ieta = new TGraphErrors();
  sigma_vs_ieta->SetMarkerStyle(20);
  sigma_vs_ieta->SetMarkerSize(1);
  sigma_vs_ieta->SetMarkerColor(kBlue+2);
  
  TGraphErrors *scale_vs_ieta = new TGraphErrors();
  scale_vs_ieta->SetMarkerStyle(20);
  scale_vs_ieta->SetMarkerSize(1);
  scale_vs_ieta->SetMarkerColor(kBlue+2);
  
  TF1 *fgaus = new TF1("fgaus","gaus",-10,10);
  int np = 0;
  for (int i = 1; i < hcmap-> GetNbinsY()+1; i++){
    float etaring = hcmap-> GetYaxis()->GetBinCenter(i);
    //float etaring = hcmap-> GetYaxis()->GetBinLowEdge(i);
    if (int(etaring)==0) continue;
    if (hspread[i-1]-> GetEntries() == 0) continue;
    if (fabs(etaring) > 60) hspread[i-1]->Rebin(2);
    float e     = 0.5*hcmap-> GetYaxis()->GetBinWidth(i);
    fgaus->SetParameter(1,1);
    fgaus->SetParameter(2,hspread[i-1]->GetRMS());
    fgaus->SetRange(1-5*hspread[i-1]->GetRMS(),1+5*hspread[i-1]->GetRMS());
    hspread[i-1]->Fit("fgaus","QR");
    sigma_vs_ieta-> SetPoint(np,etaring,fgaus->GetParameter(2)/fgaus->GetParameter(1));
    //cout << etaring << "  " << fgaus->GetParameter(2)/fgaus->GetParameter(1) << endl;
    sigma_vs_ieta-> SetPointError(np, e ,fgaus->GetParError(2)/fgaus->GetParameter(1));
    scale_vs_ieta-> SetPoint(np,etaring,fgaus->GetParameter(1));
    scale_vs_ieta-> SetPointError(np,e,fgaus->GetParError(1));
    np++;
    
  }
  
  
  if (evalStat){
  TFile *f2 = new TFile(infile2);
  TH2F *h_scale_EB_2 = (TH2F*)f2->Get("h_scale_EB");
  TH2F *hcmap2 = (TH2F*) h_scale_EB->Clone("hcmap2");
  hcmap2 -> Reset("ICEMS");

  // Mean over phi 

  for (int iEta = 1 ; iEta < h_scale_EB_2->GetNbinsY()+1 ; iEta ++)
  {
   float SumIC = 0;
   int numIC = 0;
   
   for(int iPhi = 1 ; iPhi < h_scale_EB_2->GetNbinsX()+1 ; iPhi++)
   {
    if( h_scale_EB_2->GetBinContent(iPhi,iEta) !=0)
    {
      SumIC = SumIC + h_scale_EB_2->GetBinContent(iPhi,iEta);
      numIC ++ ;
    }
   }
   
   for (int iPhi = 1; iPhi< h_scale_EB_2->GetNbinsX()+1  ; iPhi++)
   {
    if(numIC!=0 && SumIC!=0)
    hcmap2->SetBinContent(iPhi,iEta,h_scale_EB_2->GetBinContent(iPhi,iEta)/(SumIC/numIC));
   }
  }
 

  TFile *f3 = new TFile(infile3);
  TH2F *h_scale_EB_3 = (TH2F*)f3->Get("h_scale_EB");
  TH2F *hcmap3 = (TH2F*) h_scale_EB->Clone("hcmap3");
  hcmap3 -> Reset("ICEMS");

  // Mean over phi 

  for (int iEta = 1 ; iEta < h_scale_EB_3->GetNbinsY()+1 ; iEta ++)
  {
   float SumIC = 0;
   int numIC = 0;
   
   for(int iPhi = 1 ; iPhi < h_scale_EB_3->GetNbinsX()+1 ; iPhi++)
   {
    if( h_scale_EB_3->GetBinContent(iPhi,iEta) !=0)
    {
      SumIC = SumIC + h_scale_EB_3->GetBinContent(iPhi,iEta);
      numIC ++ ;
    }
   }
   
   for (int iPhi = 1; iPhi< h_scale_EB_3->GetNbinsX()+1  ; iPhi++)
  {
    if(numIC!=0 && SumIC!=0)
    hcmap3->SetBinContent(iPhi,iEta,h_scale_EB_3->GetBinContent(iPhi,iEta)/(SumIC/numIC));
  }
  }

    TH1F *hstatprecision[171];
    
    for (int jbin = 1; jbin < hcmap2-> GetNbinsY()+1; jbin++){
      //int etaring = -85+(jbin-1);
      float etaring = hcmap2-> GetYaxis()->GetBinCenter(jbin);
      sprintf(hname,"hstatprecision_ring_ieta%02d",etaring);
      hstatprecision[jbin-1] = new TH1F(hname, hname, nbins,-0.5,0.5);
      for (int ibin = 1; ibin < hcmap2-> GetNbinsX()+1; ibin++){
	float ic1 = hcmap2->GetBinContent(ibin,jbin);
	float ic2 = hcmap3->GetBinContent(ibin,jbin);
	if (ic1>0 && ic1<2 && ic1!=1 && ic2>0 && ic2 <2 && ic2!=1){
	  hstatprecision[jbin-1]->Fill((ic1-ic2)/(ic1+ic2)); // sigma (diff/sum) gives the stat. precision on teh entire sample
	}
      }


    }

    TGraphErrors *statprecision_vs_ieta = new TGraphErrors();
    statprecision_vs_ieta->SetMarkerStyle(20);
    statprecision_vs_ieta->SetMarkerSize(1);
    statprecision_vs_ieta->SetMarkerColor(kRed+2);
   
    int n = 0;
    for (int i = 1; i < hcmap2-> GetNbinsY()+1; i++){
      etaring = hcmap2-> GetYaxis()->GetBinCenter(i);
      //etaring = hcmap2-> GetYaxis()->GetBinLowEdge(i);
      if (etaring==0) continue;
      if ( hstatprecision[i-1]->GetEntries() == 0) continue;
      if (fabs(etaring) > 60)hstatprecision[i-1]->Rebin(2);
      float e     = 0.5*hcmap2-> GetYaxis()->GetBinWidth(i);
      fgaus->SetParameter(1,1);
      fgaus->SetParameter(2,hstatprecision[i-1]->GetRMS());
      fgaus->SetRange(-5*hstatprecision[i-1]->GetRMS(),5*hstatprecision[i-1]->GetRMS());
      hstatprecision[i-1]->Fit("fgaus","QR");
      statprecision_vs_ieta-> SetPoint(n,etaring,fgaus->GetParameter(2));
      statprecision_vs_ieta-> SetPointError(n,e,fgaus->GetParError(2));
      n++;
    }
 
    TGraphErrors *residual_vs_ieta = new TGraphErrors();
    residual_vs_ieta->SetMarkerStyle(20);
    residual_vs_ieta->SetMarkerSize(1);
    residual_vs_ieta->SetMarkerColor(kGreen+2);
    
    

    for (int i= 0; i < statprecision_vs_ieta-> GetN(); i++){
      double spread, espread;
      double stat, estat;
      double residual, eresidual;
      double xdummy,ex;
      sigma_vs_ieta-> GetPoint(i, xdummy, spread );
      espread = sigma_vs_ieta-> GetErrorY(i);
      statprecision_vs_ieta-> GetPoint(i, xdummy, stat );
      estat = statprecision_vs_ieta-> GetErrorY(i);
      ex = statprecision_vs_ieta-> GetErrorX(i);
      if (spread > stat ){
	residual  = sqrt( spread*spread - stat*stat );
	eresidual = sqrt( pow(spread*espread,2) + pow(stat*estat,2))/residual;
      }
      else {
	residual = 0;
	eresidual = 0;
      }
      cout << residual << " " << eresidual << endl;
      residual_vs_ieta->SetPoint(i,xdummy, residual);
      residual_vs_ieta->SetPointError(i,ex,eresidual);


    }
    
  }
  


  //------------------------------------------------------------------------


    
  
  //-----------------------------------------------------------------
  //--- Draw plots
  //-----------------------------------------------------------------
  TCanvas *c[10];

  // --- plot 0 : map of coefficients 
  c[0] = new TCanvas("cmap","cmap");
  c[0] -> cd();
  c[0]->SetLeftMargin(0.1); 
  c[0]->SetRightMargin(0.13); 
  c[0]->SetGridx();
  hcmap->GetXaxis()->SetNdivisions(1020);
  hcmap->GetXaxis() -> SetLabelSize(0.03);
  hcmap->Draw("COLZ");
  hcmap->GetXaxis() ->SetTitle("i#phi");
  hcmap->GetYaxis() ->SetTitle("i#eta");
  hcmap->GetZaxis() ->SetRangeUser(0.9,1.1);
  
  c[7] = new TCanvas("cmap2","cmap2");
  c[7] -> cd();
  c[7]->SetLeftMargin(0.1); 
  c[7]->SetRightMargin(0.13); 
  c[7]->SetGridx();
  h_scale_EB->GetXaxis()->SetNdivisions(1020);
  h_scale_EB->GetXaxis() -> SetLabelSize(0.03);
  h_scale_EB->Draw("COLZ");
  h_scale_EB->GetXaxis() ->SetTitle("i#phi");
  h_scale_EB->GetYaxis() ->SetTitle("i#eta");
  h_scale_EB->GetZaxis() ->SetRangeUser(0.9,1.1);


 
  
  // --- plot 1 : ring precision vs ieta
  c[1] = new TCanvas("csigma","csigma");
  c[1]->SetGridx();
  c[1]->SetGridy();
  sigma_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.10);
  sigma_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  sigma_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("#sigma_{c}");
  sigma_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
  sigma_vs_ieta->Draw("ap");
  if (evalStat){
    statprecision_vs_ieta->Draw("psame");
    sigma_vs_ieta->Draw("psame");
    TLegend * leg = new TLegend(0.6,0.7,0.89, 0.89);
    leg->SetFillColor(0);
    leg->AddEntry(statprecision_vs_ieta,"statistical precision", "LP");
    leg->AddEntry(sigma_vs_ieta,"spread", "LP");
    leg->Draw("same");
  }

  // --- plot 2 : scale vs ieta
  c[2] = new TCanvas("c_scale_vs_ieta","c_scale_vs_ieta");
  c[2]->SetGridx();
  c[2]->SetGridy();
  scale_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(0.95,1.05);
  scale_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  scale_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("scale");
  scale_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
  scale_vs_ieta->Draw("ap");
  
  // --- plot 3 : spread all coefficients
  c[3] = new TCanvas("cspread","cspread",500,500);
  hspreadall->SetFillStyle(3004);
  hspreadall->SetFillColor(kGreen+2);
  hspreadall->GetXaxis()-> SetRangeUser(0.8,1.2);
  hspreadall->GetXaxis()-> SetTitle("c");
  hspreadall->Draw("hs");
  gPad->Update();

  TPaveStats *s_spread = (TPaveStats*)(hspreadall->GetListOfFunctions()->FindObject("stats"));
  s_spread -> SetX1NDC(0.55); //new x start position
  s_spread -> SetX2NDC(0.85); //new x end position
  s_spread -> SetY1NDC(0.750); //new x start position
  s_spread -> SetY2NDC(0.85); //new x end position
  s_spread -> SetOptStat(1110);
  s_spread -> SetTextColor(kGreen+2);
  s_spread -> SetTextSize(0.03);
  s_spread -> Draw("sames");

  //--- plot 4 : occupancy map
  c[4] = new TCanvas("cOcc","cOcc");
  c[4]->SetLeftMargin(0.1); 
  c[4]->SetRightMargin(0.13); 
  c[4]-> cd();
  c[4]->SetGridx();
  h_occupancy->GetXaxis()->SetNdivisions(1020);
  h_occupancy->GetXaxis() -> SetLabelSize(0.03);
  h_occupancy->Draw("COLZ");
  h_occupancy->GetXaxis() ->SetTitle("i#phi");
  h_occupancy->GetYaxis() ->SetTitle("i#eta");

  
  // --- plot 5 : statistical precision vs ieta
  if (evalStat){
  c[5] = new TCanvas("cstat","cstat");
  c[5]->SetGridx();
  c[5]->SetGridy();
  statprecision_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.10);
  statprecision_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  statprecision_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("#sigma((c_{P}-c_{D})/(c_{P}+c_{D}))");
  statprecision_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
  statprecision_vs_ieta->Draw("ap");

//   TF1 *fp = new TF1("fp","pol0");
//   fp->SetRange(-40,40);
//   statprecision_vs_ieta->Fit("fp","QRN");
//   float stat = fp->GetParameter(0);
//   float estat = fp->GetParError(0);
//   cout << "Statistical precision in |ieta| < 40 --> " <<  stat << " +/- " << estat << endl;
//   sigma_vs_ieta->Fit("fp","QRN");
//   float spread = fp->GetParameter(0);
//   float espread = fp->GetParError(0);
//   cout << "Spread in |ieta| < 40 --> " <<  spread << " +/- " << espread << endl;
//   float residual = sqrt( spread*spread - stat*stat );
//   float eresidual = sqrt( pow(spread*espread,2) + pow(stat*estat,2))/residual;
//   cout << "Residual miscalibration : " << residual << " +/- " << eresidual << endl;
  
  c[6] = new TCanvas("cresidual","cresidual");
  c[6]->SetGridx();
  c[6]->SetGridy();
  residual_vs_ieta->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.05);
  residual_vs_ieta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  residual_vs_ieta->GetHistogram()->GetYaxis()-> SetTitle("residual spread");
  residual_vs_ieta->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
  residual_vs_ieta->Draw("ap");
  
   
  }
 
  //-----------------------------------------------------------------
  //--- Print plots
  //-----------------------------------------------------------------
  
  if (printPlots){

    //gStyle->SetOptStat(1110);
    c[0]->Print("IC_map.png",fileType);
    c[1]->Print("IC_precision_vs_ieta.png",fileType);
    c[2]->Print("IC_scale_vs_ieta.png",fileType);
    c[3]->Print("IC_spread.png",fileType);
    c[4]->Print("occupancy_map.png",fileType);
  }
}
