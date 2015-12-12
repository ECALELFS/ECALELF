#! /bin/bash
#you should start from a file with only d and s
#file=miniAOD_November2015_withPho
file=Rereco_December2015
region1=scaleStep2smearing_1 
region2=scaleStep2smearing_2 
#invMass_type=invMass_SC_corr
invMass_type=invMass_SC_pho_regrCorr

#1)Fai pileupHist
#script/pileup_histoMaker.sh $file

#2)Fai i pileupTree e categorizza
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --addBranch=smearerCat invMass_var=${invMass_type} --saveRootMacro
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --addBranch=smearerCat invMass_var=${invMass_type} --saveRootMacro

#pileUPtree
for tag in `grep "^s" data/validation/${file}.dat | awk -F" " ' { print $1 } '`
do
    #tag e' s1, s2, ...
    mv tmp/mcPUtree${tag}.root  data/puTree/${tag}_${file}.root
    echo "${tag} pileup data/puTree/${tag}_${file}.root" >> data/validation/${file}.dat 
done

#categorie
mv tmp/smearerCat_${region1}_*${file}* friends/smearerCat/ 
mv tmp/smearerCat_${region2}_*${file}* friends/smearerCat/ 
for tag in `grep "^s" data/validation/${file}.dat | awk -F" " ' { print $1 } '`
do
    echo "${tag} smearerCat_${region1} friends/smearerCat/smearerCat_${region1}_${tag}-${file}.root" >> data/validation/${file}.dat 
    echo "${tag} smearerCat_${region2} friends/smearerCat/smearerCat_${region2}_${tag}-${file}.root" >> data/validation/${file}.dat 
done
for tag in `grep "^d" data/validation/${file}.dat | awk -F" " ' { print $1 } '`
do  
    echo "${tag} smearerCat_${region1} friends/smearerCat/smearerCat_${region1}_${tag}-${file}.root" >> data/validation/${file}.dat 
    echo "${tag} smearerCat_${region2} friends/smearerCat/smearerCat_${region2}_${tag}-${file}.root" >> data/validation/${file}.dat 
done


#Un giro di prova
#./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region1}.dat --invMass_var=${invMass_type}  --commonCut=Et_20-noPF --autoBin --smearerFit
#--initFile=step2/init_1.txt
#./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/${region2}.dat --invMass_var=${invMass_type} --commonCut=Et_20-noPF --autoBin --smearerFit  
#--initFile=step2/init_2.txt
#./script/fit.sh test/dato/fitres/outProfile-${region1}-Et_20-noPF.root
#./script/fit.sh test/dato/fitres/outProfile-${region2}-Et_20-noPF.root
#Likelihood_plot_dir=~/scratch1/www/Validation_ntuple_Paolo
#mv test/dato/img/outProfile-${region1}-Et_20-noPF-*.png ${Likelihood_plot_dir}
#mv test/dato/img/outProfile-${region2}-Et_20-noPF-*.png ${Likelihood_plot_dir}