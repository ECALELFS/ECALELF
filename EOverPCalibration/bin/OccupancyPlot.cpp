#include <iostream>
#include <iomanip>
#include <cmath>
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TROOT.h"
#include "TLatex.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>
#include <map>
#include "../interface/treeReader.h"


void SetWHLTPathNames(std::vector<std::pair<std::string, std::pair<int, int> > > & WHLTPathNames) ;


int main (int argc, char ** argv)
{

	std::string category = std::string(argv[1]);

	double xtalWidth = 0.01745329;
	double luminosity = 11.35;
	double etaringEE = 0.0325;

// Acquisition of input file
	TChain* treeDATA = new TChain("simpleNtupleEoverP/SimpleNtupleEoverP");

	treeDATA->Add("/gwteray/users/gerosa/ECALNTUPLES/22JanReReco/SingleElectron_RUN2012D_22Jan2013-v1.root");


	treeReader *fReader = new treeReader((TTree*)(treeDATA), false);
	treeDATA->SetBranchStatus("*", 0);
	treeDATA->SetBranchStatus("isW", 1);
	treeDATA->SetBranchStatus("isZ", 1);
	treeDATA->SetBranchStatus("ele1_seedZside", 1);
	treeDATA->SetBranchStatus("ele1_seedIx", 1);
	treeDATA->SetBranchStatus("ele1_seedIy", 1);
	treeDATA->SetBranchStatus("ele1_seedIphi", 1);
	treeDATA->SetBranchStatus("ele1_seedIeta", 1);
	treeDATA->SetBranchStatus("ele1_e3x3", 1);
	treeDATA->SetBranchStatus("ele1_scEta", 1);
	treeDATA->SetBranchStatus("ele1_scERaw", 1);

	treeDATA->SetBranchStatus("ele2_seedZside", 1);
	treeDATA->SetBranchStatus("ele2_seedIx", 1);
	treeDATA->SetBranchStatus("ele2_seedIy", 1);
	treeDATA->SetBranchStatus("ele2_seedIphi", 1);
	treeDATA->SetBranchStatus("ele2_seedIeta", 1);
	treeDATA->SetBranchStatus("ele2_e3x3", 1);
	treeDATA->SetBranchStatus("ele2_scEta", 1);
	treeDATA->SetBranchStatus("ele2_scERaw", 1);

// Set Single Electron Trigger Names

	std::vector<std::pair<std::string, std::pair<int, int> > > WHLTPathNames;
	SetWHLTPathNames(WHLTPathNames);

	TH2F* h_OccupancyEB = new TH2F("h_OccupancyEB", "h_OccupancyEB", 360, 1, 361, 171, -85, 86);
	TH2F* h_OccupancyEB2 = new TH2F("h_OccupancyEB2", "h_OccupancyEB", 360, 1, 361, 171, -85, 86);

	TH2F* h_OccupancyEE[2];
	h_OccupancyEE[0] = new TH2F("h_OccupancyEEM", "h_OccupancyEEP", 100, 1, 101, 100, 1, 101);
	h_OccupancyEE[1] = new TH2F("h_OccupancyEEP", "h_OccupancyEEM", 100, 1, 101, 100, 1, 101);

	TH2F* h_OccupancyEE2[2];
	h_OccupancyEE2[1] = new TH2F("h_OccupancyEEP2", "h_OccupancyEEP2", 100, 1, 101, 100, 1, 101);
	h_OccupancyEE2[0] = new TH2F("h_OccupancyEEM2", "h_OccupancyEEM2", 100, 1, 101, 100, 1, 101);


	std::cout << " DATA Entries = " << treeDATA->GetEntries() << std::endl;

	for(int entry = 0; entry < treeDATA->GetEntries(); ++entry) {

		if( entry % 100000 == 0 ) std::cout << "reading saved entry " << entry << "\r" << std::flush;
		treeDATA->GetEntry(entry);

		//if(fReader->getInt("isHLTEle27")[0] != 1 || fReader->getInt("isGood")[0] !=1 ) continue;

		bool skipEvent = true;
		bool isWHLT = false;

		// W triggers
		for(unsigned int HLTIt = 0; HLTIt < WHLTPathNames.size(); ++HLTIt) {

			if( fReader->getInt("runId")[0] < ((WHLTPathNames.at(HLTIt)).second).first )  continue;
			if( fReader->getInt("runId")[0] > ((WHLTPathNames.at(HLTIt)).second).second ) continue;

			for(unsigned int iHLTIt = 0; iHLTIt < fReader->GetString("HLT_Names")->size(); ++iHLTIt) {
				if( (fReader->GetString("HLT_Names")->at(iHLTIt) == WHLTPathNames.at(HLTIt).first) && (fReader->GetInt("HLT_Accept")->at(iHLTIt) == 1) ) isWHLT = true;
			}

			if( isWHLT == true ) skipEvent = false;
		}


		if(skipEvent == true) continue ;


		if(fReader->getInt("isW")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == 0) h_OccupancyEB->Fill(fReader->getInt("ele1_seedIphi")[0], fReader->getInt("ele1_seedIeta")[0]);
		if(fReader->getInt("isW")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == 1) h_OccupancyEE[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);
		if(fReader->getInt("isW")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == -1) h_OccupancyEE[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);


//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==0 && fReader->getInt("isZ")[0] ==0)
//     h_OccupancyEB->Fill(fReader->getInt("ele1_seedIphi")[0],fReader->getInt("ele1_seedIeta")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==1 && fReader->getInt("isZ")[0] ==0)
//     h_OccupancyEE[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==-1 && fReader->getInt("isZ")[0] ==0)
//     h_OccupancyEE[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//   if(category == "Default"){
//      if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==0 && fReader->getInt("isZ")[0] ==0)
//        h_OccupancyEB2->Fill(fReader->getInt("ele1_seedIphi")[0],fReader->getInt("ele1_seedIeta")[0]);
//   }

//  if(category == "R9"){
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==0 && fReader->getInt("isZ")[0] ==0 && fReader->getFloat("ele1_r9")[0]>0.94)
//    h_OccupancyEB2->Fill(fReader->getInt("ele1_seedIphi")[0],fReader->getInt("ele1_seedIeta")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==1 && fReader->getInt("isZ")[0] ==0 && fReader->getFloat("ele1_r9")[0]>0.94)
//    h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==-1 && fReader->getInt("isZ")[0] ==0 && fReader->getFloat("ele1_r9")[0]>0.94)
//    h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//  }

//  if(category == "fbrem") {
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==0 && fReader->getInt("isZ")[0] ==0 && fabs(fReader->getFloat("ele1_fbrem")[0])<0.5)
//     h_OccupancyEB2->Fill(fReader->getInt("ele1_seedIphi")[0],fReader->getInt("ele1_seedIeta")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==1 && fReader->getInt("isZ")[0] ==0 && fabs(fReader->getFloat("ele1_fbrem")[0])<0.5)
//     h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==-1 && fReader->getInt("isZ")[0] ==0 && fabs(fReader->getFloat("ele1_fbrem")[0])<0.5)
//     h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//  }

//  if(category == "Pt"){
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==0 && fReader->getInt("isZ")[0] ==0 && fReader->getFloat("ele1_pt")[0]>50)
//    h_OccupancyEB2->Fill(fReader->getInt("ele1_seedIphi")[0],fReader->getInt("ele1_seedIeta")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==1 && fReader->getInt("isZ")[0] ==0 && fReader->getFloat("ele1_pt")[0]>50)
//    h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==-1 && fReader->getInt("isZ")[0] ==0 && fReader->getFloat("ele1_pt")[0]>50)
//    h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//  }

		if(fReader->getInt("isW")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == 1 ) {
			if(fabs(fReader->getFloat("ele1_scEta")[0]) < 1.75 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.8)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 1.75 && fabs(fReader->getFloat("ele1_scEta")[0]) < 2. && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.88)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2. && fabs(fReader->getFloat("ele1_scEta")[0]) < 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.92)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.94)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);
		}


//  if(category=="Default"){
//  if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==1 && fReader->getInt("isZ")[0] ==0){
//    if(fabs(fReader->getFloat("ele1_scEta")[0])<1.75 && fReader->getFloat("ele1_r9")[0]>0.8)
//          h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//    if(fabs(fReader->getFloat("ele1_scEta")[0])>=1.75 && fabs(fReader->getFloat("ele1_scEta")[0])<2. && fReader->getFloat("ele1_r9")[0]>0.88)
//          h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//    if(fabs(fReader->getFloat("ele1_scEta")[0])>=2. && fabs(fReader->getFloat("ele1_scEta")[0])<2.15 && fReader->getFloat("ele1_r9")[0]>0.92)
//          h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//    if(fabs(fReader->getFloat("ele1_scEta")[0])>=2.15 && fReader->getFloat("ele1_r9")[0]>0.94)
//          h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//  }
// }


		if(fReader->getInt("isW")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == -1) {

			if(fabs(fReader->getFloat("ele1_scEta")[0]) < 1.75 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.8)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 1.75 && fabs(fReader->getFloat("ele1_scEta")[0]) < 2. && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.88)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2. && fabs(fReader->getFloat("ele1_scEta")[0]) < 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.92)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.94)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);
		}


//  if(category == "Default"){

//   if(fReader->getInt("isGood")[0] ==1 && fReader->getInt("ele1_seedZside")[0]==-1 && fReader->getInt("isZ")[0] ==0){

//    if(fabs(fReader->getFloat("ele1_scEta")[0])<1.75 && fReader->getFloat("ele1_r9")[0]>0.8)
//      h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//    if(fabs(fReader->getFloat("ele1_scEta")[0])>=1.75 && fabs(fReader->getFloat("ele1_scEta")[0])<2. && fReader->getFloat("ele1_r9")[0]>0.88)
//      h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//    if(fabs(fReader->getFloat("ele1_scEta")[0])>=2. && fabs(fReader->getFloat("ele1_scEta")[0])<2.15 && fReader->getFloat("ele1_r9")[0]>0.92)
//      h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);

//    if(fabs(fReader->getFloat("ele1_scEta")[0])>=2.15 && fReader->getFloat("ele1_r9")[0]>0.94)
//      h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0],fReader->getInt("ele1_seedIy")[0]);
//   }
//  }


		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == 0) h_OccupancyEB->Fill(fReader->getInt("ele1_seedIphi")[0], fReader->getInt("ele1_seedIeta")[0]);
		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele2_seedZside")[0] == 0) h_OccupancyEB->Fill(fReader->getInt("ele2_seedIphi")[0], fReader->getInt("ele2_seedIeta")[0]);

		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == 1) h_OccupancyEE[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);
		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele2_seedZside")[0] == 1) h_OccupancyEE[1]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == -1) h_OccupancyEE[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);
		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele2_seedZside")[0] == -1) h_OccupancyEE[0]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == 1) {

			if(fabs(fReader->getFloat("ele1_scEta")[0]) < 1.75 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.8)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIphi")[0], fReader->getInt("ele1_seedIeta")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 1.75 && fabs(fReader->getFloat("ele1_scEta")[0]) < 2. && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.88)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2. && fabs(fReader->getFloat("ele1_scEta")[0]) < 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.92)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.94)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

		}

		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele2_seedZside")[0] == 1) {

			if(fabs(fReader->getFloat("ele2_scEta")[0]) < 1.75 && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.8)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele2_seedIphi")[0], fReader->getInt("ele2_seedIeta")[0]);

			if(fabs(fReader->getFloat("ele2_scEta")[0]) >= 1.75 && fabs(fReader->getFloat("ele2_scEta")[0]) < 2. && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.88)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

			if(fabs(fReader->getFloat("ele2_scEta")[0]) >= 2. && fabs(fReader->getFloat("ele2_scEta")[0]) < 2.15 && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.92)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

			if(fabs(fReader->getFloat("ele2_scEta")[0]) >= 2.15 && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.94)
				h_OccupancyEE2[1]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

		}

		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele1_seedZside")[0] == -1) {

			if(fabs(fReader->getFloat("ele1_scEta")[0]) < 1.75 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.8)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIphi")[0], fReader->getInt("ele1_seedIeta")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 1.75 && fabs(fReader->getFloat("ele1_scEta")[0]) < 2. && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.88)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2. && fabs(fReader->getFloat("ele1_scEta")[0]) < 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.92)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

			if(fabs(fReader->getFloat("ele1_scEta")[0]) >= 2.15 && fReader->getFloat("ele1_e3x3")[0] / fReader->getFloat("ele1_scERaw")[0] > 0.94)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele1_seedIx")[0], fReader->getInt("ele1_seedIy")[0]);

		}

		if(fReader->getInt("isZ")[0] == 1 && fReader->getInt("ele2_seedZside")[0] == -1) {

			if(fabs(fReader->getFloat("ele2_scEta")[0]) < 1.75 && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.8)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele2_seedIphi")[0], fReader->getInt("ele2_seedIeta")[0]);

			if(fabs(fReader->getFloat("ele2_scEta")[0]) >= 1.75 && fabs(fReader->getFloat("ele2_scEta")[0]) < 2. && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.88)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

			if(fabs(fReader->getFloat("ele2_scEta")[0]) >= 2. && fabs(fReader->getFloat("ele2_scEta")[0]) < 2.15 && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.92)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

			if(fabs(fReader->getFloat("ele2_scEta")[0]) >= 2.15 && fReader->getFloat("ele2_e3x3")[0] / fReader->getFloat("ele2_scERaw")[0] > 0.94)
				h_OccupancyEE2[0]->Fill(fReader->getInt("ele2_seedIx")[0], fReader->getInt("ele2_seedIy")[0]);

		}


	}

// Graph ocuppancy for single xtal

	TGraphErrors *OccupancyEB_vs_Eta = new TGraphErrors();
	TGraphErrors *OccupancyEEP_vs_Eta = new TGraphErrors();
	TGraphErrors *OccupancyEEM_vs_Eta = new TGraphErrors();

	TGraphErrors *OccupancyAll = new TGraphErrors();
	TGraphErrors *OccupancyAll2 = new TGraphErrors();


// Graph for EB
	int nPoint = 0, nPointAll = 0, nPointAll2 = 0;


	for(int iEta = 0; iEta < h_OccupancyEB->GetNbinsY() ; iEta++) {
		double sumHit = 0, nHit = 0;
		for(int iPhi = 0; iPhi < h_OccupancyEB->GetNbinsX() ; iPhi++) {
			if(h_OccupancyEB->GetBinContent(iPhi + 1, iEta + 1) == 0) continue;
			sumHit = sumHit + h_OccupancyEB->GetBinContent(iPhi + 1, iEta + 1);
			nHit++;
		}
		if(iEta < 85 && sumHit != 0 && nHit != 0 ) {
			OccupancyEB_vs_Eta->SetPoint(nPoint, -(85 - iEta)*xtalWidth, sumHit / (nHit * luminosity));
			OccupancyEB_vs_Eta->SetPointError(nPoint, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));

			OccupancyAll->SetPoint(nPointAll, -(85 - iEta)*xtalWidth, sumHit / (nHit * luminosity));
			OccupancyAll->SetPointError(nPointAll, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));
		}

		if(iEta < 85 && sumHit == 0 && nHit == 0 ) {
			OccupancyEB_vs_Eta->SetPoint(nPoint, -(85 - iEta)*xtalWidth, 0.);
			OccupancyEB_vs_Eta->SetPointError(nPoint, 0., 0.);
			OccupancyAll->SetPoint(nPointAll, -(85 - iEta)*xtalWidth, 0.);
			OccupancyAll->SetPointError(nPointAll, 0., 0.);
		}

		if(iEta == 85 && sumHit != 0 && nHit != 0) {
			OccupancyEB_vs_Eta->SetPoint(nPoint, 0., sumHit / (nHit * luminosity));
			OccupancyEB_vs_Eta->SetPointError(nPoint, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));
			OccupancyAll->SetPoint(nPointAll, 0., sumHit / (nHit * luminosity));
			OccupancyAll->SetPoint(nPointAll, 0., sumHit / (nHit * luminosity));
		}

		if(iEta == 85 && sumHit == 0 && nHit == 0 ) {
			OccupancyEB_vs_Eta->SetPoint(nPoint, 0., 0.);
			OccupancyEB_vs_Eta->SetPointError(nPoint, 0., 0.);
			OccupancyAll->SetPoint(nPointAll, 0., 0.);
			OccupancyAll->SetPointError(nPointAll, 0., 0.);
		}

		if(iEta > 85 && sumHit != 0 && nHit != 0) {
			OccupancyEB_vs_Eta->SetPoint(nPoint, (iEta - 85)*xtalWidth, sumHit / (nHit * luminosity));
			OccupancyEB_vs_Eta->SetPointError(nPoint, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));
			OccupancyAll->SetPoint(nPointAll, (iEta - 85)*xtalWidth, sumHit / (nHit * luminosity));
			OccupancyAll->SetPointError(nPointAll, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));
		}

		if(iEta < 85 && sumHit == 0 && nHit == 0 ) {
			OccupancyEB_vs_Eta->SetPoint(nPoint, (iEta - 85)*xtalWidth, 0.);
			OccupancyEB_vs_Eta->SetPointError(nPoint, 0., 0.);
			OccupancyAll->SetPoint(nPointAll, (iEta - 85)*xtalWidth, 0.);
			OccupancyAll->SetPointError(nPointAll, 0., 0.);
		}

		nPoint++;
		nPointAll++;
		nPointAll2++;
	}

// Graph for EB
	nPoint = 0;
	nPointAll = 0;
	nPointAll2 = 0;

	for(int iEta = 0; iEta < h_OccupancyEB2->GetNbinsY() ; iEta++) {
		double sumHit = 0, nHit = 0;
		for(int iPhi = 0; iPhi < h_OccupancyEB2->GetNbinsX() ; iPhi++) {
			if(h_OccupancyEB2->GetBinContent(iPhi + 1, iEta + 1) == 0) continue;
			sumHit = sumHit + h_OccupancyEB2->GetBinContent(iPhi + 1, iEta + 1);
			nHit++;
		}
		if(iEta < 85 && sumHit != 0 && nHit != 0 ) {
			OccupancyAll2->SetPoint(nPointAll2, -(85 - iEta)*xtalWidth, sumHit / (nHit * luminosity));
			OccupancyAll2->SetPointError(nPointAll2, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));
		}

		if(iEta < 85 && sumHit == 0 && nHit == 0 ) {
			OccupancyAll2->SetPoint(nPointAll2, -(85 - iEta)*xtalWidth, 0.);
			OccupancyAll2->SetPointError(nPointAll2, 0., 0.);
		}

		if(iEta == 85 && sumHit != 0 && nHit != 0) {
			OccupancyAll2->SetPointError(nPointAll2, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));
			OccupancyAll2->SetPointError(nPointAll2, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));

		}

		if(iEta == 85 && sumHit == 0 && nHit == 0 ) {
			OccupancyAll2->SetPoint(nPointAll2, 0., 0.);
			OccupancyAll2->SetPointError(nPointAll2, 0., 0.);
		}

		if(iEta > 85 && sumHit != 0 && nHit != 0) {
			OccupancyAll2->SetPoint(nPointAll2, (iEta - 85)*xtalWidth, sumHit / (nHit * luminosity));
			OccupancyAll2->SetPointError(nPointAll2, 0., (sumHit / (nHit * luminosity)) / sqrt(nHit * luminosity));

		}

		if(iEta < 85 && sumHit == 0 && nHit == 0 ) {
			OccupancyAll2->SetPoint(nPointAll2, (iEta - 85)*xtalWidth, 0.);
			OccupancyAll2->SetPointError(nPointAll2, 0., 0.);
		}

		nPoint++;
		nPointAll++;
		nPointAll2++;
	}



// Plot for EE : EEP and EEM
/// ring geometry for the endcap
	TH2F *hrings[2];
	hrings[0] = (TH2F*) h_OccupancyEE[0]->Clone("hringsEEM");
	hrings[1] = (TH2F*) h_OccupancyEE[1]->Clone("hringsEEP");
	hrings[0] ->Reset("ICMES");
	hrings[1] ->Reset("ICMES");
	hrings[0] ->ResetStats();
	hrings[1] ->ResetStats();

	FILE *fRing;
	fRing = fopen("macros/eerings.dat", "r");
	int x, y, z, ir;
	while(fscanf(fRing, "(%d,%d,%d) %d \n", &x, &y, &z, &ir) != EOF ) {
		if(z > 0) hrings[1]->Fill(x, y, ir);
		if(z < 0) hrings[0]->Fill(x, y, ir);
	}

	std::vector<double> sumHitEEM ;
	std::vector<int> nHitEEM ;
	sumHitEEM.assign(40, 0.);
	nHitEEM.assign(40, 0);
	std::vector<double> sumHitEEP ;
	std::vector<int> nHitEEP ;
	sumHitEEP.assign(40, 0.);
	nHitEEP.assign(40, 0);

	for(int k = 0; k < 2 ; k++) {
		for(int ix = 0; ix < h_OccupancyEE[k]->GetNbinsX() ; ix++) {
			for(int iy = 0; iy < h_OccupancyEE[k]->GetNbinsY() ; iy++) {
				if(h_OccupancyEE[k]->GetBinContent(ix + 1, iy + 1) == 0) continue;
				int mybin = h_OccupancyEE[k] -> FindBin(ix, iy);
				int ring  = int(hrings[k]-> GetBinContent(mybin));
				float occ = h_OccupancyEE[k]->GetBinContent(mybin);
				if(k == 0) {
					sumHitEEM.at(ring) = sumHitEEM.at(ring) + occ;
					nHitEEM.at(ring) = nHitEEM.at(ring) + 1;
				}
				if(k == 1) {
					sumHitEEP.at(ring) = sumHitEEP.at(ring) + occ;
					nHitEEP.at(ring) = nHitEEP.at(ring) + 1;
				}
			}
		}
	}

	for(int k = 0; k < 2 ; k++) {
		nPoint = 0;
		if(k == 0) {

			for(unsigned int iring = 0; iring < sumHitEEM.size() ; iring++) {
				if(sumHitEEM.at(iring) != 0 && nHitEEM.at(iring) != 0) {
					OccupancyEEM_vs_Eta->SetPoint(nPoint, -1.*(85 * xtalWidth + iring * etaringEE), sumHitEEM.at(iring) / (nHitEEM.at(iring)*luminosity));
					OccupancyEEM_vs_Eta->SetPointError(nPoint, 0., (sumHitEEM.at(iring) / (nHitEEM.at(iring)*luminosity)) / sqrt(nHitEEM.at(iring)*luminosity));
					OccupancyAll->SetPoint(nPointAll, -1.*(85 * xtalWidth + iring * etaringEE), sumHitEEM.at(iring) / (nHitEEM.at(iring)*luminosity));
					OccupancyAll->SetPointError(nPointAll, 0., (sumHitEEM.at(iring) / (nHitEEM.at(iring)*luminosity)) / sqrt(nHitEEM.at(iring)*luminosity));
					nPoint++;
					nPointAll++;
				}
				if(sumHitEEM.at(iring) == 0 || nHitEEM.at(iring) == 0) {
					OccupancyEEM_vs_Eta->SetPoint(nPoint, -1.*(85 * xtalWidth + iring * etaringEE), 0.);
					OccupancyEEM_vs_Eta->SetPointError(nPoint, 0., 0.);
					OccupancyAll->SetPoint(nPointAll, -1.*(85 * xtalWidth + iring * etaringEE), 0.);
					OccupancyAll->SetPointError(nPointAll, 0., 0.);
					nPoint++;
					nPointAll++;
				}

			}
		}

		if(k == 1) {
			for(unsigned int iring = 0; iring < sumHitEEP.size() ; iring++) {
				if(sumHitEEP.at(iring) != 0 && nHitEEP.at(iring) != 0) {
					OccupancyEEP_vs_Eta->SetPoint(nPoint, (85 * xtalWidth + iring * etaringEE), sumHitEEP.at(iring) / (nHitEEP.at(iring)*luminosity));
					OccupancyEEP_vs_Eta->SetPointError(nPoint, 0., (sumHitEEP.at(iring) / (nHitEEP.at(iring)*luminosity)) / sqrt(nHitEEP.at(iring)*luminosity));
					OccupancyAll->SetPoint(nPointAll, (85 * xtalWidth + iring * etaringEE), sumHitEEP.at(iring) / (nHitEEP.at(iring)*luminosity));
					OccupancyAll->SetPointError(nPointAll, 0., (sumHitEEP.at(iring) / (nHitEEP.at(iring)*luminosity)) / sqrt(nHitEEP.at(iring)*luminosity));
					nPoint++;
					nPointAll++;
				}

				if(sumHitEEP.at(iring) == 0 || nHitEEP.at(iring) == 0) {
					OccupancyEEP_vs_Eta->SetPoint(nPoint, (85 * xtalWidth + iring * etaringEE), 0.);
					OccupancyEEP_vs_Eta->SetPointError(nPoint, 0., 0.);
					OccupancyAll->SetPoint(nPointAll, (85 * xtalWidth + iring * etaringEE), 0.);
					OccupancyAll->SetPointError(nPointAll, 0., 0.);
					nPoint++;
					nPointAll++;
				}
			}
		}
	}


	sumHitEEM.clear() ;
	nHitEEM.clear() ;
	sumHitEEM.assign(40, 0.);
	nHitEEM.assign(40, 0);
	sumHitEEP.clear() ;
	nHitEEP.clear() ;
	sumHitEEP.assign(40, 0.);
	nHitEEP.assign(40, 0);

	for(int k = 0; k < 2 ; k++) {
		for(int ix = 0; ix < h_OccupancyEE2[k]->GetNbinsX() ; ix++) {
			for(int iy = 0; iy < h_OccupancyEE2[k]->GetNbinsY() ; iy++) {
				if(h_OccupancyEE2[k]->GetBinContent(ix + 1, iy + 1) == 0) continue;
				int mybin = h_OccupancyEE2[k] -> FindBin(ix, iy);
				int ring  = int(hrings[k]-> GetBinContent(mybin));
				float occ = h_OccupancyEE2[k]->GetBinContent(mybin);
				if(k == 0) {
					sumHitEEM.at(ring) = sumHitEEM.at(ring) + occ;
					nHitEEM.at(ring) = nHitEEM.at(ring) + 1;
				}
				if(k == 1) {
					sumHitEEP.at(ring) = sumHitEEP.at(ring) + occ;
					nHitEEP.at(ring) = nHitEEP.at(ring) + 1;
				}
			}
		}
	}

	for(int k = 0; k < 2 ; k++) {
		nPoint = 0;
		if(k == 0) {
			for(unsigned int iring = 0; iring < sumHitEEM.size() ; iring++) {
				if(sumHitEEM.at(iring) != 0 && nHitEEM.at(iring) != 0) {
					OccupancyAll2->SetPoint(nPointAll2, -1.*(85 * xtalWidth + iring * etaringEE), sumHitEEM.at(iring) / (nHitEEM.at(iring)*luminosity));
					OccupancyAll2->SetPointError(nPointAll2, 0., (sumHitEEM.at(iring) / (nHitEEM.at(iring)*luminosity)) / sqrt(nHitEEM.at(iring)*luminosity));
					nPoint++;
					nPointAll2++;
				}
				if(sumHitEEM.at(iring) == 0 || nHitEEM.at(iring) == 0) {
					OccupancyAll2->SetPoint(nPointAll2, -1.*(85 * xtalWidth + iring * etaringEE), 0.);
					OccupancyAll2->SetPointError(nPointAll2, 0., 0.);
					nPoint++;
					nPointAll2++;
				}
			}

		}
		if(k == 1) {
			for(unsigned int iring = 0; iring < sumHitEEP.size() ; iring++) {
				if(sumHitEEP.at(iring) != 0 && nHitEEP.at(iring) != 0) {
					OccupancyAll2->SetPoint(nPointAll2, (85 * xtalWidth + iring * etaringEE), sumHitEEP.at(iring) / (nHitEEP.at(iring)*luminosity));
					OccupancyAll2->SetPointError(nPointAll2, 0., (sumHitEEP.at(iring) / (nHitEEP.at(iring)*luminosity)) / sqrt(nHitEEP.at(iring)*luminosity));
					nPoint++;
					nPointAll2++;
				}
				if(sumHitEEP.at(iring) == 0 && nHitEEP.at(iring) == 0) {
					OccupancyAll2->SetPoint(nPointAll2, (85 * xtalWidth + iring * etaringEE), 0.);
					OccupancyAll2->SetPointError(nPointAll2, 0., 0.);
					nPoint++;
					nPointAll2++;
				}

			}
		}
	}

	gROOT->Reset();
	gROOT->SetStyle("Plain");

	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);
	gStyle->SetFitFormat("6.3g");
	gStyle->SetPalette(1);

	gStyle->SetTextFont(42);
	gStyle->SetTextSize(0.05);
	gStyle->SetTitleFont(42, "xyz");
	gStyle->SetTitleSize(0.05);
	gStyle->SetLabelFont(42, "xyz");
	gStyle->SetLabelSize(0.05);
	gStyle->SetTitleXOffset(0.8);
	gStyle->SetTitleYOffset(1.1);
	gROOT->ForceStyle();

	TFile *th = new TFile("output/Occupancy.root", "RECREATE");
	th->cd();

// Final plots
	TCanvas *cEB = new TCanvas ("EB occupancy", "EB occupancy");
	cEB->cd();
	cEB->SetGridx();
	cEB->SetGridy();
	h_OccupancyEB->Write();
	th->Close();


// Final plots
	TCanvas *cEEp = new TCanvas ("EE+ occupancy", "EE+ occupancy");
	cEEp->cd();
	cEEp->SetGridx();
	cEEp->SetGridy();
	OccupancyEEP_vs_Eta->SetMarkerStyle(20);
	OccupancyEEP_vs_Eta->SetMarkerColor(kGreen + 2);
	OccupancyEEP_vs_Eta->GetHistogram()->GetXaxis()-> SetRangeUser(1.5, 2.5);
	OccupancyEEP_vs_Eta->GetHistogram()->GetYaxis()-> SetTitle("Single xtal Event / fb^{-1}");
	OccupancyEEP_vs_Eta->GetHistogram()->GetXaxis()-> SetTitle("#eta");
	OccupancyEEP_vs_Eta->Draw("ap");


// Final plots
	TCanvas *cEEm = new TCanvas ("EE- occupancy", "EE- occupancy");
	cEEm->cd();
	cEEm->SetGridx();
	cEEm->SetGridy();
	OccupancyEEM_vs_Eta->SetMarkerStyle(20);
	OccupancyEEM_vs_Eta->SetMarkerColor(kGreen + 2);
	OccupancyEEM_vs_Eta->GetHistogram()->GetXaxis()-> SetRangeUser(-2.5, -1.5);
	OccupancyEEM_vs_Eta->GetHistogram()->GetYaxis()-> SetTitle("Single xtal Event / fb^{-1}");
	OccupancyEEM_vs_Eta->GetHistogram()->GetXaxis()-> SetTitle("#eta");
	OccupancyEEM_vs_Eta->Draw("ap");


	TCanvas* cAll = new TCanvas("cAll", "history plot vs date", 1);
	cAll ->cd();
	cAll ->SetGridx();
	cAll ->SetGridy();
	OccupancyAll->SetMarkerStyle(20);
	OccupancyAll->SetMarkerColor(kRed + 2);
	OccupancyAll->GetHistogram()->GetXaxis()-> SetRangeUser(-2.55, 2.55);
	OccupancyAll->GetHistogram()->GetYaxis()-> SetTitle("Number of Electrons / Crystal / fb^{-1}");
	OccupancyAll->GetHistogram()->GetXaxis()-> SetTitle("#eta_{seed}");
	OccupancyAll->Draw("ap");
	OccupancyAll2->SetMarkerStyle(20);
	OccupancyAll2->SetMarkerColor(kGreen + 2);
	OccupancyAll2->Draw("psame");
	TLegend * leg = new TLegend(0.5, 0.7, 0.6, 0.87);
	leg->SetFillColor(0);
	leg->AddEntry(OccupancyAll, "Preselected Events", "LP");
	leg->AddEntry(OccupancyAll2, "Events Used For Calibration", "LP");
	leg->Draw("same");


	char latexBuffer[250];

	sprintf(latexBuffer, "CMS 2012 Preliminary");
	TLatex* latex = new TLatex(0.18, 0.92, latexBuffer);
	latex -> SetNDC();
	latex -> SetTextFont(62);
	latex -> SetTextSize(0.035);
	latex -> Draw("same");

	sprintf(latexBuffer, "#sqrt{s} = 8 TeV   L = 11.35 fb^{-1}");
	TLatex* latex2 = new TLatex(0.6, 0.92, latexBuffer);
	latex2 -> SetNDC();
	latex2 -> SetTextFont(42);
	latex2 -> SetTextSize(0.035);
	latex2 -> Draw("same");

	cAll -> Print(("EleOccupancy_W_" + category + ".C").c_str(), "cxx");

	return 0;
}



void SetWHLTPathNames(std::vector<std::pair<std::string, std::pair<int, int> > > & WHLTPathNames)
{

	std::pair<int, int> WRunRanges1(190456, 190738);
	std::pair<std::string, std::pair<int, int> > WHLTPathName1("HLT_Ele27_WP80_v8", WRunRanges1);
	std::pair<int, int> WRunRanges2(190782, 191411);
	std::pair<std::string, std::pair<int, int> > WHLTPathName2("HLT_Ele27_WP80_v9", WRunRanges2);
	std::pair<int, int> WRunRanges3(191691, 196531);
	std::pair<std::string, std::pair<int, int> > WHLTPathName3("HLT_Ele27_WP80_v10", WRunRanges3);
	std::pair<int, int> WRunRanges4(198022, 209151);
	std::pair<std::string, std::pair<int, int> > WHLTPathName4("HLT_Ele27_WP80_v11", WRunRanges4);

	WHLTPathNames.push_back(WHLTPathName1);
	WHLTPathNames.push_back(WHLTPathName2);
	WHLTPathNames.push_back(WHLTPathName3);
	WHLTPathNames.push_back(WHLTPathName4);
}

