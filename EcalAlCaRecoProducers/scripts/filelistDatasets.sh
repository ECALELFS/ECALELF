#!/bin/bash
source $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh

usage(){
    echo "`basename $0` parsed_option_line"
    echo "  parsed_option_line: parseProdSandboxOptions.sh sandbox_datasets.dat"
    echo "  parsed_option_line: parseProdSandboxOptions.sh alcareco_datasets.dat"
}
############
# Given a line from the following files, it creates the list of files:
# sandbox_datasets.dat  -> for alcareco (prompt datasets, same to produce the sandbox)
# sandboxRereco.log     -> for rereco   
# alcareco_datasets.dat -> for alcareco (official rerecoes, MCs, others)

#-r 198111-198913 -d /DoubleElectron/Run2012C-ZElectron-PromptSkim-v2/RAW-RECO -n DoubleElectron-ZSkim-RUN2012C-v2 --store caf --remote_dir group/alca_ecalcalib/sandbox




#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hd:n:s:r:t: -l help,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,scheduler:,tagFile:,sandbox_remote_dir:,check -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-d|--datasetpath) DATASETPATH=$2; shift ;;
	-n|--datasetname) DATASETNAME=$2; shift ;;
	-s|--skim) SKIM=$2 ; shift;;
	-r|--runrange) RUNRANGE=$2; shift;;
	--store) STORAGE_ELEMENT=$2; shift;;
	--remote_dir) USER_REMOTE_DIR_BASE=$2; shift;;
	--sandbox_remote_dir) SANDBOX_REMOTE_DIR_BASE=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	-t | --tag) TAGFILE=$2; shift;;
	--check) CHECK=y;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done


#------------------------------ checking
if [ -z "$DATASETPATH" ];then 
    echo "[ERROR] DATASETPATH not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$DATASETNAME" ];then 
    echo "[ERROR] DATASETNAME not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$RUNRANGE" ];then 
    echo "[ERROR] RUNRANGE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

# if [ -z "$STORAGE_ELEMENT" ];then 
#     echo "[ERROR] STORAGE_ELEMENT not defined" >> /dev/stderr
#     usage >> /dev/stderr
#     exit 1
# fi


if [ -z "$USER_REMOTE_DIR_BASE" ];then 
    echo "[ERROR] USER_REMOTE_DIR_BASE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

setEnergy $DATASETPATH


if [ -n "$TAGFILE" ]; then
    TAG=`basename $TAGFILE .py`
fi
USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${TAG}/${DATASETNAME}/${RUNRANGE:-allRange}

sample=$DATASETNAME-$RUNRANGE


echo $sample
echo /eos/cms/store/$USER_REMOTE_DIR_BASE


if [ -n "$CHECK" ];then
    if [ -n "$TAG" ];then
	makefilelist.sh -f filelist/check $sample /eos/cms/store/$USER_REMOTE_DIR || exit 1
	if [ "`cat filelist/check/$sample.list | wc -l` != 0" ];then
	    rm filelist/check/$sample.list
	else exit 2
	fi
    else
	if eos.select ls /eos/cms/store/${USER_REMOTE_DIR} ; then
	    makefilelist.sh -f filelist/check $sample /eos/cms/store/$USER_REMOTE_DIR || exit 1
	    if [ "`cat filelist/check/$sample.list | wc -l` != 0" ];then
		rm filelist/check/$sample.list
	else exit 2
	    fi
	else
	    makefilelist.sh -f filelist/check $sample /eos/cms/store/${USER_REMOTE_DIR_BASE} || exit 1
	    if [ "`cat filelist/check/$sample.list | wc -l` != 0" ];then
		rm filelist/check/$sample.list
	else exit 2
	    fi
	    
	fi
    fi
    
else

    if [ -n "$TAG" ];then
	makefilelist.sh -f filelist/$TAG $sample /eos/cms/store/$USER_REMOTE_DIR || exit 1
    else
	if eos.select ls /eos/cms/store/${USER_REMOTE_DIR} ; then
	    makefilelist.sh $sample /eos/cms/store/$USER_REMOTE_DIR || exit 1
	else
	    makefilelist.sh $sample /eos/cms/store/${USER_REMOTE_DIR_BASE} || exit 1
	fi
    fi

fi


# remove PUDumper files!
if [ -n "$TAG" ];then
    sed -i '/PUDumper/ d' filelist/*/*.list
else
    sed -i '/PUDumper/ d' filelist/*.list
fi
