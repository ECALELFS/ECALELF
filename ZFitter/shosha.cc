// compile with:
//   g++ -Wall shosha.cc -O2 `root-config --libs --cflags` -Iinterface src/Stats.cc && time stdbuf -o 0 ./a.out
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
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
        Int_t   bx;
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


typedef std::map<int, int> RunList;
typedef std::vector<std::set<int> >  BxList;

RunList runlist;
BxList bxlist;


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
        t->SetBranchAddress("runTime", &e.ts);
        brlist.push_back("runTime");
        t->SetBranchAddress("nBX", &e.bx);
        brlist.push_back("nBX");
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

void read_categories_run(const char * filename, std::map<std::string, std::pair<size_t, size_t> > & sel_runs)
{
        fprintf(stdout, "[read_categories_run] starting\n");
        FILE * fd = fopen(filename, "r");
        if (!fd) {
                fprintf(stderr, "%s: read_categories_run: cannot open file\n", filename);
                exit(1);
        }
        char name[64];
        size_t run_min, run_max;
        while (fscanf(fd, "%lu-%lu %*u %*u-%*u", &run_min, &run_max) != EOF) {
                sprintf(name, "%lu_%lu", run_min, run_max);
                sel_runs[name] = std::make_pair(run_min, run_max);
                c.run_names.push_back(name);
                c.run_boundaries.push_back(run_min);
                fprintf(stderr, "found run range: %lu %lu\n", run_min, run_max);
        }
        sprintf(name, "%lu_%lu", run_max, (long unsigned int)999999);
        sel_runs[name] = std::make_pair(run_max, (long unsigned int)999999);
        c.run_names.push_back(name);
        c.run_boundaries.push_back(run_max);
        fprintf(stderr, "added run range: %lu %lu\n", run_max, (long unsigned int)999999);
        fprintf(stdout, "[read_categories_run] done\n");
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
                fprintf(stderr, "Warning: could not find boundary for run %d\n", e.run);
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


enum BxFlag { kNone = 0, kFirst, kMiddle, kMTwelve, kMax };

std::string flag_to_string(BxFlag b)
{
        switch (b) {
                case kNone:
                        return "kNone";
                case kFirst:
                        return "kFirst";
                case kMiddle:
                        return "kMiddle";
                case kMTwelve:
                        return "kMTwelve";
                case kMax:
                        return "kMax";
        }
        return "error";
}


std::set<int> read_bx_list(const char * line)
{
        int bx;
        std::stringstream s(line);
        std::string dummy;
        s >> dummy;
        std::set<int> v;
        while (1) {
                s >> bx;
                if (!s) break;
                //fprintf(stderr, "found bx %d\n", bx);
                v.insert(bx);
        }
        return v;
}


void read_bunch_list(const char * filename, RunList & run_list, BxList & bx_list)
{
        FILE * fd = fopen(filename, "r");
        if (fd == NULL) {
                fprintf(stderr, "Error opening file `%s', abort.\n", filename);
                exit(1);
        }
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        size_t cnt_pattern = 0, cnt_bx = 0;
	while (1) {
		read = getline(&line, &len, fd);
		if (read == -1) break;
		if (line[0] == 'R') {
                        assert(cnt_bx % 3 == 0);
                        int run;
                        std::stringstream s(line);
                        std::string dummy;
                        s >> dummy >> dummy;
                        while (1) {
                                s >> run;
                                if (!s) break;
                                run_list[run] = cnt_pattern * 3;
                        }
                        ++cnt_pattern;
		} else if (line[0] == 'F') {
                        std::cout << "pushing index " << cnt_pattern - 1 << " at place: " << cnt_bx << " with line " << line;
                        bx_list.push_back(read_bx_list(line));
                        ++cnt_bx;
		} else if (line[0] == 'M') {
                        bx_list.push_back(read_bx_list(line));
                        ++cnt_bx;
		} else if (line[0] == 'S') {
                        bx_list.push_back(read_bx_list(line));
                        ++cnt_bx;
                }
	}
	if (line) free(line);
}


BxFlag bx_flag(size_t i)
{
        switch (i) {
                case 0:
                        return kFirst;
                case 1:
                        return kMiddle;
                case 2:
                        return kMTwelve;
                default:
                        fprintf(stderr, "Found unknown flag `%lu' for BX, abort.\n", i);
                        exit(2);
        }

}


BxFlag select_bunch(const RunList & run_list, BxList & bx_list)
{
        auto krun = run_list.find(e.run);
        //std::cout << "run: " << e.run << " found? " << (krun != run_list.end()) << "\n";
        if (krun == run_list.end()) return kNone;
        for (size_t is = krun->second, js = 0; js < 3; ++js) {
                auto kbx = bx_list[is + js].find(e.bx);
                //if (kbx != bx_list[is + js].end()) std::cout << "run: " << e.run << " bx: " << e.bx << " type: " << js << " index: " << is << "\n";
                if (kbx != bx_list[is + js].end()) return bx_flag(js);
        }
        return kNone;
}


std::string category_bx()
{
        return flag_to_string(select_bunch(runlist, bxlist));
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
        static char cat_nPV[16];
        int bpv = e.nPV / 3;
        sprintf(cat_nPV, "nPVf%02dt%02d", bpv * 3, (bpv + 1) * 3 - 1);
        crun.push_back(cat_nPV);
        std::vector<std::string> ceta;
        float eta = fabs(e.etaSC[idx]);
        if (isEB(eta))      ceta.push_back("EB");
        else if (isEE(eta)) ceta.push_back("EE");
        if (0);
        else if (isMod1(eta)) ceta.push_back("mod1");
        else if (isMod2(eta)) ceta.push_back("mod2");
        else if (isMod3(eta)) ceta.push_back("mod3");
        else if (isMod4(eta)) ceta.push_back("mod4");
        std::string bx = category_bx();
        //if (bx != "") crun.push_back(r + '_' + bx);
        std::vector<std::string> res;
        for (auto & r : crun) {
                for (auto & e : ceta) {
                        res.push_back(r + "_" + e);
                        res.push_back(r + "_" + bx + e);
                }
        }
        return res;
}


std::vector<std::string> category_dielectron()
{
        std::vector<std::string> res;
        res.push_back("inclusive");
        if (isEB(e.etaSC[0]) && isEB(e.etaSC[1])) {
                res.push_back("EBEB");
                if (std::max(fabs(e.etaSC[0]), fabs(e.etaSC[1])) < 1.) {
                        res.push_back("LELE");
                }
        } else {
                res.push_back("notEBEB");
        }
        if (isEE(e.etaSC[0]) && isEE(e.etaSC[1])) res.push_back("EEEE");
        //std::string bx = category_bx();
        //std::vector<std::string> res_fin;
        //for (auto & r : res) {
        //        res_fin.push_back(r);
        //        res_fin.push_back(r + "_" + bx);
        //}
        //return res_fin;
        return res;
}


void usage(const char * prg, const char * msg)
{
        fprintf(stderr, "Error: %s\n", msg);
        fprintf(stdout, "Usage: %s <root file> [optional label]\n", prg);
        fprintf(stdout, "Where [optional label] is a suffix for the output files.\n");
        exit(1);
}


int main(int argc, char ** argv)
{
        if (argc < 2) usage(argv[0], "takes at least one argument");

        char label[32] = "none";
        if (argc > 2) sprintf(label, "%s", argv[2]);

        fprintf(stderr, "starting...\n");

        TFile * fin = TFile::Open(argv[1]);
        TTree * t = (TTree*)fin->Get("selected");
        //std::cout << fin << " " << ds << "\n";
        
        set_branches(t);

        std::map<size_t, size_t> runs_meta_data = get_meta_data(t);

        std::map<std::string, std::pair<size_t, size_t> > selections_runs;
        //define_categories_run(runs_meta_data, selections_runs);
        read_categories_run("run.list", selections_runs);

        read_bunch_list("bunches.dat", runlist, bxlist);

        std::vector<std::unique_ptr<TH1F> > histos(kVars);
        histos[kR9]    = std::unique_ptr<TH1F>(new TH1F("hr9", "R9", 1200, 0.0, 1.2));
        histos[kSieie] = std::unique_ptr<TH1F>(new TH1F("sieie", "sieie", 300, 0.002, 0.04));
        //histos[kSieie] = std::unique_ptr<TH1F>(new TH1F("sieie", "sieie", 300, 0., 0.075));
        histos[kMass]  = std::unique_ptr<TH1F>(new TH1F("hmass", "mass", 300, 60., 120.));
        histos[kTime]  = std::unique_ptr<TH1F>(new TH1F("htime", "time", 1000, 1456786800., 1478613587.));
        histos[kNPV]   = std::unique_ptr<TH1F>(new TH1F("hnPV", "nPV", 100, 0., 100.));

        std::map<int, std::unique_ptr<TH1F> > histos_bx;
        std::map<int, std::vector<std::unique_ptr<TH1F> > > histos_bx_sel;

        char tmp[64];
        sprintf(tmp, "histos_%s.root", label);
        TFile * fout = TFile::Open(tmp, "recreate");

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
        sprintf(tmp, "%lu_%lu", runs_meta_data.begin()->first, runs_meta_data.rbegin()->first);
        std::string cat_run_inclusive(category_run_inclusive());
        static char cat_nPV[16];
        for (size_t ien = 0; ien < nentries; ++ien) {
                t->GetEntry(ien);
                if (ien % 12347 == 0) fprintf(stderr, " Processed events: %lu (%.2f%%)  run=%d  mass=%f\r", ien, (Float_t)ien / nentries * 100, e.run, e.mass);
                if (!(e.mass >= 60 && e.mass <= 120)) continue;
                int bpv = e.nPV / 3;
                sprintf(cat_nPV, "nPVf%02dt%02d", bpv * 3, (bpv + 1) * 3 - 1);
                for (auto & c : category_dielectron()) {
                        data[kMass][cat_run_inclusive + "_" + c].add(e.mass);
                        data[kMass][category_run() + "_" + c].add(e.mass);
                        data[kMass][cat_run_inclusive + "_" + cat_nPV].add(e.mass);
                        data[kMass][cat_run_inclusive + "_" + category_bx()].add(e.mass);
                }
                data[kTime][category_run()].add(e.ts);
                data[kTime][cat_run_inclusive].add(e.ts);
                data[kNPV][category_run()].add(e.nPV);
                data[kNPV][cat_run_inclusive].add(e.nPV);
                if (histos_bx.find(e.run) == histos_bx.end()) {
			char tmp[32];
			sprintf(tmp, "h_bx_%03u", e.run);
			histos_bx[e.run] = std::unique_ptr<TH1F>(new TH1F(tmp, tmp, 3564, 0., 3564.));
			histos_bx_sel[e.run] = std::vector<std::unique_ptr<TH1F> >(kMax);
			for (int b = kNone; b < kMax; ++b) {
				sprintf(tmp, "h_bx_%03u_sel_%s", e.run, flag_to_string((BxFlag)b).c_str());
				histos_bx_sel[e.run][b] = std::unique_ptr<TH1F>(new TH1F(tmp, tmp, 3564, 0., 3564.));
			}
                }
                histos_bx[e.run]->Fill(e.bx);
                BxFlag bx_id = select_bunch(runlist, bxlist);
                histos_bx_sel[e.run][bx_id]->Fill(e.bx);
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
                sprintf(tmp, "output_%s_%s.dat", vars[cnt].c_str(), label);
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
