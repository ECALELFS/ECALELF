#! /bin/bash
file=${1}
output_path=/eos/project/c/cms-ecal-calibration/data/puHistos
#pileup histos for data
for f in `grep "^d" data/validation/${file}.dat | grep selected | awk -F" " ' { print $3 } '`
do
    root -l -b $f macro/pu.C;
    tag=`grep "$f" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
    mv tmp/pu_temp.root tmp/pu_${tag}.root
done 
hadd -f ${output_path}/pu_data_${file}.root tmp/pu_d*.root

#pileup histos for MC
for f in `grep "^s" data/validation/${file}.dat | grep selected | awk -F" " ' { print $3 } '`
do
    root -l -b $f macro/pu.C;
    tag=`grep "$f" data/validation/${file}.dat | grep selected | awk -F" " ' { print $1 } '`
    mv tmp/pu_temp.root tmp/pu_${tag}.root
done 
hadd -f ${output_path}/pu_MC_${file}.root tmp/pu_s*.root

rm tmp/pu_*.root

echo "d pileupHist ${output_path}/pu_data_${file}.root" >> data/validation/${file}.dat 
echo "s pileupHist ${output_path}/pu_MC_${file}.root" >> data/validation/${file}.dat
