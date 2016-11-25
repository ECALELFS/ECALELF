#ifndef readJsonFile_h
#define readJsonFile_h

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>





std::map<int, std::vector<std::pair<int, int> > >
readJsonFile(const std::string& inFileName);

bool AcceptEventByRunAndLumiSection(const int& runId, const int& lumiId,
                                    std::map<int, std::vector<std::pair<int, int> > >& jsonMap);


#endif
