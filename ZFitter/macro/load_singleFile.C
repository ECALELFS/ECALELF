{
  gROOT->ProcessLine(".L macro/PlotDataMC.C+");
  gROOT->ProcessLine(".L macro/addChainWithFriends.C+");


  TChain *data   = (TChain *) _file0->Get("selected");

  ReassociateFriends(_file0, data);

  TDirectory *curDir = new TDirectory("curDir","");
  curDir->cd();

  TString outputPath="tmp/";

}

