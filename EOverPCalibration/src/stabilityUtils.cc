#include "../interface/stabilityUtils.h"

using namespace std;

int dateToInt(const std::string& date)
{
	int day;
	int month;
	int year;
	int tempo;

	TString date2 = date;

	TObjArray *splitted = date2.Tokenize("-");

	TObjString *Objstring1 = (TObjString *) splitted->At(0);
	TObjString *Objstring2 = (TObjString *) splitted->At(1);
	TObjString *Objstring3 = (TObjString *) splitted->At(2);


	TString string1 = Objstring1->GetString();
	TString string2 = Objstring2->GetString();
	TString string3 = Objstring3->GetString();


	std::cout << "day: " << string1 << std::endl;
	std::cout << "month: " << string2 << std::endl;
	std::cout << "year: " << string3 << std::endl;


	std::string stringday = string1.Data();
	std::string stringmonth = string2.Data();
	std::string stringyear = string3.Data();

	day = atoi(stringday.c_str());
	month = atoi(stringmonth.c_str());
	year = atoi(stringyear.c_str());
	/*
	istringstream buffer1(stringday);
	buffer1 >> day;
	istringstream buffer2(stringmonth);
	buffer2 >> month;
	istringstream buffer3(stringyear);
	buffer3 >> year;
	*/


	std::cout << "day: " << day << std::endl;
	std::cout << "month: " << month << std::endl;
	std::cout << "year: " << year << std::endl;
	delete splitted;


	tm time;
	time.tm_sec = 0;
	time.tm_min = 0;
	time.tm_hour = 0;
	time.tm_mday = day;
	time.tm_mon = month - 1;
	time.tm_year = year - 1900;


	tempo = timegm(&time);

	std::cout << "tempo: " << tempo << std::endl;


	return mktime(&time);

}




void SetHistoStyle(TH1* h, const std::string& label)
{
	h -> SetLineWidth(2);

	h -> GetXaxis() -> SetTitleSize(0.04);
	h -> GetXaxis() -> SetLabelSize(0.04);

	h -> GetYaxis() -> SetTitleSize(0.04);
	h -> GetYaxis() -> SetLabelSize(0.04);

	h -> GetYaxis() -> SetTitleOffset(1.5);

	if( label == "EoP") {
		h -> SetMarkerColor(kRed + 2);
		h -> SetLineColor(kRed + 2);
		h -> SetFillColor(kRed + 2);
		h -> SetFillStyle(3004);
		h -> SetMarkerStyle(7);
	}

	if( label == "EoC") {
		h -> SetMarkerColor(kGreen + 2);
		h -> SetLineColor(kGreen + 2);
		h -> SetFillColor(kGreen + 2);
		h -> SetFillStyle(3004);
		h -> SetMarkerStyle(7);
	}
}






TH1F* MellinConvolution(const std::string& name, TH1F* h_template, TH1F* h_Las)
{
	histoFunc* templateHistoFunc = new histoFunc(h_template);

	TF1* f_template = new TF1("f_template", templateHistoFunc, 0.8 * (h_Las->GetMean()), 1.4 * (h_Las->GetMean()), 3, "histoFunc");

	f_template -> SetNpx(10000);
	f_template -> FixParameter(0, 1.);
	f_template -> SetParameter(1, 1.);
	f_template -> FixParameter(2, 0.);

	TH1F* h_convolutedTemplate = (TH1F*)( h_template->Clone(name.c_str()) );
	h_convolutedTemplate -> Reset();

	for(int bin = 1; bin <= h_Las->GetNbinsX(); ++bin) {
		float scale  = h_Las->GetBinCenter(bin);
		float weight = h_Las->GetBinContent(bin);

		if( weight > 0. ) {
			f_template -> SetParameter(0, weight);
			f_template -> SetParameter(1, 1. / scale);

			for(int bin2 = 1; bin2 <= h_convolutedTemplate->GetNbinsX(); ++bin2) {
				float binCenter = h_convolutedTemplate -> GetBinCenter(bin2);
				h_convolutedTemplate -> Fill(binCenter, f_template->Eval(binCenter));
			}
		}
	}

	h_convolutedTemplate -> Scale(h_template->Integral() / h_convolutedTemplate->Integral());

	return h_convolutedTemplate;
}
