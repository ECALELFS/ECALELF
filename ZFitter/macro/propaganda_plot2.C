{
	TCut B_region_cut = "(((((((abs(etaEle[0]) >= 0)&&(abs(etaEle[0]) <  1))&&((abs(etaEle[1]) >= 0)&&(abs(etaEle[1]) <  1)))&&((((abs(etaEle[0]) < 1.4442)&&(abs(etaEle[1]) < 1.4442))&&(((abs(seedXSCEle[0]) < 26)||(abs(seedXSCEle[0]) > 25 && abs(seedXSCEle[0]) < 46))||(abs(seedXSCEle[0]) > 45 && abs(seedXSCEle[0]) < 66)))&&(((abs(seedXSCEle[1]) < 26)||(abs(seedXSCEle[1]) > 25 && abs(seedXSCEle[1]) < 46))||(abs(seedXSCEle[1]) > 45 && abs(seedXSCEle[1]) < 66))))&&(((((abs(seedXSCEle[0]) > 0 +5 && abs(seedXSCEle[0]) < 26-5)||(abs(seedXSCEle[0]) > 25+5 && abs(seedXSCEle[0]) < 46-5))||(abs(seedXSCEle[0]) > 45+5 && abs(seedXSCEle[0]) < 66-5))||(abs(seedXSCEle[0]) > 65+5 && abs(seedXSCEle[0]) < 86-5))&&(((seedYSCEle[0]-1) %20 >= 5)&&((seedYSCEle[0]-1) %20 < 20-5))))&&(((((abs(seedXSCEle[1]) > 0 +5 && abs(seedXSCEle[1]) < 26-5)||(abs(seedXSCEle[1]) > 25+5 && abs(seedXSCEle[1]) < 46-5))||(abs(seedXSCEle[1]) > 45+5 && abs(seedXSCEle[1]) < 66-5))||(abs(seedXSCEle[1]) > 65+5 && abs(seedXSCEle[1]) < 86-5))&&(((seedYSCEle[1]-1) %20 >= 5)&&((seedYSCEle[1]-1) %20 < 20-5))))&&((R9Ele[0] >= 0.94)&&(R9Ele[1] >= 0.94)))&&(recoFlagsEle[0] > 1 && recoFlagsEle[1] > 1)";

	gROOT->ProcessLine(".L macro/PlotDataMC.C+");
	gROOT->ProcessLine(".L src/setTDRStyle.C");
	//gROOT->ProcessLine(".L src/ElectronCategory_class.cc+");
	//ElectronCategory_class cutter;
	setTDRStyle();
	//  TGaxis::SetMaxDigits(3);

	//  TChain *signal = (TChain *) _file0->Get("selected");
	TChain *data   = (TChain *) _file1->Get("selected");
	//signal->GetEntries();
	data->GetEntries();
	//  _file0->cd();
	//  signal->Scan("smearEle:puWeight:invMassSigma_SC_regrCorr_pho");
	//_file1->cd();
	//  data->Scan("etaEle:smearEle:invMassSigma_SC_regrCorr_pho:corrEle");

	float canvas_scale = 1.2;
	TCanvas *c = new TCanvas("c", "c", 1, 1, 696 * canvas_scale, 548 * canvas_scale);
	c->cd();


	TChain *data_chain = data;

	float y_max_EB = 0;
	float y_max_EE = 0;

	TLegend legend(0.60, 0.65, 0.84, 0.94);
	legend.SetFillStyle(1001);
	legend.SetBorderSize(0);
	legend.SetTextFont(22); // 132
	legend.SetTextSize(0.031);
	legend.SetFillColor(0); // colore di riempimento bianco
	legend.SetMargin(0.26);  // percentuale della larghezza del simbolo

	TPaveText pv(0.2, 0.6, 0.52, 0.92, "ndc");

	pv.AddText("CMS Preliminary 2011");
	pv.AddText("#sqrt{s} = 7TeV, L = 4.98 fb^{-1}");
	//  pv.AddText("CMS Preliminary 2012");
	//  pv.AddText("#sqrt{s} = 8TeV, L = 19.6 fb^{-1}");

	pv.AddText("");
	pv.SetTextAlign(11);

	pv.SetFillStyle(1);
	pv.SetFillColor(0);
	pv.SetBorderSize(0);

	TPaveText pv_EB(pv);
	pv_EB.AddText("ECAL barrel");

	TPaveText pv_EE(pv);
	pv_EE.AddText("ECAL endcap");

	TCut EB_cut = GetCut("EB-eleID_7");
	TCut EE_cut = GetCut("EE-eleID_7");


	data_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EB(170,45,130)", EB_cut);
	TH1F *invMass_SC_regrCorr_ele_EB = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EB");
	data_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EE(170,45,130)", EE_cut);
	TH1F *invMass_SC_regrCorr_ele_EE = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EE");

	data_chain->Draw("invMass_e5x5>>invMass_e5x5_EB(120,60,120)", EB_cut);
	TH1F *invMass_e5x5_EB = (TH1F *) gROOT->FindObject("invMass_e5x5_EB");

	data_chain->Draw("invMass_e5x5>>invMass_e5x5_EE(120,60,120)", EE_cut);
	TH1F *invMass_e5x5_EE = (TH1F *) gROOT->FindObject("invMass_e5x5_EE");

	data_chain->Draw("invMass_rawSC>>invMass_rawSC_EB(160,50,130)", EB_cut);
	TH1F *invMass_rawSC_EB = (TH1F *) gROOT->FindObject("invMass_rawSC_EB");
	data_chain->Draw("invMass_rawSC>>invMass_rawSC_EE(160,50,130)", EE_cut);
	TH1F *invMass_rawSC_EE = (TH1F *) gROOT->FindObject("invMass_rawSC_EE");
	data_chain->Draw("invMass_rawSC_esSC>>invMass_rawSC_esSC_EE(160,50,130)", EE_cut);
	TH1F *invMass_rawSC_esSC_EE = (TH1F *) gROOT->FindObject("invMass_rawSC_esSC_EE");

	data_chain->Draw("invMass_SC>>invMass_SC_EB(160,50,130)", EB_cut);
	TH1F *invMass_SC_EB = (TH1F *) gROOT->FindObject("invMass_SC_EB");
	data_chain->Draw("invMass_SC>>invMass_SC_EE(160,50,130)", EE_cut);
	TH1F *invMass_SC_EE = (TH1F *) gROOT->FindObject("invMass_SC_EE");

	// ------------------------------ plot energy correction effect
	legend.Clear();
	legend.AddEntry(invMass_e5x5_EB, "E_{5x5 crystals}", "lf");
	legend.AddEntry(invMass_rawSC_EB, "E^{SuperCluster}_{uncorrected}", "lf");
	legend.AddEntry(invMass_SC_regrCorr_ele_EB, "E^{SuperCluster}_{corrected}", "l");
	legend.SetTextFont(42); // 132
	legend.SetTextSize(0.045);
	legend.SetBorderSize(1);

	y_max_EB = invMass_SC_regrCorr_ele_EB->GetMaximum() * 1.2;

	invMass_e5x5_EB->Draw();
	invMass_e5x5_EB->SetFillColor(kTeal - 8);
	invMass_e5x5_EB->SetFillStyle(3005);
	invMass_e5x5_EB->SetLineColor(kTeal - 8);
	invMass_e5x5_EB->SetLineStyle(0);
	invMass_e5x5_EB->SetLineWidth(3);
	invMass_e5x5_EB->SetMarkerStyle(20);
	invMass_e5x5_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_e5x5_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_e5x5_EB->GetYaxis()->SetTitleOffset(1.2);
	invMass_e5x5_EB->SetMaximum(y_max_EB);

	invMass_rawSC_EB->Draw("same");
	invMass_rawSC_EB->SetFillColor(kGreen - 6);
	invMass_rawSC_EB->SetFillStyle(3004);
	invMass_rawSC_EB->SetLineColor(kGreen - 6);
	invMass_rawSC_EB->SetLineStyle(0);
	invMass_rawSC_EB->SetLineWidth(2);
	invMass_rawSC_EB->SetMarkerStyle(20);
	invMass_rawSC_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_rawSC_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_rawSC_EB->GetYaxis()->SetTitleOffset(1.2);
	invMass_rawSC_EB->SetMaximum(y_max_EB);

	invMass_SC_regrCorr_ele_EB->Draw("same");
	invMass_SC_regrCorr_ele_EB->SetFillColor(1);
	invMass_SC_regrCorr_ele_EB->SetFillStyle(0);
	invMass_SC_regrCorr_ele_EB->SetLineColor(1);
	invMass_SC_regrCorr_ele_EB->SetLineStyle(0);
	invMass_SC_regrCorr_ele_EB->SetLineWidth(4);
	invMass_SC_regrCorr_ele_EB->SetMarkerStyle(20);
	invMass_SC_regrCorr_ele_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_SC_regrCorr_ele_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_SC_regrCorr_ele_EB->GetYaxis()->SetTitleOffset(1.2);

	legend.Draw();
	pv_EB.Draw();
	c->SaveAs("E_corrections-EB.eps");
	c->SaveAs("E_corrections-EB.C");
	c->SaveAs("E_corrections-EB.png");


	y_max_EE = invMass_SC_regrCorr_ele_EE->GetMaximum() * 1.2;

	invMass_e5x5_EE->Draw();
	invMass_e5x5_EE->SetFillColor(kTeal - 8);
	invMass_e5x5_EE->SetFillStyle(3005);
	invMass_e5x5_EE->SetLineColor(kTeal - 8); //38
	invMass_e5x5_EE->SetLineStyle(0);
	invMass_e5x5_EE->SetLineWidth(3);
	invMass_e5x5_EE->SetMarkerStyle(20);
	invMass_e5x5_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_e5x5_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_e5x5_EE->GetYaxis()->SetTitleOffset(1.2);
	invMass_e5x5_EE->SetMaximum(y_max_EE);

	invMass_rawSC_EE->Draw("same");
	invMass_rawSC_EE->SetFillColor(kGreen - 6);
	invMass_rawSC_EE->SetFillStyle(3004);
	invMass_rawSC_EE->SetLineColor(kGreen - 6); //46
	invMass_rawSC_EE->SetLineStyle(0);
	invMass_rawSC_EE->SetLineWidth(2);
	invMass_rawSC_EE->SetMarkerStyle(20);
	invMass_rawSC_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_rawSC_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_rawSC_EE->GetYaxis()->SetTitleOffset(1.2);
	invMass_rawSC_EE->SetMaximum(y_max_EE);

	invMass_rawSC_esSC_EE->Draw("same");
	invMass_rawSC_esSC_EE->SetFillColor(kOrange - 3);
	invMass_rawSC_esSC_EE->SetFillStyle(0);
	invMass_rawSC_esSC_EE->SetLineColor(kOrange - 3); //56
	invMass_rawSC_esSC_EE->SetLineStyle(0);
	invMass_rawSC_esSC_EE->SetLineWidth(4);
	invMass_rawSC_esSC_EE->SetMarkerStyle(20);
	invMass_rawSC_esSC_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_rawSC_esSC_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_rawSC_esSC_EE->GetYaxis()->SetTitleOffset(1.2);
	invMass_rawSC_esSC_EE->SetMaximum(y_max_EE);

	invMass_SC_regrCorr_ele_EE->Draw("same");
	invMass_SC_regrCorr_ele_EE->SetFillColor(1);
	invMass_SC_regrCorr_ele_EE->SetFillStyle(0);
	invMass_SC_regrCorr_ele_EE->SetLineColor(1);
	invMass_SC_regrCorr_ele_EE->SetLineStyle(0);
	invMass_SC_regrCorr_ele_EE->SetLineWidth(4);
	invMass_SC_regrCorr_ele_EE->SetMarkerStyle(20);
	invMass_SC_regrCorr_ele_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
	invMass_SC_regrCorr_ele_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
	invMass_SC_regrCorr_ele_EE->GetYaxis()->SetTitleOffset(1.2);

	legend.Clear();
	legend.AddEntry(invMass_e5x5_EE, "E_{5x5 crystals}", "lf");
	legend.AddEntry(invMass_rawSC_EE, "E^{SuperCluster}_{uncorrected}", "lf");
	legend.AddEntry(invMass_rawSC_esSC_EE, "E^{SuperCluster+ES}_{uncorrected}", "l");
	legend.AddEntry(invMass_SC_regrCorr_ele_EE, "E^{SuperCluster+ES}_{corrected}", "l");

	legend.Draw();
	pv_EE.Draw();

//   c->SaveAs("propaganda_e5x5_vs_regrCorr_ele-EE"+extension+goldString+".eps");
//   c->SaveAs("propaganda_e5x5_vs_regrCorr_ele-EE"+extension+goldString+".C");
//   c->SaveAs("propaganda_e5x5_vs_regrCorr_ele-EE"+extension+goldString+".png");

	c->SaveAs("E_corrections-EE.eps");
	c->SaveAs("E_corrections-EE.C");
	c->SaveAs("E_corrections-EE.png");

}



float canvas_scale = 1.2;
//TCanvas *c = new TCanvas("c", "c",1,1,696*canvas_scale,548*canvas_scale);
//c->cd();



TString selectedTreeDir = "/u2/shervin/Higgs/selected/";
//  TString selectedTreeDir="root://eoscms//cms/store/caf/user/shervin/Calibration/7TeV/Higgs/selected/";
//--------------- prendo il file con gli eventi selezionati
// mi serve una chain per i dati, una per il segnale MC ed una per i fondi
TChain *data_chain = new TChain("selected", "data");
data_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT/DoubleElectron-ZSkim-RUN2012A-13Jul-v1/190456-193621/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012A-13Jul-v1-190456-193621.root");
data_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT/DoubleElectron-ZSkim-RUN2012B-13Jul-v1/193834-196531/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012B-13Jul-v1-193834-196531.root");
data_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT/DoubleElectron-ZSkim-RUN2012C-v2/198111-198913/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v2-198111-198913.root");
data_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT/DoubleElectron-ZSkim-RUN2012C-v3/198934-203755/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v3-198934-203755.root");
data_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Feb2013_ICcomb_v7/DoubleElectron-ZSkim-RUN2012D-v1/203773-209465/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012D-v1-203773-209465.root");
data_chain->GetEntries();

TChain *data_noLaser_chain = new TChain("selected", "data noLaser");
data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012A-13Jul-v1/190456-193621/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012A-13Jul-v1-190456-193621.root");
data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012B-13Jul-v1/193834-196531/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012B-13Jul-v1-193834-196531.root");
data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012C-v2/198111-198913/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v2-198111-198913.root");
data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012C-v3/198934-203755/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v3-198934-203755.root");
data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Feb2013_ICcomb_v7_noLas/DoubleElectron-ZSkim-RUN2012D-v1/203773-209465/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012D-v1-203773-209465.root");
data_noLaser_chain->GetEntries();

//   // data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012A-13Jul-v1/190456-193621/190456-208686-13Julv2_Prompt-Moriond2013-Moriond2013/DoubleElectron-ZSkim-RUN2012A-13Jul-v1-190456-193621.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012B-13Jul-v1/193834-196531/190456-208686-13Julv2_Prompt-Moriond2013-Moriond2013/DoubleElectron-ZSkim-RUN2012B-13Jul-v1-193834-196531.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012C-v2/198111-198913/190456-208686-13Julv2_Prompt-Moriond2013-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v2-198111-198913.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012C-v3/198934-203755/190456-208686-13Julv2_Prompt-Moriond2013-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v3-198934-203755.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Feb2013_ICcomb_v7/DoubleElectron-ZSkim-RUN2012D-v1/203773-209465/190456-208686-13Julv2_Prompt-Moriond2013-Moriond2013/DoubleElectron-ZSkim-RUN2012D-v1-203773-209465.root");

// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuple/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012A-13Jul-v1/190456-193621/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012A-13Jul-v1-190456-193621.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuple/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012B-13Jul-v1/193834-196531/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012B-13Jul-v1-193834-196531.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuple/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012C-v2/198111-198913/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v2-198111-198913.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuple/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas/DoubleElectron-ZSkim-RUN2012C-v3/198934-203755/190456-208686-13Julv2_Prompt-Moriond2013/DoubleElectron-ZSkim-RUN2012C-v3-198934-203755.root");
// //   data_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuple/ALCARERECO/Cal_Feb2013_ICcomb_v7_noLas/DoubleElectron-ZSkim-RUN2012D-v1/203773-209465/190456-208686-13Julv2_Prompt/DoubleElectron-ZSkim-RUN2012D-v1-203773-209465.root");

TChain *data_noIC_noLaser_chain = new TChain("selected", "data noIC noLaser");
data_noIC_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas_noIC/DoubleElectron-ZSkim-RUN2012A-13Jul-v1/190456-193621/190456-208686-13Julv2_Prompt-Moriond2013_noInvMass/DoubleElectron-ZSkim-RUN2012A-13Jul-v1-190456-193621.root");
data_noIC_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas_noIC/DoubleElectron-ZSkim-RUN2012B-13Jul-v1/193834-196531/190456-208686-13Julv2_Prompt-Moriond2013_noInvMass/DoubleElectron-ZSkim-RUN2012B-13Jul-v1-193834-196531.root");
data_noIC_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas_noIC/DoubleElectron-ZSkim-RUN2012C-v2/198111-198913/190456-208686-13Julv2_Prompt-Moriond2013_noInvMass/DoubleElectron-ZSkim-RUN2012C-v2-198111-198913.root");
data_noIC_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Dic2012_ICcombTCcomb_v5_offGT_noLas_noIC/DoubleElectron-ZSkim-RUN2012C-v3/198934-203755/190456-208686-13Julv2_Prompt-Moriond2013_noInvMass/DoubleElectron-ZSkim-RUN2012C-v3-198934-203755.root");
data_noIC_noLaser_chain->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARERECO/Cal_Feb2013_ICcomb_v7_noLas_noIC/DoubleElectron-ZSkim-RUN2012D-v1/203773-209465/190456-208686-13Julv2_Prompt-Moriond2013_noInvMass/DoubleElectron-ZSkim-RUN2012D-v1-203773-209465.root");
data_noIC_noLaser_chain->GetEntries();

//  data_chain -> Add(selectedTreeDir+"selected-skimZ-rerecoPrompt_V1-RUN2011B-LasDataTest-Z_WP80_PU.root");

TString extension = "";
TString selection = "-Z_WP80_PU";

// last rereco && old Josh regression
//       extension="-FT_R_42_V21B";
//       data_chain -> Add(selectedTreeDir+"selected-skimZ-rereco10May-RUN2011A"+extension+"-Z_WP80_PU.root");
//       data_chain -> Add(selectedTreeDir+"selected-skimZ-rerecoPrompt_V4-RUN2011A"+extension+"-Z_WP80_PU.root");
//       data_chain -> Add(selectedTreeDir+"selected-skimZ-rerecoPrompt_V5-RUN2011A"+extension+"-Z_WP80_PU.root");
//       data_chain -> Add(selectedTreeDir+"selected-skimZ-rerecoPrompt_V6-RUN2011A"+extension+"-Z_WP80_PU.root");
//       data_chain -> Add(selectedTreeDir+"selected-skimZ-rerecoPrompt_V1-RUN2011B"+extension+"-Z_WP80_PU.root");


// last rereco && new Josh regression
//  extension="-NewRegr";
//data_chain ->  Add(selectedTreeDir+"selected-skimZ-rereco30Nov-RUN2011A"+extension+"-Z_WP80_PU.root");
///data_chain ->  Add(selectedTreeDir+"selected-skimZ-rereco30Nov-RUN2011B"+extension+"-Z_WP80_PU.root");

TCut selection_cut = "eleID[0] >=7 && eleID[1] >=7 && PtEle[0]>20 && PtEle[1]>20";


#ifdef GOLD
EB_cut += gold_cut;
EE_cut += gold_cut;
goldString = "-gold";
#endif


TLegend legend(0.60, 0.65, 0.84, 0.94);
legend.SetFillStyle(1001);
legend.SetBorderSize(0);
legend.SetTextFont(22); // 132
legend.SetTextSize(0.031);
legend.SetFillColor(0); // colore di riempimento bianco
legend.SetMargin(0.26);  // percentuale della larghezza del simbolo

TPaveText pv(0.2, 0.6, 0.52, 0.92, "ndc");

pv.AddText("CMS Preliminary 2012");
pv.AddText("#sqrt{s} = 8TeV, L = 19.6 fb^{-1}");
pv.AddText("");
pv.SetTextAlign(11);

pv.SetFillStyle(1);
pv.SetFillColor(0);
pv.SetBorderSize(0);

TPaveText pv_EB(pv);
pv_EB.AddText("ECAL barrel");

TPaveText pv_EE(pv);
pv_EE.AddText("ECAL endcap");



data_noIC_noLaser_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EB_noIC_noLaser(170,45,130)", EB_cut);
TH1F *invMass_SC_regrCorr_ele_noIC_noLaser_EB = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EB_noIC_noLaser");
data_noIC_noLaser_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EE_noIC_noLaser(170,45,130)", EE_cut);
TH1F *invMass_SC_regrCorr_ele_noIC_noLaser_EE = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EE_noIC_noLaser");

data_noLaser_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EB_noLaser(170,45,130)", EB_cut);
TH1F *invMass_SC_regrCorr_ele_noLaser_EB = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EB_noLaser");
data_noLaser_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EE_noLaser(170,45,130)", EE_cut);
TH1F *invMass_SC_regrCorr_ele_noLaser_EE = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EE_noLaser");


//------------------------------ REGRESSION
legend.Clear();
//   legend.AddEntry(invMass_SC_regrCorr_ele_noIC_noLaser_EB, "E^{regr}_{SC} noIC noLaser");
//   legend.AddEntry(invMass_SC_regrCorr_ele_noLaser_EB, "E^{regr}_{SC} noLaser");
//   legend.AddEntry(invMass_SC_regrCorr_ele_EB, "E^{regr}_{SC}");
legend.AddEntry(invMass_SC_regrCorr_ele_noIC_noLaser_EB, "no corrections", "lf");
legend.AddEntry(invMass_SC_regrCorr_ele_noLaser_EB, "Intercalibrations (IC)", "lf");
legend.AddEntry(invMass_SC_regrCorr_ele_EB, "IC + LM corrections", "l");


//invMass_SC_regrCorr_ele_EB->Sumw2();
//  invMass_SC_regrCorr_ele_EB->Scale(1./invMass_SC_regrCorr_ele_EB->Integral());
y_max_EB = invMass_SC_regrCorr_ele_EB->GetMaximum() * 1.2;
y_scale = invMass_SC_regrCorr_ele_EB->Integral(); // normalizzo allo stesso numero di eventi

invMass_SC_regrCorr_ele_noIC_noLaser_EB->Scale(y_scale / invMass_SC_regrCorr_ele_noIC_noLaser_EB->Integral());
invMass_SC_regrCorr_ele_noIC_noLaser_EB->Draw();
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetFillColor(38);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetFillStyle(3004);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetLineColor(38);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetLineStyle(0);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetLineWidth(3);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_noIC_noLaser_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_noIC_noLaser_EB->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_regrCorr_ele_noIC_noLaser_EB->SetMaximum(y_max_EB);


invMass_SC_regrCorr_ele_noLaser_EB->Scale(y_scale / invMass_SC_regrCorr_ele_noLaser_EB->Integral());
invMass_SC_regrCorr_ele_noLaser_EB->Draw("same");
invMass_SC_regrCorr_ele_noLaser_EB->SetFillColor(46);
invMass_SC_regrCorr_ele_noLaser_EB->SetFillStyle(3005);
invMass_SC_regrCorr_ele_noLaser_EB->SetLineColor(46);
invMass_SC_regrCorr_ele_noLaser_EB->SetLineStyle(0);
invMass_SC_regrCorr_ele_noLaser_EB->SetLineWidth(2);
invMass_SC_regrCorr_ele_noLaser_EB->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_noLaser_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_noLaser_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_noLaser_EB->GetYaxis()->SetTitleOffset(1.2);
//invMass_SC_regrCorr_ele_noLaser_EB->SetMaximum(y_max_EB);


invMass_SC_regrCorr_ele_EB->Draw("same");
invMass_SC_regrCorr_ele_EB->SetFillColor(1);
invMass_SC_regrCorr_ele_EB->SetFillStyle(0);
invMass_SC_regrCorr_ele_EB->SetLineColor(1);
invMass_SC_regrCorr_ele_EB->SetLineStyle(0);
invMass_SC_regrCorr_ele_EB->SetLineWidth(4);
invMass_SC_regrCorr_ele_EB->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_EB->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_regrCorr_ele_EB->GetYaxis()->SetRangeUser(0, y_max_EB);


legend.Draw();
pv_EB.Draw();
c->SaveAs("propaganda_noIC_noLaser-regrCorr_ele-EB" + extension + goldString + ".eps");
c->SaveAs("propaganda_noIC_noLaser-regrCorr_ele-EB" + extension + goldString + ".C");
c->SaveAs("propaganda_noIC_noLaser-regrCorr_ele-EB" + extension + goldString + ".png");


//   legend.Draw();
//   pv.Draw();
//   c->SaveAs("propaganda_noIC_noLaser-EB.eps");
//   c->SaveAs("propaganda_noIC_noLaser-EB.C");
//   c->SaveAs("propaganda_noIC_noLaser-EB.png");

//   legend.Clear();
//   legend.AddEntry(invMass_SC_regrCorr_ele_noIC_noLaser_EB, "E_{SC} noIC noLaser");
//   legend.AddEntry(invMass_SC_regrCorr_ele_noLaser_EB, "E_{SC} noLaser");
//   legend.AddEntry(invMass_SC_regrCorr_ele_EB, "E_{SC}");

//invMass_SC_regrCorr_ele_EE->Sumw2();
y_max_EE = invMass_SC_regrCorr_ele_EE->GetMaximum() * 1.2;
y_scale = invMass_SC_regrCorr_ele_EE->Integral(); // normalizzo allo stesso numero di eventi


invMass_SC_regrCorr_ele_noIC_noLaser_EE->Scale(y_scale / invMass_SC_regrCorr_ele_noIC_noLaser_EE->Integral());
invMass_SC_regrCorr_ele_noIC_noLaser_EE->Draw();
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetFillColor(38);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetFillStyle(3004);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetLineColor(38);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetLineStyle(0);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetLineWidth(3);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_noIC_noLaser_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_noIC_noLaser_EE->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_regrCorr_ele_noIC_noLaser_EE->SetMaximum(y_max_EE);

invMass_SC_regrCorr_ele_noLaser_EE->Scale(y_scale / invMass_SC_regrCorr_ele_noLaser_EE->Integral());
invMass_SC_regrCorr_ele_noLaser_EE->Draw("same");
invMass_SC_regrCorr_ele_noLaser_EE->SetFillColor(46);
invMass_SC_regrCorr_ele_noLaser_EE->SetFillStyle(3005);
invMass_SC_regrCorr_ele_noLaser_EE->SetLineColor(46);
invMass_SC_regrCorr_ele_noLaser_EE->SetLineStyle(0);
invMass_SC_regrCorr_ele_noLaser_EE->SetLineWidth(2);
invMass_SC_regrCorr_ele_noLaser_EE->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_noLaser_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_noLaser_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_noLaser_EE->GetYaxis()->SetTitleOffset(1.2);
//invMass_SC_regrCorr_ele_noLaser_EE->SetMaximum(y_max_EE);

invMass_SC_regrCorr_ele_EE->Draw("same");
invMass_SC_regrCorr_ele_EE->SetFillColor(1);
invMass_SC_regrCorr_ele_EE->SetFillStyle(0);
invMass_SC_regrCorr_ele_EE->SetLineColor(1);
invMass_SC_regrCorr_ele_EE->SetLineStyle(0);
invMass_SC_regrCorr_ele_EE->SetLineWidth(4);
invMass_SC_regrCorr_ele_EE->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_EE->GetYaxis()->SetTitleOffset(1.2);



legend.Draw();
pv_EE.Draw();
c->SaveAs("propaganda_noIC_noLaser-regrCorr_ele-EE" + extension + goldString + ".eps");
c->SaveAs("propaganda_noIC_noLaser-regrCorr_ele-EE" + extension + goldString + ".C");
c->SaveAs("propaganda_noIC_noLaser-regrCorr_ele-EE" + extension + goldString + ".png");


}
//------------------------------

invMass_SC_EB->Draw();
invMass_SC_EB->SetFillColor(38);
invMass_SC_EB->SetFillStyle(3004);
invMass_SC_EB->SetLineColor(38);
invMass_SC_EB->SetLineStyle(0);
invMass_SC_EB->SetLineWidth(3);
invMass_SC_EB->SetMarkerStyle(20);
invMass_SC_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_EB->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_EB->SetMaximum(y_max_EB);
invMass_SC_EB->GetXaxis()->SetRangeUser(75, 105);

invMass_SC_regrCorr_ele_EB->Draw("same");
invMass_SC_regrCorr_ele_EB->SetFillColor(1);
invMass_SC_regrCorr_ele_EB->SetFillStyle(0);
invMass_SC_regrCorr_ele_EB->SetLineColor(1);
invMass_SC_regrCorr_ele_EB->SetLineStyle(0);
invMass_SC_regrCorr_ele_EB->SetLineWidth(4);
invMass_SC_regrCorr_ele_EB->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_EB->GetYaxis()->SetTitleOffset(1.2);

legend.Clear();
legend.AddEntry(invMass_SC_EB, "E^{SC}");
legend.AddEntry(invMass_SC_regrCorr_ele_EB, "E^{SC}_{regr}", "l");

legend.Draw();
pv_EB.Draw();

//   c->SaveAs("propaganda_SC_vs_regrCorr_ele-EB"+extension+goldString+".eps");
//   c->SaveAs("propaganda_SC_vs_regrCorr_ele-EB"+extension+goldString+".C");
//   c->SaveAs("propaganda_SC_vs_regrCorr_ele-EB"+extension+goldString+".png");

c->SaveAs("SC_vs_regression-EB" + extension + goldString + "-v2.eps");
c->SaveAs("SC_vs_regression-EB" + extension + goldString + "-v2.C");
c->SaveAs("SC_vs_regression-EB" + extension + goldString + "-v2.png");

invMass_SC_EE->Draw();
invMass_SC_EE->SetFillColor(38);
invMass_SC_EE->SetFillStyle(3004);
invMass_SC_EE->SetLineColor(38);
invMass_SC_EE->SetLineStyle(0);
invMass_SC_EE->SetLineWidth(3);
invMass_SC_EE->SetMarkerStyle(20);
invMass_SC_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_EE->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_EE->SetMaximum(y_max_EE);
invMass_SC_EE->GetXaxis()->SetRangeUser(75, 105);

invMass_SC_regrCorr_ele_EE->Draw("same");
invMass_SC_regrCorr_ele_EE->SetFillColor(1);
invMass_SC_regrCorr_ele_EE->SetFillStyle(0);
invMass_SC_regrCorr_ele_EE->SetLineColor(1);
invMass_SC_regrCorr_ele_EE->SetLineStyle(0);
invMass_SC_regrCorr_ele_EE->SetLineWidth(4);
invMass_SC_regrCorr_ele_EE->SetMarkerStyle(20);
invMass_SC_regrCorr_ele_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_regrCorr_ele_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_regrCorr_ele_EE->GetYaxis()->SetTitleOffset(1.2);

legend.Draw();
pv_EE.Draw();

c->SaveAs("SC_vs_regression-EE" + extension + goldString + "-v2.eps");
c->SaveAs("SC_vs_regression-EE" + extension + goldString + "-v2.C");
c->SaveAs("SC_vs_regression-EE" + extension + goldString + "-v2.png");

//   c->SaveAs("propaganda_SC_vs_regrCorr_ele-EE"+extension+goldString+".eps");
//   c->SaveAs("propaganda_SC_vs_regrCorr_ele-EE"+extension+goldString+".C");
//   c->SaveAs("propaganda_SC_vs_regrCorr_ele-EE"+extension+goldString+".png");






//  data_chain -> Add(selectedTreeDir+"selected-skimZ-rerecoPrompt_V1-RUN2011B-LasDataTest-Z_WP80_PU.root");


//   data_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EB(160,50,130)", EB_cut);
//   TH1F *invMass_SC_regrCorr_ele_EB = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EB");
//   data_chain->Draw("invMass_SC_regrCorr_ele>>invMass_SC_regrCorr_ele_EE(160,50,130)", EE_cut);
//   TH1F *invMass_SC_regrCorr_ele_EE = (TH1F *) gROOT->FindObject("invMass_SC_regrCorr_ele_EE");


//   data_chain->Draw("invMass_SC>>invMass_SC_EB(160,50,130)", EB_cut);
//   TH1F *invMass_SC_EB = (TH1F *) gROOT->FindObject("invMass_SC_EB");
//   data_chain->Draw("invMass_SC>>invMass_SC_EE(160,50,130)", EE_cut);
//   TH1F *invMass_SC_EE = (TH1F *) gROOT->FindObject("invMass_SC_EE");

data_noIC_noLaser_chain->Draw("invMass_SC>>invMass_SC_EB_noIC_noLaser(160,50,130)", EB_cut);
TH1F *invMass_SC_noIC_noLaser_EB = (TH1F *) gROOT->FindObject("invMass_SC_EB_noIC_noLaser");
data_noIC_noLaser_chain->Draw("invMass_SC>>invMass_SC_EE_noIC_noLaser(160,50,130)", EE_cut);
TH1F *invMass_SC_noIC_noLaser_EE = (TH1F *) gROOT->FindObject("invMass_SC_EE_noIC_noLaser");

data_noLaser_chain->Draw("invMass_SC>>invMass_SC_EB_noLaser(160,50,130)", EB_cut);
TH1F *invMass_SC_noLaser_EB = (TH1F *) gROOT->FindObject("invMass_SC_EB_noLaser");
data_noLaser_chain->Draw("invMass_SC>>invMass_SC_EE_noLaser(160,50,130)", EE_cut);
TH1F *invMass_SC_noLaser_EE = (TH1F *) gROOT->FindObject("invMass_SC_EE_noLaser");

legend.Clear();
legend.AddEntry(invMass_SC_noIC_noLaser_EB, "E_{SC} noIC noLaser");
legend.AddEntry(invMass_SC_noLaser_EB, "E_{SC} noLaser");
legend.AddEntry(invMass_SC_EB, "E_{SC}");


std::cout << "Normalization factor: " << invMass_SC_EB->Integral() << std::endl;

y_max_EB = invMass_SC_EB->GetMaximum() * 1.2; // prima di scalare perche' tanto lo riscalo
float y_scale = invMass_SC_EB->Integral();
invMass_SC_EB->Scale(1. / y_scale);



invMass_SC_noIC_noLaser_EB->Scale(1. / invMass_SC_noIC_noLaser_EB->Integral());
invMass_SC_noIC_noLaser_EB->Draw();
invMass_SC_noIC_noLaser_EB->SetFillColor(38);
invMass_SC_noIC_noLaser_EB->SetFillStyle(3004);
invMass_SC_noIC_noLaser_EB->SetLineColor(38);
invMass_SC_noIC_noLaser_EB->SetLineStyle(0);
invMass_SC_noIC_noLaser_EB->SetLineWidth(3);
invMass_SC_noIC_noLaser_EB->SetMarkerStyle(20);
invMass_SC_noIC_noLaser_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_noIC_noLaser_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_noIC_noLaser_EB->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_noIC_noLaser_EB->SetMaximum(y_max_EB);
invMass_SC_noIC_noLaser_EB->Scale(y_scale);

invMass_SC_noLaser_EB->Scale(1. / invMass_SC_noLaser_EB->Integral());
invMass_SC_noLaser_EB->Draw("same");
invMass_SC_noLaser_EB->SetFillColor(46);
invMass_SC_noLaser_EB->SetFillStyle(3005);
invMass_SC_noLaser_EB->SetLineColor(46);
invMass_SC_noLaser_EB->SetLineStyle(0);
invMass_SC_noLaser_EB->SetLineWidth(2);
invMass_SC_noLaser_EB->SetMarkerStyle(20);
invMass_SC_noLaser_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_noLaser_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_noLaser_EB->GetYaxis()->SetTitleOffset(1.2);
//invMass_SC_noLaser_EB->SetMaximum(y_max_EB);
invMass_SC_noLaser_EB->Scale(y_scale);

invMass_SC_EB->Draw("same");
invMass_SC_EB->SetFillColor(1);
invMass_SC_EB->SetFillStyle(0);
invMass_SC_EB->SetLineColor(1);
invMass_SC_EB->SetLineStyle(0);
invMass_SC_EB->SetLineWidth(4);
invMass_SC_EB->SetMarkerStyle(20);
invMass_SC_EB->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_EB->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_EB->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_EB->GetYaxis()->SetRangeUser(0, y_max_EB);
invMass_SC_EB->Scale(y_scale);

legend.Draw();
pv_EB.Draw();
c->SaveAs("propaganda_noIC_noLaser-EB" + extension + goldString + ".eps");
c->SaveAs("propaganda_noIC_noLaser-EB" + extension + goldString + ".C");
c->SaveAs("propaganda_noIC_noLaser-EB" + extension + goldString + ".png");


//   legend.Draw();
//   pv.Draw();
//   c->SaveAs("propaganda_noIC_noLaser-EB"+extension+goldString+".eps");
//   c->SaveAs("propaganda_noIC_noLaser-EB"+extension+goldString+".C");
//   c->SaveAs("propaganda_noIC_noLaser-EB"+extension+goldString+".png");

legend.Clear();
legend.AddEntry(invMass_SC_noIC_noLaser_EB, "E_{SC} noIC noLaser");
legend.AddEntry(invMass_SC_noLaser_EB, "E_{SC} noLaser");
legend.AddEntry(invMass_SC_EB, "E_{SC}");

invMass_SC_EE->Scale(1. / invMass_SC_EE->Integral());
y_max_EE = invMass_SC_EE->GetMaximum() * 1.2;


invMass_SC_noIC_noLaser_EE->Scale(1. / invMass_SC_noIC_noLaser_EE->Integral());
invMass_SC_noIC_noLaser_EE->Draw();
invMass_SC_noIC_noLaser_EE->SetFillColor(38);
invMass_SC_noIC_noLaser_EE->SetFillStyle(3004);
invMass_SC_noIC_noLaser_EE->SetLineColor(38);
invMass_SC_noIC_noLaser_EE->SetLineStyle(0);
invMass_SC_noIC_noLaser_EE->SetLineWidth(3);
invMass_SC_noIC_noLaser_EE->SetMarkerStyle(20);
invMass_SC_noIC_noLaser_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_noIC_noLaser_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_noIC_noLaser_EE->GetYaxis()->SetTitleOffset(1.2);
invMass_SC_noIC_noLaser_EE->SetMaximum(y_max_EE);

invMass_SC_noLaser_EE->Scale(1. / invMass_SC_noLaser_EE->Integral());
invMass_SC_noLaser_EE->Draw("same");
invMass_SC_noLaser_EE->SetFillColor(46);
invMass_SC_noLaser_EE->SetFillStyle(3005);
invMass_SC_noLaser_EE->SetLineColor(46);
invMass_SC_noLaser_EE->SetLineStyle(0);
invMass_SC_noLaser_EE->SetLineWidth(2);
invMass_SC_noLaser_EE->SetMarkerStyle(20);
invMass_SC_noLaser_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_noLaser_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_noLaser_EE->GetYaxis()->SetTitleOffset(1.2);
//invMass_SC_noLaser_EE->SetMaximum(y_max_EE);

invMass_SC_EE->Draw("same");
invMass_SC_EE->SetFillColor(1);
invMass_SC_EE->SetFillStyle(0);
invMass_SC_EE->SetLineColor(1);
invMass_SC_EE->SetLineStyle(0);
invMass_SC_EE->SetLineWidth(4);
invMass_SC_EE->SetMarkerStyle(20);
invMass_SC_EE->GetXaxis()->SetTitle("M_{ee} (GeV/c^{2})");
invMass_SC_EE->GetYaxis()->SetTitle("Events / 0.5 GeV");
invMass_SC_EE->GetYaxis()->SetTitleOffset(1.2);



legend.Draw();
pv_EE.Draw();
c->SaveAs("propaganda_noIC_noLaser-EE" + extension + goldString + ".eps");
c->SaveAs("propaganda_noIC_noLaser-EE" + extension + goldString + ".C");
c->SaveAs("propaganda_noIC_noLaser-EE" + extension + goldString + ".png");




}

