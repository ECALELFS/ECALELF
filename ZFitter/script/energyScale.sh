#!/bin/bash
source script/functions.sh
source script/bash_functions_calibration.sh
#shopt -s expand_aliases
#source ~/.bashrc

# energy scale derived in different steps:
# - time dependence (step1)
# - material dependence  (step2)
# - smearings (step4)
# - quick likelihood scan in Et bins (profile only) (step5)
# - first Et minimization (step6)
# - second Et minimization (step7)
# - Et minization (closure test) (step8)
# - gain study (step9)

index= #useless?
eos_path=/eos/project/c/cms-ecal-calibration
tmp_path=.
tmp_path_plots=${tmp_path}/plots
baseDir=${tmp_path}/test
eosDir=${eos_path}/test
updateOnly="--updateOnly --fit_type_value=1" # --profileOnly --initFile=init.txt"
commonCut=EtLeading_32-EtSubLeading_20-noPF-isEle #Hgg trigger emulation (2 GeV above the trigger)
#commonCut=Et_20-noPF #Standard common Cuts for Z calibration
#commonCut=Et_30-noPF #Et_30 for 0 T calibration
#default selection is loose25nsRun22016Moriond #you can change this via steps_maker.sh
selection=loose25nsRun22016Moriond #cutBasedElectronID-Spring15-25ns-V1-standalone-loose
invMass_var=invMass_ECAL_ele
#invMass_var=invMass_SC_pho_regrCorr #you can change this via script (steps_maker.sh)
Et_smear=
smearRange="--smearMassMin 80 --smearMassMax 100"
###########################################################
regionFileStep1=data/regions/scaleStep1.dat

regionFileStep2EB=data/regions/scaleStep2smearing_1.dat
regionFileStep2EE=data/regions/scaleStep2smearing_2.dat
regionFileStep4EBEE=data/regions/scaleStep4smearing_0.dat
regionFileStep4EBEE_Hgg=data/regions/scaleStep0_Hgg.dat
regionFileStep4EBEE_r9=data/regions/scaleStep0_r9.dat
regionFileStep4Inclusive=data/regions/scaleStep4_Inclusive.dat
regionFileStep4EB=data/regions/scaleStep4smearing_1.dat
regionFileStep4EE=data/regions/scaleStep4smearing_2.dat
regionFileStep4EB_93=data/regions/scaleStep4smearing_1_R9_93.dat
regionFileStep4EE_93=data/regions/scaleStep4smearing_2_R9_93.dat
regionFileStep4EB_95=data/regions/scaleStep4smearing_1_R9_95.dat
regionFileStep4EE_95=data/regions/scaleStep4smearing_2_R9_95.dat

regionFileStep5EB=data/regions/scaleStep2smearing_6.dat
regionFileStep5EE=data/regions/scaleStep2smearing_7.dat
#echo "Test of finer Et Bins"
regionFileStep5EB=data/regions/scaleStep5_FineET1.dat
regionFileStep5EE=data/regions/scaleStep5_FineET2.dat

regionFileStep5EB=data/regions/scaleStep5_JetMetCheck2.dat
regionFileStep5EE=data/regions/scaleStep5_FineET2.dat

regionFileStep6EB=data/regions/scaleStep6smearing_1.dat
regionFileStep6EE=data/regions/scaleStep6smearing_2.dat

#############################################################
usage(){
    echo "`basename $0` [options]" 
    echo " -f arg            config file"
#    echo " --regionFile arg             "
    echo " --runRangesFile arg           "
    echo " --selection arg (=$selection)"
    echo " --invMass_var arg"
    echo " --scenario arg"
    echo " --commonCut arg (=$commonCut)"
    echo " --smearRange low,hi (=(80,100))"
    echo " --step arg: 1, 2, 2fit, 3, 3weight, slide, 3stability, syst, 1-2,1-3,1-3stability and all ordered combination"
    echo " --index arg"
}
#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:s: -l help,regionFile:,runRangesFile:,selection:,invMass_var:,scenario:,step:,baseDir:,commonCut:,smearRange:,noJobs,index:,force,r9_transf -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-f) configFile=$2; shift;;
	-s|--step) STEP=$2; shift;;
	--force) FORCE=y;;
	--r9_transf) r9_transf=y;;
	--invMass_var) invMass_var=$2; echo "[OPTION] invMass_var = ${invMass_var}"; shift;;
	--scenario) scenario=$2; echo "[OPTION] scenario = ${scenario}"; shift;;
	--index) index=$2; shift;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;; 
	--selection) selection=$2; echo "[OPTION] selection = ${selection}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--commonCut) commonCut=$2; echo "[OPTION] commonCut = $commonCut"; shift;;
	--smearRange)
		smearRange=(${2//,/ })
		smearRange="--smearMassMin ${smearRange[0]} --smearMassMax ${smearRange[1]}";
		echo "[OPTION] smearRange = $smearRange"; shift;;
	--noJobs) noJobs=y;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done

##Using files with R9 reweight
if [ -n "${r9_transf}" ];then
    echo "[INFO] You are using the r9_transformed-files"
    regionFileStep2EB=data/regions/scaleStep2smearing_1_R9prime.dat
    regionFileStep2EE=data/regions/scaleStep2smearing_2_R9prime.dat
    regionFileStep4EB=data/regions/scaleStep4smearing_1_R9prime.dat
    regionFileStep4EE=data/regions/scaleStep4smearing_2_R9prime.dat
    #regionFileStep4EB=data/regions/scaleStep0.dat
    #regionFileStep4EE=data/regions/scaleStep0_bis.dat
    #regionFileStep4EB_93=data/regions/scaleStep4smearing_1_R9prime_93.dat
    #regionFileStep4EE_93=data/regions/scaleStep4smearing_2_R9prime_93.dat
    #regionFileStep4EB_95=data/regions/scaleStep4smearing_1_R9prime_95.dat
    #regionFileStep4EE_95=data/regions/scaleStep4smearing_2_R9prime_95.dat
fi


file=`basename ${configFile} .dat`

if [ ! -d "tmp" ];then mkdir tmp; fi

# file with ntuples
if [ -z "${configFile}" ];then
    echo "[ERROR] configFile not specified" >> /dev/stderr
    exit 1
fi

if [ -z "${runRangesFile}" ];then
    echo "[ERROR] runRangesFile not specified" >> /dev/stderr
    exit 1
fi

#case ${selection} in
#    WP80PU)
#        ;;
#    WP90PU)
#	;;
#    loose)
#	;;
#    medium)
#	;;
#    tight)
#	;;
#    diphotonIso25nsRun2Boff)
#	;;
#    noID) #To be used in rare cases ;-)
#	;;
#    *)
#	echo "[ERROR] Selection ${selection} not configured" >> /dev/stderr
#        exit 1
#        ;;
#esac



case ${STEP} in
    help) 
	echo "List of steps:"
	echo "   - 1: stability vs time (runNumber x eta)"
	echo "   - 2: eta x R9 (smearing method)"
#	echo "   - 2fit: eta x R9 (fit method)"
#	echo "   - 3: closure test of 2: eta x R9 (fit method)"
	echo "   - 4: closure test of 2: eta x R9 (smearing method), for final histograms and profiles"
	echo "   - 5: plot and profile only in Et categories with smearings from step4"
	echo "   - 6: smearings from step4, scales in Et x eta x R9 categories"
	echo "   - 7: eta x R9 x Et, scale and smearings"
	echo "   - 8: eta x R9 x Et, scale and smearings applied: closure test"
	;;
    0)	STEP0=y;;
    1)	STEP1=y;;
    1stability) STEP1Stability=y;;
    1plotter) STEP1Plotter=y;;
    2) 	STEP2=y;;
    3) 	STEP3=y;; 
    4)  STEP4=y;;
    4bis)  STEP4bis=y; extension=amctnlo;;
    # for systematics
    4amctnlo) STEP4=y; extension=amctnlo;; 
    4madgraph) STEP4=y; extension=madgraph;; 
    4weight) STEP4=y; extension=weight;;
    4medium) STEP4=y; extension=medium;;
    4tight)  STEP4=y; extension=tight;;
    4Et_22)  STEP4=y; extension=Et_22;;
    4Et_25)  STEP4=y; extension=Et_25;;
    4R9_93)  STEP4=y; extension=r9syst_93;;
    4R9_95)  STEP4=y; extension=r9syst_95;;
    4pho)    STEP4=y; extension=invMass_SC_pho_regrCorr;;
    4ele)    STEP4=y; extension=invMass_SC_corr;;
    5*)  STEP5=y;;
    6*)  STEP6=y;;
    7*)  STEP7=y;;
    8*)  STEP8=y;;
    9)  STEP9=y;;
    9nPV)  STEP9=y; extension=nPV;;
    10)  STEP10=y;;
    12)  STEP12=y;;
#     madgraph) MCSAMPLE=y; extension=madgraph;; 
#     powheg) MCSAMPLE=y; extension=powheg;; 
#     sherpa) MCSAMPLE=y; extension=sherpa;; 
#    madgraph) PDFWEIGHT=y; extension=madgraph;; 
#    powheg)   PDFWEIGHT=y; extension=powheg;; 
#    sherpa)   PDFWEIGHT=y; extension=sherpa;; 
    pdfWeight) PDFWEIGHT=y;  extension=pdfWeight;;
    fsrWeight) PDFWEIGHT=y;  extension=fsrWeight;;
    weakWeight) PDFWEIGHT=y;  extension=weakWeight;;
    pdfWeightZPt) PDFWEIGHT=y;     extension=pdfWeightZPt;;
    gainSwitch) GAINSWITCH=y; extension=gainSwitch;echo "[OPTION] $extension";;
    gainSwitch2) GAINSWITCH=y; extension=gainSwitch2;echo "[OPTION] $extension";;
    gainSwitch3) GAINSWITCH=y; extension=gainSwitch3;echo "[OPTION] $extension";;
    gainSwitch4) GAINSWITCH=y; extension=gainSwitch4;echo "[OPTION] $extension";;
    gainSwitch5) GAINSWITCH=y; extension=gainSwitch5;echo "[OPTION] $extension";;
    gainSwitch6) GAINSWITCH=y; extension=gainSwitch6;echo "[OPTION] $extension";;
    gainSwitch7) GAINSWITCH=y; extension=gainSwitch7;echo "[OPTION] $extension";;
    gainSwitchEne) GAINSWITCH=y; extension=gainSwitchEne;echo "[OPTION] $extension";;
    gainSwitchSeedEne) GAINSWITCH=y; extension=gainSwitchSeedEne;echo "[OPTION] $extension";;
    1-2) STEP1=y; STEP2=y;;
    1-3) STEP1=y; STEP2=y; STEP3=y; SLIDE=y;;
    2-3) STEP2=y; STEP3=y; SLIDE=y;;
    1-3stability) STEP1=y; STEP2=y; STEP3=y; STEP3Stability=y; SLIDE=y;;
    2-3stability) STEP2=y; STEP3=y; STEP3Stability=y; SLIDE=y;;
    slide) SLIDE=y;;
    3stability) STEP3Stability=y;;
	syst) SYSTEMATICS=y;;
    3weight) STEP3WEIGHT=y; STEP3=y; extension=weight;;
    2fit) STEP2FIT=y;;
    1-2fit) STEP1=y; STEP2FIT=y;;
    all) STEP1stability=y; STEP1=y; STEP2FIT=y; STEP3=y; STEP3Stability=y; STEP4=y;; # SLIDE=y;;
    *)
	echo "[ERROR] Step ${STEP} not implemented" >> /dev/stderr
	exit 1
	;;
esac

#####################
outFileStep1=step1-${invMass_var}-${selection}-${commonCut}-HggRunEta_scales.dat
#outFileStep2=step2${extension}-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat
outFileStep2=step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9_scales.dat
outFileStep4=step4-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat
outFileStep7=step7-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9Et.dat
outFileStep8=step8-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9Et.dat
outFileStep9=step9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9Et.dat
outFileStep12=step12-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9Et.dat

# Make sure that the files indicated in outFileList are in the same order of stepNameList
outFileList=(
"${outFileStep1}"
"${outFileStep2}"
"${outFileStep4}"
"step8-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9Et.dat"
"step9gainSwitch-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9Et.dat"
"${outFileStep9}"
"${outFileStep12}"
)
stepNameList=(
"step1"
"step2"
"step4"
"step8"
"step9gainSwitch"
"step9"
"step12"
)

if [ -n "${STEP0}" ];then
    echo "This is step0, just to check the dir name"
    mcName ${configFile}
    mcName=$(echo $mcName | sed 's|.root||g')

    puName ${configFile}
    echo "PUName: $puName"
    echo "selection: $selection"
    echo "mcName: ${mcName}"

    if [ "${invMass_var}" == "invMass_regrCorr_egamma" ];then
	outDirMC=$baseDir/MCodd/${mcName}/${puName}/${selection}/${invMass_var}
	outDirMC_eos=$eosDir/MCodd/${mcName}/${puName}/${selection}/${invMass_var}
	isOdd="--isOddMC"
    else
	outDirMC=$baseDir/MC/${mcName}/${puName}/${selection}/${invMass_var}
	outDirMC_eos=$eosDir/MC/${mcName}/${puName}/${selection}/${invMass_var}
    fi
    echo "outDirMC is ${outDirMC}"
    outDirData=$baseDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
    outDirData_eos=$eosDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
    outDirTable=${outDirData}/table
    outDirTable_eos=${outDirData_eos}/table
fi

## pileup reweight name
echo "Starting configFile is "${configFile}
mcName ${configFile}
mcName=$(echo $mcName | sed 's|.root||g')
puName ${configFile}
echo "PUName: $puName"
echo "selection: $selection"
echo "mcName: ${mcName}"

if [ "${invMass_var}" == "invMass_regrCorr_egamma" ];then
    outDirMC=$baseDir/MCodd/${mcName}/${puName}/${selection}/${invMass_var}
    outDirMC_eos=$eosDir/MCodd/${mcName}/${puName}/${selection}/${invMass_var}
    isOdd="--isOddMC"
else
    outDirMC=$baseDir/MC/${mcName}/${puName}/${selection}/${invMass_var}
    outDirMC_eos=$eosDir/MC/${mcName}/${puName}/${selection}/${invMass_var}
fi
echo "outDirMC is ${outDirMC}"
outDirData=$baseDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
outDirData_eos=$eosDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
outDirTable=${outDirData}/table
outDirTable_eos=${outDirData_eos}/table

if [ ! -e "${outDirData}/table" ];then mkdir ${outDirData}/table -p; fi
if [ ! -e "${outDirData}/log" ];then mkdir ${outDirData}/log -p; fi
if [ ! -e "${outDirData_eos}" ];then mkdir ${outDirData_eos} -p; fi

if [ "${extension}" == "weight" ];then
    tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
    for tag in $tags
      do
      if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c r9Weight`" == "0" ];then
	  echo "[STATUS] Adding r9weight trees to $tag"
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile data/regions/scaleStep2smearing_1.dat  \
	      --saveRootMacro --r9WeightFile=data/r9weights/R9weights_CMSSW53X_RD1.root \
	      --smearerFit ${smearRange}
	  mv tmp/r9Weight_${tag}-`basename $configFile .dat`.root data/r9weights/ || exit 1
	  echo -e "$tag\tr9Weight\tdata/r9weights/r9Weight_${tag}-`basename $configFile .dat`.root" >> $configFile
      fi
    done
fi


if [ -n "${STEP1}" ];then
    # runNumber x eta with fitMethod
    regionFile=data/regions/scaleStep1.dat
    outFile=${outFileStep1}
    echo "outDirMC in step1 is "${outDirMC}
    if [ ! -e "${outDirTable}/${outFile}" -o -n "$FORCE" ];then
	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/fitres" ];then mkdir ${outDirData}/fitres -p; fi
	if [ ! -e "${outDirData}/img" ];then mkdir ${outDirData}/img -p; fi
	if [ ! -e "${outDirData}/step1" ];then mkdir ${outDirData}/step1 -p; fi
	if [ ! -e "tmp/step1_outDir.log" ];then echo ${outDirMC} > tmp/step1_outDir.log; fi #check on the outDirMC name (write it better)

	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile}  \
	    $isOdd $updateOnly --zFit --selection=${selection}  --invMass_var ${invMass_var} --commonCut $commonCut \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img \
	    |tee ${outDirData}/log/`basename ${outFile} .dat`.log || exit 1
	
	./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    >  ${outDirTable}/`basename ${outFile} .dat`.tex
	
	./script/tex2txt.sh ${outDirTable}/`basename ${outFile} .dat`.tex | awk -F "\t" -f awk/recalibOutput.awk |grep -v '^%'| grep -v 'EB '| grep -v 'EE ' > ${outDirTable}/${outFile}
	
	#save root files with step1 corrections
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	   --saveRootMacro --corrEleType HggRunEta \
	    --corrEleFile ${outDirTable}/${outFile/_scales.dat/} || exit 1 #EnergyScaleCorrection_class already puts at the end of the file name: _scales.dat

	mv tmp/scaleEle_HggRunEta_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step1/    
	echo "[STATUS] Step 1 done"
	echo "time dep-scale corrections built from "${outDirTable}/${outFile}

	if [ ! -e "${outDirMC_eos}" ];then mkdir ${outDirMC_eos} -p; fi
	if [ ! -e "${outDirData_eos}" ];then mkdir ${outDirData_eos} -p; fi
	rsync -ahuv ${outDirData} ${outDirData_eos}
	rsync -ahuv ${outDirMC} ${outDirMC_eos}

    else 
	echo "${outDirTable}/${outFile} exists, so:"
	echo "[STATUS] Step 1 already done"
	echo " "
	echo " "
	echo " "
	echo "[INFO] What you did in step1 is:
******************************************************************************
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile}  \
	    $isOdd $updateOnly --selection=${selection}  --invMass_var ${invMass_var} --commonCut $commonCut \
	    --zFit --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img \
	    |tee ${outDirData}/log/`basename ${outFile} .dat`.log || exit 1
	
	./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    >  ${outDirTable}/`basename ${outFile} .dat`.tex
	
	./script/tex2txt.sh ${outDirTable}/`basename ${outFile} .dat`.tex | awk -F \"\t\" -f awk/recalibOutput.awk |grep -v '^%'| grep -v 'EB '| grep -v 'EE ' > ${outDirTable}/${outFile}
	
	#save root files with step1 corrections
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	    --saveRootMacro --corrEleType HggRunEta \
	    --corrEleFile ${outDirTable}/${outFile/_scales.dat/} || exit 1
	
	mv tmp/scaleEle_HggRunEta_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step1/
******************************************************************************"
    fi

fi

if [ -n "${STEP1Stability}" ];then
##################################################
    regionFile=data/regions/stability.dat

    if [ ! -e "${outDirData}/step1/fitres" ];then mkdir ${outDirData}/step1/fitres -p; fi
    if [ ! -e "${outDirData}/step1/img" ];then mkdir ${outDirData}/step1/img -p; fi

    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
	$updateOnly --zFit --invMass_var ${invMass_var} --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step1/fitres \
	--corrEleType HggRunEta \
	--corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta \
	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step1/img |tee ${outDirData}/log/step1-stability.log || exit 1

    ./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step1/fitres \
	>  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex || exit 1

    mv ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex ${outDirTable}/step1_stability-${invMass_var}-${selection}_with_cumulative.tex 
    ##In case you prefer not to plot cumulative EB and EE:
    cat ${outDirTable}/step1_stability-${invMass_var}-${selection}_with_cumulative.tex| grep -v EB-runNumber | grep -v EE-runNumber > ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex

    if [ ! -e "${outDirMC_eos}" ];then mkdir ${outDirMC_eos} -p; fi
    if [ ! -e "${outDirData_eos}" ];then mkdir ${outDirData_eos} -p; fi
    rsync -ahuv ${outDirData} ${outDirData_eos}
    rsync -ahuv ${outDirMC} ${outDirMC_eos}
fi

if [ -n "${STEP1Plotter}" ];then
##################################################
    xVar=runNumber

    if [ ! -d ${outDirData}/step1/img/stability/before_run_corr/$xVar ];then
	mkdir -p ${outDirData}/step1/img/stability/before_run_corr/$xVar
    fi

    ./script/stability.sh -t  ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta_scales.tex \
	--outDirImgData ${outDirData}/step1/img/stability/before_run_corr/$xVar/ -x $xVar -y peak || exit 1
    ./script/stability.sh -t  ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta_scales.tex \
	--outDirImgData ${outDirData}/step1/img/stability/before_run_corr/$xVar/ -x $xVar -y scaledWidth || exit 1
    ./script/stability.sh -t  ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta_scales.tex \
	--outDirImgData ${outDirData}/step1/img/stability/before_run_corr/$xVar/ -x $xVar -y peak --allRegions || exit 1
    ./script/stability.sh -t  ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta_scales.tex \
	--outDirImgData ${outDirData}/step1/img/stability/before_run_corr/$xVar/ -x $xVar -y scaledWidth --allRegions || exit 1

    echo "Initial scale vs run plots in ${outDirData_eos}/step1/img/stability/before_run_corr/$xVar/"
    
##What you have after corr, i.e. now, at the end of step1stability
    xVar=runNumber
    if [ ! -d ${outDirData}/step1/img/stability/$xVar ];then
	mkdir -p ${outDirData}/step1/img/stability/$xVar
    fi

    if [ -e ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex ];then
	./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	    --outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y peak || exit 1
	./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	    --outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y scaledWidth || exit 1
	./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	    --outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y peak --allRegions || exit 1
	./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	    --outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y scaledWidth --allRegions || exit 1
	echo "Final scale vs run plots in ${outDirData}/step1/img/stability/$xVar/"
    fi

##################################################################################################################################
####Publish plots
    if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/" ];then 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/
    fi
    if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/" ];then 
	echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/ is being created"
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/ 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1_stab 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/Fits 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1_stab/Fits 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/Fits/data 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1_stab/Fits/data 
	www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/Fits/MC 
    fi

##############Summary plots
    #Copy the runRangesFile so it's easier to read the summary plot
    cp ${runRangesFile} ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/  
    cp ${runRangesFile} ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1_stab/  
    ##Inial summary
    mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step1/img/stability/before_run_corr/runNumber/*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/  
    mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step1/img/stability/before_run_corr/runNumber/*.pdf ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/  
    ###Final summary (after applying step1 corrections)
    mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step1/img/stability/runNumber/*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1_stab/ 

################Fit plots
    ###MC Fits
    mv ${tmp_path}/test/MC/${mcName}/${puName}/${selection}/${invMass_var}/img/*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/Fits/MC/  
    ###Initial Data Fits
    mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/img/*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1/Fits/data/
    ###Final Data Fits (after appyting step1 corrections}
    mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step1/img/*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step1_stab/Fits/data/
fi


if [ -n "${STEP2FIT}" ];then
    #eta x R9 with fit method
    regionFile=data/regions/scaleStep2.dat
    outFile=step2fit-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat

    if [ ! -e "${outDirTable}/${outFile}" ];then
   
	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/step2/fitres" ];then mkdir ${outDirData}/step2/fitres -p; fi
	if [ ! -e "${outDirData}/step2/img" ];then mkdir ${outDirData}/step2/img -p; fi

	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEta\t${outDirData}/step1/scaleEle_HggRunEta_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step2/`basename $configFile`
	
	./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile`  --regionsFile ${regionFile}   \
	    $isOdd  $updateOnly --selection=${selection} --invMass_var ${invMass_var} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
	    --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/img --corrEleType=HggRunEta \
	    > ${outDirData}/log/`basename $outFile .dat`.log || exit 1
	
	./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
	    > ${outDirTable}/`basename $outFile .dat`.tex || exit 1
	
	cut -d '&' -f 1-5   ${outDirTable}/${outFile} |  sed "s|-$commonCut||" > tmp/s.tex
	./script/CalcResCorrection.sh tmp/s.tex > tmp/res_corr.dat

	./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat \
	    --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat \
	    | grep -v '#' > ${outDirTable}/${outFile}
#    ./script/makeCorrVsRunNumber.py -l -c --file2=res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |sed -f sed/tex.sed > ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex

#	cat ${outDirTable}/${outFile} | cut -f 1,3-6 | awk '{printf("%s\t%s - %s\t%s\t%s \n", $1,$2,$3, $4, $5)}' | sed -r 's|\t|\t\&\t|g;' | sed -f sed/tex.sed | sed 's|R9|\\RNINE|' > ${outDirTable}/correctionNote-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	
    fi
fi

if [ -n "${STEP2}" ];then
    #eta x R9 with smearing method on top of step1

    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi
	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/step2${extension}/fitres" ];then mkdir ${outDirData}/step2${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step2${extension}/img" ];then mkdir ${outDirData}/step2${extension}/img -p; fi
	if [ ! -e "${outDirData}/step2${extension}/fitres_test" ];then mkdir ${outDirData}/step2${extension}/fitres_test -p; fi
	if [ ! -e "${outDirMC_eos}/fitres" ];then mkdir ${outDirMC_eos}/fitres -p; fi
	if [ ! -e "${outDirMC_eos}/img" ];then mkdir ${outDirMC_eos}/img -p; fi
	if [ ! -e "${outDirData_eos}/step2${extension}/fitres" ];then mkdir ${outDirData_eos}/step2${extension}/fitres -p; fi
	if [ ! -e "${outDirData_eos}/step2${extension}/img" ];then mkdir ${outDirData_eos}/step2${extension}/img -p; fi

    regionFileEB=${regionFileStep2EB}
    regionFileEE=${regionFileStep2EE}
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    regionFile=$regionFileEB
    outFile=outFile-`basename ${outFileStep2} _scales.dat`.dat
    #outFileStep2 has only the scales inside, outFile has both scales and smearings

    #check if step1 has been done
    #checkStepDep step1

    #categorize in eta-r9(prime) regions

    if [[ $scenario = "Categorize" ]] || [[ $scenario = "" ]]; then
	
	touch ${outDirData}/step2${extension}/`basename ${configFile}` #It seems better to touch first, if you write on eos
	cat $configFile > ${outDirData}/step2${extension}/`basename ${configFile}`
	for tag in `grep "^d" $configFile | grep selected | awk -F" " ' { print $1 } '`
	do
	    echo "[STATUS] Writing scale root files produced in step1 for " $tag
	    echo "${tag} scaleEle_HggRunEta ${outDirData}/step1/scaleEle_HggRunEta_${tag}-`basename $configFile .dat`.root" >> ${outDirData}/step2${extension}/`basename ${configFile}`
	done

	mkSmearerCatSignal $regionFileEB ${eos_path}/data/smearerCat/${invMass_var} $outDirData/step2${extension}/`basename $configFile` &> $outDirData/step2${extension}/mkSmearer_EB.log
	mkSmearerCatSignal $regionFileEE ${eos_path}/data/smearerCat/${invMass_var} $outDirData/step2${extension}/`basename $configFile` &> $outDirData/step2${extension}/mkSmearer_EE.log
	mkSmearerCatData   $regionFileEB ${outDirData}/step2${extension} $outDirData/step2${extension}/`basename $configFile` --corrEleType=HggRunEta >> $outDirData/step2${extension}/mkSmearer_EB.log 2>&1
	mkSmearerCatData   $regionFileEE ${outDirData}/step2${extension} $outDirData/step2${extension}/`basename $configFile` --corrEleType=HggRunEta >> $outDirData/step2${extension}/mkSmearer_EE.log 2>&1

    fi

    if [[ $scenario = "Test_job" ]]; then
	if [ ! -e "${outDirTable}/${outFile}" -o -n "$FORCE" ];then
	    if [ ! -e "${outDirData}/step2${extension}/fitres_test" ];then mkdir ${outDirData}/step2${extension}/fitres_test -p; fi
	    echo "You are sending a test job to see if you need to pass an initFile"	
	fi

	./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} --selection=${selection}  --smearerFit ${smearRange} --autoNsmear --autoBin --corrEleType=HggRunEta --outDirFitResData=${outDirData}/step2${extension}/fitres_test
	
#	./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} --selection=${selection} --smearerFit ${smearRange} --autoNsmear --autoBin  --corrEleType=HggRunEta --outDirFitResData=${outDirData}/step2${extension}/fitres_test

	echo "Now you should decide if you want to copy and edit"
	echo "cp ${outDirData}/step2${extension}/fitres_test/params-${basenameEB}-${commonCut}.txt ${outDirData}/step2${extension}/fitres/init-params-step2_1-${commonCut}.txt"
	echo "Now you should decide if you want to copy and edit"
	echo "cp ${outDirData}/step2${extension}/fitres_test/params-${basenameEE}-${commonCut}.txt ${outDirData}/step2${extension}/fitres/init-params-step2_2-${commonCut}.txt"
    fi
    
    if [[ $scenario = "Submit_jobs" ]] || [[ $scenario = "" ]]; then
	echo "Jobs are submitted only if this file doesn't exist: ${outDirTable}/${outFile}"

	if [ ! -e "${outDirTable}/${outFile}" -o -n "$FORCE" ];then

	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/step2${extension}/fitres" ];then mkdir ${outDirData}/step2${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step2${extension}/img" ];then mkdir ${outDirData}/step2${extension}/img -p; fi  
	if [ ! -e "${outDirMC_eos}/fitres" ];then mkdir ${outDirMC_eos}/fitres -p; fi
	if [ ! -e "${outDirMC_eos}/img" ];then mkdir ${outDirMC_eos}/img -p; fi
	if [ ! -e "${outDirData_eos}/step2${extension}/fitres" ];then mkdir ${outDirData_eos}/step2${extension}/fitres -p; fi
	if [ ! -e "${outDirData_eos}/step2${extension}/img" ];then mkdir ${outDirData_eos}/step2${extension}/img -p; fi  
	#In general I didn't need any initFile for step2
	initFile_1=""
	initFile_2=""

	for index in `seq 1 50`
	do
	    mkdir ${outDirData}/step2/${index}/fitres/ -p 
	    mkdir ${outDirData}/step2/${index}/img -p 
	done

	if [ "${noJobs}" != "y" ]
	then
		bsub -q cmscaf1nd \
			-oo ${outDirData}/step2/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
			-eo ${outDirData}/step2/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
			-J "${basenameEB} step2[1-50]" \
			"cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; mkdir ${outDirData}/step2/\$LSB_JOBINDEX/fitres/ -p; mkdir ${outDirData}/step2/\$LSB_JOBINDEX/img -p; ./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} --selection=${selection} --outDirFitResMC=${outDirMC}/fitres --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step2/\$LSB_JOBINDEX/fitres  --smearerFit ${smearRange} --autoNsmear --autoBin ${initFile_1}  --corrEleType=HggRunEta || exit 1; touch ${outDirData}/step2/\$LSB_JOBINDEX/${basenameEB}-done"

		bsub -q cmscaf1nd \
			-oo ${outDirData}/step2/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
			-eo ${outDirData}/step2/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
			-J "${basenameEE} step2[1-50]" \
			"cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; mkdir ${outDirData}/step2/\$LSB_JOBINDEX/fitres/ -p; mkdir ${outDirData}/step2/\$LSB_JOBINDEX/img -p; ./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} --selection=${selection} --outDirFitResMC=${outDirMC}/fitres --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step2/\$LSB_JOBINDEX/fitres  --smearerFit ${smearRange} --autoNsmear --autoBin ${initFile_2}  --corrEleType=HggRunEta || exit 1; touch ${outDirData}/step2/\$LSB_JOBINDEX/${basenameEE}-done"

		while [ "`bjobs -J \"${basenameEB} step2\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 1m; done
		while [ "`bjobs -J \"${basenameEE} step2\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 1m; done
	fi

	fi
	fi #Submit_jobs

    if [[ $scenario = Fit_Likelihood_1 ]] || [[ $scenario = "" ]]; then
	rm *core* #remove these stupid dumps, which are incredibly heavy
	echo "[STATUS] Adding jobs together from ${outDirData}/step2/*/fitres/"
	./script/haddTGraph.sh -o ${outDirData}/step2/fitres/outProfile-${basenameEB}-${commonCut}.root ${outDirData}/step2/*/fitres/outProfile-${basenameEB}-${commonCut}.root
	./script/haddTGraph.sh -o ${outDirData}/step2/fitres/outProfile-${basenameEE}-${commonCut}.root ${outDirData}/step2/*/fitres/outProfile-${basenameEE}-${commonCut}.root
	
	######################################################
	echo "{" > tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
	echo "FitProfile2(\"${outDirData}/step2/fitres/outProfile-${basenameEB}-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
	echo "FitProfile2(\"${outDirData}/step2/fitres/outProfile-${basenameEE}-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
	echo "}" >> tmp/fitProfiles.C
	root -l -b -q tmp/fitProfiles.C &> $outDirData/step2/fitProfile.log
	

	cat ${outDirData}/step2${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config > ${outDirTable}/${outFile}
	grep -v absEta_0_1 ${outDirData}/step2${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config >> ${outDirTable}/${outFile}
	echo "results (scale and smearings) for step2 are in ${outDirTable}/${outFile}"
#	cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outDirTable}/${outFile}
    fi
    if [[ $scenario = Plot_after_fit ]] || [[ $scenario = "" ]] || [[ $scenario = Fit_Likelihood ]]; then
	echo "[STATUS] Plotting Data/MC using as initFile the final results (just to have the proper data/MC plots)"
    ./bin/ZFitter.exe -f $outDirData/step2${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step2${extension}/img/ --outDirFitResData=${outDirData}/step2${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outDirTable}/${outFile} --corrEleType=HggRunEta --plotOnly  #|| exit 1
    ./bin/ZFitter.exe -f $outDirData/step2${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${selection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step2${extension}/img/ --outDirFitResData=${outDirData}/step2${extension}/fitres  --smearerFit ${smearRange} --autoNsmear --autoBin --initFile=${outDirTable}/${outFile} --corrEleType=HggRunEta --plotOnly  #|| exit 1
    fi #Fit_and_plot

    #### Setting file with scale corrections of step1 x step2
    if [[ $scenario = write_down_corr_step1_step2 ]] || [[ $scenario = "" ]] || [[ $scenario = Fit_Likelihood ]]; then
	###scale
	sed -i 's|^EB-||;s|^EE-||' ${outDirTable}/${outFileStep1}
	# incomprehensible sed manipulation
	grep scale ${outDirData}/step2/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config                     | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | sed 's|\([^ \t]*\)-gold|\1 gold|; s|\([^ \t]*\)-bad|\1 bad|;s|\([^ \t]*\)-highR9|\1 highR9|; s|\([^ \t]*\)-lowR9|\1 lowR9|'  > tmp/res_corr.dat 
	grep scale ${outDirData}/step2/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config | grep -v absEta_0_1| sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | sed 's|\([^ \t]*\)-gold|\1 gold|; s|\([^ \t]*\)-bad|\1 bad|;s|\([^ \t]*\)-highR9|\1 highR9|; s|\([^ \t]*\)-lowR9|\1 lowR9|'  >> tmp/res_corr.dat 
	
	awk -f awk/prodScaleCorrSteps.awk tmp/res_corr.dat ${outDirTable}/${outFileStep1} > ${outDirTable}/${outFileStep2}

	###constTerm
	#echo ${outDirData}/step2/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config
	#grep constTerm ${outDirData}/step2/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${commonCut}||g" > tmp/smear.dat
	#grep constTerm ${outDirData}/step2/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config |  grep -v absEta_0_1 | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${commonCut}||g" >> tmp/smear.dat
	#categories=`grep constTerm tmp/smear.dat | cut -f 1`
	#categories=`echo $categories | sed "s/constTerm_//g"`
	#categories=(${categories// / }) # array
	#smearings=`grep constTerm tmp/smear.dat | cut -f 2`
	#smearings=(${smearings// / }) # array
	#errors_smear=`grep constTerm tmp/smear.dat | cut -f 3`
	#errors_smear=(${errors_smear// / }) # array
	#if [ -e ${outDirTable}/smearing_corrections.dat ]; then rm ${outDirTable}/smearing_corrections.dat; fi
	#for i in "${!smearings[@]}"; do 
	#    echo ${categories[$i]} ${smearings[$i]}>> ${outDirTable}/smearing_corrections.dat
	#done
	#echo "complete set of corrections for scale step1*step2 is in ${outDirTable}/smearing_corrections.dat"
	echo "complete set of corrections for scale step1*step2 is in ${outDirTable}/${outFileStep2}"
    fi
    if [[ $scenario = root_corr_step1_step2 ]] || [[ $scenario = "" ]] || [[ $scenario = Fit_Likelihood ]]; then
	#save root files with step1*step2 (scale corrections)
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --corrEleType HggRunEtaR9 --corrEleFile ${outDirTable}/${outFileStep2/_scales.dat/} # || exit 1
	#./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --smearEleType stochastic --smearEleFile ${outDirTable}/smearing_corrections.dat || exit 1
	mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/    
    fi

#################copying the dat file over the eos web space#######################
    if [[ $scenario = finalize_step2 ]] || [[ $scenario = "" ]] || [[ $scenario = Fit_Likelihood ]]; then
	if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2" ];then 
	    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2 is being created"
	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2/ -p
	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2/DataMC/ -p
	fi
	mv ${outDirData}/step2/img/outProfile-*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2/
	cp ${outDirTable}/${outFileStep2}           ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2/ #do not mv this file: otherwise "checkStepDep step2" will fail!
	./script/latex_table_writer.sh ${outDirTable}/${outFile} -${commonCut}
	echo table_`basename ${outFile} .dat`"_scale_tex.dat"
	cp tmp/table_`basename ${outFile} .dat`_scale_tex.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2/
	cp tmp/table_`basename ${outFile} .dat`_smear_tex.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2/

        ###Data_MC_plots at the end of step2 (here the MC is shifted (not the data as in the analyses) to see the original position of the data peak)
	file2EB=`basename ${regionFileEB} .dat`
	file2EE=`basename ${regionFileEE} .dat`
#
	./script/plot_histos_validation.sh ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step2${extension}/fitres/histos-${file2EB}-${commonCut}.root
	./script/plot_histos_validation.sh ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step2${extension}/fitres/histos-${file2EE}-${commonCut}.root
	mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step2${extension}/./img/histos-*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step2${extension}/DataMC/
    fi

    if [[ $scenario = rsynch ]]; then
	if [ ! -e "${outDirMC_eos}" ];then mkdir ${outDirMC_eos} -p; fi
	if [ ! -e "${outDirData_eos}" ];then mkdir ${outDirData_eos} -p; fi
	rsync -ahuv ${outDirData} ${outDirData_eos}
	rsync -ahuv ${outDirMC} ${outDirMC_eos}
    fi
#Here step2 is closed	
fi



if [ -n "${STEP3}" ];then
    regionFile=data/regions/scaleStep3.dat
    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step3/fitres" ];then mkdir ${outDirData}/step3/fitres -p; fi
    if [ ! -e "${outDirData}/step3/img" ];then mkdir ${outDirData}/step3/img -p; fi

    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step3/`basename $configFile`
    
    ./bin/ZFitter.exe -f $outDirData/step3/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step3/img \
	--corrEleType HggRunEtaR9  > ${outDirData}/log/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    
    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1

fi


echo before step4
if [ -n "${STEP4}" ];then
    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi

    if [ "${extension}" == "Et_22" ];then
	echo "[INFO] You are using Et_22-noPF as commonCut"
	oldCommonCut=$commonCut
	commonCut="Et_22-noPF"
    elif [ "${extension}" == "Et_25" ];then
	echo "[INFO] You are using Et_25-noPF as commonCut"
	oldCommonCut=$commonCut
	commonCut="Et_25-noPF"
    fi
    oldMass=${invMass_var}
    if [ "${extension}" == "invMass_SC_pho_regrCorr" ];then
	echo "[INFO] You are using invMass_SC_pho_regrCorr"
	invMass_var="invMass_SC_pho_regrCorr"
    fi
    if [ "${extension}" == "invMass_SC_corr" ];then
	echo "[INFO] You are using invMass_SC_corr"
	invMass_var="invMass_SC_corr"
    fi
    
    #eta x R9 with smearing method having fixed the scale in step2
    regionFileEB=${regionFileStep4EB}
    regionFileEE=${regionFileStep4EE}
    if [ "${extension}" == "r9syst_93" ]; then
	regionFileEB=${regionFileStep4EB_93}
	regionFileEE=${regionFileStep4EE_93}
    elif [ "${extension}" == "r9syst_95" ]; then
	regionFileEB=${regionFileStep4EB_95}
	regionFileEE=${regionFileStep4EE_95}
    fi
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    regionFile=$regionFileEB
    outFile=$outDirTable/outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat
    
    checkStepDep step2
    
    if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
    if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
    if [ ! -e "${outDirData}/step4${extension}/fitres" ];then mkdir ${outDirData}/step4${extension}/fitres -p; fi
    if [ ! -e "${outDirData}/step4${extension}/img" ];then    mkdir ${outDirData}/step4${extension}/img -p; fi
    if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
    if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
    if [ ! -e "${outDirData}/step4${extension}/fitres" ];then mkdir ${outDirData}/step4${extension}/fitres -p; fi
    if [ ! -e "${outDirData}/step4${extension}/img" ];then    mkdir ${outDirData}/step4${extension}/img -p; fi
    
    if [ -e "${outDirTable}/params-step4-${commonCut}.txt" ];then 
	initFile="--initFile=${outDirTable}/params-step4-${commonCut}.txt"; 
    else 
	initFile=""
    fi
    
    if [[ $scenario = "cat_step4" ]] || [[ $scenario = "" ]]; then
	touch ${outDirData}/step4${extension}/`basename ${configFile}`
	cat $configFile > ${outDirData}/step4${extension}/`basename ${configFile}`
	for tag in `grep "^d" $configFile | grep selected | awk -F" " ' { print $1 } '`
	do
            echo "[STATUS] Writing scale root files produced in step1 for " $tag
	    echo "${tag} scaleEle_HggRunEtaR9 ${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" >> ${outDirData}/step4${extension}/`basename ${configFile}`
	done
	echo "configFile for step4" is $outDirData/step4${extension}/`basename $configFile`
	
	MCscenario=""
	case $extension in
	    amctnlo)  sample=s1 ;;
            madgraph) sample=s2 ;;
            *)        sample=s1 ;;
	esac
	
	case $extension in
            amctnlo|madgraph)
		cat ${outDirData}/step4${extension}/`basename ${configFile}` \
		    |grep -v '#' \
                    | grep -v "r9weights" \
                    | grep -v "pileup" \
		    | sed '/^d/d' \
                    | sed "/^${sample}/{p; s|^s|d|}" \
                    > tmp/step4${extension}.dat
		mv tmp/step4${extension}.dat $outDirData/step4${extension}/`basename $configFile`
		MCscenario="--noPU --onlyScale" #add this is the job definition
		;;
	esac
	
	mkSmearerCatSignal $regionFileEB ${eos_path}/data/smearerCat/${invMass_var} $outDirData/step4${extension}/`basename $configFile`
	mkSmearerCatSignal $regionFileEE ${eos_path}/data/smearerCat/${invMass_var} $outDirData/step4${extension}/`basename $configFile`
	mkSmearerCatData   $regionFileEB ${outDirData}/step4 $outDirData/step4${extension}/`basename $configFile` --corrEleType=HggRunEtaR9
	mkSmearerCatData   $regionFileEE ${outDirData}/step4 $outDirData/step4${extension}/`basename $configFile` --corrEleType=HggRunEtaR9
	
	case $extension in
	    ##At this point it takes care of the cat files
            amctnlo|madgraph)
		cat ${outDirData}/step4${extension}/`basename ${configFile}` \
		    | sed '/^d/d' \
                    | sed "/^${sample}/{p; s|^s|d|}" \
                    > tmp/step4${extension}.dat
		mv tmp/step4${extension}.dat $outDirData/step4${extension}/`basename $configFile`
		MCscenario="--noPU --onlyScale" #add this is the job definition
		;;
	esac
    fi # it closes cat_step4
    
    if [[ $scenario = "jobs_step4" ]] || [[ $scenario = "" ]]; then
	echo "jobs are submitted if ${outFile} does NOT exist"
	if [ ! -e "${outFile}" -o -n "$FORCE" ];then
	    if [ "${extension}" == "weight" ];then
		updateOnly="$updateOnly --useR9weight"
	    fi
	    
	    echo "jobs in step4 are configured in $extension"
	    echo "Submitting jobs with selection=${newSelection} and additional settings=${MCscenario}"

	    for index in `seq 1 50`
	    do
		mkdir ${outDirData}/step4${extension}/${index}/fitres/ -p 
		mkdir ${outDirData}/step4${extension}/${index}/img -p 
	    done
	    #cat $outDirData/step4${extension}/`basename $configFile`
	    
		if [ "${noJobs}" != "y" ]
		then
		bsub -q cmscaf1nd \
		-oo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
		-eo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
		-J "${basenameEB} step4${extension}[1-50]" \
		"cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/fitres  --smearerFit ${smearRange} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 ${MCscenario} || exit 1; touch ${outDirData}/step4${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"
	    
	    bsub -q cmscaf1nd \
		-oo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
		-eo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
		-J "${basenameEE} step4${extension}[1-50]" \
		"cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/fitres  --smearerFit ${smearRange} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 ${MCscenario} || exit 1; touch ${outDirData}/step4${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"
	    
	
	while [ "`bjobs -J \"${basenameEB} step4${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
	while [ "`bjobs -J \"${basenameEE} step4${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
	fi #noJobs
	fi #submit jobs if outFile doesn't exist
	
	rm *core* #remove these stupid dumps, which are incredibly heavy
	./script/haddTGraph.sh -o ${outDirData}/step4${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step4${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
	./script/haddTGraph.sh -o ${outDirData}/step4${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step4${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root
#    
    fi #it closes jobs_step4
    
    if [[ $scenario = "fit_step4" ]] || [[ $scenario = "" ]]; then
	./script/haddTGraph.sh -o ${outDirData}/step4${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step4${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
	./script/haddTGraph.sh -o ${outDirData}/step4${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step4${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root
	echo "you are fitting in step4"
	######################################################
	echo "{" > tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
	echo "FitProfile2(\"${outDirData}/step4${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
	echo "FitProfile2(\"${outDirData}/step4${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
	echo "}" >> tmp/fitProfiles.C
	root -l -b -q tmp/fitProfiles.C
	
	cat ${outDirData}/step4${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config > ${outFile}
	grep -v absEta_0_1 ${outDirData}/step4${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config >> ${outFile}
    fi #it closes fit_step4

    if [[ $scenario = "smear_step4" ]] || [[ $scenario = "" ]]; then
	##cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}## what's the purpose of this?
	##getSmearEleCfg ${outFile} > `dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat ##awk script does not exist anymore
	###write constTerm at the end of step4
	grep constTerm ${outFile} |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${commonCut}||g" > tmp/smear.dat
	categories=`grep constTerm tmp/smear.dat | cut -f 1`
	categories=`echo $categories | sed "s/constTerm_//g"`
	categories=(${categories// / }) # array
	smearings=`grep constTerm tmp/smear.dat | cut -f 2`
	smearings=(${smearings// / }) # array
	errors_smear=`grep constTerm tmp/smear.dat | cut -f 3`
	errors_smear=(${errors_smear// / }) # array
	if [ -e `dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat ]; then rm `dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat; fi
	if [ -e `dirname ${outFile}`/`basename $outFile .dat`-smearEle_err.dat ]; then rm `dirname ${outFile}`/`basename $outFile .dat`-smearEle_err.dat; fi
	for i in "${!smearings[@]}"; do 
	    echo ${categories[$i]} ${smearings[$i]}>> `dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat;
	    echo ${categories[$i]} ${smearings[$i]} ${errors_smear[$i]}>> `dirname ${outFile}`/`basename $outFile .dat`-smearEle_err.dat
	done
	echo "complete set of smearing corrections is in `dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat"

	#save root files with smearing corrections
	#./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --smearEleType stochastic --smearEleFile `dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat || exit 1
	#mv corrections somewhere: do not leave them in tmp/
    fi
    
##PlotOnly
    if [[ $scenario = "plotOnly_step4" ]] || [[ $scenario = "" ]]; then
	echo "[STATUS] You are plotting data-MC comparisons using this file "${outFile} " as input"
	./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  ${MCscenario} || exit 1
    
    
	./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly ${MCscenario} || exit 1
    
	#EB, EE, Inclusive plots 
	#You need to add the --smearEleFile/Type in bin/ZFitter
	#./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile $regionFileStep4EBEE $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  --smearEleFile=`dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat --smearEleType=EtaR9_const ${MCscenario} || exit 1

	#./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile $regionFileStep4EBEE_r9 $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  --smearEleFile=`dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat --smearEleType=EtaR9_const ${MCscenario} || exit 1

	#./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile $regionFileStep4EBEE_Hgg $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  --smearEleFile=`dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat --smearEleType=EtaR9_const ${MCscenario} || exit 1

	#./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile $regionFileStep4Inclusive $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit ${smearRange}  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  --smearEleFile=`dirname ${outFile}`/`basename $outFile .dat`-smearEle.dat --smearEleType=EtaR9_const ${MCscenario} || exit 1
    fi #this closes plotOnly

    if [ "${extension}" == "Et_22" -o "${extension}" == "Et_25" ];then
	commonCut=${oldCommonCut}
    fi
    if [ "${extension}" == "invMass_SC_pho_regrCorr" ];then
	invMass_var=$oldMass
    fi

    if [[ $scenario = "finalize_step4" ]] || [[ $scenario = "" ]]; then
	if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}" ];then 
	    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension} is being created"
	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/ -p
	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/DataMC/ -p
	fi

#       This is in case you want to re-do the likelihood fits to get better determination of the minima
#	echo "you are fitting in finalize_step4" #in case you used the wrong label
#	echo "{" > tmp/fitProfiles.C
#	echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
#	echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
#	echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
#	echo "FitProfile2(\"${outDirData}/step4${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#	echo "FitProfile2(\"${outDirData}/step4${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#	echo "}" >> tmp/fitProfiles.C
#	root -l -b -q tmp/fitProfiles.C

	mv ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/img/outProfile-scaleStep4smearing_*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/
	cp ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/table/outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/
	cp ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/table/outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9-smearEle_err.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/
	./script/latex_table_writer.sh ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/table/outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat -${commonCut}
	echo "
	mv tmp/table_outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9_scale_tex.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/"
	mv tmp/table_outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9_scale_tex.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/
	mv tmp/table_outFile-step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9_smear_tex.dat ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/

    ###Data_MC_plots at the end of step4
	file4EB=`basename ${regionFileStep4EB} .dat`
	file4EE=`basename ${regionFileStep4EE} .dat`
	file4EBEE=`basename ${regionFileStep4EBEE} .dat`
	file4EBEE_Hgg=`basename ${regionFileStep4EBEE_Hgg} .dat`
	file4EBEE_r9=`basename ${regionFileStep4EBEE_r9} .dat`
	file4Inclusive=`basename ${regionFileStep4Inclusive} .dat`

	./script/plot_histos_validation.sh ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/fitres/histos-${file4EB}-${commonCut}.root
	./script/plot_histos_validation.sh ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/fitres/histos-${file4EE}-${commonCut}.root
	#./script/plot_histos_validation.sh ${eos_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/fitres/histos-${file4EBEE}-${commonCut}.root
	#./script/plot_histos_validation.sh ${eos_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/fitres/histos-${file4EBEE_r9}-${commonCut}.root
	#./script/plot_histos_validation.sh ${eos_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/fitres/histos-${file4EBEE_Hgg}-${commonCut}.root
	#./script/plot_histos_validation.sh ${eos_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/fitres/histos-${file4Inclusive}-${commonCut}.root
	mv ${tmp_path}/test/dato/${file}/${selection}/${oldMass}/step4${extension}/./img/histos-* ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${oldMass}/step4${extension}/DataMC/
    fi

    if [[ $scenario = rsynch ]]; then
	if [ ! -e "${outDirMC_eos}" ];then mkdir ${outDirMC_eos} -p; fi
	if [ ! -e "${outDirData_eos}" ];then mkdir ${outDirData_eos} -p; fi
	rsync -ahuv ${outDirData} ${outDirData_eos}
	rsync -ahuv ${outDirMC} ${outDirMC_eos}
    fi
fi #step4_is_closed

echo After step4
#######Et steps begin!#######
if [ -n "${STEP5}" -o -n "${STEP6}" -o -n "${STEP7}" -o -n "${STEP8}" ];then
	if [[ $invMass_var == *"rawSC"* ]]
	then
		echo "[WARNING] **************** Overriding smearRange for rawSC invmas *******"
		smearRange="--smearMassMin 60 --smearMassMax 100"
	fi
	thisstep=step$STEP
	echo Doing $thisstep
	step5EBconfig=outProfile-`basename ${regionFileStep5EB} .dat`-${commonCut}-FitResult-.config
	step5EEconfig=outProfile-`basename ${regionFileStep5EE} .dat`-${commonCut}-FitResult-.config
	case ${STEP} in
		5EB)
			step_args="--profileOnly --plotOnly  --onlyScale"
			scalesfromstep=step2
			corrEleType=HggRunEtaR9
			newCorrEleType=HggRunEtaR9Et_step5_EB
			laststep_EB_config=${outDirData}/step2/img/outProfile-`basename ${regionFileStep2EB} .dat`-${commonCut}-FitResult-.config
			laststep_EE_config=${outDirData}/step2/img/outProfile-`basename ${regionFileStep2EE} .dat`-${commonCut}-FitResult-.config
			scales_config=${laststep_EE_config}
			;;
		5EE)
			step_args="--profileOnly --plotOnly  --onlyScale"
			scalesfromstep=step2
			corrEleType=HggRunEtaR9
			newCorrEleType=HggRunEtaR9Et
			laststep_EB_config=${outDirData}/step5EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step2/img/outProfile-`basename ${regionFileStep2EE} .dat`-${commonCut}-FitResult-.config
			scales_config=${laststep_EB_config}
			;;
		6EB)
			step_args=" --onlyScale"
			scalesfromstep=Moriond17
			corrEleType=Moriond1723JanPhoScales
			corrEleType=Moriond1723JanEleScales
			newCorrEleType=HggRunEtaR9Et_step6_EB
			laststep_EB_config=${outDirData}/step5EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step5EE/img/${step5EEconfig}
			scales_config=${outDirData}/step2/img/outProfile-`basename ${regionFileStep2EE} .dat`-${commonCut}-FitResult-.config
			;;
		6EE)
			step_args=" --onlyScale"
			scalesfromstep=step6EB
			corrEleType=HggRunEtaR9Et_step6_EB
			newCorrEleType=HggRunEtaR9Et
			laststep_EB_config=${outDirData}/step6EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step5EE/img/${step5EEconfig}
			scales_config=${laststep_EB_config}
			;;
		7EB)
			step_args=""
			scalesfromstep=step2
			scalesfromstep=Moriond17
			corrEleType=HggRunEtaR9
			corrEleType=Moriond1723JanEleScales
			newCorrEleType=HggRunEtaR9Et_step7_EB
			laststep_EB_config=${outDirData}/step5EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step5EE/img/${step5EEconfig}
			scales_config=${outDirData}/step2/img/outProfile-`basename ${regionFileStep2EE} .dat`-${commonCut}-FitResult-.config
			;;
		7EE)
			step_args=""
			scalesfromstep=step7EB
			corrEleType=HggRunEtaR9Et_step7_EB
			newCorrEleType=HggRunEtaR9Et
			laststep_EB_config=${outDirData}/step7EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step5EE/img/${step5EEconfig}
			scales_config=${laststep_EB_config}
			;;
		8EB)
			step_args=""
			scalesfromstep=step7EE
			corrEleType=HggRunEtaR9Et
			newCorrEleType=HggRunEtaR9Et_step8_EB
			laststep_EB_config=${outDirData}/step7EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step7EE/img/${step5EEconfig}
			scales_config=${laststep_EE_config}
			;;
		8EE)
			step_args=""
			scalesfromstep=step8EB
			corrEleType=HggRunEtaR9Et_step8_EB
			newCorrEleType=HggRunEtaR9Et
			laststep_EB_config=${outDirData}/step8EB/img/${step5EBconfig}
			laststep_EE_config=${outDirData}/step7EE/img/${step5EEconfig}
			scales_config=${laststep_EB_config}
			;;
	esac

    echo "You are making $thisstep"
    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi
    if [ ! -e "${outDirData}/${thisstep}${extension}/fitres" ];then mkdir ${outDirData}/${thisstep}${extension}/fitres -p; fi
    if [ ! -e "${outDirData}/${thisstep}${extension}/img" ];then    mkdir ${outDirData}/${thisstep}${extension}/img -p; fi

    configFile_stepN=$outDirData/${thisstep}${extension}/`basename $configFile`
    # root files for corrections have already been created in step2 and checked in step4
    # include them in the dat file for step5
	 # setup correction files from previous step in dat file
    if [ ! -e "${configFile_stepN}" ];then
		 echo "[STATUS] Creating dat file for ${thisstep}${extension}"
		 cp $configFile $configFile_stepN
		 data_tags=`grep -v '#' $configFile_stepN | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [d][1-9]`
		 is_scale_cat=`grep scaleEle_${corrEleType} $configFile_stepN | wc -l`
		 if [ "${is_scale_cat}" == "0" ]; then # you don't want to write the scale cat if they are already written
	    	 echo "[STATUS] Writing the scale root files in ${configFile_stepN}"
	    	 for tag in $data_tags; do
				 echo -e "${tag}\tscaleEle_${corrEleType}\t${outDirData}/${scalesfromstep}/scaleEle_${corrEleType}_${tag}-`basename $configFile_stepN .dat`.root" >> $configFile_stepN
	    	 done
		 fi
    fi
    
    echo "config File used for jobs in ${thisstep}${extension} is " $outDirData/${thisstep}${extension}/`basename $configFile`

   #eta x Et with smearing method (use step4 as initialization)
	case ${STEP} in
		*EB)
			regionFile=${regionFileStep5EB}
			;;
		*EE)
			regionFile=${regionFileStep5EE}
			;;
		*)
			regionFile=${regionFileStep5EB}
	esac
    basenameRegionFile=`basename $regionFile .dat`
    outFile=${outDirTable}/${thisstep}${extension}-${invMass_var}-${newSelection}-${commonCut}-${newCorrEleType}.dat

    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
	echo "[WARNING] You are running ${thisstep}${extension} without having step2"
    fi

	#leave initFile empty (or built it form step4 (to be checked)
	thisstep_params=${outDirTable}/params-${thisstep}${extension}-${commonCut}.txt
	if [ -e "${thisstep_params}" ];then
		echo Using $thisstep_params for initFile
		initFile="--initFile=${outDirTable}/params-${thisstep}${extension}-${commonCut}.txt";
	else
		echo "[WARNING] init file $thisstep_params not found" | tee /dev/stderr
		echo "          creating file from "  | tee /dev/stderr
		echo "          EB: ${laststep_EB_config}"  | tee /dev/stderr
		echo "          EE: ${laststep_EE_config}"  | tee /dev/stderr
		case ${STEP} in
			5*)
				#Set constTerm excursion from 0 - 0.03 in the profile
				#Since you are running with constTermFix: smearings are NOT organized in Et bins
				grep constTerm ${laststep_EB_config} |sed -r 's|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.03)|' > ${thisstep_params}
				#Create init for alpha (to be fixed)
				#s|L|C L|
				if [ "${Et_smear}" == "" ];then
					#if not smearing Et -> put alpha constant at 0
					grep constTerm ${laststep_EB_config} |sed -r 's|constTerm|alpha|;s|[.0-9]* [+]/- [.0-9]*|0.0000 +/- 0.0100 C|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.20)|' >> ${thisstep_params}
				fi
				# EE
				grep constTerm ${laststep_EE_config} |grep -v absEta_0_1 |sed -r 's|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.03)|' >> ${thisstep_params}
				#Create init for alpha
				if [ "${Et_smear}" == "" ];then
					#if not smearing Et -> put alpha constant at 0
					grep constTerm ${laststep_EE_config} |grep -v absEta_0_1 |sed -r 's|constTerm|alpha|;s|[.0-9]* [+]/- [.0-9]*|0.0000 +/- 0.0100 C|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.20)|' >> ${thisstep_params}
				fi
				;;
			6*|7*|8*)
				rm ${thisstep_params}
				for cf in $laststep_EB_config $laststep_EE_config
				do
					if grep phi ${cf} &> /dev/null
					then
						awk -f awk/rhophi_to_constalpha.awk ${cf} |sort -V | grep -v EB >> ${thisstep_params}
					else
						sort -V ${cf} | grep -v EB >> ${thisstep_params}
					fi
					sed -i "s|\(scale_.*\)L(.*)|\1L(0.95 - 1.05)|g" ${thisstep_params}
					sed -i "s|\(constTerm_.*\)L(.*)|\1L(0. - 0.05)|g" ${thisstep_params}
					sed -i "s|\(alpha_.*\)L(.*)|\1L(0. - 0.15)|g" ${thisstep_params}
				done
				;;
		esac
		echo "[DEBUG] ##### ${thisstep_params} BEGIN ##### "
		cat ${thisstep_params}
		echo "[DEBUG] ##### ${thisstep_params} END   ##### "
		#exit 0 #-> don't exit: first create the file, then run !

	fi
	initFile="--initFile=${thisstep_params}"; 


	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
	if [ ! -e "${outDirData}/${thisstep}${extension}/fitres" ];then mkdir ${outDirData}/${thisstep}${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/${thisstep}${extension}/img" ];then    mkdir ${outDirData}/${thisstep}${extension}/img -p; fi
	
	
	if [ "${extension}" == "weight" ];then
	    updateOnly="$updateOnly --useR9weight"
	fi

	#Categorize in Et X Eta, if files already exist, do not remake them
	# to force remaking of files, add -f to end of each of these
	mkSmearerCatSignal $regionFile ${eos_path}/data/smearerCat/${invMass_var} $outDirData/${thisstep}${extension}/`basename $configFile` 2>&1 | tee $outDirData/${thisstep}${extension}/mkSmearer.log
	mkSmearerCatData   $regionFile ${outDirData}/${thisstep}${extension} $outDirData/${thisstep}${extension}/`basename $configFile` --corrEleType=${corrEleType} 2>&1 | tee $outDirData/${thisstep}${extension}/mkSmearer.log


	job_args="-f $outDirData/${thisstep}${extension}/`basename $configFile` $isOdd $updateOnly"
	job_args="$job_args --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut}"
	job_args="$job_args --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img"
	job_args="$job_args --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin"
	job_args="$job_args ${initFile} --corrEleType=${corrEleType} ${step_args}"
	if [[ $scenario = "test_jobs_${thisstep}" ]]; then
	    echo "./bin/ZFitter.exe ${job_args} --regionsFile ${regionFile} --outDirImgData=${outDirData}/${thisstep}${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${thisstep}${extension}/\$LSB_JOBINDEX/fitres"
	fi

	if [[ $scenario = "jobs_${thisstep}" ]] || [[ $scenario = "" ]]; then
		if [ "$noJobs" != "y" ]
		then
			echo "[STATUS] SENDING JOBS @${thisstep}${extension}"
			for index in `seq 1 50`
			do
				mkdir ${outDirData}/${thisstep}${extension}/${index}/fitres/ -p 
				mkdir ${outDirData}/${thisstep}${extension}/${index}/img -p 
			done

			bsub -q cmscaf1nd \
				-oo ${outDirData}/${thisstep}${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameRegionFile}-stdout.log \
				-eo ${outDirData}/${thisstep}${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameRegionFile}-stderr.log \
				-J "${basenameRegionFile} ${thisstep}${extension}[1-50]" \
				"cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
			./bin/ZFitter.exe ${job_args} --regionsFile ${regionFile} --outDirImgData=${outDirData}/${thisstep}${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${thisstep}${extension}/\$LSB_JOBINDEX/fitres || exit 1; touch ${outDirData}/${thisstep}${extension}/\$LSB_JOBINDEX/`basename $regionFile .dat`-done"

			while [ "`bjobs -J \"${basenameRegionFile} ${thisstep}${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done

		fi
	fi #scenario=jobs_step5

	FitResult_thisstep=${outDirData}/${thisstep}${extension}/img/outProfile-${basenameRegionFile}-${commonCut}-FitResult-.config
    if [[ $scenario = "fit" ]] || [[ $scenario = "" ]]; then
		 echo "Doing Fits $thisstep"
	######################################################
	./script/haddTGraph.sh -o ${outDirData}/${thisstep}${extension}/fitres/outProfile-$basenameRegionFile-${commonCut}.root ${outDirData}/${thisstep}${extension}/*/fitres/outProfile-$basenameRegionFile-${commonCut}.root
    echo "{" > tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/${thisstep}${extension}/fitres/outProfile-$basenameRegionFile-${commonCut}.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles.C
    echo "}" >> tmp/fitProfiles.C
    root -l -b -q tmp/fitProfiles.C &> $outDirData/${thisstep}${extension}/fitProfiles.log
	 echo "Doing Fits $thisstep done"

   #Write the dat file with fit results
	rm ${outFile}

	case ${scales_config} in
		*step2*)
			remove_barrel_cat="grep -v absEta_0_1"
			;;
		*)
			remove_barrel_cat="grep -v EB"
	esac

	case ${STEP} in
		*EB)
			#EB file
			awk -f awk/rhophi_to_constalpha.awk ${FitResult_thisstep} | sort -V >> ${outFile}
			#EE file
			#awk -f awk/rhophi_to_constalpha.awk ${scales_config}      | sort -V | $remove_barrel_cat >> ${outFile}
			;;
		*EE)
			#EB file
			awk -f awk/rhophi_to_constalpha.awk ${scales_config}      | sort -V >> ${outFile}
			#EE file
			awk -f awk/rhophi_to_constalpha.awk ${FitResult_thisstep} | sort -V | $remove_barrel_cat >> ${outFile}
			;;
	esac
    #echo "Fit results of ${thisstep}${extension} in " ${outDirData}/${thisstep}${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config

	 #grab constants from init file
    cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}
    echo "outFile of ${thisstep}${extension} is " ${outFile} " (this will be the initFile of the plots)"
    fi #fit scenario

    if [[ $scenario = "plot" ]] || [[ $scenario = "" ]]; then
		 echo "[STATUS] Plot ${thisstep}"
    	 ./bin/ZFitter.exe -f $outDirData/${thisstep}${extension}/`basename $configFile` --regionsFile ${regionFile} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/${thisstep}${extension}/img/ --outDirFitResData=${outDirData}/${thisstep}${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=${corrEleType} --plotOnly &> $outDirData/${thisstep}${extension}/zfitter_plot|| exit 1
		 echo "[STATUS] Plot ${thisstep} done "
    fi

	scalesfromstepConfig=${outDirTable}/${scalesfromstep}${extension}-${invMass_var}-${newSelection}-${commonCut}-${corrEleType}_scales.dat
	thisstepConfig=${outDirTable}/${thisstep}${extension}-${invMass_var}-${newSelection}-${commonCut}-${newCorrEleType}_scales.dat
	if [[ $scenario = "write_corr" ]] || [[ $scenario = "" ]]; then
		echo "[STATUS] Writing scaleEle ${thisstep}"
		echo "[STATUS] Combining ${scalesfromstepConfig}"
		echo "[STATUS] with      ${FitResult_thisstep}"
		grep scale ${FitResult_thisstep} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | sed 's|\(Et_[0-9]*_[0-9]*\)-\([^ \t]*\)|\2 \1 |' > tmp/res_corr_${thisstep}.dat
		awk -f awk/prodScaleCorrSteps.awk tmp/res_corr_${thisstep}.dat ${scalesfromstepConfig} > ${thisstepConfig}
		#save root files with step6 corrections
		#echo ${regionFile}
		./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --corrEleType ${newCorrEleType} --corrEleFile ${thisstepConfig/_scales.dat/} &> $outDirData/${thisstep}${extension}/zfitter_corr.log || exit 1
		mv tmp/scaleEle_${newCorrEleType}_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/${thisstep}/
		echo "[STATUS] Writing scaleEle ${thisstep} done"
	fi


	if [[ $scenario = "finalize_${thisstep}" ]] || [[ $scenario = "" ]]; then
		if [ ! -d "${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension}" ];then
			echo "${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension} is being created"
			www_mkdir ${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension}/ -p
			www_mkdir ${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension}/DataMC/ -p
		fi
		cp ${outFile} ${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension}/
		mv ${outDirData}/${thisstep}${extension}/img/outProfile-*.png ${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension}/
		./script/plot_histos_validation.sh ${outDirData}/${thisstep}${extension}/fitres/histos-${basenameRegionFile}-${commonCut}.root
		mv ${outDirData}/${thisstep}${extension}/./img/histos-* ${tmp_path_plots}/RUN2_ECAL_Calibration/${file}/${invMass_var}/${thisstep}${extension}/DataMC/
	fi

fi

#if [ -n "${STEP6}" ];then
#    echo "You are making step6"
#    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
#	newSelection=${extension}
#    else
#	newSelection=${selection}
#    fi
#    #eta x Et with smearing method (use step5 quick likelihood scan as initialization)
#    regionFileEB=${regionFileStep5EB}
#    regionFileEE=${regionFileStep5EE}
#    basenameEB=`basename $regionFileEB .dat`
#    basenameEE=`basename $regionFileEE .dat`
#    outFile=${outDirTable}/step6${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#
#    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
#	echo "[ERROR] Impossible to run step6 without step2" >> /dev/stderr
#	exit 1
#    fi
#    
#    echo "initFile for step6 built from step5"
#    if [ -e "${outDirTable}/params-step6-${commonCut}.txt" ];then 
#	initFile="--initFile=${outDirTable}/params-step6-${commonCut}.txt"; 
#    else 
#	echo "[INFO] initFile for step6 from step5 results: ${outDirTable}/step5${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat"
#	cp ${outDirTable}/step5${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat ${outDirTable}/params-step6-${commonCut}.txt
#	initFile="--initFile=${outDirTable}/params-step6-${commonCut}.txt"; 
#    fi
#
#    ##Ranges of the parameters need to be adjusted
#    grep scale ${outDirTable}/params-step6-${commonCut}.txt | sed "s|L(.*)|L(0.95 - 1.05)|g" > tmp/temp.txt 
#    grep constTerm ${outDirTable}/params-step6-${commonCut}.txt | sed "s|L(.*)|L(0. - 0.05)|g" >> tmp/temp.txt 
#    grep alpha ${outDirTable}/params-step6-${commonCut}.txt >> tmp/temp.txt 
#    mv tmp/temp.txt ${outDirTable}/params-step6-${commonCut}.txt
#
#    echo "job submission in step6 is done if ${outFile} does NOT exist"
#    if [ ! -e "${outFile}" ];then
#
#	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
#	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
#	if [ ! -e "${outDirData}/step6${extension}/fitres" ];then mkdir ${outDirData}/step6${extension}/fitres -p; fi
#	if [ ! -e "${outDirData}/step6${extension}/img" ];then    mkdir ${outDirData}/step6${extension}/img -p; fi
#	
#	
#	if [ "${extension}" == "weight" ];then
#	    updateOnly="$updateOnly --useR9weight"
#	fi
#
#	echo "configFile for step6 is " ${configFile}
#	cp $configFile $outDirData/step6${extension}/`basename $configFile`
#	data_tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [d][1-9]`
#	conf_step6=$outDirData/step6${extension}/`basename $configFile`
#	echo "ConfigFile for step6 is " $outDirData/step6${extension}/`basename $configFile`
#	is_scale_cat= `grep scaleEle_HggRunEtaR9_ $conf_step6`
#	if [ "${is_scale_cat}" == "" ]; then # you don't want to write the scale cat several times
#	    for tag in $data_tags; do
#		echo -e "${tag}\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" >> $outDirData/step6${extension}/`basename $configFile`
#	    done
#	fi
#
#	echo "config File used for jobs in step6 is " $outDirData/step6${extension}/`basename $configFile`
#
#	for index in `seq 1 50`
#	  do
#	  mkdir ${outDirData}/step6${extension}/${index}/fitres/ -p 
#	  mkdir ${outDirData}/step6${extension}/${index}/img -p 
#	done
#
##like step5 but minimizing (not profile only)
#	bsub -q cmscaf1nd \
#	    -oo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
#	    -eo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
#	    -J "${basenameEB} step6${extension}[1-50]" \
#	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
#./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step6${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"
#
#	bsub -q cmscaf1nd \
#	    -oo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
#	    -eo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
#	    -J "${basenameEE} step6${extension}[1-50]" \
#	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
#./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step6${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"
#
#    
#    while [ "`bjobs -J \"${basenameEB} step6${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#    while [ "`bjobs -J \"${basenameEE} step6${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#
#    ./script/haddTGraph.sh -o ${outDirData}/step6${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step6${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
#    ./script/haddTGraph.sh -o ${outDirData}/step6${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step6${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root
#
#    
#	######################################################33
#    echo "{" > tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step6${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step6${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#    echo "}" >> tmp/fitProfiles.C
#    root -l -b -q tmp/fitProfiles.C
#
#    fi #all of this is done only if ${outFile} doesn't exist
#
#    echo "InitFile is "${initFile}
#    echo "Fit results of step6 in " ${outDirData}/step6${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config
#    cat ${outDirData}/step6${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config > ${outFile}
#    ##it was commented before
#    grep -v absEta_0_1 ${outDirData}/step6${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config >> ${outFile}
#
#    cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}
#    echo "outFile of step6 is " ${outFile} " (this will be the initFile of the plots)"
#
#    if [[ $scenario = "plot_only" ]] || [[ $scenario = "" ]]; then
#	./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/img/ --outDirFitResData=${outDirData}/step6${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  || exit 1
#	./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/img/ --outDirFitResData=${outDirData}/step6${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  || exit 1
#    fi
#
#    if [[ $scenario = "write_corr" ]] || [[ $scenario = "" ]]; then
#	grep scale ${outFile} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | sed 's|\(Et_[0-9]*_[0-9]*\)-\([^ \t]*\)|\2 \1 |' > tmp/res_corr_step6.dat
#	outFileStep6=${outDirTable}/scale_step6${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#	awk -f awk/prodScaleCorrSteps.awk tmp/res_corr_step6.dat ${outDirTable}/${outFileStep2} > ${outFileStep6}
#	#save root files with step1 corrections
#	echo ${regionFile}
#    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --corrEleType HggRunEtaR9Et --corrEleFile ${outFileStep6} || exit 1
#    mv tmp/scaleEle_HggRunEtaR9Et_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step6/    
#    fi
#
#    if [[ $scenario = "finalize_step6" ]] || [[ $scenario = "" ]]; then
#	if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6" ];then 
#	    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6 is being created"
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6/ -p
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6/DataMC/ -p
#	fi
#
#	cp test/dato/${file}/${selection}/${invMass_var}/step6/img/outProfile-*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6/
#	cp ${outFile} ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6/
#	./script/plot_histos_validation.sh test/dato/${file}/${selection}/${invMass_var}/step6/fitres/histos-${basenameEB}-${commonCut}.root
#	cp test/dato/${file}/${selection}/${invMass_var}/step6/./img/histos-* ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step6/DataMC/
#    fi
#
#fi

#if [ -n "${STEP7}" ];then
#    echo "Inside step 7"
#    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
#	newSelection=${extension}
#    else
#	newSelection=${selection}
#    fi
#    #eta x Et with smearing method (use step5 quick likelihood scan as initialization)
#    regionFileEB=${regionFileStep5EB}
#    regionFileEE=${regionFileStep5EE}
#    basenameEB=`basename $regionFileEB .dat`
#    basenameEE=`basename $regionFileEE .dat`
#    outFile=${outDirTable}/step7${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#
#    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
#	echo "[ERROR] Impossible to run step7 without step2" >> /dev/stderr
#	exit 1
#    fi
#    
#    echo "initFile for step7 built from step6"
#    if [ -e "${outDirTable}/params-step7-${commonCut}.txt" ];then 
#	initFile="--initFile=${outDirTable}/params-step7-${commonCut}.txt"; 
#    else 
#	echo "[INFO] initFile for step7 from step6 results: ${outDirTable}/step6${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat"
#	cp ${outDirTable}/step6${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat ${outDirTable}/params-step7-${commonCut}.txt
#	initFile="--initFile=${outDirTable}/params-step7-${commonCut}.txt"; 
#    fi
#
#    ##Ranges of the parameters need to be adjusted
#    #In step7 Et scale corrections are loaded, hence init for scale is 1 (in principle)
#    #grep scale ${outDirTable}/params-step7-${commonCut}.txt | sed "s|L(.*)|L(0.98 - 1.02)|g" > tmp/temp.txt #Reduce range at second iteration
#    grep constTerm ${outDirTable}/params-step7-${commonCut}.txt | sed "s|L(.*)|L(0. - 0.03)|g" > tmp/temp.txt #Reduce range at second iteration
#    grep alpha ${outDirTable}/params-step7-${commonCut}.txt >> tmp/temp.txt 
#    mv tmp/temp.txt ${outDirTable}/params-step7-${commonCut}.txt
#
#    echo "job submission in step7 is done if ${outFile} does NOT exist"
#    if [ ! -e "${outFile}" ];then
#
#	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
#	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
#	if [ ! -e "${outDirData}/step7${extension}/fitres" ];then mkdir ${outDirData}/step7${extension}/fitres -p; fi
#	if [ ! -e "${outDirData}/step7${extension}/img" ];then    mkdir ${outDirData}/step7${extension}/img -p; fi
#	
#	
#	if [ "${extension}" == "weight" ];then
#	    updateOnly="$updateOnly --useR9weight"
#	fi
#
#	echo "configFile for step7 is " ${configFile}
#	echo ${configFile}
#	#you have to re-catogorize => remove smearerCat. Actually you don't care about MC, so re-write it
#	grep -v smearerCat_${basenameEB} ${configFile}> $outDirData/step7${extension}/`basename $configFile`
#	grep -v smearerCat_${basenameEE} $outDirData/step7${extension}/`basename $configFile` > tmp/temp.txt
#	mv tmp/temp.txt $outDirData/step7${extension}/`basename $configFile`
#	MC_tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s][1-9]`
#	for tag in $MC_tags; do
#	    echo -e "${tag}\tsmearerCat_${basenameEB}\tdata/smearerCat/smearerCat_${basenameEB}_${tag}-`basename $configFile .dat`.root" >> $outDirData/step7${extension}/`basename $configFile`
#	    echo -e "${tag}\tsmearerCat_${basenameEE}\tdata/smearerCat/smearerCat_${basenameEE}_${tag}-`basename $configFile .dat`.root" >> $outDirData/step7${extension}/`basename $configFile`
#	done
#
#	#Now write the scale corrections root files
#	data_tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [d][1-9]`
#	conf_step7=$outDirData/step7${extension}/`basename $configFile`
#	echo "ConfigFile for step7 is " $outDirData/step7${extension}/`basename $configFile`
#	#is_scale_cat= `grep scaleEle_HggRunEtaR9_ $conf_step7`
#	is_Et_scale_cat= `grep scaleEle_HggRunEtaR9Et_ $conf_step7` #Note the Et corrections root files that you produced in step6
#	if [ "${is_Et_scale_cat}" == "" ]; then # you don't want to write the scale cat several times
#	    for tag in $data_tags; do
#		#The root files with the Et corrections are saved in step6
#		echo -e "${tag}\tscaleEle_HggRunEtaR9Et\t${outDirData}/step6/scaleEle_HggRunEtaR9Et_${tag}-`basename $configFile .dat`.root" >> $outDirData/step7${extension}/`basename $configFile`
#	    done
#	fi
#
#	#[INFO]: this is important: you need to categorize again (n data) AFTER applying the first set of Et scale calibration @ step6 => allow category migration
#	#mkSmearerCatData also writes the cat in the dat file
#        mkSmearerCatData   $regionFileEB ${outDirData}/step7 $outDirData/step7${extension}/`basename $configFile` --corrEleType=HggRunEtaR9Et
#	mkSmearerCatData   $regionFileEE ${outDirData}/step7 $outDirData/step7${extension}/`basename $configFile` --corrEleType=HggRunEtaR9Et
#	echo "config File used for jobs in step7 is " $outDirData/step7${extension}/`basename $configFile`
#
#	for index in `seq 1 50`
#	  do
#	  mkdir ${outDirData}/step7${extension}/${index}/fitres/ -p 
#	  mkdir ${outDirData}/step7${extension}/${index}/img -p 
#	done
#
#        #apply Et corrections found in step6 e minimize again
#	bsub -q cmscaf1nd \
#	    -oo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
#	    -eo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
#	    -J "${basenameEB} step7${extension}[1-50]" \
#	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
#./bin/ZFitter.exe -f $outDirData/step7${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9Et || exit 1; touch ${outDirData}/step7${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"
#
#	bsub -q cmscaf1nd \
#	    -oo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
#	    -eo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
#	    -J "${basenameEE} step7${extension}[1-50]" \
#	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
#./bin/ZFitter.exe -f $outDirData/step7${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9Et || exit 1; touch ${outDirData}/step7${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"
#
#    
#    while [ "`bjobs -J \"${basenameEB} step7${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#    while [ "`bjobs -J \"${basenameEE} step7${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#
#    echo "    ./script/haddTGraph.sh -o ${outDirData}/step7${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step7${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root"
#    ./script/haddTGraph.sh -o ${outDirData}/step7${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step7${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
#    ./script/haddTGraph.sh -o ${outDirData}/step7${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step7${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root
#
#	######################################################33
#    echo "{" > tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step7${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step7${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#    echo "}" >> tmp/fitProfiles.C
#    root -l -b -q tmp/fitProfiles.C
#    fi #all of this is done only if ${outFile} doesn't exist
#
#    echo "InitFile is "${initFile}
#    echo "Fit results of step7 in " ${outDirData}/step7${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config
#    cat ${outDirData}/step7${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config > ${outFile}
#    ##it was commented before
#    grep -v absEta_0_1 ${outDirData}/step7${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config >> ${outFile}
#
#    cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}
#    echo "outFile of step7 is " ${outFile} " (this will be the initFile of the plots)"
#
#    if [[ $scenario = "plot_only" ]] || [[ $scenario = "" ]]; then
#	./bin/ZFitter.exe -f $outDirData/step7${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step7${extension}/img/ --outDirFitResData=${outDirData}/step7${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9Et --plotOnly  || exit 1
#	./bin/ZFitter.exe -f $outDirData/step7${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step7${extension}/img/ --outDirFitResData=${outDirData}/step7${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9Et --plotOnly  || exit 1
#    fi
#
#    if [[ $scenario = "write_corr" ]] || [[ $scenario = "" ]]; then
#	#In the previous step you multiplied step6 * step2. step2 had no Et structure then the category name in res_corr_step6 was splitted in 2 parts: 
#	#lowEtaxxx Et_20_25 1.0002 0.004 (4 rows)
#	#using awk/prodScaleCorrSteps.awk you got outFileStep6 which has Et structure unlike outFilestep2
#	
#	#Now you want to multiply step7 * outFileStep6 => you don't have to split the category name in 2 rows (one with eta-r9 and the other with Et) 
#	#On the other hand you want to have the correct order eta-R9-Et and so you use sed to swap the category name components (\2-\1)
#	#In fact the category name is Et_20_25-lowEta-lowR9 ===> lowEta-lowR9-Et_20_25
#
#	grep scale ${outFile} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | sed 's|\(Et_[0-9]*_[0-9]*\)-\([^ \t]*\)|\2-\1 |' > tmp/res_corr_step7.dat
#	outFileStep7=${outDirTable}/scale_step7${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#	outFileStep6=${outDirTable}/scale_step6${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#	echo $outFileStep7
#	awk -f awk/prodScaleCorrSteps.awk tmp/res_corr_step7.dat ${outFileStep6} > ${outFileStep7}
#	#save root files with step1 corrections
#    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --corrEleType HggRunEtaR9Et --corrEleFile ${outFileStep7} || exit 1
#    mv tmp/scaleEle_HggRunEtaR9Et_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step7/    
#    fi
#
#    if [[ $scenario = "finalize_step7" ]] || [[ $scenario = "" ]]; then
#	if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7" ];then 
#	    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7 is being created"
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7/ -p
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7/DataMC/ -p
#	fi
#
#	cp test/dato/${file}/${selection}/${invMass_var}/step7/img/outProfile-*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7/
#	cp ${outFile} ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7/
#	./script/plot_histos_validation.sh test/dato/${file}/${selection}/${invMass_var}/step7/fitres/histos-${basenameEB}-${commonCut}.root
#	cp test/dato/${file}/${selection}/${invMass_var}/step7/./img/histos-* ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step7/DataMC/
#    fi
#
#fi
#
###Copied from step7
##STEP7 ->STEP8
##step7 -> step8
##step6 -> step7
#if [ -n "${STEP8}" ];then
#    echo "Inside step 8"
#    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
#	newSelection=${extension}
#    else
#	newSelection=${selection}
#    fi
#    #eta x Et with smearing method (use step5 quick likelihood scan as initialization)
#    regionFileEB=${regionFileStep5EB}
#    regionFileEE=${regionFileStep5EE}
#    basenameEB=`basename $regionFileEB .dat`
#    basenameEE=`basename $regionFileEE .dat`
#    outFile=${outDirTable}/step8${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#
#    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
#	echo "[ERROR] Impossible to run step8 without step2" >> /dev/stderr
#	exit 1
#    fi
#    
#    echo "initFile for step8 built from step7"
#    if [ -e "${outDirTable}/params-step8-${commonCut}.txt" ];then 
#	initFile="--initFile=${outDirTable}/params-step8-${commonCut}.txt"; 
#    else 
#	echo "[INFO] initFile for step8 from step7 results: ${outDirTable}/step7${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat"
#	cp ${outDirTable}/step7${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat ${outDirTable}/params-step8-${commonCut}.txt
#	initFile="--initFile=${outDirTable}/params-step8-${commonCut}.txt"; 
#    fi
#
#    ##Ranges of the parameters need to be adjusted
#    #In step8 Et scale corrections are loaded, hence init for scale is 1 (in principle)
#    #grep scale ${outDirTable}/params-step8-${commonCut}.txt | sed "s|L(.*)|L(0.98 - 1.02)|g" > tmp/temp.txt #Reduce range at second iteration
#    grep constTerm ${outDirTable}/params-step8-${commonCut}.txt | sed "s|L(.*)|L(0. - 0.03)|g" > tmp/temp.txt #Reduce range at second iteration
#    grep alpha ${outDirTable}/params-step8-${commonCut}.txt >> tmp/temp.txt 
#    mv tmp/temp.txt ${outDirTable}/params-step8-${commonCut}.txt
#
#    echo "job submission in step8 is done if ${outFile} does NOT exist"
#    if [ ! -e "${outFile}" ];then
#
#	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
#	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
#	if [ ! -e "${outDirData}/step8${extension}/fitres" ];then mkdir ${outDirData}/step8${extension}/fitres -p; fi
#	if [ ! -e "${outDirData}/step8${extension}/img" ];then    mkdir ${outDirData}/step8${extension}/img -p; fi
#	
#	
#	if [ "${extension}" == "weight" ];then
#	    updateOnly="$updateOnly --useR9weight"
#	fi
#
#	echo "configFile for step8 is " ${configFile}
#	echo ${configFile}
#	#you have to re-catogorize => remove smearerCat. Actually you don't care about MC, so re-write it
#	grep -v smearerCat_${basenameEB} ${configFile}> $outDirData/step8${extension}/`basename $configFile`
#	grep -v smearerCat_${basenameEE} $outDirData/step8${extension}/`basename $configFile` > tmp/temp.txt
#	mv tmp/temp.txt $outDirData/step8${extension}/`basename $configFile`
#	MC_tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s][1-9]`
#	for tag in $MC_tags; do
#	    echo -e "${tag}\tsmearerCat_${basenameEB}\tdata/smearerCat/smearerCat_${basenameEB}_${tag}-`basename $configFile .dat`.root" >> $outDirData/step8${extension}/`basename $configFile`
#	    echo -e "${tag}\tsmearerCat_${basenameEE}\tdata/smearerCat/smearerCat_${basenameEE}_${tag}-`basename $configFile .dat`.root" >> $outDirData/step8${extension}/`basename $configFile`
#	done
#
#	#Now write the scale corrections root files
#	data_tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [d][1-9]`
#	conf_step8=$outDirData/step8${extension}/`basename $configFile`
#	echo "ConfigFile for step8 is " $outDirData/step8${extension}/`basename $configFile`
#	#is_scale_cat= `grep scaleEle_HggRunEtaR9_ $conf_step8`
#	is_Et_scale_cat= `grep scaleEle_HggRunEtaR9Et_ $conf_step8` #Note the Et corrections root files that you produced in step7
#	if [ "${is_Et_scale_cat}" == "" ]; then # you don't want to write the scale cat several times
#	    for tag in $data_tags; do
#		#The root files with the Et corrections are saved in step7
#		echo -e "${tag}\tscaleEle_HggRunEtaR9Et\t${outDirData}/step7/scaleEle_HggRunEtaR9Et_${tag}-`basename $configFile .dat`.root" >> $outDirData/step8${extension}/`basename $configFile`
#	    done
#	fi
#
#	#[INFO]: this is important: you need to categorize again (n data) AFTER applying the first set of Et scale calibration @ step7 => allow category migration
#	#mkSmearerCatData also writes the cat in the dat file
#        mkSmearerCatData   $regionFileEB ${outDirData}/step8 $outDirData/step8${extension}/`basename $configFile` --corrEleType=HggRunEtaR9Et
#	mkSmearerCatData   $regionFileEE ${outDirData}/step8 $outDirData/step8${extension}/`basename $configFile` --corrEleType=HggRunEtaR9Et
#	echo "config File used for jobs in step8 is " $outDirData/step8${extension}/`basename $configFile`
#
#	for index in `seq 1 50`
#	  do
#	  mkdir ${outDirData}/step8${extension}/${index}/fitres/ -p 
#	  mkdir ${outDirData}/step8${extension}/${index}/img -p 
#	done
#
#        #apply Et corrections found in step7 e minimize again
#	bsub -q cmscaf1nd \
#	    -oo ${outDirData}/step8${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
#	    -eo ${outDirData}/step8${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
#	    -J "${basenameEB} step8${extension}[1-50]" \
#	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
#./bin/ZFitter.exe -f $outDirData/step8${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step8${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step8${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9Et || exit 1; touch ${outDirData}/step8${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"
#
#	bsub -q cmscaf1nd \
#	    -oo ${outDirData}/step8${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
#	    -eo ${outDirData}/step8${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
#	    -J "${basenameEE} step8${extension}[1-50]" \
#	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
#./bin/ZFitter.exe -f $outDirData/step8${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step8${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step8${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9Et || exit 1; touch ${outDirData}/step8${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"
#
#    
#    while [ "`bjobs -J \"${basenameEB} step8${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#    while [ "`bjobs -J \"${basenameEE} step8${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#
#    echo "    ./script/haddTGraph.sh -o ${outDirData}/step8${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step8${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root"
#    ./script/haddTGraph.sh -o ${outDirData}/step8${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step8${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
#    ./script/haddTGraph.sh -o ${outDirData}/step8${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step8${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root
#
#	######################################################33
#    echo "{" > tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step8${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step8${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
#    echo "}" >> tmp/fitProfiles.C
#    root -l -b -q tmp/fitProfiles.C
#    fi #all of this is done only if ${outFile} doesn't exist
#
#    echo "InitFile is "${initFile}
#    echo "Fit results of step8 in " ${outDirData}/step8${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config
#    cat ${outDirData}/step8${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config > ${outFile}
#    ##it was commented before
#    grep -v absEta_0_1 ${outDirData}/step8${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult-.config >> ${outFile}
#
#    cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}
#    echo "outFile of step8 is " ${outFile} " (this will be the initFile of the plots)"
#
#    if [[ $scenario = "plot_only" ]] || [[ $scenario = "" ]]; then
#	./bin/ZFitter.exe -f $outDirData/step8${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step8${extension}/img/ --outDirFitResData=${outDirData}/step8${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9Et --plotOnly  || exit 1
#	./bin/ZFitter.exe -f $outDirData/step8${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step8${extension}/img/ --outDirFitResData=${outDirData}/step8${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9Et --plotOnly  || exit 1
#    fi
#
#    if [[ $scenario = "write_corr" ]] || [[ $scenario = "" ]]; then
#	grep scale ${outFile} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | sed 's|\(Et_[0-9]*_[0-9]*\)-\([^ \t]*\)|\2 \1 |' > tmp/res_corr_step8.dat
#	outFileStep8=${outDirTable}/scale_step8${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#	outFileStep7=${outDirTable}/scale_step7${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9Et.dat
#	echo $outFileStep8
#	awk -f awk/prodScaleCorrSteps.awk tmp/res_corr_step8.dat ${outFileStep7} > ${outFileStep8}
#	#save root files with step1 corrections
#    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile} --saveRootMacro --corrEleType HggRunEtaR9Et --corrEleFile ${outFileStep8} || exit 1
#    mv tmp/scaleEle_HggRunEtaR9Et_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step8/    
#    fi
#
#    if [[ $scenario = "finalize_step8" ]] || [[ $scenario = "" ]]; then
#	if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8" ];then 
#	    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8 is being created"
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8/ -p
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8/DataMC/ -p
#	fi
#
#	cp test/dato/${file}/${selection}/${invMass_var}/step8/img/outProfile-*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8/
#	cp ${outFile} ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8/
#	./script/plot_histos_validation.sh test/dato/${file}/${selection}/${invMass_var}/step8/fitres/histos-${basenameEB}-${commonCut}.root
#	cp test/dato/${file}/${selection}/${invMass_var}/step8/./img/histos-* ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step8/DataMC/
#    fi
#fi #it closes step8
#
#############################################################
#if [ -n "${GAINSWITCH}" ];then
#    
#    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
#	newSelection=${extension}
#    else
#	newSelection=${selection}
#    fi
#
#    case ${extension} in
#	gainSwitch) regionFileEB=./data/regions/gainSwitch.dat;;
#	gainSwitch2) regionFileEB=./data/regions/gainSwitch_2.dat;;
#	gainSwitch3) regionFileEB=./data/regions/gainSwitch_3.dat;;
#	gainSwitch4) regionFileEB=./data/regions/gainSwitch_4.dat;;
#	gainSwitch5) regionFileEB=./data/regions/gainSwitch_5.dat;;
#	gainSwitch6) regionFileEB=./data/regions/gainSwitch_6.dat;;
#	gainSwitch7) regionFileEB=./data/regions/gainSwitch_7.dat;;
#	gainSwitchEne) regionFileEB=./data/regions/gainSwitch_Ene.dat;;
#	gainSwitchSeedEne) regionFileEB=./data/regions/gainSwitch_seedEne.dat;;
#    esac
#
#    basenameEB=`basename $regionFileEB .dat`
#
#    outFile=${outDirTable}/step9${extension}-${invMass_var}-${newSelection}-${commonCut}-gainSwitch.dat
#
#    #check if the output file of step8 is present
#    #checkStepDep step8
#
#    if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
#    if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
#    if [ ! -e "${outDirData}/step9${extension}/fitres" ];then mkdir ${outDirData}/step9${extension}/fitres -p; fi
#    if [ ! -e "${outDirData}/step9${extension}/img" ];then    mkdir ${outDirData}/step9${extension}/img -p; fi
#    if [ ! -e "${outDirData}/step9/fitres" ];then mkdir ${outDirData}/step9/fitres -p; fi
#    if [ ! -e "${outDirData}/step9/img" ];then    mkdir ${outDirData}/step9/img -p; fi
#    
#    #Now write the scale corrections root files
#    # Here I am assuming that the dat file has already the scale category in place (consider to change this) 
#    cp $configFile $outDirData/step9${extension}/`basename $configFile`
#    echo $outDirData/step9${extension}/`basename $configFile`
#
#    echo "***************************" ${outFile}
#    #if [ ! -e "${outFile}" -o -n "$FORCE" ];then
#    #if [[ $scenario = "job_gainSwitch" ]]; then
#    if [[ $scenario = "job_gainSwitch" ]] || [[ $scenario = "" ]]; then
#        #Now categorize for the gain switch study
#        mkSmearerCatData   $regionFileEB ${outDirData}/step9${extension} $outDirData/step9${extension}/`basename $configFile` --corrEleType=HggRunEtaR9
#	mkSmearerCatSignal $regionFileEB $outDirData/step9${extension}/`basename $configFile`
#	#Even without pre-applying scale corr (only in case you know what you are doing)
#	#mkSmearerCatData   $regionFileEB ${outDirData}/step9 $outDirData/step9${extension}/`basename $configFile`
#
#	echo "************Sending jobs"
#	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
#	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
#	if [ ! -e "${outDirData}/step9${extension}/fitres" ];then mkdir ${outDirData}/step9${extension}/fitres -p; fi
#	if [ ! -e "${outDirData}/step9${extension}/img" ];then    mkdir ${outDirData}/step9${extension}/img -p; fi
#       	
#	if [ "${extension}" == "weight" ];then
#	    updateOnly="$updateOnly --useR9weight"
#	fi
#
#	for index in `seq 1 50`
#	  do
#	  mkdir ${outDirData}/step9${extension}/${index}/fitres/ -p 
#	  mkdir ${outDirData}/step9${extension}/${index}/img -p 
#	done
#
#   	./bin/ZFitter.exe -f $outDirData/step9${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step9${extension}/img/ --outDirFitResData=${outDirData}/step9${extension}/fitres  ${Et_smear} --autoNsmear ${initFile} --corrEleType=HggRunEtaR9 --smearerFit ${smearRange} --nEventsMinDiag=150 --nEventsMinOffDiag=150 --initFile=initGain.dat --plotOnly | tee ${outDirData}/step9${extension}/fitres/debug_fit.dat;# --autoBin --constTermFix $updateOnly --onlyScale;
#
#	#exit 0
#	#bsub -q 2nd \
#	 #   -oo ${outDirData}/step9${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
#	  #  -eo ${outDirData}/step9${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
#	   # -J "${basenameEB} step9${extension}[1-50]" \
#	    #"cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
##./bin/ZFitter.exe -f $outDirData/step9${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step9${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step9${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9  --onlyScale || exit 1;"    
##	while [ "`bjobs -J \"${basenameEB} step9${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#    #./script/haddTGraph.sh -o ${outDirData}/step9${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step9${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
##fi
#    fi #sending job closure
#
#    if [[ $scenario = "finalize_gain" ]] || [[ $scenario = "" ]]; then
#	######################################################
#    echo "{" > tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
#    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
#    echo "FitProfile2(\"${outDirData}/step9${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles.C
#    echo "}" >> tmp/fitProfiles.C
#    root -l -b -q tmp/fitProfiles.C
#
#    cp ${outDirData}/step9${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult-.config ${outFile}
#    #cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}
#
#
#	if [ ! -d "${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9${extension}" ];then 
#	    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9_gain is being created"
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9${extension}/ -p
#	    www_mkdir ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9${extension}/DataMC/ -p
#	fi
#	echo "copying the likelihood"
#	mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step9${extension}/img/outProfile-*.png ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9${extension}/
#	mv ${outFile} ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9${extension}/
#	./script/plot_histos_validation.sh ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step9${extension}/fitres/histos-${basenameEB}-${commonCut}.root
#	mv ${tmp_path}/test/dato/${file}/${selection}/${invMass_var}/step9${extension}/./img/histos-* ${eos_path}/www/RUN2_ECAL_Calibration/${file}/${invMass_var}/step9${extension}/DataMC/
#    fi
#    
#    #echo "[STATUS] Making data/MC plots"
#     #./bin/ZFitter.exe -f $outDirData/step9${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step9${extension}/img/ --outDirFitResData=${outDirData}/step9${extension}/fitres --constTermFix  --smearerFit ${smearRange}  ${Et_smear} --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9Et --plotOnly  || exit 1
#
#fi
