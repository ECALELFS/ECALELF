/****************************************************************************
 * class defining a Cruijff function as pdf
 *
 */


#include "../interface/Cruijff_pdf_class.hh"

//#define DEBUG


Cruijff_pdf_class::~Cruijff_pdf_class(void)
{

}

Cruijff_pdf_class::Cruijff_pdf_class(RooRealVar& invMass_, double mRes_, TString name, TString title):

	M("M_{Z}",   "",  invMass_.getVal(), invMass_.getMin(), invMass_.getMax(), "GeV/c^{2}"), // potrebbe avere un range piu' stretto
	sigma_L(  "\\sigma_{L}", "Width left side",  2.14, 0.,  10.0, "GeV/c^{2}"),
	sigma_R(  "\\sigma_{R}", "Width right side", 2.14, 0.,  10.0, "GeV/c^{2}"),

	alpha_L(  "\\alpha_{L}", "tail left side",  0.2, 0.,  1., ""),
	alpha_R(  "\\alpha_{R}", "tail right side", 0.2, 0.,  1., ""),

	//--------------- pdf totale signal
	// convoluzione della Breit-Wigner e della risoluzione sperimentale
	cruijff_pdf(name, title, invMass_, M, sigma_L, sigma_R, alpha_L, alpha_R),
	mRes(mRes_),
	pdf(cruijff_pdf),
	params(M, sigma_L, sigma_R, alpha_L, alpha_R)

{

	return;
}

// Cruijff_pdf_class::Cruijff_pdf_class(const Cruijff_pdf_class& other, const char* name){
// }



std::pair<double, double> Cruijff_pdf_class::GetDeltaM(void)
{
#ifdef DEBUG
	M.Print();
#endif
	return std::pair<double, double>(M.getVal() - mRes, M.getError());
}

std::pair<double, double> Cruijff_pdf_class::GetWidth(void)
{
#ifdef DEBUG
	sigma_L.Print();
	sigma_R.Print();
#endif
	return std::pair<double, double>(0.5 * (sigma_L.getVal() + sigma_R.getVal()),
	                                 0.5 * sqrt(
	                                     sigma_L.getError() * sigma_L.getError() +
	                                     sigma_R.getError() * sigma_R.getError()
	                                 )
	                                );
}

std::pair<double, double> Cruijff_pdf_class::GetDeltaM(RooFitResult *fitres)
{
	RooRealVar *var = (RooRealVar *) fitres->floatParsFinal().find(M.GetName());
	if(var == NULL) var = &M;
	return std::pair<double, double>(var->getVal() - mRes, var->getError());
}

std::pair<double, double> Cruijff_pdf_class::GetWidth(RooFitResult *fitres)
{
	RooRealVar *var_L = (RooRealVar *) fitres->floatParsFinal().find(sigma_L.GetName());
	if(var_L == NULL) var_L = &sigma_L;
	RooRealVar *var_R = (RooRealVar *) fitres->floatParsFinal().find(sigma_R.GetName());
	if(var_R == NULL) var_R = &sigma_R;

	return std::pair<double, double>(0.5 * (var_L->getVal() + var_R->getVal()),
	                                 0.5 * sqrt(
	                                     var_L->getError() * var_L->getError() +
	                                     var_R->getError() * var_R->getError()
	                                 )
	                                );
}


void Cruijff_pdf_class::SetFitType(int _fit_type)
{
	switch(_fit_type) {
	case 0: // all floating
		std::cout << "[INFO] All parameters floating" << std::endl;
		alpha_L.setConstant(kFALSE);
		alpha_R.setConstant(kFALSE);
		break;
	case 1: // fixed tail
		alpha_L.setConstant();
		alpha_R.setConstant();
		break;
	default:
		alpha_L.setConstant();
		alpha_R.setConstant();
		break;
	}
	return;
}

void Cruijff_pdf_class::InitFitValues(void)
{
	M.setVal(91.188);
	sigma_L.setVal(1);
	sigma_R.setVal(1);
	alpha_L.setVal(0.2);
	alpha_R.setVal(0.2);
	return;
}

