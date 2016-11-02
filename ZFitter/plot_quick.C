{
ElectronCategory_class j
TString ELEID_LOOSE=j.GetCut("eleID_cutBasedElectronID_Spring15_25ns_V1_standalone_loose",false,1,false).GetTitle();
TString ELEID_MEDIUM=j.GetCut("eleID_cutBasedElectronID_Spring15_25ns_V1_standalone_medium",false,1,false).GetTitle();
TString ELEID_TIGHT=j.GetCut("eleID_cutBasedElectronID_Spring15_25ns_V1_standalone_tight",false,1,false).GetTitle();
TString ET=j.GetCut("Et_25-noPF",1).GetTitle();
TString Charge="(chargeEle[0]==1 || chargeEle[0]==-1) && (chargeEle[1]==1 || chargeEle[1]==-1)";
TString cut_loose=ELEID_LOOSE+"&&"+ET+"&&"+Charge;
TString cut_medium=ELEID_MEDIUM+"&&"+ET+"&&"+Charge;
TString cut_tight=ELEID_TIGHT+"&&"+ET+"&&"+Charge;
data->Draw("pModeGsfEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/pModeGsfEle0_loose.png");
data->Draw("trackMomentumErrorEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/trackMomentumErrorEle0_loose.png");
data->Draw("seedEnergySCEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/seedEnergySCEle0_loose.png");
data->Draw("phiSCEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/phiSCEle0_loose.png");
data->Draw("PtEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/PtEle0_loose.png");
data->Draw("rawEnergySCEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/rawEnergySCEle0_loose.png");
data->Draw("etaSCEle[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/etaSCEle0_loose.png");
data->Draw("etaSCEle[0]",cut_medium);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/etaSCEle0_medium.png");
data->Draw("etaSCEle[0]",cut_tight);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/etaSCEle0_tight.png");
data->Draw("R9Ele[0]");
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/R90.png");
data->Draw("R9Ele[0]",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/R90_loose.png");
data->Draw("invMass_rawSC") 
data->Draw("invMass_rawSC",cut_loose);
c1->SaveAs("~/scratch1/www/Pt1Pt2/ntuples_13TeV/invMass_rawSC.png");
data->Draw("recoFlagsEle[0]");
data->Draw("HLTfire");
}
