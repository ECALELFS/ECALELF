import ROOT

def ReassociateFriends(treename,filename):
	'''
	Associate friends of treename with branches in filename
	'''
	file0 = ROOT.TFile.Open(filename, "UPDATE")
	
	tree = file0.Get(treename)
	for f in [_ for _ in tree.GetListOfFriends()]:
		name = f.GetName()
		tree.RemoveFriend(f.GetTree())
		tree.AddFriend(name, file0)
	tree.Write()

if __name__ == "__main__":
	import sys
	ReassociateFriends("selected",sys.argv[1])
