//
// Compare L3 and Template starting from .txt files (IC mediated on phi ring)
//

{
  // Set style options

  gROOT->Reset();
  gROOT->SetStyle("Plain");

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(1); 
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


  char TxtFile1[1000] = "/afs/cern.ch/user/r/rgerosa/scratch0/FastCalibrator/WZAnalysis_SingleEle_Run2011AB.txt";
  char TxtFile2[1000] = "/data2/malberti/Calibration/calibrationTemplate_XTAL_allEvents_Jul05_Aug05_Oct03_Run2011B_new.txt";

  char buffer[80];
  int Ieta,Iphi;
  float IC;
  
  ifstream file1 (TxtFile1);
  ifstream file2 (TxtFile2);

  for (int i=0;i<3;i++)   file1.getline (&buffer[0],1000); //skip 3 lines

  TH1F *hL3 = new TH1F("L3","L3",300,0.5,1.5);
  TH1F *hTemplate = new TH1F("Template","Template",300,0.5,1.5);
  TH1F *hDiff = new TH1F("Diff","Diff",300,-0.5,0.5);

  TH2F *IC_L3 = new TH2F("IC_L3","L3",360,1, 361, 171, -85, 86);
  TH2F *IC_Template = new TH2F("IC_Template","Template",360,1, 361, 171, -85, 86);

  TH2F* corr = new TH2F("IC_L3_vs_IC_Template","IC_L3_vs_IC_Template",100,0.8,1.2,100,0.8,1.2);

  while (!file1.eof()){

    file1.getline (&buffer[0],1000); // read channels
    istrstream line (buffer);
    line >> Ieta;
    line >> Iphi;
    line >> IC;

    hL3 -> Fill(IC);
    IC_L3 -> Fill(Iphi,Ieta,IC);

  }


  while (!file2.eof()){
    file2.getline (&buffer[0],1000); // read channels
    istrstream line (buffer);    
    line >> Ieta;
    line >> Iphi;
    line >> IC;

    if (IC > 1.5 || IC < 0.5) continue;

    hTemplate -> Fill(IC);
    IC_Template-> Fill(Iphi,Ieta,IC);

  }


  for (int iphi=1; iphi<=360; iphi++)
  {
    for (int ieta=1; ieta<=171; ieta++)
    {
      if (IC_L3->GetBinContent(iphi,ieta) != 0 && IC_Template->GetBinContent(iphi,ieta) != 0){
        hDiff->Fill( IC_L3->GetBinContent(iphi,ieta) - IC_Template->GetBinContent(iphi,ieta) );
        corr -> Fill(IC_L3->GetBinContent(iphi,ieta),IC_Template->GetBinContent(iphi,ieta));
        
     }
    }
  }


  TCanvas *c1 = new TCanvas();
  hL3->Draw();

  TCanvas *c2 = new TCanvas();
  hTemplate->Draw();

  TCanvas *c3 = new TCanvas();
  IC_L3 -> GetZaxis() -> SetRangeUser(0.93,1.07);
  IC_L3->Draw("colz");

  TCanvas *c4 = new TCanvas();
  IC_Template -> GetZaxis() -> SetRangeUser(0.85,1.15);
  IC_Template->Draw("colz");

  TCanvas *c5 = new TCanvas();
  hDiff->Draw();

  TCanvas *c6 = new TCanvas();
  corr->GetXaxis()->SetRangeUser(0.8,1.2);
  corr->GetYaxis()->SetRangeUser(0.8,1.2);
  corr->Draw();
  
  double correlation_coeffcient=corr->GetCorrelationFactor();
  double N=corr->GetEntries();
  double t=(fabs(correlation_coeffcient)*sqrt(N-2))/(sqrt(1-pow(fabs(correlation_coeffcient),2)));
  double Probability=2*(1-TMath::StudentI(t,N-2));
 
  
  std::cout<<" Correlation Value = "<<corr->GetCorrelationFactor()<<" Probability = "<<Probability<<std::endl;

  bool printPlot=false;
  

  if(printPlot)
  {
   c1->Print("hL3.png","png");
   c2->Print("hTemplate.png","png");
   c3->Print("IC_L3.png","png");
   c4->Print("IC_Template.png","png");
   c5->Print("hDiff.png","png");
   c6->Print("corr.png","png");
  }

}