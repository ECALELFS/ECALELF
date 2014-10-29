#!/bin/bash


#tag_name=""
commonCut=Et_25-trigger-noPF
selection=loose
invMass_var=invMass_SC_regrCorr_ele
configFile=data/validation/monitoring_2012_53X.dat
regionsFile=data/regions/scaleStep2smearing_9.dat

runRangesFile=data/runRanges/monitoring.dat
baseDir=test
updateOnly="--updateOnly"
outDirImg="tmp"
# VALIDATION=y
# STABILITY=y
# SLIDES=y

usage(){
    echo "`basename $0` [options]" 
    echo "----- optional paramters"
    echo " -f arg (=${configFile})"
    echo " --noPU"
    echo " --outDirImg arg (=${outDirImg})"
    echo " --addBranch arg" 
    echo " --regionsFile arg (=${regionsFile})"
    echo " --corrEleType arg"
    echo " --corrEleFile arg"
    echo " --fitterOptions arg"
#    echo " --puName arg             "
#    echo " --runRangesFile arg (=${runRangesFile})  run ranges for stability plots"
#    echo " --selection arg (=${selection})     "
#    echo " --invMass_var arg (=${invMass_var})"
#    echo " --validation        "
#    echo " --stability         "
#    echo " --etaScale          "
#    echo " --systematics       "
#    echo " --slides            "
#    echo " --baseDir arg (=${baseDir})  base directory for outputs"
#    echo " --rereco arg  name of the tag used fro the rereco"
#    echo " --test"
#    echo " --commonCut"
#    echo " --period RUN2012A, RUN2012AB, RUN2012C, RUN2012D"
}

desc(){
    echo "#####################################################################"
    echo "## This script make the usual validation table and stability plots"
    echo "## "
    echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf: -l help,runRangesFile:,selection:,invMass_var:,puName:,baseDir:,rereco:,validation,stability,etaScale,systematics,slides,onlyTable,test,commonCut:,period:,noPU,outDirImg:,addBranch:,regionsFile:,corrEleType:,corrEleFile:,fitterOptions: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


set -- $options

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) desc;usage; exit 0;;
        -f) configFile=$2; shift;;
	--noPU) noPU="--noPU";;
	--addBranch) addBranchList="${addBranchList} --addBranch=$2"; shift;;
	--regionsFile) regionsFile=$2; shift;;
	--corrEleType) corrEleType="--corrEleType=$2"; shift;;
	--corrEleFile) corrEleFile="--corrEleFile=$2"; shift;;
	--fitterOptions) fitterOptions="$fitterOptions $2"; shift;;
        --invMass_var) invMass_var=$2; echo "[OPTION] invMass_var = ${invMass_var}"; shift;;
	--outDirImg) outDirImg=$2; shift;;
	--puName) puName=$2; shift;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--rereco) rereco=$2; echo "[OPTION] rereco = $rereco"; shift;;
	--validation)  VALIDATION=y;;
	--stability)   STABILITY=y;;
	--etaScale)    ETA=y;;
	--systematics) SYSTEMATICS=y;;
	--slides)      SLIDES=y;;
	--onlyTable)   ONLYTABLE=y;;
	--test)        TEST=y;;
	--selection)   selection=$2; echo "[OPTION] Selection=${selection}"; shift;;
	--commonCut)   commonCut=$2; echo "[OPTION] commonCut=${commonCut}"; shift;;
	--period) PERIOD=$2; shift;
	    case $PERIOD in
		RUN2012A)
		    ;;
		RUN2012B)
		    ;;
		RUN2012AB)
		    ;;
		RUN2012C)
		    ;;
		RUN2012D)
		    ;;
		*)
		    echo "[ERROR] period not defined. Only RUN2012A, RUN2012B, RUN2012AB, RUN2012C" >> /dev/stderr
		    exit 1
		    ;;
	    esac
	    period="--period=$PERIOD"
	    ;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

if [ -z "${VALIDATION}" -a -z "${STABILITY}" -a -z "${SLIDES}" -a -z "${SYSTEMATICS}" -a -z "${ETA}" ];then
    # if no option selected, run all the sequence
    VALIDATION=y
    STABILITY=y
    ETA=y
    SLIDES=y
fi

if [ ! -d "tmp" ];then mkdir tmp; fi
if [ ! -d "${outDirImg}" ];then 
    mkdir ${outDirImg} -p; 
    mkdir ${outDirImg}/../img -p
fi

# file with ntuples
if [ -z "${configFile}" -a -z "${TEST}" ];then
    echo "[ERROR] configFile not specified" >> /dev/stderr
    exit 1
elif  [ -z "${configFile}" -a -n "${TEST}" ];then
    configFile=data/validation/test.dat
fi

# saving the root files with the chains
rm tmp/*_chain.root
./bin/ZFitter.exe --saveRootMacro -f ${configFile} --regionsFile=${regionsFile} ${noPU} ${addBranchList} ${corrEleFile} ${corrEleType} ${fitterOptions} || exit 1

# adding all the chains in one file
for file in tmp/s[0-9]*_selected_chain.root tmp/d_selected_chain.root tmp/s_selected_chain.root 
  do
  name=`basename $file .root | sed 's|_.*||'`
  echo $name
  hadd tmp/${name}_chain.root tmp/${name}_*_chain.root
  filelist="$filelist tmp/${name}_chain.root"
done
#hadd tmp/s_chain.root tmp/s_*_chain.root
#hadd tmp/d_chain.root tmp/d_*_chain.root

#NOTA => previously, 3 times signalA
cat > tmp/load.C <<EOF
{
  gROOT->ProcessLine(".L macro/PlotDataMC.C+");
  gROOT->ProcessLine(".L src/setTDRStyle.C");
  gROOT->ProcessLine(".L macro/addChainWithFriends.C+");

  setTDRStyle();


  TChain *data   = (TChain *) _file0->Get("selected");
  TChain *signalA = (TChain *) _file1->Get("selected");
  TChain *signalB = (TChain *) _file2->Get("selected");
  //TChain *signalC = (TChain *) _file3->Get("selected");

  ReassociateFriends(_file0, data);
  ReassociateFriends(_file1, signalA);
  ReassociateFriends(_file2, signalB);
//  ReassociateFriends(_file3, signalC);

   TDirectory *curDir = new TDirectory("curDir","");
   curDir->cd();

  TString outputPath="${outDirImg}/";

// look at standardDataMC.C for some examples  
}

EOF

echo "Now you can run:"
echo "root -l $filelist tmp/load.C tmp/standardDataMC.C" 
#echo "root -l tmp/$filelist tmp/load.C tmp/standardDataMC.C" 
echo "change the outputPath string in load.C to have the plots in the correct directory"

