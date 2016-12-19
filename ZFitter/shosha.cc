// compile with:
//   g++ -Wall shosha.cc -O2 `root-config --libs --cflags` -Iinterface src/Stats.cc && time stdbuf -o 0 ./a.out
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TFriendElement.h"
#include "TH1F.h"
#include "TTree.h"

#include "interface/Stats.hh"

typedef enum { kEffSigma, kHSM, kREM, kMedian } Estimator;
typedef enum { kMass = 0, kTime, kNPV, kR9, kSieie, kVars } Variable;

typedef struct {
        Int_t   run;
        UInt_t  ts;
        Int_t   nPV;
        Float_t mass;
        Float_t etaSC[3];
        Float_t r9[3];
        Float_t sieie[3];
} Event;
Event e;

std::vector<std::string> brlist;

typedef struct {
        std::vector<size_t>      run_boundaries;;
        std::vector<std::string> run_names;
        std::vector<size_t>      eta_boundaries;;
        std::vector<std::string> eta_names;
} Categories;
Categories c;


void enable_branches(TTree * t)
{
        t->SetBranchStatus("*", 0);
        fprintf(stderr, "opened\n");
        for (auto s : brlist) {
                fprintf(stdout, "enabling branch `%s'\n", s.c_str());
                t->SetBranchStatus(s.c_str(), 1);
        }
}


void set_branches(TTree * t)
{
        t->SetBranchAddress("runNumber", &e.run);
        brlist.push_back("runNumber");
        t->SetBranchAddress("eventTime", &e.ts);
        brlist.push_back("eventTime");
        t->SetBranchAddress("nPV", &e.nPV);
        brlist.push_back("nPV");
        t->SetBranchAddress("invMass_SC_must_regrCorr_ele", &e.mass);
        brlist.push_back("invMass_SC_must_regrCorr_ele");
        t->SetAlias("mass", "invMass_SC_must_regrCorr_ele");
        t->SetBranchAddress("R9Ele", e.r9);
        brlist.push_back("R9Ele");
        t->SetBranchAddress("etaSCEle", e.etaSC);
        brlist.push_back("etaSCEle");
        t->SetBranchAddress("sigmaIEtaIEtaSCEle", e.sieie);
        brlist.push_back("sigmaIEtaIEtaSCEle");
        //t->SetCacheSize(5000000000);
        enable_branches(t);
}


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


std::map<size_t, size_t> get_meta_data(TTree * t)
{
        fprintf(stdout, "[get_meta_data] starting\n");
        std::map<size_t, size_t> runs;
        t->SetBranchStatus("*", 0);
        t->SetBranchStatus("runNumber", 1);
        size_t nentries = t->GetEntries();
        for (size_t ien = 0; ien < nentries; ++ien) {
                t->GetEntry(ien);
                runs[e.run] += 1;
        }
        enable_branches(t);
        fprintf(stdout, "[get_meta_data] done: found %lu run(s).\n", runs.size());
        return runs;
}


void define_categories_run(const std::map<size_t, size_t> & run_md, std::map<std::string, std::pair<size_t, size_t> > & sel_runs)
{
        fprintf(stdout, "[define_categories_run] starting\n");
        std::vector<size_t> runs;
        if (run_md.size() == 0) return;
        size_t cnt = 0, event_limit = 500000;
        runs.push_back(run_md.begin()->first);
        size_t r_last = 0;
        for (auto & r : run_md) {
                cnt += r.second;
                if (cnt > event_limit) {
                        cnt = 0;
                        runs.push_back(r.first);
                }
                r_last = r.first;
        }
        runs.push_back(r_last);
        char name[64];
        sprintf(name, "%lu_%lu", runs[0], runs[runs.size() - 1]);
        sel_runs[name] = std::make_pair(runs[0], runs[runs.size() - 1]);
        for (size_t s = 0; s < runs.size() - 2; ++s) {
                sprintf(name, "%lu_%lu", runs[s], runs[s + 1]);
                sel_runs[name] = std::make_pair(runs[s], runs[s + 1]);
                fprintf(stderr, "new run category: %s  lower bound: %lu\n", name, runs[s]);
                c.run_names.push_back(name);
                c.run_boundaries.push_back(runs[s]);
        }
        //c.run_boundaries.push_back(runs[runs.size() - 1] + 1);
        //sprintf(name, "%lu_%lu", runs[runs.size() - 2], runs[runs.size() - 1]);
        //c.run_names.push_back(name);
        fprintf(stdout, "[define_categories_run] done\n");
}


void print_categories()
{
        for (size_t i = 0; i < c.run_names.size(); ++i) {
                fprintf(stdout, "%s --> %lu\n", c.run_names[i].c_str(), c.run_boundaries[i]);
        }
}


void print_vector(std::vector<float> & v, std::ostream & os = std::cout)
{
        for (size_t s = 0; s < v.size(); ++s) {
                os << " " << v[s];
        }
}


bool select_run(std::pair<size_t, size_t> runs)
{
        return (size_t)e.run >= runs.first && (size_t)e.run < runs.second;
}


std::string category_run()
{
        auto it = std::upper_bound(c.run_boundaries.begin(), c.run_boundaries.end(), e.run);
        if (it != c.run_boundaries.end()) {
                return c.run_names[it - c.run_boundaries.begin() - 1];
        } else {
                return c.run_names[c.run_names.size() - 1];
        }
        return "";
}


std::string category_run_inclusive()
{
        char tmp[32];
        sprintf(tmp, "%lu_%lu", c.run_boundaries[0], c.run_boundaries[c.run_boundaries.size() - 1]);
        return tmp;
}


bool isEB(float eta)   { return fabs(eta) < 1.48; }
bool isEE(float eta)   { return fabs(eta) > 1.56 && fabs(eta) < 2.5; }
bool isMod1(float eta) { return fabs(eta) <= 0.45; }
bool isMod2(float eta) { return fabs(eta) > 0.45 && fabs(eta) <= 0.79; }
bool isMod3(float eta) { return fabs(eta) > 0.79 && fabs(eta) <= 1.14; }
bool isMod4(float eta) { return fabs(eta) > 1.14 && fabs(eta) <= 1.48; }


std::vector<std::string> category_electron(int idx)
{
        std::vector<std::string> crun; 
        crun.push_back(category_run_inclusive());
        std::string r =  category_run();
        if (r != "") crun.push_back(r);
        std::vector<std::string> ceta;
        float eta = fabs(e.etaSC[idx]);
        if (isEB(eta))      ceta.push_back("EB");
        else if (isEE(eta)) ceta.push_back("EE");
        if (0);
        else if (isMod1(eta)) ceta.push_back("mod1");
        else if (isMod2(eta)) ceta.push_back("mod2");
        else if (isMod3(eta)) ceta.push_back("mod3");
        else if (isMod4(eta)) ceta.push_back("mod4");
        std::vector<std::string> res;
        for (auto & r : crun) {
                for (auto & e : ceta) {
                        res.push_back(r + "_" + e);
                }
        }
        return res;
}


std::vector<std::string> category_dielectron()
{
        std::vector<std::string> res;
        res.push_back("inclusive");
        if (isEB(e.etaSC[0]) && isEB(e.etaSC[1])) res.push_back("EBEB");
        else                                      res.push_back("notEBnotEB");
        if (isEE(e.etaSC[0]) && isEE(e.etaSC[1])) res.push_back("EEEE");
        return res;
}


int main()
{
        fprintf(stderr, "starting...\n");
        TFile * fin = TFile::Open("./dataset.root");
        TTree * t = (TTree*)fin->Get("selected");
        //std::cout << fin << " " << ds << "\n";
        
        set_branches(t);

        std::map<size_t, size_t> runs_meta_data = get_meta_data(t);

        std::map<std::string, std::pair<size_t, size_t> > selections_runs;
        define_categories_run(runs_meta_data, selections_runs);

        std::vector<std::unique_ptr<TH1F> > histos(kVars);
        histos[kR9]    = std::unique_ptr<TH1F>(new TH1F("hr9", "R9", 1200, 0.0, 1.2));
        histos[kSieie] = std::unique_ptr<TH1F>(new TH1F("sieie", "sieie", 300, 0.002, 0.04));
        //histos[kSieie] = std::unique_ptr<TH1F>(new TH1F("sieie", "sieie", 300, 0., 0.075));
        histos[kMass]  = std::unique_ptr<TH1F>(new TH1F("hmass", "mass", 300, 60., 120.));
        histos[kTime]  = std::unique_ptr<TH1F>(new TH1F("htime", "time", 1000, 1456786800., 1478613587.));
        histos[kNPV]   = std::unique_ptr<TH1F>(new TH1F("hnPV", "nPV", 100, 0., 100.));

        TFile * fout = TFile::Open("histos.root", "recreate");

        std::vector<std::string> vars(kVars);
        vars[kR9]    = "r9";
        vars[kSieie] = "sieie";
        vars[kMass]  = "mass";
        vars[kTime]  = "time";
        vars[kNPV]   = "nPV";

        std::vector<std::map<std::string, stats> > data(kVars);
        for (size_t v = 0; v < kVars; ++v) {
                data[v] = std::map<std::string, stats>();
        }
        size_t nentries = t->GetEntries();
        char tmp[32];
        sprintf(tmp, "%lu_%lu", runs_meta_data.begin()->first, runs_meta_data.rbegin()->first);
        std::string run_range(tmp);
        for (size_t ien = 0; ien < nentries; ++ien) {
                t->GetEntry(ien);
                if (ien % 12347 == 0) fprintf(stderr, " Processed events: %lu (%.2f%%)  run=%d  mass=%f\r", ien, (Float_t)ien / nentries * 100, e.run, e.mass);
                if (!(e.mass >= 70 && e.mass <= 100)) continue;
                for (auto & c : category_dielectron()) {
                        data[kMass][run_range + "_" + c].add(e.mass);
                        data[kMass][category_run() + "_" + c].add(e.mass);
                }
                data[kTime][category_run()].add(e.ts);
                data[kTime][category_run_inclusive()].add(e.ts);
                data[kNPV][category_run()].add(e.nPV);
                data[kNPV][category_run_inclusive()].add(e.nPV);
                std::vector<std::string> cat;
                for (int jel = 0; jel < 2; ++jel) {
                        cat = category_electron(jel);
                        for (auto & s : cat) {
                                //fprintf(stderr, "--> %lu %f --> %s\n", (size_t)e.run, e.etaSC[jel], s.c_str());
                                data[kR9][s].add(e.r9[jel]);
                                data[kSieie][s].add(e.sieie[jel]);
                        }
                }
        }
        fprintf(stderr, "\ndata categorized\n");

        size_t cnt = 0;
        for (auto & var : data) {
                char tmp[128];
                sprintf(tmp, "output_%s.dat", vars[cnt].c_str());
                FILE * fd = fopen(tmp, "w");
                for (auto & cat : var) {
                        auto & s = cat.second;
                        if (histos[cnt] != 0) {
                                TH1F * h = (TH1F*)histos[cnt]->Clone(("h_" + vars[cnt] + "_" + cat.first).c_str());
                                s.fillHisto(h);
                        } else {
                                fprintf(stderr, "error filling histogram for var %lu (%s)  category %s\n", cnt, vars[cnt].c_str(), cat.first.c_str());
                        }
                        fprintf(stdout, "computing variables for category %s %s\n", vars[cnt].c_str(), cat.first.c_str());
                        fprintf(fd, "%s_%s  %lu  %f %f %f %f %f %f %f\n", vars[cnt].c_str(), cat.first.c_str(), s.n(), s.mean(), s.stdDev(), s.eff_sigma(), s.median(), s.eff_mean(0.68269 / 2.), s.min(), s.max());
                }
                fclose(fd);
                ++cnt;
        }
        fout->Write();
        return 0;
}
