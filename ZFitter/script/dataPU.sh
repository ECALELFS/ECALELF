#!/bin/bash
CERT_BASE_DIR=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification
MINBIAS=69400

usage(){
    echo 
    echo "============================== Usage: "
    echo "`basename $0` <-a | < -p pileupFile  -j jsonFile> > [--pixel] [--printOnly]" #| -u crab_working_dir >" 
    echo " -a: automatic retreive of the last jsonFile and last pileupFile"
    echo "Optional parameters:"
    echo " -m minbias (=$MINBIAS)"
    echo " -r 190000-200000 : run range"
    echo " --printOnly"
    echo " --pixel : use PU JSON with pixel correction"
    echo "=============================="

}

desc(){
    echo "
######################################################################
## This script take 
## - a JSON file 
## - a PU JSON file that has the istantaneous lumi per LS
## - the minimum bias cross section
## and calculates the observed PU distribution in data
## Usually JSON files are in 
##  /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Prompt
## PU JSON files are in
## /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/PileUp
######################################################################
"
}



runRangeJSON(){
_RUNRANGE=$1
_JSON=$2
_RUNMIN=`echo $_RUNRANGE | cut -d '-' -f 1`
_RUNMAX=`echo $_RUNRANGE | cut -d '-' -f 2`
_JSONNEW=$3
## take the runlist:
#_RUNLIST=`printJSON.py $_JSON | sed 's|{||;s|}||;s|\"||g;' | cut -d ':' -f 1` 
##echo $RUNLIST
#
#_newJSON="{"
#for run in $_RUNLIST
#  do
#  if [ "$run" -ge "$_RUNMIN" -a "$run" -le "$_RUNMAX" ]; then _newJSON=$_newJSON"\"$run\": [[1,10000]],"; fi
#done 
#_newJSON=$_newJSON"}"
#_newJSON=`echo $_newJSON | sed 's|,}|}|'`
#echo $_newJSON > tmp/$_RUNRANGE-tmp.json
#compareJSON.py --and tmp/$_RUNRANGE-tmp.json $_JSON 
filterJSON.py --min=$_RUNMIN --max=$_RUNMAX $_JSON --output=$_JSONNEW
}



getLast(){
    if [ ! -e "tmp" ];then mkdir tmp; fi
    echo "Automatic retreive of JSON file"
    CERT_DIR=$CERT_BASE_DIR/Collisions12/8TeV
    JSON_DIR=$CERT_DIR/Prompt
    PUJSON_DIR=$CERT_DIR/PileUp

#    JSON=`ls -tr $JSON_DIR/*8TeV_PromptReco_Collisions12_JSON[_][v][0-9].txt |tail -1`
    JSON=`ls -tr $JSON_DIR/*8TeV_PromptReco_Collisions12_JSON.txt |tail -1`
    echo "[INFO] JSON=$JSON"
    PUJSON=`ls -tr $PUJSON_DIR/pileup_JSON_DCSONLY_*$PIXEL.txt |tail -1`
    echo "[INFO] PUJSON=$PUJSON"
    echo "[INFO] Using PUJSON=${PUJSON_DIR}/pileup_latest.txt"
    PUJSON=${PUJSON_DIR}/pileup_latest.txt
    #echo "[WARNING] RUN2012A and RUN2012B are rerecoed with rereco13Jul json file"
#    JSON29JunRereco="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-196531_8TeV_29Jun2012ReReco_Collisions12_JSON.txt"
    #JSON13JulRereco="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt"
    #runRangeJSON 190456-196531 $JSON13JulRereco > tmp/rereco.json
    #runRangeJSON 196532-300000 $JSON > tmp/prompt.json
#    echo "compareJSON.py --or ${JSON13JulRereco} $JSON > tmp/last.json"
    #compareJSON.py --or tmp/rereco.json tmp/prompt.json > tmp/13Julv2_lastPrompt.json
    #JSON=tmp/13Julv2_lastPrompt.json
}


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hu:p:j:am:r: -l help,desc,printOnly,pixel -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	--desc) desc;;
	-u) UI_WORKING_DIR=$2; shift;;
	-j) JSON=$2; shift;;
	-p) PUJSON=$2; shift;;
	-a) getLast;;
	-m) MINBIAS=$2; shift;;
	-r) RUNRANGE=$2; shift;;
	--printOnly) PRINTONLY=y;;
	--pixel) PIXEL=y;;
	-h|--help) usage; exit 0;;
	(--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

if [ -z "${PUJSON}" ];then
    echo -e "[ERROR]  PUJSON not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "${JSON}" -a -z "${UI_WORKING_DIR}" ];then
	echo -e "[ERROR] JSON file and crab working directory not defined" >> /dev/stderr
	usage >> /dev/stderr
	exit 1
fi

JSONNAME=`basename $JSON .txt`
JSONNAME=`basename $JSONNAME .json`
if [ -n "$PIXEL" ];then JSONNAME=$JSONNAME-pixelcorr; fi


if [ -n "$RUNRANGE" ];then
    
    runRangeJSON $RUNRANGE $JSON tmp/$RUNRANGE.json
    JSON=tmp/$RUNRANGE.json
    #echo $newJSON > tmp/$RUNRANGE-tmp.json
#    echo $JSON
    #compareJSON.py --and tmp/$RUNRANGE-tmp.json $JSON > 

    echo "JSON file: ${JSON}"
    JSONNAME=$JSONNAME-$RUNRANGE
    echo "JSONNAME=${JSONNAME}"
fi

if [ -n "$PRINTONLY" ];then exit 0;fi

if [ ! -e "data/puFiles/$JSONNAME.root" ];then
    if [ ! -d "data/puFiles" ];then mkdir data/puFiles -p; fi
    pileupCalc.py -i ${JSON} --inputLumiJSON $PUJSON --calcMode true --minBiasXsec $MINBIAS --maxPileupBin 100 --numPileupBins 100  data/puFiles/$JSONNAME.root
fi


