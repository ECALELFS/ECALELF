#include "Calibration/EOverPCalibration/interface/LHEReader.h"



LHEReader::LHEReader(const std::string& LHEFileName)
{
	m_LHEFile = new std::ifstream(LHEFileName.c_str(), std::ios::in);

	std::string buffer;
	while(1) {
		getline(*m_LHEFile, buffer);
		if( buffer == "<event>" ) break;
	}
}

// ----------------------------------------------------------------





LHEReader::~LHEReader()
{}

// ----------------------------------------------------------------






bool LHEReader::GetNextEvent()
{
	IDUP.clear();
	ISTUP.clear();
	MOTH1UP.clear();
	MOTH2UP.clear();
	ICOL1UP.clear();
	ICOL2UP.clear();
	PXUP.clear();
	PYUP.clear();
	PZUP.clear();
	PEUP.clear();
	PMUP.clear();
	VTIMUP.clear();
	SPINUP.clear();



	//common event information
	(*m_LHEFile) >> NUP >> IDPRUP >> XWGTUP >> SCALUP >> AQEDUP >> AQCDUP;

	// NUP lines, one for each particle
	int idup, istup, moth1up, moth2up, icol1up, icol2up;
	float pxup, pyup, pzup, peup, pmup, vtimup, spinup;

	for(int i = 0; i < NUP; ++i) {
		(*m_LHEFile) >> idup >> istup >> moth1up >> moth2up >> icol1up >> icol2up >> pxup >> pyup >> pzup >> peup >> pmup >> vtimup >> spinup;

		IDUP.push_back(idup);
		ISTUP.push_back(istup);
		MOTH1UP.push_back(moth1up);
		MOTH2UP.push_back(moth2up);
		ICOL1UP.push_back(icol1up);
		ICOL2UP.push_back(icol2up);
		PXUP.push_back(pxup);
		PYUP.push_back(pyup);
		PZUP.push_back(pzup);
		PEUP.push_back(peup);
		PMUP.push_back(pmup);
		VTIMUP.push_back(vtimup);
		SPINUP.push_back(spinup);
	}


	// Goto next event
	bool nextEventFound = false;
	std::string buffer;
	while( !(*m_LHEFile).eof() ) {
		getline(*m_LHEFile, buffer);
		if( buffer == "<event>" ) {
			nextEventFound = true;
			break;
		}
	}


	return nextEventFound;
}

// ----------------------------------------------------------------






void LHEReader::PrintEvent()
{
	std::cout << "<event>" << std::endl;

	std::cout << " " << NUP
	          << " " << IDPRUP
	          << " " << std::scientific << std::setprecision(7) << std::setw(14) << XWGTUP
	          << " " << std::scientific << std::setprecision(7) << std::setw(14) << SCALUP
	          << " " << std::scientific << std::setprecision(7) << std::setw(14) << AQEDUP
	          << " " << std::scientific << std::setprecision(7) << std::setw(14) << AQCDUP
	          << std::endl;

	for(int i = 0; i < NUP; ++i) {
		std::cout << " " << std::fixed << std::setprecision(0) << std::setw(7) << IDUP.at(i)
		          << " " << std::fixed << std::setprecision(0) << std::setw(4) << ISTUP.at(i)
		          << " " << std::fixed << std::setprecision(0) << std::setw(4) << MOTH1UP.at(i)
		          << " " << std::fixed << std::setprecision(0) << std::setw(4) << MOTH2UP.at(i)
		          << " " << std::fixed << std::setprecision(0) << std::setw(4) << ICOL1UP.at(i)
		          << " " << std::fixed << std::setprecision(0) << std::setw(4) << ICOL2UP.at(i)
		          << " " << std::scientific << std::setprecision(11) << std::setw(18) << PXUP.at(i)
		          << " " << std::scientific << std::setprecision(11) << std::setw(18) << PYUP.at(i)
		          << " " << std::scientific << std::setprecision(11) << std::setw(18) << PZUP.at(i)
		          << " " << std::scientific << std::setprecision(11) << std::setw(18) << PEUP.at(i)
		          << " " << std::scientific << std::setprecision(11) << std::setw(18) << PMUP.at(i)
		          << " " << std::scientific << std::setprecision(5) << std::setw(10) << VTIMUP.at(i)
		          << " " << std::fixed << std::setprecision(1) << std::setw(4) << SPINUP.at(i)
		          << std::endl;
	}

	std::cout << "<event>" << std::endl;
}

// ----------------------------------------------------------------
