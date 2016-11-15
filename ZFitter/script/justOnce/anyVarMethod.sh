#!/bin/bash
# this script tests the anyVar method for biases

configFile=data/validation/test_ss.dat
#configFile=data/validation/rereco/Cal_Nov2016/Cal_Nov2016_Alpha_v1.dat
regionsFile=data/regions/validation.dat
echo "
############################################################
# check linearity of peak position estimator: 
#   each variable is varied by a fixed scale value
#   the peak estimator vs the inject scale shift should be linear with slope = 1
############################################################"
outDirBase=test/dato/anyVar_linearity
scales=(0.90 0.95 0.98 0.99 1.00 1.01 1.02 1.05 1.10)
smearings=(0.001 0.005 0.010 0.015 0.020)
modulos=(10 50 100 500 1000)

for scale in ${scales[@]}
do
	for modulo in ${modulos[@]}
	do
	outDir=$outDirBase/scale/$scale/$modulo/
	mkdir -p $outDir
		(./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=$modulo --scale=$scale --outDirFitResData=$outDir &> $scale-$modulo.log) &
	done
	wait
done
wait

exit 0
for smearing in ${smearings[@]}
do
	outDir=$outDirBase/smearing/$smearing/
	mkdir -p $outDir
#	(./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=10 --smearing=$scale --outDirFitResData=$outDir &> $smearing.log) &
done
wait

# get the list of variables 
vars=(`ls -1 $outDirBase/${scale}/*.dat | sed "s|$outDirBase/${scale}/||;s|.dat||"`)
echo ${vars[@]}

#awk/anyVarMethod.awk
# script to get mean and standard deviation over different "modulos"

#awk/anyVarMethod_ratio.awk
# make the ratio w.r.t. scale=1

for var in ${vars[@]}
do

	# calculate mean and stdDev
	file=$outDirBase/scale-$var.dat
	echo -n "" > $file
	for scale in ${scales[@]}
	do
		sort $outDirBase/$scale-modulo_*/$var.dat |uniq  | awk -v scale=$scale -f awk/anyVarMethod.awk >> $file
	done

	#make the ratio w.r.t. scale=1
	fileRatio=$outDirBase/scaleRatio-$var.dat
	awk -f awk/anyVarMethod_ratio.awk $file | sort | awk -f awk/splitCategory.awk | sed '1,2 d' > $fileRatio
	
done


echo "Now plot with gnuplot:"
echo "load 'macro/anyVar_linearity.gpl'"

