#!/bin/bash
file=data/runRanges/runRangeLimits2.dat


# output to a file with
# run timeMin label

# remove the commented lines
sed '/^#/ d' $file > tmp/runLimit.clean

#find the timeMin
for run in `cat tmp/runLimit.clean | sed 's| #.*||'`
do
	echo -ne $run"\t"
	grep $run data/runRanges/monitoringRun2016.dat | sed 's|.*:||;s|#.*||' | awk '(NF==3){print $3}' | sed 's|-.*||' | sort | uniq
	echo 
done | awk '(NF==2){print $0}' > tmp/runLimitTime

echo "set x2tics (\\" > $file.label
for run in `cut -f 1 tmp/runLimitTime`
do
	label=`grep $run $file | sed 's|.*#||'`
	time=`grep $run tmp/runLimitTime | awk '{print $2}'`
	
	echo "\"$run: $label\" $time,\\"
done | sed 's|,$||' >> $file.label
echo ")" >> $file.label

index=1
for run in `cut -f 1 tmp/runLimitTime`
do
	time=`grep $run tmp/runLimitTime | awk '{print $2}'`
	echo "set arrow $index from $time,graph 0 to $time,graph 1 nohead dt 2" >> $file.label
	let index=$index+1
done


