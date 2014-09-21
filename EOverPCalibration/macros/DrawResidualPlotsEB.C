//
// Macro to produce ECAL single electron calibration plots
//

void DrawResidualPlotsEB(
			    Char_t* infile1 = "/data1/rgerosa/L3_Weight/PromptSkim/WZAnalysis_SingleEle_Run2011AB-PromptSkim_Z_noEP.root",
			    Char_t* infile2 = "/data1/rgerosa/L3_Weight/PromptSkim/Odd_WZAnalysis_SingleEle_Run2011AB-PromptSkim_Z_noEP.root",
			    Char_t* infile3 = "/data1/rgerosa/L3_Weight/PromptSkim/Even_WZAnalysis_SingleEle_Run2011AB-PromptSkim_Z_noEP.root",
                            int evalStat = 1,
                            int inputLoops = 25,
                            Char_t* fileType = "png", 
                            Char_t* dirName = ".")
{

  // Draw plots in different modality : xtal, TT, SM
  TString modality = "xtal"; 
  bool printPlots = false;
  const int nLoops = inputLoops;

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

  cout << "Making calibration plots for: " << infile2 << endl;
  
  char hname[100];
  char htitle[100];
  TF1 *fgaus = new TF1("fgaus","gaus",-10,10);
    
  TFile *f1 = new TFile(infile1);
  TFile *f2 = new TFile(infile2);
  TFile *f3 = new TFile(infile3);
  TH2F *hcmap1[nLoops];
  TH2F *hcmap2[nLoops];
  TH2F *hcmap3[nLoops];
  TGraphErrors *residual_vs_ieta[nLoops];
      
  TH2F *hcmap1_TT[nLoops];
  TH2F *hcmap2_TT[nLoops];
  TH2F *hcmap3_TT[nLoops];

  for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {
  
    sprintf(hname,"h2_%d_hC_scale_EB",iLoop);
    hcmap1[iLoop] = (TH2F*)f1->Get(hname); 
    hcmap2[iLoop] = (TH2F*)f2->Get(hname); 
    hcmap3[iLoop] = (TH2F*)f3->Get(hname); 

    if ( modality == "TT" ) {
      sprintf(hname,"h1_%d_hC_scale_EB_TT",iLoop);
      hcmap1_TT[iLoop] = new TH2F(hname, hname, 72,1, 72, 35, -17, 18);
      sprintf(hname,"h2_%d_hC_scale_EB_TT",iLoop);
      hcmap2_TT[iLoop] = new TH2F(hname, hname, 72,1, 72, 35, -17, 18);
      sprintf(hname,"h3_%d_hC_scale_EB_TT",iLoop);
      hcmap3_TT[iLoop] = new TH2F(hname, hname, 72,1, 72, 35, -17, 18);
      
      for ( int iTTphi = 1; iTTphi < 72; iTTphi++ ) {
        
        for ( int iTTeta = -17; iTTeta < 18; iTTeta++ ) {
          if ( iTTeta == 0 ) continue;
          float theICsum = hcmap1[iLoop] -> Integral(iTTphi*5, iTTphi*5+4, (iTTeta+18)*5, (iTTeta+18)*5+4)/25.;
          hcmap1_TT[iLoop] -> SetBinContent (iTTphi, iTTeta+18, theICsum);
          theICsum = hcmap2[iLoop] -> Integral(iTTphi*5, iTTphi*5+4, (iTTeta+18)*5, (iTTeta+18)*5+4)/25.;
          hcmap2_TT[iLoop] -> SetBinContent (iTTphi, iTTeta+18, theICsum);
          theICsum = hcmap3[iLoop] -> Integral(iTTphi*5, iTTphi*5+4, (iTTeta+18)*5, (iTTeta+18)*5+4)/25.;
          hcmap3_TT[iLoop] -> SetBinContent (iTTphi, iTTeta+18, theICsum);

        }
        
      }
      
    }
          
    TH1F *hscale[171];
    TH1F *hsigma[171];
    TH1F *hresidual[171];
    
    if ( modality == "xtal" ) {
    
      for (int jbin = 1; jbin < hcmap2[iLoop]-> GetNbinsY()+1; jbin++){
        //int etaring = -85+(jbin-1);
        float etaring = hcmap2[iLoop]-> GetYaxis()->GetBinCenter(jbin);
        sprintf(hname,"hscale_ring_ieta%02d",etaring);
        hscale[jbin-1] = new TH1F(hname, hname, 250,0.9,1.1);
        sprintf(hname,"hsigma_ring_ieta%02d",etaring);
        hsigma[jbin-1] = new TH1F(hname, hname, 250,-0.1,0.1);
        for (int ibin = 1; ibin < hcmap2[iLoop]-> GetNbinsX()+1; ibin++){
          float ic  = hcmap1[iLoop]->GetBinContent(ibin,jbin);
          hscale[jbin-1] -> Fill (ic);
          float ic1 = hcmap2[iLoop]->GetBinContent(ibin,jbin);
          float ic2 = hcmap3[iLoop]->GetBinContent(ibin,jbin);
          if (ic1>0 && ic1<2 && ic1!=1 && ic2>0 && ic2 <2 && ic2!=1){
            hsigma[jbin-1]->Fill((ic1-ic2)/(ic1+ic2)); 
          }
        }
      }
    
    }
    
    else if ( modality == "TT" ) {
      
      for (int jbin = 1; jbin < hcmap2_TT[iLoop]-> GetNbinsY()+1; jbin++){
        //int etaring = -85+(jbin-1);
        float etaring = hcmap2_TT[iLoop]-> GetYaxis()->GetBinCenter(jbin);
        sprintf(hname,"hscale_ring_ieta%02d",etaring);
        hscale[jbin-1] = new TH1F(hname, hname, 250,0.9,1.1);
        sprintf(hname,"hsigma_ring_ieta%02d",etaring);
        hsigma[jbin-1] = new TH1F(hname, hname, 250,-0.1,0.1);
        for (int ibin = 1; ibin < hcmap2_TT[iLoop]-> GetNbinsX()+1; ibin++){
          float ic  = hcmap1_TT[iLoop]->GetBinContent(ibin,jbin);
          hscale[jbin-1] -> Fill (ic);
          float ic1 = hcmap2_TT[iLoop]->GetBinContent(ibin,jbin);
          float ic2 = hcmap3_TT[iLoop]->GetBinContent(ibin,jbin);
          if (ic1>0 && ic1<2 && ic1!=1 && ic2>0 && ic2 <2 && ic2!=1){
            hsigma[jbin-1]->Fill((ic1-ic2)/(ic1+ic2)); 
          }
        }
      }
      
    }
    else if ( modality == "SM" ) {
      
      for (int ibin = 1; ibin < hcmap2[iLoop]-> GetNbinsX()+1; ibin++){
        // Get the SM number
        int iSM = (ibin-1)/20 + 1;
        if ( (ibin-1)%20 == 0 ) {
          sprintf(hname,"hscale_ring_ieta%02d",iSM);
          hscale[iSM-1] = new TH1F(hname, hname, 250,0.9,1.1);
          sprintf(hname,"hscale_ring_ieta%02d",iSM+18);
          hscale[iSM-1+18] = new TH1F(hname, hname, 250,0.9,1.1);
          sprintf(hname,"hsigma_ring_ieta%02d",iSM);
          hsigma[iSM-1] = new TH1F(hname, hname, 250,-0.1,0.1);
          sprintf(hname,"hsigma_ring_ieta%02d",iSM+18);
          hsigma[iSM-1+18] = new TH1F(hname, hname, 250,-0.1,0.1);
        }
        for (int jbin = 1; jbin < hcmap2[iLoop]-> GetNbinsY()+1; jbin++){
          float ic  = hcmap1[iLoop]->GetBinContent(ibin,jbin);
          float ic1 = hcmap2[iLoop]->GetBinContent(ibin,jbin);
          float ic2 = hcmap3[iLoop]->GetBinContent(ibin,jbin);
          if (ic1>0 && ic1<2 && ic1!=1 && ic2>0 && ic2 <2 && ic2!=1){
            float sigma = (ic1-ic2)/(ic1+ic2); // sigma (diff/sum) gives the stat. precision on teh entire sample
            if ( jbin < 86 ) { //EE-
              hscale[iSM-1+18]->Fill(ic);
              hsigma[iSM-1+18]->Fill(sigma); // sigma (diff/sum) gives the stat. precision on teh entire sample
            }
            else { //EE+
              hscale[iSM-1]->Fill(ic); 
              hsigma[iSM-1]->Fill(sigma); // sigma (diff/sum) gives the stat. precision on teh entire sample
            }
          }
        }
      }
      
    }

    
    residual_vs_ieta[iLoop] = new TGraphErrors();
    residual_vs_ieta[iLoop]->SetMarkerStyle(20);
    residual_vs_ieta[iLoop]->SetMarkerSize(1);
    residual_vs_ieta[iLoop]->SetMarkerColor(kRed+2);
  
    int n = 0;
    
    if ( modality == "xtal" ) {

      for (int i = 1; i < hcmap2[iLoop]-> GetNbinsY()+1; i++){
        etaring = hcmap2[iLoop]-> GetYaxis()->GetBinCenter(i);
        if (etaring==0) continue;
        if ( hsigma[i-1]->GetEntries() == 0) continue;
        float e     = 0.5*hcmap2[iLoop]-> GetYaxis()->GetBinWidth(i);
        // Get the sigma
        fgaus->SetParameter(1,1);
        fgaus->SetParameter(2,hsigma[i-1]->GetRMS());
        fgaus->SetRange(-5*hsigma[i-1]->GetRMS(),5*hsigma[i-1]->GetRMS());
        hsigma[i-1] -> Fit("fgaus","QR");
        float sigma = fgaus -> GetParameter(2);
        // Get the scale
        fgaus->SetParameter(1,1);
        fgaus->SetParameter(2,hscale[i-1]->GetRMS());
        fgaus->SetRange(-5*hscale[i-1]->GetRMS()+hscale[i-1]->GetMean(),5*hscale[i-1]->GetRMS()+hscale[i-1]->GetMean());
        hscale[i-1] -> Fit("fgaus","QR");
        float scale = fgaus -> GetParameter(2);
        float residual = scale*scale - sigma*sigma;
        if ( residual < 0 ) residual = -sqrt(-residual);
        else residual = sqrt(residual);
        residual_vs_ieta[iLoop]-> SetPoint(n,etaring,residual);
        residual_vs_ieta[iLoop]-> SetPointError(n,e,fgaus->GetParError(2));
        n++;
      }
      
    }
    
    else if ( modality == "TT" ) {
      
      for (int i = 1; i < hcmap2_TT[iLoop]-> GetNbinsY()+1; i++){
        etaring = hcmap2_TT[iLoop]-> GetYaxis()->GetBinCenter(i);
        if (etaring==0) continue;
        if ( hsigma[i-1]->GetEntries() == 0) continue;
        float e     = 0.5*hcmap2_TT[iLoop]-> GetYaxis()->GetBinWidth(i);
        // Get the sigma
        fgaus->SetParameter(1,1);
        fgaus->SetParameter(2,hsigma[i-1]->GetRMS());
        fgaus->SetRange(-5*hsigma[i-1]->GetRMS(),5*hsigma[i-1]->GetRMS());
        hsigma[i-1] -> Fit("fgaus","QR");
        float sigma = fgaus -> GetParameter(2);
        // Get the scale
        fgaus->SetParameter(1,1);
        fgaus->SetParameter(2,hscale[i-1]->GetRMS());
        fgaus->SetRange(-5*hscale[i-1]->GetRMS()+hscale[i-1]->GetMean(),5*hscale[i-1]->GetRMS()+hscale[i-1]->GetMean());
        hscale[i-1] -> Fit("fgaus","QR");
        float scale = fgaus -> GetParameter(2);
        float residual = scale*scale - sigma*sigma;
        if ( residual < 0 ) residual = -sqrt(-residual);
        else residual = sqrt(residual);
        residual_vs_ieta[iLoop]-> SetPoint(n,etaring,residual);
        residual_vs_ieta[iLoop]-> SetPointError(n,e,fgaus->GetParError(2));
        n++;
      }
      
    }
    
    else if ( modality == "SM" ) {
      
      for (int i = 1; i < 36+1; i++){

        if ( hsigma[i-1]->GetEntries() == 0) continue;
        float e     = 0.5;
        // Get the sigma
        fgaus->SetParameter(1,1);
        fgaus->SetParameter(2,hsigma[i-1]->GetRMS());
        fgaus->SetRange(-5*hsigma[i-1]->GetRMS(),5*hsigma[i-1]->GetRMS());
        hsigma[i-1] -> Fit("fgaus","QR");
        float sigma = fgaus -> GetParameter(2);
        // Get the scale
        fgaus->SetParameter(1,1);
        fgaus->SetParameter(2,hscale[i-1]->GetRMS());
        fgaus->SetRange(-5*hscale[i-1]->GetRMS()+hscale[i-1]->GetMean(),5*hscale[i-1]->GetRMS()+hscale[i-1]->GetMean());
        hscale[i-1] -> Fit("fgaus","QR");
        float scale = fgaus -> GetParameter(2);
        float residual = scale*scale - sigma*sigma;
        if ( residual < 0 ) residual = -sqrt(-residual);
        else residual = sqrt(residual);
        residual_vs_ieta[iLoop]-> SetPoint(n,i,residual);
        residual_vs_ieta[iLoop]-> SetPointError(n,e,fgaus->GetParError(2));
        n++;

        
      }
      
      
    }
    
//       delete hresidual;
    
  }
  


  //------------------------------------------------------------------------


    
  
  //-----------------------------------------------------------------
  //--- Draw plots
  //-----------------------------------------------------------------
  TCanvas *c[nLoops];
  
  // --- plot 5 : statistical precision vs ieta
  if (evalStat){
    
    for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {
      sprintf(hname,"cstat_%d",iLoop);
      c[iLoop] = new TCanvas(hname,hname);
      c[iLoop]->SetGridx();
      c[iLoop]->SetGridy();
      residual_vs_ieta[iLoop]->GetHistogram()->GetYaxis()-> SetRangeUser(0.0001,0.07);
      residual_vs_ieta[iLoop]->GetHistogram()->GetXaxis()-> SetRangeUser(-85,85);
      residual_vs_ieta[iLoop]->GetHistogram()->GetYaxis()-> SetTitle("residual miscalibration");
      residual_vs_ieta[iLoop]->GetHistogram()->GetXaxis()-> SetTitle("i#eta");
      if ( modality = "SM" ) residual_vs_ieta[iLoop]->GetHistogram()->GetXaxis()-> SetTitle("iSM");
      residual_vs_ieta[iLoop]->Draw("ap");
    }
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


