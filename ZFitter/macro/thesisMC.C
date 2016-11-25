{
	std::vector<TString> mcLabel_vec;
	mcLabel_vec.push_back("Madgraph");
	mcLabel_vec.push_back("Powheg");
	mcLabel_vec.push_back("Sherpa");
	TString dataLabel = "Data";

	c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "nPV", "(40,0,40)", "eleID_loose-trigger-noPF-Et_25", "",
	dataLabel, mcLabel_vec, "nVtx", "", false, true);
	c->SaveAs(outputPath + "nPV.eps");
	delete c;
}

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "etaEle", "(100,-2.5,2.5)", "eleID_loose-trigger-noPF-Et_25", "", "", mcLabel_vec, "#eta", "", false, true, true,false,false,true); // use r9weight
//    c->SaveAs(outputPath+"etaEle-r9reweighted-Et_25.eps");
//    c->SaveAs(outputPath+"etaEle-r9reweighted-Et_25.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "R9Ele", "(100,0.7,1.2)", "EB-eleID_loose-trigger-noPF-Et_25", "", "", mcLabel_vec, "R_{9}", "", false, true, true,false,false,true);
//   c->SaveAs(outputPath+"R9Ele_EB-r9reweighted.eps");
//   c->SaveAs(outputPath+"R9Ele_EB-r9reweighted.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "R9Ele", "(110,0.3,1.4)", "EB-eleID_loose-trigger-noPF-Et_25", "", "", mcLabel_vec, "R_{9}", "",true,  false, true,false,false,true);
//   c->SaveAs(outputPath+"R9Ele_EB-r9reweighted-log.eps");
//   c->SaveAs(outputPath+"R9Ele_EB-r9reweighted-log.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "R9Ele", "(100,0.7,1.2)", "EE-eleID_loose-trigger-noPF-Et_25", "", "", mcLabel_vec, "R_{9}", "", false, true, true,false,false,true);
//   c->SaveAs(outputPath+"R9Ele_EE-r9reweighted.eps");
//   c->SaveAs(outputPath+"R9Ele_EE-r9reweighted.C");
//   delete c;
//    c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "R9Ele", "(110,0.3,1.4)", "EE-eleID_loose-trigger-noPF-Et_25", "", "", mcLabel_vec, "R_{9}", "",true, false, true,false,false,true);
//    c->SaveAs(outputPath+"R9Ele_EE-r9reweighted-log.eps");
//    c->SaveAs(outputPath+"R9Ele_EE-r9reweighted-log.C");
//    delete c;

// }
//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle/cosh(etaEle)", "(100,0,200)", "eleID_loose-trigger-noPF-EB", "", "", mcLabel_vec, "E_{T} SC [GeV]", "", false, true);
//   c->SaveAs(outputPath+"EtSCEle-EB.eps");
//   c->SaveAs(outputPath+"EtSCEle-EB.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle/cosh(etaEle)", "(100,0,160)", "eleID_loose-trigger-noPF-EE", "", "", mcLabel_vec, "E_{T} SC [GeV]", "", false, true);
//   c->SaveAs(outputPath+"EtSCEle-EE.eps");
//   c->SaveAs(outputPath+"EtSCEle-EE.C");
//   delete c;
// }

//   TFile *fout = new TFile(outputPath+"ErecEtrue_stdSC.root","recreate");
//   fout->cd();

//   TCanvas *c = new TCanvas("c","");
//   c->cd();
//   std::vector<TString> selections;
//   selections.push_back(TString("energyMCEle[0]>20 && energyMCEle[0]<25"));
//   selections.push_back(TString("energyMCEle[0]>25 && energyMCEle[0]<30"));
//   selections.push_back(TString("energyMCEle[0]>30 && energyMCEle[0]<35"));
//   selections.push_back(TString("energyMCEle[0]>35 && energyMCEle[0]<40"));
//   selections.push_back(TString("energyMCEle[0]>40 && energyMCEle[0]<45"));
//   selections.push_back(TString("energyMCEle[0]>45 && energyMCEle[0]<50"));
//   selections.push_back(TString("energyMCEle[0]>50 && energyMCEle[0]<55"));
//   selections.push_back(TString("energyMCEle[0]>55 && energyMCEle[0]<60"));
//   selections.push_back(TString("energyMCEle[0]>60 && energyMCEle[0]<65"));
//   selections.push_back(TString("energyMCEle[0]>65 && energyMCEle[0]<70"));
//   selections.push_back(TString("energyMCEle[0]>70 && energyMCEle[0]<75"));
//   selections.push_back(TString("energyMCEle[0]>75 && energyMCEle[0]<80"));
//   selections.push_back(TString("energyMCEle[0]>80 && energyMCEle[0]<85"));
//   selections.push_back(TString("energyMCEle[0]>85 && energyMCEle[0]<90"));
//   selections.push_back(TString("energyMCEle[0]>90 && energyMCEle[0]<95"));
//   selections.push_back(TString("energyMCEle[0]>95 && energyMCEle[0]<100"));

//   int index=0;
//   for(std::vector<TString>::const_iterator sel_itr = selections.begin();
//       sel_itr  != selections.end();
//       sel_itr++){
//     TString indexString; indexString+=index;
//     signalA->Draw("energySCEle[0]/energyMCEle[0]>>ErecEtrue_"+indexString+"_lead(200,0,2)",
// 		  GetCut("eleID_loose-trigger-noPF-Et_20-EB",1)+TCut(*sel_itr));
//     TH1F *h = gROOT->FindObject("ErecEtrue_"+indexString+"_lead");
//     h->Write();
//     signalA->Draw("energySCEle[1]/energyMCEle[1]>>ErecEtrue_"+indexString+"_sublead(200,0,2)",
// 		  GetCut("eleID_loose-trigger-noPF-Et_20-EB",2)+TCut(sel_itr->ReplaceAll("[0]","[1]")));
//     TH1F *h2 = gROOT->FindObject("ErecEtrue_"+indexString+"_sublead");
//     h2->Write();
//     std::cout << index
// 	      << "\t" << h->GetMean() << "\t" << h->GetRMS()
// 	      << "\t" << h2->GetMean() << "\t" << h2->GetRMS()
// 	      << std::endl;

//     index++;

//   }
//   fout->Close();

// }
//   TCanvas *c = new TCanvas("c","");
//   c->cd();

//   signalA->Draw("invMass_SC_regrCorrSemiParV8_ele>>invMassRegrV8Ele(120,60,120)",
// 		GetCut("eleID_loose-trigger-noPF-Et_20-EB"));
//   signalA->Draw("invMass_SC>>invMassSC(120,60,120)",
// 		GetCut("eleID_loose-trigger-noPF-Et_20-EB"));
//   invMassRegrV8Ele->Draw();
//   invMassSC       ->Draw("same");
//   c->SaveAs("invMass_regrComparison.C");

// }
//   TFile *fout = new TFile(outputPath+"fout.root","recreate");
//   fout->cd();

//   TCanvas *c = new TCanvas("c","");
//   c->cd();

//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele[0]:energyMCEle[0]>>ErecvsEtrue_EBhighEtaBad_lead(200,0,200,200,0,200)",
// 		GetCut("eleID_loose-trigger-noPF-Et_20-EB-absEta_1_1.4442-bad",1),"colz");
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele[0]:energyMCEle[0]>>ErecvsEtrue_EBhighEtaGold_lead(200,0,200,200, 0, 200)",
// 		GetCut("eleID_loose-trigger-noPF-Et_20-EB-absEta_1_1.4442-gold",1),"colz");
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele[0]:energyMCEle[0]>>ErecvsEtrue_EBlowEtaBad_lead(200,0,200,200, 0, 200)",
// 		GetCut("eleID_loose-trigger-noPF-Et_20-EB-absEta_0_1-bad",1),"colz");
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele[0]:energyMCEle[0]>>ErecvsEtrue_EBlowEtaGold_lead(200,0,200,200, 0, 200)",
// 		GetCut("eleID_loose-trigger-noPF-Et_20-EB-absEta_0_1-gold",1),"colz");


//   ErecvsEtrue_EBhighEtaBad_lead->Write();
//   ErecvsEtrue_EBhighEtaGold_lead->Write();
//   ErecvsEtrue_EBlowEtaBad_lead->Write();
//   ErecvsEtrue_EBlowEtaGold_lead->Write();
//   fout->Close();

// }
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele/energyMCEle:energyMCEle>>ErecEtruevsE_EBhighEtaGold(200,0,200,60,0.8,1.1)",
// 		GetCut("eleID_loose-trigger-noPF-Et_25-EB-absEta_1_1.4442-gold"),"profx");

//   ErecEtruevsE_EBhighEtaGold->Draw();
//   c->SaveAs(outputPath+"ErecEtrue_vs_E-EBhighEtaGold.C");
// }
// }
//   std::vector<TString> runRanges=ReadRegionsFromFile("data/runRanges/monitoring.dat");
//   for(std::vector<TString>::const_iterator runRanges_itr=runRanges.begin();
//       runRanges_itr != runRanges.end();
//       runRanges_itr++){
//     //std::cout << *runRanges_itr << std::endl;;
//     TString runRange=*runRanges_itr;
//     runRange.ReplaceAll("-","_");
//     runRange.Insert(0,"-runNumber_");

//     data->Draw("nPV>>nPVvsUnixTime(60,0,60)", GetCut("eleID_loose-trigger-noPF-Et_25"+runRange));
//     double mean = nPVvsUnixTime->GetMean();
//     double rms  = nPVvsUnixTime->GetRMS();
//     std::cout << runRange << *runRanges_itr << "\t" << mean << "\t" << rms << std::endl;

//   }

// }
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele/energyMCEle>>ErecEtrueEBhighEtaGold(60,0.8,1.1)", GetCut("eleID_loose-trigger-noPF-Et_25-EB-absEta_1_1.4442-gold"));
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele/energyMCEle>>ErecEtrueEBhighEtaBad(60,0.8,1.1)", GetCut("eleID_loose-trigger-noPF-Et_25-EB-absEta_1_1.4442-bad"));

//   ErecEtrueEBhighEtaBad->Draw();
//   ErecEtrueEBhighEtaGold->Draw("same");
//   ErecEtrueEBhighEtaBad->SetLineColor(6);
//   ErecEtrueEBhighEtaGold->SetLineColor(1);
//   ErecEtrueEBhighEtaBad->SetLineStyle(7);
//   c->SaveAs(outputPath+"ErecEtrue-EBhighEta.C");
// }
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele/energyMCEle:abs(etaEle)>>ErecEtruevsEtaGold( 100,0,2.5,100,0,2)", GetCut("eleID_loose-trigger-noPF-Et_25-gold"),"profx");
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele/energyMCEle:abs(etaEle)>>ErecEtruevsEtaBad( 100,0,2.5,100,0,2)", GetCut("eleID_loose-trigger-noPF-Et_25-bad"),"profx");
//   ErecEtruevsEtaGold->Draw("E");
//   ErecEtruevsEtaGold->GetXaxis()->SetTitle("|#eta|");
//   ErecEtruevsEtaGold->GetYaxis()->SetTitle("E_{rec}/E_{MC}");
//   ErecEtruevsEtaBad->Draw("Esame");
//   c->SaveAs(outputPath+"ErecEtrue_vs_eta.C");
// }
//   signalA->Draw("R9Ele[0]:abs(etaEle[0])>>R9vsEta( 100,0,2.5,120,0,1.2)", GetCut("eleID_loose-trigger-noPF-Et_25"),"profx");

//   R9vsEta->Draw("E");
//   R9vsEta->GetXaxis()->SetTitle("|#eta|");
//   R9vsEta->GetYaxis()->SetTitle("R_{9}");
//   c->SaveAs(outputPath+"R9_vs_eta.C");
// }
//   signalA->Draw("nHitsSCEle[0]:R9Ele[0]>>R9vsnHits(120,0,1.2, 100,0,100)", GetCut("eleID_loose-trigger-noPF-Et_25-EB-fiducial"),"colz");
//   //  signalA->Draw("R9Ele[0]:abs(etaEle[0])>>eCorr_stdSC_gold(100,0,2.5, 550,0.9,2)", GetCut("eleID_loose-trigger-noPF-Et_25-gold"),"profx");
//   R9vsnHits->Draw("colz");
//   R9vsnHits->GetXaxis()->SetTitle("R_{9}");
//   R9vsnHits->GetYaxis()->SetTitle("Number of crystals in SC");
//   R9vsnHits->GetXaxis()->SetRangeUser(0.4,1);
//   c->SaveAs(outputPath+"R9_vs_nHits-EB-colz");
//   c->SaveAs(outputPath+"R9_vs_nHits-EB-colz.C");

// }
//   TFile *fout = new TFile(outputPath+"fout.root","recreate");
//   fout->cd();

//   signalA->Draw("energySCEle[0]/(esEnergySCEle[0]+rawEnergySCEle[0]):abs(etaEle[0])>>eCorr_stdSC_gold(100,0,2.5, 550,0.9,2)", GetCut("eleID_loose-trigger-noPF-Et_25-gold"),"profx");
//   signalA->Draw("energySCEle[0]/(esEnergySCEle[0]+rawEnergySCEle[0]):abs(etaEle[0])>>eCorr_stdSC_bad(100,0,2.5, 550,0.9,2)", GetCut("eleID_loose-trigger-noPF-Et_25-bad"),"profx");
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele[0]/(esEnergySCEle[0]+rawEnergySCEle[0]):abs(etaEle[0])>>eCorr_regrEleV5_gold(100,0,2.5, 550,0.9,2)", GetCut("eleID_loose-trigger-noPF-Et_25-gold"),"profx");
//   signalA->Draw("energySCEle_regrCorrSemiParV5_ele[0]/(esEnergySCEle[0]+rawEnergySCEle[0]):abs(etaEle[0])>>eCorr_regrEleV5_bad(100,0,2.5, 550,0.9,2)", GetCut("eleID_loose-trigger-noPF-Et_25-bad"),"profx");

//   eCorr_stdSC_bad->SetMarkerStyle(20);
//   eCorr_stdSC_bad->SetMarkerSize(1.2);
//   eCorr_stdSC_bad->SetMarkerColor(kBlack);

//   eCorr_stdSC_gold->SetMarkerStyle(21);
//   eCorr_stdSC_gold->SetMarkerSize(1.2);
//   eCorr_stdSC_gold->SetMarkerColor(kRed);

//   eCorr_regrEleV5_gold->SetMarkerStyle(22);
//   eCorr_regrEleV5_gold->SetMarkerSize(1.2);
//   eCorr_regrEleV5_gold->SetMarkerColor(kBlue);

//   eCorr_regrEleV5_bad->SetMarkerStyle(22);
//   eCorr_regrEleV5_bad->SetMarkerSize(1.2);
//   eCorr_regrEleV5_bad->SetMarkerColor(kGreen);

//   eCorr_stdSC_gold->Write();
//   eCorr_stdSC_bad->Write();
//   eCorr_regrEleV5_gold->Write();
//   eCorr_regrEleV5_bad->Write();

//   TCanvas *c = new TCanvas("c","");
//   c->cd();
//   eCorr_stdSC_bad	->Draw("E");
//   eCorr_stdSC_gold	->Draw("Esame");
//   eCorr_regrEleV5_gold	->Draw("Esame");
//   eCorr_regrEleV5_bad	->Draw("Esame");
//   c->SaveAs(outputPath+"energyCorrection_stdSC_vs_eta.eps");
//   c->SaveAs(outputPath+"energyCorrection_stdSC_vs_eta.C");

//   fout->Close();

// }

// }
//   // kinematic variables
//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle", "(100,0,200)", "eleID_loose-trigger-noPF-EB", "", "", mcLabel_vec, "energy SC [GeV]", "", false, true);
//   c->SaveAs(outputPath+"energySCEle-EB.eps");
//   c->SaveAs(outputPath+"energySCEle-EB.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle", "(100,0,300)", "eleID_loose-trigger-noPF-EE", "", "", mcLabel_vec, "energy SC [GeV]", "", false, true);
//   c->SaveAs(outputPath+"energySCEle-EE.eps");
//   c->SaveAs(outputPath+"energySCEle-EE.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle/cosh(etaEle)", "(100,0,200)", "eleID_loose-trigger-noPF-EB", "", "", mcLabel_vec, "E_{T} SC [GeV]", "", false, true);
//   c->SaveAs(outputPath+"EtSCEle-EB.eps");
//   c->SaveAs(outputPath+"EtSCEle-EB.C");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle/cosh(etaEle)", "(100,0,160)", "eleID_loose-trigger-noPF-EE", "", "", mcLabel_vec, "E_{T} SC [GeV]", "", false, true);
//   c->SaveAs(outputPath+"EtSCEle-EE.eps");
//   c->SaveAs(outputPath+"EtSCEle-EE.C");
//   delete c;

// //   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle", "(100,0,200)", "eleID_loose-trigger-noPF-EB", "", "", mcLabel_vec, "energy SC [GeV]", "", true, true);
// //    c->SaveAs(outputPath+"energySCEle-EB-log.eps");
// //    delete c;
// //   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle", "(100,0,200)", "eleID_loose-trigger-noPF-EE", "", "", mcLabel_vec, "energy SC [GeV]", "", true, true);
// //    c->SaveAs(outputPath+"energySCEle-EE-log.eps");
// //    delete c;

// //   c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "energySCEle/etaEle", "(100,0,200)", "eleID_loose-trigger-noPF-EB", "", "", mcLabel_vec, "E_{T} SC [GeV]", "", true, true);
// //   c->SaveAs(outputPath+"EtSCEle-EB-log.eps");
// //   delete c;
// //   c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "energySCEle/etaEle", "(100,0,200)", "eleID_loose-trigger-noPF-EE", "", "", mcLabel_vec, "E_{T} SC [GeV]", "", true, true);
// //   c->SaveAs(outputPath+"EtSCEle-EE-log.eps");
// //   delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "etaEle", "(100,-2.5,2.5)", "eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "#eta", "", false, true);
c->SaveAs(outputPath + "etaEle-Et_25.eps");
c->SaveAs(outputPath + "etaEle-Et_25.C");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "phiEle", "(100,-5,5)", "eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "#phi", "", false, true);
c->SaveAs(outputPath + "phiEle-Et_25.C");
c->SaveAs(outputPath + "phiEle-Et_25.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "etaSCEle", "(100,-2.5,2.5)", "eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "#eta_{SC}", "", false, true);
c->SaveAs(outputPath + "etaSCEle-Et_25.eps");
c->SaveAs(outputPath + "etaSCEle-Et_25.C");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "phiSCEle", "(100,-5,5)", "eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "#phi_{SC}", "", false, true);
c->SaveAs(outputPath + "phiSCEle-Et_25.C");
c->SaveAs(outputPath + "phiSCEle-Et_25.eps");
delete c;
}
c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "R9Ele", "(100,0.7,1.2)", "EB-eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "R_{9}", "", false, true);
c->SaveAs(outputPath + "R9Ele_EB.eps");
c->SaveAs(outputPath + "R9Ele_EB.C");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "R9Ele", "(110,0.3,1.4)", "EB-eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "R_{9}", "", true,  false);
c->SaveAs(outputPath + "R9Ele_EB-log.eps");
c->SaveAs(outputPath + "R9Ele_EB-log.C");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "R9Ele", "(100,0.7,1.2)", "EE-eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "R_{9}", "", false, true);
c->SaveAs(outputPath + "R9Ele_EE.eps");
c->SaveAs(outputPath + "R9Ele_EE.C");
delete c;
c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "R9Ele", "(110,0.3,1.4)", "EE-eleID_loose-trigger-noPF-Et_25", "", dataLabel, mcLabel_vec, "R_{9}", "", true, false);
c->SaveAs(outputPath + "R9Ele_EE-log.eps");
c->SaveAs(outputPath + "R9Ele_EE-log.C");
delete c;


// pileup
c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "nPU", "(60,0,60)", "eleID_loose-trigger-noPF-Et_25", "",
                dataLabel, mcLabel_vec, "nPU", "", false, false);
c->SaveAs(outputPath + "nPU.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "nPV", "(40,0,40)", "eleID_loose-trigger-noPF-Et_25", "",
                dataLabel, mcLabel_vec, "nVtx", "", false, false);
c->SaveAs(outputPath + "nPV-noweight.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "nPV", "(40,0,40)", "eleID_loose-trigger-noPF-Et_25", "",
                dataLabel, mcLabel_vec, "nVtx", "", false, true);
c->SaveAs(outputPath + "nPV.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "rho", "(60,-1,29)", "eleID_loose-trigger-noPF-Et_25", "",
                dataLabel, mcLabel_vec, "#rho", "", false, true);
c->SaveAs(outputPath + "rho.eps");
delete c;

}

}
mcLabel_vec.clear();
mcLabel_vec.push_back("Simulation");
c = PlotDataMCs(data, MakeChainVector(signalB), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EB", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC_regrCorrSemiParV5_ele-EB.eps");
c->SaveAs(outputPath + "invMass_SC_regrCorrSemiParV5_ele-EB.C");
delete c;


c = PlotDataMCs(data, MakeChainVector(signalB), "invMass_SC_regrCorrSemiParV5_ele", "(200,80,100)", "eleID_loose-trigger-noPF-EE", "", "Data", mcLabel_vec, "M_{ee} [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC_regrCorrSemiParV5_ele-EE.eps");
c->SaveAs(outputPath + "invMass_SC_regrCorrSemiParV5_ele-EE.C");
delete c;


}
selected->Draw("R9Ele[0]*rawEnergySCEle[0]/energyMCEle[0]", "energyMCEle[0]!=0");
selected->Draw("e5x5SCEle[0]/energyMCEle[0]", "energyMCEle[0]!=0");


}

//TString outputPath="tmp/"; //test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011/WP80_PU/img/";
//TString outputPath="test/dato/moriond2013/WP80_PU/img/";
std::vector<TString> mcLabel_vec;
mcLabel_vec.push_back("Madgraph RD");
mcLabel_vec.push_back("Powheg RD");
mcLabel_vec.push_back("Sherpa RD");

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "invMass_SC", "(400,80,100)", "eleID_loose-trigger-noPF-EB", "", dataLabel, mcLabel_vec, "M_{ee} std SC [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC-EB.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "invMass_SC_regrCorr_ele", "(400,80,100)", "eleID_loose-trigger-noPF-EB", "", dataLabel, mcLabel_vec, "M_{ee} ele-tuned V3 [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC_regrCorr_ele-EB.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "invMass_SC", "(400,80,100)", "eleID_loose-trigger-noPF-EE", "", dataLabel, mcLabel_vec, "M_{ee} std SC [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC-EE.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "invMass_SC_regrCorr_ele", "(400,80,100)", "eleID_loose-trigger-noPF-EE", "", dataLabel, mcLabel_vec, "M_{ee} ele-tuned V3 [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC_regrCorr_ele-EE.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "invMass_SC_regrCorrSemiParV5_ele", "(400,80,100)", "eleID_loose-trigger-noPF-EB", "", dataLabel, mcLabel_vec, "M_{ee} ele-tuned V5 [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC_regrCorr_ele-EB.eps");
delete c;

c = PlotDataMCs(data, MakeChainVector(signalA, signalB, signalC), "invMass_SC_regrCorrSemiParV5_ele", "(400,80,100)", "eleID_loose-trigger-noPF-EE", "", dataLabel, mcLabel_vec, "M_{ee} ele-tuned V5 [GeV]", "", false, true);
c->SaveAs(outputPath + "invMass_SC_regrCorr_ele-EE.eps");
delete c;


//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_ele / invMass_SC_regrCorrSemiParV5_ele -1", "(250,-0.025,0.025)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "(ele-tuned) regrV4/regrV5 -1 ", "", false, true);
// }
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCEleSemiParV5-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_pho/invMass_SC_regrCorrSemiParV5_pho -1", "(250,-0.025,0.025)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "(pho-tuned) regrV4/regrV5 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCEleSemiParV5_pho-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_ele/invMass_SC_regrCorrSemiParV5_ele -1", "(250,-0.025,0.025)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "(ele-tuned) regrV4/regrV5 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCEleSemiParV5-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_pho/invMass_SC_regrCorrSemiParV5_pho -1", "(250,-0.025,0.025)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "(pho-tuned) regrV4/regrV5 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCEleSemiParV5_pho-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_ele/invMass_SC_regrCorr_ele -1", "(250,-0.025,0.025)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV4/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCEle-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV5_ele/invMass_SC_regrCorr_ele -1", "(250,-0.025,0.025)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV5/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV5_regrSCEle-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_pho/invMass_SC_regrCorr_pho -1", "(250,-0.025,0.025)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV4/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCPho-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV5_pho/invMass_SC_regrCorr_pho -1", "(250,-0.025,0.025)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV5/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV5_regrSCPho-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_ele/invMass_SC_regrCorr_ele -1", "(250,-0.025,0.025)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV4/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCEle-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV5_ele/invMass_SC_regrCorr_ele -1", "(250,-0.025,0.025)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV5/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV5_regrSCEle-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV4_pho/invMass_SC_regrCorr_pho -1", "(250,-0.025,0.025)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV4/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV4_regrSCPho-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB), "invMass_SC_regrCorrSemiParV5_pho/invMass_SC_regrCorr_pho -1", "(250,-0.025,0.025)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron regrV5/regrV3 -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEleSemiParV5_regrSCPho-EE.eps");
//   delete c;





//   // regression comparison
//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron (Hgg) regression/std. SC -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEle_stdSC-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Photon (Hgg) regression/std. SC -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCPho_stdSC-EB.eps");
//   delete c;


//   //
//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.2,0.2)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron (Egamma) regression/std. SC -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EB.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "invMass_SC_regrCorr_ele/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron (Hgg) regression/std. SC -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCEle_stdSC-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "invMass_SC_regrCorr_pho/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Photon (Hgg) regression/std. SC -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrSCPho_stdSC-EE.eps");
//   delete c;

//     c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "invMass_regrCorr_egamma/invMass_SC -1", "(500,-0.1,0.1)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Electron (Egamma) regression/std. SC -1 ", "", false, true);
//   c->SaveAs(outputPath+"/regrEleEgamma_stdSC-EE.eps");
//   delete c;


//   // cluster corrections and ES energy fraction
//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "esEnergySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0,0.2)", "absEta_1.7_2.5-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "ES energy fraction: ES/(rawSC+ES) ", "", false, true);
//   c->SaveAs(outputPath+"/esEnergyFraction.eps");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EB-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, true);
//   c->SaveAs(outputPath+"/energyCorrections_EB.eps");
//   delete c;

//   c = PlotDataMCs(data, MakeChainVector(signalA,signalB,signalC), "energySCEle/(rawEnergySCEle+esEnergySCEle)", "(100,0.99,1.15)", "EE-eleID_loose-trigger-noPF-Et_25","", dataLabel, mcLabel_vec, "Energy corrections F: E_{SC}/(E_{rawSC}+E_{ES}) ", "", false, true);
//   c->SaveAs(outputPath+"/energyCorrections_EE.eps");
//   delete c;


