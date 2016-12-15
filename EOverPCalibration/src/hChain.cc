#include "../interface/hChain.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TCanvas.h"

hChain::hChain (TString baseName, TString baseTitle,
                int nbins, double min, double max, int NUM) :
	m_baseName (baseName)
{
	for (int i = 0 ; i < NUM ; ++i) {
		TString name = TString ("h_") ;
		name += i ;
		name += TString ("_") + m_baseName ;
		TString title = baseTitle + TString (" ") ;
		title += i ;
		TH1F * dummy = new TH1F (name, title, nbins, min, max) ;
		m_histos.push_back (dummy) ;

//        name = TString ("n_") ;
//        name += i ;
//        name += TString ("_") + m_baseName ;
//        TNtuple * ndummy = new TNtuple (name, title, "var") ;
//        m_ntuples.push_back (ndummy) ;
	}
}


//PG --------------------------------------------------------


hChain::~hChain ()
{
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		delete m_histos.at (i) ;
//  for (int i=0 ; i<m_ntuples.size () ; ++i)
//    delete m_ntuples.at (i) ;
}


//PG --------------------------------------------------------


void
hChain::SetColors (std::vector<int> colors)
{
	//PG this is weak, assumes correct number of elements
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i) {
		m_histos.at (i)->SetLineColor (colors.at (i)) ;
		m_histos.at (i)->SetLineWidth (2) ;
	}
}


//PG --------------------------------------------------------


void
hChain::Fill (int i, double val)
{
	m_histos.at (i)->Fill (val) ;
//    m_ntuples.at (i)->Fill (val) ;
	return ;
}


//RG --------------------------------------------------------



void
hChain::SetBinContent (int i, int bin, double val)
{
	m_histos.at (i)->SetBinContent (bin, val) ;
//    m_ntuples.at (i)->Fill (val) ;
	return ;
}

//RG --------------------------------------------------------



void
hChain::SetBinError (int i, int bin, double val)
{
	m_histos.at (i)->SetBinError (bin, val) ;
//    m_ntuples.at (i)->Fill (val) ;
	return ;
}



//PG --------------------------------------------------------


void
hChain::Print (bool isLog, int rebin, TString altName)
{
	gROOT->SetStyle ("Plain") ;

	for (unsigned int i = 0 ; i < m_histos.size () ; ++i) {
		m_histos.at (i)->SetFillStyle (3001) ;
		m_histos.at (i)->SetStats (0) ;
		m_histos.at (i)->Rebin (rebin) ;
	}

	TString name = "s_" ;
	if (altName == "default") name += m_baseName ;
	else name += altName ;
	THStack hs (name, "stacked m_histos") ;
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		hs.Add (m_histos.at (i)) ;

	TCanvas c1 ;
	c1.cd () ;
	if (isLog) c1.SetLogy () ;
	hs.Draw ("nostack") ;

	if (altName == "default") name = m_baseName + TString ("_h1.gif") ;
	else name = altName + TString ("_h1.gif") ;
	c1.Print (name, "gif") ;
	double max = 1.1 * findNMax () ;
	double min = 0.9 * findNMin () ;
	TH2F bkg ("bkg", "",
	          10, m_histos.at (0)->GetXaxis ()->GetXmin (),
	          m_histos.at (0)->GetXaxis ()->GetXmax (),
	          10, min, max) ;
	bkg.SetStats (0) ;
	bkg.AddDirectory (0) ;
	bkg.Draw () ;
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		m_histos.at (i)->DrawNormalized ("same") ;
	name = TString ("N") + m_baseName + TString ("_h1.gif") ;
	c1.Print (name, "gif") ;
}


//PG --------------------------------------------------------

/*
void
hChain::PrintEach (bool isLog, int rebin)
  {
    for (unsigned int i=0 ; i<m_histos.size () ; ++i)
    {
      m_histos.at (i)->SetFillStyle (3001) ;
      m_histos.at (i)->SetStats (0) ;
      m_histos.at (i)->Rebin (rebin) ;
    }

    TCanvas c1 ;
    c1.cd () ;
    if(isLog) c1.SetLogy();

    for (unsigned int i=0 ; i<m_histos.size () ; ++i)
      {
        m_histos.at (i)->SetStats (0) ;
        m_histos.at (i)->Draw () ;
        TString name = m_baseName ;
                name += TString ("_") ;
                name += i ;
                name += TString ("_h1.gif") ;
        c1.Print (name,"gif") ;
      }

  }
*/

//PG --------------------------------------------------------


void
hChain::Write (TFile & outputFile)
{
	outputFile.cd () ;
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		//if(m_histos.at (i) -> GetEntries() > 0)
		m_histos.at (i)->Write () ;
//    for (unsigned int i=0 ; i<m_ntuples.size () ; ++i)
//      m_ntuples.at (i)->Write () ;
	return ;
}


//PG --------------------------------------------------------




void
hChain::Write (const std::string& dirName, TFile & outputFile)
{
	outputFile.cd () ;
	outputFile.mkdir(dirName.c_str());
	outputFile.cd(dirName.c_str());
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		//if(m_histos.at (i) -> GetEntries() > 0)
		m_histos.at (i)->Write () ;
//    for (unsigned int i=0 ; i<m_ntuples.size () ; ++i)
//      m_ntuples.at (i)->Write () ;
	return ;
}


//PG --------------------------------------------------------



double
hChain::findNMin ()
{
	double minVal = m_histos.at (0)->GetMinimum (0.0000001) /
	                m_histos.at (0)->Integral () ;
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i) {
		double dummy = m_histos.at (i)->GetMinimum (0.0000001) /
		               m_histos.at (i)->Integral () ;
		if (minVal > dummy) minVal = dummy ;
	}
	return minVal ;
}


//PG --------------------------------------------------------


double
hChain::findNMax ()
{
	double maxVal = m_histos.at (0)->GetMaximum () /
	                m_histos.at (0)->Integral () ;
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i) {
		double dummy = m_histos.at (i)->GetMaximum () /
		               m_histos.at (i)->Integral () ;
		if (maxVal < dummy) maxVal = dummy ;
	}
	return maxVal ;
}


//PG --------------------------------------------------------


void
hChain::Normalize (int index)
{
	if(m_histos.at (index) -> GetEntries() > 0)
		m_histos.at (index)->Scale (1. / m_histos.at(index)->GetEntries()) ;
}



void
hChain::Scale (int index, double factor)
{
	m_histos.at (index)->Scale (factor) ;
}

//RG -----------------------------------------------------
double
hChain::GetEffectiveEntries(int index)
{
	return m_histos.at(index)->GetEffectiveEntries();
}

//RG -----------------------------------------------------
void
hChain::Reset()
{
	for(unsigned int index = 0; index < m_histos.size(); index++)
		m_histos.at(index)->Delete();
}

//RG -------------------------------------------------------
TH1F*
hChain::GetHisto(int index)
{
	return m_histos.at(index);
}
