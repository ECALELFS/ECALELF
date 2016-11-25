// Local containment correction vs local eta of basic clusters
// --- correction function derived from E/p data
// --- caveat : E/p realtive scale vs local phi depends on (eta*charge).
//     this is an average correction for all R9 and all (eta*charge) electrons

#include "TF1.h"

TF1 *f = new TF1("f", "[0]+([1]*(x)-[2]*pow(x,2))", -1, 1.);

double BasicClusterLocalContainmentCorrectionPhi (float localPhi, int imod)
{

	// --- correction function derived from E/p data

	// pol2
	if (imod == 0) f->SetParameters(1.00403, -0.0012733 , 0.042925);
	if (imod == 1) f->SetParameters(1.00394, -0.00137567 , 0.0416698);
	if (imod == 2) f->SetParameters(1.00298, -0.00111589 , 0.0320377);
	if (imod == 3)f->SetParameters(1.00269, -0.00153347 , 0.0296769);

	double corr = f-> Eval(localPhi);
	return(1. / corr);

}

