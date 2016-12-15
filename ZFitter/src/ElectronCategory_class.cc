//#define DEBUG
#include "../interface/ElectronCategory_class.hh"
#include "../../ZNtupleDumper/interface/eleIDMap.h"

#define OldDefinitions
#define GAP
#include <TPRegexp.h>
#include <stdlib.h>
#include <memory>
ElectronCategory_class::~ElectronCategory_class()
{
	return;
}

ElectronCategory_class::ElectronCategory_class(bool isRooFit_, bool roofitNameAsNtuple_):
	_isRooFit(isRooFit_),
	_roofitNameAsNtuple(roofitNameAsNtuple_),
	energyBranchName("energySCEle_corr"),
	//energyBranchName("energySCEle_pho_regrCorr"),
	//energyBranchName("energySCEle_must_regrCorr_ele"),
	_corrEle(false)
{

	return;
}

// nEle tells you if the cut is on electron 1 (nEle=1), electron 2 (nEle=2), both(nEle=0)
TCut ElectronCategory_class::GetCut(TString region, bool isMC, int nEle, bool corrEle)
{

	if(region.Sizeof() <= 1) {
		std::cerr << "[ERROR]: no region defined" << std::endl;
		return "error";
	}

	TCut cut;
	cut.Clear();

	std::set<TString> cutSet = GetCutSet(region);
	for(std::set<TString>::const_iterator itr = cutSet.begin();
	        itr != cutSet.end();
	        itr++) {
		TString cut_string = *itr;
#ifdef DEBUG
		std::cout << cut_string << std::endl;
#endif
		if(isMC && cut_string.Contains("runNumber")) {
			cut_string.Insert(0, "(");
			cut_string += "|| runNumber==1)";
		}
		if(isMC && cut_string.Contains("lumiBlock")) continue;
		//      std::cout << "[DEBUG] Skipping runNumber cut for MC " << isMC << "\t" << string << std::endl;

		if(isMC == false && (corrEle || _corrEle)) {
			//std::cerr << "[INFO] scaleEle for GetCut" << std::endl;
			cut_string.ReplaceAll(energyBranchName + "_ele1", energyBranchName + "_ele1 * scaleEle_ele1");
			cut_string.ReplaceAll(energyBranchName + "_ele2", energyBranchName + "_ele2 * scaleEle_ele2");
			TString invMassName = energyBranchName;
			invMassName.ReplaceAll("energySCEle", "invMass_SC");
			cut_string.ReplaceAll(invMassName, invMassName + "*sqrt(scaleEle_ele1 * scaleEle_ele2)");
		}
		// se contiene solo ele2 rimuovi, altrimenti sostituisci
		if(nEle != 0) {
			if(nEle == 1) {
				cut_string.ReplaceAll("_ele2", "_ele1");
			} else {
				cut_string.ReplaceAll("_ele1", "_ele2");
			}

//       if(nEle==1){
// 	if(!cut_string.Contains("_ele1")) continue;
// 	else cut_string.ReplaceAll("_ele2","_ele1");
//       } else {
// 	if(!cut_string.Contains("_ele2")) continue;
// 	else cut_string.ReplaceAll("_ele1","_ele2");
//       }
		}
		if(!_isRooFit) {
			//  TString cut_string(cut_string_string);
			cut_string.ReplaceAll("_ele1", "[0]");
			cut_string.ReplaceAll("_ele2", "[1]");
		}

		cut += cut_string;
	}

#ifdef DEBUG
	std::cout << cut << std::endl;
#endif
	return cut; //full AND of cuts

}


std::set<TString> ElectronCategory_class::GetCutSet(TString region)
{
	TCut cut_string;
//	cut_string.Clear();

	eleIDMap eleID_map;

	std::set<TString> cutSet;
	// events %2 == 0 are dropped
	TCut odd_cut = "eventNumber%2 !=0";
	TCut even_cut = "eventNumber%2 ==0";


	//Need to implement this part for Pt
	//
	TCut EB_ele1_cut = "abs(etaEle_ele1) < 1.4442";
	TCut EB_ele2_cut = "abs(etaEle_ele2) < 1.4442";
	TCut EB_cut = EB_ele1_cut && EB_ele2_cut;


	TCut EBp_ele1_cut = "etaEle_ele1 > 0 && etaEle_ele1 < 1.4442";
	TCut EBp_ele2_cut = "etaEle_ele2 > 0 && etaEle_ele2 < 1.4442";
	TCut EBp_cut = EBp_ele1_cut && EBp_ele2_cut;

	TCut EBm_ele1_cut = "etaEle_ele1 < 0 && etaEle_ele1 > -1.4442";
	TCut EBm_ele2_cut = "etaEle_ele2 < 0 && etaEle_ele2 > -1.4442";
	TCut EBm_cut = EBm_ele1_cut && EBm_ele2_cut;

	TCut EE_ele1_cut = "abs(etaEle_ele1) > 1.566 && abs(etaEle_ele1) < 2.5";
	TCut EE_ele2_cut = "abs(etaEle_ele2) > 1.566 && abs(etaEle_ele2) < 2.5";
	TCut EE_cut = EE_ele1_cut  &&  EE_ele2_cut;


	TCut EEp_ele1_cut = "etaEle_ele1 > 1.566 && etaEle_ele1 < 2.5";
	TCut EEp_ele2_cut = "etaEle_ele2 > 1.566 && etaEle_ele2 < 2.5";
	TCut EEp_cut = EEp_ele1_cut && EEp_ele2_cut;

	TCut EEm_ele1_cut = "etaEle_ele1 < -1.566 && etaEle_ele1 > -2.5";
	TCut EEm_ele2_cut = "etaEle_ele2 < -1.566 && etaEle_ele2 > -2.5";
	TCut EEm_cut = EEm_ele1_cut && EEm_ele2_cut;

	TCut preshower_ele1_cut = "abs(etaEle_ele1) > 1.7";
	TCut preshower_ele2_cut = "abs(etaEle_ele2) > 1.7";



	TCut EBlowEta_ele1_cut = "abs(etaEle_ele1) < 1.";
	TCut EBlowEta_ele2_cut = "abs(etaEle_ele2) < 1.";
	TCut EBlowEta_cut = EBlowEta_ele1_cut && EBlowEta_ele2_cut;

	TCut EBhighEta_ele1_cut = EB_ele1_cut && "abs(etaEle_ele1) >= 1.";
	TCut EBhighEta_ele2_cut = EB_ele2_cut && "abs(etaEle_ele2) >= 1.";
	TCut EBhighEta_cut = EBhighEta_ele1_cut && EBhighEta_ele2_cut;

	TCut EElowEta_ele1_cut =	EE_ele1_cut && "abs(etaEle_ele1) < 2.";
	TCut EElowEta_ele2_cut = EE_ele2_cut && "abs(etaEle_ele2) < 2.";
	TCut EElowEta_cut = EElowEta_ele1_cut && EElowEta_ele2_cut;

	TCut EEhighEta_ele1_cut =  EE_ele1_cut && "abs(etaEle_ele1) >= 2.";
	TCut EEhighEta_ele2_cut = EE_ele2_cut && "abs(etaEle_ele2) >= 2.";
	TCut EEhighEta_cut = EEhighEta_ele1_cut && EEhighEta_ele2_cut;


	TCut gold_ele1_cut = "R9Ele_ele1 >= 0.94";
	TCut gold_ele2_cut = "R9Ele_ele2 >= 0.94";
	TCut gold_cut = gold_ele1_cut && gold_ele2_cut;

	TCut bad_ele1_cut = "R9Ele_ele1 < 0.94";
	TCut bad_ele2_cut = "R9Ele_ele2 < 0.94";
	TCut bad_cut = bad_ele1_cut && bad_ele2_cut;

	TCut highR9_ele1_cut = "R9Eleprime_ele1 >= 0.94";
	TCut highR9_ele2_cut = "R9Eleprime_ele2 >= 0.94";
	TCut highR9_cut = highR9_ele1_cut && highR9_ele2_cut;

	TCut lowR9_ele1_cut = "R9Eleprime_ele1 < 0.94";
	TCut lowR9_ele2_cut = "R9Eleprime_ele2 < 0.94";
	TCut lowR9_cut = lowR9_ele1_cut && lowR9_ele2_cut;

	// EB Reference Region
	TCut EBRefReg_ele1_cut = "(abs(seedXSCEle_ele1) > 5 && abs(seedXSCEle_ele1) < 21 && (seedYSCEle_ele1 %20) > 5 && (seedYSCEle_ele1%20) <16)";
	TCut EBRefReg_ele2_cut = "(abs(seedXSCEle_ele2) > 5 && abs(seedXSCEle_ele2) < 21 && (seedYSCEle_ele2 %20) > 5 && (seedYSCEle_ele2%20) <16)";
	TCut EBRefReg_cut = EBRefReg_ele1_cut && EBRefReg_ele2_cut;

	//EE reference region
	TCut EERefReg_ele1_cut = "abs(etaEle_ele1) >= 1.7 && abs(etaEle_ele1) <= 2.3";
	EERefReg_ele1_cut += "abs(phiEle_ele1 - 1.5707963267949) >0.1 && abs(phiEle_ele1 + 1.5707963267949) >0.1";
	TCut EERefReg_ele2_cut = "abs(etaEle_ele2) >= 1.7 && abs(etaEle_ele2) <= 2.3";
	EERefReg_ele2_cut += "abs(phiEle_ele2 - 1.5707963267949) >0.1 && abs(phiEle_ele2 + 1.5707963267949) >0.1";
	TCut EERefReg_cut = EERefReg_ele1_cut && EERefReg_ele2_cut;


	//------------------------------ di servizio
	TCut EBlowEtaGold_ele1_cut = EBlowEta_ele1_cut && gold_ele1_cut;
	TCut EBlowEtaGold_ele2_cut = EBlowEta_ele2_cut && gold_ele2_cut;

	TCut EBlowEtaBad_ele1_cut = EBlowEta_ele1_cut && bad_ele1_cut;
	TCut EBlowEtaBad_ele2_cut = EBlowEta_ele2_cut && bad_ele2_cut;

	TCut EBhighEtaGold_ele1_cut = EBhighEta_ele1_cut && gold_ele1_cut;
	TCut EBhighEtaGold_ele2_cut = EBhighEta_ele2_cut && gold_ele2_cut;

	TCut EBhighEtaBad_ele1_cut = EBhighEta_ele1_cut && bad_ele1_cut;
	TCut EBhighEtaBad_ele2_cut = EBhighEta_ele2_cut && bad_ele2_cut;

	TCut EElowEtaGold_ele1_cut = EElowEta_ele1_cut && gold_ele1_cut;
	TCut EElowEtaGold_ele2_cut = EElowEta_ele2_cut && gold_ele2_cut;

	TCut EElowEtaBad_ele1_cut = EElowEta_ele1_cut && bad_ele1_cut;
	TCut EElowEtaBad_ele2_cut = EElowEta_ele2_cut && bad_ele2_cut;

	TCut EEhighEtaGold_ele1_cut = EEhighEta_ele1_cut && gold_ele1_cut;
	TCut EEhighEtaGold_ele2_cut = EEhighEta_ele2_cut && gold_ele2_cut;

	TCut EEhighEtaBad_ele1_cut = EEhighEta_ele1_cut && bad_ele1_cut;
	TCut EEhighEtaBad_ele2_cut = EEhighEta_ele2_cut && bad_ele2_cut;


	TCut TB_ele1_cut = "(iSM_ele1 == 1 || iSM_ele1 == 2 || iSM_ele1 == 10 || iSM_ele1 == 11 || iSM_ele1 == 15 || iSM_ele1 == 19 || iSM_ele1 == 20 || iSM_ele1 == 21 || iSM_ele1 == 24)";
	TCut TB_ele2_cut = "(iSM_ele2 == 1 || iSM_ele2 == 2 || iSM_ele2 == 10 || iSM_ele2 == 11 || iSM_ele2 == 15 || iSM_ele2 == 19 || iSM_ele2 == 20 || iSM_ele2 == 21 || iSM_ele2 == 24)";
	TCut TB_cut = TB_ele1_cut && TB_ele2_cut;

	TCut alphaSM_ele1_cut = "iSM_ele1==2 || iSM_ele1==4 || iSM_ele1==5 || iSM_ele1==8 || iSM_ele1==11 || iSM_ele1==25 || iSM_ele1==27 || iSM_ele1==28 || iSM_ele1==34 || iSM_ele1==35";
	TCut alphaSM_ele2_cut = "iSM_ele2==2 || iSM_ele2==4 || iSM_ele2==5 || iSM_ele2==8 || iSM_ele2==11 || iSM_ele2==25 || iSM_ele2==27 || iSM_ele2==28 || iSM_ele2==34 || iSM_ele2==35";


	//  EB+ : 2, 4, 5, 8, 11
	//   (17:58:27) Riccardo Paramatti: EB- : 7, 9, 10, 16, 17
	TCut noPFEle_cut = "recoFlagsEle_ele1 > 1 && recoFlagsEle_ele2 > 1";
	TCut fiducial_cut = "eleID[0]%2==1 && eleID[1]%2==1";

	TCut isEle_ele1_cut = "chargeEle_ele1 == 1 || chargeEle_ele1 == -1";
	TCut isEle_ele2_cut = "chargeEle_ele2 == 1 || chargeEle_ele2 == -1";
	TCut isEle_cut = isEle_ele1_cut && isEle_ele2_cut;

	/*
	  se region contiene runNumber:
	  prendi il primo range dal primo _ _ dopo runNumber
	  e il secondo range dal secondo _ _ dopo runNumber e fai il taglio

	  fai il loop per prendere gli intervalli di runNumber e crea un vettore di pair
	*/

	std::unique_ptr<TObjArray> region_splitted(region.Tokenize("-"));

	for(int i = 0 ; i < region_splitted->GetEntries(); i++) {
		TString string;
		for(string = ((TObjString *) region_splitted->At(i))->GetString();
		        ((TObjString *) region_splitted->At(i))->GetString().EndsWith("_");
		        i++) {
			string += "-";
			string += ((TObjString *)region_splitted->At(i + 1))->GetString();
		}
#ifdef DEBUG
		std::cout << string << std::endl;
#endif

		if(string.CompareTo("all") == 0) {
			continue;
		}

		if(string.CompareTo("isEle") == 0) {
			cut_string += isEle_cut;
			cutSet.insert(TString(isEle_cut));
			continue;
		}

		if(string.CompareTo("odd") == 0) {
			cut_string += odd_cut;
			cutSet.insert(TString(odd_cut));
			continue;
		}

		if(string.CompareTo("even") == 0) {
			cut_string += even_cut;
			cutSet.insert(TString(even_cut));
			continue;
		}

		if(string.CompareTo("EB") == 0) {
			cut_string += EB_cut;
			cutSet.insert(TString(EB_cut));
			continue;
		}

		if(string.CompareTo("EBp") == 0) {
			cut_string += EBp_cut;
			cutSet.insert(TString(EBp_cut));
			continue;
		}
		if(string.CompareTo("EBm") == 0) {
			cut_string += EBm_cut;
			cutSet.insert(TString(EBm_cut));
			continue;
		}


		if(string.CompareTo("EE") == 0) {
			cut_string += EE_cut;
			cutSet.insert(TString(EE_cut));
			continue;
		}

		if(string.CompareTo("EEp") == 0) {
			cut_string += EEp_cut;
			cutSet.insert(TString(EEp_cut));
			continue;
		}
		if(string.CompareTo("EEm") == 0) {
			cut_string += EEm_cut;
			cutSet.insert(TString(EEm_cut));
			continue;
		}

		if(string.CompareTo("EB_EE") == 0) {
			cut_string += (EB_ele1_cut && EE_ele2_cut) || (EB_ele2_cut && EE_ele1_cut);
			cutSet.insert(TString((EB_ele1_cut && EE_ele2_cut) || (EB_ele2_cut && EE_ele1_cut)));
			continue;
		}

		//--------------with R9Transformation by Matteo
		if(string.CompareTo("lowR9") == 0) {
			cut_string += lowR9_cut;
			cutSet.insert(TString(lowR9_cut));
			continue;
		}

		if(string.CompareTo("highR9") == 0) {
			cut_string += highR9_cut;
			cutSet.insert(TString(highR9_cut));
			continue;
		}

		//--------------without R9Transformation

		if(string.CompareTo("bad") == 0) {
			cut_string += bad_cut;
			cutSet.insert(TString(bad_cut));
			continue;
		}

		if(string.CompareTo("gold") == 0) {
			cut_string += gold_cut;
			cutSet.insert(TString(gold_cut));
			continue;
		}
		//---------------
		//--------------- gain
		if(string.Contains("gainEle")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 2) {
				std::cerr << "ERROR: incomplete eleID region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);

			TString string1 = Objstring1->GetString();
			if(string1 == "12") string1 = "0";
			else if(string1 == "6") string1 = "1";
			else if(string1 == "1") string1 = "2";

			TCut cutEle1("gainEle_ele1 ==" + string1);
			TCut cutEle2("gainEle_ele2 ==" + string1);

			if(string.Contains("SingleEle"))
				cutSet.insert(TString(cutEle1 || cutEle2));
			else {
				cutSet.insert(TString(cutEle1));
				cutSet.insert(TString(cutEle2));
			}
			delete splitted;
			continue;
		}


		//--------------- ETA
		if(string.Contains("absEta")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete absEta region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("abs(etaEle_ele1) >= " + string1);
			TCut cutEle1_2("abs(etaEle_ele1) <  " + string2);
			TCut cutEle2_1("abs(etaEle_ele2) >= " + string1);
			TCut cutEle2_2("abs(etaEle_ele2) <  " + string2);

			TCut absEta_ele1_cut = cutEle1_1 && cutEle1_2;
			TCut absEta_ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("SingleEle")) {
				cut_string += absEta_ele1_cut || absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut || absEta_ele2_cut));
			} else {
				cut_string += absEta_ele1_cut && absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut));
				cutSet.insert(TString(absEta_ele2_cut));
			}
			delete splitted;
			continue;
		}

		//--------------- Inclusive
		if(string.Contains("Inclusive")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete absEta region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);
			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("abs(etaEle_ele1) >= " + string1 + " && abs(etaEle_ele1) < 1.442");
			TCut cutEle1_2("abs(etaEle_ele1) >=  1.566 && abs(etaEle_ele1) <" + string2);
			TCut cutEle2_1("abs(etaEle_ele2) >= " + string1 + " && abs(etaEle_ele2) < 1.442");
			TCut cutEle2_2("abs(etaEle_ele2) >=  1.566 && abs(etaEle_ele2) <" + string2);


			TCut Inclusive_ele1_cut = cutEle1_1 || cutEle1_2 ; //-->note the or in the inclusive scenario
			TCut Inclusive_ele2_cut = cutEle2_1 || cutEle2_2 ; //-->note the or in the inclusive scenario

			cut_string += Inclusive_ele1_cut && Inclusive_ele2_cut;
			cutSet.insert(TString(Inclusive_ele1_cut));
			cutSet.insert(TString(Inclusive_ele2_cut));

			continue;
		}


		//--------------- IETA
		if(string.Contains("absIEta")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete absEta region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("abs(seedXSCEle_ele1) >= " + string1);
			TCut cutEle1_2("abs(seedXSCEle_ele1) <= " + string2);
			TCut cutEle2_1("abs(seedXSCEle_ele2) >= " + string1);
			TCut cutEle2_2("abs(seedXSCEle_ele2) <= " + string2);

			TCut absEta_ele1_cut = cutEle1_1 && cutEle1_2;
			TCut absEta_ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("EB"))  cut_string += EB_cut;
			else if(string.Contains("EE")) cut_string += EE_cut;
			else cut_string += EB_cut;

			if(string.Contains("SingleEle")) {
				cut_string += absEta_ele1_cut || absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut || absEta_ele2_cut));
			} else {
				cut_string += absEta_ele1_cut && absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut));
				cutSet.insert(TString(absEta_ele2_cut));
			}

			delete splitted;
			continue;
		}

		if(string.Contains("distIEta")) {
			if(string.EndsWith("_")) string += ((TObjString*)(region_splitted->At(++i)))->GetString();
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() != 3) {
				std::cerr << "ERROR: incomplete or wrong distIEta region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TString a;
			a += (string1.Atoi() - string2.Atoi());
			TString b;
			b += string1.Atoi() + string2.Atoi();

			TCut cutEle1_1("abs(seedXSCEle_ele1) >= " + a);
			TCut cutEle1_2("abs(seedXSCEle_ele1) <= " + b);
			TCut cutEle2_1("abs(seedXSCEle_ele2) >= " + a);
			TCut cutEle2_2("abs(seedXSCEle_ele2) <= " + b);

			TCut absEta_ele1_cut = cutEle1_1 && cutEle1_2;
			TCut absEta_ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("EB"))  cut_string += EB_cut;
			else if(string.Contains("EE")) cut_string += EE_cut;
			else cut_string += EB_cut;

			if(string.Contains("SingleEle")) {
				cut_string += absEta_ele1_cut || absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut || absEta_ele2_cut));
			} else {
				cut_string += absEta_ele1_cut && absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut && absEta_ele2_cut));
			}

			delete splitted;
			continue;
		}

		if(string.Contains("distIPhi")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() != 3) {
				std::cerr << "ERROR: incomplete or wrong distIPhi region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TString a;
			a += (string1.Atoi() - string2.Atoi());
			TString b;
			b += string1.Atoi() + string2.Atoi();

			TCut cutEle1_1("abs(seedYSCEle_ele1) >= " + a);
			TCut cutEle1_2("abs(seedYSCEle_ele1) <= " + b);
			TCut cutEle2_1("abs(seedYSCEle_ele2) >= " + a);
			TCut cutEle2_2("abs(seedYSCEle_ele2) <= " + b);

			TCut absEta_ele1_cut = cutEle1_1 && cutEle1_2;
			TCut absEta_ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("EB"))  cut_string += EB_cut;
			else if(string.Contains("EE")) cut_string += EE_cut;
			else cut_string += EB_cut;

			if(string.Contains("SingleEle")) {
				cut_string += absEta_ele1_cut || absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut || absEta_ele2_cut));
			} else {
				cut_string += absEta_ele1_cut && absEta_ele2_cut;
				cutSet.insert(TString(absEta_ele1_cut && absEta_ele2_cut));
			}

			delete splitted;
			continue;
		}

		if(string.Contains("IEta")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 2 || splitted->GetEntries() > 3 ) {
				std::cerr << "ERROR: incomplete or wrong IEta region definition" << std::endl;
				continue;
			}

			if(splitted->GetEntries() == 2) {
				TObjString *Objstring1 = (TObjString *) splitted->At(1);

				TString string1 = Objstring1->GetString();

				TCut cutEle1_1("(seedXSCEle_ele1) == " + string1);
				TCut cutEle2_1("(seedXSCEle_ele2) == " + string1);

				TCut absEta_ele1_cut = cutEle1_1;
				TCut absEta_ele2_cut = cutEle2_1;

				if(string.Contains("EB"))  cut_string += EB_cut;
				else if(string.Contains("EE")) cut_string += EE_cut;
				else cut_string += EB_cut;

				if(string.Contains("SingleEle")) {
					cut_string += absEta_ele1_cut || absEta_ele2_cut;
					cutSet.insert(TString(absEta_ele1_cut || absEta_ele2_cut));
				} else {
					cut_string += absEta_ele1_cut && absEta_ele2_cut;
					cutSet.insert(TString(absEta_ele1_cut && absEta_ele2_cut));
				}

			} else {

				TObjString *Objstring1 = (TObjString *) splitted->At(1);
				TObjString *Objstring2 = (TObjString *) splitted->At(2);

				TString string1 = Objstring1->GetString();
				TString string2 = Objstring2->GetString();

				TCut cutEle1_1("(seedXSCEle_ele1) >= " + string1);
				TCut cutEle1_2("(seedXSCEle_ele1) <= " + string2);
				TCut cutEle2_1("(seedXSCEle_ele2) >= " + string1);
				TCut cutEle2_2("(seedXSCEle_ele2) <= " + string2);

				TCut absEta_ele1_cut = cutEle1_1 && cutEle1_2;
				TCut absEta_ele2_cut = cutEle2_1 && cutEle2_2;

				if(string.Contains("EB"))  cut_string += EB_cut;
				else if(string.Contains("EE")) cut_string += EE_cut;
				else cut_string += EB_cut;

				if(string.Contains("SingleEle")) {
					cut_string += absEta_ele1_cut || absEta_ele2_cut;
					cutSet.insert(TString(absEta_ele1_cut || absEta_ele2_cut));
				} else {
					cut_string += absEta_ele1_cut && absEta_ele2_cut;
					cutSet.insert(TString(absEta_ele1_cut && absEta_ele2_cut));
				}

			}

			delete splitted;
			continue;
		}

		//--------------- runNumber
		if(string.Contains("runNumber")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete runNumber region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cut1("runNumber >= " + string1);
			TCut cut2("runNumber <=  " + string2);

			cut_string += cut1 && cut2;
			cutSet.insert(TString(cut1 && cut2));


			delete splitted;
			continue;
		}

		//--------------- lumiBlock
		if(string.Contains("lumiBlock")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete lumiBlock region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cut1("lumiBlock >= " + string1);
			TCut cut2("lumiBlock <=  " + string2);

			cut_string += cut1 && cut2;
			cutSet.insert(TString(cut1 && cut2));


			delete splitted;
			continue;
		}

		//--------------- fbrem
		if(string.Contains("fbrem")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete fbrem region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("fbremEle_ele1 >= " + string1);
			TCut cutEle1_2("fbremEle_ele1 <  " + string2);
			TCut cutEle2_1("fbremEle_ele2 >= " + string1);
			TCut cutEle2_2("fbremEle_ele2 <  " + string2);

			TCut ele1_cut = cutEle1_1 && cutEle1_2;
			TCut ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("SingleEle")) {
				cut_string += ele1_cut || ele2_cut;
				cutSet.insert(TString(ele1_cut || ele2_cut));
			} else {
				cut_string += ele1_cut && ele2_cut;
				cutSet.insert(TString(ele1_cut && ele2_cut));
			}

			delete splitted;
			continue;
		}

		//--------------- R9
		if(string.Contains("lowR9_")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() != 2) {
				std::cerr << "ERROR: incomplete lowR9 cut point definition" << std::endl;
				continue;
			} else {
				TObjString *Objstring1 = (TObjString *) splitted->At(1);
				TString string1 = Objstring1->GetString();

				TCut cutEle1("R9Eleprime_ele1 < " + string1);
				TCut cutEle2("R9Eleprime_ele2 < " + string1);
				cut_string += cutEle1 && cutEle2;
				cutSet.insert(TString(cutEle1 && cutEle2));
				delete splitted;
			}
		}

		//--------------- Et
		if(string.Contains("Et_") || string.Contains("EtSingleEle_")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() != 3 && splitted->GetEntries() != 2) {
				std::cerr << "ERROR: incomplete Et region definition" << std::endl;
				continue;
			}

			if(splitted->GetEntries() == 2) {
				TObjString *Objstring1 = (TObjString *) splitted->At(1);

				TString string1 = Objstring1->GetString();


				TCut cutEle1("" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) >= " + string1);
				TCut cutEle2("" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) >= " + string1);

				cut_string += cutEle1 && cutEle2;
				cutSet.insert(TString(cutEle1 && cutEle2));
				delete splitted;
				continue;
			} else {
				TObjString *Objstring1 = (TObjString *) splitted->At(1);
				TObjString *Objstring2 = (TObjString *) splitted->At(2);

				TString string1 = Objstring1->GetString();
				TString string2 = Objstring2->GetString();

				TCut cutEle1_1("" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) >= " + string1);
				TCut cutEle1_2("" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) <  " + string2);
				TCut cutEle2_1("" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) >= " + string1);
				TCut cutEle2_2("" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) <  " + string2);

				TCut ele1_cut = cutEle1_1 && cutEle1_2;
				TCut ele2_cut = cutEle2_1 && cutEle2_2;

				if(string.Contains("SingleEle")) {
					cut_string += ele1_cut || ele2_cut;
					cutSet.insert(TString(ele1_cut || ele2_cut));
				} else {
					if(string.Contains("!")) {
						cut_string += (!ele1_cut) && (!ele2_cut);
						cutSet.insert(TString(!ele1_cut));
						cutSet.insert(TString(!ele2_cut));
					} else {
						cut_string += ele1_cut && ele2_cut;
						cutSet.insert(TString(ele1_cut));
						cutSet.insert(TString(ele2_cut));
					}
				}

				delete splitted;
				continue;
			}
		}

		//--------------- Et cuts per electron (emulate trigger)
		if(string.Contains("EtLeading_")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() != 2) {
				std::cerr << "ERROR: incomplete EtLeading region definition" << std::endl;
				continue;
			}

			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			//usually the cut is EtLeading_32, so string 1 is 32
			TString string1 = Objstring1->GetString();

			//Case mass dep: EtLeading_MassOver3
			//string1 is MassOver3
			TObjArray * splitted_string1 = string1.Tokenize("Over"); //In case you want a mass dep cut
			TString ismass_dep = ((TObjString *) splitted_string1->At(0))->GetString();
			if(ismass_dep == "Mass") {
				std::cout << "mass dep cut" << std::endl;
				TString fraction = ((TObjString *) splitted_string1->At(1))->GetString();
				string1 = "invMass_SC_pho_regrCorr/" + fraction;
			}
			TCut cutEle1("" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) >= " + "( " + energyBranchName + "_ele1/cosh(etaSCEle_ele1) >" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) )*" + string1);
			TCut cutEle2("" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) >= " + "( " + energyBranchName + "_ele2/cosh(etaSCEle_ele2) >" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) )*" + string1);

			cut_string += cutEle1 && cutEle2;
			cutSet.insert(TString(cutEle1 && cutEle2));
			delete splitted;
			continue;
		}

		if(string.Contains("EtSubLeading_")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() != 2) {
				std::cerr << "ERROR: incomplete EtLeading region definition" << std::endl;
				continue;
			}

			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TString string1 = Objstring1->GetString();
			//as for the Leading, you may want to apply Mass dep cut
			TObjArray * splitted_string1 = string1.Tokenize("Over"); //In case you want a mass dep cut
			TString ismass_dep = ((TObjString *) splitted_string1->At(0))->GetString();
			if(ismass_dep == "Mass") {
				std::cout << "mass dep cut" << std::endl;
				TString fraction = ((TObjString *) splitted_string1->At(1))->GetString();
				string1 = "invMass_SC_pho_regrCorr/" + fraction;
			}
			TCut cutEle1("" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) >= " + "( " + energyBranchName + "_ele1/cosh(etaSCEle_ele1) <" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) )*" + string1);
			TCut cutEle2("" + energyBranchName + "_ele2/cosh(etaSCEle_ele2) >= " + "( " + energyBranchName + "_ele2/cosh(etaSCEle_ele2) <" + energyBranchName + "_ele1/cosh(etaSCEle_ele1) )*" + string1);

			cut_string += cutEle1 && cutEle2;
			cutSet.insert(TString(cutEle1 && cutEle2));
			delete splitted;
			continue;
		}


		//--------------- eleID
		if(string.Contains("eleID_")) {
			std::unique_ptr<TObjArray> splitted(string.Tokenize("_"));
			if(splitted->GetEntries() < 2) {
				std::cerr << "ERROR: incomplete eleID region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);

			TString string1 = Objstring1->GetString();
			string1.ReplaceAll("|", "-");
			//Note for expert users: the original eleID name has "-", hence to avoid splitting "-" are replaced with "|" by ZFitter.cpp
			//This is the moment where the "|" are again mapped in their righteous "-" (jeux de cartes)
			string1.Form("%d", eleID_map.eleIDmap[string1.Data()]);
			TCut cutEle1("(eleID_ele1 & " + string1 + ")==" + string1);
			TCut cutEle2("(eleID_ele2 & " + string1 + ")==" + string1);

			cut_string += cutEle1 && cutEle2;
			cutSet.insert(TString(cutEle1 && cutEle2));
			continue;
		}

		//--------------- iSM
		if(string.Contains("iSM")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 2) {
				std::cerr << "ERROR: incomplete Et region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);

			TString string1 = Objstring1->GetString();


			TCut cutEle1("iSM_ele1 == " + string1);
			TCut cutEle2("iSM_ele2 == " + string1);

			if(string.Contains("SingleEle")) {
				cut_string += cutEle1 || cutEle2;
				cutSet.insert(TString(cutEle1 || cutEle2));
			} else {
				cut_string += cutEle1 && cutEle2;
				cutSet.insert(TString(cutEle1 && cutEle2));
			}

			delete splitted;
			continue;
		}

		//--------------- energy cut
		if(string.Contains("energy")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete energySC region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("energySCEle_corr_ele1 >= " + string1);
			TCut cutEle1_2("energySCEle_corr_ele1 <  " + string2);
			TCut cutEle2_1("energySCEle_corr_ele2 >= " + string1);
			TCut cutEle2_2("energySCEle_corr_ele2 <  " + string2);

			TCut ele1_cut = cutEle1_1 && cutEle1_2;
			TCut ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("SingleEle")) {
				cut_string += ele1_cut || ele2_cut;
				cutSet.insert(TString(ele1_cut || ele2_cut));
			} else {
				cut_string += ele1_cut && ele2_cut;
				cutSet.insert(TString(ele1_cut && ele2_cut));
			}

			delete splitted;
			continue;
		}

		//--------------- energy cut
		if(string.Contains("seedEnergy")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete energySC region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("seedEnergySCEle_ele1 >= " + string1);
			TCut cutEle1_2("seedEnergySCEle_ele1 <  " + string2);
			TCut cutEle2_1("seedEnergySCEle_ele2 >= " + string1);
			TCut cutEle2_2("seedEnergySCEle_ele2 <  " + string2);

			TCut ele1_cut = cutEle1_1 && cutEle1_2;
			TCut ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("SingleEle")) {
				cut_string += ele1_cut || ele2_cut;
				cutSet.insert(TString(ele1_cut || ele2_cut));
			} else {
				cut_string += ele1_cut && ele2_cut;
				cutSet.insert(TString(ele1_cut && ele2_cut));
			}

			delete splitted;
			continue;
		}


		//--------------- nHitsSCEle
		if(string.Contains("nHitsSCEle")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete nHitsSCEle region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutEle1_1("nHitsSCEle >= " + string1);
			TCut cutEle1_2("nHitsSCEle <  " + string2);
			TCut cutEle2_1("nHitsSCEle >= " + string1);
			TCut cutEle2_2("nHitsSCEle <  " + string2);

			TCut ele1_cut = cutEle1_1 && cutEle1_2;
			TCut ele2_cut = cutEle2_1 && cutEle2_2;

			if(string.Contains("SingleEle")) {
				cut_string += ele1_cut || ele2_cut;
				cutSet.insert(TString(ele1_cut || ele2_cut));
			} else {
				cut_string += ele1_cut && ele2_cut;
				cutSet.insert(TString(ele1_cut && ele2_cut));
			}

			delete splitted;
			continue;
		}

		//--------------- ZPt
		//example ZPt_30_40
		if(string.Contains("ZPt")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete Pt region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cutdiEle_1("ZPt >= " + string1);
			TCut cutdiEle_2("ZPt <  " + string2);

			TCut ZPt_cut = cutdiEle_1 && cutdiEle_2;

			cut_string += ZPt_cut;
			cutSet.insert(TString(ZPt_cut));
			delete splitted;
			continue;
		}

		//--------------- R9
		if(string.Contains("R9")) {
			TObjArray *splitted = string.Tokenize("_");

			if(string.Contains("R9p") || string.Contains("R9m")) {
				if(splitted->GetEntries() != 2) {
					std::cerr << "ERROR: incomplete R9p region definition" << std::endl;
					continue;
				}
				TString sign;
				if(string.Contains("R9p")) sign = " >= ";
				else if(string.Contains("R9m"))  sign = " < ";
				else exit(1);

				TObjString *Objstring1 = (TObjString *) splitted->At(1);

				TString string1 = Objstring1->GetString();

				TCut R9_ele1_cut("R9Ele_ele1" + sign + string1);
				TCut R9_ele2_cut("R9Ele_ele2" + sign + string1);

				if(string.Contains("SinsgleEle"))
					//	  cut_string += (R9_ele1_cut || R9_ele2_cut);
					cutSet.insert(TString(R9_ele1_cut || R9_ele2_cut));
				else {
					cutSet.insert(TString(R9_ele1_cut));
					cutSet.insert(TString(R9_ele2_cut)); //cut_string+=(R9_ele1_cut && R9_ele2_cut);
				}
			} else {
				if(splitted->GetEntries() < 3) {
					std::cerr << "ERROR: incomplete R9 region definition" << std::endl;
					continue;
				}
				TObjString *Objstring1 = (TObjString *) splitted->At(1);
				TObjString *Objstring2 = (TObjString *) splitted->At(2);

				TString string1 = Objstring1->GetString();
				TString string2 = Objstring2->GetString();

				TCut cutEle1_1("R9Ele_ele1 >= " + string1);
				TCut cutEle1_2("R9Ele_ele1 <  " + string2);
				TCut cutEle2_1("R9Ele_ele2 >= " + string1);
				TCut cutEle2_2("R9Ele_ele2 <  " + string2);

				TCut R9_ele1_cut = cutEle1_1 && cutEle1_2;
				TCut R9_ele2_cut = cutEle2_1 && cutEle2_2;

				if(string.Contains("SingleEle")) cutSet.insert(TString(R9_ele1_cut || R9_ele2_cut)); //cut_string += R9_ele1_cut || R9_ele2_cut;
				else {
					cutSet.insert(TString(R9_ele1_cut)); //cut_string+=R9_ele1_cut && R9_ele2_cut;
					cutSet.insert(TString(R9_ele2_cut));
				}
			}
			delete splitted;
			continue;
		}

		//---------------R9Eleprime
		if(string.Contains("r9prime") || string.Contains("r9prime")) {
			TObjArray *splitted = string.Tokenize("_");
			if(string.Contains("r9primep") || string.Contains("r9primem")) {
				if(splitted->GetEntries() != 2) {
					std::cerr << "ERROR: incomplete r9prime region definition" << std::endl;
					continue;
				}
				TString sign;
				if(string.Contains("r9primep")) sign = " >= ";
				else if(string.Contains("r9primem"))  sign = " < ";
				else exit(1);

				TObjString *Objstring1 = (TObjString *) splitted->At(1);

				TString string1 = Objstring1->GetString();

				TCut ele1_cut("R9Eleprime_ele1" + sign + string1);
				TCut ele2_cut("R9Eleprime_ele2" + sign + string1);

				cutSet.insert(TString(ele1_cut));
				cutSet.insert(TString(ele2_cut));
			}

			delete splitted;
			continue;
		}

		//--------------- nPV
		if(string.Contains("nPV")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete nPV region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);
			TObjString *Objstring2 = (TObjString *) splitted->At(2);

			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			TCut cut1("nPV >= " + string1);
			TCut cut2("nPV <  " + string2);
			cut_string += cut1 && cut2;
			cutSet.insert(TString(cut1));
			cutSet.insert(TString(cut2));
			delete splitted;
			continue;
		}


#ifdef GAP
		//--------------- Gap
		if(string.Contains("EBGapEle")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 2) {
				std::cerr << "ERROR: incomplete EBGapEle1 region definition" << std::endl;
				continue;
			}
			TString eleIndex;
			if(string.Contains("Ele1")) eleIndex = "ele1";
			if(string.Contains("Ele2")) eleIndex = "ele2";
			if(eleIndex.Sizeof() <= 1) {
				std::cerr << "[ERROR]: Ele index not defined: EBGapEle1 or EBGapEle2" << std::endl;
				continue;
			}

			TObjString *Objstring1 = (TObjString *) splitted->At(1);

			TString string1 = Objstring1->GetString();
			TString ieta_gap_value = string1; // inutili per ora
			TString iphi_gap_value = string1; // inutili per ora

			// attento che la notazione e' invertita:
			//  - questo taglio richiede sia fuori dai gap di n ieta_gap_value cristalli

			TCut EB_m1_ietagap_cut("abs(ieta_" + eleIndex + ") > 0 +" + ieta_gap_value + " && abs(ieta_" + eleIndex + ") < 26-" + ieta_gap_value);
			TCut EB_m2_ietagap_cut("abs(ieta_" + eleIndex + ") > 25+" + ieta_gap_value + " && abs(ieta_" + eleIndex + ") < 46-" + ieta_gap_value);
			TCut EB_m3_ietagap_cut("abs(ieta_" + eleIndex + ") > 45+" + ieta_gap_value + " && abs(ieta_" + eleIndex + ") < 66-" + ieta_gap_value);
			TCut EB_m4_ietagap_cut("abs(ieta_" + eleIndex + ") > 65+" + ieta_gap_value + " && abs(ieta_" + eleIndex + ") < 86-" + ieta_gap_value);

			TCut EB_iphigap_low_cut("(iphi_" + eleIndex + "-1) %20 >= " + iphi_gap_value);
			TCut EB_iphigap_high_cut("(iphi_" + eleIndex + "-1) %20 < 20-" + iphi_gap_value);

			TCut ietagap_cut = EB_m1_ietagap_cut || EB_m2_ietagap_cut || EB_m3_ietagap_cut || EB_m4_ietagap_cut;
			TCut iphigap_cut = EB_iphigap_low_cut && EB_iphigap_high_cut;

			if(string.Contains("!")) cut_string += ietagap_cut && iphigap_cut;
			else 	cut_string += (!ietagap_cut) || (!iphigap_cut);
			delete splitted;
			continue;
		}


		//--------------- Gap
		if(string.Contains("Mod")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 2) {
				std::cerr << "ERROR: incomplete EBGapEle1 region definition" << std::endl;
				continue;
			}

			TObjString *Objstring1 = (TObjString *) splitted->At(1);

			TString string1 = Objstring1->GetString();

			TCut cut1;
			TCut cut2;
			if(string1.Contains("1")) {
				cut1 = cut1 || ("abs(seedXSCEle_ele1) < 26");
				cut2 = cut2 || ("abs(seedXSCEle_ele2) < 26");
			}

			if(string1.Contains("2")) {
				cut1 = cut1 || ("abs(seedXSCEle_ele1) > 25 && abs(seedXSCEle_ele1) < 46");
				cut2 = cut2 || ("abs(seedXSCEle_ele2) > 25 && abs(seedXSCEle_ele2) < 46");
			}

			if(string1.Contains("3")) {
				cut1 = cut1 || ("abs(seedXSCEle_ele1) > 45 && abs(seedXSCEle_ele1) < 66");
				cut2 = cut2 || ("abs(seedXSCEle_ele2) > 45 && abs(seedXSCEle_ele2) < 66");
			}

			if(string1.Contains("4")) {
				cut1 = cut1 || ("abs(seedXSCEle_ele1) > 65 && abs(seedXSCEle_ele1) < 86");
				cut2 = cut2 || ("abs(seedXSCEle_ele2) > 65 && abs(seedXSCEle_ele2) < 86");
			}

			cut_string += EB_cut && cut1 && cut2;
			delete splitted;
			continue;
		}

#endif

		//--------------- Classification Ele
		if(string.Contains("CL")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 2) {
				std::cerr << "ERROR: incomplete CL region definition" << std::endl;
				continue;
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(1);

			TString string1 = Objstring1->GetString();


			TCut cutEle1("classificationEle_ele1 == " + string1);
			TCut cutEle2("classificationEle_ele2 == " + string1);

			if(string.Contains("SingleEle")) cut_string += cutEle1 || cutEle2;
			else cut_string += cutEle1 && cutEle2;
			delete splitted;
			continue;
		}


		//--------------- invMassSigma
		if(string.Contains("invMassSigma")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete invMassSigma region definition" << std::endl;
				continue;
			}

			TObjString *Objstring0 = (TObjString *) splitted->At(0);
			TString string0 = Objstring0->GetString();
			for(int iString = 1; iString < splitted->GetEntries() - 2; iString++) {
				Objstring0 = (TObjString *) splitted->At(iString);
				string0 += "_";
				string0 += Objstring0->GetString();
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(splitted->GetEntries() - 2);
			TObjString *Objstring2 = (TObjString *) splitted->At(splitted->GetEntries() - 1);


			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			string0.ReplaceAll("invMassSigma", "");

			TCut cutEle_1("invMassSigma" + string0 + ">= " + string1);
			TCut cutEle_2("invMassSigma" + string0 + "< " + string2);

			cut_string += cutEle_1 && cutEle_2;
			cutSet.insert(TString(cutEle_1));
			cutSet.insert(TString(cutEle_2));
			delete splitted;
			continue;
		}

		//--------------- invMassRelSigma
		if(string.Contains("invMassRelSigma")) {
			TObjArray *splitted = string.Tokenize("_");
			if(splitted->GetEntries() < 3) {
				std::cerr << "ERROR: incomplete invMassSigma region definition" << std::endl;
				continue;
			}

			TObjString *Objstring0 = (TObjString *) splitted->At(0);
			TString string0 = Objstring0->GetString();
			for(int i = 1; i < splitted->GetEntries() - 2; i++) {
				Objstring0 = (TObjString *) splitted->At(i);
				string0 += "_";
				string0 += Objstring0->GetString();
			}
			TObjString *Objstring1 = (TObjString *) splitted->At(splitted->GetEntries() - 2);
			TObjString *Objstring2 = (TObjString *) splitted->At(splitted->GetEntries() - 1);


			TString string1 = Objstring1->GetString();
			TString string2 = Objstring2->GetString();

			string0.ReplaceAll("invMassRelSigma", "");

			TCut cutEle_1("invMassSigma" + string0 + "/invMass" + string0 + ">= " + string1);
			TCut cutEle_2("invMassSigma" + string0 + "/invMass" + string0 + "< " + string2);

			cut_string += cutEle_1 && cutEle_2;
			cutSet.insert(TString(cutEle_1));
			cutSet.insert(TString(cutEle_2));
			delete splitted;
			continue;
		}


		if(string.CompareTo("trigger") == 0) {
			cutSet.insert(TString("HLTfire==1"));
			continue;
		}
		if(string.CompareTo("notrigger") == 0) {
			cutSet.insert(TString("HLTfire==0"));
			continue;
		}


		//------------------------------ categorie particolari
		if(string.CompareTo("RefReg", TString::kIgnoreCase) == 0 || string.CompareTo("REF_REG") == 0 || string.CompareTo("EBRefReg", TString::kIgnoreCase) == 0) {
			cutSet.insert(TString(EB_cut));
			cutSet.insert(TString(EBRefReg_cut));
			continue;
		}
		if(string.CompareTo("EERefReg", TString::kIgnoreCase) == 0) {
			cutSet.insert(TString(EE_cut));
			cutSet.insert(TString(EERefReg_cut));
			continue;
		}

//     if(string.Contains("CenterModule")){
//       cut_string+=EB_cut;
//       if(string.Contains("SingleEle")) cut_string+= (CenterModule_ele1_cut && !CenterModule_ele2_cut) || (CenterModule_ele2_cut && !CenterModule_ele1_cut);
//       else cut_string += CenterModule_cut;
//       continue;
//     }

//     if(string.Contains("NoBorderModule")){
//       cut_string += EB_cut;
//       if(string.Contains("SingleEle")) cut_string+= (NoBorderModule_ele1_cut && !NoBorderModule_ele2_cut) || (NoBorderModule_ele2_cut && !NoBorderModule_ele1_cut);
//       else cut_string += NoBorderModule_cut;
//       continue;
//     }

//     if(string.CompareTo("GAP5")==0){
//       cut_string += (!CenterModule_ele1_cut) && (!CenterModule_ele2_cut);
//       continue;
//     }

//     if(string.CompareTo("GAP1")==0){
//       cut_string += (!NoBorderModule_ele1_cut) && (!NoBorderModule_ele2_cut);
//       continue;
//     }

		if(string.Contains("TB")) {
			if(string.Contains("!")) cut_string += !TB_cut;
			else cut_string += TB_cut;
			continue;
		}

		if(string.Contains("alphaSM")) {
			TCut c;
			if(string.Contains("SingleEle")) c += ( (alphaSM_ele1_cut && !alphaSM_ele2_cut) ||
				                                        (!alphaSM_ele1_cut && alphaSM_ele2_cut)
				                                      );
			else {
				if(string.Contains("!")) c += (!alphaSM_ele1_cut && !alphaSM_ele2_cut);
				else  c += (alphaSM_ele1_cut && alphaSM_ele2_cut);
			}

			cutSet.insert(TString(c));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EBlowEtaBad") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EBlowEtaBad_ele2_cut) || (EBlowEtaBad_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EBlowEtaBad_ele2_cut) || (EBlowEtaBad_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EBhighEtaBad") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EBhighEtaBad_ele2_cut) || (EBhighEtaBad_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EBhighEtaBad_ele2_cut) || (EBhighEtaBad_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EBhighEtaGold") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EBhighEtaGold_ele2_cut) || (EBhighEtaGold_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EBhighEtaGold_ele2_cut) || (EBhighEtaGold_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EElowEtaGold") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EElowEtaGold_ele2_cut) || (EElowEtaGold_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EElowEtaGold_ele2_cut) || (EElowEtaGold_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EElowEtaBad") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EElowEtaBad_ele2_cut) || (EElowEtaBad_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EElowEtaBad_ele2_cut) || (EElowEtaBad_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EEhighEtaBad") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EEhighEtaBad_ele2_cut) || (EEhighEtaBad_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EEhighEtaBad_ele2_cut) || (EEhighEtaBad_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}

		if(string.CompareTo("EBlowEtaGold_EEhighEtaGold") == 0) {
			cut_string += ((EBlowEtaGold_ele1_cut && EEhighEtaGold_ele2_cut) || (EEhighEtaGold_ele1_cut && EBlowEtaGold_ele2_cut));
			cutSet.insert(TString((EBlowEtaGold_ele1_cut && EEhighEtaGold_ele2_cut) || (EEhighEtaGold_ele1_cut && EBlowEtaGold_ele2_cut)));
			continue;
		}


		//------------------------------ VARIE ED EVENTUALI

		//--------------- generic branch
//     TObjArray *splitted = string.Tokenize("_");
//     if(splitted->GetEntries() > 1 ){
//       std::cerr << "ERROR: incomplete energySC region definition" << std::endl;
// 	continue;
//       }
//       TObjString *Objstring1 = (TObjString *) splitted->At(1);
//       TObjString *Objstring2 = (TObjString *) splitted->At(2);

//       TString string1 = Objstring1->GetString();
//       TString string2 = Objstring2->GetString();

//       TCut cutEle1_1("energySCEle_ele1 >= "+string1);
//       TCut cutEle1_2("energySCEle_ele1 <  "+string2);
//       TCut cutEle2_1("energySCEle_ele2 >= "+string1);
//       TCut cutEle2_2("energySCEle_ele2 <  "+string2);

//       TCut ele1_cut = cutEle1_1 && cutEle1_2;
//       TCut ele2_cut = cutEle2_1 && cutEle2_2;

//       if(string.Contains("SingleEle")){
// 	cut_string += ele1_cut || ele2_cut;
// 	cutSet.insert(TString(ele1_cut || ele2_cut));
//       } else {
// 	cut_string+=ele1_cut && ele2_cut;
// 	cutSet.insert(TString(ele1_cut && ele2_cut));
//       }

//       delete splitted;
//       continue;
//     }


		if(string.Contains("fiducial")) {
			cut_string += fiducial_cut;
			cutSet.insert(TString(fiducial_cut));
			continue;
		}

		if(string.Contains("noPF")) {
			// noPF electrons for the calibration
			cut_string += noPFEle_cut;
			cutSet.insert(TString(noPFEle_cut));
			continue;
		}

		std::cout << "[WARNING] Region " << string << " not found" << std::endl;

	}

#ifdef DEBUG
	std::cout << "Cut string: " << std::endl;
	for(std::set<TString>::const_iterator itr = cutSet.begin();
	        itr != cutSet.end();
	        itr++) {
		TString cut_string = *itr;
		std::cout << cut_string << std::endl;
	}
	//  cut_string.Print();
#endif



#ifdef DEBUG
	std::cout << "Cut string = " << cut_string << std::endl;
#endif

	return cutSet;
}


TString ElectronCategory_class::GetBranchNameRooFit(TString region)
{


	return "";
}

std::set<TString> ElectronCategory_class::GetBranchNameNtuple(TString region)
{

	std::set<TString> branchNames;

	TString cut(GetCut(region, false, 1, false));

	TPRegexp reg("[|&!]+");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("[><=%]+");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("\\[0\\]");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("[- .0-9]{2,}");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("abs[ ]*\\(");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("cosh[ ]*\\(");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("[\\(\\)]{1,}");
	reg.Substitute(cut, "", "g");
	reg = TPRegexp("[*/+-]{1,}");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("[ ]{1,}");
	reg.Substitute(cut, " ", "g");
	reg = TPRegexp("SingleEle");
	reg.Substitute(cut, " ", "g");
	cut.ReplaceAll("scaleEle", "");

	TObjArray *splitted = cut.Tokenize(" ");
	for(int i = 0; i < splitted->GetEntries(); i++) {
		TObjString *Objstring1 = (TObjString *) splitted->At(i);

		TString string1 = Objstring1->GetString();

		branchNames.insert(string1);
	}
//   std::cout << cut << std::endl;
//   if(cut.Contains("etaEle")) branchNames.insert("etaEle");
//   if(cut.Contains("ieta")) branchNames.insert("seedXSCEle");
//   if(cut.Contains("iphi")) branchNames.insert("seedYSCEle");
//   if(cut.Contains("R9Ele")) branchNames.insert("R9Ele");
//   if(cut.Contains("CL")) branchNames.insert("classificationEle");
//   if(cut.Contains("runNumber")) branchNames.insert("runNumber");
//   if(cut.Contains("recoFlagsEle")) branchNames.insert("recoFlagsEle");
//   if(cut.Contains("etaSCEle")) branchNames.insert("etaSCEle");
//   if(cut.Contains("energySCEle")) branchNames.insert("energySCEle");

#ifdef DEBUG
	for(std::set<TString>::const_iterator itr = branchNames.begin();
	        itr != branchNames.end();
	        itr++) {
		std::cout << "[DEBUG] " << region << "\t" << *itr << std::endl;
	}
#endif
	return branchNames;
}
