#ifndef zeeevent_hh
#define zeeevent_hh



class ZeeEvent{
 public:
  //  Long64_t eventNumber; 
  //  TString region_name_ele1;
  //  TString region_name_ele2;
  //  int region_ele1;
  //  int region_ele2;
  float energy_ele1;
  float energy_ele2;
  //float angle_eta_ele1_ele2;
  float invMass;
  //  float eta_ele1; 
  //  float eta_ele2;
/*   float phi_ele1; */
/*   float phi_ele2; */
  float weight;
//  float pu_weight;
//  float r9weight1;
//  float r9weight2;


  
};

typedef std::vector<ZeeEvent> zee_events_t;
#endif

