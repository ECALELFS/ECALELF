#ifndef LHEReader_h
#define LHEReader_h

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>



class LHEReader
{
public:

	//! ctor
	LHEReader(const std::string& LHEFileName);

	//! dtor
	~LHEReader();

	//! methods
	bool GetNextEvent();
	void PrintEvent();

	int GetNUP()
	{
		return NUP;
	};
	int GetIDPRUP()
	{
		return IDPRUP;
	};
	float GetXWGTUP()
	{
		return XWGTUP;
	};
	float GetSCALUP()
	{
		return SCALUP;
	};
	float GetAQEDUP()
	{
		return AQEDUP;
	};
	float GetAQCDUP()
	{
		return AQCDUP;
	};

	std::vector<int> GetIDUP()
	{
		return IDUP;
	};
	std::vector<int> GetISTUP()
	{
		return ISTUP;
	};
	std::vector<int> GetMOTH1UP()
	{
		return MOTH1UP;
	};
	std::vector<int> GetMOTH2UP()
	{
		return MOTH2UP;
	};
	std::vector<int> GetICOL1UP()
	{
		return ICOL1UP;
	};
	std::vector<int> GetICOL2UP()
	{
		return ICOL2UP;
	};
	std::vector<float> GetPXUP()
	{
		return PXUP;
	};
	std::vector<float> GetPYUP()
	{
		return PYUP;
	};
	std::vector<float> GetPZUP()
	{
		return PZUP;
	};
	std::vector<float> GetPEUP()
	{
		return PEUP;
	};
	std::vector<float> GetPMUP()
	{
		return PMUP;
	};
	std::vector<float> GetVTIMUP()
	{
		return VTIMUP;
	};
	std::vector<float> GetSPINUP()
	{
		return SPINUP;
	};



private:

	int NUP;
	int IDPRUP;
	float XWGTUP;
	float SCALUP;
	float AQEDUP;
	float AQCDUP;

	std::ifstream* m_LHEFile;
	std::vector<int> IDUP;
	std::vector<int> ISTUP;
	std::vector<int> MOTH1UP;
	std::vector<int> MOTH2UP;
	std::vector<int> ICOL1UP;
	std::vector<int> ICOL2UP;
	std::vector<float> PXUP;
	std::vector<float> PYUP;
	std::vector<float> PZUP;
	std::vector<float> PEUP;
	std::vector<float> PMUP;
	std::vector<float> VTIMUP;
	std::vector<float> SPINUP;
};

#endif
