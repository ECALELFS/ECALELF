#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TString.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TH2F.h>
#include <map>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TList.h>
#include <TObject.h>
#include <TKey.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TGaxis.h>
#include <iomanip> 
#define batch

//typedef std::vector<bin_t>  hist_t;
typedef TH1F*  hist_t;

TString img_filename(TString filename, TString region, TString ext=".eps"){
  //  TString filename=file->GetName(); //"testing"+region+"_Histos.root";
  //  TFile file(filename,"open");
  
  filename.ReplaceAll("fitres/","./img/");
    
  if(region.Length()!=0) region.Insert(0,"_");
  
  filename.ReplaceAll(".root",ext);
  filename.Insert(filename.Length()-4,region);
  return filename;
}

typedef struct{
  double y;
  double y2;
  double n;
} bin_t;

void AddHist(hist_t& hist, TH1F *h){
  hist=h;
  return;
}

TH1F* GetMeanHist(hist_t hist, TH1F *h, TString name, bool data=false){
  return hist;
}

TH1F *GetRatioHist(TH1F *data, TH1F *mc){

  TString mcName=mc->GetName();
  TH1F *ratio_hist = (TH1F *) data->Clone(mcName+"_ratio");
  ratio_hist->Sumw2();
  ratio_hist->Divide(mc);


  return ratio_hist;
}

//void PlotRatioHist(TH1F *ratio

void PlotCanvas(TCanvas *c, TH1F *mc, TH1F *data, TH1F *mcSmeared){

  TH1F *ratio_smeared=GetRatioHist(data, mcSmeared);
  TH1F *ratio_mc=GetRatioHist(data, mc);

  c->Clear();
  c->cd();
  TPad *pad1;
  pad1=(TPad *) gROOT->FindObject("histPlot_pad");
  if(pad1==NULL) pad1  = new TPad("histPlot_pad","",0,0.3,1,1);
  TPad *pad2 = (TPad *) gROOT->FindObject("ratioPlot_pad");
  if(pad2==NULL) pad2  = new TPad("ratioPlot_pad","",0,0.,1,0.3);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  mc->DrawNormalized("hist",data->Integral());
  
  mcSmeared->SetLineWidth(3);
  mcSmeared->DrawNormalized("same hist",data->Integral());
  
  data->Draw("E same");

  
  pad2->cd();
  pad2->SetTopMargin(0);
  pad2->SetTicks(0,0);

  ratio_smeared->Draw();
  ratio_smeared->GetYaxis()->SetRangeUser(0.5,1.5);
  ratio_mc->Draw("same");
  ratio_mc->SetMarkerStyle(22);
  ratio_mc->SetMarkerColor(kRed);
  
  //  TLine *line = gROOT->FindObject("orizz_line");
  //  if(line==NULL) line = new TLine(
  TLine *line = new TLine(75,1,105,1);
  line->Draw();
  //  ratio_mc->GetYaxis()->Draw("X+");

  //  ratio_mc->Draw("A");
  //  ratio_mc->GetYaxis()->Draw();

  return;

}



void Plot(TCanvas *c, TH1F *data, TH1F *mc, TH1F *mcSmeared=NULL, TLegend *legend=NULL, TString region="", TString filename="",  TString energy="8 TeV", TString lumi="", bool ratio=true){

  c->Clear();
  TPad * pad1 = new TPad("pad1", "pad1",0.00,0.25, 1,1.);  
  TPad * pad2 = new TPad("pad2", "pad2",0.00,0.00, 1,0.25);  
  TPad * pad3 = new TPad("pad3", "pad3",0.75,0.00, 1.,0.2);

  float yscale=0.75/(pad1->GetYlowNDC() -pad2->GetYlowNDC());
  float xscale=1;

  if(ratio){


  pad1->SetRightMargin(0.15);
  pad1->SetBottomMargin(0.01);
  //  pad1->SetLogy();
  pad1->Draw();
  pad1->cd();
  c->cd();
  pad2->SetGrid();
  pad2->SetBottomMargin(0.5);
  pad2->SetTopMargin(0.01);
  pad2->SetRightMargin(0.15);
  pad2->Draw();
  pad2->cd();
  c->cd();
  pad3->SetGrid();    
  //pad2->SetTopMargin(0.01);
  pad3->SetBottomMargin(0.4);
  pad3->SetRightMargin(0.1);
  //  pad3->Draw();
  //  pad3->cd();
    
  pad1->cd();

  }

  TGaxis::SetMaxDigits(3);

  if(legend==NULL) legend = new TLegend(0.7,0.8,0.95,0.92);
	
  data->SetMarkerStyle(20);
       
  mc->SetLineColor(kRed);
  mc->SetLineWidth(2);
  mc->SetFillStyle(1001); //3002
  mc->SetFillColor(kRed);
  
  mc->Draw();
  
  mc->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
  //    mc->GetXaxis()->SetRangeUser(75,105);
  char ylabel[100];
  sprintf(ylabel,"Events/%.1f GeV",mc->GetBinWidth(1));
  mc->GetYaxis()->SetTitle(ylabel);
  mc->GetYaxis()->SetTitleOffset(1.3);

  TH1* mcNorm = mc->DrawNormalized("hist",data->Integral());
  float ymax=std::max(mcNorm->GetMaximum(),data->GetMaximum())*1.2;
  mcNorm->GetYaxis()->SetRangeUser(0,ymax);
  
  mcSmeared->SetLineWidth(3);
  TH1* mcSmeared_norm = mcSmeared->DrawNormalized("same hist",data->Integral());
  
  data->Draw("E same");
  
  //	std::cout << data->Integral() << std::endl;
  
  //    PlotCanvas(c, mc, data, mcSmeared);
  
  legend->Clear();
  legend->AddEntry(mc,"MC","f");
  legend->AddEntry(mcSmeared,"MC smeared","l");
  legend->AddEntry(data,"data","p");
  double KS=data->KolmogorovTest(mcSmeared);
  TString ks="Kolmogorov test: ";
  char line[50];
  sprintf(line, "%.2f", KS);
  ks+=line;
  std::cout << "" << ks << std::endl;
  
  TPaveText pave(0.182,0.8,0.38,0.92, "ndc");
  // 	pv->DeleteText();
  pave.SetFillColor(0);
  pave.SetTextAlign(12);
  pave.SetBorderSize(0);
  pave.SetTextSize(0.036);
  pave.AddText("CMS Preliminary");
  pave.AddText("#sqrt{s}="+energy);
  if(lumi.Sizeof()>1) pave.AddText("L="+lumi+" fb^{-1}");
  //    pave.AddText("");
  
  //pave.AddText(ks);
  
  
  //c->cd(0);
  pave.Draw();	


    //    pave.Paint();
    //    pave.Print();
    legend->SetTextFont(22); // 132
    legend->SetTextSize(0.04); // l'ho preso mettendo i punti con l'editor e poi ho ricavato il valore con il metodo GetTextSize()
    //  legend->SetFillColor(0); // colore di riempimento bianco
    legend->SetMargin(0.4);  // percentuale della larghezza del simbolo
  //    SetLegendStyle(legend);
    
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetFillStyle(1001);
    legend->Draw();


    if(ratio){
      TH1F *sRatio = (TH1F*) data->Clone("sRatio");
      sRatio->Divide(mcSmeared_norm);

      pad2->cd();
      sRatio->Draw();

      TGraphErrors *ratioGraph = new TGraphErrors(sRatio);
      ratioGraph->SetMarkerColor(kBlue);
      ratioGraph->Draw("AP");
      ratioGraph->GetXaxis()->SetTitle(mc->GetXaxis()->GetTitle());
      ratioGraph->GetYaxis()->SetTitle("Data/MC");
      
      ratioGraph->GetYaxis()->SetTitleSize(sRatio->GetYaxis()->GetTitleSize()*yscale);
      ratioGraph->GetYaxis()->SetTitleOffset(sRatio->GetYaxis()->GetTitleOffset()/yscale);
      ratioGraph->GetYaxis()->SetLabelSize(sRatio->GetYaxis()->GetLabelSize()*yscale);
      ratioGraph->GetYaxis()->SetLabelOffset(sRatio->GetYaxis()->GetLabelOffset()*yscale);
      ratioGraph->GetXaxis()->SetTitleSize(sRatio->GetYaxis()->GetTitleSize() *yscale   );
      ratioGraph->GetXaxis()->SetTitleOffset(0.92);
      ratioGraph->GetXaxis()->SetLabelSize(sRatio->GetYaxis()->GetLabelSize() *yscale   );
      ratioGraph->GetXaxis()->SetLabelOffset(sRatio->GetYaxis()->GetLabelOffset());
      
      ratioGraph->GetYaxis()->SetRangeUser(1- 2*ratioGraph->GetRMS(2),1+2*ratioGraph->GetRMS(2));
      ratioGraph->GetYaxis()->SetNdivisions(5);
      
      c->cd();
    }

    c->SaveAs(img_filename(filename, region, ".eps"));
    c->SaveAs(img_filename(filename, region, ".png"));
    c->SaveAs(img_filename(filename, region, ".C"));
    delete mc;
    mc=NULL;
    delete data;
    data=NULL;
    delete mcSmeared;
    mcSmeared=NULL;
}



void PlotMeanHist(TString filename, TString energy="8TeV", TString lumi="", int rebin=0, TString myRegion=""){
  

  TH2F eventFraction("eventFraction", "", 10, 0, 9, 10, 0, 9);
  int index_max=0;
  double  region_array[20][20];

  TString pdfWeightIndex=filename;
  pdfWeightIndex.ReplaceAll("test/dato/22Jan2012-runDepMCAll_v3/loose/invMass_SC_regrCorrSemiParV5_pho/step9pdfWeight/","");
  pdfWeightIndex.ReplaceAll("/fitres/histos-scaleStep2smearing_9-Et_20-trigger-noPF.root","");
  std::ofstream f_out("tmp/stat-"+pdfWeightIndex+".dat");

  TFile f_in(filename, "read");
  if(f_in.IsZombie()){
    std::cerr << "File opening error: " << filename << std::endl;
    return;
  }
  
  TList *KeyList = f_in.GetListOfKeys();

  std::map<TString, hist_t > dataHist;
  std::map<TString, hist_t > mcHist;
  std::map<TString, hist_t > mcSmearedHist;
  

  std::cout << KeyList->GetEntries() << std::endl;
  for(int i =0; i <  KeyList->GetEntries(); i++){

    TH1F *h = (TH1F *)((TKey *)KeyList->At(i))->ReadObj(); 
    //    TString keyName = (KeyList->At(i))->GetName();
   
    TString keyName = h->GetName();
    
    TString type=keyName;
    type.Remove(0,type.Last('_')+1);

    TString region=keyName;
    region.Remove(region.Last('_'));
    //std::cout << type << "\t" << keyName << "\t" << region << std::endl;

    //    if(region.CompareTo("region_2_2")!=0) continue;
    //    std::cout << "hist region = " << region << std::endl;
    /*------------------------------ somma sugli istogrammi */
    std::cout << "Data Entries in " << keyName << "\t" << h->GetEntries() << std::endl;

    if(type.CompareTo("data")==0 || type.CompareTo("smeardata")==0){
      if(dataHist.find(region)==dataHist.end()){
 	hist_t hist_vec; // vector of bins of ONE histogram
 	AddHist(hist_vec, h); // add the content of the histogram h to the vector
	dataHist[region]=h;
      } //else 	AddHist(dataHist[region], h);
    }

    if(type.CompareTo("MC")==0){
      if(mcHist.find(region)==mcHist.end()){
	hist_t hist_vec;
	AddHist(hist_vec, h);
	mcHist[region]=h;
      } //else 	AddHist(mcHist[region], h);
    }

    if(type.CompareTo("smearMC")==0){
      if(mcSmearedHist.find(region)==mcSmearedHist.end()){
	hist_t hist_vec;
	AddHist(hist_vec, h);
	mcSmearedHist[region]=h;
      } //else 	AddHist(mcSmearedHist[region], h);
    }
  }


  /*------------------------------ Plotto */
  TCanvas *c = new TCanvas("c","c");

  //    TPaveText *pv = new TPaveText(0.7,0.7,1, 0.8);    
  TLegend *legend = new TLegend(0.7,0.8,0.95,0.92);
  legend->SetFillStyle(3001);
  legend->SetFillColor(1);
  legend->SetTextFont(22); // 132
  legend->SetTextSize(0.04); // l'ho preso mettendo i punti con l'editor e poi ho ricavato il valore con il metodo GetTextSize()
  //  legend->SetFillColor(0); // colore di riempimento bianco
  legend->SetMargin(0.4);  // percentuale della larghezza del simbolo
  //    SetLegendStyle(legend);

  TH1F *mc_all[3]={NULL}, *data_all[3]={NULL}, *mcSmeared_all[3]={NULL};
  
  for(std::map<TString, hist_t>::const_iterator itr = dataHist.begin();
      itr!= dataHist.end(); itr++){
    c->cd();
    TString region=itr->first;
    if(myRegion!="" && !region.Contains(myRegion)) continue;
    //     for(int i_region=0; i_region < n_region -1; i_region++){
    //       for(int j_region=i_region; j_region < n_region; j_region++){
    // 	region="region_";
    // 	region+=i_region;
    // 	region+="_";
    // 	region+=j_region;
      
    //    f_in.ls();
    TH1F *h_ref = (TH1F *)((TKey *)KeyList->At(0))->ReadObj();
    TH1F *mc=GetMeanHist(mcHist[region], h_ref, "mc_"+region);
    //	std::cout << mc->GetEntries() << "\t" << mc->Integral() << std::endl;
    //	return;;
    TH1F *data=GetMeanHist(dataHist[region], h_ref, "data_"+region, true);	
    TH1F *mcSmeared=GetMeanHist(mcSmearedHist[region], h_ref, "mcSmeared_"+region);
    f_out << pdfWeightIndex 
	  << "\t" << data->GetName() << "\t" << data->GetTitle() 
	  << "\t" << data->GetEntries() << "\t" << data->Integral() 
	  << "\t" << data->GetMean() << "\t" << data->GetMeanError() 
	  << "\t" << data->GetRMS() << "\t" << data->GetRMSError() 
	  << std::endl;
//     if(region.Contains("EE")){
//       if(mc_all[1] == NULL) mc_all[1]=(TH1F *) mc->Clone("EE_mc_hist");
//       else mc_all[1]->Add(mc);
//       if(data_all[1] == NULL) data_all[1]=(TH1F *) data->Clone("EE_data_hist");
//       else data_all[1]->Add(data);
//       if(mcSmeared_all[1] == NULL) mcSmeared_all[1]=(TH1F *) mcSmeared->Clone("EE_mcSmeared_hist");
//       else mcSmeared_all[1]->Add(mcSmeared);
//     }

//     if(region.Contains("EB")){
//       if(mc_all[0] == NULL) mc_all[0]=(TH1F *) mc->Clone("EB_mc_hist");
//       else mc_all[0]->Add(mc);
//       if(data_all[0] == NULL) data_all[0]=(TH1F *) data->Clone("EB_data_hist");
//       else data_all[0]->Add(data);
//       if(mcSmeared_all[0] == NULL) mcSmeared_all[0]=(TH1F *) mcSmeared->Clone("EB_mcSmeared_hist");
//       else mcSmeared_all[0]->Add(mcSmeared);
//     }
    
//     if(mc_all[2] == NULL) mc_all[2]=(TH1F *) mc->Clone("all_mc_hist");
//     else mc_all[2]->Add(mc);
//     if(data_all[2] == NULL) data_all[2]=(TH1F *) data->Clone("all_data_hist");
//     else data_all[2]->Add(data);
//     if(mcSmeared_all[2] == NULL) mcSmeared_all[2]=(TH1F *) mcSmeared->Clone("all_mcSmeared_hist");
//     else mcSmeared_all[2]->Add(mcSmeared);
    
    if(rebin>0){
      data->Rebin(rebin);
      mc->Rebin(rebin);
      mcSmeared->Rebin(rebin);
    }
    if(data->Integral()!=0 && mc->Integral()!=0 && mcSmeared->Integral()!=0)
      Plot(c, data,mc,mcSmeared,legend, region, filename, energy, lumi);
  }
  
  //    Plot(c, data_all[0],mc_all[0],mcSmeared_all[0],legend, "EBinclusive", filename, energy, lumi);
  //  Plot(c, data_all[1],mc_all[1],mcSmeared_all[1],legend, "EEinclusive", filename, energy, lumi);
  //  Plot(c, data_all[2],mc_all[2],mcSmeared_all[2],legend, "Allinclusive", filename, energy, lumi);
  //    }
  delete c;
  f_in.Close();

//   TCanvas c2("eventFraction_cavas", "");
//   c2.cd();
//   gStyle->SetPaintTextFormat(".1f %%");
//   eventFraction.SetMarkerSize(2);
//   int n_region=(sqrt(8*dataHist.size()+1)-1)/2-1;
//   eventFraction.GetXaxis()->SetRangeUser(0,n_region);
//   eventFraction.GetYaxis()->SetRangeUser(0,n_region);  
//   eventFraction.GetXaxis()->CenterLabels(kTRUE);
//   eventFraction.GetYaxis()->CenterLabels(kTRUE);
//   eventFraction.GetXaxis()->SetNdivisions(n_region*2);
//   eventFraction.GetYaxis()->SetNdivisions(n_region*2);

//   int i;
//   for(i=0; i < eventFraction.GetNbinsY() && eventFraction.GetBinContent(0,i) > 0; i++){
//   }
//   std::cout << i << std::endl;
//   if(i < eventFraction.GetNbinsY()){
//     std::cout << "Reduction" << std::endl;
//         eventFraction.GetXaxis()->SetRangeUser(0,i);
// 	eventFraction.GetYaxis()->SetRangeUser(0,i);
//   }
  //std::cout << "Entries: " << eventFraction.Integral() << std::endl;;
  //eventFraction.Scale(100./eventFraction.Integral());
  //  eventFraction.Draw("text00");
  //  c2.SaveAs("eventFraction.eps");


//   double integral=0;
//   for(int i=0; i < index_max+1; i++){
//       for(int j=i; j < index_max+1; j++){
// 	std::cout << i << "\t" << j << "\t" << region_array[i][j] << std::endl;
// 	integral+=region_array[i][j];
//       }
//   }
//   std::cout << "Integral = " << integral << std::endl;

//   std::cout << "\\begin{pmatrix}" << std::endl;
//   for(int i=0; i < index_max+1; i++){
//       for(int j=0; j < index_max+1; j++){
// 	if(j<i) std::cout << "\t\t& ";
// 	else {
// 	  region_array[i][j]/=integral/100.; // in percentuale
// 	  //std::cout << i << "\t" << j << "\t" << region_array[i][j] << " %" << std::endl;
// 	  std::cout << std::setprecision(2) << region_array[i][j] << " \\%";
// 	  if(j!=index_max) std::cout << "\t& "; 
// 	}
//       }
//       std::cout << "\t\\\\" << std::endl;
//   }
//   std::cout << "\\end{pmatrix}" << std::endl;
  f_out.close();
  return;
}



