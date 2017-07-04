import argparse
import sys

parser = argparse.ArgumentParser(description='Tex summary table of steps 5 to 8.')
parser.add_argument('dir', type=str,
								  help='directory with step outfiles')
parser.add_argument('invMass', type=str,
								  help='invMass variable')
parser.add_argument('source', type=str,
								  help='fitresult or outfile')

args = parser.parse_args()

#import ROOT
import re
from distutils.version import LooseVersion
from collections import defaultdict

def CategorySort(s):
	s = s.replace("bad","0")
	s = s.replace("good","1")
	s = re.split(r"\D", s)
	return map(float,filter(None,s))

def sort_method(s):
	digits = "1234567890."
	key = [""]
	n = s[0] in digits
	for c in s:
		ni = c in digits
		if n == ni:
			key[-1] += c
		else:
			if n:
				key[-1] = float(key[-1])
			key.append(c)
			n = ni
	return key

class Step:
	pattern = re.compile("([^_]*)_(.*)\s*=\s* ([-0-9.e]*) \+/- ([-]?inf|[0-9.e]*).*")
	def __init__(self, n, dirname, datfiles, commoncut):
		self.variables = defaultdict(dict)
		self.n= n
		self.name = "Step {}".format(n)
		for datfile in datfiles:
			filename = dirname + "/" + datfile.format(step=n)
			try:
				with open(filename, "r") as f:
					for line in f:
						if "phi4" in line: continue
						line = line.replace("-" + commoncut, "")
						m = self.pattern.match(line)
						varname, category, value, value_err = m.groups()
						if "scale" == varname:
							v = "${:.2f} \pm {:.3f}$".format(100* (1 - float(value)), 100*float(value_err))
						elif "phi" == varname:
							v = "${:.2f} \pm {:.3f}$".format( float(value), float(value_err))
						else:
							v = "${:.2f} \pm {:.3f}$".format(100* float(value), 100*float(value_err))
						self.variables[varname.strip()][category.strip()] = v
			except:
				pass
	def __str__(self):
		s = ""
		for var in self.variables:
			for cat in self.variables[var]:
				x = self.variables[var][cat]
				s += "{} {} {} \n".format(var,cat,x)
		return s

class Table:
	pattern = re.compile("(.*) = .*")
	var_title = {
			"scale": r"$1 - \Delta P$ [\%]",
			"phi":   r"$\phi$ [rad]",
			"constTerm": r"$\Delta C$ [\%]",
			"alpha": r"$\alpha$ [\%]",
			"rho": r"$\rho$ [\%]",
			}
	def __init__(self, dirname, datfiles, commoncut):
		self.categories = []
		self.columns = []
		self.map = {}
		for datfile in datfiles:
			with open(dirname + "/" + datfile, "r") as f:
				for line in f:
					if self.var_names[0] in line:
						if "phi4" in line: continue
						m = self.pattern.match(line.replace("-" + commoncut, "").replace(self.var_names[0] + "_",""))
						category, = m.groups()
						category = category.strip()
						try:
							cat_string = self.splitCategory(category)
						except:
							continue
						self.map[cat_string] = category
						self.categories.append(cat_string)
		self.categories.sort(key=sort_method)
		self.CountCategories()
	def CountCategories(self):
		self.print_row = [ [0] for i in range(self.n_bins) ]
		for i in xrange(self.n_bins):
			last_bin = ""
			for cat in self.categories:
				this_bin = cat.split()[i]
				if this_bin == last_bin:
					self.print_row[i][-1] += 1
				else:
					self.print_row[i].append(self.print_row[i][-1] + 1)
					last_bin = this_bin
			
	def AddStep(self, step):
		self.columns.append(step)
	def Print(self):
		tex_form = "|" + ("r|"*self.n_bins) + ("r|" * len(self.columns))
		header = r'\begin{tabular}{' + tex_form + r'}'
		footer = r'\end{tabular}\vspace{5mm}'

		print header
		print "\\hline"
		for var_name in self.var_names:
			print " & "*self.n_bins
			print " & ".join(["{} {}".format(column.name,self.var_title[var_name]) for column in self.columns]),
			print "\\\\ \\hline"
			for nrow, cat in enumerate(self.categories):
				labels = cat.split()
				for i in range(self.n_bins):
					if nrow in self.print_row[i]:
						print self.TexLabel(labels[i]),
					print "&",
				key = self.map[cat]
				print " & ".join([ column.variables[var_name][key] if key in column.variables[var_name] else " " for column in self.columns]),
				print "\\\\"
				for i in xrange(self.n_bins - 1):
					if nrow + 1 in self.print_row[i]:
						print "\\cline{{{}-{}}}".format(i+1, self.n_bins +  len(self.columns))
						break
		print footer
		print
	def TexLabel(self,label):
		label = re.sub("Et_([0-9.]*)_([0-9.]*)", lambda x: "${0} < E_T < {1}$".format(*x.groups()), label)
		label = re.sub("absEta_([0-9.]*)_([0-9.]*)", lambda x: "${0} < |\eta| < {1}$".format(*x.groups()), label)
		label = label.replace("gold","$R_9 > 0.94$")
		label = label.replace("bad","$R_9 < 0.94$")
		return label


class ScaleTable(Table):
	def __init__(self, dirname, datfile, commoncut):
		self.var_names = ["scale"]
		self.n_bins = 3 # eta r9 et
		Table.__init__(self,dirname,datfile,commoncut)
	def splitCategory(self, category):
		Et,eta,r9 = category.split("-")
		return " ".join((eta, r9, Et))



class SmearTable(Table):
	def __init__(self, dirname, datfile, commoncut, var_names = ["constTerm", "alpha"]):
		self.var_names = var_names
		self.n_bins = 2 # eta r9
		Table.__init__(self,dirname,datfile,commoncut)
	def splitCategory(self, category):
		eta,r9 = category.split("-")
		return " ".join((eta, r9))



	#def addStep(self, step):

commoncut = "EtLeading_32-EtSubLeading_20-noPF-isEle"
corrEleType = "HggRunEtaR9Et"
outfile_form_table = "table/step{step}EE-{invmass}-loose25nsRun22016Moriond-{commoncut}-{corrEleType}.dat".format(step="{step}", invmass=args.invMass, commoncut=commoncut, corrEleType=corrEleType)
outfile_form_result_EB = "step{step}EB/img/outProfile-scaleStep2smearing_6-{commoncut}-FitResult-.config".format(step="{step}", commoncut=commoncut)
outfile_form_result_EE = "step{step}EE/img/outProfile-scaleStep2smearing_7-{commoncut}-FitResult-.config".format(step="{step}", commoncut=commoncut)

if args.source == "outfile":
	outfiles = [outfile_form_table]
elif args.source == "fitresult":
	outfiles = [outfile_form_result_EB, outfile_form_result_EE]
else: 
	print "source = fitresult or outfile"
	sys.exit()
outfile_form = outfiles[0]

scale_table        = ScaleTable(args.dir, [o.format(step=5) for o in outfiles], commoncut)
smear_table        = SmearTable(args.dir, [o.format(step=5) for o in outfiles], commoncut)
smear_rhophi_table = SmearTable(args.dir, [o.format(step=7) for o in outfiles], commoncut, ["rho", "phi"])

steps = [ Step(i, args.dir, outfiles, commoncut) for i in [5,6,7,8] ]

for step in steps:
	scale_table.AddStep(step)
	smear_table.AddStep(step)

smear_rhophi_table.AddStep(steps[2])
smear_rhophi_table.AddStep(steps[3])

print r'\documentclass{article} \usepackage{mathtools} \usepackage{multirow} \usepackage[margin=.5in]{geometry} \usepackage{pdflscape} \begin{document}\begin{landscape}'
scale_table.Print()
smear_table.Print()
smear_rhophi_table.Print()
print r'\end{landscape}\end{document}'
