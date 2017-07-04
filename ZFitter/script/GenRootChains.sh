#!/bin/bash

configFile=data/validation/monitoring_2015.dat
regionsFile=data/regions/validation.dat

usage(){
    echo "`basename $0` [options]" 
    echo "----- optional paramters"
    echo " -f arg (=${configFile})"
    echo " --noPU"
    echo " --outDir arg "
    echo " --addBranch arg" 
    echo " --regionsFile arg (=${regionsFile})"
    echo " --corrEleType arg"
    echo " --corrEleFile arg"
    echo " --fitterOptions arg"
}

desc(){
    echo "#####################################################################"
    echo "## This creates root files in tmp/[configFile] to simplify plotting"
    echo "## " echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -o hf: -l help,addBranch:,regionsFile:,outDir:,corrEleType:,corrEleFile:,fitterOptions: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


eval set -- "$options"

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) desc;usage; exit 0;;
        -f) configFiles=($configFiles $2); shift;;
	--noPU) noPU="--noPU";;
	--addBranch) addBranchList="${addBranchList} --addBranch=$2"; shift;;
	--regionsFile) regionsFile=$2; shift;;
	--corrEleType) corrEleType="--corrEleType=$2"; shift;;
	--corrEleFile) corrEleFile="--corrEleFile=$2"; shift;;
	--outDir) outdir=$2; shift;;
	--fitterOptions) fitterOptions="$2"; shift;;
    esac
    shift
done

let nFiles=${#configFiles[@]}-1

for i in `seq 0 $nFiles`
do
	configFile=${configFiles[i]}

echo "Create Chains for ${configFile}"

# saving the root files with the chains
rm tmp/*_chain.root

if [ "$outdir" == "" ]
then
	outdir=tmp/`basename $configFile .dat`/
fi
mkdir -p $outdir
rm $outdir/*_chain.root
echo storing in $outdir

echo ./bin/ZFitter.exe --saveRootMacro -f ${configFile} --regionsFile=${regionsFile} ${noPU} ${addBranchList} ${corrEleFile} ${corrEleType} ${fitterOptions} || exit 1
./bin/ZFitter.exe --saveRootMacro -f ${configFile} --regionsFile=${regionsFile} ${noPU} ${addBranchList} ${corrEleFile} ${corrEleType} ${fitterOptions} || exit 1

# adding all the chains in one file
for file in tmp/s[0-9]*_selected_chain.root tmp/d_selected_chain.root tmp/s_selected_chain.root 
do
  echo "hadding $file"
  name=`basename $file .root | sed 's|_.*||'`
  echo $name
  hadd $outdir/${name}_chain.root tmp/${name}_*_chain.root
  filelist="$filelist $outdir/${name}_chain.root"
  python python/reassociate.py $outdir/${name}_chain.root
done

done

echo "Now you can run:"
echo "python macro/standardDataMC.py invMass invMass_ECAL_ele -d $outdir/d_chain.root -s $outdir/s_chain.root" 

exit 0
