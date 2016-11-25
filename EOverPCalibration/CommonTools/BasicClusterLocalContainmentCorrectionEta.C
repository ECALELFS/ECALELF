// Local containment correction vs local eta of basic clusters
// --- correction function derived from E/p data
// --- tested both pol2 and plo4 --> very similar results

#include "TF1.h"

//TF1 *f = new TF1("f","[0]+([1]*(x)-[2]*pow(x,2)-[3]*pow(x,3)-[4]*pow(x,4))",-1,1.);
TF1 *f = new TF1("f", "[0]+([1]*(x)-[2]*pow(x,2))", -1, 1.);

double BasicClusterLocalContainmentCorrectionEta (float localEta, int imod)
{

	// pol4
	// if (imod==0) f->SetParameters(1.00613,0.0049502 , 0.0677101 , 0.0148299 , 0.00523093);
	// if (imod==1) f->SetParameters(1.00633,0.00403076 , 0.0689901 , 0.00242213 , 0.0239275);
	// if (imod==2) f->SetParameters(1.00648,0.0111289 , 0.0878569 , 0.0299266 , -0.0597349);
	// if (imod==3) f->SetParameters(1.01058,0.0135225 , 0.16731 , 0.0208505 , -0.21014);

	// pol2
	if (imod == 0) f->SetParameters(1.00603, 0.00300789 , 0.0667232);
	if (imod == 1) f->SetParameters(1.00655, 0.00386189 , 0.073931);
	if (imod == 2) f->SetParameters(1.00634, 0.00631341 , 0.0764134);
	if (imod == 3) f->SetParameters(1.00957, 0.0113306 , 0.123808);


	double corr = f-> Eval(localEta);
	return(1. / corr);

}

