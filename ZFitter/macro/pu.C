{
  gROOT->ProcessLine(".L macro/puMaker.C");
  puMaker();
  gROOT->ProcessLine(".q");
}
