//
// Macro to produce ECAL single electron calibration plots
//

void DrawCalibrationPlotsEB_phiGroups(Char_t* infile1 =
"/data1/rgerosa/L3_Weight/PromptSkim_recoFlag/EB/WZAnalysis_SingleElectron_Run2011AB_WElectron-PromptSkim_Z_noEP.root",
			    Char_t* infile2 = "/data1/rgerosa/L3_Weight/PromptSkim_recoFlag/EB/Even_WZAnalysis_SingleElectron_Run2011AB_WElectron-PromptSkim_Z_noEP.root",
			    Char_t* infile3 = "/data1/rgerosa/L3_Weight/PromptSkim_recoFlag/EB/Odd_WZAnalysis_SingleElectron_Run2011AB_WElectron-PromptSkim_Z_noEP.root",
			    int evalStat = 1,
			    Char_t* fileType = "png", 
			    Char_t* dirName = ".")
{


  // by xtal
  int nbins = 500;

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
  
  TH1F *hspreadGroupEta[34];
  TH1F *hspreadGroupEtaFold[17];

  char hname[100];
  char htitle[100];
  
  int ringGroupSize = 5;
  int nStep = 0;

  for (int jbin = 1; jbin < hcmap-> GetNbinsY()+1; jbin++){
   if (jbin < 86 && (jbin-1)%ringGroupSize == 0 ) {
      nStep++;
      sprintf(hname,"hspread_ringGroup_ieta%02d",nStep);
      hspreadGroupEta[nStep-1]= new TH1F(hname, hname, nbins/2,0.5,1.5);
      sprintf(hname,"hspread_ringGroup_ietaFolded%02d",nStep);
      hspreadGroupEtaFold[nStep-1]= new TH1F(hname, hname, nbins/2,0.5,1.5);
   }
   if (jbin > 86 && (jbin-2)%ringGroupSize == 0 ) {
      nStep++;
      sprintf(hname,"hspread_ringGroup_ieta%02d",nStep);
      hspreadGroupEta[nStep-1]= new TH1F(hname, hname, nbins/2,0.5,1.5);
   }

   for (int ibin = 1; ibin < hcmap-> GetNbinsX()+1; ibin++){
      float ic = hcmap->GetBinContent(ibin,jbin);
   if (ic>0 && ic<2 && ic!=1)    {
	hspreadGroupEta[nStep-1]->Fill(ic);
        if (nStep <= 17) hspreadGroupEtaFold[nStep-1]->Fill(ic);
        else             hspreadGroupEtaFold[34-nStep]->Fill(ic);
      }
    }
  }


  TGraphErrors *sigma_vs_GroupEta = new TGraphErrors();
  sigma_vs_GroupEta->SetMarkerStyle(20);
  sigma_vs_GroupEta->SetMarkerSize(1);
  sigma_vs_GroupEta->SetMarkerColor(kBlue+2);

  TGraphErrors *sigma_vs_GroupEtaFold = new TGraphErrors();
  sigma_vs_GroupEtaFold->SetMarkerStyle(20);
  sigma_vs_GroupEtaFold->SetMarkerSize(1);
  sigma_vs_GroupEtaFold->SetMarkerColor(kBlue+2);

  TF1 *fgaus = new TF1("fgaus","gaus",-10,10);
  int np = 0;
  for (int i = 1; i < 35; i++){
    float etaring = hcmap->GetYaxis()->GetBinCenter((ringGroupSize*i + ringGroupSize*(i-1))/2 + 1);
    float e     = 0.5*ringGroupSize;
    fgaus->SetParameter(1,1);
    fgaus->SetParameter(2,hspreadGroupEta[i-1]->GetRMS());
    fgaus->SetRange(1-5*hspreadGroupEta[i-1]->GetRMS(),1+5*hspreadGroupEta[i-1]->GetRMS());
    hspreadGroupEta[i-1]->Fit("fgaus","QR");

    sigma_vs_GroupEta-> SetPoint(np,etaring,fgaus->GetParameter(2));
    sigma_vs_GroupEta-> SetPointError(np,e,fgaus->GetParError(2));
    np++;
  }

  np = 0;
  for (int i = 1; i < 18; i++){
    float etaring = hcmap->GetYaxis()->GetBinCenter((ringGroupSize*i + ringGroupSize*(i-1))/2 + 1);
    float e     = 0.5*ringGroupSize;
    fgaus->SetParameter(1,1);
    fgaus->SetParameter(2,hspreadGroupEtaFold[i-1]->GetRMS());
    fgaus->SetRange(1-5*hspreadGroupEtaFold[i-1]->GetRMS(),1+5*hspreadGroupEtaFold[i-1]->GetRMS());
    hspreadGroupEtaFold[i-1]->Fit("fgaus","QR");

    sigma_vs_GroupEtaFold-> SetPoint(np,fabs(etaring),fgaus->GetParameter(2));
    sigma_vs_GroupEtaFold-> SetPointError(np,e,fgaus->GetParError(2));
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
  TH2F *hcmap3 = (TH2F*) h_scale_EB_3->Clone("hcmap3");
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
    TH1F *hstatprecisionGroupEta[34];
    TH1F *hstatprecisionGroupEtaFold[17];

    nStep = 0;
    for (int jbin = 1; jbin < hcmap-> GetNbinsY()+1; jbin++){
    if (jbin < 86 && (jbin-1)%ringGroupSize == 0 ) {
       nStep++;
       sprintf(hname,"hstatprecision_ringGroup_ieta%02d",nStep);
       hstatprecisionGroupEta[nStep-1]= new TH1F(hname, hname, nbins,-0.5,0.5);
       sprintf(hname,"hstatprecision_ringGroup_ietaFolded%02d",nStep);
       hstatprecisionGroupEtaFold[nStep-1]= new TH1F(hname, hname, nbins,-0.5,0.5);
    }
    if (jbin > 86 && (jbin-2)%ringGroupSize == 0 ) {
       nStep++;
       sprintf(hname,"hspread_ringGroup_ieta%02d",nStep);
       hstatprecisionGroupEta[nStep-1]= new TH1F(hname, hname, nbins,-0.5,0.5);
    }

    for (int ibin = 1; ibin < hcmap2-> GetNbinsX()+1; ibin++){
       float ic1 = hcmap2->GetBinContent(ibin,jbin);
       float ic2 = hcmap3->GetBinContent(ibin,jbin);
    if (ic1>0 && ic1<2 && ic1!=1 && ic2>0 && ic2 <2 && ic2!=1)    {
	hstatprecisionGroupEta[nStep-1]->Fill((ic1-ic2)/(ic1+ic2));
        if (nStep <= 17) hstatprecisionGroupEtaFold[nStep-1]->Fill((ic1-ic2)/(ic1+ic2));
        else             hstatprecisionGroupEtaFold[34-nStep]->Fill((ic1-ic2)/(ic1+ic2));
       }
     }
   }


    TGraphErrors *statprecision_vs_GroupEta = new TGraphErrors();
    statprecision_vs_GroupEta->SetMarkerStyle(20);
    statprecision_vs_GroupEta->SetMarkerSize(1);
    statprecision_vs_GroupEta->SetMarkerColor(kRed+2);
    
    TGraphErrors *statprecision_vs_GroupEtaFold = new TGraphErrors();
    statprecision_vs_GroupEtaFold->SetMarkerStyle(20);
    statprecision_vs_GroupEtaFold->SetMarkerSize(1);
    statprecision_vs_GroupEtaFold->SetMarkerColor(kRed+2);

    np = 0;
    for (int i = 1; i < 35; i++){
      float etaring = hcmap2->GetYaxis()->GetBinCenter((ringGroupSize*i + ringGroupSize*(i-1))/2 + 1);
      float e     = 0.5*ringGroupSize;
      fgaus->SetParameter(1,1);
      fgaus->SetParameter(2,hstatprecisionGroupEta[i-1]->GetRMS());
      fgaus->SetRange(-5*hstatprecisionGroupEta[i-1]->GetRMS(),5*hstatprecisionGroupEta[i-1]->GetRMS());
      hstatprecisionGroupEta[i-1]->Fit("fgaus","QR");

      statprecision_vs_GroupEta-> SetPoint(np,etaring,fgaus->GetParameter(2));
      statprecision_vs_GroupEta-> SetPointError(np,e,fgaus->GetParError(2));
      np++;
    }

    np = 0;
    for (int i = 1; i < 18; i++){
      float etaring = hcmap2->GetYaxis()->GetBinCenter((ringGroupSize*i + ringGroupSize*(i-1))/2 + 1);
      float e     = 0.5*ringGroupSize;
      fgaus->SetParameter(1,1);
      fgaus->SetParameter(2,hstatprecisionGroupEtaFold[i-1]->GetRMS());
      fgaus->SetRange(-5*hstatprecisionGroupEtaFold[i-1]->GetRMS(),5*hstatprecisionGroupEtaFold[i-1]->GetRMS());
      hstatprecisionGroupEtaFold[i-1]->Fit("fgaus","QR");

      statprecision_vs_GroupEtaFold-> SetPoint(np,fabs(etaring),fgaus->GetParameter(2));
      statprecision_vs_GroupEtaFold-> SetPointError(np,e,fgaus->GetParError(2));
      np++;
    }


    TGraphErrors *residual_vs_GroupEta = new TGraphErrors();
    residual_vs_GroupEta->SetMarkerStyle(20);
    residual_vs_GroupEta->SetMarkerSize(1);
    residual_vs_GroupEta->SetMarkerColor(kGreen+2);

    TGraphErrors *residual_vs_GroupEtaFold = new TGraphErrors();
    residual_vs_GroupEtaFold->SetMarkerStyle(20);
    residual_vs_GroupEtaFold->SetMarkerSize(1);
    residual_vs_GroupEtaFold->SetMarkerColor(kGreen+2);

    for (int i= 0; i < statprecision_vs_GroupEta-> GetN(); i++){
      double spread, espread;
      double stat, estat;
      double residual, eresidual;
      double xdummy,ex;
      sigma_vs_GroupEta-> GetPoint(i, xdummy, spread);
      espread = sigma_vs_GroupEta-> GetErrorY(i);
      statprecision_vs_GroupEta-> GetPoint(i, xdummy, stat);
      estat = statprecision_vs_GroupEta-> GetErrorY(i);
      ex = statprecision_vs_GroupEta-> GetErrorX(i);
      if (spread > stat ){
	residual  = sqrt( spread*spread - stat*stat );
	eresidual = sqrt( pow(spread*espread,2) + pow(stat*estat,2))/residual;
      }
      else {
	residual = 0;
	eresidual = 0;
      }
      residual_vs_GroupEta->SetPoint(i,xdummy, residual);
      residual_vs_GroupEta->SetPointError(i,ex,eresidual);
    }

    for (int i= 0; i < statprecision_vs_GroupEtaFold-> GetN(); i++){
      double spread, espread;
      double stat, estat;
      double residual, eresidual;
      double xdummy,ex;
      sigma_vs_GroupEtaFold-> GetPoint(i, xdummy, spread);
      espread = sigma_vs_GroupEtaFold-> GetErrorY(i);
      statprecision_vs_GroupEtaFold-> GetPoint(i, xdummy, stat);
      estat = statprecision_vs_GroupEtaFold-> GetErrorY(i);
      ex = statprecision_vs_GroupEtaFold-> GetErrorX(i);
      if (spread > stat ){
	residual  = sqrt( spread*spread - stat*stat );
	eresidual = sqrt( pow(spread*espread,2) + pow(stat*estat,2))/residual;
      }
      else {
	residual = 0;
	eresidual = 0;
      }
      residual_vs_GroupEtaFold->SetPoint(i,xdummy, residual);
      residual_vs_GroupEtaFold->SetPointError(i,ex,eresidual);
    }

}

  //------------------------------------------------------------------------

  //-----------------------------------------------------------------
  //--- Draw plots
  //-----------------------------------------------------------------
  TCanvas *c[4];

  c[0] = new TCanvas("csigma","csigma");
  c[0]->SetGridx();
  c[0]->SetGridy();
  sigma_vs_GroupEta->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.10);
  sigma_vs_GroupEta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  sigma_vs_GroupEta->GetHistogram()->GetYaxis()-> SetTitle("#sigma_{c}");
  sigma_vs_GroupEta->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
  sigma_vs_GroupEta->Draw("ap");
  if (evalStat){
    statprecision_vs_GroupEta->Draw("psame");
    sigma_vs_GroupEta->Draw("psame");
    TLegend * leg = new TLegend(0.6,0.7,0.89, 0.89);
    leg->SetFillColor(0);
    leg->AddEntry(statprecision_vs_GroupEta,"statistical precision", "LP");
    leg->AddEntry(sigma_vs_GroupEta,"spread", "LP");
    leg->Draw("same");
  }

  c[1] = new TCanvas("csigmaFold","csigmaFold");
  c[1]->SetGridx();
  c[1]->SetGridy();
  sigma_vs_GroupEtaFold->GetHistogram()->GetYaxis()-> SetRangeUser(0.00,0.10);
  sigma_vs_GroupEtaFold->GetHistogram()->GetXaxis()-> SetRangeUser(0,85);
  sigma_vs_GroupEtaFold->GetHistogram()->GetYaxis()-> SetTitle("#sigma_{c}");
  sigma_vs_GroupEtaFold->GetHistogram()->GetXaxis()-> SetTitle("|i#eta|");
  sigma_vs_GroupEtaFold->Draw("ap");
  if (evalStat){
    statprecision_vs_GroupEtaFold->Draw("psame");
    sigma_vs_GroupEtaFold->Draw("psame");
    TLegend * leg = new TLegend(0.6,0.7,0.89, 0.89);
    leg->SetFillColor(0);
    leg->AddEntry(statprecision_vs_GroupEtaFold,"statistical precision", "LP");
    leg->AddEntry(sigma_vs_GroupEtaFold,"spread", "LP");
    leg->Draw("same");
  }


  c[2] = new TCanvas("cresidual","cresidual");
  c[2]->SetGridx();
  c[2]->SetGridy();
  residual_vs_GroupEta->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.05);
  residual_vs_GroupEta->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
  residual_vs_GroupEta->GetHistogram()->GetYaxis()-> SetTitle("residual spread");
  residual_vs_GroupEta->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
  residual_vs_GroupEta->Draw("ap");

  c[3] = new TCanvas("cresidualFold","cresidualFold");
  c[3]->SetGridx();
  c[3]->SetGridy();
  residual_vs_GroupEtaFold->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.05);
  residual_vs_GroupEtaFold->GetHistogram()->GetXaxis()-> SetRangeUser(0,85);
  residual_vs_GroupEtaFold->GetHistogram()->GetYaxis()-> SetTitle("residual spread");
  residual_vs_GroupEtaFold->GetHistogram()->GetXaxis()-> SetTitle("|i#eta|");
  residual_vs_GroupEtaFold->Draw("ap");


}
