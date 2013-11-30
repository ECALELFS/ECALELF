#ifndef EnergyScaleCorrection_class_hh
#define EnergyScaleCorrection_class_hh
#include <TString.h>
#include <iostream>
//#include "../include/root_header.h"
#include<fstream>
#include<map>
#include<math.h>
//#include "correction_class.h"
#include <TChain.h>
#include <TRandom3.h>
#include <string>



class correctionCategory_class{
 public:
  unsigned int runmin;
  unsigned int runmax;
  float r9min;
  float r9max;
  float etmin;
  float etmax;
  float etamin;
  float etamax;
  std::string category;

  float scale;
  float scale_err;
  //  float smearing;
  //float smearing_err;

 public:
  inline  correctionCategory_class(const unsigned int runNumber, const float etaEle, const float R9Ele, const float EtEle){
    runmin=runNumber; runmax=runNumber;
    etamin=fabs(etaEle); etamax=fabs(etaEle);
    r9min=R9Ele; r9max=R9Ele;
    etmin=EtEle; etmax=EtEle;
  }

  correctionCategory_class(TString category_);

/*   friend bool operator < (const correctionCategory_class& a, const correctionCategory_class& b){ */
/*     if(a.runmin < b.runmin) return true;   */
/*     if(a.runmax > b.runmax) return false;  */

/*     if(a.etamin < b.etamin) return true;  */
/*     if(a.etamax > b.etamax) return false; */

/*     if(a.r9min  < b.r9min) return true;  */
/*     if(a.r9max  > b.r9max) return  false; */

/*     if(a.etmin  < b.etmin) return true;   */
/*     if(a.etmax  > b.etmax) return  false; */
/*     return false; */

/*   };     */


  //  inline correction_class(void){};
  bool operator<(const correctionCategory_class& b) const;
/*   friend bool less(const correctionCategory_class& a, const correctionCategory_class& b) const{ */
/*     return (a < b); */
/*   }; */

  friend ostream& operator << (ostream& os, const correctionCategory_class a){
    os <<  a.runmin << " " << a.runmax 
       << "\t" << a.etamin << " "<<a.etamax 
       << "\t" << a.r9min << " "<<a.r9max 
       << "\t" << a.etmin << " " << a.etmax;
    return os;
  };
/*   bool operator == (const unsigned int runNumber, const float etaEle, const float R9Ele, const float EtEle){ */
/*     return (runNumber > runmin && runNumber < runmax  */
/* 	    && fabs(etaEle) > etamin && fabs(etaEle) < etamax  */
/* 	    && R9Ele > r9min && R9Ele < r9max  */
/* 	    && EtEle > etmin && EtEle < etmax); */
/*   }; */

/*   bool operator < (const unsigned int runNumber, const float etaEle, const float R9Ele, const float EtEle){ */
/*     return (runNumber < runmin && runNumber < runmax */
/* 	    && fabs(etaEle) > etamin && fabs(etaEle) < etamax */
/* 	    && R9Ele > r9min && R9Ele < r9max */
/* 	    && EtEle > etmin && EtEle < etmax); */
/*   }; */
};


//typedef std::map < TString, std::pair<double, double> > correction_map_t;
typedef std::map < correctionCategory_class, std::pair<double, double> > correction_map_t;


typedef struct{
  int runMax;
  correction_map_t correction_map;
} correction_t;

class EnergyScaleCorrection_class{

 public:
  bool isHggCat;
  bool noCorrections, noSmearings;
  TString correctionType, smearingType;
 public:
  //  EnergyScaleCorrection_class(TString correctionFileName, bool isHggCat_=false);
  EnergyScaleCorrection_class(TString correctionFileName, TString correctionType_, 
			      TString smearingFileName="", TString smearingType_="");
  ~EnergyScaleCorrection_class(void);
  void ReadFromFile(TString filename);
  float getScaleOffset(int runNumber, bool isEBEle, double R9Ele, double etaSCEle, double EtEle); // deprecated
  float ScaleCorrection(int runNumber, bool isEBEle, double R9Ele, double etaSCEle, 
			double EtEle,
			int nPV, float nPVmean);
  TTree *GetCorrTree(TChain *tree, bool fastLoop=true, 
		     TString runNumberBranchName="runNumber",
		     TString R9EleBranchName="R9Ele",
		     TString etaEleBranchName="etaEle",
		     TString etaSCEleBranchName="etaSCEle",
		     TString energySCEleBranchName="energySCEle",
		     TString nPVBranchName="nPV");

 private:
  TString GetElectronCategory(bool isEBEle, double R9Ele, double etaSCEle);
  float GetMean_nPV(TChain *tree, bool fastLoop, TString nPVBranchName);

  void Add(TString category_, int runMin_, int runMax_, double deltaP_, double err_deltaP_);
 public:
  std::map< int, correction_t >::const_iterator runCorrection_itr;
  std::map< int, correction_t >::const_iterator FindRunCorrection_itr(int runNumber);
  std::map< int, correction_t > runMin_map;

  //============================== smearings
 private:
  TRandom3 *rgen_;

  correction_map_t smearings;
  void AddSmearing(TString category_, int runMin_, int runMax_, //double smearing_, double err_smearing_);
		   double constTerm, double err_constTerm, double alpha, double err_alpha);

  float getSmearing(float energy, bool isEBEle, float R9Ele, float etaSCEle);


 public:
  float getSmearingSigma(float energy, bool isEBEle, float R9Ele, float etaSCEle);
  void ReadSmearingFromFile(TString filename);
  TTree *GetSmearTree(TChain *tree, bool fastLoop, 
		      TString energyEleBranchName,
		      TString R9EleBranchName="R9Ele",
		      TString etaEleBranchName="etaEle",
		      TString etaSCEleBranchName="etaSCEle"
		      );

};


#endif
