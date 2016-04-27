/*****************************************************************************
 * Shervin                                                                   *
 *****************************************************************************/
#ifndef ROO_CRUIJFF
#define ROO_CRUIJFF

//#ifndef __CINT__
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
//#endif

//class RooRealVar;

class RooCruijff : public RooAbsPdf
{
public:
	//  RooCruijff() {} ;
	RooCruijff(const char *name, const char *title,
	           RooAbsReal& _x, RooAbsReal& _mean,
	           RooAbsReal& _sigmaL, RooAbsReal& _sigmaR,
	           RooAbsReal& _alphaL, RooAbsReal& _alphaR
	          );
	RooCruijff(const RooCruijff& other, const char* name = 0) ;
	virtual TObject* clone(const char* newname) const
	{
		return new RooCruijff(*this, newname);
	}
	inline virtual ~RooCruijff() { }

	//  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
	//  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

	//  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const;
	//  void generateEvent(Int_t code);

protected:

	RooRealProxy x ;
	RooRealProxy mean ;
	RooRealProxy sigmaL ;
	RooRealProxy sigmaR;
	RooRealProxy alphaL;
	RooRealProxy alphaR;

	Double_t evaluate() const ;

	//private:
	// questo da' errore, capire perche' e cosa manca
	//ClassDef(RooCruijff,1) // Cruijff PDF
};

#endif
