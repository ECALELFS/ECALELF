#!/bin/bash
commonCut=Et_25-trigger-noPF
selection=loose
invMass_var=invMass_SC_regrCorr_pho
baseDir=test
updateOnly="--updateOnly" # --profileOnly --initFile=init.txt"
#updateOnly=""

usage(){
    echo "`basename $0` [options]" 
    echo " --fileEB arg"
    echo " --fileEE arg"
    echo " --fileStep1 arg"
    echo " --outFile arg"
#     echo " --runRangesFile arg           "
#     echo " --selection arg (=$selection)"
#     echo " --invMass_var arg"
    echo " --commonCut arg (=$commonCut)"
#    echo " --step arg: 1, 2, 2fit, 3, 3weight, slide, 3stability, syst, 1-2,1-3,1-3stability and all ordered combination"
}
#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:s: -l help,fileEB:,fileEE:,fileStep1:,outFile:,selection:,invMass_var:,step:,baseDir:,commonCut: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	--fileEB) file1=$2; shift;;
	--fileEE) file2=$2; shift;;
	--fileStep1) file3=$2; shift;;
	--outFile) outFile=$2; shift;;
# 	-s|--step) STEP=$2; shift;;
# 	--invMass_var) invMass_var=$2; echo "[OPTION] invMass_var = ${invMass_var}"; shift;;
# 	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;; 
# 	--selection) selection=$2; echo "[OPTION] selection = ${selection}"; shift;;
# 	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--commonCut) commonCut=$2; echo "[OPTION] commonCut = $commonCut"; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


if [ -z "${file1}" ];then
    echo "[ERROR] fileEB not provided" >> /dev/stderr
    exit 1
fi

if [ -z "${file2}" ];then
    echo "[ERROR] fileEE not provided" >> /dev/stderr
    exit 1
fi

if [ -z "${outFile}" ];then
    echo "[ERROR] outFile not provided" >> /dev/stderr
    exit 1
fi

if [ -z "${file3}" ];then
    echo "[ERROR] fileStep1 not provided" >> /dev/stderr
    exit 1
fi


grep scale ${file1} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' > tmp/res_corr.dat
grep scale ${file2} | grep EE | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' >> tmp/res_corr.dat


./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat --file1=${file3} |grep -v '#' > ${outFile}.dat
./script/makeCorrVsRunNumber.py -l -c --file2=tmp/res_corr.dat --file1=${file3} |sed -f sed/tex.sed > ${outFile}.tex

exit 0
#!/bin/bash
commonCut="Et_20"
file1=$1
file2=$2
file3=$3
outFile=$4

grep scale ${file1} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' > tmp/res_corr.dat
grep scale ${file2} | grep EE | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' >> tmp/res_corr.dat


./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat --file1=${file3} |grep -v '#' > ${outFile}.dat
./script/makeCorrVsRunNumber.py -l -c --file2=tmp/res_corr.dat --file1=${file3} |sed -f sed/tex.sed > ${outFile}.tex
