#!/bin/bash


pubDir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALELF/Z_monitoring_validation/PromptMonitoring

usage(){
    echo "`basename $0`"
}

desc(){
    echo "############################################################"

}



# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf: -l help,outDirTable:,outDirImgData:,outDirImgMC:,pubDir: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


set -- $options

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) desc;usage; exit 0;;
	--outDirTable) outDirTable=$2;shift;;
	--outDirImgData) outDirImgData=$2;shift;;
	--outDirImgMC) outDirImgMC=$2;shift;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done




###### check public directory for publication
if [ -z "${pubDir}" ];then
    echo "[ERROR] pubDir not specified" >> /dev/stderr
    exit 1
fi

if [ ! -d "${pubDir}" ];then
    echo "[ERROR] pubDir $pubDir not found or not a directory" >> /dev/stderr
    exit 1
fi


####### create the folders
if [ ! -e "${pubDir}/backup" ];then mkdir -p ${pubDir}/backup; fi
if [ ! -e "${pubDir}/table" ];then mkdir -p ${pubDir}/table; fi
if [ ! -e "${pubDir}/img" ];then mkdir -p ${pubDir}/img; fi
if [ ! -e "${pubDir}/img/MC" ];then mkdir -p ${pubDir}/img/MC; fi
if [ ! -e "${pubDir}/img/Data" ];then mkdir -p ${pubDir}/img/Data; fi
if [ ! -e "${pubDir}/img/fit" ];then mkdir -p ${pubDir}/img/fit; fi
if [ ! -e "${pubDir}/img/stability" ];then mkdir -p ${pubDir}/img/stability; fi

# make a backup 
tar -cjvf ${pubDir}/../`basename ${pubDir}`-`date +%d_%m_%y`.tar.bz2 ${pubDir}/backup || exit 1

rsyncOption=" --backup-dir=${pubDir}/backup --suffix=-bak -b" 
rsync -ahuv $rsyncOption ${outDirTable}/monitoring_summary-*.tex ${pubDir}/table
for file in ${pubDir}/table/*.tex
  do
  ./script/tex2txt.sh --format ${file} > ${pubDir}/table/`basename $file .tex`.dat
done
rsync -ahuv $rsyncOption ${outDirImgMC}/*.eps ${pubDir}/img/fit/MC
rsync -ahuv $rsyncOption ${outDirImgData}/*.eps ${pubDir}/img/fit/Data
rsync -ahuv $rsyncOption ${outDirImgData}/stability ${pubDir}/img/stability
