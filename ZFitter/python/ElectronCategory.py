import ROOT


ROOT.gROOT.LoadMacro("src/ElectronCategory_class.cc+")
#ROOT.gInterpreter.ProcessLine('#include "src/ElectronCategory_class.cc"')
cutter = ROOT.ElectronCategory_class()
defaultEnergyBranch = cutter.energyBranchName

def ReduceFormula(formula):
	import re
	reg = ["[|&!:,]+"]
	reg += ["[><=%]+"]
	reg += ["\\[[01]\\]"]
	reg += ["[- .0-9]{2,}"]
	reg += ["abs[ ]*\\("]
	reg += ["sqrt[ ]*\\("]
	reg += ["max[ ]*\\("]
	reg += ["min[ ]*\\("]
	reg += ["cosh[ ]*\\("]
	reg += ["[\\(\\)]{1,}"]
	reg += ["[*/+-]{1,}"]
	reg += ["[ ]{1,}"]
	reg += ["SingleEle"]
	for r in reg:
		formula = re.sub(r, " ", formula)
	formula.replace("scaleEle", "");

	return set(formula.split())

if __name__ == "__main__":
	myset = cutter.GetBranchNameNtupleVec("Et_25")
	print [x for x in myset]
	print "Done"
