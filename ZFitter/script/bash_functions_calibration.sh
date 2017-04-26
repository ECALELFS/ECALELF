#! /bin/bash
shopt -s expand_aliases
source ~/.bashrc

eos_path=/eos/project/c/cms-ecal-calibration
www_mkdir(){
if [[ $1 = "" ]]; then echo "you should specify a directory"; else mkdir -p $1; cp ${eos_path}/www/index.php $1; fi
}

#1)Make pileupHist
pileupHist(){
script/pileup_histoMaker.sh $file
}

pileupTrees(){
#region name is there just because it's mandatory for ZFitter
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat --saveRootMacro 

#pileUPtree
for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    #tag is s1, s2, ...
    mv tmp/mcPUtree${tag}.root ${eos_path}/data/puTree/${tag}_${file}.root
    #mv tmp/${tag}_pileup_chain.root  ${eos_path}/data/puTree/${tag}_${file}.root #this does not work
    echo "${tag} pileup ${eos_path}/data/puTree/${tag}_${file}.root" >> data/validation/${file}.dat 
done
}

addBranch(){
	sORds=
	branchnames=
	addbranch=
	while [[ "$1" != "--" ]] && [[ "$1" != "" ]]
	do
		sORds="$sORds $1"
		branchnames="$branchnames $2"
		if [[ $2 != "pileup" ]]; then addbranch="$addbranch --addBranch $2"
		fi
		echo $1 $2 $addbranch
		shift 2
	done
	shift
	options="$@"
	echo $options
	#region name is there just because it's mandatory for ZFitter
	echo ./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat $addbranch $options
	./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat $addbranch $options

	branchnames=($branchnames)
	sORds=($sORds)
	for i in $(seq 0 $(( ${#branchnames[@]} - 1 )) )
	do
		branchname=${branchnames[$i]}
		sORd=${sORds[$i]}
		echo $sORd $branchname
		#handle pileup slightly differently
		if [[ $branchname == "pileup" ]]
		then
			for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
			do
				#tag is s1, s2, ...
				mv tmp/mcPUtree${tag}.root ${eos_path}/data/puTree/${tag}_${file}.root &&
					echo "${tag} pileup ${eos_path}/data/puTree/${tag}_${file}.root" >> data/validation/${file}.dat 
			done
		else
			#all EleIDSF branches go in EleIDSF/
			branchname_dir=$branchname
			if [[ "$branchname" == *"EleIDSF"* ]]
			then
				branchname_dir=EleIDSF
			fi
			for tag in `grep "^[${sORd}]" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
			do
				#tag is s1, s2, ...
				tagfile=${branchname}_${tag}-${file}.root
				mv tmp/${tagfile} ${eos_path}/data/${branchname_dir}/ &&
					echo "${tag} $branchname ${eos_path}/data/$branchname_dir/${tagfile}" >> data/validation/${file}.dat ||
					echo "[ERROR] moving tmp/${tagfile} to ${eos_path}/data/${branchname_dir}/"
			done
		fi
	done
}

Categorize(){
    ./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --addBranch=smearerCat invMass_var=${invMass_type} --saveRootMacro
    mv tmp/smearerCat_${region1}_*${file}* friends/smearerCat/ 
    for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
    do
	echo "${tag} smearerCat_${region1} friends/smearerCat/smearerCat_${region1}_${tag}-${file}.root" >> data/validation/${file}.dat 
    done
    for tag in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
    do  
	echo "${tag} smearerCat_${region1} friends/smearerCat/smearerCat_${region1}_${tag}-${file}.root" >> data/validation/${file}.dat 
    done
    if [[ ${region2} != "" ]];then
	./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --addBranch=smearerCat invMass_var=${invMass_type} --saveRootMacro
	
	mv tmp/smearerCat_${region2}_*${file}* friends/smearerCat/ 
	for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
	do
	    echo "${tag} smearerCat_${region2} friends/smearerCat/smearerCat_${region2}_${tag}-${file}.root" >> data/validation/${file}.dat 
	done
	for tag in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
	do  
	    echo "${tag} smearerCat_${region2} friends/smearerCat/smearerCat_${region2}_${tag}-${file}.root" >> data/validation/${file}.dat 
	done
    fi

}

Just_plot(){
echo "test"
echo "your histograms will appear in RUN2_ECAL_Calibration/${extension}/"
   command_region1="./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} ${eleID} $corrections --plotOnly | tee test/dato/fitres/temp_log_file.dat"
   echo "[COMMAND:] "$command_region1
   eval $command_region1
  if [[ ${region2} != "" ]];then
      command_region2="./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} ${eleID} $corrections --plotOnly| tee test/dato/fitres/temp_log_file_2.dat"
      echo $command_region2
      eval $command_region2
  fi

}

#Un giro di prova
Test_1_job(){ #There is also Test_1_job_stoc 
echo "test"
echo "your test job will appear in RUN2_ECAL_Calibration/${extension}/"
   command_region1="./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} ${eleID} $corrections | tee test/dato/fitres/temp_log_file.dat #--corrEleType=HggRunEta #--corrEleType=Zcorr_76 --initFile=init_test_ICHEP.dat --profileOnly --onlyDiagonal"
   echo "[COMMAND:] "$command_region1
   eval $command_region1
  if [[ ${region2} != "" ]];then
      command_region2="./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} ${eleID} $corrections | tee test/dato/fitres/temp_log_file_2.dat"
      echo $command_region2
      eval $command_region2
  fi

Plot_Test_job
}

Plot_Test_job(){
echo "Plot Test job"

  ./script/fit.sh test/dato/fitres/outProfile-${region1}-${CommonCuts}.root
if [[ ${region2} != "" ]];then
   ./script/fit.sh test/dato/fitres/outProfile-${region2}-${CommonCuts}.root
fi

if [ ! -e "/afs/g/gfasanel/scratch1/www/RUN2_ECAL_Calibration/${extension}/" ];then mkdir ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/ -p; mkdir ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp -p; cp ~/scratch1/www/index.php ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/; cp ~/scratch1/www/index.php ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp; fi
#moving the fits in the web-space
#mv test/dato/img/outProfile-${region1}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp
mv test/dato/img/outProfile-${region1}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp

if [[ ${region2} != "" ]];then
   mv test/dato/img/outProfile-${region2}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp
fi

#Plot_data_MC del test job                                                                                                                                                     
echo "{" > tmp/plotter_data_MC.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C++\");" >> tmp/plotter_data_MC.C
#Nei temp non ci metto le pecette di luminosita' e centro di massa
#PlotMeanHist("test/dato/fitres/stochastic_smearing_applied/histos-scaleStep0-Et_30-noPF.root","13 TeV B=0T","0.6")
echo "PlotMeanHist(\"test/dato/fitres/histos-${region1}-${CommonCuts}.root\");" >> tmp/plotter_data_MC.C
if [[ ${region2} != "" ]];then
    echo "PlotMeanHist(\"test/dato/fitres/histos-${region2}-${CommonCuts}.root\");" >> tmp/plotter_data_MC.C
fi
echo "}" >> tmp/plotter_data_MC.C
root -l -b -q ~/rootlogon.C tmp/plotter_data_MC.C

reducedCommonCuts=$(echo $CommonCuts| sed 's/-noPF//g')
#mv test/dato/img/histos-${region1}-${reducedCommonCuts}* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp
mv test/dato/img/histos-${region1}-${reducedCommonCuts}* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp
if [[ ${region2} != "" ]];then
    mv test/dato/img/histos-${region2}-${reducedCommonCuts}* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp
fi
}

Test_1_job_stoc(){
echo "test stoc"
   command_region1="./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} ${eleID} $corrections"
   echo $command_region1
   eval $command_region1
  if [[ ${region2} != "" ]];then
      command_region2="./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} ${eleID} $corrections"
      echo $command_region2
      eval $command_region2
  fi

  echo "Fitting root file test/dato/fitres/stochastic_smearing_applied/outProfile-${region1}-${CommonCuts}.root"
  ./script/fit.sh test/dato/fitres/stochastic_smearing_applied/outProfile-${region1}-${CommonCuts}.root
  mv test/dato/img/stochastic_smearing_applied/outProfile-${region1}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc

if [[ ${region2} != "" ]];then
   ./script/fit.sh test/dato/fitres/stochastic_smearing_applied/outProfile-${region2}-${CommonCuts}.root
   mv test/dato/img/stochastic_smearing_applied/outProfile-${region2}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc
fi

if [ ! -e "/afs/g/gfasanel/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc" ];then mkdir ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc -p; cp ~/scratch1/www/index.php ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc; fi
#moving the fits in the web-space
mv test/dato/img/stochastic_smearing_applied/outProfile-${region1}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc

if [[ ${region2} != "" ]];then
   mv test/dato/img/stochastic_smearing_applied/outProfile-${region2}-${CommonCuts}-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc
fi

#Plot_data_MC del test job                                                                                                                                                      
echo "{" > tmp/plotter_data_MC.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C++\");" >> tmp/plotter_data_MC.C
#Nei temp non ci metto le pecette di luminosita' e centro di massa
#PlotMeanHist("test/dato/fitres/stochastic_smearing_applied/histos-scaleStep0-Et_30-noPF.root","13 TeV B=0T","0.6")
echo "PlotMeanHist(\"test/dato/fitres/stochastic_smearing_applied/histos-${region1}-${CommonCuts}.root\");" >> tmp/plotter_data_MC.C
if [[ ${region2} != "" ]];then
    echo "PlotMeanHist(\"test/dato/fitres/stochastic_smearing_applied/histos-${region2}-${CommonCuts}.root\");" >> tmp/plotter_data_MC.C
fi
echo "}" >> tmp/plotter_data_MC.C
root -l -b -q ~/rootlogon.C tmp/plotter_data_MC.C
mv test/dato/img/stochastic_smearing_applied/histos-${region1}-${CommonCuts}* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc
if [[ ${region2} != "" ]];then
    mv test/dato/img/stochastic_smearing_applied/histos-${region2}-${CommonCuts}* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc
fi

echo "Plots for stochastic check are in /afs/g/gfasanel/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_stoc"

}

Submit_50_jobs(){
#####Una volta tunati i job di prova, sottometti 50 job
 if [ -e "${outDirData}/${extension}/fitres" ];then echo "${outDirData}/${extension}/fitres already exists: be sure not to mix different job tasks---> EXIT"; exit;fi
 if [ ! -e "${outDirData}/${extension}/fitres" ];then mkdir ${outDirData}/${extension}/fitres -p; fi
 if [ ! -e "${outDirData}/${extension}/img" ];then mkdir ${outDirData}/${extension}/img -p; fi

############# SUBMITTING JOBS ###########################################

 for index in `seq 1 50` #Making directories
 do
     mkdir ${outDirData}/${extension}/${index}/fitres/ -p
     mkdir ${outDirData}/${extension}/${index}/img -p
 done
 
 bsub -q 2nd\
            -oo ${outDirData}/${extension}/%I/fitres/${region1}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${region1}-stderr.log\
            -J "${region1} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters1} --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/;"  

 bsub -q 2nd\
            -oo ${outDirData}/${extension}/%I/fitres/${region2}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${region2}-stderr.log\
            -J "${region2} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type} --commonCut=${CommonCuts} --autoBin --smearerFit ${initParameters2} --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/;"
}
#wait until jobs are finished, then make the mean likelihood and fit
Likelihood_Fitter(){
#check jobs
#while [ "`bjobs -J \"${region1} ${extension}[1-50]\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#while [ "`bjobs -J \"${region2} ${extension}[1-50]\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
###########################Make Likelihood plots###########################
./script/haddTGraph.sh -o ${outDirData}/${extension}/fitres/outProfile-${region1}-${CommonCuts}.root ${outDirData}/${extension}/*/fitres/outProfile-${region1}-${CommonCuts}.root
echo "{" > tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "FitProfile2(\"${outDirData}/${extension}/fitres/outProfile-${region1}-${CommonCuts}.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "}" >> tmp/fitProfiles_${invMass_type}_${extension}.C
if [ ! -e "${outDirData}/${extension}/img" ];then mkdir test/dato/img/${extension}/img -p; fi
root -l -b -q tmp/fitProfiles_${invMass_type}_${extension}.C
echo "Likelihood plots are in ${outDirData}/${extension}/img/"
if [ ! -e "~/scratch1/www/${extension}/" ];then mkdir ~/scratch1/www/${extension}/ -p; cp ~/scratch1/www/index.php ~/scratch1/www/${extension}/; fi
mv ${outDirData}/${extension}/img/*.png ~/scratch1/www/${extension}/
echo "************Find png plots in scratch1/www/${extension}/img/"
##REGION2
./script/haddTGraph.sh -o ${outDirData}/${extension}/fitres/outProfile-${region2}-${CommonCuts}.root ${outDirData}/${extension}/*/fitres/outProfile-${region2}-${CommonCuts}.root
echo "{" > tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "FitProfile2(\"${outDirData}/${extension}/fitres/outProfile-${region2}-${CommonCuts}.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "}" >> tmp/fitProfiles_${invMass_type}_${extension}.C
if [ ! -e "${outDirData}/${extension}/img" ];then mkdir test/dato/img/${extension}/img -p; fi
root -l -b -q tmp/fitProfiles_${invMass_type}_${extension}.C
echo "Likelihood plots are in ${outDirData}/${extension}/img/"
if [ ! -e "~/scratch1/www/${extension}/" ];then mkdir ~/scratch1/www/${extension}/ -p; cp ~/scratch1/www/index.php ~/scratch1/www/${extension}/; fi
mv ${outDirData}/${extension}/img/*.png ~/scratch1/www/${extension}/
echo "************Find png plots in scratch1/www/${extension}/img/"
}

Plot_data_MC(){
if [ ! -e "~/scratch1/www/${extension}/data_MC/" ]; then mkdir ~/scratch1/www/${extension}/data_MC -p; cp ~/scratch1/www/index.php ~/scratch1/www/${extension}/data_MC/; fi
echo "{" > tmp/plotter_data_MC.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/plotter_data_MC.C
echo "PlotMeanHist(\"${outDirData}/${extension}/1/fitres/histos-${region1}-${CommonCuts}.root\");" >> tmp/plotter_data_MC.C
echo "PlotMeanHist(\"${outDirData}/${extension}/1/fitres/histos-${region2}-${CommonCuts}.root\");" >> tmp/plotter_data_MC.C
echo "}" >> tmp/plotter_data_MC.C
root -l -b -q tmp/plotter_data_MC.C
mv ${outDirData}/${extension}/1/./img/histos-${region1}*.png ~/scratch1/www/${extension}/data_MC/
mv ${outDirData}/${extension}/1/./img/histos-${region2}*.png ~/scratch1/www/${extension}/data_MC/
}

Write_down_dat_corr(){
##Scale corrections: per i dati -> 1/scala_MC
grep scale ${outDirData}/${extension}/img/outProfile-${region1}-${CommonCuts}-FitResult-.config |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${CommonCuts}||g"> tmp/corr_MC.dat
grep scale ${outDirData}/${extension}/img/outProfile-${region2}-${CommonCuts}-FitResult-.config |  grep -v absEta_0_1 | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${CommonCuts}||g">> tmp/corr_MC.dat

categories=`grep scale tmp/corr_MC.dat | cut -f 1`
categories=`echo $categories | sed "s/scale_//g"`
categories=(${categories// / }) # array
scales=`grep scale tmp/corr_MC.dat | cut -f 2`
scales=(${scales// / }) # array
scales_data=()
errors=`grep scale tmp/corr_MC.dat | cut -f 3`
errors=(${errors// / }) # array

for scale in "${scales[@]}"
do
scales_data+=(`echo "1./$scale" | bc -l`)
done

if [ -e data_scale/scale_corrections_${file}_${extension}.dat ]; then rm data_scale/scale_corrections_${file}_${extension}.dat; fi
for i in "${!scales_data[@]}"; do 
echo ${categories[$i]} "runNumber 0 999999" ${scales_data[$i]} ${errors[$i]} >> data_scale/scale_corrections_${file}_${extension}.dat
done


grep constTerm ${outDirData}/${extension}/img/outProfile-${region1}-${CommonCuts}-FitResult-.config |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${CommonCuts}||g" >> tmp/corr_MC.dat
grep constTerm ${outDirData}/${extension}/img/outProfile-${region2}-${CommonCuts}-FitResult-.config |  grep -v absEta_0_1 | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-${CommonCuts}||g" >> tmp/corr_MC.dat

smearings=`grep constTerm tmp/corr_MC.dat | cut -f 2`
smearings=(${smearings// / }) # array
errors_smear=`grep constTerm tmp/corr_MC.dat | cut -f 3`
errors_smear=(${errors_smear// / }) # array
if [ -e mc_smear/smearing_corrections_${file}_${extension}.dat ]; then rm mc_smear/smearing_corrections_${file}_${extension}.dat; fi
for i in "${!smearings[@]}"; do 
echo ${categories[$i]} ${smearings[$i]}>> mc_smear/smearing_corrections_${file}_${extension}.dat
done

mv tmp/corr_MC.dat tmp/corr_${file}_${extension}_MC.dat
}

Write_down_root_corr(){
Write_down_dat_corr
#qui, il file di regione non serve a niente, solo a far funzionare lo script che senza non ti fa fare niente
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat invMass_var=${invMass_type} --saveRootMacro --corrEleType=EtaR9_${extension} --corrEleFile=data_scale/scale_corrections_${file}_${extension}.dat 
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat invMass_var=${invMass_type} --saveRootMacro --smearEleType=stochastic_${extension} --smearEleFile=mc_smear/smearing_corrections_${file}_${extension}.dat

mv tmp/scaleEle_EtaR9_${extension}_d*-${file}.root friends/others/
mv tmp/smearEle_stochastic_${extension}_s*-${file}.root friends/others/

for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    echo "${tag} smearEle_stochastic_${extension} friends/others/smearEle_stochastic_${extension}_${tag}-${file}.root" >> data/validation/${file}.dat 
done
for tag in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do  
    echo "${tag} scaleEle_EtaR9_${extension} friends/others/scaleEle_EtaR9_${extension}_${tag}-${file}.root" >> data/validation/${file}.dat 
done
}

closure_test(){
#senza initParameters, ma applicando le correzioni trovate
#Likelihood con minimo a 1 per la scala e zero per lo smearing
#Data/MC plot perfetti
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit --corrEleType=EtaR9_${extension} --smearEleType=stochastic_${extension} --profileOnly --plotOnly > test/dato/fitres/closure_step2_1.dat
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type}  --commonCut=${CommonCuts} --autoBin --smearerFit --corrEleType=EtaR9_${extension} --smearEleType=stochastic_${extension} --profileOnly --plotOnly > test/dato/fitres/closure_step2_2.dat
if [ ! -e "test/dato/${extension}/closure_step2_${file}_${extension}/" ]; then mkdir test/dato/${extension}/closure_step2_${file}_${extension}/; fi
mv test/dato/fitres/closure_step2*.dat test/dato/${extension}/closure_step2_${file}_${extension}/ #output of the closure tests "job"
mv test/dato/fitres/outProfile-${region1}-${CommonCuts}.root test/dato/${extension}/closure_step2_${file}_${extension}/
mv test/dato/fitres/outProfile-${region2}-${CommonCuts}.root test/dato/${extension}/closure_step2_${file}_${extension}/
mv test/dato/fitres/histos-${region1}-${CommonCuts}.root test/dato/${extension}/closure_step2_${file}_${extension}/
mv test/dato/fitres/histos-${region2}-${CommonCuts}.root test/dato/${extension}/closure_step2_${file}_${extension}/
}

Prepare_Table(){
Write_down_dat_corr
echo "\begin{table}[htb]"> tmp/table_${file}_${extension}.tex
echo " \caption{Results for scale and smearing corrections: scale has to be applied to data, smearing to MC.}">> tmp/table_${file}_${extension}.tex
echo " \begin{center}">> tmp/table_${file}_${extension}.tex
echo "   \begin{tabular}{ccc}">> tmp/table_${file}_${extension}.tex
echo "     \hline">> tmp/table_${file}_${extension}.tex
echo "     Category & Scale & Smearing\\\\ \hline">> tmp/table_${file}_${extension}.tex #I want to write \\ -> you have to escape \ 

for i in "${!scales_data[@]}"; do
categories_name[$i]=`echo ${categories[$i]} | sed "s/_/-/g"`
printf "%s %s %.5lf %s %.5lf %s %.4lf %s %.4lf %s\n" "${categories_name[$i]}" "&" "${scales_data[$i]}" "$\pm$" "${errors[$i]}" "&" "${smearings[$i]}" "$\pm$" "${errors_smear[$i]}" "\\\\">> tmp/table_${file}_${extension}.tex
done


echo "   \hline">> tmp/table_${file}_${extension}.tex
echo "    \end{tabular}">> tmp/table_${file}_${extension}.tex
echo "  \end{center}">> tmp/table_${file}_${extension}.tex
echo "\end{table}">> tmp/table_${file}_${extension}.tex

mv tmp/table_${file}_${extension}.tex ${outDirData}/${extension}/fitres/table_${file}_${extension}.tex
}

Prepare_Slide_8_cat_profileOnly(){
Prepare_Table
echo "\documentclass[9pt, xcolor=dvipsnames]{beamer}" >tmp/slide_${file}_${extension}_profileOnly.tex
echo "\usepackage{hyperref}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{document}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
less ${outDirData}/${extension}/fitres/table_${file}_${extension}.tex >>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####SCALE step2_1
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-scale${categories[0]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-scale${categories[1]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[0]} (left) and ${categories_name[1]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-scale${categories[2]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-scale${categories[3]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[2]} (left) and ${categories_name[3]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####SCALE step2_2
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-scale${categories[4]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-scale${categories[5]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[4]} (left) and ${categories_name[5]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-scale${categories[6]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-scale${categories[7]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[6]} (left) and ${categories_name[7]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####constTerm step2_1
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-constTerm${categories[0]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-constTerm${categories[1]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[0]} (left) and ${categories_name[1]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-constTerm${categories[2]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-constTerm${categories[3]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[2]} (left) and ${categories_name[3]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####constTerm step2_2
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-constTerm${categories[4]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-constTerm${categories[5]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[4]} (left) and ${categories_name[5]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-constTerm${categories[6]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-constTerm${categories[7]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[6]} (left) and ${categories_name[7]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
##Sostituire gli _ con \_ per latex
path=$(echo $extension | sed "s|_|\\\_|g")
echo "Find data/MC plots in \href{https://gfasanel.web.cern.ch/gfasanel/${path}/data\_MC/}{https://gfasanel.web.cern.ch/gfasanel/${path}/data\_MC/}">>tmp/slide_${file}_${extension}_profileOnly.tex
#
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\end{document}">>tmp/slide_${file}_${extension}_profileOnly.tex

cd tmp/
pdflatex slide_${file}_${extension}_profileOnly.tex
cd ../

mv tmp/slide_${file}_${extension}_profileOnly.tex ${outDirData}/${extension}/
mv tmp/slide_${file}_${extension}_profileOnly.pdf ${outDirData}/${extension}/
cp ${outDirData}/${extension}/slide_${file}_${extension}_profileOnly.tex ~/scratch1/www/${extension}/
cp ${outDirData}/${extension}/slide_${file}_${extension}_profileOnly.pdf ~/scratch1/www/${extension}/

}

Prepare_Slide_8_cat(){
Prepare_Table
echo "\documentclass[9pt, xcolor=dvipsnames]{beamer}" >tmp/slide_${file}_${extension}.tex
echo "\usepackage{hyperref}">>tmp/slide_${file}_${extension}.tex
echo "\begin{document}">>tmp/slide_${file}_${extension}.tex
echo "\begin{frame}">>tmp/slide_${file}_${extension}.tex
less ${outDirData}/${extension}/fitres/table_${file}_${extension}.tex >>tmp/slide_${file}_${extension}.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}.tex

for index in `seq 0 3`
do
    echo "\begin{frame}">>tmp/slide_${file}_${extension}.tex
    echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}.tex
    echo "\begin{center}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-scale${categories[$index]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-${CommonCuts}-constTerm${categories[$index]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\end{center}">>tmp/slide_${file}_${extension}.tex
    echo "\caption{scale parameter (left) and smearing parameter (right) for ${categories_name[$index]}}">>tmp/slide_${file}_${extension}.tex
    echo "\end{figure}">>tmp/slide_${file}_${extension}.tex
    echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}.tex
    echo "\begin{center}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/data_MC/histos-${region1}-${CommonCuts}_${categories[$index]}-${CommonCuts}${categories[$index]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\end{center}">>tmp/slide_${file}_${extension}.tex
    echo "\caption{Data/MC comparison for ${categories_name[$index]}}">>tmp/slide_${file}_${extension}.tex
    echo "\end{figure}">>tmp/slide_${file}_${extension}.tex
    echo "\end{frame}">>tmp/slide_${file}_${extension}.tex
done

for index in `seq 4 7`
do
    echo "\begin{frame}">>tmp/slide_${file}_${extension}.tex
    echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}.tex
    echo "\begin{center}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-scale${categories[$index]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-${CommonCuts}-constTerm${categories[$index]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\end{center}">>tmp/slide_${file}_${extension}.tex
    echo "\caption{scale parameter (left) and smearing parameter (right) for ${categories_name[$index]}}">>tmp/slide_${file}_${extension}.tex
    echo "\end{figure}">>tmp/slide_${file}_${extension}.tex
    echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}.tex
    echo "\begin{center}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/data_MC/histos-${region2}-${CommonCuts}_${categories[$index]}-${CommonCuts}${categories[$index]}-${CommonCuts}}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\end{center}">>tmp/slide_${file}_${extension}.tex
    echo "\caption{Data/MC comparison for ${categories_name[$index]}}">>tmp/slide_${file}_${extension}.tex
    echo "\end{figure}">>tmp/slide_${file}_${extension}.tex
    echo "\end{frame}">>tmp/slide_${file}_${extension}.tex
done

#echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
###Sostituire gli _ con \_ per latex
#path=$(echo $extension | sed "s|_|\\\_|g")
#echo "Find data/MC plots in \href{https://gfasanel.web.cern.ch/gfasanel/${path}/data\_MC/}{https://gfasanel.web.cern.ch/gfasanel/${path}/data\_MC/}">>tmp/slide_${file}_${extension}.tex
##
#echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
#
echo "\end{document}">>tmp/slide_${file}_${extension}.tex

cd tmp/
pdflatex slide_${file}_${extension}.tex
cd ../

mv tmp/slide_${file}_${extension}.tex ${outDirData}/${extension}/
mv tmp/slide_${file}_${extension}.pdf ${outDirData}/${extension}/
cp ${outDirData}/${extension}/slide_${file}_${extension}.tex ~/scratch1/www/${extension}/
cp ${outDirData}/${extension}/slide_${file}_${extension}.pdf ~/scratch1/www/${extension}/

}
