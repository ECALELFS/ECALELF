#include <TChain.h>
#include <TList.h>
#include <TFriendElement.h>
#include <TFile.h>
//#include <TIter.h>


#include <iostream>

#define DEBUG

void ReassociateFriends(TFile *f, TChain *chain)
{
	f->cd();
	chain->GetEntries();
	chain->LoadTree(0);
	TList *friends = chain->GetListOfFriends();
	if(friends == NULL) return;
	TIter newfriend_itr(friends);
	std::vector<TChain*> newFriendVec;
	for(TFriendElement *friendElement = (TFriendElement*) newfriend_itr.Next();
	        friendElement != NULL; friendElement = (TFriendElement*) newfriend_itr.Next()) {
		TString treeName = friendElement->GetTreeName();
		TTree *tree = friendElement->GetTree();
		std::cout << "[STATUS] Removing old friend " << treeName  << std::endl;
		chain->RemoveFriend(tree);
		TChain *friendChain = (TChain *) f->Get(treeName);
		if(friendChain == NULL) continue;
		newFriendVec.push_back(friendChain);
	}

	for(std::vector<TChain*>::const_iterator chain_itr = newFriendVec.begin();
	        chain_itr != newFriendVec.end();
	        chain_itr++) {
		TChain *friendChain = *chain_itr;
		chain->AddFriend(friendChain);
	}

	//TFriendElement *element = (TFriendElement*) friends->First();
	//element->GetTree()->Print();
	return;
}

TChain *GenChain(int index = 1)
{
	TChain *c = NULL;
	if(index == 1) {
		c = new TChain("selected", "1");
		c->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARECOSIM/DYJets-Summer12-START53-ZSkim-runDependent/194533-194533/semiParamRegression/DYJets-Summer12-START53-ZSkim-runDependent-194533-194533.root");
		c->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARECOSIM/DYJets-Summer12-START53-ZSkim-runDependent/200519-200519/semiParamRegression/DYJets-Summer12-START53-ZSkim-runDependent-200519-200519.root");
		c->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARECOSIM/DYJets-Summer12-START53-ZSkim-runDependent/206859-206859/semiParamRegression/DYJets-Summer12-START53-ZSkim-runDependent-206859-206859.root");
		TChain *p = new TChain("pileup", "");
		p->Add("tmp/mcPUtrees1.root");
		c->AddFriend(p);
	} else {
		c = new TChain("selected", "1");
		c->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARECOSIM/DYToEE_M20_powheg-Summer12-START53-ZSkim-runDependent/194533-194533/semiParamRegression/DYToEE_M20_powheg-Summer12-START53-ZSkim-runDependent-194533-194533.root");
		c->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARECOSIM/DYToEE_M20_powheg-Summer12-START53-ZSkim-runDependent/200519-200519/semiParamRegression/DYToEE_M20_powheg-Summer12-START53-ZSkim-runDependent-200519-200519.root");
		c->Add("root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/ntuples/8TeV/ALCARECOSIM/DYToEE_M20_powheg-Summer12-START53-ZSkim-runDependent/206859-206859/semiParamRegression/DYToEE_M20_powheg-Summer12-START53-ZSkim-runDependent-206859-206859.root");
		TChain *p = new TChain("pileup", "");
		p->Add("tmp/mcPUtrees2.root");
		c->AddFriend(p);

	}
	return c;
}


TChain * addChainAndFriends(std::vector<TChain *> chain_vec)
{
	TChain *sumChain = new TChain((*chain_vec.begin())->GetName(), "");

	std::map<TString, TChain *> friends_map;
	for(std::vector<TChain *>::const_iterator chain_itr = chain_vec.begin();
	        chain_itr != chain_vec.end();
	        chain_itr++) {

		TList *friendList = (*chain_itr)->GetListOfFriends();
		TIter newfriend_itr(friendList);

		for(TFriendElement *friendElement = (TFriendElement*) newfriend_itr.Next();
		        friendElement != NULL; friendElement = (TFriendElement*) newfriend_itr.Next()) {
			TString treeName = friendElement->GetTreeName();
			std::cout << "[STATUS] Adding new friend " << treeName  << std::endl;
			std::map<TString, TChain*>::iterator map_itr = friends_map.find(treeName);
			if(map_itr == friends_map.end()) {
				friends_map[treeName] = new TChain(treeName, "");
			}
			friends_map[treeName]->Add((TChain *)friendElement->GetTree());
		}
	}

	for(std::vector<TChain *>::const_iterator chain_itr = chain_vec.begin();
	        chain_itr != chain_vec.end();
	        chain_itr++) {
		sumChain->Add(*chain_itr);
	}

	for(std::map<TString, TChain *>::const_iterator map_itr = friends_map.begin();
	        map_itr != friends_map.end();
	        map_itr++) {
		sumChain->AddFriend(map_itr->second);
	}
	sumChain->GetListOfFriends()->Print();
	return sumChain;
}

TChain * addChainAndFriends(TChain *c1, TChain *c2)
{
	std::vector<TChain *> chain_vec;
	chain_vec.push_back(c1);
	chain_vec.push_back(c2);
	return addChainAndFriends(chain_vec);
}
