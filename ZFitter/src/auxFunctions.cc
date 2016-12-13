#include "../interface/auxFunctions.hh"
#include <iostream>

std::string energyBranchNameFromInvMassName(std::string invMass_var)
{
	std::string energyBranchName = "";
	if(invMass_var == "invMass_ECAL_ele") energyBranchName = "energy_ECAL_ele";
	else if(invMass_var == "invMass_ECAL_pho") energyBranchName = "energy_ECAL_pho";
	else if(invMass_var == "invMass_5x5SC") energyBranchName = "energy_5x5SC";
	else if(invMass_var == "invMass") energyBranchName = "energyEle";
	else if(invMass_var == "invMass_rawSC") energyBranchName = "rawEnergySCEle";
	else if(invMass_var == "invMass_SC_corr") energyBranchName = "energySCEle_corr";
	else if(invMass_var == "invMass_SC_pho_regrCorr") energyBranchName = "energySC_pho_regrCorr";
	else {
		std::cerr << "Energy branch name not define for invariant mass branch: " << invMass_var << std::endl;
		exit(1);
	}
	return energyBranchName;
}
