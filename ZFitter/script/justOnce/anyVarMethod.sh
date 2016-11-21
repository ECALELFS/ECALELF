#!/bin/bash
# this script tests the anyVar method for biases

configFile=data/validation/rereco/Cal_Nov2016/Cal_Nov2016_ref.dat
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
modulos=(10) # 50 100 1000)

for modulo in ${modulos[@]}
do
	echo "[`basename $0`] Processing modulo = $modulo"
	for scale in ${scales[@]}
	do
		echo "[`basename $0`] -> Processing scale = $scale"
		outDir=$outDirBase/scale/$scale/$modulo/
		mkdir -p $outDir
		(./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=$modulo --scale=$scale --outDirFitResData=$outDir &> $outDirBase/$scale-$modulo.log) &
		#./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=$modulo --scale=$scale --outDirFitResData=$outDir 
	done
	wait
done
wait

for smearing in ${smearings[@]}
do
	outDir=$outDirBase/smearing/$smearing/
	mkdir -p $outDir
#	(./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=10 --smearing=$scale --outDirFitResData=$outDir &> $smearing.log) &
done
wait

# get the list of variables 
vars=(`ls -1 $outDirBase/scale/${scale}/${modulo}-modulo_0/*.dat | sed "s|.*/||;s|.dat||"`)
echo "Variables analyzed: ${vars[@]}"

#awk/anyVarMethod.awk
# script to get mean and standard deviation over different "modulos"

#awk/anyVarMethod_ratio.awk
# make the ratio w.r.t. scale=1

for var in ${vars[@]}
do
	for modulo in ${modulos[@]}
	do
		# calculate mean and stdDev
		file=$outDirBase/scale/modulo_$modulo-$var.dat
		fileRatio=$outDirBase/scale/scaleRatio-modulo_$modulo-$var.dat
		echo "Output file: $fileRatio"
		echo -n "" > $file
		for scale in ${scales[@]}
		do
			echo $scale
			sort $outDirBase/scale/$scale/${modulo}-modulo_*/$var.dat | grep -v "#" | uniq  | awk -v scale=$scale  -f awk/anyVarMethod.awk  >> $file
		done
		sort $file | uniq > $file.tmp; 

		awk -f awk/splitCategory.awk $file.tmp | sed '1,2 d' > $file

		#make the ratio w.r.t. scale=1
		awk -f awk/anyVarMethod_ratio.awk $file.tmp | sort | awk -f awk/splitCategory.awk | sed '1,2 d' > $fileRatio

		rm $file.tmp
	done
	
done


echo "Now plot with gnuplot:"
echo "load 'macro/anyVar_linearity.gpl'"

