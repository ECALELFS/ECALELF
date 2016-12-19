#include "../interface/runDivide_class.hh"
#define DEBUG
#include <cassert>
#ifdef DEBUG
#include <TStopwatch.h>
#endif

runDivide_class::runDivide_class(void)
{
}

runDivide_class::~runDivide_class(void)
{

}


void runDivide_class::ReadRunRangeLimits(std::string fileName)
{

	//no file provided for limits: no limits
	if(fileName == "") {
		std::cout << "[WARNING] No run range limits file provided" << std::endl;
		return;
	}

	std::ifstream file(fileName);
	if(!file.good()) {
		std::cerr << "[ERROR] File " << fileName << "to opened" << std::endl;
		return;
	}

	//filling the limits
	run_t limit_value;
	while(file.peek() != EOF && file.good()) {
		if(file.peek() == 10) { //new line
			file.get();
			continue;
		}
		while(file.peek() == 32) { // space
			file.get();
			continue;
		}
		if(file.peek() == 35) { // comment, the rest of the line can be skipped
			file.ignore(1000, 10);
			continue;
		}

		file >> limit_value;
#ifdef DEBUG
		std::cout << "[DEBUG] " << limit_value << std::endl;
#endif

		limits.insert(limit_value);
	}

	return;
}


/** this method reads the ntuple and saves the number of events per run in a map */
void runDivide_class::LoadRunEventNumbers(TChain *tree, std::string runNumber_branchName, std::string runTime_branchName)
{
#ifdef DEBUG
	TStopwatch w;
	w.Start();
#endif
	run_t runNumber;
	time_t runTime;

	tree->SetBranchStatus("*", 0);
	tree->GetBranch(runNumber_branchName.c_str())->ResetBit(kDoNotProcess);
	tree->GetBranch(runTime_branchName.c_str())->ResetBit(kDoNotProcess);
//	tree->SetBranchStatus(runNumber_branchName, 1);
//	tree->SetBranchStatus(runTime_branchName, 1);

	tree->SetBranchAddress(runNumber_branchName.c_str(), &runNumber);
	tree->SetBranchAddress(runTime_branchName.c_str(), &runTime);

	//loop over tree and count the events per run
	tree->GetEntries();
	for(Long64_t ientry = 0; ientry < tree->GetEntriesFast(); ++ientry) {
		tree->GetEntry(ientry);
		auto itr = _runMap.find(runNumber);
		if( itr == _runMap.end()) {
			_runMap.insert(itr, std::make_pair(runNumber, line(runTime, runNumber)));
		} else {
			itr->second.update(runTime);
		}
	}
	tree->ResetBranchAddresses();
#ifdef DEBUG
	w.Stop();
	w.Print();
#endif
	return;
}




// loop over the map containing the number of events per run and divide the runs
void runDivide_class::FillRunLimits(unsigned int nEvents_min, float nEventsFrac_min)
{


#ifdef DEBUG
	std::cout << "[DEBUG] nEvents_min: " << nEvents_min << std::endl;
#endif

	// initialize limits, first limit >= first run
	auto  limit_itr = limits.begin(); //

	_runRangeMap.insert(_runMap.begin(), ++_runMap.begin()); //insert the first
	while(_runMap.begin()->first >= *limit_itr  && limit_itr != limits.end()) { // find the first limit
		limit_itr++;
	}

	for(auto runM = ++_runMap.begin(); runM != _runMap.end(); runM++) {
#ifdef DEBUG
		std::cout << "[RUN]" << runM->first << "\t" << runM->second._nEvents << "\t" << *limit_itr << std::endl;
#endif
		// if passed a run limit, then force a new run
		if(runM->first >= *limit_itr  && limit_itr != limits.end()) { // force a new run
			while(runM->first >= *limit_itr  && limit_itr != limits.end()) { // find the next limit
				limit_itr++;
			}
			auto runN = runM;
			runN++;
			_runRangeMap.insert(runM, runN);
#ifdef DEBUG
			std::cout << "[AFTER RUN LIMIT]" << runM->first << "\t" << runM->second._nEvents << "\t" << *limit_itr << std::endl;
#endif
		} else {
			if(_runRangeMap.rbegin()->second._nEvents < nEvents_min && ( abs(_runRangeMap.rbegin()->second._nEvents + runM->second._nEvents - nEvents_min) < abs(_runRangeMap.rbegin()->second._nEvents  - nEvents_min))) {
				_runRangeMap.rbegin()->second.add(runM->second); // add to the last
#ifdef DEBUG
				std::cout << "[NOT ENOUGH EVENTS]" << runM->first << "\t" << runM->second._nEvents << "\t" << *limit_itr << std::endl;
#endif
			} else {
#ifdef DEBUG
				std::cout << "[ENOUGH EVENTS]" << runM->first << "\t" << runM->second._nEvents << "\t" << *limit_itr << std::endl;
#endif
				auto runN = runM;
				runN++;
				_runRangeMap.insert(runM, runN);
			}
		}
	}


	// second round to agregate the run ranges with few events
	auto limitB_itr = limits.rbegin(); //
	for(auto runM = _runRangeMap.rbegin(); runM != _runRangeMap.rend(); ++runM) {
		auto runN = runM;
		runN++;

		while(runM->first < *limitB_itr && limitB_itr != limits.rend()) { //find the limit following the current run
			limitB_itr++;
		}
		if(limitB_itr != limits.rend() && runN->first < *limitB_itr) continue;
		if(runM->second._nEvents < nEvents_min * nEventsFrac_min) {
#ifdef DEBUG
			std::cout << "Erase: " << runM->first << "\t" << runM->second._nEvents << "\t" << runN->first << "\t" << runN->second._nEvents << "\t" << _runRangeMap.size() << std::endl;
#endif
			runN->second.add(runM->second);
			_runRangeMap.erase((runN).base());
		}
	}

	return;
}



//==============================
void runDivide_class::Divide(TChain *tree, std::string fileName,
                             unsigned int nEvents_min,
                             std::string runNumber_branchName,
                             std::string time_branchName)
{
#ifdef DEBUG
	std::cout << "[DEBUG] Reading run range limits from file: " << fileName << std::endl;
#endif
	ReadRunRangeLimits(fileName);

#ifdef DEBUG
	std::cout << "[DEBUG] Loading events per run from tree" << std::endl;
#endif
	LoadRunEventNumbers(tree, runNumber_branchName, time_branchName);

#ifdef DEBUG
	std::cout << "[DEBUG] Filling run limits" << std::endl;
#endif
	FillRunLimits(nEvents_min);
	return;

}


std::ostream& operator<<(std::ostream& os, runDivide_class& r)
{
	for(auto runM = r._runRangeMap.begin(); runM != r._runRangeMap.end(); runM++) {
		auto runN = runM;
		runN++;
		if(runN != r._runRangeMap.end()) {
			char range[60];
			sprintf(range, "%u-%u\t%llu\t%u-%u", runM->first, (runN->first) - 1, runM->second._nEvents, runM->second._timeMin, runM->second._timeMax);
			os << range << std::endl;
		}
	}
	return os;
}

