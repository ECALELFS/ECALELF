#!/bin/bash

usage(){
    echo "This script take one file with the mc PU weights and one ZNtuple"
    echo "and produce a tree with one branch with the PU weight per event"
    echo "`basename $0` -w weightFile -f mcTreeFile [-o outputFileName]" 
}


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hw:f:o: -l help -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-w) puWeightFile=$2; shift;;
	-f) mcFile=$2; shift;;
	-o) puWeightName=`basename $2`; shift;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done
 
if [ -z "$puWeightFile" ];then
    echo "[ERROR] puWeightFile not defined: use option -w" >> /dev/stderr
    exit 1
elif [ ! -r "$puWeightFile" ];then
    echo "[ERROR] puWeightFile not found or not readable" >> /dev/stderr
    exit 1
fi

if [ -z "$mcFile" ];then
    echo "[ERROR] mcFile not defined: use option -w" >> /dev/stderr
    exit 1
fi

    

treeName=selected
cat > tmp/puWeightTree.C <<EOF

{

 gROOT->LoadMacro("src/puWeights_class.cc");
puWeights_class puWeights;
puWeights.ReadFromFile("$puWeightFile");

TFile f("$mcFile");
TTree *tree = (TTree *) f.Get("$treeName");
if(tree==NULL){
std::cerr << "[ERROR] TTree with name $treeName not found in file $mcFile" << std::endl;
}

// take nPU from ntuples
Int_t nPU=-1;
tree->SetBranchAddress("nPU", &nPU);

// declare new tree in new file with branch puWeight
Float_t weight=1;
TFile f_out("$puWeightName.root","CREATE");
f_out.cd();
TTree puTree("puWeight","$puWeightName");
puTree.Branch("puWeight", &weight, "puWeight/F");

// loop over the ntuple and fill the new branch of the new tree
Long64_t entries = tree->GetEntries();
for(Long64_t ientry = 0; ientry < entries; ientry++){
tree->GetEntry(ientry);
weight = puWeights.GetWeight(nPU);
puTree.Fill();
}

f_out.cd();
puTree.Write();
f_out.Close();


}
EOF


root -l -b  q tmp/puWeightTree.C
