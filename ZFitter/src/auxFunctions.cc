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
	else {
		std::cerr << "Energy branch name not define for invariant mass branch: " << invMass_var << std::endl;
		exit(1);
	}
	return energyBranchName;
}

std::string invMassNameFromEnergyBranchName(std::string energyBranchName)
{
	std::string invMass_var = "";
	if( energyBranchName == "energy_ECAL_ele") invMass_var = "invMass_ECAL_ele";
	else if(energyBranchName == "energy_ECAL_pho") invMass_var = "invMass_ECAL_pho";
	else if(energyBranchName == "energy_5x5SC") invMass_var = "invMass_5x5SC";
	else if(energyBranchName == "energyEle") invMass_var = "invMass";
	else if(energyBranchName == "rawEnergySCEle") invMass_var = "invMass_rawSC";
	else {
		std::cerr << "invMass branch name not define for energy branch: " << energyBranchName << std::endl;
		exit(1);
	}
	return invMass_var;
}
