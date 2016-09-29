#ifndef _TECALCHAIN_H
#define _TECALCHAIN_H

#include <TChain.h>

class TECALChain : public TChain
{
public:
	virtual void SetEntryList(TEntryList *elist)
	{
		if (fEntryList) {
			//check, if the chain is the owner of the previous entry list
			//(it happens, if the previous entry list was created from a user-defined
			//TEventList in SetEventList() function)
			if (fEntryList->TestBit(kCanDelete)) {
				TEntryList *tmp = fEntryList;
				fEntryList = 0; // Avoid problem with RecursiveRemove.
				delete tmp;
			} else {
				fEntryList = 0;
			}
		}
		if (!elist) {
			fEntryList = 0;
			fEventList = 0;
			return;
		}
		if (!elist->TestBit(kCanDelete)) {
			//this is a direct call to SetEntryList, not via SetEventList
			fEventList = 0;
		}
		if (elist->GetN() == 0) {
			fEntryList = elist;
			return;
		}
		if (fProofChain) {
			//for processing on proof, event list and entry list can't be
			//set at the same time.
			fEventList = 0;
			fEntryList = elist;
			return;
		}

		Int_t ne = fFiles->GetEntries();
		Int_t listfound = 0;
		TString treename, filename;

		TEntryList *templist = 0;
		for (Int_t ie = 0; ie < ne; ie++) {
			auto chainElement = (TChainElement*)fFiles->UncheckedAt(ie);
			treename = chainElement->GetName();
			filename = chainElement->GetTitle();
			templist = elist->GetEntryList(filename + "/" + treename, filename);
			/* std::cout << "[yacine] treename :: " << treename << std::endl; */
			/* std::cout << "[yacine] filename :: " << filename << std::endl; */
			/* std::cout << "[yacine] templist :: " << templist << std::endl; */
			if (templist) {
				listfound++;
				templist->SetTreeNumber(ie);
			}
		}
		if (listfound == 0) {
			Error("SetEntryList", "[yacine] No list found for the trees in this chain");
			fEntryList = 0;
			return;
		}
		fEntryList = elist;
		TList *elists = elist->GetLists();
		Bool_t shift = kFALSE;
		TIter next(elists);

		//check, if there are sub-lists in the entry list, that don't
		//correspond to any trees in the chain
		while((templist = (TEntryList*)next())) {
			if (templist->GetTreeNumber() < 0) {
				shift = kTRUE;
				break;
			}
		}
		fEntryList->SetShift(shift);
	}
};

#endif

