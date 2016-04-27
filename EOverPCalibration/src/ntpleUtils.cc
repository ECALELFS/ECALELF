#include "../interface/ntpleUtils.h"


//  ------------------------------------------------------------



std::map<int, int> GetTotalEvents(const std::string& histoName, const std::string& inputFileList)
{
	std::ifstream inFile(inputFileList.c_str());
	std::string buffer;
	std::map<int, int> totalEvents;

	if(!inFile.is_open()) {
		std::cerr << "** ERROR: Can't open '" << inputFileList << "' for input" << std::endl;
		return totalEvents;
	}

	while(1) {
		inFile >> buffer;
		if(!inFile.good()) break;

		TFile* f = TFile::Open(buffer.c_str());
		TH1F* histo = NULL;
		f -> GetObject(histoName.c_str(), histo);
		if(histo == NULL) {
			std::cout << ">>>ntpleUtils::Error in getting object " << histoName << std::endl;
			exit(-1);
		}


		for(int bin = 1; bin <= histo -> GetNbinsX(); ++bin)
			totalEvents[bin] += int(histo -> GetBinContent(bin));

		f -> Close();
		delete f;
	}

	return totalEvents;
}



TH1F* GetTotalHisto(const std::string& histoName, const std::string& inputFileList)
{
	std::ifstream inFile(inputFileList.c_str());
	std::string buffer;
	TH1F* totalHisto = NULL;

	if(!inFile.is_open()) {
		std::cerr << ">>>ntpleUtils::GetTotalHisto::can't open file " << inputFileList << " for input" << std::endl;
		return totalHisto;
	}

	bool isFirstFile = true;
	while(1) {
		inFile >> buffer;
		if(!inFile.good()) break;

		//std::cout << "ntpleUtils::GetTotalHisto::getting histogram " << histoName << " from file " << buffer << std::endl;
		TFile* f = TFile::Open(buffer.c_str());
		TH1F* histo = NULL;
		f -> GetObject(histoName.c_str(), histo);
		if(histo == NULL) {
			std::cout << ">>>ntpleUtils::GetTotalHisto::Error in getting object " << histoName << std::endl;
			exit(-1);
		}

		if( isFirstFile )
			totalHisto = (TH1F*)(histo->Clone());
		else
			totalHisto -> Add(histo);

		if( !isFirstFile ) {
			f -> Close();
			delete f;
			isFirstFile = false;
		}
	}

	return totalHisto;
}



std::map<int, std::string> GetBinLabels(const std::string& histoName, const std::string& inputFileList)
{
	std::ifstream inFile(inputFileList.c_str());
	std::string buffer;
	std::map<int, std::string> binLabels;

	if(!inFile.is_open()) {
		std::cerr << "** ERROR: Can't open '" << inputFileList << "' for input" << std::endl;
		return binLabels;
	}

	while(1) {
		inFile >> buffer;
		if(!inFile.good()) break;

		TFile* f = TFile::Open(buffer.c_str());
		TH1F* histo = NULL;
		f -> GetObject(histoName.c_str(), histo);
		if(histo == NULL) {
			std::cout << ">>>ntpleUtils::Error in getting object " << histoName << std::endl;
			exit(-1);
		}


		for(int bin = 1; bin <= histo -> GetNbinsX(); ++bin)
			binLabels[bin] = std::string(histo -> GetXaxis() -> GetBinLabel(bin));

		f -> Close();
		delete f;
	}

	return binLabels;
}



bool FillChain(TChain& chain, const std::string& inputFileList)
{
	std::ifstream inFile(inputFileList.c_str());
	std::string buffer;

	if(!inFile.is_open()) {
		std::cerr << "** ERROR: Can't open '" << inputFileList << "' for input" << std::endl;
		return false;
	}

	while(1) {
		inFile >> buffer;
		if(!inFile.good()) break;
		if( buffer.at(0) == '#' ) continue;
		chain.Add(buffer.c_str());
		std::cout << ">>> ntupleUtils::FillChain - treeName = " << chain.GetName() << " from file " << buffer << std::endl;
	}

	return true;
}



//  ------------------------------------------------------------



double deltaPhi(const double& phi1, const double& phi2)
{
	double deltaphi = fabs(phi1 - phi2);
	if (deltaphi > 6.283185308) deltaphi -= 6.283185308;
	if (deltaphi > 3.141592654) deltaphi = 6.283185308 - deltaphi;
	return deltaphi;
}

double deltaEta(const double& eta1, const double& eta2)
{
	double deltaeta = fabs(eta1 - eta2);
	return deltaeta;
}

double deltaR(const double& eta1, const double& phi1,
              const double& eta2, const double& phi2)
{
	double deltaphi = deltaPhi(phi1, phi2);
	double deltaeta = deltaEta(eta1, eta2);
	double deltar = sqrt(deltaphi * deltaphi + deltaeta * deltaeta);
	return deltar;
}



//  ------------------------------------------------------------



// void Print4V(const ROOT::Math::XYZTVector& p)
// {
//   std::cout << std::fixed << std::setprecision(2)
//             << "(E,px,py,pz) = "
//             << "(" << std::setw(8) << p.energy()
//             << "," << std::setw(8) << p.px()
//             << "," << std::setw(8) << p.py()
//             << "," << std::setw(8) << p.pz()
//             << ")"
//             << "\tpt = "
//             << std::setw(7) << p.pt()
//             << "\teta = "
//             << std::setw(6) << p.eta()
//             << "\tphi = "
//             << std::setw(5) << p.phi()
//             << std::endl;
// }
