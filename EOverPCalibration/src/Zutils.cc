#include "Calibration/EOverPCalibration/interface/Zutils.h"
#include <cmath>
#include "TMath.h"
#include "TRandom3.h"
#include "TStyle.h"

int nPoint;
double mZ_min, mZ_max ;
std::string energySC ;

/*** breit-wigner ***/
double breitWigner(double* x, double* par)
{
	//[0] = N
	//[1] = mass
	//[2] = width

	double xx = x[0];
	double M = par[1];
	double G = par[2];

	double gamma = sqrt(M * M * (M * M + G * G));
	double norm = (2.*sqrt(2) * M * G * gamma) / (TMath::Pi() * sqrt(M * M + gamma));

	return par[0] * norm / ( pow((xx * xx - M * M), 2) + M * M * G * G );
}

/*** crystall ball with low tail ***/
double crystalBallLow(double* x, double* par)
{
	//[0] = N
	//[1] = mean
	//[2] = sigma
	//[3] = alpha
	//[4] = n

	double xx = x[0];
	double mean = par[1];
	double sigma = par[2];
	double alpha = par[3];
	double n = par[4];

	if( (xx - mean) / sigma <= -1.*fabs(alpha) ) {
		double A = pow(n / fabs(alpha), n) * exp(-0.5 * TMath::Power(fabs(alpha), 2));
		double B = n / fabs(alpha) - fabs(alpha);
		return par[0] * A * pow((B - (xx - mean) / sigma), -1.*n);
	}

	else {
		return par[0] * exp(-1. * (xx - mean) * (xx - mean) / (2 * sigma * sigma) );
	}

}

/*** double crystall ball ***/
double crystalBallLowHigh(double* x, double* par)
{
	//[0] = N
	//[1] = mean
	//[2] = sigma
	//[3] = alpha
	//[4] = n
	//[5] = alpha2
	//[6] = n2
	//[7] = scale

	double xx = x[0] * par[7];
	double mean = par[1];
	double sigma = par[2];
	double alpha = par[3];
	double n = par[4];
	double alpha2 = par[5];
	double n2 = par[6];

	if( (xx - mean) / sigma > fabs(alpha) ) {
		double A = pow(n / fabs(alpha), n) * exp(-0.5 * alpha * alpha);
		double B = n / fabs(alpha) - fabs(alpha);

		return par[0] * par[7] * A * pow(B + (xx - mean) / sigma, -1.*n);
	}

	else if( (xx - mean) / sigma < -1.*fabs(alpha2) ) {
		double A = pow(n2 / fabs(alpha2), n2) * exp(-0.5 * alpha2 * alpha2);
		double B = n2 / fabs(alpha2) - fabs(alpha2);

		return par[0] * par[7] * A * pow(B - (xx - mean) / sigma, -1.*n2);
	}

	else {
		return par[0] * par[7] * exp(-1. * (xx - mean) * (xx - mean) / (2 * sigma * sigma) );
	}

}

/*** breit-wigner convoluted with crystalBall ***/
double breitWigner_crystalBallLow(double* x, double* par)
{
	//[0] = N
	//[1] = b.w. - mass
	//[2] = b.w. - width
	//[3] = c.b. - mean
	//[4] = c.b. - sigma
	//[5] = c.b. - alpha
	//[6] = c.b. - n

	double xx = x[0];
	double N = par[0];
	double M = par[1];
	double G = par[2];
	double mean = par[3];
	double sigma = par[4];
	double alpha = par[5];
	double n = par[6];

	TF1* bw = new TF1("temp_bw", breitWigner, mZ_min, mZ_max, 3);
	bw -> FixParameter(0, 1.);
	bw -> FixParameter(1, M);
	bw -> FixParameter(2, G);

	TF1* cb = new TF1("temp_cb", crystalBallLow, mZ_min, mZ_max, 5);

	cb -> FixParameter(0, N);
	cb -> FixParameter(1, mean);
	cb -> FixParameter(2, sigma);
	cb -> FixParameter(3, alpha);
	cb -> FixParameter(4, n);

	double stepWidth = (mZ_max - mZ_min) / nPoint;
	double val = 0.;

	for(int ibin = 0; ibin < nPoint; ++ibin) {
		double yy = mZ_min + ibin * stepWidth;
		val += bw->Eval(yy) * cb->Eval(xx - yy);
	}

	delete bw;
	delete cb;
	return val;
}

/*** Visible BwCB FWH estimation assumption the function is monotonic in the interval***/
std::pair<double, double> breitWigner_crystalBallLowFWHM(TF1* bwcb, const double &min, const double &max, const double &Precision, const int & maxCycle)
{

	std::pair<double, double> extreme ;
	extreme.first = -99. ;
	extreme.second = -99.;

	std::cout << " Entering " << std::endl;

	double xMAX = bwcb->GetMaximumX();

	std::cout << " xMax = " << xMAX << std::endl;

	double MAX = bwcb->GetMaximum();

	std::cout << " MAX = " << MAX << std::endl;

	std::pair<double, double> IntervalUp ;
	IntervalUp.first = xMAX ;
	IntervalUp.second = max;

	std::pair<double, double> IntervalDown ;
	IntervalDown.first = min ;
	IntervalDown.second = xMAX;

	bool EndUp = false, EndDown = false;

	for(int icycle = 0; icycle < maxCycle; icycle++) {

		double xCenterDown = (IntervalDown.second - IntervalDown.first ) / 2 + IntervalDown.first;
		double xCenterUp = (IntervalUp.second - IntervalUp.first ) / 2 + IntervalUp.first;

		if(!EndDown) MoveInterval(bwcb, xCenterDown, IntervalDown, MAX / 2);
		if(!EndUp) MoveInterval(bwcb, xCenterUp, IntervalUp, MAX / 2);

		if(fabs(bwcb->Eval(IntervalDown.second) - bwcb->Eval(IntervalDown.first)) <= Precision) EndDown = true;

		if(fabs(bwcb->Eval(IntervalUp.second) - bwcb->Eval(IntervalUp.first)) <= Precision) EndUp = true;

		if(EndUp && EndDown) break;

	}

	extreme.first = (IntervalDown.second - IntervalDown.first ) / 2 + IntervalDown.first;
	extreme.second = (IntervalUp.second - IntervalUp.first ) / 2 + IntervalUp.first;

	std::cout << " extreme first = " << extreme.first << " extreme second = " << extreme.second << std::endl;

	return extreme;

}

/*** Decide the right interval to bracket a point assumption the function is monotonic in the interval ***/
void MoveInterval(TF1* bwcb, const double & xCenter, std::pair<double, double> & Interval, const double & MAX)
{

	if(bwcb->Eval(xCenter) < MAX && ((bwcb->Eval(Interval.second) - bwcb->Eval(Interval.first)) / (Interval.second - Interval.first)) > 0) Interval.first = xCenter;

	if(bwcb->Eval(xCenter) > MAX && ((bwcb->Eval(Interval.second) - bwcb->Eval(Interval.first)) / (Interval.second - Interval.first)) > 0) Interval.second = xCenter;

	if(bwcb->Eval(xCenter) < MAX && ((bwcb->Eval(Interval.second) - bwcb->Eval(Interval.first)) / (Interval.second - Interval.first)) < 0) Interval.second = xCenter;

	if(bwcb->Eval(xCenter) > MAX && ((bwcb->Eval(Interval.second) - bwcb->Eval(Interval.first)) / (Interval.second - Interval.first)) < 0) Interval.first = xCenter;


}


/*** Binned chi^{2} fit ***/

void BinnedFitZPeak(const std::string& category, const int& rebin, TH1F* h_mZ_DATA, TH1F* h_mZ_MC,
                    int nPoints, const double &min, const double &max, std::string energyType )
{

	nPoint = nPoints;
	mZ_min = min;
	mZ_max = max;
	energySC = energyType;

	TRandom3 * rand = new TRandom3();

	//-----------
	// draw peaks
	std::cout << ">>>Zutils::DrawZPeak::draw peaks" << std::endl;
	std::string h_mZ_DATA_Name = h_mZ_DATA->GetName();
	std::string h_mZ_MC_Name = h_mZ_MC->GetName();

	TCanvas* c = new TCanvas(("c_mZ_DATA" + h_mZ_DATA_Name).c_str(), ("mZ - " + h_mZ_DATA_Name).c_str(), 0, 0, 600, 600);
	c -> cd();
	c -> SetGridx();
	c -> SetGridy();

	c -> SetLeftMargin(0.15);
	c -> SetRightMargin(0.15);

	char axisTitle[50];
	sprintf(axisTitle, "events / %.2e GeV/c^{2}", h_mZ_DATA->GetBinWidth(1));
	h_mZ_DATA -> Rebin(rebin);
	h_mZ_DATA -> GetXaxis() -> SetLabelSize(0.03);
	h_mZ_DATA -> GetXaxis() -> SetLabelFont(42);
	h_mZ_DATA -> GetXaxis() -> SetTitleSize(0.03);
	h_mZ_DATA -> GetXaxis() -> SetTitleOffset(1.5);
	h_mZ_DATA -> GetXaxis() -> SetTitle(("m(e^{+}e^{-}) - " + category).c_str());
	h_mZ_DATA -> GetYaxis() -> SetLabelSize(0.03);
	h_mZ_DATA -> GetYaxis() -> SetTitle(axisTitle);

	h_mZ_DATA -> SetLineWidth(2);
	h_mZ_DATA -> SetLineColor(kGreen + 2);
	h_mZ_DATA -> SetMarkerColor(kBlack);
	h_mZ_DATA -> SetMarkerStyle(20);
	h_mZ_DATA -> SetMarkerSize(0.5);
	h_mZ_DATA -> GetYaxis() -> SetTitleOffset(1.5);
	h_mZ_DATA -> Draw("P");
	gPad->Update();

	h_mZ_DATA  -> GetYaxis() -> SetRangeUser(0., 1.05 * h_mZ_DATA->GetMaximum());
	gPad->Update();

	TCanvas* c1 = new TCanvas(("c_mZ_MC" + h_mZ_MC_Name).c_str(), ("mZ - " + h_mZ_MC_Name).c_str(), 0, 0, 600, 600);
	c1 -> cd();
	c1 -> SetGridx();
	c1 -> SetGridy();

	c1 -> SetLeftMargin(0.15);
	c1 -> SetRightMargin(0.15);

	h_mZ_MC -> Rebin(rebin);
	h_mZ_MC -> Scale(1.*h_mZ_DATA->GetEntries() / h_mZ_MC->GetEntries());
	h_mZ_MC -> GetXaxis() -> SetLabelSize(0.03);
	h_mZ_MC -> GetXaxis() -> SetLabelFont(42);
	h_mZ_MC -> GetXaxis() -> SetTitleSize(0.03);
	h_mZ_MC -> GetXaxis() -> SetTitleOffset(1.5);
	h_mZ_MC -> GetXaxis() -> SetTitle(("m(e^{+}e^{-}) - " + category).c_str());
	h_mZ_MC -> GetYaxis() -> SetLabelSize(0.03);
	h_mZ_MC -> GetYaxis() -> SetTitleOffset(1.5);
	h_mZ_MC -> GetYaxis() -> SetTitle(axisTitle);

	h_mZ_MC -> SetLineWidth(2);
	h_mZ_MC -> SetLineColor(kRed + 1);
	h_mZ_MC -> SetMarkerColor(kBlack);
	h_mZ_MC -> SetMarkerStyle(20);
	h_mZ_MC -> SetMarkerSize(0.5);
	h_mZ_MC -> Draw("P");
	gPad->Update();

	h_mZ_MC  -> GetYaxis() -> SetRangeUser(0., 1.05 * h_mZ_MC->GetMaximum());
	gPad->Update();

	//----------
	// fit peaks

	std::cout << ">>>Zutils::DrawZPeak::fit peaks" << std::endl;
	std::string funcNameDATA = "bw_cb_DATA_" + category + "_" + energySC;
	std::string funcNameMC = "bw_cb_MC_" + category + "_" + energySC;

	if(category == "EE-EE" || category == "EEp" || category == "EEm" || category == "EE_R9_g" || category == "EE_R9_l" || category == "EE_Eta_l" || category == "EE_Eta_g" ) {

		TF1* bw_cb_DATA = new TF1(funcNameDATA.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_DATA -> SetNpx(10000);
		bw_cb_DATA -> SetLineColor(kGreen + 2);
		bw_cb_DATA -> SetLineWidth(2);
		bw_cb_DATA -> SetLineStyle(1);
		c->cd();

		TFitResultPtr rpDATA;
		int fStatusDATA;
		do {
			SetParameterFunctionEE_EE(bw_cb_DATA, rand);
			rpDATA = h_mZ_DATA -> Fit(funcNameDATA.c_str(), "SR0EL");
			fStatusDATA = rpDATA;
		} while(fStatusDATA != 0);

		bw_cb_DATA -> Draw("same");


		TF1* bw_cb_MC = new TF1(funcNameMC.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_MC -> SetNpx(10000);
		bw_cb_MC -> SetLineWidth(2);
		bw_cb_MC -> SetLineStyle(1);
		bw_cb_MC -> SetLineColor(kRed + 1);
		c1->cd();

		int fStatusMC;
		TFitResultPtr rpMC;
		do {

			SetParameterFunctionEE_EE(bw_cb_MC, rand);
			rpMC = h_mZ_MC -> Fit(funcNameMC.c_str(), "SR0EWL");
			fStatusMC = rpMC;
			std::cout << " fStatus " << fStatusMC << std::endl;
		} while(fStatusMC != 0);

		bw_cb_MC -> Draw("same");
	}

	if(category == "EB-EE" || category == "EB_R9_l") {

		TF1* bw_cb_DATA = new TF1(funcNameDATA.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_DATA -> SetNpx(10000);
		bw_cb_DATA -> SetLineColor(kGreen + 2);
		bw_cb_DATA -> SetLineWidth(2);
		bw_cb_DATA -> SetLineStyle(1);
		c->cd();
		TFitResultPtr rpDATA;
		int fStatusDATA;
		do {
			SetParameterFunctionEB_EE(bw_cb_DATA, rand);
			rpDATA = h_mZ_DATA -> Fit(funcNameDATA.c_str(), "SR0EL");
			fStatusDATA = rpDATA;
		} while(fStatusDATA != 0);

		bw_cb_DATA -> Draw("same");

		TF1* bw_cb_MC = new TF1(funcNameMC.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_MC -> SetNpx(10000);
		bw_cb_MC -> SetLineWidth(2);
		bw_cb_MC -> SetLineStyle(1);
		bw_cb_MC -> SetLineColor(kRed + 1);
		c1->cd();

		TFitResultPtr rpMC;
		int fStatusMC;
		do {
			SetParameterFunctionEB_EE(bw_cb_MC, rand);
			rpMC = h_mZ_MC -> Fit(funcNameMC.c_str(), "SR0EWL");
			fStatusMC = rpMC;
		} while(fStatusMC != 0);

		bw_cb_MC -> Draw("same");
	}

	if(category == "EB-EB" || category == "EBp" || category == "EBm" || category == "EB_R9_g" || category == "EB_Eta_l" || category == "EB_Eta_g") {

		TF1* bw_cb_DATA = new TF1(funcNameDATA.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_DATA -> SetNpx(10000);
		bw_cb_DATA -> SetLineColor(kGreen + 2);
		bw_cb_DATA -> SetLineWidth(2);
		bw_cb_DATA -> SetLineStyle(1);
		c->cd();

		TFitResultPtr rpDATA;
		int fStatusDATA;
		do {
			SetParameterFunctionEB_EB(bw_cb_DATA, rand);
			rpDATA = h_mZ_DATA -> Fit(funcNameDATA.c_str(), "SR0EL");
			fStatusDATA = rpDATA;
		} while(fStatusDATA != 0);

		bw_cb_DATA -> Draw("same");

		TF1* bw_cb_MC = new TF1(funcNameMC.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_MC -> SetNpx(10000);
		bw_cb_MC -> SetLineWidth(2);
		bw_cb_MC -> SetLineStyle(1);
		bw_cb_MC -> SetLineColor(kRed + 1);
		c1->cd();

		TFitResultPtr rpMC;
		int fStatusMC;
		do {
			SetParameterFunctionEB_EB(bw_cb_MC, rand);
			rpMC = h_mZ_MC -> Fit(funcNameMC.c_str(), "SR0EWL+");
			fStatusMC = rpMC;
		} while(fStatusMC != 0);

		bw_cb_MC -> Draw("same");

	}

	// ------------------
	// get the statistics


	TPaveStats* p_mZ_DATA = (TPaveStats*)(h_mZ_DATA->GetListOfFunctions()->FindObject("stats"));
	p_mZ_DATA->SetX1NDC(0.69);
	p_mZ_DATA->SetX2NDC(0.99);
	p_mZ_DATA->SetY1NDC(0.69);
	p_mZ_DATA->SetY2NDC(0.99);
	p_mZ_DATA->SetTextColor(kGreen + 2);
	p_mZ_DATA->SetOptFit(0012);
	p_mZ_DATA->Draw("same");

	TPaveStats* p_mZ_MC = (TPaveStats*)(h_mZ_MC->GetListOfFunctions()->FindObject("stats"));
	p_mZ_MC->SetX1NDC(0.69);
	p_mZ_MC->SetX2NDC(0.99);
	p_mZ_MC->SetY1NDC(0.69);
	p_mZ_MC->SetY2NDC(0.99);
	p_mZ_MC->SetTextColor(kRed + 1);
	p_mZ_MC->SetOptFit(0012);
	p_mZ_MC->Draw("same");


	// ---------------
	// print the plots

	c -> Print((h_mZ_DATA_Name + "_rerecoICHEP2012.png").c_str(), "png");
	c1 -> Print((h_mZ_MC_Name + "_MC_IC_v2.png").c_str(), "png");

	delete c;
	delete c1;
}


void BinnedFitZPeak(const std::string& category, const int& rebin, TH1F* h_mZ_DATA,
                    int nPoints, const double &min, const double &max, std::string energyType )
{

	nPoint = nPoints;
	mZ_min = min;
	mZ_max = max;
	energySC = energyType ;

	TRandom3 * rand = new TRandom3();

	//-----------
	// draw peaks
	std::cout << ">>>Zutils::DrawZPeak::draw peaks" << std::endl;
	std::string h_mZ_DATA_Name = h_mZ_DATA->GetName();

	TCanvas* c = new TCanvas(("c_mZ_DATA" + h_mZ_DATA_Name).c_str(), ("mZ - " + h_mZ_DATA_Name).c_str(), 0, 0, 600, 600);
	c -> cd();
	c -> SetGridx();
	c -> SetGridy();

	c -> SetLeftMargin(0.15);
	c -> SetRightMargin(0.15);

	char axisTitle[50];
	sprintf(axisTitle, "events / %.2e GeV/c^{2}", h_mZ_DATA->GetBinWidth(1));
	h_mZ_DATA -> Rebin(rebin);
	h_mZ_DATA -> GetXaxis() -> SetLabelSize(0.03);
	h_mZ_DATA -> GetXaxis() -> SetLabelFont(42);
	h_mZ_DATA -> GetXaxis() -> SetTitleSize(0.03);
	h_mZ_DATA -> GetXaxis() -> SetTitleOffset(1.5);
	h_mZ_DATA -> GetXaxis() -> SetTitle(("m(e^{+}e^{-}) - " + category).c_str());
	h_mZ_DATA -> GetYaxis() -> SetLabelSize(0.03);
	h_mZ_DATA -> GetYaxis() -> SetTitle(axisTitle);

	h_mZ_DATA -> SetLineWidth(2);
	h_mZ_DATA -> SetLineColor(kGreen + 2);
	h_mZ_DATA -> SetMarkerColor(kBlack);
	h_mZ_DATA -> SetMarkerStyle(20);
	h_mZ_DATA -> SetMarkerSize(0.5);
	h_mZ_DATA -> GetYaxis() -> SetTitleOffset(1.5);
	h_mZ_DATA -> Draw("P");
	gPad->Update();

	h_mZ_DATA  -> GetYaxis() -> SetRangeUser(0., 1.05 * h_mZ_DATA->GetMaximum());
	gPad->Update();

	//----------
	// fit peaks

	std::cout << ">>>Zutils::DrawZPeak::fit peaks" << std::endl;
	std::string funcNameDATA = "bw_cb_DATA_" + category + "_" + energyType;


	if(category == "EE-EE" || category == "EEp" || category == "EEm" || category == "EE_R9_g" || category == "EE_R9_l" || category == "EE_Eta_l" || category == "EE_Eta_g" ) {
		TF1* bw_cb_DATA = new TF1(funcNameDATA.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_DATA -> SetNpx(10000);
		bw_cb_DATA -> SetLineColor(kGreen + 2);
		bw_cb_DATA -> SetLineWidth(2);
		bw_cb_DATA -> SetLineStyle(1);
		c->cd();
		TFitResultPtr rp;
		int fStatus;
		do {
			SetParameterFunctionEE_EE(bw_cb_DATA, rand);
			rp = h_mZ_DATA -> Fit(funcNameDATA.c_str(), "SR0EL");
			fStatus = rp;
		} while(fStatus != 0);

		bw_cb_DATA -> Draw("same");

	}

	if(category == "EB-EE" || category == "EB_R9_l" ) {
		TF1* bw_cb_DATA = new TF1(funcNameDATA.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_DATA -> SetNpx(10000);
		bw_cb_DATA -> SetLineColor(kGreen + 2);
		bw_cb_DATA -> SetLineWidth(2);
		bw_cb_DATA -> SetLineStyle(1);
		c->cd();
		TFitResultPtr rp;
		int fStatus;
		do {
			SetParameterFunctionEB_EE(bw_cb_DATA, rand);
			rp = h_mZ_DATA -> Fit(funcNameDATA.c_str(), "SR0EL");
			fStatus = rp;
		} while(fStatus != 0);
		bw_cb_DATA -> Draw("same");
	}

	if(category == "EB-EB" || category == "EBp" || category == "EBm" || category == "EB_R9_g" || category == "EB_Eta_l" || category == "EB_Eta_g") {
		TF1* bw_cb_DATA = new TF1(funcNameDATA.c_str(), breitWigner_crystalBallLow, mZ_min, mZ_max, 7);
		bw_cb_DATA -> SetNpx(10000);
		bw_cb_DATA -> SetLineColor(kGreen + 2);
		bw_cb_DATA -> SetLineWidth(2);
		bw_cb_DATA -> SetLineStyle(1);
		c->cd();
		TFitResultPtr rp;
		int fStatus;
		do {
			SetParameterFunctionEB_EB(bw_cb_DATA, rand);
			rp = h_mZ_DATA -> Fit(funcNameDATA.c_str(), "SR0EL");
			fStatus = rp;
		} while(fStatus != 0);
		bw_cb_DATA -> Draw("same");
	}


	// ------------------
	// get the statistics


	TPaveStats* p_mZ_DATA = (TPaveStats*)(h_mZ_DATA->GetListOfFunctions()->FindObject("stats"));
	p_mZ_DATA->SetX1NDC(0.69);
	p_mZ_DATA->SetX2NDC(0.99);
	p_mZ_DATA->SetY1NDC(0.69);
	p_mZ_DATA->SetY2NDC(0.99);
	p_mZ_DATA->SetTextColor(kGreen + 2);
	p_mZ_DATA->SetOptFit(0012);
	p_mZ_DATA->Draw("same");

	// ---------------
	// print the plots

	c -> Print((h_mZ_DATA_Name + "_DATA_DATA_rerecoICHEP2012_v2.png").c_str(), "png");

	delete c;
}

void SetParameterFunctionEE_EE(TF1* bw_cb, TRandom3 * rand)
{

	// Z invariant Mass
	bw_cb -> SetParName(1, "M_{Z}");
	bw_cb -> FixParameter(1, 91.18);
	// Z FWHM
	bw_cb -> SetParName(2, "#Gamma_{Z}");
	bw_cb -> FixParameter(2, 2.4952);
// Normalization
	bw_cb -> SetParName(0, "N");
	bw_cb -> SetParameter(0, rand->Uniform(10., 20.));
	// Delta M
	bw_cb -> SetParName(3, "#Deltam");
	bw_cb -> SetParameter(3, rand->Uniform(0., 1.5));
	// Sigma CB
	bw_cb -> SetParName(4, "#sigma_{CB}");
	bw_cb -> SetParameter(4, rand->Uniform(2., 3.5));
	// Alpha CB
	bw_cb -> SetParName(5, "#alpha");
	bw_cb -> SetParameter(5, rand->Uniform(1., 2.));
	// n parameter CB
	bw_cb -> SetParName(6, "n");
	bw_cb -> SetParameter(6, rand->Uniform(1.5, 4.));
}

void SetParameterFunctionEB_EE(TF1* bw_cb, TRandom3 * rand)
{

	// Z invariant Mass
	bw_cb -> SetParName(1, "M_{Z}");
	bw_cb -> FixParameter(1, 91.18);
	// Z FWHM
	bw_cb -> SetParName(2, "#Gamma_{Z}");
	bw_cb -> FixParameter(2, 2.4952);
	// Normalization
	bw_cb -> SetParName(0, "N");
	bw_cb -> SetParameter(0, rand->Uniform(300., 600.));
	// Delta M
	bw_cb -> SetParName(3, "#Deltam");
	bw_cb -> SetParameter(3, rand->Uniform(0.9, 1.5));
	// Sigma CB
	bw_cb -> SetParName(4, "#sigma_{CB}");
	bw_cb -> SetParameter(4, rand->Uniform(2., 3.));
	// Alpha CB
	bw_cb -> SetParName(5, "#alpha");
	bw_cb -> SetParameter(5, rand->Uniform(0.7, 1.4));
	// n parameter CB
	bw_cb -> SetParName(6, "n");
	bw_cb -> SetParameter(6, rand->Uniform(2., 4.));
}

void SetParameterFunctionEB_EB(TF1* bw_cb, TRandom3 * rand)
{

	// Z invariant Mass
	bw_cb -> SetParName(1, "M_{Z}");
	bw_cb -> FixParameter(1, 91.18);
	// Z FWHM
	bw_cb -> SetParName(2, "#Gamma_{Z}");
	bw_cb -> FixParameter(2, 2.4952);
	// Normalization
	bw_cb -> SetParName(0, "N");
	bw_cb -> SetParameter(0, rand->Uniform(300., 500.));
	// Delta M
	bw_cb -> SetParName(3, "#Deltam");
	bw_cb -> SetParameter(3, rand->Uniform(-0.5, 0.7));
	// Sigma CB
	bw_cb -> SetParName(4, "#sigma_{CB}");
	bw_cb -> SetParameter(4, rand->Uniform(0.7, 1.6));
	// Alpha CB
	bw_cb -> SetParName(5, "#alpha");
	bw_cb -> SetParameter(5, rand->Uniform(1., 1.9));
	// n parameter CB
	bw_cb -> SetParName(6, "n");
	bw_cb -> SetParameter(6, rand->Uniform(1.5, 3.7));
}
