#!/bin/bash
################
# Semplified script for rereco and ntuple of 2011 and 2012 data
##############
NTUPLE_REMOTE_DIR=group/alca_ecalcalib/ecalelf/ntuples
ALCARERECO_REMOTE_DIR=group/alca_ecalcalib/ecalelf/alcarereco
#or defined as `echo $dataset |  sed 's|.*--remote_dir ||;s| .*$||;s|alcaraw|alcarereco|'`
SCHEDULER=caf
CREATE=y  #not used
SUBMIT=y  #not used
#NTUPLE=y  #not used
#RERECO=y  #not used
fileList=alcaraw_datasets.dat

usage(){
    echo "`basename $0` -p period -t tagFile"
    echo "    -t | --tag_file tagFile"
    echo "    --scheduler caf,lsf (=$SCHEDULER)"
    echo "--------------- FOR RERECO only"
    echo "    -p | --period <test | RUN2011AB | RUN2012AB | RUN2012ABC | Cal_Nov2012 | Cal_Dic2012>"
    echo "    --singleEle: if you want to rereco also single electron datasets"
    echo "--------------- FOR NTUPLE only"
    echo "    --json_name jsonName"
    echo "    --json jsonFile.root"
    echo "    --doExtraCalibTree"
    echo "    --doEleIDTree"
    echo "    --noStandardTree"
    echo "--------------- Expert"
    echo "    --alcarerecoOnly"
 #   echo "    --ntupleOnly"
    echo "    --submitOnly"
    echo "    --createOnly"
    echo "    --check"
    echo "--------------- ECALELF Tutorial"
    echo "    --tutorial"
}


#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:p: -l help,tag_file:,period:,scheduler:,singleEle,createOnly,submitOnly,check,alcarerecoOnly,json_name:,json:,tutorial,doExtraCalibTree,doEleIDTree,noStandardTree -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-t | --tag) TAGFILE=$2; echo "[OPTION] TAGFILE:$TAGFILE"; TAG=`basename ${TAGFILE} .py`; shift;;
	-p | --period) PERIOD=$2; shift;;
	--tutorial) 
	    echo "[OPTION] Entering in tutorial mode"
	    TUTORIAL="--tutorial";
	    ;;
	--scheduler) SCHEDULER=$2; 
	    case $SCHEDULER in 
		lsf)
		    ;;
		caf)
		    ;;
		*)
		    echo "[ERROR] Scheduler ${SCHEDULER} not defined: use only lsf or caf" >> /dev/stderr
		    exit 1
		    ;;
	    esac
	    shift;;
	--singleEle) SINGLEELE=y;;
	--createOnly) echo "[OPTION] createOnly"; unset SUBMIT; EXTRAOPTION="--createOnly";;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE; EXTRAOPTION="--submitOnly";;
	--check)      echo "[OPTION] checking jobs"; CHECK=y; EXTRAOPTION="--check";;
	--alcarerecoOnly) echo "[OPTION] alcarerecoOnly"; unset NTUPLE; EXTRAEXTRAOPTION="--alcarerecoOnly";;
	#--ntupleOnly) echo "[OPTION] ntupleOnly"; unset RERECO; EXTRAEXTRAOPTION="--ntupleOnly";;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;
	--doExtraCalibTree) DOEXTRACALIBTREE="${DOEXTRACALIBTREE} --doExtraCalibTree";;
	--doEleIDTree)      DOEXTRACALIBTREE="${DOEXTRACALIBTREE} --doEleIDTree";;
	--noStandardTree)   DOEXTRACALIBTREE="${DOEXTRACALIBTREE} --noStandardTree";;

    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

#------------------------------ checking

if [ ! -r "$TAGFILE" ];then
    echo "[ERROR] TAGFILE not found or not readable" >> /dev/stderr
    exit 1
fi

# check on readability of json
# check json name

#UI_WORKING_DIR=prod_alcarereco


periods=`grep -v '#' $fileList | sed -r 's|[ \t]+|\t|g' | cut -f 7 | sed 's|,|\n|g' | sort | uniq`
for period in $periods
  do
  if [ "$PERIOD" == "$period" ];then
      break;
  fi
  unset period
done
if [ -z "$PERIOD" ];then
    echo "[ERROR] PERIOD $PERIOD not defined"
    exit 1
fi



if [ -n "${TUTORIAL}" ];then
    case ${PERIOD} in
	RUN2012A)
	    ;;
	*)
	    echo "[ERROR] With the tutorial mode, the only permitted period is:"
	    echo "        RUN2012A"
	    #echo "        Be sure to have it in alcaraw_datasets.dat and to have selected it using the parseDatasetFile.sh"
	    exit 1
	    ;;
    esac
    if [ -n "${SINGLEELE}" ];then
	echo "[WARNING] No single electron rereco in tutorial mode" >> /dev/stderr
	unset SINGLEELE
    fi
fi

datasets=`./scripts/parseDatasetFile.sh $fileList | grep VALID | sed 's|$|,|' | grep "${PERIOD},"`
# set IFS to newline in order to divide using new line the datasets
IFS=$'\n'




for dataset in $datasets
  do
  if [ -z "${SINGLEELE}" -a "`echo $dataset | grep -c SingleElectron`" != "0" ];then continue; fi
  
  #  RUNRANGE=`echo $dataset | cut -d ' ' -f 2`
  #  DATASETNAME=`echo $dataset | cut -d ' ' -f 6`
  
  #--ui_working_dir ${UI_WORKING_DIR} \
  echo "============================================================"
  ./scripts/prodAlcarereco.sh -t ${TAGFILE} \
      --scheduler $SCHEDULER  --json=${JSONFILE} --json_name=${JSONNAME} \
      ${DOEXTRACALIBTREE} ${EXTRAOPTION} ${EXTRAEXTRAOPTION} ${TUTORIAL} $dataset 

done


exit 0

