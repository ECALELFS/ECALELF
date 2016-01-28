#! /bin/bash
file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
branch=$2

if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi

if [[ $2 = "" ]]; then
    echo "you should specify the friend branch name"; exit;
fi

echo "File name is "$file
echo "branch name is "$branch
#regionsFile e' solo per finta, visto che e' una opzione necessaria
./bin/ZFitter.exe -f data/validation/${file}.dat --regionsFile=data/regions/scaleStep0.dat --addBranch=$branch --saveRootMacro

#you'll obtain files like tmp/R9Eleprime_d1-Rereco_December2015.root
#Move friend branches in safe place and write them in the validation file
for tag in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    #tag e' s1, s2, ...                                                                                                                                                        
    mv tmp/${branch}_${tag}-${file}.root  friends/others/
    echo "${tag} ${branch} friends/others/${branch}_${tag}-${file}.root" >> data/validation/${file}.dat
done

for tag in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
do
    #tag e' s1, s2, ...                                                                                                                                                        
    mv tmp/${branch}_${tag}-${file}.root  friends/others/
    echo "${tag} ${branch} friends/others/${branch}_${tag}-${file}.root" >> data/validation/${file}.dat
done

