void puMaker(){ 
  TTree* t = (TTree*)  _file0->Get("selected");
  t->Draw("nPV+1>>pileup(60,0,60)");
  TH1F *h = (TH1F *) gROOT->FindObject("pileup");
  h->SaveAs("tmp/pu_temp.root");
}
