#include <RooStats/MCMCIntervalPlot.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TMatrixDSym.h>
#include <TCanvas.h>
#include <RooMultiVarGaussian.h>

#include <TCut.h>
#include <TEntryList.h>
#include <TH2F.h>
#include <TH1F.h>

#include <RooRealVar.h>
#include <RooArgSet.h>
#include <RooDataHist.h>
#include <RooHistPdf.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>

#include <iostream>
#include <TFile.h>
#include <TString.h>

#include <TH1F.h>
#include <TH2F.h>
#include <TH2D.h>
#include <map>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TList.h>
#include <TObject.h>
#include <TKey.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TGaxis.h>
#include <iomanip>
#include <TList.h>

#include <Roo2DKeysPdf.h>

#include <TH2F.h>
#include <iostream>

Double_t getMinimumFromTree(TTree *tree, TString minimumVar, TString returnVar)
{
	Double_t var, minVar;
	Double_t min = 1e20, minimum;

	tree->SetBranchAddress(returnVar, &var);
	tree->SetBranchAddress(minimumVar, &minVar);

	tree->SetBranchStatus("*", 0);
	tree->SetBranchStatus(returnVar, 1);
	tree->SetBranchStatus(minimumVar, 1);

	tree->GetEntries();
	for(Long64_t jentry = 0; jentry < tree->GetEntriesFast(); jentry++) {
		tree->GetEntry(jentry);
		//std::cout << returnVar << "\t" << min << "\t" << minVar << "\t" << minimum << "\t" << var << std::endl;
		if(minVar < min) {
			min = minVar;
			minimum = var;
		}
	}
	//std::cout << "---> minimum = " << minimum << std::endl;
	tree->SetBranchStatus("*", 1);
	tree->ResetBranchAddresses();
	return minimum;
}

RooDataSet *th22dataset(TH2F *hist)
{
	RooRealVar *v1 = new RooRealVar("constTerm", "", 0.01, 0, 0.02);
	RooRealVar *v2 = new RooRealVar("alpha", "", 0.0, 0, 0.2);
	RooRealVar *nll = new RooRealVar("nll", 0, 0, 1e20);
	RooArgSet argSet(*v1, *v2, *nll);
	RooDataSet *dataset = new RooDataSet(TString(hist->GetName()) + "_dataset", "", argSet, "nll");

	for(Int_t iBinX = 1; iBinX <= hist->GetNbinsX(); iBinX++) {
		for(Int_t iBinY = 1; iBinY <= hist->GetNbinsY(); iBinY++) {
			Double_t binContent = hist->GetBinContent(iBinX, iBinY);
			if(binContent != 0) {
				v1->setVal(hist->GetXaxis()->GetBinCenter(iBinX));
				v2->setVal(hist->GetYaxis()->GetBinCenter(iBinY));
				nll->setVal(binContent);
				dataset->add(argSet);
			}
		}
	}
	return dataset;
}

Roo2DKeysPdf *SmoothKeys(TH2F *h)
{
	RooDataSet *dataset = th22dataset(h);
	RooRealVar *v1 = (RooRealVar *) dataset->get()->find("constTerm");
	RooRealVar *v2 =  (RooRealVar *) dataset->get()->find("alpha");


	Roo2DKeysPdf *myPdf = new Roo2DKeysPdf("mypdf", "", *v1, *v2, *dataset);
	return myPdf;

}
Roo2DKeysPdf *Smooth(TH2F *h, Int_t ntimes, Option_t *option)
{

	// Smooth bin contents of this 2-d histogram using kernel algorithms
	// similar to the ones used in the raster graphics community.
	// Bin contents in the active range are replaced by their smooth values.
	// If Errors are defined via Sumw2, they are scaled.
	// 3 kernels are proposed k5a, k5b and k3a.
	// k5a and k5b act on 5x5 cells (i-2,i-1,i,i+1,i+2, and same for j)
	// k5b is a bit more stronger in smoothing
	// k3a acts only on 3x3 cells (i-1,i,i+1, and same for j).
	// By default the kernel "k5a" is used. You can select the kernels "k5b" or "k3a"
	// via the option argument.
	// If TAxis::SetRange has been called on the x or/and y axis, only the bins
	// in the specified range are smoothed.
	// In the current implementation if the first argument is not used (default value=1).
	//
	// implementation by David McKee (dmckee@bama.ua.edu). Extended by Rene Brun

	Double_t k5a[5][5] =  { { 0, 0, 1, 0, 0 },
		{ 0, 2, 2, 2, 0 },
		{ 1, 2, 5, 2, 1 },
		{ 0, 2, 2, 2, 0 },
		{ 0, 0, 1, 0, 0 }
	};
	Double_t k5b[5][5] =  { { 0, 1, 2, 1, 0 },
		{ 1, 2, 4, 2, 1 },
		{ 2, 4, 8, 4, 2 },
		{ 1, 2, 4, 2, 1 },
		{ 0, 1, 2, 1, 0 }
	};
	Double_t k3a[3][3] =  { { 0, 1, 0 },
		{ 1, 2, 1 },
		{ 0, 1, 0 }
	};

	//  if (ntimes > 1) {
	// Warning("Smooth","Currently only ntimes=1 is supported");
	//   }
	TString opt = option;
	opt.ToLower();
	if(opt.Contains("keys")) return SmoothKeys(h);

	Int_t ksize_x = 5;
	Int_t ksize_y = 5;
	Double_t *kernel = &k5a[0][0];
	if (opt.Contains("k5b")) kernel = &k5b[0][0];
	if (opt.Contains("k3a")) {
		kernel = &k3a[0][0];
		ksize_x = 3;
		ksize_y = 3;
	}

	// find i,j ranges
	Int_t ifirst = h->GetXaxis()->GetFirst();
	Int_t ilast  = h->GetXaxis()->GetLast();
	Int_t jfirst = h->GetYaxis()->GetFirst();
	Int_t jlast  = h->GetYaxis()->GetLast();

	// Determine the size of the bin buffer(s) needed
	Double_t nentries = h->GetEntries();
	Int_t nx = h->GetNbinsX();
	Int_t ny = h->GetNbinsY();
	Int_t bufSize  = (nx + 2) * (ny + 2);
	Double_t *buf  = new Double_t[bufSize];
	Double_t *ebuf = 0;
	if (h->GetSumw2()->fN) ebuf = new Double_t[bufSize];

	// Copy all the data to the temporary buffers
	Int_t i, j, bin;
	for (i = ifirst; i <= ilast; i++) {
		for (j = jfirst; j <= jlast; j++) {
			bin = h->GetBin(i, j);
			buf[bin] = h->GetBinContent(bin);
			if (ebuf) ebuf[bin] = h->GetBinError(bin);
		}
	}

	// Kernel tail sizes (kernel sizes must be odd for this to work!)
	Int_t x_push = (ksize_x - 1) / 2;
	Int_t y_push = (ksize_y - 1) / 2;

	// main work loop
	for (i = ifirst; i <= ilast; i++) {
		for (j = jfirst; j <= jlast; j++) {
			Double_t content = 0.0;
			Double_t error = 0.0;
			Double_t norm = 0.0;
			if(h->GetBinContent(i, j) <= 0) continue;
			for (Int_t n = 0; n < ksize_x; n++) {
				for (Int_t m = 0; m < ksize_y; m++) {
					Int_t xb = i + (n - x_push);
					Int_t yb = j + (m - y_push);
					if ( (xb >= 1) && (xb <= nx) && (yb >= 1) && (yb <= ny)) {
						bin = h->GetBin(xb, yb);
						if(buf[bin] > 0) {
							//if(xb>9 && xb < 12) std::cout << bin << "\t" << xb << "\t" << yb << "\t" << buf[bin] << "\t" << h->GetBinContent(xb,yb) << std::endl;
							Double_t k = kernel[n * ksize_y + m];
							//if ( (k != 0.0 ) && (buf[bin] != 0.0) ) { // General version probably does not want the second condition
							if ( k != 0.0 ) {
								norm    += k;
								content += k * buf[bin];
								if (ebuf) error   += k * k * buf[bin] * buf[bin];
							}
						}
					}
				}
			}

			if ( norm != 0.0 ) {
				h->SetBinContent(i, j, content / norm);
				if (ebuf) {
					error /= (norm * norm);
					h->SetBinError(i, j, sqrt(error));
				}
			}
		}
	}
	//fEntries = nentries;

	delete [] buf;
	delete [] ebuf;
}


TGraph* bestFit(TTree *t, TString x, TString y, TString nll)
{
	t->Draw(y + ":" + x, nll + " == 0");
	TGraph *gr0 = (TGraph*) gROOT->FindObject("Graph")->Clone();
	gr0->SetMarkerStyle(34);
	gr0->SetMarkerSize(2.0);
	if (gr0->GetN() > 1) gr0->Set(1);
	return gr0;
}

TH2D* frameTH2D(TH2D *in)
{

	Double_t xw = in->GetXaxis()->GetBinWidth(0);
	Double_t yw = in->GetYaxis()->GetBinWidth(0);

	Int_t nx = in->GetNbinsX();
	Int_t ny = in->GetNbinsY();

	Double_t x0 = in->GetXaxis()->GetXmin();
	Double_t x1 = in->GetXaxis()->GetXmax();

	Double_t y0 = in->GetYaxis()->GetXmin();
	Double_t y1 = in->GetYaxis()->GetXmax();

	TH2D *framed = new TH2D(
	    Form("%s framed", in->GetName()),
	    Form("%s framed", in->GetTitle()),
	    nx + 2, x0 - xw, x1 + xw,
	    ny + 2, y0 - yw, y1 + yw
	);

	//Copy over the contents
	for(int ix = 1; ix <= nx ; ix++) {
		for(int iy = 1; iy <= ny ; iy++) {
			framed->SetBinContent(1 + ix, 1 + iy, in->GetBinContent(ix, iy));
		}
	}
	//Frame with huge values
	nx = framed->GetNbinsX();
	ny = framed->GetNbinsY();
	for(int ix = 1; ix <= nx ; ix++) {
		framed->SetBinContent(ix,  1, 1000.);
		framed->SetBinContent(ix, ny, 1000.);
	}
	for(int iy = 2; iy <= ny - 1 ; iy++) {
		framed->SetBinContent( 1, iy, 1000.);
		framed->SetBinContent(nx, iy, 1000.);
	}

	return framed;
}


TList* contourFromTH2(TH2 *h2in, double threshold)
{
	std::cout << "Getting contour at threshold " << threshold << " from " << h2in->GetName() << std::endl;
	//http://root.cern.ch/root/html/tutorials/hist/ContourList.C.html
	Double_t contours[1];
	contours[0] = threshold;

	TH2D *h2 = frameTH2D((TH2D*)h2in);

	h2->SetContour(1, contours);

	// Draw contours as filled regions, and Save points
	h2->Draw("CONT Z LIST");
	gPad->Update(); // Needed to force the plotting and retrieve the contours in TGraphs

	// Get Contours
	TObjArray *conts = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
	TList* contLevel = NULL;

	if (conts == NULL || conts->GetSize() == 0) {
		printf("*** No Contours Were Extracted!\n");
		return 0;
	}

	TList *ret = new TList();
	for(int i = 0; i < conts->GetSize(); i++) {
		contLevel = (TList*)conts->At(i);
		printf("Contour %d has %d Graphs\n", i, contLevel->GetSize());
		for (int j = 0, n = contLevel->GetSize(); j < n; ++j) {
			TGraph *gr1 = (TGraph*) contLevel->At(j)->Clone();
			ret->Add(gr1);
		}
	}
	return ret;
}


TGraphErrors g(TTree *genTree, TString constTermName = "")
{
	Double_t alpha, constTerm;
	genTree->SetBranchAddress("alpha", &alpha);
	genTree->SetBranchAddress("constTerm", &constTerm);

	Long64_t nEntries = genTree->GetEntries();

	TH1F h("smearingHist", "", 10000, 0, 0.1);
	TGraphErrors graph;
	Int_t iPoint = 0;
	for(Double_t energy = 20; energy < 150; energy += 10) {
		h.Reset();
		for(Long64_t jentry = 0; jentry < nEntries; jentry++) {
			genTree->GetEntry(jentry);
			h.Fill(sqrt(alpha * alpha / energy + constTerm * constTerm));
		}
		graph.SetPoint(iPoint, energy, h.GetMean());
		graph.SetPointError(iPoint, 0, h.GetRMS());
		iPoint++;
	}
	h.SetTitle(constTermName);
	h.SaveAs("tmp/h-" + constTermName + ".root");
	graph.Set(iPoint);
	genTree->ResetBranchAddresses();
	graph.Draw("A L3");
	graph.SetFillColor(kBlue);
	graph.SetLineColor(kYellow);
	graph.GetXaxis()->SetTitle("Energy [GeV]");
	graph.GetYaxis()->SetTitle("Additional smearing [%]");
	return graph;
}

TGraphErrors g(TTree *tree, TString alphaName, TString constTermName)
{
	Double_t alpha, constTerm;
	alphaName.ReplaceAll("-", "_");
	constTermName.ReplaceAll("-", "_");

	tree->SetBranchAddress(alphaName, &alpha);
	tree->SetBranchAddress(constTermName, &constTerm);

	//Long64_t nEntries=genTree->GetEntries();

	TGraphErrors graph;
	Int_t iPoint = 0;

	tree->GetEntry(0);
	//std::cout << alpha << "\t" << constTerm << std::endl;
	Double_t alpha2 = alpha * alpha;
	Double_t const2 = constTerm * constTerm;
	for(Double_t energy = 20; energy < 150; energy += 10) {
		Double_t addSmearing = (sqrt(alpha2 / energy + const2));
		//std::cout << alpha << "\t" << constTerm << "\t" << addSmearing << std::endl;
		graph.SetPoint(iPoint, energy, addSmearing);
		graph.SetPointError(iPoint, 0, 0);
		iPoint++;
	}

	graph.Set(iPoint);
	tree->ResetBranchAddresses();
	graph.Draw("A L");
	graph.SetFillColor(kBlue);
	graph.SetLineColor(kYellow);
	graph.GetXaxis()->SetTitle("Energy [GeV]");
	graph.GetYaxis()->SetTitle("Additional smearing [%]");
	return graph;
}

TGraphErrors g(Double_t alpha, Double_t constTerm)
{
	TGraphErrors graph;
	Int_t iPoint = 0;

	//  std::cout << alpha << "\t" << constTerm << std::endl;
	Double_t alpha2 = alpha * alpha;
	Double_t const2 = constTerm * constTerm;
	for(Double_t energy = 20; energy < 150; energy += 10) {
		Double_t addSmearing = (sqrt(alpha2 / energy + const2));

		graph.SetPoint(iPoint, energy, addSmearing);
		graph.SetPointError(iPoint, 0, 0);
		iPoint++;
	}

	graph.Set(iPoint);
	graph.Draw("A L");
	graph.SetFillColor(kBlue);
	graph.SetLineColor(kYellow);
	graph.GetXaxis()->SetTitle("Energy [GeV]");
	graph.GetYaxis()->SetTitle("Additional smearing [%]");
	return graph;
}

TGraphErrors bestFit(TTree *tree, TString alphaName, TString constTermName)
{
	Double_t alpha, constTerm;
	alphaName.ReplaceAll("-", "_");
	constTermName.ReplaceAll("-", "_");

	tree->SetBranchAddress(alphaName, &alpha);
	tree->SetBranchAddress(constTermName, &constTerm);

	//Long64_t nEntries=genTree->GetEntries();

	TGraphErrors graph;
	Int_t iPoint = 0;

	tree->GetEntry(0);
	graph.SetPoint(0, constTerm, alpha);
	graph.SetPointError(0, 0, 0);
	iPoint++;

	graph.Set(iPoint);
	tree->ResetBranchAddresses();
	graph.Draw("A P");
//   graph.SetFillColor(kBlue);
//   graph.SetLineColor(kYellow);
//   graph.GetXaxis()->SetTitle("Energy [GeV]");
//   graph.GetYaxis()->SetTitle("Additional smearing [%]");
	return graph;
}

RooHistPdf *nllToL(TH2F* hist)
{
	TH2F *h = (TH2F*) hist->Clone(TString(hist->GetName()) + "_L");

	h->Reset();
	Double_t min = 1e20, max = 0;
	for(Int_t iBinX = 1; iBinX <= hist->GetNbinsX(); iBinX++) {
		for(Int_t iBinY = 1; iBinY <= hist->GetNbinsY(); iBinY++) {
			Double_t binContent = hist->GetBinContent(iBinX, iBinY);
			if(min > binContent && binContent != 0) min = binContent;
			if(max < binContent) max = binContent;
		}
	}

	for(Int_t iBinX = 1; iBinX <= hist->GetNbinsX(); iBinX++) {
		for(Int_t iBinY = 1; iBinY <= hist->GetNbinsY(); iBinY++) {
			Double_t binContent = hist->GetBinContent(iBinX, iBinY);
			Double_t b = binContent <= 0 ? 0 : exp(-binContent + min + 50);
			//      if(binContent != 0 && binContent-min<100) std::cout << iBinX << "\t" << iBinY << "\t" << binContent << "\t" << -binContent+min << "\t" << b << std::endl;
			//h->Fill(hist->GetXaxis()->GetBinLowEdge(iBinX), hist->GetYaxis()->GetBinLowEdge(iBinY),b);
			h->SetBinContent(iBinX, iBinY, b);
		}
	}


	RooRealVar *var1 = new RooRealVar("constTerm", "", 0.1);
	RooRealVar *var2 = new RooRealVar("alpha", "", 0.1);

	RooDataHist *dataHist = new RooDataHist(TString(hist->GetName()) + "_dataHist", hist->GetTitle(), RooArgSet(*var1, *var2), h);
	RooHistPdf *histPdf = new RooHistPdf(TString(hist->GetName()) + "_histPdf", hist->GetTitle(), RooArgSet(*var1, *var2), *dataHist);

	//delete dataHist;
	return histPdf;
}

TH2F *prof2d(TTree *tree, TString var1Name, TString var2Name, TString nllName, TString binning = "(241,-0.0005,0.2405,60,0.00025,0.03025)", bool delta = false, int nSmooth = 0, TString optSmooth = "k3a")
{

	var1Name.ReplaceAll("-", "_");
	var2Name.ReplaceAll("-", "_");
	//  tree->Print();
	TString histName = "h";
	//std::cout << var1Name << "\t" << var2Name << "\t" << histName << std::endl;
	tree->Draw(var1Name + ":" + var2Name + ">>" + histName + binning);
	TH2F *hEntries = (TH2F*)gROOT->FindObject(histName);
	if(hEntries == NULL) return NULL;
	//std::cerr << "e qui ci sono?" << std::endl;
	tree->Draw(var1Name + ":" + var2Name + ">>shervin" + binning, nllName);

	TH2F *h = (TH2F*)gROOT->FindObject("shervin");
	if(h == NULL) return NULL;
	h->Divide(hEntries);


	//std::cerr << "io sono qui" << std::endl;
	delete hEntries;
	Double_t min = 1e20, max = 0;

	if(nSmooth > 0) {
		for(int iSmooth = 0; iSmooth < nSmooth; iSmooth++) {
			Smooth(h, 1, optSmooth);
		}
	}

	Int_t iBinXmin = 0, iBinYmin = 0;
	if(delta) {
		for(Int_t iBinX = 1; iBinX <= h->GetNbinsX(); iBinX++) {
			for(Int_t iBinY = 1; iBinY <= h->GetNbinsY(); iBinY++) {
				Double_t binContent = h->GetBinContent(iBinX, iBinY);
				if(min > binContent && binContent != 0) {
					min = binContent;
					iBinXmin = iBinX;
					iBinYmin = iBinY;
				}
				if(max < binContent) max = binContent;
			}
		}
		//std::cout << "min=" << min << "\tmax=" << max<<std::endl;
		for(Int_t iBinX = 1; iBinX <= h->GetNbinsX(); iBinX++) {
			for(Int_t iBinY = 1; iBinY <= h->GetNbinsY(); iBinY++) {
				Double_t binContent = h->GetBinContent(iBinX, iBinY);
				//std::cout << binContent << std::endl;
				if(binContent == 0 && iBinX > 1 && h->GetBinContent(iBinX - 1, iBinY) != 0 ) {
					binContent = h->GetBinContent(iBinX - 1, iBinY);
				}
				if(binContent != 0) binContent -= min - 0.0002;
				else binContent = -1;
				h->SetBinContent(iBinX, iBinY, binContent);
			}
		}
	}
//   std::cout << "iBinXmin = " << iBinXmin
// 	    << "\tiBinYmin = " << iBinYmin
// 	    << "\tminBin content = " << h->GetBinContent(iBinXmin, iBinYmin)
//     //	    << "\tminX = h->GetXaxis()->GetBinCenter(
// 	    << std::endl;
	h->GetZaxis()->SetRangeUser(0.000001, 50);
	//std::cerr << "io sono qui 3" << std::endl;
	return h;
//   Double_t variables[2];
//   Double_t nll;
//   tree->SetBranchAddress(var1Name, &(variables[0]));
//   tree->SetBranchAddress(var2Name, &(variables[1]));
//   tree->SetBranchAddress(nllName, &(nll));


//   Long64_t nEntries=tree->GetEntries();
//   for(Long64_t jentry=0; jentry<nEntries; jentry++){

}


// TMatrixDSym* GetCovariance( RooStats::MarkovChain *chain, TString var1, TString var2){

//   RooRealVar *nll = chain->GetNLLVar();
//   RooRealVar *weight= chain->GetWeightVar();
//   RooFormulaVar newWeight("f","","-@0",nll);

//   RooArgSet *args = chain->Get();
//   RooArgSet newArg;
//   RooArgSet oldArg;
//   oldArg.add(*(args->find(var1)));
//   oldArg.add(*(args->find(var2)));
//   oldArg.add(*nll);
//   newArg.add(*(args->find(var1)));
//   newArg.add(*(args->find(var2)));
//   //newArg.add(*nll);
//   //newArg.add(newWeight);

//   RooDataSet dataset("dataset","",   chain->GetAsDataSet(oldArg), RooArgSet(oldArg,newWeight), "", newWeight->GetName());
//   return dataset->covarianceMatrix(newArg);
// }

//------------------------------
TCut m(TTree *tree, TString var1, TString var2)
{
	Double_t min = tree->GetMinimum("nll_MarkovChain_local_");
	TString selMin = "nll_MarkovChain_local_ == ";
	selMin += min;

	tree->Draw(">>entrylist", selMin, "entrylist");
	TEntryList *entrylist = (TEntryList*) gROOT->FindObject("entrylist");
	if(entrylist->GetN() != 1) {
		std::cout << "Entrylist N=" << entrylist->GetN() << std::endl;
		return "";
	}
	Long64_t minEntry = entrylist->GetEntry(0);

	TObjArray *branchArray = tree->GetListOfBranches();
	branchArray->GetEntries();

	std::vector<TString> minSel_vec;

	for(int i = 0; i < branchArray->GetEntriesFast(); i++) {
		TBranch *b = (TBranch *) branchArray->At(i);
		TString name = b->GetName();
		Double_t address;
		b->SetAddress(&address);
		b->GetEntry(minEntry);
		name += "==";
		name += address;
		minSel_vec.push_back(name);
	}

	TCut cut;
	for(int i = 0; i < branchArray->GetEntriesFast(); i++) {
		TBranch *b = (TBranch *) branchArray->At(i);
		TString name = b->GetName();
		if(name.Contains("nll")) continue;
		if(name.Contains(var1)) continue;
		if(name.Contains(var2)) continue;
		cut += minSel_vec[i];
	}
	return cut;
}

RooMultiVarGaussian *MultiVarGaussian(RooDataSet *dataset)
{

	RooArgSet *args = (RooArgSet *) dataset->get()->Clone();
	//   argSet.remove(*chain->GetNLLVar(), kFALSE, kTRUE);

	RooArgSet *mu = (RooArgSet *)args->Clone();
	TMatrixDSym *matrix = dataset->covarianceMatrix();
	RooMultiVarGaussian *g = new RooMultiVarGaussian("multi", "", RooArgList(*args), RooArgList(*mu), *matrix);
	return g;



	return NULL;
}

TTree *dataset2tree(RooDataSet *dataset)
{

	const RooArgSet *args = dataset->get();
	if(args == NULL) return NULL;
	RooArgList argList(*args);

	Double_t variables[50];
	Long64_t nEntries = dataset->numEntries();
	//nEntries=1;
	TTree *tree = new TTree("tree", "tree");
	tree->SetDirectory(0);
	TIterator *it1 = NULL;
	it1 = argList.createIterator();
	int index1 = 0;
	for(RooRealVar *var = (RooRealVar *) it1->Next(); var != NULL;
	        var = (RooRealVar *) it1->Next(), index1++) {
		TString name(var->GetName());
		name.ReplaceAll("-", "_");
		tree->Branch(name, &(variables[index1]), name + "/D");
	}

	//  tree->Print();

	for(Long64_t jentry = 0; jentry < nEntries; jentry++) {

		TIterator *it = NULL;
		RooArgList argList1(*(dataset->get(jentry)));
		it = argList1.createIterator();
		//(dataset->get(jentry))->Print();
		int index = 0;
		for(RooRealVar *var = (RooRealVar *) it->Next(); var != NULL;
		        var = (RooRealVar *) it->Next(), index++) {
			variables[index] = var->getVal();
			//var->Print();
		}

		delete it;
		tree->Fill();
	}
	tree->ResetBranchAddresses();
	//  tree->Print();
	return tree;
}

void mcmcDraw(TFile *f)
{
	RooStats::MarkovChain *chain = (RooStats::MarkovChain *)f->Get("_markov_chain");
	RooStats::MCMCInterval mcInter;
	mcInter.SetChain(*chain);
	RooStats::MCMCIntervalPlot mcInterPlot(mcInter);
	mcInterPlot.DrawNLLHist();
	mcInterPlot.DrawNLLVsTime();
	TCanvas *c = (TCanvas *)gROOT->FindObject("c1");
	c -> SaveAs("tmp/NLLVsTime.png");
	c->Clear();
	mcInterPlot.Draw();
	c->SaveAs("tmp/l.png");

//   TTree *tree = (TTree *)chain->GetAsDataSet()->tree();
//   RooArgSet *args = (RooArgSet *) chain->Get()->Clone();
//   RooArgSet argSet(*chain->Get());
//   argSet.remove(*chain->GetNLLVar(), kFALSE, kTRUE);

//   TObjArray *branchList = tree->GetListOfBranches();
//   std::set<TString> branchNames;
//   for(int i=0; i < branchList->GetSize(); i++){
//     if(branchList->At(i)!=NULL){
//       TString bName(branchList->At(i)->GetName());
//       if(bName != chain->GetNLLVar()->GetName() &&
// 	 bName != chain->GetWeightVar()->GetName())
// 	branchNames.insert(bName);
//     }
//   }



//   for(std::set<TString>::const_iterator b_itr = branchNames.begin();
//       b_itr != branchNames.end();
//       b_itr++){
//     std::cout << *b_itr << std::endl;
//     //argSet.add(*(args->find(*b_itr)));
//     tree->Draw(*b_itr+">>h",TString(chain->GetWeightVar()->GetName()));
//     TH1F *h = (TH1F *) gROOT->FindObject("h");
//     TString cut=TString::Format("(100,%.2f,%.2f)",h->GetMean()-3*h->GetRMS() ,h->GetMean()+3*h->GetRMS());
//     delete h;
//     tree->Draw(*b_itr+">>h"+cut,TString(chain->GetWeightVar()->GetName()));
//     h = (TH1F *) gROOT->FindObject("h");

//     //((RooRealVar *)(args->find(*b_itr)))->setVal(
//     std::cout << *b_itr << "\t" << h->GetMean() << "\t" << h->GetRMS() << std::endl;;
//     h->Draw();
//     h->Fit("gaus");
//     c->SaveAs("tmp/"+*b_itr+".png");
//     h->SaveAs("tmp/"+*b_itr+".root");
//     //    args->find(*b_itr)
//     delete h;
//   }
//   argSet.Print();

//   argSet.writeToStream(std::cout, kFALSE);

//   TMatrixDSym *matrix = chain->GetAsDataSet()->covarianceMatrix(argSet);
//   matrix->SaveAs("tmp/matrix.root");

}

TGraphErrors plot(TFile *f, TString alpha, TString constTerm)
{
	RooStats::MarkovChain *chain = (RooStats::MarkovChain *)f->Get("_markov_chain");
	TTree *tree = dataset2tree(chain->GetAsDataSet());
	TH2F *hist = prof2d(tree, alpha, constTerm, "nll_MarkovChain_local_");
	RooHistPdf *histPdf = nllToL(hist);
	TTree *genTree = dataset2tree(histPdf->generate(*histPdf->getVariables(), 1000000, kTRUE, kFALSE));
	TGraphErrors graph = g(genTree);

	delete tree;
	delete hist;
	delete histPdf;
	delete genTree;

	return graph;

}

TGraphErrors plot(RooDataSet *dataset, TString alpha, TString constTerm)
{
	TTree *tree = dataset2tree(dataset);
	TH2F *hist = prof2d(tree, alpha, constTerm, "nll");
	RooHistPdf *histPdf = nllToL(hist);
	TTree *genTree = dataset2tree(histPdf->generate(*histPdf->getVariables(), 1000000, kTRUE, kFALSE));
	TGraphErrors graph = g(genTree);

	delete tree;
	delete hist;
	delete histPdf;
	delete genTree;

	return graph;

}


void MakePlots(TString filename, float zmax = 30, int nSmooth = 10, TString opt = "", TString energy = "8TeV", TString lumi = "")
{
	TString outDir = filename;
	outDir.Remove(outDir.Last('/'));
	outDir += "/img/" + opt;
	//outDir="tmp/k5b/";
	//std::map<TString, TH2F *> deltaNLL_map;

	/*------------------------------ Plotto */
	TCanvas *c = new TCanvas("c", "c");

	TFile f_in(filename, "read");
	if(f_in.IsZombie()) {
		std::cerr << "File opening error: " << filename << std::endl;
		return;
	}

	TList *KeyList = f_in.GetListOfKeys();
	std::cout << KeyList->GetEntries() << std::endl;
	for(int i = 0; i <  KeyList->GetEntries(); i++) {
		c->Clear();
		TKey *key = (TKey *)KeyList->At(i);
		if(TString(key->GetClassName()) != "RooDataSet") continue;
		RooDataSet *dataset = (RooDataSet *) key->ReadObj();
		if(dataset == NULL) {
			std::cerr << "[WARNING] No dataset for " << key->GetName() << "\t" << key->GetTitle() << std::endl;
			continue;
		}
		TString constTermName = dataset->GetName();
		TString alphaName = constTermName;
		alphaName.ReplaceAll("constTerm", "alpha");

		if(constTermName.Contains("absEta_1_1.4442-gold")) continue;
		if(constTermName.Contains("rho") || constTermName.Contains("phi")) continue;
		if(constTermName.Contains("scale")) continue;
		TTree *tree = dataset2tree(dataset);
		TGraphErrors bestFit_ = bestFit(tree, alphaName, constTermName);
		//    TString binning="(241,-0.0005,0.2405,60,0.00025,0.03025)";
		TString binning = "(241,-0.0005,0.2405,301,-0.00005,0.03005)";

		TH2F *hist = prof2d(tree, constTermName, alphaName, "nll", binning, true, nSmooth, opt);
//     std::cout << "Bin width = " << hist->GetXaxis()->GetBinWidth(10) << "\t" << hist->GetYaxis()->GetBinWidth(10) << std::endl;
//     std::cout << "Bin 1 center = " << hist->GetXaxis()->GetBinCenter(1) << "\t" << hist->GetYaxis()->GetBinCenter(1) << std::endl;
//     std::cout << "Bin 10 center = " << hist->GetXaxis()->GetBinCenter(10) << "\t" << hist->GetYaxis()->GetBinCenter(10) << std::endl;
//     return;
		hist->Draw("colz");
		hist->GetZaxis()->SetRangeUser(0, zmax);
		hist->GetXaxis()->SetRangeUser(0, 0.15);
		hist->GetYaxis()->SetRangeUser(0, 0.018);

		hist->GetXaxis()->SetTitle("#Delta S");
		hist->GetYaxis()->SetTitle("#Delta C");

		Int_t iBinX, iBinY;
		Double_t x, y;
		hist->GetBinWithContent2(0.0002, iBinX, iBinY, 1, -1, 1, -1, 0.0000001);
		x = hist->GetXaxis()->GetBinCenter(iBinX);
		y = hist->GetYaxis()->GetBinCenter(iBinY);
		std::cout << "Best Fit: " << x << "\t" << y << std::endl;
		TGraph nllBestFit(1, &x, &y);

		TString fileName = outDir + "/" + constTermName;
		fileName += "-";
		fileName += nSmooth;

		nllBestFit.SetMarkerStyle(3);
		nllBestFit.SetMarkerColor(kRed);
		nllBestFit.Draw("P same");

		std::cout << fileName << std::endl;
		ofstream fout(fileName + ".dat", ios_base::app);
		fout << constTermName << "\t" << x << "\t" << y << std::endl;


		c->SaveAs(fileName + ".png");
		c->SaveAs(fileName + ".eps");
		if(fileName.Contains("constTerm")) c->SaveAs(fileName + ".C");

		fileName += "-zoom";
		hist->GetZaxis()->SetRangeUser(0, 1);
		//hist->GetXaxis()->SetRangeUser(0.00,0.12);
		//hist->GetYaxis()->SetRangeUser(0,0.005);
		c->SaveAs(fileName + ".png");
		c->SaveAs(fileName + ".eps");


//     hist->SaveAs(outDir+"/deltaNLL-"+constTermName+".root");
//     hist->Draw("colz");
//     bestFit_.Draw("P same");
//     bestFit_.SetMarkerSize(2);



//     nllBestFit.SetMarkerStyle(3);
//     nllBestFit.SetMarkerColor(kRed);
//     TList* contour68 = contourFromTH2(hist, 0.68);

//     hist->Draw("colz");
//     hist->GetZaxis()->SetRangeUser(0,zmax);
//     //bestFit_.Draw("P same");
//     nllBestFit.Draw("P same");
//     //contour68->Draw("same");
		delete hist;
		RooAbsPdf *histPdf = NULL;
		if(!opt.Contains("keys")) {
			hist = prof2d(tree, alphaName, constTermName, "nll",
			              binning, false, nSmooth, opt);
			histPdf = nllToL(hist);
		} else {
			hist = prof2d(tree, alphaName, constTermName, "nll",
			              binning, false, nSmooth);
			histPdf = Smooth(hist, 1, "keys");
		}
		delete hist;


//     RooDataSet *gen_dataset=histPdf->generate(*histPdf->getVariables(),1000000,kTRUE,kFALSE);
//     TTree *genTree = dataset2tree(gen_dataset);
//     genTree->SaveAs(fileName+"-genTree.root");
//     delete gen_dataset;
//     delete histPdf;

//     TGraphErrors toyGraph = g(genTree, constTermName);
//     TGraphErrors bestFitGraph = g(tree,alphaName, constTermName);
//     TGraphErrors bestFitScanGraph = g(y, x);
//     delete genTree;
//     delete tree;
//     toyGraph.SetFillColor(kGreen);
//     toyGraph.SetLineColor(kBlue);
//     toyGraph.SetLineStyle(2);
//     bestFitGraph.SetLineColor(kBlack);
//     bestFitScanGraph.SetLineColor(kRed);
//     bestFitScanGraph.SetLineWidth(2);



//     TMultiGraph g_multi("multigraph","");
//     g_multi.Add(&toyGraph,"L3");
//     g_multi.Add(&toyGraph,"L");
//     g_multi.Add(&bestFitGraph, "L");
//     g_multi.Add(&bestFitScanGraph, "L");

//     g_multi.Draw("A");

//     c->Clear();
//     g_multi.Draw("A");
//     c->SaveAs(outDir+"/smearing_vs_energy-"+constTermName+".png");
//     c->SaveAs(outDir+"/smearing_vs_energy-"+constTermName+".eps");
//     //    TPaveText *pv = new TPaveText(0.7,0.7,1, 0.8);
// //     TLegend *legend = new TLegend(0.7,0.8,0.95,0.92);
// //     legend->SetFillStyle(3001);
// //     legend->SetFillColor(1);
// //     legend->SetTextFont(22); // 132
// //     legend->SetTextSize(0.04); // l'ho preso mettendo i punti con l'editor e poi ho ricavato il valore con il metodo GetTextSize()
// //   //  legend->SetFillColor(0); // colore di riempimento bianco
// //     legend->SetMargin(0.4);  // percentuale della larghezza del simbolo
//     //    SetLegendStyle(legend);

//     //Plot(c, data,mc,mcSmeared,legend, region, filename, energy, lumi);
	}

	f_in.Close();

	return;
}

TGraph *GetRho(TTree *t, TString alphaName, TString constTermName)
{
	t->Draw("nll:" + constTermName.ReplaceAll("-", "_"), alphaName.ReplaceAll("-", "_") + " == 0");
	TGraph *gr0 = (TGraph*) gROOT->FindObject("Graph")->Clone();
	gr0->SetMarkerStyle(34);
	gr0->SetMarkerSize(1.0);
	//if (gr0->GetN() > 1) gr0->Set(1);
	return gr0;
}


TTree *ToyTree(TString dirname = "test/dato/fitres/Hgg_Et-toys/0.01-0.00", TString fname = "outProfile-scaleStep2smearing_7-Et_25-trigger-noPF-EB.root", TString opt = "", int nSmooth = 10)
{
	TString outDir = dirname;
	outDir.ReplaceAll("fitres", "img");
	outDir = "tmp/";
	//std::map<TString, TH2F *> deltaNLL_map;

	//bool smooth=false;
	//if(opt.Contains("smooth")) smooth=true;


	/*------------------------------ Plotto */
	TCanvas c("ctoy", "c");


	TTree *toys = new TTree("toys", "");
	toys->SetDirectory(0);
	Double_t constTerm_tree, constTermTrue_tree;
	Double_t alpha_tree, alphaTrue_tree;
	char catName[100];
	Int_t catIndex;
	toys->Branch("constTerm", &constTerm_tree, "constTerm/D");
	toys->Branch("alpha", &alpha_tree, "alpha/D");
	toys->Branch("constTermTrue", &constTermTrue_tree, "constTermTrue/D");
	toys->Branch("alphaTrue", &alphaTrue_tree, "alphaTrue/D");

	toys->Branch("catName", catName, "catName/C");
	toys->Branch("catIndex", &catIndex, "catIndex/I");
	std::map<TString, Int_t> catIndexMap;

	///1/
	for(int itoy = 2; itoy <= 50; itoy++) {
		TString filename = dirname + "/";
		filename += itoy;
		filename += "/" + fname;
		TString fout = dirname + "/";
		fout += itoy;
		fout += "/";
		TFile f_in(filename, "read");
		if(f_in.IsZombie()) {
			std::cerr << "File opening error: " << filename << std::endl;
			continue; //return NULL;
		}
		//std::cout << filename << std::endl;
		TList *KeyList = f_in.GetListOfKeys();
		//std::cout << KeyList->GetEntries() << std::endl;
		for(int i = 0; i <  KeyList->GetEntries(); i++) {
			c.Clear();
			TKey *key = (TKey *)KeyList->At(i);
			if(TString(key->GetClassName()) != "RooDataSet") continue;
			RooDataSet *dataset = (RooDataSet *) key->ReadObj();

			TString constTermName = dataset->GetName();
			TString alphaName = constTermName;
			alphaName.ReplaceAll("constTerm", "alpha");
			if(constTermName.Contains("scale")) continue;
			if(constTermName.Contains("alpha")) continue;
			if(constTermName.Contains("1.4442-gold")) continue;
			TTree *tree = dataset2tree(dataset);

			TGraph *rhoGraph = GetRho(tree, alphaName, constTermName);

			rhoGraph->SaveAs(fout + "rhoGraph-" + constTermName + ".root");


			TGraphErrors bestFit_ = bestFit(tree, alphaName, constTermName);
			//TString binning="(241,-0.0005,0.2405,61,-0.00025,0.03025)"; //"(40,0.00025,0.02025,61,-0.0022975,0.1401475)";
			TString binning = "(241,-0.0005,0.2405,301,-0.00005,0.03005)";

			TH2F *hist = prof2d(tree, constTermName, alphaName, "nll", binning, true, nSmooth, opt);
			//hist->SaveAs("myhist.root");

			Int_t iBinX, iBinY;
			hist->GetBinWithContent2(0.0002, iBinX, iBinY, 1, -1, 1, -1, 0.0000001);

			//      if(iBinX!=0 && iBinY!=0 && iBinX < 41 && iBinY < 62){
			{
				TString catName_ = constTermName;
				catName_.ReplaceAll("constTerm_", "");
				catName_.ReplaceAll("-", "_");
				if(catIndexMap.count(catName_) == 0) catIndexMap.insert(std::pair<TString, Int_t>(catName_, catIndexMap.size()));
				catIndex = catIndexMap[catName_];
				constTerm_tree =  hist->GetYaxis()->GetBinCenter(iBinY);
				alpha_tree = hist->GetXaxis()->GetBinCenter(iBinX);
				sprintf(catName, "%s", catName_.Data());
				bestFit_.GetPoint(0, constTermTrue_tree, alphaTrue_tree);
// 	std::cout << constTerm_tree << " " << constTermTrue_tree
// 		  << "\t" << alpha_tree << " " << alphaTrue_tree
// 		  << std::endl;

				if(opt.Contains("scandiff")) {
					constTermTrue_tree = getMinimumFromTree(tree, "nll", TString(constTermName).ReplaceAll("-", "_"));
				} else       if(opt.Contains("scan")) {
					constTerm_tree = getMinimumFromTree(tree, "nll", TString(constTermName).ReplaceAll("-", "_"));
				}
				//std::cout << iBinX << "\t" << iBinY << "\t" << constTerm_tree - getMinimumFromTree(tree, "nll",TString(constTermName).ReplaceAll("-","_")) << std::endl;

				toys->Fill();
//       }else{
// 	hist->SaveAs("myhist.root");
// 	exit(0);
			}


			delete tree;
			delete hist;

		}
		f_in.Close();
	}
	//toys->SaveAs("tmp/toysTree.root");


	return toys;
}

TH1F *PlotToys(TTree *tree, Int_t catIndex, bool constTerm)
{
	TH1F *h = NULL;
	TString cutString = "catIndex==";
	cutString += catIndex;
	if(constTerm) {
		tree->Draw("constTerm-constTermTrue>>constTerm", cutString);
		h  = (TH1F *) gROOT->FindObject("constTerm");
	} else {
		tree->Draw("alpha-alphaTrue>>alpha", cutString);
		h  = (TH1F *) gROOT->FindObject("alpha");
	}
	return h;
}

void DrawToyTree(TTree *toys, TString outDir)
{

//   toys->Draw("constTerm>>constTerm_0","catIndex==0");
//   toys->Draw("constTerm>>constTerm_1","catIndex==1");
//   toys->Draw("constTerm>>constTerm_2","catIndex==2");
//   toys->Draw("constTerm>>constTerm_3","catIndex==3");
//   toys->Draw("alpha>>alpha_3","catIndex==3");
//   toys->Draw("alpha>>alpha_2","catIndex==2");
//   toys->Draw("alpha>>alpha_1","catIndex==1");
//   toys->Draw("alpha>>alpha_0","catIndex==0");
//   toys->Draw("constTerm:alpha>>constTerm_alpha_0","catIndex==0");
//   toys->Draw("constTerm:alpha>>constTerm_alpha_1","catIndex==1");
//   toys->Draw("constTerm:alpha>>constTerm_alpha_2","catIndex==2");
//   toys->Draw("constTerm:alpha>>constTerm_alpha_3","catIndex==3");
	TString binning = "(241,-0.0005,0.2405,61,-0.00025,0.03025)"; //"(40,0.00025,0.02025,61,-0.0022975,0.1401475)";

	TString constTermBinning = "(80,-0.02015,0.02025)";
	TString alphaTermBinning = "(300,-0.1495,0.1505)";
	toys->Draw("constTerm-constTermTrue>>constTerm_0" + constTermBinning, "catIndex==0");
	toys->Draw("constTerm-constTermTrue>>constTerm_1" + constTermBinning, "catIndex==1");
	toys->Draw("constTerm-constTermTrue>>constTerm_2" + constTermBinning, "catIndex==2");
	toys->Draw("constTerm-constTermTrue>>constTerm_3" + constTermBinning, "catIndex==3");
	toys->Draw("alpha-alphaTrue>>alpha_3" + alphaTermBinning, "catIndex==3");
	toys->Draw("alpha-alphaTrue>>alpha_2" + alphaTermBinning, "catIndex==2");
	toys->Draw("alpha-alphaTrue>>alpha_1" + alphaTermBinning, "catIndex==1");
	toys->Draw("alpha-alphaTrue>>alpha_0" + alphaTermBinning, "catIndex==0");
	toys->Draw("constTerm:alpha>>constTerm_alpha_0" + binning, "catIndex==0");
	toys->Draw("constTerm:alpha>>constTerm_alpha_1" + binning, "catIndex==1");
	toys->Draw("constTerm:alpha>>constTerm_alpha_2" + binning, "catIndex==2");
	toys->Draw("constTerm:alpha>>constTerm_alpha_3" + binning, "catIndex==3");

	TH1F *constTerm_alpha_0 = (TH1F *) gROOT->FindObject("constTerm_alpha_0");
	TH1F *constTerm_alpha_1 = (TH1F *) gROOT->FindObject("constTerm_alpha_1");
	TH1F *constTerm_alpha_2 = (TH1F *) gROOT->FindObject("constTerm_alpha_2");
	TH1F *constTerm_alpha_3 = (TH1F *) gROOT->FindObject("constTerm_alpha_3");

	TH1F *constTerm_0 = (TH1F *) gROOT->FindObject("constTerm_0");
	TH1F *constTerm_1 = (TH1F *) gROOT->FindObject("constTerm_1");
	TH1F *constTerm_2 = (TH1F *) gROOT->FindObject("constTerm_2");
	TH1F *constTerm_3 = (TH1F *) gROOT->FindObject("constTerm_3");

	TH1F *alpha_0 = (TH1F *) gROOT->FindObject("alpha_0");
	TH1F *alpha_1 = (TH1F *) gROOT->FindObject("alpha_1");
	TH1F *alpha_2 = (TH1F *) gROOT->FindObject("alpha_2");
	TH1F *alpha_3 = (TH1F *) gROOT->FindObject("alpha_3");

	gStyle->SetOptStat(1);

	double xq[3] = {0.5, 0.68, 0.95}, yq[3];
	//  for (Int_t i=0;i<nq;i++)
	//    xq[i] = (double)(i+1)/nq;

	TCanvas *c = new TCanvas("c", "");

	constTerm_alpha_0->Draw("colz");
	constTerm_alpha_0->GetYaxis()->SetTitle("#Delta C");
	constTerm_alpha_0->GetXaxis()->SetTitle("#Delta S");
	constTerm_alpha_0->GetXaxis()->SetRangeUser(0, 0.15);
	constTerm_alpha_0->GetYaxis()->SetRangeUser(0, 0.015);
	c->SaveAs(outDir + "/constTerm_alpha_0.png");
	c->SaveAs(outDir + "/constTerm_alpha_0.eps");

	constTerm_alpha_1->Draw("colz");
	constTerm_alpha_1->GetYaxis()->SetTitle("#Delta C");
	constTerm_alpha_1->GetXaxis()->SetTitle("#Delta S");
	constTerm_alpha_1->GetXaxis()->SetRangeUser(0, 0.15);
	constTerm_alpha_1->GetYaxis()->SetRangeUser(0, 0.015);
	c->SaveAs(outDir + "/constTerm_alpha_1.png");
	c->SaveAs(outDir + "/constTerm_alpha_1.eps");

	constTerm_alpha_2->Draw("colz");
	constTerm_alpha_2->GetYaxis()->SetTitle("#Delta C");
	constTerm_alpha_2->GetXaxis()->SetTitle("#Delta S");
	constTerm_alpha_2->GetXaxis()->SetRangeUser(0, 0.15);
	constTerm_alpha_2->GetYaxis()->SetRangeUser(0, 0.015);
	c->SaveAs(outDir + "/constTerm_alpha_2.png");
	c->SaveAs(outDir + "/constTerm_alpha_2.eps");

	constTerm_alpha_3->Draw("colz");
	constTerm_alpha_3->GetYaxis()->SetTitle("#Delta C");
	constTerm_alpha_3->GetXaxis()->SetTitle("#Delta S");
	constTerm_alpha_3->GetXaxis()->SetRangeUser(0, 0.15);
	constTerm_alpha_3->GetYaxis()->SetRangeUser(0, 0.015);
	c->SaveAs(outDir + "/constTerm_alpha_3.png");
	c->SaveAs(outDir + "/constTerm_alpha_3.eps");

	ofstream fout(outDir + "/bias.dat");
	fout << "#------------------------------" << std::endl;
	fout << "#median\t68%\t95%" << std::endl;
	constTerm_0->Draw();
	constTerm_0->GetXaxis()->SetTitle("#Delta C - #Delta C True");
	constTerm_0->GetQuantiles(3, yq, xq);
	fout << "constTerm_0: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	constTerm_0->Rebin(2);
	c->SaveAs(outDir + "/constTerm_0.png");
	c->SaveAs(outDir + "/constTerm_0.eps");

	constTerm_1->Draw();
	constTerm_1->GetXaxis()->SetTitle("#Delta C - #Delta C True");
	constTerm_1->GetQuantiles(3, yq, xq);
	fout << "constTerm_1: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	constTerm_1->Rebin(2);
	c->SaveAs(outDir + "/constTerm_1.png");
	c->SaveAs(outDir + "/constTerm_1.eps");

	constTerm_2->Draw();
	constTerm_2->GetXaxis()->SetTitle("#Delta C - #Delta C True");
	constTerm_2->GetQuantiles(3, yq, xq);
	fout << "constTerm_2: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	//  constTerm_2->GetXaxis()->SetRangeUser(-0.005,0.005);
	constTerm_2->Rebin(2);
	c->SaveAs(outDir + "/constTerm_2.png");
	c->SaveAs(outDir + "/constTerm_2.eps");

	constTerm_3->Draw();
	constTerm_3->GetXaxis()->SetTitle("#Delta C - #Delta C True");
	constTerm_3->GetQuantiles(3, yq, xq);
	fout << "constTerm_3: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	constTerm_3->Rebin(2);
	c->SaveAs(outDir + "/constTerm_3.png");
	c->SaveAs(outDir + "/constTerm_3.eps");

	alpha_0->Draw();
	alpha_0->GetXaxis()->SetTitle("#Delta S - #Delta S True");
	alpha_0->GetQuantiles(3, yq, xq);
	fout << "alpha_0: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	alpha_0->Rebin(2);
	c->SaveAs(outDir + "/alpha_0.png");
	c->SaveAs(outDir + "/alpha_0.eps");

	alpha_1->Draw();
	alpha_1->GetXaxis()->SetTitle("#Delta S - #Delta S True");
	alpha_1->GetQuantiles(3, yq, xq);
	fout << "alpha_1: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	alpha_1->Rebin(2);
	c->SaveAs(outDir + "/alpha_1.png");
	c->SaveAs(outDir + "/alpha_1.eps");

	alpha_2->Draw();
	alpha_2->GetXaxis()->SetTitle("#Delta S - #Delta S True");
	alpha_2->GetQuantiles(3, yq, xq);
	fout << "alpha_2: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	alpha_2->Rebin(2);
	c->SaveAs(outDir + "/alpha_2.png");
	c->SaveAs(outDir + "/alpha_2.eps");

	alpha_3->Draw();
	alpha_3->GetXaxis()->SetTitle("#Delta S - #Delta S True");
	alpha_3->GetQuantiles(3, yq, xq);
	fout << "alpha_3: " << yq[0] << "\t" << yq[1] << "\t" << yq[2] << std::endl;
	alpha_3->Rebin(2);
	c->SaveAs(outDir + "/alpha_3.png");
	c->SaveAs(outDir + "/alpha_3.eps");
	return;
}
// lowEtaBad
//tree->Draw(constTermName+":"+alphaName+">>h(16,0.0,0.08,20,0,0.02)",nllVarName+"-1.170791e+07","colz",10442-10232,10232)

//highEtaBad
//tree->Draw(constTermName+":"+alphaName+">>h(16,0.0,0.08,20,0,0.02)",nllVarName+"-1.170791e+07","colz",11430-11061,11061)

//lowEtaGold
//tree->Draw(constTermName+":"+alphaName+">>h(16,0.0,0.08,20,0,0.02)",nllVarName+"-1.170791e+07-4","colz",10226-9995,9995)


