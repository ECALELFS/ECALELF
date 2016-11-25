#include <TFile.h>
#include <TList.h>
#include <TKey.h>
#include <TObject.h>

#include <TString.h>
#include <TGraph.h>

void haddTGraph(std::vector<TString> filenames, TString outFileName)
{

	TFile f_out(outFileName, "recreate");

	for(std::vector<TString>::const_iterator f_itr = filenames.begin();
	        f_itr != filenames.end();
	        f_itr++) {
		TFile f_in(*f_itr);
		//f_in.Open();
		f_in.cd();

		TList *KeyList = f_in.GetListOfKeys();
		for(int i = 0; i <  KeyList->GetEntries(); i++) {
			TKey *currentKey = (TKey *)KeyList->At(i);
			TString className = currentKey->GetClassName();
			// se non e' un TGraph passa oltre
			//std::cout << i << " " << className << std::endl;
			if (! className.CompareTo("TGraph") == 0) continue;
			TGraph *g = (TGraph *)currentKey->ReadObj();
			//TString objName = TGraph->GetName();

			f_out.cd();
			g->Write();
			f_in.cd();
		}
		f_in.Close();
	}

	f_out.Close();
}
