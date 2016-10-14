#include <TFile.h>
#include <TF1.h>
#include <TList.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TKey.h>
#include <TObject.h>
#include <TString.h>
#include <TGraph.h>
#include <fstream> 

//gROOT->ProcessLine(".L src/setTDRStyle.cc");//Not working
//setTDRStyle();

Double_t asymmetricParabola(Double_t* x,Double_t* par);

void fitOneProfile(TString profile, TString outDirName="test/dato/img/TestProfiles"){
  std::ofstream ofs (outDirName+"/fitOneProfile.txt", std::ofstream::app);
  TFile f_in(profile);
  f_in.cd();

  TList *KeyList = f_in.GetListOfKeys();
  for(int i =0; i <  KeyList->GetEntries(); i++){
    TKey *currentKey=(TKey *)KeyList->At(i);
    TString className=currentKey->GetClassName();
    // se non e' un TGraph passa oltre
    if (! (className.CompareTo("TGraph")==0)) continue;
    TGraph *g = (TGraph *)currentKey->ReadObj(); 
    TCanvas *c =new TCanvas("likelihood profile");
    c->SetBottomMargin(0.15);
    c->SetLeftMargin(0.15);
    g->SetTitle(currentKey->GetName());
    g->Draw("AP*");
    g->GetYaxis()->SetTitleSize(0.045);
    g->GetYaxis()->SetTitle("NLL");
    g->GetXaxis()->SetTitleSize(0.045);

    if(((TString)currentKey->GetName()).Contains("constTerm")){
      g->GetXaxis()->SetTitle("#sigma");
      TF1* f=new TF1("f","pol2",0.00,0.015);
      g->Fit(f,"OFR+","",0.00,0.015);
      //TF1* fun=new TF1("fun",asymmetricParabola,0.98,1.02,4);
      //fun->SetParameter(0,0);
      //double minX=f->GetMinimumX();
      //fun->SetParameter(1,minX);
      double sigma=1./sqrt(2* f->GetParameter(2));
      //fun->SetParameter(2,1/(2*sigma*sigma));
      //fun->SetParameter(3,1/(2*sigma*sigma));
      //g->Fit(fun,"OFR+","",0.98,1.02);
      g->Draw("AP*"); 
      ofs<<"**** "<<g->GetTitle()<<endl;
      ofs<<"minimum is "<<f->GetMinimumX() <<" pm "<<sigma<<endl;

    }else if(((TString)currentKey->GetName()).Contains("scale")){
      g->GetXaxis()->SetTitle("scale");
      TF1* f=new TF1("f","pol2",0.98,1.03);
      g->Fit(f,"OFR+","",0.98,1.03);
      //TF1* fun=new TF1("fun",asymmetricParabola,0.98,1.02,4);
      //fun->SetParameter(0,0);
      //double minX=f->GetMinimumX();
      //fun->SetParameter(1,minX);
      double sigma=1./sqrt(2* f->GetParameter(2));
      //fun->SetParameter(2,1/(2*sigma*sigma));
      //fun->SetParameter(3,1/(2*sigma*sigma));
      //g->Fit(fun,"OFR+","",0.98,1.02);
      g->GetXaxis()->SetRangeUser(0.965,1.5);
      g->Draw("AP*"); 
      ofs<<"**** "<<g->GetTitle()<<endl;
      ofs<<"minimum is "<<f->GetMinimumX() <<" pm "<<sigma<<endl;
    }

    //ofs.close();
    TString out_name=(TString)currentKey->GetName();
    out_name.ReplaceAll("profileChi2_","");
    std::cout<<out_name<<std::endl;
    std::cout<<"Saving plots"<<std::endl;
    c->SaveAs(outDirName+"/"+out_name+".png");

    if(profile.Contains("ptRatio_leading")){
      c->SaveAs(outDirName+"/"+out_name+"_ptRatio_leading.png");
    }else if(profile.Contains("ptRatio_random")){
      c->SaveAs(outDirName+"/"+out_name+"_ptRatio_random.png");
    }else if(profile.Contains("ptSum_leading")){
      c->SaveAs(outDirName+"/"+out_name+"_ptSum.png");
    }

  }
  
  f_in.Close();
  
}

Double_t asymmetricParabola(Double_t* x,Double_t* par)
{
  Double_t value=0.;
  if (x[0]<=par[1])
    value=par[2]*(x[0]-par[1])*(x[0]-par[1]);
  else
    value=par[3]*(x[0]-par[1])*(x[0]-par[1]);
  return value+par[0];
}
