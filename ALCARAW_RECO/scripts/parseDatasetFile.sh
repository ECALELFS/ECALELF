#!/bin/bash
# This script is meant to transform the .dat file in a series of
#options for the other scripts, interpreting the meaning of the columns
usage(){
    echo "`basename $0` FILE" 
    echo "   FILE: alcaraw_datasets.dat | alcarereco_datasets.dat | alcareco_datasets.dat"
}

FILE=$1

#./script/prodAlcaraw.sh -r 190450-193686 -d /DoubleElectron/Run2012A-PromptReco-v1/RECO -n DoubleElectron-RUN2012A-v1  --store caf --remote_dir store/group/alca_ecalcalib/alcaraw 

if [ -z "$FILE" ];then
    echo "[ERROR] You must provide a file" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi


case `basename $FILE` in
    alcaraw_datasets.dat)
	awk '($NF!=0 && (! /^#/)) {print "-r",$1, "-d", $2, "-n", $3, "--store", $4, "--remote_dir", $5, $6, $7}' $FILE | grep -v INVALID
	;;
    alcarereco_datasets.dat)
	awk '($NF!=0 && (! /^#/)) {print "-r",$1, "-d", $2, "-n", $3, "--store", $4, "--remote_dir", $5, "-t", $6}' $FILE
	;;
    alcareco_datasets.dat)
	awk '($NF!=0 && (! /^#/) && (NF>7 && $8!="-")) {print "-r",$1, "-d", $2, "-n", $3, "--store", $4, "--remote_dir", $5, $6, $7, "--dbs_url", $8}; ($NF!=0 && (! /^#/) && (NF==7|| $8=="-")){print "-r",$1, "-d", $2, "-n", $3, "--store", $4, "--remote_dir", $5, $6, $7};' $FILE | grep -v INVALID
	;;
    ntuple_datasets.dat)
	awk -F '\t' '($NF!=0 && (! /^#/)){if($7!=""){print "-r",$1, "-d", $2, "-n", $3, "--store", $4, "--remote_dir", $5, "--type", $6, "-t", $7, "--json_name", $8} else {print "-r",$1, "-d", $2, "-n", $3, "--store", $4, "--remote_dir", $5, "--type", $6, "--json_name", $8}}' $FILE
	;;
    *)
	echo "[ERROR] File provided not recognized" >> /dev/stderr
	usage >> /dev/stderr
	;;
esac



