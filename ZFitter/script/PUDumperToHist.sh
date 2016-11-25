#!/bin/bash

OUTFILE=nPUtrue.root
usage(){
    echo "`basename $0` -f file.root -o outfile.root"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:o: -l help -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-f) FILES="$FILES $2"; shift;;
#	-d) FILEDIR=$2; shift;;
	-o) OUTFILE=$2; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done



if [ -z "${FILES}" ];then
    echo "[ERROR] PUDumper file not provided" >> /dev/stderr
    exit 1
fi

echo $FILES
cat >tmp/PUDumperToHist.C <<EOF
{
  TChain c("PUDumper/pileup");
EOF
for FILE in ${FILES}
  do
  echo $FILE
cat >>tmp/PUDumperToHist.C <<EOF
  c.Add("${FILE}");
EOF
done
cat >>tmp/PUDumperToHist.C <<EOF
  c.Draw("nPUtrue>>pileup(100,0,100)","BX==0");
  TH1F *hist = (TH1F*) gROOT->FindObject("pileup");
  hist->SaveAs("${OUTFILE}");
}
EOF
    

root -l -b -q tmp/PUDumperToHist.C
