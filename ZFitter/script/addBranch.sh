#! /bin/bash
file=`basename $1 .dat`
branch=$2

eos_path=/eos/project/c/cms-ecal-calibration
if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi

if [[ $2 = "" ]]; then
    echo "you should specify the friend branch name"; exit;
fi

echo "File name is "$file
echo "branch name is "$branch
#regionsFile is there only because it's a mandatory option for ZFitter
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat --addBranch=$branch --saveRootMacro

#you'll obtain files like tmp/R9Eleprime_d1-Rereco_December2015.root
#Move friend branches in safe place and write them in the validation file
for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    #tag is s1, s2, ...                                                                                                                                                        
    mv tmp/${branch}_${tag}-${file}.root  ${eos_path}/friends/others/
    echo "${tag} ${branch} ${eos_path}/friends/others/${branch}_${tag}-${file}.root" >> data/validation/${file}.dat
done

for tag in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    #tag is s1, s2, ...                                                                                                                                                        
    mv tmp/${branch}_${tag}-${file}.root  ${eos_path}/friends/others/
    echo "${tag} ${branch} ${eos_path}/friends/others/${branch}_${tag}-${file}.root" >> data/validation/${file}.dat
done

