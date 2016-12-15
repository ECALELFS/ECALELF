#include "../interface/ConvoluteTemplate.h"



TH1F* ConvoluteTemplate(const std::string& name, TH1F* h_template, TH1F* h_smearing,
                        int nPoints, double min, double max)
{
	double width = 1.*(max - min) / nPoints;

	double* FFT_re_func1 = new double[nPoints];
	double* FFT_im_func1 = new double[nPoints];
	double* FFT_re_func2 = new double[nPoints];
	double* FFT_im_func2 = new double[nPoints];
	double* FFT_re_convolution = new double[nPoints];
	double* FFT_im_convolution = new double[nPoints];



	//-----------------------------
	// define the initial functions

	char funcName[50];


	sprintf(funcName, "f_template_temp");
	histoFunc* hf_template = new histoFunc(h_template);
	TF1* f_template = new TF1(funcName, hf_template, min, max, 3, "histoFunc");

	f_template -> FixParameter(0, 1.);
	f_template -> FixParameter(1, 1.);
	f_template -> FixParameter(2, 0.);


	sprintf(funcName, "f_smearing_temp");
	histoFunc* hf_smearing = new histoFunc(h_smearing);
	TF1* f_smearing = new TF1(funcName, hf_smearing, min, max, 3, "histoFunc");
	f_smearing -> FixParameter(0, 1.);
	f_smearing -> FixParameter(1, 1.);
	f_smearing -> FixParameter(2, -1.*h_smearing->GetMean());


	// use a gaussian as smearing function
	//TF1* f_smearing = new TF1("f_smearing","1./([0]*sqrt(2.*3.14159))*exp(-1.*x*x/(2.*[0]*[0]))",min,max);
	//f_smearing -> FixParameter(0,0.05);



	//-----------------------------------------
	// define the histograms to contain the FFT
	TH1D* h_func1     = new TH1D("h_func1",    "", nPoints, min, max);
	TH1D* h_func1_FFT = new TH1D("h_func1_FFT", "", nPoints, min, max);

	TH1D* h_func2     = new TH1D("h_func2",    "", nPoints, min, max);
	TH1D* h_func2_FFT = new TH1D("h_func2_FFT", "", nPoints, min, max);

	for(int bin = 1; bin <= nPoints; ++bin) {
		h_func1 -> SetBinContent(bin, f_template->Eval(min + width * (bin - 1)));
		h_func2 -> SetBinContent(bin, f_smearing->Eval(min + width * (bin - 1)));
	}

	//h_func1 -> Write();
	//h_func2 -> Write();



	//-----------
	// do the FFT

	h_func1 -> FFT(h_func1_FFT, "MAG R2C M");
	TVirtualFFT* FFT_func1 = TVirtualFFT::GetCurrentTransform();
	FFT_func1 -> GetPointsComplex(FFT_re_func1, FFT_im_func1);
	FFT_func1 -> Delete();

	h_func2 -> FFT(h_func2_FFT,  "MAG R2C M");
	TVirtualFFT* FFT_func2 = TVirtualFFT::GetCurrentTransform();
	FFT_func2 -> GetPointsComplex(FFT_re_func2, FFT_im_func2);
	FFT_func2 -> Delete();



	//---------------------------------
	// convolution in the Fourier space

	for(int bin = 1; bin <= nPoints; ++bin) {
		FFT_re_convolution[bin - 1] = FFT_re_func1[bin - 1] * FFT_re_func2[bin - 1] - FFT_im_func1[bin - 1] * FFT_im_func2[bin - 1];
		FFT_im_convolution[bin - 1] = FFT_re_func1[bin - 1] * FFT_im_func2[bin - 1] + FFT_im_func1[bin - 1] * FFT_re_func2[bin - 1];
	}



	//----------------
	// do the anti FFT

	TVirtualFFT* AFFT_convolution = TVirtualFFT::FFT(1, &nPoints, "C2R M K");
	AFFT_convolution -> SetPointsComplex(FFT_re_convolution, FFT_im_convolution);
	AFFT_convolution -> Transform();

	TH1* h_convolution_temp = NULL;
	h_convolution_temp = TH1::TransformHisto(AFFT_convolution, h_convolution_temp, "Re");
	h_convolution_temp -> SetName("h_convolution_temp");
	h_convolution_temp -> Scale((max - min) / nPoints / nPoints);
	AFFT_convolution -> Delete();

	TH1D* h_convolution = new TH1D("h_convolution", "", nPoints, min, max);
	for(int bin = 1; bin <= nPoints; ++bin) {
		h_convolution -> Fill((max - min) / nPoints * (bin - 1), h_convolution_temp->GetBinContent(bin));
	}

	histoFunc* convolutionHistoFunc = new histoFunc((TH1F*)(h_convolution));
	TF1* f_convolution = new TF1("f_convolution", convolutionHistoFunc, min, max, 3, "histoFunc");
	f_convolution -> FixParameter(0, 1);
	f_convolution -> FixParameter(1, 1);
	f_convolution -> FixParameter(2, 0.);



	int nBins = h_template->GetNbinsX();
	float xMin = h_template->GetBinLowEdge(1);
	float xMax = h_template->GetBinLowEdge(nBins) + h_template->GetBinWidth(nBins);
	float xWidth = (xMax - xMin) / nBins;
	TH1F* h_final = new TH1F(name.c_str(), "", nBins, xMin, xMax);

	for(int bin = 1; bin <= nBins; ++bin) {
		float xMinBin = h_final->GetBinLowEdge(bin);
		float xMaxBin = h_final->GetBinLowEdge(bin) + xWidth;

		h_final -> SetBinContent(bin, f_convolution->Integral(xMinBin, xMaxBin) / xWidth);
	}

	h_final -> Scale(h_template->Integral() / h_final->Integral());
	//h_final -> Write();



	f_template -> Delete();
	f_smearing -> Delete();

	h_func1_FFT -> Delete();
	h_func1 -> Delete();

	h_func2_FFT -> Delete();
	h_func2 -> Delete();

	h_convolution_temp -> Delete();
	h_convolution      -> Delete();

	f_convolution -> Delete();
	delete convolutionHistoFunc;



	return h_final;
}
