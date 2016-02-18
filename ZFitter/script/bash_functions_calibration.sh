#! /bin/bash
shopt -s expand_aliases
source ~/.bashrc
#1)Fai pileupHist
pileupHist(){
script/pileup_histoMaker.sh $file
}

pileupTrees(){
#region name is there just because it's mandatory for ZFitter
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat --saveRootMacro

#pileUPtree
for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    #tag e' s1, s2, ...
    mv tmp/mcPUtree${tag}.root  data/puTree/${tag}_${file}.root
    echo "${tag} pileup data/puTree/${tag}_${file}.root" >> data/validation/${file}.dat 
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

#Un giro di prova
Test_1_job(){
#    ./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=Et_20-noPF --autoBin --smearerFit ${initParameters1} ${eleID}
    if [[ ${region2} != "" ]];then
	./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type} --commonCut=Et_20-noPF --autoBin --smearerFit  ${initParameters2} ${eleID}
    fi
./script/fit.sh test/dato/fitres/outProfile-${region1}-Et_20-noPF.root

if [ ! -e "/afs/g/gfasanel/scratch1/www/RUN2_ECAL_Calibration/${extension}" ];then mkdir ~/scratch1/www/RUN2_ECAL_Calibration/${extension} -p; mkdir ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_4cat -p; cp ~/scratch1/www/index.php ~/scratch1/www/RUN2_ECAL_Calibration/${extension}; cp ~/scratch1/www/index.php ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_4cat; fi
mv test/dato/img/outProfile-${region1}-Et_20-noPF-* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_4cat
#Plot_data_MC del test job                                                                                                                                                      
echo "{" > tmp/plotter_data_MC.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/plotter_data_MC.C
echo "PlotMeanHist(\"test/dato/fitres/histos-${region1}-Et_20-noPF.root\");" >> tmp/plotter_data_MC.C
echo "}" >> tmp/plotter_data_MC.C
root -l -b -q ~/rootlogon.C tmp/plotter_data_MC.C
mv test/dato/img/histos-${region1}-Et_20-noPF* ~/scratch1/www/RUN2_ECAL_Calibration/${extension}/temp_4cat
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
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=Et_20-noPF --autoBin --smearerFit ${initParameters1} --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/;"  

 bsub -q 2nd\
            -oo ${outDirData}/${extension}/%I/fitres/${region2}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${region2}-stderr.log\
            -J "${region2} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type} --commonCut=Et_20-noPF --autoBin --smearerFit ${initParameters2} --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/;"
}
#wait until jobs are finished, then make the mean likelihood and fit
Likelihood_Fitter(){
#check jobs
#while [ "`bjobs -J \"${region1} ${extension}[1-50]\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
#while [ "`bjobs -J \"${region2} ${extension}[1-50]\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
###########################Make Likelihood plots###########################
./script/haddTGraph.sh -o ${outDirData}/${extension}/fitres/outProfile-${region1}-Et_20-noPF.root ${outDirData}/${extension}/*/fitres/outProfile-${region1}-Et_20-noPF.root
echo "{" > tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "FitProfile2(\"${outDirData}/${extension}/fitres/outProfile-${region1}-Et_20-noPF.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "}" >> tmp/fitProfiles_${invMass_type}_${extension}.C
if [ ! -e "${outDirData}/${extension}/img" ];then mkdir test/dato/img/${extension}/img -p; fi
root -l -b -q tmp/fitProfiles_${invMass_type}_${extension}.C
echo "Likelihood plots are in ${outDirData}/${extension}/img/"
if [ ! -e "~/scratch1/www/${extension}/" ];then mkdir ~/scratch1/www/${extension}/ -p; cp ~/scratch1/www/index.php ~/scratch1/www/${extension}/; fi
mv ${outDirData}/${extension}/img/*.png ~/scratch1/www/${extension}/
echo "************Find png plots in scratch1/www/${extension}/img/"
##REGION2
./script/haddTGraph.sh -o ${outDirData}/${extension}/fitres/outProfile-${region2}-Et_20-noPF.root ${outDirData}/${extension}/*/fitres/outProfile-${region2}-Et_20-noPF.root
echo "{" > tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles_${invMass_type}_${extension}.C
echo "FitProfile2(\"${outDirData}/${extension}/fitres/outProfile-${region2}-Et_20-noPF.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles_${invMass_type}_${extension}.C
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
echo "PlotMeanHist(\"${outDirData}/${extension}/1/fitres/histos-${region1}-Et_20-noPF.root\");" >> tmp/plotter_data_MC.C
echo "PlotMeanHist(\"${outDirData}/${extension}/1/fitres/histos-${region2}-Et_20-noPF.root\");" >> tmp/plotter_data_MC.C
echo "}" >> tmp/plotter_data_MC.C
root -l -b -q tmp/plotter_data_MC.C
mv ${outDirData}/${extension}/1/./img/histos-${region1}*.png ~/scratch1/www/${extension}/data_MC/
mv ${outDirData}/${extension}/1/./img/histos-${region2}*.png ~/scratch1/www/${extension}/data_MC/
}

Write_down_dat_corr(){
##Scale corrections: per i dati -> 1/scala_MC
grep scale ${outDirData}/${extension}/img/outProfile-${region1}-Et_20-noPF-FitResult-.config |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-Et_20-noPF||g"> tmp/corr_MC.dat
grep scale ${outDirData}/${extension}/img/outProfile-${region2}-Et_20-noPF-FitResult-.config |  grep -v absEta_0_1 | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-Et_20-noPF||g">> tmp/corr_MC.dat

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


grep constTerm ${outDirData}/${extension}/img/outProfile-${region1}-Et_20-noPF-FitResult-.config |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-Et_20-noPF||g" >> tmp/corr_MC.dat
grep constTerm ${outDirData}/${extension}/img/outProfile-${region2}-Et_20-noPF-FitResult-.config |  grep -v absEta_0_1 | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|-Et_20-noPF||g" >> tmp/corr_MC.dat

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
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=Et_20-noPF --autoBin --smearerFit --corrEleType=EtaR9_${extension} --smearEleType=stochastic_${extension} --profileOnly --plotOnly > test/dato/fitres/closure_step2_1.dat
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type}  --commonCut=Et_20-noPF --autoBin --smearerFit --corrEleType=EtaR9_${extension} --smearEleType=stochastic_${extension} --profileOnly --plotOnly > test/dato/fitres/closure_step2_2.dat
if [ ! -e "test/dato/${extension}/closure_step2_${file}_${extension}/" ]; then mkdir test/dato/${extension}/closure_step2_${file}_${extension}/; fi
mv test/dato/fitres/closure_step2*.dat test/dato/${extension}/closure_step2_${file}_${extension}/ #output of the closure tests "job"
mv test/dato/fitres/outProfile-${region1}-Et_20-noPF.root test/dato/${extension}/closure_step2_${file}_${extension}/
mv test/dato/fitres/outProfile-${region2}-Et_20-noPF.root test/dato/${extension}/closure_step2_${file}_${extension}/
mv test/dato/fitres/histos-${region1}-Et_20-noPF.root test/dato/${extension}/closure_step2_${file}_${extension}/
mv test/dato/fitres/histos-${region2}-Et_20-noPF.root test/dato/${extension}/closure_step2_${file}_${extension}/
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
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-scale${categories[0]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-scale${categories[1]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[0]} (left) and ${categories_name[1]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-scale${categories[2]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-scale${categories[3]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[2]} (left) and ${categories_name[3]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####SCALE step2_2
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-scale${categories[4]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-scale${categories[5]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[4]} (left) and ${categories_name[5]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-scale${categories[6]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-scale${categories[7]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{scale parameter for ${categories_name[6]} (left) and ${categories_name[7]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####constTerm step2_1
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-constTerm${categories[0]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-constTerm${categories[1]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[0]} (left) and ${categories_name[1]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-constTerm${categories[2]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-constTerm${categories[3]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[2]} (left) and ${categories_name[3]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex

####constTerm step2_2
echo "\begin{frame}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-constTerm${categories[4]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-constTerm${categories[5]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\caption{constTerm parameter for ${categories_name[4]} (left) and ${categories_name[5]} (right)}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\end{figure}">>tmp/slide_${file}_${extension}_profileOnly.tex

echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\begin{center}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-constTerm${categories[6]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-constTerm${categories[7]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}_profileOnly.tex
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
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-scale${categories[$index]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region1}-Et_20-noPF-constTerm${categories[$index]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\end{center}">>tmp/slide_${file}_${extension}.tex
    echo "\caption{scale parameter (left) and smearing parameter (right) for ${categories_name[$index]}}">>tmp/slide_${file}_${extension}.tex
    echo "\end{figure}">>tmp/slide_${file}_${extension}.tex
    echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}.tex
    echo "\begin{center}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/data_MC/histos-${region1}-Et_20-noPF_${categories[$index]}-Et_20-noPF${categories[$index]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}.tex
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
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-scale${categories[$index]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/outProfile-${region2}-Et_20-noPF-constTerm${categories[$index]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}.tex
    echo "\end{center}">>tmp/slide_${file}_${extension}.tex
    echo "\caption{scale parameter (left) and smearing parameter (right) for ${categories_name[$index]}}">>tmp/slide_${file}_${extension}.tex
    echo "\end{figure}">>tmp/slide_${file}_${extension}.tex
    echo "\begin{figure}[!htbp]">>tmp/slide_${file}_${extension}.tex
    echo "\begin{center}">>tmp/slide_${file}_${extension}.tex
    echo "\includegraphics[width=0.43\textwidth]{{/afs/cern.ch/user/g/gfasanel/scratch1/www/${extension}/data_MC/histos-${region2}-Et_20-noPF_${categories[$index]}-Et_20-noPF${categories[$index]}-Et_20-noPF}.png}">>tmp/slide_${file}_${extension}.tex
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