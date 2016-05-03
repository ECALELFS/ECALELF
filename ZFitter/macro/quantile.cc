#include "TH1F.h"
#include <iostream>

using namespace std;
void quantiles(TH1F* h, int nq = 5)
{
	// demo for quantiles
	Double_t xq[nq];  // position where to compute the quantiles in [0,1]
	Double_t yq[nq];  // array to contain the quantiles
	for (Int_t i = 0; i < nq; i++) xq[i] = Float_t(i + 1) / nq;
	h->GetQuantiles(nq, yq, xq);

	for (Int_t i = 0; i < nq; i++) {
		cout << "quantile at: " << xq[i] << " value: " << yq[i] << endl;
	}
}
