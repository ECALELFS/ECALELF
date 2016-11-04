#ifndef BW_CB_pdf_hh
#define BW_CB_pdf_hh


/**\class BW_CB_pdf_class BW_CB_pdf_class.cc Calibration/ZFitter/interface/BW_CB_pdf_class.hh
   \brief Breit-Wigner convoluted with Crystal Ball function PDF
   \author Shervin Nourbakhsh

   \detail
	Class providing a RooPdf correspoding to the convolution of a
	Breit-Wigner and Crystal Ball
*/

/* \todo
 */



// roofit headers
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooCBShape.h>
#include <RooBreitWigner.h>
#include <RooFFTConvPdf.h>
#include <RooFitResult.h>

// da rinominare come ConvBwCbPdf
class BW_CB_pdf_class
{


protected:
	RooRealProxy invMass;

private:
	//public:
	//---------- CB parameters
	RooRealVar 	deltaM;
	RooRealVar 	sigma;
	RooRealVar 	cut;
	RooRealVar 	power;

	//--------------- BW parameters
	RooRealVar  mRes;
	RooRealVar  Gamma;

	//  RooRealVar& invMass_ref;
	RooBreitWigner bw_pdf;
	RooCBShape resCB;

	// convolution
	RooFFTConvPdf conv_pdf;

public:
	//  BW_CB_pdf_class(TString invMass_VarName, double invMass_min, double invMass_max);
	BW_CB_pdf_class(RooRealVar& invMass_, TString name = "bw_res", TString title = "BW and CB convoluted pdf");

	//copy constructor
	//  BW_CB_pdf_class(const BW_CB_pdf_class& other, const char* name=0);
	//virtual TObject* clone(const char* newname) const { return new BW_CB_pdf_class(*this,newname);};
	~BW_CB_pdf_class(void);

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
	void SetFitType(int value); ///< 0=all floating, 1=fixed tail

	void InitFitValues(void);
	std::pair<double, double> GetDeltaM(void);
	std::pair<double, double> GetWidth(void);
	std::pair<double, double> GetDeltaM(RooFitResult *fitres);
	std::pair<double, double> GetWidth( RooFitResult *fitres);

	//private:
	//  ClassDef(BW_CB_pdf_class, 0) // Breit-Wigner and Crystal Ball convoluted pdf
};



#endif
