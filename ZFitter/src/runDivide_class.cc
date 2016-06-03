#include "../interface/runDivide_class.hh"
#define DEBUG

runDivide_class::runDivide_class(void)
{


}

runDivide_class::~runDivide_class(void)
{

}


void runDivide_class::ReadRunRangeLimits(TString fileName)
{

	//no file provided for limits: no limits
	if(fileName.IsNull()) {
		std::cout << "[WARNING] No run range limits file provided" << std::endl;
		return;
	}

	std::ifstream file(fileName);
	if(!file.good()) {
		std::cerr << "[ERROR] File " << fileName << "to opened" << std::endl;
		return;
	}

	//filling the limits
	unsigned int limit_value;
	while(file.peek() != EOF && file.good()) {
		if(file.peek() == 10) {
			file.get();
			continue;
		}

		if(file.peek() == 35) {
			file.ignore(1000, 10);
			continue;
		}

		file >> limit_value;
#ifdef DEBUG
		std::cout << "[DEBUG] " << limit_value << std::endl;
#endif

		limits.insert(limit_value);
	}
#ifdef DEBUG
	std::cout << "Run range limits:" << std::endl;
	for(std::set<unsigned int>::const_iterator limits_itr = limits.begin();
	        limits_itr != limits.end();
	        limits_itr++) {
		std::cout << *limits_itr << std::endl;
	}
#endif

	return;
}


/** this method reads the ntuple and saves the number of events per run in a map */
void runDivide_class::LoadRunEventNumbers(TTree *tree, TString runNumber_branchName, TString runTime_branchName)
{
	//void runDivide_class::LoadRunEventNumbers(TTree *tree, TString runNumber_branchName){
	Int_t runNumber;
	UInt_t runTime;

	std::cout << "tree ptr  == " << tree << std::endl;
	std::cout << "tree name == " << tree->GetName() << std::endl;
	std::cout << "tree neve == " << tree->GetEntries() << std::endl;

	tree->SetBranchStatus("*", 0);
	tree->SetBranchStatus(runNumber_branchName, 1);
	tree->SetBranchStatus(runTime_branchName, 1);

	tree->SetBranchAddress(runNumber_branchName, &runNumber);
	tree->SetBranchAddress(runTime_branchName, &runTime);

	//loop over tree and count the events per run
	tree->GetEntries();
	for(Long64_t ientry = 0; ientry < tree->GetEntriesFast(); ientry++) {
		//tree->LoadTree(ientry);
		tree->GetEntry(ientry);

		if((runNumber > 210000 || runNumber < 150000) && ientry < 3) {
			tree->Show(ientry);
			std::cout << runNumber << "\t" << runTime << "\t" << runMinTimeRunMap[runNumber] << "\t" << runMaxTimeRunMap[runNumber] << std::endl;
		}

		if(eventsRunMap.count(runNumber) == 0) eventsRunMap[runNumber] = 1; // insert the new entry for a run that is not in the map
		else eventsRunMap[runNumber] += 1;
		if(runMinTimeRunMap.count(runNumber) == 0) runMinTimeRunMap[runNumber] = runTime;
		else runMinTimeRunMap[runNumber] = std::min(runMinTimeRunMap[runNumber], runTime);

		if(runMaxTimeRunMap.count(runNumber) == 0) runMaxTimeRunMap[runNumber] = runTime;
		else runMaxTimeRunMap[runNumber] = std::max(runMaxTimeRunMap[runNumber], runTime);

#ifdef DEBUG
		if(ientry < 10) std::cout << runNumber << "\t" << runTime << "\t" << runMinTimeRunMap[runNumber] << "\t" << runMaxTimeRunMap[runNumber] << std::endl;
#endif
	}
	tree->ResetBranchAddresses();
	return;
}




// loop over the map containing the number of events per run and divide the runs
void runDivide_class::FillRunLimits(unsigned int nEvents_min)
{
	// initialize runLimits
	runLimits.clear();


#ifdef DEBUG
	std::cout << "[DEBUG] eventsRunMap size: " << eventsRunMap.size() << std::endl;
	std::cout << "[DEBUG] nEvents_min: " << nEvents_min << std::endl;
#endif

	Long64_t countEvents = 0; //events in the run range
	unsigned int firstRun = (eventsRunMap.begin())->first; // start from the first run

	// initialize limits, first limit >= first run
	std::set<unsigned int>::const_iterator limit_itr = limits.begin(); //
	std::set<unsigned int>::const_iterator limitOld_itr = limit_itr;
	while(limit_itr != limits.end() && *limit_itr < firstRun) limit_itr++;

#ifdef DEBUG
	std::cout << "[DEBUG] Starting loop" << std::endl;
#endif
	runLimits.insert(eventsRunMap.begin()->first); // add the first run by default
	for(std::map<unsigned int, Long64_t>::const_iterator run_itr = eventsRunMap.begin();
	        run_itr != eventsRunMap.end();
	        run_itr++) {

#ifdef DEBUG
		std::cout << run_itr->first << "\t" << run_itr->second << "\t" << countEvents << "\t" << runLimits.size() << std::endl;
#endif


		if(limit_itr != limits.end() && run_itr->first >= *limit_itr) { // reset the event counter to start a new run range
			//      std::cout << run_itr->first << " >= " << *limit_itr << "\t" << *(runLimits.rbegin()) << std::endl;
			// if you have few events you can merge this range with the previous,
			// but you should not pass one of the range limits: you can merge
			// run ranges within the same limits
			if(limit_itr != limits.begin()) limitOld_itr = limit_itr;
			limitOld_itr--;
			if(countEvents < nEvents_min / 2 && (*(runLimits.rbegin()) > *limitOld_itr && runLimits.size() != 1)) {
				//	std::cout << *(--(runLimits.end())) << std::endl;
#ifdef DEBUG
				std::cout << "Remove the last and aggregate: " << *(runLimits.rbegin()) << "\t" << *limitOld_itr << std::endl;
#endif
				runLimits.erase(--(runLimits.end())); // remove the last and aggregate

			} //else std::cout << "[DEBUG] Runs from " << *(runLimits.rbegin()) << " have events: " << countEvents << " but reached run limit " << *limit_itr << std::endl;
			countEvents = 0; //reset the counter
			runLimits.insert(run_itr->first);

			while(limit_itr != limits.end() && *limit_itr <= run_itr->first) limit_itr++; // update  the limit

		} else if (countEvents > nEvents_min) {
			countEvents = 0; //reset the counter
			runLimits.insert(run_itr->first);
		}
		countEvents += run_itr->second; // add the events of this run
	}

	limitOld_itr = limit_itr;
	limitOld_itr--;
	//  std::cout << countEvents << "\t" << (*(runLimits.rbegin())) << "\t" << *limitOld_itr << std::endl;

	//#ifdef DEBUG
	//	std::cout << "Remove the last and aggregate: " << *(runLimits.rbegin()) << "\t" << *limitOld_itr << std::endl;
	//#endif

	if(countEvents < nEvents_min / 2 && (*(runLimits.rbegin()) > *limitOld_itr ) && runLimits.size() > 1) {
		//    std::cout << *(--(runLimits.end())) << std::endl;
		runLimits.erase(--(runLimits.end())); // remove the last and aggregate
	}
	// now the runLimits set has all the limits

	return;
}



//==============================

std::vector<TString> runDivide_class::GetRunRanges(void)
{
	std::vector<TString> v;
#ifdef DEBUG
	std::cout << "[DEBUG] runLimits size: " << runLimits.size() << std::endl;
#endif


	for(std::set<unsigned int>::const_iterator runRange_itr = (runLimits.begin());
	        runRange_itr != runLimits.end();
	   ) {

		TString range;
		range += *runRange_itr;
		range += "-";
		runRange_itr++;
		if(runRange_itr == runLimits.end()) {
			range += eventsRunMap.rbegin()->first;
			//      runRange_itr--;
			//      range+=(*runRange_itr);
			//      runRange_itr++;
		} else range +=  ((*runRange_itr) - 1);
#ifdef DEBUG
		std::cout << "[DEBUG] " << range << std::endl;
#endif
		v.push_back(range);

	}

	return v;
}

std::vector<TString> runDivide_class::Divide(TTree *tree, TString fileName,
        unsigned int nEvents_min,
        TString runNumber_branchName)
{

	std::cout << "[divide fnc]tree ptr  == " << tree << std::endl;
	std::cout << "[divide fnc]tree name == " << tree->GetName() << std::endl;
	std::cout << "[divide fnc]tree neve == " << tree->GetEntries() << std::endl;
#ifdef DEBUG
	std::cout << "[DEBUG] Reading run range limits from file: " << fileName << std::endl;
#endif
	ReadRunRangeLimits(fileName);

#ifdef DEBUG
	std::cout << "[DEBUG] Loading events per run from tree" << std::endl;
#endif
	LoadRunEventNumbers(tree, runNumber_branchName);

#ifdef DEBUG
	std::cout << "[DEBUG] Filling run limits" << std::endl;
#endif
	FillRunLimits(nEvents_min);
#ifdef DEBUG
	std::cout << "[DEBUG] Getting run ranges as string vector" << std::endl;
#endif
	return GetRunRanges();

}


void runDivide_class::PrintMap(void)
{
	for(std::map<unsigned int, Long64_t>::const_iterator itr = eventsRunMap.begin();
	        itr != eventsRunMap.end();
	        itr++) {
		std::cout << itr->first << "\t" << itr->second  << std::endl;
	}
	return;
}

void runDivide_class::PrintRunRanges(void)
{

	std::vector<TString> v = GetRunRanges();
	for(std::vector<TString>::const_iterator itr = v.begin();
	        itr != v.end();
	        itr++) {
		std::cout << *itr << std::endl;
	}
	return;
}


std::vector<std::pair<TString, Long64_t> > runDivide_class::GetRunRangeEvents(void)
{
	std::vector<std::pair<TString, Long64_t> > v;

#ifdef DEBUG
	std::cout << "[DEBUG] runLimits size: " << runLimits.size() << std::endl;
#endif


	for(std::set<unsigned int>::const_iterator runRange_itr = (runLimits.begin());
	        runRange_itr != runLimits.end();
	   ) {

		TString range;
		unsigned int runMin = *runRange_itr, runMax = 0;

		runRange_itr++;
		if(runRange_itr == runLimits.end()) {
			runMax = eventsRunMap.rbegin()->first;
		} else runMax =  ((*runRange_itr) - 1);

		range += runMin;
		range += "-";
		range += runMax;

		Long64_t events = 0;
		for(std::map<unsigned int, Long64_t>::const_iterator events_itr = eventsRunMap.lower_bound(runMin);
		        events_itr != eventsRunMap.upper_bound(runMax);
		        events_itr++) {
#ifdef DEBUG
			std::cout << "[DEBUG] " << events_itr->first << "\t" << events << std::endl;
#endif
			events += events_itr->second;
		}

#ifdef DEBUG
		std::cout << "[DEBUG] " << range << "\t" << events << "\t" << runMin << "\t" << runMax << "\t"  << std::endl;
#endif
		std::pair<TString, Long64_t> pair_tmp(range, events);
		v.push_back(pair_tmp);

	}

	return v;
}

void runDivide_class::PrintRunRangeEvents(void)
{

	std::vector<std::pair<TString, Long64_t> > v = GetRunRangeEvents();
	for(std::vector<std::pair<TString, Long64_t> >::const_iterator itr = v.begin();
	        itr != v.end();
	        itr++) {

		std::cout << itr->first << "\t" << itr->second << "\t" << GetRunRangeTime(itr->first) << std::endl;
	}
	return;
}

/** \todo fix bug for first run that has timeStamp 0 if there is only one interval */
TString runDivide_class::GetRunRangeTime(TString runRange)
{
	TString runMin_ = runRange;
	runMin_.Remove(runRange.First('-'));
	TString runMax_ = runRange;
	runMax_.Remove(0, runRange.First('-') + 1);
	TString runTimeRange;
	std::map<unsigned int, UInt_t>::const_iterator runMax_itr, runMin_itr;
	runMin_itr = runMinTimeRunMap.lower_bound(runMin_.Atoi());
	runMax_itr = runMaxTimeRunMap.upper_bound(runMax_.Atoi());
	--runMax_itr;
	runTimeRange += runMin_itr->second;
	runTimeRange += "-";
	runTimeRange += runMax_itr->second;


	//std::cout <<  runMinTimeRunMap[runMin_.Atoi()] <<  "\t" << runMaxTimeRunMap[runMax_.Atoi()] << std::endl;

	return runTimeRange;
}

