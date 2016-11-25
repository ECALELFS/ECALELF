#include "../interface/ntpleUtils2.h"

TH1F* templateHisto;
TF1* templateFunc;

std::vector<double>* mydata;

/*** fit the template ***/
void FitTemplate(const bool& draw)
{
	TCanvas* c_template;
	if( draw ) {
		c_template = new TCanvas("c_template", "template");
		c_template -> cd();
		c_template -> SetGridx();
		c_template -> SetGridy();

		templateHisto -> Scale(1. / templateHisto->GetEntries());
		templateHisto -> SetFillColor(kCyan + 2);
		templateHisto -> Draw();
	}

	templateFunc = new TF1("templateFunc", crystalBallLowHigh_v2, 0., 10., 8);
	templateFunc -> SetNpx(10000);
	templateFunc -> SetLineWidth(2);
	templateFunc -> SetLineColor(kRed);

	templateFunc -> SetParameters(templateHisto->GetMaximum(), 1., 0.05, 1., 2., 2., 1., 1.);
	templateFunc -> FixParameter(7, 1.);
	templateFunc -> SetParLimits(3, 0., 10.);
	templateFunc -> SetParLimits(5, 0., 10.);

	templateFunc -> SetParName(0, "N");
	templateFunc -> SetParName(1, "#mu");
	templateFunc -> SetParName(2, "#sigma");
	templateFunc -> SetParName(3, "#alpha_{high}");
	templateFunc -> SetParName(4, "n_{high}");
	templateFunc -> SetParName(5, "#alpha_{low}");
	templateFunc -> SetParName(6, "n_{low}");

	templateHisto -> Fit("templateFunc", "NQR+", "", 0.5, 3.);

	if( draw )
		templateFunc -> Draw("same");
}


/*** double crystall ball ***/
double crystalBallLowHigh_v2(double* x, double* par)
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
	} else if( (xx - mean) / sigma < -1.*fabs(alpha2) ) {
		double A = pow(n2 / fabs(alpha2), n2) * exp(-0.5 * alpha2 * alpha2);
		double B = n2 / fabs(alpha2) - fabs(alpha2);
		return par[0] * par[7] * A * pow(B - (xx - mean) / sigma, -1.*n2);
	} else {
		return par[0] * par[7] * exp(-1. * (xx - mean) * (xx - mean) / (2 * sigma * sigma) );
	}
}
