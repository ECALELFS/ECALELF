{
	TString histName = "mc_hist";
	TH1F *runA_ = (TH1F *) _file0->Get(histName);
	TH1F *runB_ = (TH1F *) _file1->Get(histName);
	TH1F *runC_ = (TH1F *) _file2->Get(histName);
	//  TH1F *runD_ = (TH1F *) _file3->Get(histName);



	TH1F *runA = (TH1F *) runA_->DrawNormalized();
	TH1F *runB = (TH1F *) runB_->DrawNormalized("same");
	TH1F *runC = (TH1F *) runC_->DrawNormalized("same");
	//TH1F *runD = (TH1F *) runD_->DrawNormalized("same");

	runA->GetXaxis()->SetTitle("R9");
	runA->GetYaxis()->SetRangeUser(0.0000000001, 0.16);
	runB->GetYaxis()->SetRangeUser(0.0000000001, 0.16);
	runC->GetYaxis()->SetRangeUser(0.0000000001, 0.16);
	//runD->GetYaxis()->SetRangeUser(0.0000000001,0.16);

	runA->SetLineColor(kRed);
	runB->SetLineColor(kGreen);
	runC->SetLineColor(kBlue);
	//runD->SetLineColor(kBlack);

	runA->SetFillColor(kRed);
	runB->SetFillColor(kGreen);
	runC->SetFillColor(kBlue);
	//runD->SetFillColor(kBlack);

	runA->SetFillStyle(0);
	runB->SetFillStyle(0);
	runC->SetFillStyle(0);
	//runD->SetFillStyle(0);

	runA->SetLineWidth(3);
	runB->SetLineWidth(2);
	runC->SetLineWidth(2);
	//runD->SetLineWidth(3);

	TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
	leg->AddEntry(runA, "RUN A", "lf");
	leg->AddEntry(runB, "RUN B", "lf");
	leg->AddEntry(runC, "RUN C", "lf");
	//leg->AddEntry(runD, "RUN D","lf");

	leg->Draw();


} {
	TString histName = "mc_hist";
	TH1F *runA_ = (TH1F *) _file0->Get(histName);
	TH1F *runB_ = (TH1F *) _file1->Get(histName);
	TH1F *runC_ = (TH1F *) _file2->Get(histName);
	//  TH1F *runD_ = (TH1F *) _file3->Get(histName);



	TH1F *runA = (TH1F *) runA_->DrawNormalized();
	TH1F *runB = (TH1F *) runB_->DrawNormalized("same");
	TH1F *runC = (TH1F *) runC_->DrawNormalized("same");
	//TH1F *runD = (TH1F *) runD_->DrawNormalized("same");

	runA->GetXaxis()->SetTitle("R9");
	runA->GetYaxis()->SetRangeUser(0.0000000001, 0.16);
	runB->GetYaxis()->SetRangeUser(0.0000000001, 0.16);
	runC->GetYaxis()->SetRangeUser(0.0000000001, 0.16);
	//runD->GetYaxis()->SetRangeUser(0.0000000001,0.16);

	runA->SetLineColor(kRed);
	runB->SetLineColor(kGreen);
	runC->SetLineColor(kBlue);
	//runD->SetLineColor(kBlack);

	runA->SetFillColor(kRed);
	runB->SetFillColor(kGreen);
	runC->SetFillColor(kBlue);
	//runD->SetFillColor(kBlack);

	runA->SetFillStyle(0);
	runB->SetFillStyle(0);
	runC->SetFillStyle(0);
	//runD->SetFillStyle(0);

	runA->SetLineWidth(3);
	runB->SetLineWidth(2);
	runC->SetLineWidth(2);
	//runD->SetLineWidth(3);

	TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
	leg->AddEntry(runA, "RUN A", "lf");
	leg->AddEntry(runB, "RUN B", "lf");
	leg->AddEntry(runC, "RUN C", "lf");
	//leg->AddEntry(runD, "RUN D","lf");

	leg->Draw();


}
