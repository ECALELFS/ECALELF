#include "../interface/BW_CB_pdf_class.hh"

BW_CB_pdf_class::~BW_CB_pdf_class(void)
{

}

BW_CB_pdf_class::BW_CB_pdf_class(RooRealVar& invMass_, TString name, TString title):
	//  RooAbsPdf(name.Data(),title.Data()),
	//  invMass(invMass_.GetName(), invMass_.GetTitle(), this, invMass_),
	//  RooFFTConvPdf(name.Data(),title.Data(), invMass_, bw_pdf,resCB),
	deltaM("\\Delta m",   "Bias",  -1.452, -35, 10, "GeV/c^{2}"),
	sigma(  "\\sigma_{CB}", "Width", 2.14, 0.,  10.0, "GeV/c^{2}"),
	cut(    "\\alpha",     "Cut",   1.424, 0.01, 5.0),
	power(  "\\gamma",     "Power", 1.86, 0.5, 100.0),

	mRes("M_{Z^{0}}", "Z0 Resonance  Mass", 91.188, 85.0, 95.0, "GeV/c^{2}"),
	Gamma("Gamma", "Gamma", 2.49, 2.0, 3.0, "GeV/c^{2}"),

	//--------------- pdf totale signal
	// convoluzione della Breit-Wigner e della risoluzione sperimentale
	bw_pdf("bw", "A Breit-Wigner Distribution", invMass_, mRes, Gamma),
	resCB("resCB", "A  Crystal Ball Lineshape", invMass_, deltaM, sigma, cut, power),
	conv_pdf(name, title, invMass_, bw_pdf, resCB),

	pdf(conv_pdf),
	params(deltaM, sigma, cut, power)
{

	mRes.setConstant();  // fissata al PDG
	Gamma.setConstant(); // fissata al PDG

	return;
}

// BW_CB_pdf_class::BW_CB_pdf_class(const BW_CB_pdf_class& other, const char* name){
// }


// RooAbsPdf& BW_CB_pdf_class::GetPdf(void){
//   return pdf;
// }

std::pair<double, double> BW_CB_pdf_class::GetDeltaM(void)
{
	return std::make_pair(deltaM.getVal(), deltaM.getError());
}

std::pair<double, double> BW_CB_pdf_class::GetWidth(void)
{
	return std::make_pair(sigma.getVal(), sigma.getError());
}

std::pair<double, double> BW_CB_pdf_class::GetDeltaM(RooFitResult *fitres)
{
	RooRealVar *var = (RooRealVar *) fitres->floatParsFinal().find(deltaM.GetName());
	if(var == NULL) var = &deltaM;

	return std::make_pair(var->getVal(), var->getError());
}

std::pair<double, double> BW_CB_pdf_class::GetWidth(RooFitResult *fitres)
{
	RooRealVar *var = (RooRealVar *) fitres->floatParsFinal().find(sigma.GetName());
	if(var == NULL) var = &sigma;
	return std::make_pair(var->getVal(), var->getError());
}

void BW_CB_pdf_class::SetFitType(int _fit_type)
{
	switch(_fit_type) {
	case 0: // all floating
		std::cout << "[INFO] All parameters floating" << std::endl;
		cut.setConstant(kFALSE);
		power.setConstant(kFALSE);
		break;
	case 1: // fixed tail
		cut.setConstant();
		power.setConstant();
		break;
	default:
		cut.setConstant();
		power.setConstant();
		break;
	}
	return;
}

void BW_CB_pdf_class::InitFitValues(void)
{
	power.setVal(1.86);
	cut.setVal(1.424);
	deltaM.setVal(-1.452);
	sigma.setVal(2.14);
	return;
}
