{
int i = 0;
bool	puReweight = true;

TCanvas *c[200];

std::vector<TString> mcLabel_vec, dataLabel_vec;
mcLabel_vec.push_back("madgraph");
//  mcLabel_vec.push_back("Powheg");
//  mcLabel_vec.push_back("Sherpa");

dataLabel_vec.push_back("Data");

TString eleID("eleID_loose25nsRun22016Moriond");


//TCanvas *PlotDataMCs(TChain *data, std::vector<TChain *> mc_vec, TString branchname, TString binning, 
//		     TString category,  TString selection, 
//		     TString dataLabel, std::vector<TString> mcLabel_vec, TString xLabel, TString yLabelUnit, TString outputPath, TString label4Print,
//		     bool logy=false, bool usePU=true, bool ratio=true,bool smear=false, bool scale=false, bool useR9Weight=false, TString pdfIndex=""){
PlotDataMCs(data, MakeChainVector(signalA),
 		  	"nPV", "(40,0,40)", "Et_25-" + eleID, "",
 		  	"Data", mcLabel_vec, "nVtx ", "", outputPath, "nPV",
 		  	false, puReweight, false, false);

PlotDataMCs(data, MakeChainVector(signalA),
 		  	"R9Ele", "(100,0,1)", "Et_25-" + eleID, "",
 		  	"Data", mcLabel_vec, "R_9 Ele ", "", outputPath, "R9Ele",
 		  	false, puReweight, false, false);

PlotDataMCs(data, MakeChainVector(signalA),
 		  	"etaSCEle", "(100,-2.5,2.5)", "Et_25-" + eleID, "",
 		  	"Data", mcLabel_vec, "#eta_{SC}", "", outputPath, "etaSCEle",
 		  	false, puReweight, false, false);

// }
