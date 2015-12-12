#! /bin/bash
file=${1}
#pileup histos for data
for f in `grep "^d" data/validation/${file}.dat | awk -F" " ' { print $3 } '`
do
    root -l -b $f macro/pu.C;
    tag=`grep "$f" data/validation/${file}.dat | awk -F" " ' { print $1 } '`
    mv tmp/pu_temp.root tmp/pu_${tag}.root
done 
hadd data/puHistos/pu_data_${file}.root tmp/pu_d*.root

#pileup histos for MC
for f in `grep "^s" data/validation/${file}.dat | awk -F" " ' { print $3 } '`
do
    root -l -b $f macro/pu.C;
    tag=`grep "$f" data/validation/${file}.dat | awk -F" " ' { print $1 } '`
    mv tmp/pu_temp.root tmp/pu_${tag}.root
done 
hadd data/puHistos/pu_MC_${file}.root tmp/pu_s*.root

rm tmp/pu_*.root

echo "d pileupHist data/puHistos/pu_data_${file}.root" >> data/validation/${file}.dat 
echo "s pileupHist data/puHistos/pu_MC_${file}.root" >> data/validation/${file}.dat
