#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <TMath.h>
#include <TFile.h>

#ifndef EVENT_CLASS_HH
#define EVENT_CLASS_HH

#endif
using namespace std;

class event_class{
  
public:
  event_class(void);
	~event_class(void);

unsigned int _runNumber;
Long64_t _eventNumber;

event_class(unsigned int runNumber, Long64_t eventNumber):
_runNumber(runNumber), _eventNumber(eventNumber){};
bool operator<(const event_class& b) const;

private:

};




