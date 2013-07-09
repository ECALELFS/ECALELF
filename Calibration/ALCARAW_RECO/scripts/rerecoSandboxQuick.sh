#!/bin/bash
################
# Semplified script for rereco of 2011 and 2012 data
##############

usage(){
    echo "`basename $0` -p <RUN2011AB, RUN2012AB, RUN2012C> -t tagFile"
    echo "    -t | --tag_file tagFile"
    echo "    -p | --period <RUN2011AB | RUN2012AB | RUN2012ABC | Cal_Nov2012>"
    echo "    --scheduler caf,lsf"
    echo "    --singleEle"
    echo "    --submit"
}


SCHEDULER=caf

#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:p: -l help,tag_file:,period:,scheduler:,singleEle,submit -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-t | --tag) TAGFILE=$2; echo "[OPTION] TAGFILE:$TAGFILE"; shift;;
	-p | --period) PERIOD=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	--singleEle) SINGLEELE=y;;
	--submit) SUBMIT=y;;
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
TAG=`basename ${TAGFILE} .py`

case $PERIOD in
    RUN2011AB)
	datasets=`parseProdSandboxOptions.sh sandbox_datasets.dat | grep VALID | grep -e "$PERIOD$"`
	;;
    RUN2012AB)
	datasets=`parseProdSandboxOptions.sh sandbox_datasets.dat | grep VALID | grep -e "$PERIOD$"`
	;;
    RUN2012ABC)
	datasets=`parseProdSandboxOptions.sh sandbox_datasets.dat | grep VALID | grep -e "$PERIOD$"`
	;;
    Cal_Nov2012)
	datasets=`parseProdSandboxOptions.sh sandbox_datasets.dat | grep VALID | grep -e "$PERIOD$"`
	;;
	*)
	echo "[ERROR] PERIOD $PERIOD not defined"
	exit 1
	;;
esac


# set IFS to newline in order to divide using new line the datasets
IFS=$'\n'


for dataset in $datasets
  do
  if [ -z "${SINGLEELE}" -a "`echo $dataset | grep -c SingleElectron`" != "0" ];then continue; fi


#STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${SANDBOX_REMOTE_DIR_BASE}\t${TAG}"
RUNRANGE=`echo $dataset | cut -d ' ' -f 2`
DATASETNAME=`echo $dataset | cut -d ' ' -f 6`
if [ "`cat sandboxRereco.log | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" == "0" ];then
    #echo $dataset
    ./scripts/rerecoSandbox.sh --scheduler $SCHEDULER -t $TAGFILE $dataset  || exit 1
else
    echo -n "$RUNRANGE ${DATASETNAME} $TAG already rerecoed: "
    cat sandboxRereco.log | grep ${TAG}  | grep ${DATASETNAME} | grep -c $RUNRANGE
    exit 0
fi
#  /bin/sleep 1m
done


if [ -n "${SUBMIT}" ];then 

    for dir in `ls -d -1 rereco_sandbox/$TAG/*/*`
      do
      echo $dir
      crab -c $dir -submit || exit 1
#      /bin/sleep 5m
    done

echo "jobs submitted"

    /bin/sleep 50m
    
    # after 30min check the job status
    
    ./scripts/checkAndResubmitRereco.sh -t $TAGFILE
    while [ "`cat tmp/$TAG.log | wc -l `" != "0" ]; do
	/bin/sleep 10m
	./scripts/checkAndResubmitRereco.sh -t $TAGFILE
    done
fi

