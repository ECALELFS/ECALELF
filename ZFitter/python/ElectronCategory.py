import ROOT


ROOT.gROOT.LoadMacro("src/ElectronCategory_class.cc+")
#ROOT.gInterpreter.ProcessLine('#include "src/ElectronCategory_class.cc"')
cutter = ROOT.ElectronCategory_class()
defaultEnergyBranch = cutter.energyBranchName

if __name__ == "__main__":
	myset = cutter.GetBranchNameNtupleVec("Et_25")
	print [x for x in myset]
	print "Done"
