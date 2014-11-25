//#ifndef EVENT_CLASS_HH
#define EVENT_CLASS_HH
#include "event_class.hh"
//#endif

#ifndef EVENT_CLASS_CC
#define EVENT_CLASS_CC
#endif

event_class::event_class(void){
}

event_class::~event_class(void){
}

bool event_class:: operator < (const event_class& b) const{

if(_runNumber >  b._runNumber) return false;
if(_runNumber < b._runNumber) return true;
if(_eventNumber > b._eventNumber) return false;
if(_eventNumber < b._eventNumber) return true;

return false;
}





