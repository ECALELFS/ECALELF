/** \file HouseholderDecomposition.cc
 *
 * $Date: 2007/03/13$
 * $Revision: 2.0 $
 *
 * \author Lorenzo Agostino, R.Ofierzynski, CERN
 */

#include "Calibration/Tools/interface/HouseholderDecomposition.h"
#include <cfloat>
#include <cmath>


HouseholderDecomposition::HouseholderDecomposition(int squareMode_, int mineta_, int maxeta_, int minphi_, int maxphi_)
  :squareMode(squareMode_), mineta(mineta_), maxeta(maxeta_), minphi(minphi_), maxphi(maxphi_), countEvents(0)
{
  Neta = maxeta - mineta + 1;
  if (mineta * maxeta < 0) Neta--; // there's no eta index = 0
  Nphi = maxphi - minphi + 1;
  if (Nphi <0) Nphi += 360;
  
  Nchannels = Neta * Nphi; // no. of channels, get it from edges of the region

  Nxtals = squareMode*squareMode; // no. of xtals in one event

  sigmaReplacement = 0.00001; // the sum of columns is replaced by this value in case it is zero (e.g. dead crystal)

}

HouseholderDecomposition::~HouseholderDecomposition()
{
}


vector<float> HouseholderDecomposition::runRegional(const vector<vector<float> >& eventMatrix, const vector<int>& VmaxCeta, const vector<int>& VmaxCphi, const vector<float>& energyVector, const int& nIter, const int& regLength)
{
  // make regions
  makeRegions(regLength);

  Nevents = eventMatrix.size(); // Number of events to calibrate with

  vector<float> totalSolution(Nchannels,1.);
  vector<float> iterSolution(Nchannels,1.);
  vector<vector<float> > myEventMatrix(eventMatrix);
  vector<float> myEnergyVector(energyVector);

  int i, j;

  // loop over nIter
  for (int iter=1;iter<=nIter;iter++) 
    {
      // loop over regions
      for (int ireg=0; ireg<regMinEta.size(); ireg++)
	{
	  vector<float> regIterSolution, regEnergyVector;
	  vector<int> regVmaxCeta, regVmaxCphi;
	  vector<vector<float> > regEventMatrix;

	  // initialize new instance with regional min,max indices
	  HouseholderDecomposition regionalHH(squareMode,regMinEta[ireg],regMaxEta[ireg],regMinPhi[ireg],regMaxPhi[ireg]);

	  // copy all events in region into new eventmatrix, energyvector, VmaxCeta, VmaxCphi
	  for (int ia=0; ia<VmaxCeta.size(); ia++)
	    {
	      if ((VmaxCeta[ia] >= regMinEta[ireg]) && (VmaxCeta[ia] <= regMaxEta[ireg])
		  && (VmaxCphi[ia] >= regMinPhi[ireg]) && (VmaxCphi[ia] <= regMaxPhi[ireg]))
		{
		  // save event, calculate new eventmatrix(truncated) and energy
		  regVmaxCeta.push_back(VmaxCeta[ia]);
		  regVmaxCphi.push_back(VmaxCphi[ia]);

		  vector<float> regEvent = myEventMatrix[ia];
		  float regEnergy = energyVector[ia];
		  for (int i2=0; i2<Nxtals; i2++)
		    {
		      int iFullReg = regionalHH.indexSqr2Reg(i2, VmaxCeta[ia], VmaxCphi[ia]);
		      if (iFullReg <0) // crystal outside
			{
			  regEnergy -= regEvent[i2];
			  regEvent[i2] = 0.;
			}
		    }
		  regEventMatrix.push_back(regEvent);
		  regEnergyVector.push_back(regEnergy);
		}
	    }

	  // calibrate
	  //	  cout << "HouseholderDecomposition::runRegional - Starting calibration of region " << ireg << ": eta " 
	  //	       << regMinEta[ireg] << " to " << regMaxEta[ireg] << ", phi " << regMinPhi[ireg] << " to " << regMaxPhi[ireg] << endl;
	  regIterSolution = regionalHH.iterate(regEventMatrix, regVmaxCeta, regVmaxCphi, regEnergyVector);
	  //	  cout << "HouseholderDecomposition::runRegional - calibration of region finished. " << endl;

	  // save solution into global iterSolution
	  // don't forget to delete the ones that are on the border !
	  for (int i1=0; i1<regIterSolution.size(); i1++)
	    {
	      int regFrame = regLength/2;
	      int currRegEtaRange = regMaxEta[ireg] - regMinEta[ireg] + 1;
	      int currRegPhiRange = regMaxPhi[ireg] - regMinPhi[ireg] + 1;
	      int currRegEta = i1 / currRegPhiRange + regMinEta[ireg];
	      int currRegPhi = i1 % currRegPhiRange + regMinPhi[ireg];
	      int newindex = -100;
	      // if crystal well inside:
	      if ( (currRegEta >= (regMinEta[ireg]+regFrame*(!(regMinEta[ireg]==mineta))) ) &&
		   (currRegEta <= (regMaxEta[ireg]-regFrame*(!(regMaxEta[ireg]==maxeta))) ) &&
		   (currRegPhi >= (regMinPhi[ireg]+regFrame*(!(regMinPhi[ireg]==minphi))) ) &&
		   (currRegPhi <= (regMaxPhi[ireg]-regFrame*(!(regMaxPhi[ireg]==maxphi))) ) )
		{
		  newindex = (currRegEta-mineta)*Nphi + currRegPhi-minphi;
		  iterSolution[newindex] = regIterSolution[i1];
		}
	    }
	} // end loop over regions

      if (iterSolution.empty()) return iterSolution;
      
      // re-calibrate eventMatrix with solution
      for (int ievent = 0; ievent<Nevents; ievent++)
	{
	  myEventMatrix[ievent] = recalibrateEvent(myEventMatrix[ievent], VmaxCeta[ievent], VmaxCphi[ievent], iterSolution);
	}
      
      // save solution into theCalibVector
      for (int i=0; i<Nchannels; i++) 
	{
	  totalSolution[i] *= iterSolution[i];
	}
      
    } // end loop over nIter

  return totalSolution;

}


vector<float> HouseholderDecomposition::iterate(const vector<vector<float> >& eventMatrix, const vector<int>& VmaxCeta, const vector<int>& VmaxCphi, const vector<float>& energyVector, const int& nIter, const bool& normalizeFlag)
{
  Nevents = eventMatrix.size(); // Number of events to calibrate with

  vector<float> totalSolution(Nchannels,1.);
  vector<float> iterSolution;
  vector<vector<float> > myEventMatrix(eventMatrix);
  vector<float> myEnergyVector(energyVector);

  int i, j;

  // Iterate the correction
  for (int iter=1;iter<=nIter;iter++) 
    {

      // if normalization flag is set, normalize energies
      float sumOverEnergy;
      if (normalizeFlag)
	{
	  float scale = 0.;
	  
	  for (i=0; i<Nevents; i++)
	    {
	      sumOverEnergy = 0.;
	      for (j=0; j<Nxtals; j++) {sumOverEnergy += myEventMatrix[i][j];}
	      sumOverEnergy /= myEnergyVector[i];
	      scale += sumOverEnergy;
	    }
	  scale /= Nevents;
	  
	  for (i=0; i<Nevents; i++) {myEnergyVector[i] *= scale;}	  
	} // end normalize energies



      // now the real work starts:
      iterSolution = iterate(myEventMatrix,VmaxCeta,VmaxCphi,myEnergyVector);

      if (iterSolution.empty()) return iterSolution;

      // re-calibrate eventMatrix with solution
      for (int ievent = 0; ievent<Nevents; ievent++)
	{
	  myEventMatrix[ievent] = recalibrateEvent(myEventMatrix[ievent], VmaxCeta[ievent], VmaxCphi[ievent], iterSolution);
	}

      for (int i=0; i<Nchannels; i++) 
	{
	  // save solution into theCalibVector
	  totalSolution[i] *= iterSolution[i];
	}

    } // end iterate correction

  return totalSolution;
}



vector<float> HouseholderDecomposition::iterate(const vector<vector<float> >& eventMatrix, const vector<int>& VmaxCeta, const vector<int>& VmaxCphi, const vector<float>& energyVectorOrig)
{
  vector<float> solution; 

  Nevents=eventMatrix.size();      // Number of events to calibrate with

  if (Nchannels > Nevents)
    {
      cout << "Householder::runIter(): more channels to calibrate than events available. " << endl;
      cout << "  Nchannels=" << Nchannels << endl;
      cout << "  Nevents=" << Nevents << endl;
      cout << " ******************    ERROR   *********************" << endl;
      return solution; // empty vector
    }

  // input: eventMatrixOrig - the unzipped matrix
  eventMatrixOrig = unzipMatrix(eventMatrix,VmaxCeta,VmaxCphi);

 if (eventMatrixOrig.size() != energyVectorOrig.size())
    {
      cout << "Householder::runIter(): matrix dimensions non-conformant. " << endl;
      cout << "  energyVectorOrig.size()=" << energyVectorOrig.size() << endl;
      cout << "  eventMatrixOrig.size()=" << eventMatrixOrig.size() << endl;
      cout << " ******************    ERROR   *********************" << endl;
      return solution; // empty vector
    }


  int i,j;
  eventMatrixProc = eventMatrixOrig;
  energyVectorProc = energyVectorOrig; // copy energyVectorOrig vector
  vector<float> e(Nchannels);
  alpha.assign(Nchannels,0.);
  pivot.assign(Nchannels,0);
  

  //--------------------
  bool decomposeSuccess = decompose();

  if( !decomposeSuccess ) 
    {
      cout << "Householder::runIter(): Failed: Singular condition in decomposition."<< endl;
      cout << "***************** PROBLEM in DECOMPOSITION *************************"<<endl;
      return solution; // empty vector
    }

  /* DBL_EPSILON: Difference between 1.0 and the minimum float greater than 1.0 */
  float mydbleps = 2.22045e-16;  //DBL_EPSILON;
  float etasqr = mydbleps*mydbleps; 
  //  cout << "LOOK at DBL_EPSILON :" << mydbleps <<endl;


  //--------------------
  // apply transformations to rhs - find solution vector
  solution.assign(Nchannels,0.);
  solve(solution);


  // compute residual vector energyVectorProc
  for (i=0;i<Nevents;i++) 
    {
      energyVectorProc[i] = energyVectorOrig[i];
      for (j=0; j<Nchannels; j++)
	{
	  energyVectorProc[i]-=eventMatrixOrig[i][j]*solution[j];
	}
    }


  //--------------------
  // compute first correction vector e
  solve(e);

  float normy0=0.;
  float norme1=0.;
  float norme0;
    



  for (i=0;i<Nchannels;i++) 
    {
      normy0 += solution[i] * solution[i];
      norme1 += e[i] * e[i];
    }
  
//  cout << "Householder::runIter(): applying first correction";
//  cout << " normy0 = " << normy0;
//  cout << " norme1 = " << norme1 << endl;

  // not attempt at obtaining the solution is made unless the norm of the first
  // correction  is significantly smaller than the norm of the initial solution
  if (norme1>(0.0625*normy0)) 
    {
      //      cout << "Householder::runIter(): first correction is too large. Failed." << endl;
    }

  // improve the solution
  for (i=0; i<Nchannels; i++)
    {
      solution[i]+=e[i];
    }

  //  cout << "Householder::runIter(): improving solution...." << endl;

  //--------------------
  // only continue iteration if the correction was significant
  while (norme1>(etasqr*normy0)) 
    {
      //      cout << "Householder::runIter(): norme1 = " << norme1 << endl;
    
      for (i=0; i<Nevents; i++) 
	{
	  energyVectorProc[i] = energyVectorOrig[i];
	  for (j=0; j<Nchannels; j++)
	    {
	      energyVectorProc[i] -= eventMatrixOrig[i][j] * solution[j];
	    }
	}

    // compute next correction vector
    solve(e);

    norme0 = norme1;
    norme1 = 0.;

    for (i=0;i<Nchannels;i++)
      {
	norme1+=e[i]*e[i];
      }

    // terminate iteration if the norm of the new correction failed to decrease
    // significantly compared to the norm of the previous correction
    if (norme1>(0.0625*norme0)) break;

    // apply correction vector
    for (i=0;i<Nchannels;i++)
      {
	solution[i]+=e[i];
      }
  }

  //clean up
  eventMatrixOrig.clear();
  eventMatrixProc.clear();
  energyVectorProc.clear();
  alpha.clear();
  pivot.clear();

  
  return solution;
}


bool HouseholderDecomposition::decompose()
{
  int i,j,jbar,k;
  float beta,sigma,alphak,eventMatrixkk;
  vector<float> y(Nchannels);
  vector<float> sum(Nchannels);

  //  cout << "Householder::decompose() started" << endl;
  
  for (j=0;j<Nchannels;j++) 
    {
      // jth column sum: squared sum for each crystal
      sum[j]=0.;
      for (i=0;i<Nevents;i++)
	sum[j]+=eventMatrixProc[i][j]*eventMatrixProc[i][j];
      
      // bookkeeping vector
      pivot[j] = j;
    }
  
  for (k=0;k<Nchannels;k++) 
    {
      // kth Householder transformation
      sigma = sum[k];
      jbar = k;
      
      // go through all following columns
      // find the largest sumSquared in the following columns
      for (j=k+1;j<Nchannels;j++) 
	{
	  if (sum[j] > sigma) 
	    {
	      sigma = sum[j];
	      jbar=j;
	    }
	}

      if (jbar != k) 
	{
	  // column interchange:
	  //     interchange within: bookkeeping vector, squaredSum, eventMatrixProc 

	  i = pivot[k];
	  pivot[k]=pivot[jbar];
	  pivot[jbar]=i;

	  sum[jbar]=sum[k];
	  sum[k]=sigma;
	  
	  for (i=0;i<Nevents;i++) 
	    {
	      sigma=eventMatrixProc[i][k];
	      eventMatrixProc[i][k]=eventMatrixProc[i][jbar];
	      eventMatrixProc[i][jbar]=sigma;
	    }
	} // end column interchange

      // now store in sigma the squared sum of the readoutEnergies for this column(crystal)
      sigma=0.;
      for (i=k;i<Nevents;i++)
	{
	  sigma+=eventMatrixProc[i][k]*eventMatrixProc[i][k];
	}

      // found a zero-column, bail out
      if (sigma == 0.) 
	{
//	  cout << "Householder::decompose() failed" << endl;
//	  return false;
	  // rof 14.12.2006: workaround to avoid failure of algorithm because of dead crystals:
	  sigma = sigmaReplacement;
	  //	  cout << "Householder::decompose - found zero column " << jbar << ", replacing sum of column elements by " << sigma << endl;
	}


      // the following paragraph acts only on the diagonal element:
      // if element=0, then calculate alpha & beta

      // take the diagonal element
      eventMatrixkk = eventMatrixProc[k][k];

      if (eventMatrixkk < 0.) 
	alpha[k] = sqrt(sigma);
      else
	alpha[k] = sqrt(sigma) * (-1.);

      alphak = alpha[k];

      beta = 1 / (sigma - eventMatrixkk * alphak);
      // replace it
      eventMatrixProc[k][k] = eventMatrixkk - alphak;

      for (j=k+1; j<Nchannels; j++) 
	{
	  y[j] = 0.;

	  for (i=k; i<Nevents; i++)
	    {
	      y[j] += eventMatrixProc[i][k] * eventMatrixProc[i][j];
	    }

	  y[j] *= beta;
	}

      for (j=k+1; j<Nchannels; j++) 
	{
	  for (i=k; i<Nevents; i++) 
	    {
	      eventMatrixProc[i][j] -= eventMatrixProc[i][k] * y[j];
	      sum[j] -= eventMatrixProc[k][j] * eventMatrixProc[k][j];
	    }
	}
    } // end of kth householder transformation
  
  //  cout << "Householder::decompose() finished" << endl;
  
  return true;
}


void HouseholderDecomposition::solve(vector<float> &y)
{
  vector<float> z(Nchannels,0.);

  float gamma;
  int i,j;

  //  cout << "Householder::solve() begin" << endl;

  for (j=0; j<Nchannels; j++) 
    {
      // apply jth transformation to the right hand side
      gamma = 0.;
      for (i=j; i<Nevents; i++)
	{
	  gamma += eventMatrixProc[i][j] * energyVectorProc[i];
	}
      gamma /= (alpha[j] * eventMatrixProc[j][j]);

      for (i=j; i<Nevents; i++)
	{
	  energyVectorProc[i] += gamma * eventMatrixProc[i][j];
	}
    }

  z[Nchannels-1] = energyVectorProc[Nchannels-1] / alpha[Nchannels-1];

  for (i=Nchannels-2; i>=0; i--) 
    {
      z[i] = energyVectorProc[i];
      for (j=i+1; j<Nchannels; j++)
	{
	  z[i] -= eventMatrixProc[i][j]*z[j];
	}
      z[i] /= alpha[i];
    }
  
  for (i=0; i<Nchannels; i++)
    {
      y[pivot[i]] = z[i];
    }
  
  //  cout << "Householder::solve() finished." << endl;
  
}


vector<float> HouseholderDecomposition::recalibrateEvent(const vector<float>& eventSquare, const int& maxCeta, const int& maxCphi, const vector<float>& recalibrateVector)
{
  vector<float> newEventSquare(eventSquare);
  int iFull;

  for (int i=0; i<Nxtals; i++) 
    {
      iFull = indexSqr2Reg(i, maxCeta, maxCphi);
      if (iFull >=0)
	newEventSquare[i] *= recalibrateVector[iFull];
    }
  return newEventSquare;
}


int HouseholderDecomposition::indexSqr2Reg(const int& sqrIndex, const int& maxCeta, const int& maxCphi)
{
  int regionIndex;

  // get the current eta, phi indices
  int curr_eta = maxCeta - squareMode/2 + sqrIndex%squareMode;
  if (curr_eta * maxCeta <= 0) {if (maxCeta > 0) curr_eta--; else curr_eta++; }  // JUMP over 0

  int curr_phi = maxCphi - squareMode/2 + sqrIndex/squareMode;
  if (curr_phi < 1) curr_phi += 360;
  if (curr_phi > 360) curr_phi -= 360;

  bool negPhiDirection = (maxphi < minphi);
  int iFullphi;

  regionIndex = -1;

  if (curr_eta >= mineta && curr_eta <= maxeta)
    if ( (!negPhiDirection && (curr_phi >= minphi && curr_phi <= maxphi)) ||
	 (negPhiDirection && !(curr_phi >= minphi && curr_phi <= maxphi))      ) 
      {
	iFullphi = curr_phi - minphi;
	if (iFullphi < 0) iFullphi += 360;
	regionIndex = (curr_eta - mineta) * (maxphi - minphi + 1 + 360*negPhiDirection) + iFullphi;
      }

  return regionIndex;
}

vector<vector<float> > HouseholderDecomposition::unzipMatrix(const vector< vector<float> >& eventMatrix, const vector<int>& VmaxCeta, const vector<int>& VmaxCphi)
{
  vector< vector<float> > fullMatrix;

  int iFull;

  for (int i=0; i<Nevents; i++)
    {
      vector<float> foo(Nchannels,0.);
      for (int k=0; k<Nxtals; k++)
	{
	  iFull = indexSqr2Reg(k, VmaxCeta[i], VmaxCphi[i]);
	  if (iFull >=0)
	    foo[iFull] = eventMatrix[i][k];
	}
      fullMatrix.push_back(foo);
    }

  return fullMatrix;
}

void HouseholderDecomposition::makeRegions(const int& regLength)
{
  //  int regFrame = regLength/2;
  int regFrame = squareMode/2;
  // first eta:
  int remEta = Neta % regLength;
  if (remEta > regLength/2) remEta -= regLength;
  int numSubRegEta = Neta / regLength + (remEta < 0)*1;

  int addtoEta = remEta / numSubRegEta;
  int addtomoreEta = remEta % numSubRegEta; // add "addtomore" number of times (addto+1), remaining times add just (addto)

  // then phi:
  int remPhi = Nphi % regLength;
  if (remPhi > regLength/2) remPhi -= regLength;
  int numSubRegPhi = Nphi / regLength + (remPhi < 0)*1;

  int addtoPhi = remPhi / numSubRegPhi;
  int addtomorePhi = remPhi % numSubRegPhi; // add "addtomore" number of times (addto+-1), remaining times add just (addto)

  // now put it all together
  int addedLengthEta = 0;
  int addedLengthPhi = 0;
  int startIndexEta = mineta;
  int startIndexPhi;
  int endIndexEta = 0;
  int endIndexPhi;
  for (int i=0; i < numSubRegEta; i++)
    {
      addedLengthEta = regLength + addtoEta + addtomoreEta/abs(addtomoreEta)*(i<abs(addtomoreEta));
      endIndexEta = startIndexEta + addedLengthEta - 1;

      startIndexPhi = minphi;
      endIndexPhi = 0;
      for (int j=0; j < numSubRegPhi; j++)
	{
	  addedLengthPhi = regLength + addtoPhi + addtomorePhi/abs(addtomorePhi)*(j<abs(addtomorePhi));
	  endIndexPhi = startIndexPhi + addedLengthPhi - 1;

	  regMinPhi.push_back(startIndexPhi - regFrame*(j!=0) );
	  regMaxPhi.push_back(endIndexPhi + regFrame*(j!=(numSubRegPhi-1)) );
	  regMinEta.push_back(startIndexEta - regFrame*(i!=0) );
	  regMaxEta.push_back(endIndexEta + regFrame*(i!=(numSubRegEta-1)) );

	  startIndexPhi = endIndexPhi + 1;
	}
      startIndexEta = endIndexEta + 1;
    }

//  // print it all
//  cout << "Householder::makeRegions created the following regions for calibration:" << endl;
//  for (int i=0; i<regMinEta.size(); i++)
//    cout << "Region " << i << ": eta = " << regMinEta[i] << " to " << regMaxEta[i] << ", phi = " << regMinPhi[i] << " to " << regMaxPhi[i] << endl;

}
