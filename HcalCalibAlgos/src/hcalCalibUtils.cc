#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

#include "TString.h"
#include "Calibration/HcalCalibAlgos/interface/hcalCalibUtils.h"

using namespace std;

void sumDepths(vector<TCell> &selectCells) {
    
  // Assignes teh sum of the energy in cells with the same iEta, iPhi to the cell with depth=1.
  // All cells with depth>1 are removed form the container. If
  // the cell at depth=1 is not present: create it and follow the procedure. 
    
  if (selectCells.size()==0) return;
    
  vector<TCell> selectCellsDepth1;
  vector<TCell> selectCellsHighDepth;
    
  //
  // NB: Here we add depth 3 for iEta==16 in *HE* to the value in the barrel
  // this approach is reflected in several of the following loops: make sure
  // to check when making changes.
  //
  // In some documents it is described as having depth 1, the mapping in CMSSW uses depth 3.
    
  for (vector<TCell>::iterator i_it = selectCells.begin(); i_it != selectCells.end(); ++i_it) {
    if (HcalDetId(i_it->id()).depth()==1) {
      selectCellsDepth1.push_back(*i_it);
    }
    else {
      selectCellsHighDepth.push_back(*i_it);
    }
  }
    
    
    
  // case where depth 1 has zero energy, but higher depths with same (iEta, iPhi) have energy.
  // For iEta<=15 there is one depth -> selectCellsHighDepth is empty and we do not get in the loop.
  for (vector<TCell>::iterator i_it2 = selectCellsHighDepth.begin(); i_it2 != selectCellsHighDepth.end(); ++i_it2) {


    // protect against corrupt data      
    if (HcalDetId(i_it2->id()).ietaAbs()<15 && HcalDetId(i_it2->id()).depth()>1) {
      cout << "ERROR!!! there are no HB cells with depth>1 for iEta<15!\n"
	   << "Check the input data..." << endl;
      cout << "HCalDetId: "  <<  HcalDetId(i_it2->id()) << endl;
      return;
    }


    bool foundDepthOne = false;
    for (vector<TCell>::iterator i_it = selectCellsDepth1.begin(); i_it != selectCellsDepth1.end(); ++i_it) {
      if (HcalDetId(i_it->id()).ieta()==HcalDetId(i_it2->id()).ieta() && 
	  HcalDetId(i_it->id()).iphi()==HcalDetId(i_it2->id()).iphi())
	foundDepthOne = true;
      continue;
    }
    if (!foundDepthOne) { // create entry for depth 1 with 0 energy
	
      UInt_t newId;
      if (abs(HcalDetId(i_it2->id()).ieta())==16)
	newId = HcalDetId(HcalBarrel, HcalDetId(i_it2->id()).ieta(), HcalDetId(i_it2->id()).iphi(), 1);
      else
	newId = HcalDetId(HcalDetId(i_it2->id()).subdet(), HcalDetId(i_it2->id()).ieta(), HcalDetId(i_it2->id()).iphi(), 1);
	
      selectCellsDepth1.push_back(TCell(newId, 0.0));
      //////////            cout << "\nCreated a dummy cell in depth one to recover energy!!!\n" << endl;
    }
  }
    
  for (vector<TCell>::iterator i_it = selectCellsDepth1.begin(); i_it != selectCellsDepth1.end(); ++i_it) {
    for (vector<TCell>::iterator i_it2 = selectCellsHighDepth.begin(); i_it2 != selectCellsHighDepth.end(); ++i_it2) {
      if (HcalDetId(i_it->id()).ieta()==HcalDetId(i_it2->id()).ieta() && 
	  HcalDetId(i_it->id()).iphi()==HcalDetId(i_it2->id()).iphi()) {
	i_it->SetE(i_it->e()+i_it2->e());
	i_it2->SetE(0.0); // paranoid, aren't we...
      }
    }
  }
    
  // replace the original vectors with the new ones
  selectCells = selectCellsDepth1;
    
  return;
}


void combinePhi(vector<TCell> &selectCells) {
  
  // Map: NxN -> N cluster
  // Comine the targetE of cells with the same iEta

  if (selectCells.size()==0) return;
    
  // new container for the TCells
  // dummy cell id created with iEta; iPhi=1; depth
  // if combinePhi() is run after combining depths, depth=1
  vector<TCell> combinedCells;
    
  map<UInt_t, vector<Float_t> > etaSliceE; // keyed by id of cell with iEta and **iPhi=1**
    
  // map the cells to the eta ring
  vector<TCell>::iterator i_it = selectCells.begin();
  for (; i_it != selectCells.end(); ++i_it) {
    DetId id = HcalDetId(i_it->id());
    UInt_t thisKey = HcalDetId(HcalDetId(id).subdet(), HcalDetId(id).ieta(), 1, HcalDetId(id).depth() );
    etaSliceE[thisKey].push_back(i_it->e());
  }
    
  map<UInt_t, vector<Float_t> >::iterator m_it = etaSliceE.begin();
  for (; m_it != etaSliceE.end(); ++m_it) {
    combinedCells.push_back(TCell(m_it->first, accumulate(m_it->second.begin(), m_it->second.end(), 0.0) ) );
  }
    
  // replace the original TCell vector with the new one
  selectCells = combinedCells;
    
}


void combinePhi(vector<TCell> &selectCells, vector<TCell> &combinedCells) {
  
  // Map: NxN -> N cluster
  // Comine the targetE of cells with the same iEta

  if (selectCells.size()==0) return;
    
  map<UInt_t, vector<Float_t> > etaSliceE; // keyed by id of cell with iEta and **iPhi=1**
    
  // map the cells to the eta ring
  vector<TCell>::iterator i_it = selectCells.begin();
  for (; i_it != selectCells.end(); ++i_it) {
    DetId id = HcalDetId(i_it->id());
    UInt_t thisKey = HcalDetId(HcalDetId(id).subdet(), HcalDetId(id).ieta(), 1, HcalDetId(id).depth() );
    etaSliceE[thisKey].push_back(i_it->e());
  }
    
  map<UInt_t, vector<Float_t> >::iterator m_it = etaSliceE.begin();
  for (; m_it != etaSliceE.end(); ++m_it) {
    combinedCells.push_back(TCell(m_it->first, accumulate(m_it->second.begin(), m_it->second.end(), 0.0) ) );
  }
        
}



void getIEtaIPhiForHighestE(vector<TCell>& selectCells, Int_t& iEtaMostE, UInt_t& iPhiMostE) {
  
  
  vector<TCell> summedDepthsCells = selectCells;

  sumDepths(summedDepthsCells);  
  vector<TCell>::iterator  highCell = summedDepthsCells.begin();
   
  // sum depths locally to get highest energy tower
 
  Float_t highE = -999;
    
  for (vector<TCell>::iterator it=summedDepthsCells.begin(); it!=summedDepthsCells.end(); ++it) {
    if (highE < it->e()) {
      highCell = it;
      highE = it->e();
    }
  }

  iEtaMostE = HcalDetId(highCell->id()).ieta();
  iPhiMostE = HcalDetId(highCell->id()).iphi();

  return;
}


//
// Remove RecHits outside the 3x3 cluster and replace the  vector that will
// be used in the minimization. Acts on "event" level.
// This can not be done for iEta>20 due to segmentation => result should be restricted
// to iEta<20. Attempted to minimize affect at the boundary...

void filterCells3x3(vector<TCell>& selectCells, Int_t iEtaMaxE, UInt_t iPhiMaxE) {
    
  vector<TCell> filteredCells;
    
  Int_t dEta, dPhi;
    
  for (vector<TCell>::iterator it=selectCells.begin(); it!=selectCells.end(); ++it) {


    Bool_t passDEta = false;
    Bool_t passDPhi = false;

    dEta = HcalDetId(it->id()).ieta() - iEtaMaxE;
    dPhi = HcalDetId(it->id()).iphi() - iPhiMaxE;

    if (abs(dEta)<=1) passDEta = true;
    if (dEta== 2 && iEtaMaxE==-1) passDEta = true;
    if (dEta==-2 && iEtaMaxE== 1) passDEta = true;
        
    if (iEtaMaxE<=20) {
      
      if (abs(HcalDetId(it->id()).ieta())<=20) {
	// phi boundary
	if (abs(dPhi)==71 || abs(dPhi)<=1) passDPhi = true;
      }
      else {
	// phi segmentation in iEta>20 is different 
	// iPhi is labelled by odd numbers

	if (iEtaMaxE%2==0){
	  if (abs(dPhi)<=1 || abs(dPhi)==71) passDPhi = true;
	}
	else {
	  if (dPhi== -2 || dPhi==0 || dPhi==70) passDPhi = true;
	}
      }
    } // hottest cell wit iEta<=20
    
    else {      
      if (abs(HcalDetId(it->id()).ieta())<=20) {
	if (abs(dPhi)<=1 || dPhi==-70 || dPhi==2)  passDPhi = true;
      }
      else {
	if (abs(dPhi)<=2 || abs(dPhi)==70) passDPhi = true;
      }
    } // hottest cell wit iEta>20
                 
    if (passDEta && passDPhi) filteredCells.push_back(*it);
  }
    
  selectCells = filteredCells;

  return;
}
              
//
// Remove RecHits outside the 5x5 cluster and replace the  vector that will
// be used in the minimization. Acts on "event" level
// In principle the ntuple should be produced with 5x5 already precelected
//
// Size for iEta>20 is 3x3, but the segmantation changes by x2.
// There is som bias in the selection of towers near the boundary

void filterCells5x5(vector<TCell>& selectCells, Int_t iEtaMaxE, UInt_t iPhiMaxE) {
    
  vector<TCell> filteredCells;
    
  Int_t dEta, dPhi;
    
  for (vector<TCell>::iterator it=selectCells.begin(); it!=selectCells.end(); ++it) {
 
    dEta = HcalDetId(it->id()).ieta() - iEtaMaxE;
    dPhi = HcalDetId(it->id()).iphi() - iPhiMaxE;
        
    if (dPhi== 71) dPhi =  1;
    if (dPhi==-71) dPhi = -1;

    if (dPhi== 70) dPhi =  2;
    if (dPhi==-70) dPhi = -2;   

                        
    if (dEta== 2 && iEtaMaxE==-1) dEta =  1;
    if (dEta==-2 && iEtaMaxE== 1) dEta = -1; 

    if (dEta== 3 && iEtaMaxE==-2) dEta =  2;
    if (dEta==-3 && iEtaMaxE== 2) dEta = -2; 
      
        
    if (abs(dEta)<3 && abs(dPhi)<3) filteredCells.push_back(*it);
  }
    
  selectCells = filteredCells;

  return;
}
              

//******************************************



  // this will become part of CMSSW (but is not yet in as of 2_1_10)
  // it is defined in the new version of HcalDetId
  // remove when it is included in the new releases

bool validDetId( HcalSubdetector sd,
		 int             ies,
		 int             ip,
		 int             dp      ) 
{
  const int ie ( abs( ies ) ) ;
  
  return ( ( ip >=  1         ) &&
	   ( ip <= 72         ) &&
	   ( dp >=  1         ) &&
	   ( ie >=  1         ) &&
	   ( ( ( sd == HcalBarrel ) &&
	       ( ( ( ie <= 14         ) &&
		   ( dp ==  1         )    ) ||
		 ( ( ( ie == 15 ) || ( ie == 16 ) ) && 
		   ( dp <= 2          )                ) ) ) ||
	     (  ( sd == HcalEndcap ) &&
		( ( ( ie == 16 ) &&
		    ( dp ==  3 )          ) ||
		  ( ( ie == 17 ) &&
		    ( dp ==  1 )          ) ||
		  ( ( ie >= 18 ) &&
		    ( ie <= 20 ) &&
		    ( dp <=  2 )          ) ||
		  ( ( ie >= 21 ) &&
		    ( ie <= 26 ) &&
		    ( dp <=  2 ) &&
		    ( ip%2 == 1 )         ) ||
		  ( ( ie >= 27 ) &&
		    ( ie <= 28 ) &&
		    ( dp <=  3 ) &&
		    ( ip%2 == 1 )         ) ||
		  ( ( ie == 29 ) &&
		    ( dp <=  2 ) &&
		    ( ip%2 == 1 )         )          )      ) ||
	     (  ( sd == HcalOuter ) &&
		( ie <= 15 ) &&
		( dp ==  4 )           ) ||
	     (  ( sd == HcalForward ) &&
		( dp <=  2 )          &&
		( ( ( ie >= 29 ) &&
		    ( ie <= 39 ) &&
		    ( ip%2 == 1 )    ) ||
		  ( ( ie >= 40 ) &&
		    ( ie <= 41 ) &&
		    ( ip%4 == 3 )         )  ) ) ) ) ;
}


