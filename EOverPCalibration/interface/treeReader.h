#ifndef treeReader_h
#define treeReader_h

#include <map>
#include <vector>
#include <string>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "Math/Vector4D.h"
#include "Math/Vector3D.h"
#include "TBranch.h"
#include "TBranchElement.h"
#include "TLeaf.h"


class treeReader
{
public:

	treeReader (TTree *, bool verbosity = false) ;
	~treeReader () ;

	void GetEntry (int iEvent)
	{
		m_tree->GetEntry (iEvent) ;
	} ;
	int GetEntries ()
	{
		return m_tree->GetEntries () ;
	} ;

	std::vector<ROOT::Math::XYZVector>*  Get3V    (const std::string &name);
	std::vector<ROOT::Math::XYZTVector>* Get4V    (const std::string &name);
	std::vector<double>*                 GetDouble(const std::string &name);
	std::vector<float>*                  GetFloat (const std::string &name);
	std::vector<int>*                    GetInt   (const std::string &name);
	std::vector<bool>*                    GetBool  (const std::string &name);
	std::vector<std::string>*            GetString(const std::string &name);

	ROOT::Math::XYZVector*  get3V    (const std::string &name);
	ROOT::Math::XYZTVector* get4V    (const std::string &name);
	double*                 getDouble(const std::string &name);
	float*                  getFloat (const std::string &name);
	int*                    getInt   (const std::string &name);
	bool*                    getBool  (const std::string &name);
	std::string*            getString(const std::string &name);

private:

	std::map <std::string, std::vector<ROOT::Math::XYZVector> * >  m_3Vvectors ;
	std::map <std::string, std::vector<ROOT::Math::XYZTVector> * > m_4Vvectors ;
	std::map <std::string, std::vector<double> * >                 m_Dvectors ;
	std::map <std::string, std::vector<float> * >                  m_Fvectors ;
	std::map <std::string, std::vector<int> * >                    m_Ivectors ;
	std::map <std::string, std::vector<bool> * >                    m_Bvectors ;
	std::map <std::string, std::vector<std::string> * >            m_Svectors ;

	std::map <std::string, ROOT::Math::XYZVector * >  k_3Vvectors ;
	std::map <std::string, ROOT::Math::XYZTVector * > k_4Vvectors ;
	std::map <std::string, double * >                 k_Dvectors ;
	std::map <std::string, float * >                  k_Fvectors ;
	std::map <std::string, int * >                    k_Ivectors ;
	std::map <std::string, bool * >                    k_Bvectors ;
	std::map <std::string, std::string * >            k_Svectors ;


	TTree * m_tree ;
	bool m_verbosity ;

} ;

#endif

