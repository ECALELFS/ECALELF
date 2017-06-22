#include "Calibration/EOverPCalibration/interface/h2Chain.h"

h2Chain::h2Chain (TString baseName, TString baseTitle,
                  int nbinsx, double minx, double maxx,
                  int nbinsy, double miny, double maxy, int NUM) :
	m_baseName (baseName)
{
	TString name ;
	TString title ;
	for (int i = 0 ; i < NUM ; ++i) {
		name = TString ("h2_") ;
		name += i ;
		name += TString ("_") + m_baseName ;
		title = baseTitle + TString (" ") ;
		title += i ;
		TH2F * dummy = new TH2F (name, title, nbinsx, minx, maxx, nbinsy, miny, maxy) ;
		dummy->SetStats (0) ;
		m_histos.push_back (dummy) ;
	}
}


//PG --------------------------------------------------------


h2Chain::~h2Chain ()
{
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		delete m_histos.at (i) ;
}


//PG --------------------------------------------------------


void
h2Chain::SetColors (std::vector<int> colors)
{
	//PG this is weak, assumes correct number of elements
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i) {
		m_histos.at (i)->SetMarkerColor (colors.at (i)) ;
		m_histos.at (i)->SetLineColor (colors.at (i)) ;
		m_histos.at (i)->SetFillColor (colors.at (i)) ;
		m_histos.at (i)->SetFillStyle (3001) ;
	}
	//PG this is a hack
//    m_histos.back ()->SetLineColor (kBlack) ;
//    m_histos.back ()->SetLineWidth (2) ;
}


//PG --------------------------------------------------------


void
h2Chain::Fill (int i, double valx, double valy)
{
	m_histos.at (i)->Fill (valx, valy) ;
	return ;
}


//PG --------------------------------------------------------


void
h2Chain::Fill (int i, double valx, double valy, double weight)
{
	m_histos.at (i)->Fill (valx, valy, weight) ;
	return ;
}



//PG --------------------------------------------------------

/*
void
h2Chain::Print (bool isLog, int rebinx, int rebiny)
  {
    TCanvas *c1 = new TCanvas("c1", "c1", 52,180,500,500);
    c1->cd () ;
    if(isLog) c1->SetLogz();

    m_histos.at (0)->Draw ("box") ;
    for (unsigned int i=0 ; i<m_histos.size () - 1 ; ++i)
      {
        m_histos.at (i)->SetStats (0) ;
        m_histos.at (i)->Draw ("boxsame") ;
        m_histos.at (i)->RebinX (rebinx) ;
        m_histos.at (i)->RebinY (rebiny) ;
      }
    //PG this is a hack
//    m_histos.back ()->Draw ("same") ;
    TString name = m_baseName + TString ("_h2.gif") ;
    c1->Print (name,"gif") ;

    m_histos.at (0)->DrawNormalized ("box") ;
    for (unsigned int i=0 ; i<m_histos.size () - 1 ; ++i)
      m_histos.at (i)->DrawNormalized ("boxsame") ;
    //PG this is a hack
//    m_histos.back ()->DrawNormalized ("same") ;
    name = TString ("N") + m_baseName + TString ("_h2.gif") ;
    c1->Print (name,"gif") ;
  }
*/

//PG --------------------------------------------------------

/*
void
h2Chain::PrintEach ()
  {
    TCanvas *c1 = new TCanvas("c1","c1", 180, 52, 550, 550);
    c1->cd () ;
    for (unsigned int i=0 ; i<m_histos.size () ; ++i)
      {
        m_histos.at (i)->SetStats (0) ;
        m_histos.at (i)->Draw () ;
        TString name = m_baseName ;
                name += TString ("_") ;
                name += i ;
                name += TString ("_h2.gif") ;
        c1->Print (name,"gif") ;
      }
  }
*/

//PG --------------------------------------------------------


void
h2Chain::Write (const std::string& dirName, TFile & outputFile)
{
	outputFile.cd () ;
	outputFile.mkdir(dirName.c_str());
	outputFile.cd(dirName.c_str());
	for (unsigned int i = 0 ; i < m_histos.size () ; ++i)
		m_histos.at (i)->Write () ;
	return ;
}


//PG --------------------------------------------------------


void
h2Chain::Scale (int index, double factor)
{
	m_histos.at (index)->Scale (factor) ;
}
