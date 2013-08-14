{
  gROOT->ProcessLine(".include /afs/cern.ch/cms/slc5_amd64_gcc462/lcg/roofit/5.32.03-cms16/include");
  gROOT->ProcessLine(".L macro/mcmcDraw.C+");
  TString outDir="tmp/v8-alphaFloat-noSmooth-0.02-0.00";
  system(("mkdir -p "+outDir).Data());
  //  TTree *toys = ToyTree("test/dato/fitres/Hgg_Et-toys/old/scaleStep2smearing_5/0.01-0.00/C/","outProfile-scaleStep2smearing_5-Et_25-trigger-noPF-EB.root","scan");
  TTree *toys = ToyTree("test/dato/fitres/Hgg_Et-toys/scaleStep2smearing_8/0.02-0.00//","outProfile-scaleStep2smearing_8-Et_25-trigger-noPF-EB.root","");

  toys->Scan("catName:catIndex:constTermTrue:alphaTrue","","col=50");

  
  toys->Draw("constTerm-constTermTrue>>constTerm_0","catIndex==0");
  toys->Draw("constTerm-constTermTrue>>constTerm_1","catIndex==1");
  toys->Draw("constTerm-constTermTrue>>constTerm_2","catIndex==2");
  toys->Draw("constTerm-constTermTrue>>constTerm_3","catIndex==3");
  toys->Draw("alpha-alphaTrue>>alpha_3","catIndex==3");
  toys->Draw("alpha-alphaTrue>>alpha_2","catIndex==2");
  toys->Draw("alpha-alphaTrue>>alpha_1","catIndex==1");
  toys->Draw("alpha-alphaTrue>>alpha_0","catIndex==0");
  toys->Draw("constTerm:alpha>>constTerm_alpha_0","catIndex==0");
  toys->Draw("constTerm:alpha>>constTerm_alpha_1","catIndex==1");
  toys->Draw("constTerm:alpha>>constTerm_alpha_2","catIndex==2");
  toys->Draw("constTerm:alpha>>constTerm_alpha_3","catIndex==3");

  gStyle->SetOptStat(1);

  TCanvas *c = new TCanvas("c","");
  
  constTerm_alpha_0->Draw("colz");
  constTerm_alpha_0->GetYaxis()->SetTitle("#Delta C - #Delta C True");
  constTerm_alpha_0->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/constTerm_alpha_0.png");

  constTerm_alpha_1->Draw("colz");
  constTerm_alpha_1->GetYaxis()->SetTitle("#Delta C - #Delta C True");
  constTerm_alpha_1->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/constTerm_alpha_1.png");

  constTerm_alpha_2->Draw("colz");
  constTerm_alpha_2->GetYaxis()->SetTitle("#Delta C - #Delta C True");
  constTerm_alpha_2->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/constTerm_alpha_2.png");

  constTerm_alpha_3->Draw("colz");
  constTerm_alpha_3->GetYaxis()->SetTitle("#Delta C - #Delta C True");
  constTerm_alpha_3->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/constTerm_alpha_3.png");

  constTerm_0->Draw();
  constTerm_0->GetXaxis()->SetTitle("#Delta C - #Delta C True");
  c->SaveAs(outDir+"/constTerm_0.png");

  constTerm_1->Draw();
  constTerm_1->GetXaxis()->SetTitle("#Delta C - #Delta C True");
  c->SaveAs(outDir+"/constTerm_1.png");

  constTerm_2->Draw();
  constTerm_2->GetXaxis()->SetTitle("#Delta C - #Delta C True");
  //  constTerm_2->GetXaxis()->SetRangeUser(-0.005,0.005);
  c->SaveAs(outDir+"/constTerm_2.png");

  constTerm_3->Draw();
  constTerm_3->GetXaxis()->SetTitle("#Delta C - #Delta C True");
  c->SaveAs(outDir+"/constTerm_3.png");

  alpha_0->Draw();
  alpha_0->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/alpha_0.png");

  alpha_1->Draw();
  alpha_1->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/alpha_1.png");

  alpha_2->Draw();
  alpha_2->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/alpha_2.png");

  alpha_3->Draw();
  alpha_3->GetXaxis()->SetTitle("#Delta S - #Delta S True");
  c->SaveAs(outDir+"/alpha_3.png");
    
}

