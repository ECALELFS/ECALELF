{
  
  gROOT->ProcessLine(".L src/runDivide_class.cc+");
  runDivide_class runDivider;
  
  std::vector<TString> v=runDivider.Divide((TTree *)_file0->Get("selected"), "", 5000);
  
  //  std::map<unsigned int, Long64_t>::const_iterator itr = 
  

}
