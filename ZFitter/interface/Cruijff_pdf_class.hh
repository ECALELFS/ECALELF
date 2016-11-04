#ifndef Cruijff_pdf_hh
#define Cruijff_pdf_hh


/**\class Cruijff_pdf_class Cruijff_pdf_class.cc Calibration/ZFitter/interface/Cruijff_pdf_class.hh
   \brief Class providing a RooPdf corresponding to a Cruijff function (Gaussian with exponential tails)
   \author Shervin Nourbakhsh
*/

/* \todo
 */



// roofit headers
#include <RooRealVar.h>
#include <RooDataSet.h>
#include "RooCruijff.hh"
#include <RooFitResult.h>

// da rinominare come ConvBwCbPdf
class Cruijff_pdf_class
{


protected:
	RooRealProxy invMass;

private:
	//public:
	//---------- Cruijff parameters
	RooRealVar 	M;
	RooRealVar 	sigma_L;
	RooRealVar 	sigma_R;
	RooRealVar 	alpha_L;
	RooRealVar 	alpha_R;

	// PDF
	RooCruijff cruijff_pdf;

	double mRes; // massa della risonanza
public:
	Cruijff_pdf_class(RooRealVar& invMass_, double mRes = 91.188, TString name = "cruijff_pdf", TString title = "Cruijff function pdf");

	//copy constructor
	//  Cruijff_pdf_class(const Cruijff_pdf_class& other, const char* name=0);
	//virtual TObject* clone(const char* newname) const { return new Cruijff_pdf_class(*this,newname);};
	~Cruijff_pdf_class(void);

public:
	RooAbsPdf& pdf;
	RooArgSet params;
	inline  RooAbsPdf& GetPdf(void)
	{
		return pdf;
	};
	inline  RooArgSet* GetParams(void)
	{
		return &params;
	};
	void SetFitType(int value);
	void InitFitValues(void);

	std::pair<double, double> GetDeltaM(void);
	std::pair<double, double> GetWidth(void);
	std::pair<double, double> GetDeltaM(RooFitResult *fitres);
	std::pair<double, double> GetWidth( RooFitResult *fitres);

	//private:
	//  ClassDef(Cruijff_pdf_class, 0) // Breit-Wigner and Crystal Ball convoluted pdf
};



#endif
