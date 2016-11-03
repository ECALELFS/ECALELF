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
for scale in ${scales[@]}
do
	outDir=$outDirBase/scale/$scale/
	mkdir -p $outDir
	(./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=10 --scale=$scale --outDirFitResData=$outDir &> $scale.log) &
done
wait

for smearing in ${smearings[@]}
do
	outDir=$outDirBase/smearing/$smearing/
	mkdir -p $outDir
	(./bin/ZFitter.exe -f $configFile --regionsFile=$regionsFile --runRangesFile=d.dat  --noPU --commonCut="Et_25" --anyVar --selection=loose25nsRun2 --runToy --modulo=10 --smearing=$scale --outDirFitResData=$outDir &> $smearing.log) &
done
wait

# get the list of variables 
vars=(`ls -1 $outDirBase/${scale}/*.dat | sed "s|$outDirBase/${scale}/||;s|.dat||"`)
echo ${vars[@]}

# script to get mean and standard deviation over different "modulos"
cat > tmp/anyVarMethod.awk <<EOF
BEGIN{
  firstColumn=3
}

(NF!=0){
  cat=\$1; 
  cats[cat]=NF
  for(i=firstColumn; i <=NF; i++){
    sum[cat,i]+=\$i; 
    sum2[cat,i]+=\$i*\$i; 
  }
  n[cat]+=1
};
END{
  for(cat in cats){
    printf("%s\t%.2f", cat, scale) 
    printf("\t%d", n[cat]);
    for(i=firstColumn; i <= cats[cat]; i++){
      if(n[cat] == 0){
        printf("\t%f\t%f", 0, 0);
      } else {
        mean   =  sum[cat,i]/n[cat]             
        stdDev =  sqrt(sum2[cat,i]/n[cat]-mean*mean)
        printf("\t%f\t%f", mean, stdDev);
      }
    }
   printf("\n");
  }
}

EOF



cat > tmp/anyVarMethod_ratio.awk<<EOF
BEGIN{
  firstColumn=4
}

{
  line=\$0
  if(NR==1){
    while(1==getline <FILENAME ){
      if(NF!=0){
  cat=\$1; 
  cats[cat]=NF
        if(\$2==1){        
  for(i=firstColumn; i <=NF; i++){
    val[cat,i]=\$i; 
  }
}
      }
    }
  }
  \$0=line
  cat=\$1; 
  printf("%s\t%.2f", cat, \$2)
  for(i=firstColumn; i <=NF; i++){
    printf("\t%f", \$i/val[cat,i])
    i++
    printf("\t%f", sqrt(\$i/\$(i-1))*(\$i/\$(i-1))+(val[cat,i]/val[cat,(i-1)])*(val[cat,i]/val[cat,(i-1)]))
#    printf("\t%f", val[cat,i])
  }
  printf("\n")
};


EOF

for var in ${vars[@]}
do

	# calculate mean and stdDev
	file=$outDirBase/scale-$var.dat
	echo -n "" > $file
	for scale in ${scales[@]}
	do
		cat $outDirBase/$scale-modulo_*/$var.dat   | awk -v scale=$scale -f tmp/anyVarMethod.awk >> $file
	done

	#make the ratio w.r.t. scale=1
	fileRatio=$outDirBase/scaleRatio-$var.dat
	awk -f tmp/anyVarMethod_ratio.awk $file > $fileRatio
done


