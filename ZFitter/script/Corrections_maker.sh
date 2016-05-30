#! /bin/bash
source script/functions.sh
source script/bash_functions_calibration.sh

##file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
file=`basename $1 .dat`
if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi

##Load Z correction
invMass_type=$2
####76 corrections####
scale_file=/afs/cern.ch/user/g/gfasanel/scratch1/CMSSW_7_4_15/src/Calibration/ZFitter/test/dato/February2016_Rereco_76/loose/invMass_SC_pho_regrCorr/table/step2-invMass_SC_pho_regrCorr-loose-Et_20-noPF-HggRunEtaR9.dat
smear_file=/afs/cern.ch/user/g/gfasanel/scratch1/CMSSW_7_4_15/src/Calibration/ZFitter/test/dato/February2016_Rereco_76/loose/invMass_SC_pho_regrCorr/table/outFile-step4-invMass_SC_pho_regrCorr-loose-Et_20-noPF-HggRunEtaR9-smearEle.dat
##smear_file=smear_ele_corrections/smear_RUN2_RUN1like.dat
#scale_file=0T_corrections/0T_scale.dat
#smear_file=0T_corrections/0T_smearings.dat
####76 corrections####
scale_corr_name=$3
smear_corr_name=$4


#smear_corr_name=Zcorr_RUN1

echo -e "\e[0;31m scale correction file is \e[0m ${scale_file}"
echo -e "\e[0;31m smear correction file is \e[0m ${smear_file}"

if [ "$3" != "no_scale_corr" ]; then  
    echo "Creating scale corrections"
    ./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat invMass_var=${invMass_type} --saveRootMacro --corrEleType=${scale_corr_name} --corrEleFile=${scale_file} 
    mv tmp/scaleEle_${scale_corr_name}_d*-${file}.root friends/others/
    for tag in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
    do  
	echo "${tag} scaleEle_${scale_corr_name} friends/others/scaleEle_${scale_corr_name}_${tag}-${file}.root" >> data/validation/${file}.dat 
    done
fi
if [ "$4" != "no_smear_corr" ]; then 
    echo "Creating smear corrections" 
    ./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat invMass_var=${invMass_type} --saveRootMacro --smearEleType=${smear_corr_name} --smearEleFile=${smear_file}
    mv tmp/smearEle_${smear_corr_name}_s*-${file}.root friends/others/
    for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
    do
	echo "${tag} smearEle_${smear_corr_name} friends/others/smearEle_${smear_corr_name}_${tag}-${file}.root" >> data/validation/${file}.dat 
    done
fi





