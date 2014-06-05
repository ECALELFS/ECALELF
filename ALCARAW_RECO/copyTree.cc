#include "copyTree.h"
#include "TFile.h"

int main(int argc, char* argv[])
{
  if (argc<3)
  {
    std::cout << "> drawMee <input_tree_file.root> <output_file.root> <max_n_events> <selection>"
    << std::endl;
    return 0;
  }
  
  std::string inrootfilename(argv[1]);
  std::string outrootfilename(argv[2]);

  int maxevt = -1;
  char* selection;
  if (argc>3)
  {
    maxevt = atoi(argv[3]);
 
    if (argc>4)
    {
       selection = argv[4];
    }
  }
  
  std::cout << "Input file : " << inrootfilename.c_str() << std::endl;

  // reading data
  TFile* infile = TFile::Open(inrootfilename.c_str());
  TTree* tree = (TTree*)infile->Get("selected");
  TTree* extree = (TTree*)infile->Get("extraCalibTree");
  extree->BuildIndex("runNumber","eventNumber");
   
  // output root file
  TFile* fout = new TFile(outrootfilename.c_str(), "recreate");

  // Set the branches for the TChain/TTree
  SetTreeBranch(tree);
  gROOT->ProcessLine("#include <vector>"); 
  SetExtraTreeBranch(extree);

  // clone tree
  TTree* newtree;
  if (maxevt>0) newtree = (TTree*)tree->CopyTree(selection, "", maxevt);
  else newtree = (TTree*)tree->CopyTree(selection);
  SetTreeBranch(newtree);

  TTree* newextree = (TTree*)extree->CloneTree(0);
  SetExtraTreeBranch(newextree);

  int newEntries = newtree->GetEntries();
  for (int i=0; i<newEntries; i++)
  {
    newtree->GetEntry(i);
    extree->GetEntryWithIndex(runNumber, eventNumber);
    newextree->Fill();
  } 
  
  // write the hist to out root file
  fout->cd();
  
  newtree->Write();
  newextree->Write();

  
  // close
  fout->Close();
  
  //
  return 1;
}
