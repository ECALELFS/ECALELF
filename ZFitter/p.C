{
  int nSmooth=0;
  TString optSmooth="k3a";
  gROOT->ProcessLine(".include /afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.32.03-cms16/include");
  gROOT->ProcessLine(".L macro/mcmcDraw.C+");
  //TString filename="test/dato/fitres/Hgg_Et-toys/scaleStep2smearing_9/factorizedSherpaFixed_DataSeedFixed_smooth_autobin_cmscaf1nd/0.007-0.05-1.00/10/3/outProfile-scaleStep2smearing_9-Et_20-trigger-noPF-EB.root";
  //  TString filename="test/dato/22Jan2012-runDepMC/loose/invMass_SC_regrCorrSemiParV5_pho/step2/fitres/outProfile-scaleStep2smearing_9-Et_20-trigger-noPF.root";
  TString filename="test/dato/22Jan2012-runDepMCAll/loose/invMass_SC_regrCorrSemiParV5_pho/step8/50/fitres/outProfile-scaleStep2smearing_9-Et_20-trigger-noPF.root";

//   MakePlots(filename, 10, 10, optSmooth);
//   MakePlots(filename, 10, 3,  optSmooth);
//   MakePlots(filename, 10, 1,  optSmooth);
  MakePlots(filename, 300, 0,  optSmooth);
}
  TCanvas *c = new TCanvas("c","");
  //  gROOT->ProcessLine(".L ~/smooth.C+");
  //TString constTermName="constTerm_absEta_0_1-bad-Et_25-trigger-noPF";
  //TString constTermName="constTerm_absEta_0_1-gold-Et_25-trigger-noPF";
  TString constTermName="constTerm_absEta_1_1.4442-bad-Et_25-trigger-noPF";
  //TString constTermName="constTerm_absEta_1_1.4442-gold-Et_25-trigger-noPF-EB";
  TString alphaName=constTermName; alphaName.ReplaceAll("constTerm","alpha");
  RooDataSet *data = (RooDataSet *)_file0->Get(constTermName);
  if(data!=NULL){
  TTree *tree = dataset2tree(data);
  double min=tree->GetMinimum("nll");
  TString minString; minString+=min;
  //  tree->Draw(TString(constTermName).ReplaceAll("-","_")+":"+TString(alphaName).ReplaceAll("-","_")+">>hist(31,-0.0022975,0.1401475,40,0.00025,0.02025)","nll-"+minString,"colz",800,5);
  //  TH2F *h = (TH2F*) gROOT->FindObject("hist");
  //  h->GetZaxis()->SetRangeUser(0,100);

  TH2F *hprof = prof2d(tree,  TString(constTermName).ReplaceAll("-","_"), TString(alphaName).ReplaceAll("-","_"), "nll", 
		       "(241,-0.00025,0.24025,60,0.00025,0.03025)", true, nSmooth);
  hprof->Draw("colz");
  hprof->GetZaxis()->SetRangeUser(0,30);
  hprof->GetXaxis()->SetRangeUser(0,0.15);
  hprof->GetYaxis()->SetRangeUser(0,0.018);
  
  hprof->GetXaxis()->SetTitle("#Delta S");
  hprof->GetYaxis()->SetTitle("#Delta C");
  Int_t iBinX, iBinY;
  Double_t x,y;
  hprof->GetBinWithContent2(1.99999660253524780e-04,iBinX,iBinY);
  x= hprof->GetXaxis()->GetBinCenter(iBinX);
  y= hprof->GetYaxis()->GetBinCenter(iBinY);
  TGraph nllBestFit(1,&x,&y);

  TString fileName=constTermName;
  fileName+="-"; fileName+=nSmooth;

  nllBestFit.SetMarkerStyle(3);
  nllBestFit.SetMarkerColor(kRed);
  nllBestFit.Draw("P same");

  std::cout << fileName << std::endl;
  ofstream fout(fileName+".dat", ios_base::app);
  fout << constTermName << "\t" << x << "\t" << y << std::endl;


  c->SaveAs(fileName+".png");
  c->SaveAs(fileName+".eps");

  fileName+="-zoom";
  hprof->GetZaxis()->SetRangeUser(0,3);
  hprof->GetXaxis()->SetRangeUser(0.04,0.12);
  hprof->GetYaxis()->SetRangeUser(0,0.005);
  c->SaveAs(fileName+".png");
  c->SaveAs(fileName+".eps");

  }
}
