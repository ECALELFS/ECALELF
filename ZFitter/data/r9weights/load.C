{
  gROOT->ProcessLine(".L macro/PlotDataMC.C+");
  gROOT->ProcessLine(".L src/setTDRStyle.C");
  gROOT->ProcessLine(".L macro/addChainWithFriends.C+");

  setTDRStyle1();

  TChain *signal = (TChain *) _file0->Get("selected");
  TChain *data   = (TChain *) _file1->Get("selected");

  ReassociateFriends(_file0, signal);
  ReassociateFriends(_file1, data);

   TDirectory *curDir = new TDirectory("curDir","");
   curDir->cd();

  TString outputPath="tmp/";

// look at standardDataMC.C for some examples  
}

