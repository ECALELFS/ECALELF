{
  bool smooth=true;
  bool zoom=true;
  gROOT->ProcessLine(".include /afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.32.03-cms16/include");
  gROOT->ProcessLine(".L macro/mcmcDraw.C+");
  TCanvas *c = new TCanvas("c","");
  //  gROOT->ProcessLine(".L ~/smooth.C+");
  TString constTermName="constTerm_absEta_0_1-bad-Et_25-trigger-noPF";
  //TString constTermName="constTerm_absEta_0_1-gold-Et_25-trigger-noPF";
  //TString constTermName="constTerm_absEta_1_1.4442-bad-Et_25-trigger-noPF";
  //TString constTermName="constTerm_absEta_1_1.4442-gold-Et_25-trigger-noPF";
  TString alphaName=constTermName; alphaName.ReplaceAll("constTerm","alpha");
  RooDataSet *data = (RooDataSet *)_file0->Get(constTermName);
  
  TTree *tree = dataset2tree(data);
  double min=tree->GetMinimum("nll");
  TString minString; minString+=min;
  //  tree->Draw(TString(constTermName).ReplaceAll("-","_")+":"+TString(alphaName).ReplaceAll("-","_")+">>hist(31,-0.0022975,0.1401475,40,0.00025,0.02025)","nll-"+minString,"colz",800,5);
  //  TH2F *h = (TH2F*) gROOT->FindObject("hist");
  //  h->GetZaxis()->SetRangeUser(0,100);

  TH2F *hprof = prof2d(tree,  TString(constTermName).ReplaceAll("-","_"), TString(alphaName).ReplaceAll("-","_"), "nll", 
		       "(241,-0.00025,0.24025,60,0.00025,0.03025)", true,smooth);
  hprof->Draw("colz");
  if(zoom){
    hprof->GetZaxis()->SetRangeUser(0,10);
    hprof->GetXaxis()->SetRangeUser(0,0.15);
    hprof->GetYaxis()->SetRangeUser(0,0.018);
  } else {
    hprof->GetZaxis()->SetRangeUser(0,30);
    hprof->GetXaxis()->SetRangeUser(0,0.15);
    hprof->GetYaxis()->SetRangeUser(0,0.018);
  }
  hprof->GetXaxis()->SetTitle("#Delta #alpha");
  hprof->GetYaxis()->SetTitle("#Delta #sigma");
  Int_t iBinX, iBinY;
  Double_t x,y;
  hprof->GetBinWithContent2(1.99999660253524780e-04,iBinX,iBinY);
  x= hprof->GetXaxis()->GetBinCenter(iBinX);
  y= hprof->GetYaxis()->GetBinCenter(iBinY);
  TGraph nllBestFit(1,&x,&y);
  std::cout << constTermName << "\t" << x << "\t" << y << std::endl;
  nllBestFit.SetMarkerStyle(3);
  nllBestFit.SetMarkerColor(kRed);
  nllBestFit.Draw("P same");

  TString fileName=constTermName;
  if(!smooth) fileName+="-noSmooth";
  if(zoom) fileName+="-zoom";

  c->SaveAs(fileName+".png");
  c->SaveAs(fileName+".eps");

}
