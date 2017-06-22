#include "Calibration/EOverPCalibration/interface/readJSONFile.h"





std::map<int, std::vector<std::pair<int, int> > >
readJSONFile(const std::string& inFileName)
{
	std::ifstream inFile(inFileName.c_str(), std::ios::in);

	std::string line;
	while(!inFile.eof()) {
		std::string buffer;
		inFile >> buffer;
		line += buffer;
	}



	// define map with result
	std::map<int, std::vector<std::pair<int, int> > > jsonMap;



	// loop on JSON file
	for(std::string::const_iterator it = line.begin(); it < line.end(); ++it) {
		// find run number
		if( (*(it) == '"') && (*(it + 7) == '"') ) {
			std::string run(it + 1, it + 7);
			//std::cout << "found run " << run << std::endl;



			// find lumi sections
			std::vector<std::pair<int, int> > lumisections;
			for(std::string::const_iterator it2 = it + 10; it2 < line.end(); ++it2) {
				if( (*(it2) == ']') && (*(it2 - 1) == ']') ) break;
				if( *(it2) != '[' ) continue;

				std::string::const_iterator it_beg = it2;
				std::string::const_iterator it_mid;
				std::string::const_iterator it_end;

				for(std::string::const_iterator it3 = it_beg; it3 < line.end(); ++it3) {
					if( *(it3) == ',' ) it_mid = it3;
					if( *(it3) == ']' ) {
						it_end = it3;
						break;
					}
				}



				std::string lumi_beg(it_beg + 1, it_mid);
				std::string lumi_end(it_mid + 1, it_end);
				//std::cout << "[" << lumi_beg;
				//std::cout << ",";
				//std::cout << lumi_end << "]" << std::endl;

				std::pair<int, int> tempLS(atoi(lumi_beg.c_str()), atoi(lumi_end.c_str()));
				lumisections.push_back(tempLS);

				it2 = it_end;
			}


			jsonMap[atoi(run.c_str())] = lumisections;
		} // find run number

	} // loop on JSON file



	return jsonMap;
}






bool AcceptEventByRunAndLumiSection(const int& runId, const int& lumiId,
                                    std::map<int, std::vector<std::pair<int, int> > >& jsonMap)
{
	// select by runId
	if( jsonMap.find(runId) == jsonMap.end() ) return false;



	// select by lumiId
	std::vector<std::pair<int, int> > lumisections = jsonMap[runId];

	int skipEvent = true;
	for(unsigned int i = 0; i < lumisections.size(); ++i)
		if( (lumiId >= lumisections.at(i).first) &&
		        (lumiId <= lumisections.at(i).second) )
			skipEvent = false;

	if( skipEvent == true ) return false;


	return true;
}

