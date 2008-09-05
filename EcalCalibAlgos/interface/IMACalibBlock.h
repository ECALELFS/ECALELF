#ifndef __CINT__
#ifndef EcalCalibBlock_H
#define EcalCalibBlock_H

#include <map>
#include <string>
#include <vector>

#include "CLHEP/Matrix/GenMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "Calibration/Tools/interface/InvMatrixCommonDefs.h"
#include "Calibration/EcalCalibAlgos/interface/VEcalCalibBlock.h"

/** \class EcalCalibBlock
 
    \brief element for the single ECAL block intercalibration  

    $Date: 2008/02/25 17:50:28 $
    $Revision: 1.2 $
    $Id: IMACalibBlock.h,v 1.2 2008/02/25 17:50:28 malberti Exp $ 
    \author $Author: malberti $
*/
class IMACalibBlock : public VEcalCalibBlock
{
  public :
    //! ctor
    IMACalibBlock (const int) ;
    //! dtor
    ~IMACalibBlock () ;
    
    //! insert an entry
    void Fill (std::map<int,double>::const_iterator,
               std::map<int,double>::const_iterator,
               double pTk,
               double pSubtract,
               double sigma = 1.) ;

    //! reset the chi2 matrices
    void reset () ;
    //! solve the chi2 linear system
    int solve (int usingBlockSolver, double min, double max) ;
  private :
    
    //! give the size of a chi2 matrix
    int evalX2Size () ;
    //! complete the triangolar chi2 matrix to a sym one
    void complete () ;
    //! copy a vector into a CLHEP object
    void riempiMtr (const std::vector<double> & piena, CLHEP::HepMatrix & vuota) ;
    //! copy a vector into a CLHEP object
    void riempiVtr (const std::vector<double> & pieno, CLHEP::HepVector & vuoto) ;
    //! fill the coefficients map from the CLHEP vector solution
    void fillMap (const CLHEP::HepVector & result) ; 
  
  private :  

    //! vector for the chi2 inversion
    std::vector<double> m_kaliVector ; 
    //! matrix for the chi2 inversion
    std::vector<double> m_kaliMatrix ;
} ;


#endif
#endif

/* TODOS and dubios
   ----------------

- fare l'inversione finale
- uso le CLHEP
- ritorno CLHEP o un vector<double>? forse la seconda e' meglio
- oppure una mappa con uindex, coeff? cosi' poi si possono incollare insieme
  tutte le mappe facilmente?
- CLHEP hanno qualche metodo utile per usare STL che non ho visto?
- rem se ho gia' invertito con una variabile: ha senso?

*/
