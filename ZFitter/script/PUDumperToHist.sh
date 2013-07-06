#!/bin/bash


usage(){
    echo "`basename $0` "
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf: -l help -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-f) FILE=$2; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done



if [ -z "${FILE}" ];then
    echo "[ERROR] PUDumper file not provided" >> /dev/stderr
    exit 1
fi

FILEDIR=`dirname ${FILE}`

cat >tmp/PUDumperToHist.C <<EOF
{
  TChain c("PUDumper/pileup");
  c.Add("${FILE}");
  c.Draw("nPUtrue>>pileup","BX==0");
  TH1F *hist = gROOT->FindObject("pileup");
  hist->SaveAs("${FILEDIR}/nPUtrue.root");
}
EOF
    

root -l -b -q tmp/PUDumperToHist.C
