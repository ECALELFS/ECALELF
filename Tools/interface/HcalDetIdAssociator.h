#ifndef TrackAssociator_HcalDetIdAssociator_h
#define TrackAssociator_HcalDetIdAssociator_h 1
// -*- C++ -*-
//
// Package:    TrackAssociator
// Class:      HcalDetIdAssociator
// 
/*

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>

//
// Original Author:  Michal Szleper
//

*/
//

#include "Calibration/Tools/interface/CaloDetIdAssociator.h"

class HcalDetIdAssociator: public CaloDetIdAssociator{
 public:
   HcalDetIdAssociator():CaloDetIdAssociator(72,70,0.087){};
 protected:
   virtual std::set<DetId> getASetOfValidDetIds(){
      std::set<DetId> setOfValidIds;
      std::vector<DetId> vectOfValidIds = geometry_->getValidDetIds(DetId::Hcal, 1);//HB
      for(std::vector<DetId>::const_iterator it = vectOfValidIds.begin(); it != vectOfValidIds.end(); ++it)
         setOfValidIds.insert(*it);

      vectOfValidIds.clear();
      vectOfValidIds = geometry_->getValidDetIds(DetId::Hcal, 2);//HE
      for(std::vector<DetId>::const_iterator it = vectOfValidIds.begin(); it != vectOfValidIds.end(); ++it)
         setOfValidIds.insert(*it);

      return setOfValidIds;

   }
};
#endif
