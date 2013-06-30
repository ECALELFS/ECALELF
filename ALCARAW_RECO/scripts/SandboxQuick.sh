#!/bin/bash
################
# Semplified script for rereco and ntuple of 2011 and 2012 data
##############
NTUPLE_REMOTE_DIR=caf/user/shervin/calibration/8TeV/ZNtuples
SCHEDULER=caf
CREATE=y
SUBMIT=y
usage(){
    echo "`basename $0` -p period -t tagFile"
    echo "    -t | --tag_file tagFile"
    echo "    --scheduler caf,lsf (=$SCHEDULER)"
    echo "--------------- FOR RERECO only"
    echo "    -p | --period <test | RUN2011AB | RUN2012AB | RUN2012ABC | Cal_Nov2012 | Cal_Dic2012>"
    echo "    --rereco"
    echo "    --singleEle: if you want to rereco also single electron datasets"
    echo "--------------- FOR NTUPLE only"
    echo "    --ntuple"
    echo "    --json_name jsonName"
    echo "    --json jsonFile.root"
    echo "    --doExtraCalibTree"
    echo "--------------- Expert"
    echo "    --submitOnly"
    echo "    --createOnly"
    echo "--------------- ECALELF Tutorial"
    echo "    --tutorial"
}


#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:p: -l help,tag_file:,period:,scheduler:,singleEle,createOnly,submitOnly,ntuple,rereco,json_name:,json:,tutorial,doExtraCalibTree -- "$@")
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
	    TUTORIAL=y; SCHEDULER=caf; 
	    NTUPLE_REMOTE_DIR=group/alca_ecalcalib/sandbox/tutorial/${USER}/calibration/8TeV/ZNtuples;
	    echo "Ntuple output directory: /eos/cms/store/${NTUPLE_REMOTE_DIR}"
	    echo -n "Check if exists: "
	    eos.select ls -s /eos/cms/store/${NTUPLE_REMOTE_DIR} &> /dev/null || {
		echo "Creating it"
		eos.select mkdir -p /eos/cms/store/${NTUPLE_REMOTE_DIR}
		echo -n "Re-checking it "
		eos.select ls -s /eos/cms/store/${NTUPLE_REMOTE_DIR} || exit 1
	    }
	    echo "[OK]"
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
	--createOnly) unset SUBMIT;;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE;;
	--rereco) RERECO=y;;
	--ntuple) NTUPLE=y;;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;
	--doExtraCalibTree) DOEXTRACALIBTREE="--doExtraCalibTree";;
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

if [ -n "${NTUPLE}" ];then
    
    unset ${SINGLEELE} 
    NTIME=8m
    TYPE=alcarereco
    if [ -z "$JSONFILE" -a "$TYPE" != "ALCARECOSIM" ];then 
	echo "[ERROR] JSONFILE not defined" >> /dev/stderr
	usage >> /dev/stderr
	exit 1
    fi
    
    if [ -z "$JSONNAME" -a "$TYPE" != "ALCARECOSIM" ];then 
	echo "[ERROR] JSONNAME not defined" >> /dev/stderr
	usage >> /dev/stderr
	exit 1
    fi

    fileList=../SANDBOX/sandboxRereco.log
    datasets=`parseProdSandboxOptions.sh ${fileList} | grep -e "$TAG$"`
    if [ -z "${datasets}" ];then
	echo "[ERROR] No rereco found in sandboxRereco.log" >> /dev/stderr
	exit 1
    fi
    localFolder=prod_zntuples/${TYPE}
else
    NTIME=50m
    fileList=sandbox_datasets.dat
    localFolder=rereco_sandbox
    if [ -z "${RERECO}" ];then
	echo "[ERROR] Nor --ntuple neither --rereco options have been provided" >> /dev/stderr
	usage >> /dev/stderr
	exit 1
    fi
    case $PERIOD in
	"test")
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | grep -e "$PERIOD,"`
	    ;;
	RUN2011AB)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' | grep -e "$PERIOD,"`
	    ;;
	RUN2011B53X)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' | grep -e "$PERIOD,"`
	    ;;
	RUN2012AB)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' |  grep -e "$PERIOD,"`
	    ;;
	RUN2012C)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID |  sed 's|$|,|' | grep -e "$PERIOD,"`
	    ;;
	RUN2012D)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID |  sed 's|$|,|' | grep -e "$PERIOD,"`
	    ;;
	RUN2012ABC)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' |  grep -e "$PERIOD,"`
	    ;;
	RUN2012ABCD)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' |  grep -e "$PERIOD,"`
	    ;;
	Cal_Nov2012)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' |  grep -e "$PERIOD,"`
	    ;;
	Cal_Dic2012)
	    datasets=`parseProdSandboxOptions.sh ${fileList} | grep VALID | sed 's|$|,|' |  grep -e "$PERIOD,"`
	    ;;
	*)
	    echo "[ERROR] PERIOD $PERIOD not defined"
	    exit 1
	    ;;
    esac
fi

# set IFS to newline in order to divide using new line the datasets
IFS=$'\n'

if [ -n "${CREATE}" ];then
    nCreated=0 # this has to be within the CREATE part, because says if a creation has been requested, but then there are no submission to do

    # this is to protect multiple istances overwriting the same files
    while [ -e "tmp/sandboxQuick.lock" ]
      do
      /bin/sleep 30s
    done
    touch tmp/sandboxQuick.lock
    for dataset in $datasets
      do
      if [ -z "${SINGLEELE}" -a "`echo $dataset | grep -c SingleElectron`" != "0" ];then continue; fi

      RUNRANGE=`echo $dataset | cut -d ' ' -f 2`
      DATASETNAME=`echo $dataset | cut -d ' ' -f 6`

      if [ -n "${NTUPLE}" ];then
	  if [ ! -e "prod_zntuples/alcarereco/$TAG/$DATASETNAME/$RUNRANGE/$JSONNAME" ];then
	      echo "[STATUS] Creating task for ntuple production for dataset: ${DATASETNAME}"
	      let nCreated=${nCreated}+1
	      ./script/prodNtuples.sh --ntuple_remote_dir ${NTUPLE_REMOTE_DIR} --ui_working_dir prod_zntuples --scheduler $SCHEDULER --type ${TYPE} --json ${JSONFILE} --json_name ${JSONNAME} ${DOEXTRACALIBTREE} $dataset  || exit 1
	  else
	      echo -n "$RUNRANGE ${DATASETNAME} $TAG ntuples already produced"
	  fi
      fi

      if [ -n "${RERECO}" ];then
	  if [ "`cat sandboxRereco.log | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" == "0" ];then
	      echo "[STATUS] Creating task for rerecoing dataset: ${DATASETNAME}"
	      let nCreated=${nCreated}+1
	      ./scripts/rerecoSandbox.sh --scheduler $SCHEDULER -t $TAGFILE $dataset  || exit 1
	  else
	      echo -n "$RUNRANGE ${DATASETNAME} $TAG already rerecoed: "
	      cat sandboxRereco.log | grep ${TAG}  | grep ${DATASETNAME} | grep -c $RUNRANGE
	      continue
	  fi
      fi
    done
    rm tmp/sandboxQuick.lock
fi

if [ "${nCreated}" == "0" ];then 
    # no new jobs created, but creation has been requested
    # so no submission required
    unset SUBMIT
fi

if [ -n "${SUBMIT}" ];then 
#    TAG=`basename ${TAGFILE} .py`
    if [ -n "${NTUPLE}" ];then
	dirList=`ls -d -1 ${localFolder}/$TAG/*/*/*`
    else
	dirList=`ls -d -1 ${localFolder}/$TAG/*/*/`
    fi
    for dir in $dirList
      do
      echo $dir
      if [ ! -e "$dir/res/finished" ];then
	  crab -c $dir -submit || exit 1
      fi
#      /bin/sleep 1m
    done
    
    
    echo
    echo "[STATUS] JOBS SUBMITTED, waiting ${NTIME} for check"
    /bin/sleep ${NTIME}

    if [ -n "${NTUPLE}" ];then
	checkAndResubmitRereco.sh -t $TAGFILE --ntuple
    else
	checkAndResubmitRereco.sh -t $TAGFILE
    fi
    while [ "`cat tmp/$TAG.log | wc -l `" != "0" ]; do
	/bin/sleep 5m
	if [ -n "${NTUPLE}" ];then
	    checkAndResubmitRereco.sh -t $TAGFILE --ntuple
	else
	    checkAndResubmitRereco.sh -t $TAGFILE
	fi
    done

    if [ -n "${NTUPLE}" ];then
	for dir in $dirList
	  do
	  ./script/mergeOutput.sh -u $dir
	done
    fi
fi

