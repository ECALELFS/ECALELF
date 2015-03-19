#!/bin/bash

checkNJobs(){
    if [ "`cat filelist/$sample1.list | wc -l`" != "$nJobs" ];then
	echo "[ERROR] File missing for sample: $sample" > /dev/stderr
	let nJobs=nJobs-1
	missingList=""
	for counter in `seq -f %03g 0 $nJobs`;
	  do 
	  numRep=`grep -c "$counter\." filelist/$sample1.list`; 
	  if [ "$numRep" == "0" ];then 
	      missingList="$missingList $counter"; 
	  elif [ "$numRep" != "1" ]; then
	      echo "repeated $counter"; 
	  fi 
	done
	echo
	echo $missingList
	for jobN in $missingList; do grep $jobN batch/$sample'_joblist'; done > batch/missing-$tag-$sample'_joblist'
#	exit 1
    fi
}


tagList(){ for tag in config/reRecoTags/*.py; do basename $tag .py; done; }

checkTag(){
    # controllo che il tag esista prima di lanciare i job!
    tar -tvf batch/SANDBOX.tar config/reRecoTags/$tag.py || {
	echo "[ERROR] Tag not defined in tar file" > /dev/stderr
	echo "        If you have not created the tag please do it" > /dev/stderr
	echo "        If you have created it, do ./script/updateTag.sh" > /dev/stderr
	exit 1
    }
}

usage(){
    echo "Usage: `basename $0` -t tagFileName -e <7TeV|8TeV>" 
    echo "  Tag list:" > /dev/stderr
    tagList > /dev/stderr

}


case $# in 
    0)
	echo -n "Error: " > /dev/stderr
	usage > /dev/stderr
	exit 1
	;;
    *)
	;;
esac

while getopts ht:e: option
  do
  case $option in
      h)
	  usage >> /dev/stderr
	  exit 0
	  ;;
      t)
	  echo "[OPTION] using tag file: config/reRecoTags/$OPTARG.py"
	  tag=$OPTARG
	  ;;
      e)
	  echo "[OPTION] Checking for energy: $OPTARG" 
	  ENERGY=$OPTARG
	  case $ENERGY in 
	      7TeV)
		  ;;
	      8TeV)
		  ;;
	      *)
		  echo "[ERROR]:: Energy $ENERGY not defined" >> /dev/stderr
		  exit 1
		  ;;
	  esac
	  ;;
      *)
	  echo -n "ERROR --> "
	  usage > /dev/stderr
	  exit 1
          ;;
  esac
done

#while getopts "h" OPTIONS ; do
#    case ${OPTIONS} in
#        h|-help) echo "${usage}";;
#    esac
#done

shift $(($OPTIND-1))

if [ -z "$tag" ];then
    echo "[ERROR] Config file with reReco tags not defined" > /dev/stderr
    usage > /dev/stderr
    exit 1
fi

if [ -z "$ENERGY" ];then
    echo "[ERROR] ENERGY not defined" > /dev/stderr
    usage > /dev/stderr
    exit 1
fi

checkTag

case $ENERGY in 
    7TeV)
	sampleList="DoubleElectron-RUN2011A-10May DoubleElectron-RUN2011A-v4 DoubleElectron-RUN2011A-v5 DoubleElectron-RUN2011A-v6 DoubleElectron-RUN2011B-v1"
#	sampleList="$sampleList SingleElectron-RUN2011A-10May SingleElectron-RUN2011A-v4 SingleElectron-RUN2011A-v5 SingleElectron-RUN2011A-v6 SingleElectron-RUN2011B-v1"
	;;
    8TeV)
	sampleList="DoubleElectron-RUN2012A-v1"
#	sampleList="$sampleList SingleElectron-RUN2012A-v1"
	#ENERGY=7TeV
	;;
    *)
	exit 1
	;;
esac
for sample in $sampleList
  do

  sample1=check-$tag-$sample
  makefilelist.sh $sample1 /eos/cms/store/group/alca_ecalcalib/sandboxRecalib/$ENERGY/fromUncalib/$tag/$sample
  if [ ! -e "filelist/$sample1.list" ];then
      echo "[WARNING] $sample for tag $tag missing"
      continue
  fi

case $sample1 in
    *DoubleElectron-RUN2012A-v1*)
	nJobs=71
	;;
    *SingleElectron-RUN2012A-v1*)
	nJobs=204
	;;
    *DoubleElectron-RUN2011A-10May*)
	nJobs=20
	;;
    *DoubleElectron-RUN2011A-v4*)
	nJobs=27
	;;
    *DoubleElectron-RUN2011A-v5*)
	nJobs=12
	;;
    *DoubleElectron-RUN2011A-v6*)
	nJobs=13
	;;
    *DoubleElectron-RUN2011B-v1*)
	nJobs=63
	;;
    *SingleElectron-RUN2011A-10May*)
	nJobs=116
	;;
    *SingleElectron-RUN2011A-v4*)
	nJobs=226
	;;
    *SingleElectron-RUN2011A-v5*)
	nJobs=99
	;;
    *SingleElectron-RUN2011A-v6*)
	nJobs=103
	;;
    *SingleElectron-RUN2011B-v1*)
	nJobs=274
	;;

esac
checkNJobs
done    
exit 0



