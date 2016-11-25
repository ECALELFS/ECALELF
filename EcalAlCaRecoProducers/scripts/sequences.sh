#!/bin/bash

findDefinition(){
	object=$1
	f=`grep "${object} =" ${file}`
	case $f in
		*cms.Sequence*)
			# make a list of elements
			ff=`echo $f | sed 's|.*(||;s|\*||g;s|\[||;s|\])||;s|[,+]| |g;s|)||'`
			echo $ff
#			echo $f $ff > /dev/stderr
			return 0
			;;
		*cms.Path*)
			# make a list of elements
			ff=`echo $f | sed 's|.*(||;s|\*||g;s|\[||;s|\])||;s|[,+]| |g;s|)||'`
			echo $ff
			return 0
			;;
		
		*)
			#don't print anything
			ff=`grep -A 10 ${object} ${file} | sed '1 d;/EDFilter/{Q}; /EDProducer/{Q}; /EDAnalyzer/{Q}'`
			echo $ff
#			echo $ff >> /dev/stderr
			return 1
			;;
	esac
	
}

printDefinition(){
	#$1 is the object
	#$2 is the level
	SS=`findDefinition $1`
	if [ "$?" == "0" ];then
		if [ -z "$SS" ];then
			SS="empty"
		fi
		for S in $SS
		do 
#			echo "Now: " $S
			let L=$2
			let LL=$2+1
			while [ $L -gt 0 ]
			do
				echo -n "--"
				let L=$L-1
			done
			echo "> " $S

			printDefinition $S $LL
		done
	else
		if [ -n "$SS" ];then
			let L=$2
			while [ $L -gt 0 ]
			do
				echo -n "--"
				let L=$L-1
			done
			echo " **> " $SS
		fi
	fi
}


file=$1
path=$2


scheduledPaths=`grep Schedule ${file} | sed 's|.*Schedule(||;s|\*||g;s|\[||;s|\])||;s|,||'`


for path in $scheduledPaths
do
	echo "Path: " $path 
	printDefinition $path 1
	continue
	s1s=`findDefinition $path`
	for s1 in $s1s
	do 
		printDefinition $s1 1
	done
	
	
done

