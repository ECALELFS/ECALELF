#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "TCut.h"
#include "TEnv.h"
#include "TFile.h"
#include "TH1F.h"

#include "RooArgList.h"
#include "RooDataSet.h"
#include "RooPlot.h"
#include "RooRealVar.h"

typedef enum { kEffSigma, kHSM, kREM, kMedian } Estimator;


float eff_sigma(std::vector<float> & v, float q = 0.68269)
{
        size_t n = v.size();
        if (n < 2) return 0;
        size_t s = floor(q * n);
        float d_min = v[s] - v[0];
        for (size_t i = s; i < n; ++i) {
                float d = v[i] - v[i - s];
                if (d < d_min) d_min = d;
        }
        return d_min / 2.;
}


float half_sample_mode(std::vector<float> & v)
{
	size_t n = v.size();
	if (n == 0) return -999;
	if (n == 1) return v[0];
	else if (n == 2) return 0.5 * (v[0] + v[1]);
	else if (n == 3) {
		if (v[1] - v[0] < v[2] - v[1]) return 0.5 * (v[0] + v[1]);
		else                           return 0.5 * (v[1] + v[2]);
	}
	float wmin = v[n - 1] - v[0];
	size_t N = ceil(0.5 * n);
	int imin = 0;
	for (size_t i = 0; i < n - N + 1; ++i) {
		float w = v[i + N - 1] - v[i];
		if (w < wmin) {
			wmin = w;
			imin = i;
		}
	}
	auto it = v.begin() + imin;
	std::vector<float> vv(it, it + N - 1);
	return half_sample_mode(vv);
}


float recursive_effective_mode(std::vector<float> & v, size_t imin, size_t imax, float q = 0.25, float e = 1e-05)
{
        size_t n = imax - imin;
        if      (imax == imin) return v[imin];
        else if (n == 1 || v[imax] - v[imin] < e) {
                return 0.5 * (v[imin] + v[imax]);
        }
        size_t s = floor(q * n) - 1;
        float d_min = v[imin + s] - v[imin];
        size_t im = imin, iM = imin + s;
        for (size_t i = imin + s; i < imax; ++i) {
                float d = v[i] - v[i - s];
                if (d < d_min - e ) {
                        d_min = d;
                        im = i - s;
                        iM = i;
                } else if (d - e < d_min) {
                        iM = i;
                }
        }
        return recursive_effective_mode(v, im, iM, q, e);
}


void dump_and_sort(std::vector<std::vector<float>> & vv, RooAbsData & ds, const std::vector<std::string> & vars)
{
        for (auto var : vars) {
                vv.push_back(std::vector<float>());
        }
        for (Int_t i = 0 ; i < ds.numEntries() ; ++i) {
                const RooArgSet * as = ds.get(i);
                int cnt = 0;
		for (auto & var : vars) {
                        vv[cnt++].push_back(as->getRealValue(var.c_str()));
                }
        }
        for (auto & v : vv) {
                std::sort(v.begin(), v.end());
        }
}


std::vector<float> collect(std::vector<std::vector<float> > vv, Estimator est)
{
        std::vector<float> res;
        for (auto & v : vv) {
                std::sort(v.begin(), v.end());
                switch (est) {
                        case kREM:
                                res.push_back(recursive_effective_mode(v, 0, v.size() - 1, 0.25, 1e-05));
                                break;
                        case kHSM:
                                res.push_back(half_sample_mode(v));
                                break;
                        case kEffSigma:
                                res.push_back(eff_sigma(v));
                                break;
                        case kMedian:
                                res.push_back(v[v.size() / 2]);
                                break;
                        default:
                                break;
                }
        }
	return res;
}


void define_selections_eta(std::map<std::string, TCut> & sel)
{
        sel["EB"]            = TCut("fabs(etaSCEle) < 1.48");
        sel["EE"]            = TCut("fabs(etaSCEle) > 1.56 && fabs(etaSCEle) < 2.5");
        //sel["EENoTk"]        = TCut("fabs(etaSCEle) > 1.56"); // commented to save time
        sel["mod1"]          = TCut("fabs(etaSCEle) <= 0.45");
        sel["mod2"]          = TCut("fabs(etaSCEle) > 0.45 && fabs(etaSCEle) <= 0.79");
        sel["mod3"]          = TCut("fabs(etaSCEle) > 0.79 && fabs(etaSCEle) <= 1.14");
        sel["mod4"]          = TCut("fabs(etaSCEle) > 1.14 && fabs(etaSCEle) <= 1.48");
        sel["EELE"]          = sel["EE"] + TCut("fabs(etaSCEle) < 2");
        sel["EEHE"]          = sel["EE"] + TCut("fabs(etaSCEle) >= 2");
}


std::map<size_t, size_t> get_meta_data(RooDataSet & ds)
{
        std::map<size_t, size_t> runs;
        for (Int_t i = 0 ; i < ds.numEntries() ; ++i) {
                const RooArgSet * as = ds.get(i);
                int cnt = 0;
                runs[as->getRealValue("runNumber")] += 1;
        }
        //for (auto & r : runs) {
        //        std::cout << "Run " << r.first << " has " << r.second << " event(s).\n";
        //}
        return runs;
}


void define_selections_run(const std::map<size_t, size_t> & run_md, std::map<std::string, TCut> & sel_runs)
{
        std::vector<size_t> runs;
        size_t cnt = 0, event_limit = 500000;
        for (auto & r : run_md) {
                cnt += r.second;
                if (cnt > event_limit) {
                        cnt = 0;
                        runs.push_back(r.first);
                }
        }
        char name[64], sel[256];
        sprintf(name, "%lld_%lld", runs[0], runs[runs.size() - 1]);
        sprintf(sel, "runNumber >= %lld && runNumber < %lld", runs[0], runs[runs.size() - 1]);
        sel_runs[name] = TCut(sel);
        for (size_t s = 0; s < runs.size() - 2; ++s) {
                sprintf(name, "%lld_%lld", runs[s], runs[s + 1]);
                sprintf(sel, "runNumber >= %lld && runNumber < %lld", runs[s], runs[s + 1]);
                sel_runs[name] = TCut(sel);
        }
}


void print_vector(std::vector<float> & v, std::ostream & os = std::cout)
{
        for (size_t s = 0; s < v.size(); ++s) {
                os << " " << v[s];
        }
}


int main()
{
        TFile * fin = TFile::Open("roodataset.root");
        RooDataSet * ds = (RooDataSet*)fin->Get("ds");
        //std::cout << fin << " " << ds << "\n";

        std::map<size_t, size_t> runs_meta_data = get_meta_data(*ds);

        std::map<std::string, TCut> selections_runs;
        define_selections_run(runs_meta_data, selections_runs);

        RooRealVar r9("R9Ele", "R9Ele", 0.8, 1.2);
        RooRealVar mass("mass", "mass", 50, 150);
        RooRealVar runTime("runTime", "runTime", 0, 100000000);

        std::map<std::string, TCut> selections_eta;
        define_selections_eta(selections_eta);

        TH1F * h_r9 = new TH1F("hr9", "R9", 200, 0.8, 1.0);
        TH1F * h_mass = new TH1F("hmass", "mass", 300, 60., 120.);

        TFile * fout = TFile::Open("histos.root", "recreate");

        std::vector<std::string> vars = {"R9Ele", "sigmaIEtaIEtaSCEle", "mass"};

        // print header
        std::cout << "#selection  n_evt  average_runTime  <quantity";
        for (auto & v : vars) std::cout << " " << v;
        std::cout << "> ...\n";

        TCut generic("mass > 70 && mass < 110");

        std::vector<float> vres;
        for (auto & r : selections_runs) {
                for (auto & e : selections_eta) {
                        std::cout << e.first << "-" << r.first;
                        //ds->fillHistogram((TH1F*)h_r9->Clone(("h_r9_" + e.first + "_" + r.first).c_str()), RooArgList(r9), r.second + e.second);
                        RooAbsData * dsred = ds->reduce(r.second + e.second + generic);
                        dsred->fillHistogram((TH1F*)h_r9->Clone(("h_R9_" + e.first + "_" + r.first).c_str()), RooArgList(r9));
                        dsred->fillHistogram((TH1F*)h_mass->Clone(("h_mass_" + e.first + "_" + r.first).c_str()), RooArgList(mass));
                        std::cout << " " << dsred->sumEntries()  << " " << std::fixed << dsred->mean(runTime);
                        std::vector<std::vector<float>> vv;
                        dump_and_sort(vv, *dsred, vars);
                        vres = collect(vv, kEffSigma);
                        std::cout << " effsigma";
                        print_vector(vres);
                        vres = collect(vv, kHSM);
                        std::cout << " HSM";
                        print_vector(vres);
                        vres = collect(vv, kREM);
                        std::cout << " REM";
                        print_vector(vres);
                        vres = collect(vv, kMedian);
                        std::cout << " median";
                        print_vector(vres);
                        std::cout << "\n";
                        delete dsred;
                }
        }

        fout->Write();
        return 0;
}
