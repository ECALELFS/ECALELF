{
  c1 = new TCanvas("c1","SF ele veto",200,10,700,500);
  //c1->SetFillColor(42);
  //c1->SetGrid();
  //c1->GetFrame()->SetFillColor(21);
  //c1->GetFrame()->SetBorderSize(12);
  const Int_t n = 3;

  Double_t y[n]  = {    1.00687590075,0.943667243719,1.03375527426 };
  Double_t ey[n] = {  0.0621014822419, 0.129975570451,0.294470567649};
  Double_t x[n]  = {15.,30.,70.};
  Double_t ex[n] = {5.,10.,30.};
  gr = new TGraphErrors(n,x,y,ex,ey);
  gr->SetMarkerColor(kBlue);
  gr->SetLineColor(kBlue);
  gr->SetMarkerStyle(20);
  gr->Draw("A");
  gr->GetXaxis()->SetLimits(10,100);
  gr->SetMinimum(0.5);
  gr->SetMaximum(1.4);
  gr->GetXaxis()->SetTitle("Probe E_{T} [GeV]");
  gr->GetYaxis()->SetTitle("Ele veto SF");
  gr->Draw("APsame");
  TLegend* leg=new TLegend(0.7,0.8,0.8,0.9);
  leg->AddEntry(gr,"EB","pe");
  leg->Draw("same");

  c1->SaveAs("~/scratch1/www/TP/76/Zeeg/data_MC/sf_et_EB.png");
  c1->SaveAs("~/scratch1/www/TP/76/Zeeg/data_MC/sf_et_EB.pdf");
  c1->SaveAs("~/scratch1/www/TP/76/Zeeg/data_MC/sf_et_EB.C");

  ///////Endcap
  c2 = new TCanvas("c2","SF ele veto",200,10,700,500);
  Double_t x_endcap[2]  = {15.,30.};
  Double_t ex_endcap[2] = {5.,10.};
  Double_t y_endcap[2] = {1.02031302031,1.07214428858};
  Double_t ey_endcap[2] ={0.16282290746,0.264425511861};

  gr_endcap = new TGraphErrors(2,x_endcap,y_endcap,ex_endcap,ey_endcap);
  gr_endcap->SetMarkerColor(kRed);
  gr_endcap->SetLineColor(kRed);
  gr_endcap->SetMarkerStyle(21);
  gr_endcap->Draw("A");
  gr_endcap->GetXaxis()->SetTitle("Probe E_{T} [GeV]");
  gr_endcap->GetYaxis()->SetTitle("Ele veto SF");
  gr_endcap->GetXaxis()->SetLimits(10,40);
  gr_endcap->SetMinimum(0.5);
  gr_endcap->SetMaximum(1.4);
  gr_endcap->Draw("APsame");
  TLegend* leg_endcap=new TLegend(0.7,0.8,0.8,0.9);
  leg_endcap->AddEntry(gr_endcap,"EE","pe");
  leg_endcap->Draw("same");

  c2->SaveAs("~/scratch1/www/TP/76/Zeeg/data_MC/sf_et_EE.png");
  c2->SaveAs("~/scratch1/www/TP/76/Zeeg/data_MC/sf_et_EE.pdf");
  c2->SaveAs("~/scratch1/www/TP/76/Zeeg/data_MC/sf_et_EE.C");

    //return c1;
}
