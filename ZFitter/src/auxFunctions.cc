#include "../interface/auxFunctions.hh"
#include <iostream>

std::string energyBranchNameFromInvMassName(std::string invMass_var)
{

	std::string energyBranchName = "";
	if(invMass_var == "invMass_SC_regrCorr_ele") energyBranchName = "energySCEle_regrCorr_ele";
	else if(invMass_var == "invMass_SC_regrCorr_pho") energyBranchName = "energySCEle_regrCorr_pho";
	else if(invMass_var == "invMass_regrCorr_fra") energyBranchName = "energyEle_regrCorr_fra";
	else if(invMass_var == "invMass_regrCorr_egamma") energyBranchName = "energyEle_regrCorr_egamma";
	else if(invMass_var == "invMass_SC") energyBranchName = "energySCEle";
	else if(invMass_var == "invMass_SC_must") energyBranchName = "energySCEle_must";
	else if(invMass_var == "invMass_SC_must_regrCorr_ele") energyBranchName = "energySCEle_must_regrCorr_ele";
	else if(invMass_var == "invMass_rawSC") energyBranchName = "rawEnergySCEle";
	else if(invMass_var == "invMass_rawSC_esSC") energyBranchName = "rawEnergySCEle+esEnergySCEle";
	else if(invMass_var == "invMass_SC_corr") energyBranchName = "energySCEle_corr";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV4_ele") energyBranchName = "energySCEle_regrCorrSemiParV4_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV4_pho") energyBranchName = "energySCEle_regrCorrSemiParV4_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV5_ele") energyBranchName = "energySCEle_regrCorrSemiParV5_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV5_pho") energyBranchName = "energySCEle_regrCorrSemiParV5_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV6_ele") energyBranchName = "energySCEle_regrCorrSemiParV6_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV6_pho") energyBranchName = "energySCEle_regrCorrSemiParV6_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV7_ele") energyBranchName = "energySCEle_regrCorrSemiParV7_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV7_pho") energyBranchName = "energySCEle_regrCorrSemiParV7_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV8_ele") energyBranchName = "energySCEle_regrCorrSemiParV8_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiParV8_pho") energyBranchName = "energySCEle_regrCorrSemiParV8_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV6_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV6_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV6_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV6_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV7_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV7_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV7_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV7_pho";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV8_ele") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV8_ele";
	else if(invMass_var == "invMass_SC_regrCorrSemiPar7TeVtrainV8_pho") energyBranchName = "energySCEle_regrCorrSemiPar7TeVtrainV8_pho";
	else if(invMass_var == "invMass_efull5x5") energyBranchName = "efull5x5SCEle";
	else {
		std::cerr << "Energy branch name not define for invariant mass branch: " << invMass_var << std::endl;
		exit(1);
	}
	return energyBranchName;
}
