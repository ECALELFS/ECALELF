{
	vector<TString> runPeriods, labels;

	//runPeriods.push_back("-runNumber_190456_193621"); labels.push_back("RUN2012A");
	//runPeriods.push_back("-runNumber_193834_196531"); labels.push_back("RUN2012B");
	runPeriods.push_back("-runNumber_190456_196531");
	labels.push_back("RUN2012AB");
	runPeriods.push_back("-runNumber_198111_203002");
	labels.push_back("RUN2012C");
	runPeriods.push_back("-runNumber_203756_208686");
	labels.push_back("RUN2012D");


	TH1F *h = NULL;
	std::vector<TString>::const_iterator l_itr = labels.begin();
	for(std::vector<TString>::const_iterator p_itr = runPeriods.begin();
	p_itr != runPeriods.end();
	p_itr++, l_itr++)
	{
		int index = p_itr - runPeriods.begin();
		//    c = PlotDataMC(data, signal, "nHitsSCEle", "(180,0,180)", "EB-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "");
		//c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "");
		c = PlotDataMC(data, signal, "nPV", "(30,0,30)", "eleID_7-Et_25" + *p_itr, "", *l_itr + " (Winter13)", "Simulation runDep", "nVtx", "", false, false);

		c->SaveAs(outputPath + "/nPV" + *p_itr + ".eps");
		delete c;
		h = (TH1F *) gROOT->FindObject("data_hist");
		h->SaveAs(TString("tmp/d_hist-nPV") + (*l_itr) + ".root");

		h = (TH1F *) gROOT->FindObject("mc_hist");
		h->SaveAs(TString("tmp/s_hist-nPV") + (*l_itr) + ".root");

//   c = PlotDataMC(data, signal, "nHitsSCEle", "(180,0,180)", "EB-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "", true);
//   c->SaveAs(outputPath+"/nHitsSCEle-EB"+*p_itr+"-log.eps");
//   delete c;

//     c = PlotDataMC(data, signal, "nHitsSCEle", "(100,0,100)", "EE-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "");
//   c->SaveAs(outputPath+"/nHitsSCEle-EE"+*p_itr+".eps");
//   delete c;

//   c = PlotDataMC(data, signal, "nHitsSCEle", "(180,0,180)", "EE-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "", true);
//   c->SaveAs(outputPath+"/nHitsSCEle-EE"+*p_itr+"-log.eps");
//   delete c;

	}

}


{
	vector<TString> runPeriods, labels;

	//runPeriods.push_back("-runNumber_190456_193621"); labels.push_back("RUN2012A");
	//runPeriods.push_back("-runNumber_193834_196531"); labels.push_back("RUN2012B");
	runPeriods.push_back("-runNumber_190456_196531");
	labels.push_back("RUN2012AB");
	runPeriods.push_back("-runNumber_198111_203002");
	labels.push_back("RUN2012C");
	runPeriods.push_back("-runNumber_203756_208686");
	labels.push_back("RUN2012D");


	TH1F *h = NULL;
	std::vector<TString>::const_iterator l_itr = labels.begin();
	for(std::vector<TString>::const_iterator p_itr = runPeriods.begin();
	p_itr != runPeriods.end();
	p_itr++, l_itr++)
	{
		int index = p_itr - runPeriods.begin();
		//    c = PlotDataMC(data, signal, "nHitsSCEle", "(180,0,180)", "EB-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "");
		//c = PlotDataMC(data, signal, "R9Ele", "(110,0.3,1.4)", "EB-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "");
		c = PlotDataMC(data, signal, "nPV", "(30,0,30)", "eleID_7-Et_25" + *p_itr, "", *l_itr + " (Winter13)", "Simulation runDep", "nVtx", "", false, false);

		c->SaveAs(outputPath + "/nPV" + *p_itr + ".eps");
		delete c;
		h = (TH1F *) gROOT->FindObject("data_hist");
		h->SaveAs(TString("tmp/d_hist-nPV") + (*l_itr) + ".root");

		h = (TH1F *) gROOT->FindObject("mc_hist");
		h->SaveAs(TString("tmp/s_hist-nPV") + (*l_itr) + ".root");

//   c = PlotDataMC(data, signal, "nHitsSCEle", "(180,0,180)", "EB-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "", true);
//   c->SaveAs(outputPath+"/nHitsSCEle-EB"+*p_itr+"-log.eps");
//   delete c;

//     c = PlotDataMC(data, signal, "nHitsSCEle", "(100,0,100)", "EE-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "");
//   c->SaveAs(outputPath+"/nHitsSCEle-EE"+*p_itr+".eps");
//   delete c;

//   c = PlotDataMC(data, signal, "nHitsSCEle", "(180,0,180)", "EE-eleID_7-Et_25"+*p_itr,"", *l_itr+" (Winter13)", "Simulation", "nHits SC", "", true);
//   c->SaveAs(outputPath+"/nHitsSCEle-EE"+*p_itr+"-log.eps");
//   delete c;

	}

}


