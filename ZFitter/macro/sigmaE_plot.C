{
	std::vector<TString> smear_vec;
	smear_vec.push_back("0.0120");
	smear_vec.push_back("0.0113");
	smear_vec.push_back("0.0133");
	smear_vec.push_back("0.0203");
	smear_vec.push_back("0.0316");
	smear_vec.push_back("0.0293");
	smear_vec.push_back("0.0416");
	smear_vec.push_back("0.0396");

	std::vector<TString> region_vec;
	region_vec.push_back("EB-absEta_0_1-gold-Et_25");
	region_vec.push_back("EB-absEta_0_1-bad-Et_25");
	region_vec.push_back("EB-absEta_1_1.4442-gold-Et_25");
	region_vec.push_back("EB-absEta_1_1.4442-bad-Et_25");
	region_vec.push_back("EE-absEta_1.566_2-gold-Et_25");
	region_vec.push_back("EE-absEta_2_2.5-gold-Et_25");
	region_vec.push_back("EE-absEta_1.566_2-bad-Et_25");
	region_vec.push_back("EE-absEta_2_2.5-bad-Et_25");
	TString selection = "invMass_SC_regrCorr_pho >60 && invMass_SC_regrCorr_pho < 120";

	for(std::vector<TString>::const_iterator category_itr = region_vec.begin();
	category_itr != region_vec.end();
	category_itr++)
	{
		TString binning;

		if(category_itr->Contains("EB")) binning = "(200,0,200)";
		else binning = "(300,50,350)";

		// ENERGY WITH SMEARING
		TCanvas *c = PlotDataMC(data, signal, "energySCEle_regrCorr_pho ", binning, *category_itr, "", "Data", "MC", "Energy E [GeV]", "", true, true, true);
		c->SaveAs("tmp/img/" + *category_itr + "-energy_regrPho-smear.eps");
		delete c;

		//    ENERGY WITHOUT SMEARING
		TCanvas *c = PlotDataMC(data, signal, "energySCEle_regrCorr_pho ", binning, *category_itr, "", "Data", "MC", "Energy E [GeV]", "", true, false, true);
		c->SaveAs("tmp/img/" + *category_itr + "-energy_regrPho-nosmear.eps");
		delete c;

		binning = "(600,0,6)";
		TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho ", binning, *category_itr, "", "Data", "MC", "#sigma_{E} [GeV] ", "", true, false, false);
		c->SaveAs("tmp/img/" + *category_itr + "-sigmaE_regrPho.eps");
		delete c;

		binning = "(600,0,0.06)";
		TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho /energySCEle_regrCorr_pho ", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E ", "", true, true, true);
		c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverE_regrPho-smear.eps");
		delete c;

		binning = "(600,0,0.6)";
		TCanvas *c = PlotDataMC(data, signal, "sqrt(energySigmaSCEle_regrCorr_pho /energySCEle_regrCorr_pho * energySigmaSCEle_regrCorr_pho /energySCEle_regrCorr_pho + " + smear_vec[category_itr - region_vec.begin()] + "*" + smear_vec[category_itr - region_vec.begin()] + ")", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E + smearing", "", true, false, false);
		c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverEoplusSmear_regrPho.eps");
		delete c;

		TCanvas *c = PlotDataMC(data, signal, "invMassSigma_SC_regrCorr_pho /invMass_SC_regrCorr_pho ", binning, *category_itr, selection, "Data", "MC", "#sigma_{M}/M", "", true, true, true);
		c->SaveAs("tmp/img/" + *category_itr + "-sigmaMoverM_regrPho-smearM.eps");
		delete c;

	}
}


TCanvas *c = PlotDataMC(data, signal, "energySCEle_regrCorr_pho[0]", binning, *category_itr, "", "Data", "MC", "Energy E [GeV]", "", true, true, true);
c->SaveAs("tmp/img/" + *category_itr + "-energy_regrPho-lead-smear.eps");
delete c;
TCanvas *c = PlotDataMC(data, signal, "energySCEle_regrCorr_pho[1]", binning, *category_itr, "", "Data", "MC", "Energy E [GeV]", "", true, true, true);
c->SaveAs("tmp/img/" + *category_itr + "-energy_regrPho-sublead-smear.eps");
delete c;

TCanvas *c = PlotDataMC(data, signal, "energySCEle_regrCorr_pho[0]", binning, *category_itr, "", "Data", "MC", "Energy E [GeV]", "", true, false, true);
c->SaveAs("tmp/img/" + *category_itr + "-energy_regrPho-lead-nosmear.eps");
delete c;
TCanvas *c = PlotDataMC(data, signal, "energySCEle_regrCorr_pho[1]", binning, *category_itr, "", "Data", "MC", "Energy E [GeV]", "", true, false, true);
c->SaveAs("tmp/img/" + *category_itr + "-energy_regrPho-sublead-nosmear.eps");
delete c;

TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho[0]/energySCEle_regrCorr_pho[0]", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E ", "", true, true, true);
c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverE_regrPho-lead-smear.eps");
delete c;
TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho[1]/energySCEle_regrCorr_pho[1]", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E ", "", true, true, true);
c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverE_regrPho-sublead-smear.eps");
delete c;

TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho /energySCEle_regrCorr_pho ", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E ", "", true, false, true);
c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverE_regrPho-nosmear.eps");
delete c;
TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho[0]/energySCEle_regrCorr_pho[0]", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E ", "", true, false, true);
c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverE_regrPho-lead-nosmear.eps");
delete c;
TCanvas *c = PlotDataMC(data, signal, "energySigmaSCEle_regrCorr_pho[1]/energySCEle_regrCorr_pho[1]", binning, *category_itr, "", "Data", "MC", "#sigma_{E}/E ", "", true, false, true);
c->SaveAs("tmp/img/" + *category_itr + "-sigmaEoverE_regrPho-sublead-nosmear.eps");
delete c;

//
binning = "(100,0.,0.1)";
TCanvas *c = PlotDataMC(data, signal, "invMassSigma_SC_regrCorr_pho /invMass_SC_regrCorr_pho ", binning, *category_itr, selection, "Data", "MC", "#sigma_{M}/M", "", true, true, true);
c->SaveAs("tmp/img/" + *category_itr + "-sigmaMoverM_regrPho-smearM.eps");
delete c;


}
}

