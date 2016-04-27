#ifndef zeeevent_hh
#define zeeevent_hh

/// class ZeeEvent ZeeEvent.h "interface/ZeeEvent.h"

class ZeeEvent
{
public:
	float energy_ele1;
	float energy_ele2;
	float invMass;
	float weight;
	float *smearings_ele1, *smearings_ele2;
};

typedef std::vector<ZeeEvent> zee_events_t;
#endif
