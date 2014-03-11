puName(){
    ## pileup reweight name
    # $1 = configFile
    if [ -z "$puName" ];then
	puCount=`grep -v '#' $1  | grep  'pileupHist' | grep '^d' | cut -f 3 |wc -l`
	if [ "${puCount}" == "0" ];then
	    echo "[ERROR] No or too mani pileupHist files for data"
	    exit 1
	fi
	puFiles=`grep -v '#' $1  | grep  'pileupHist' | grep '^d' | cut -f 3`
	
	for puFile in $puFiles
	  do
	  puNames="${puNames} `basename $puFile .root | sed 's|\..*||;s|\([0-9]*-[0-9]*\)-\(.*\)|\1 json_\2|'`"
	done
	puNames=`echo $puNames | sed 's| |\n|g' | sort | uniq | sed 's|json_||;'`
	puName=`echo $puNames | sed 's| |_|g'`
	#echo $puFile
    #puName=`basename $puFile .root | sed 's|\..*||'`
	#puName=`echo $puName | sed 's|^_||'`
	#echo $puName
    fi
}

mcName(){
    ## MC name
    # $1 = configFile
    if [ -z "${mcName}" ];then
	mcCount=`grep -v '#' $1 | grep  'selected' | grep '^s' | cut -f 3 |wc -l`

	mcTags=`grep -v '#' $1  | grep  'selected' | grep '^s' | cut -f 1 | sort | uniq`
	for mcTag in ${mcTags}
	  do
	  mcFiles=`grep -v '#' $1  | grep  'selected' | grep "^${mcTags}" | cut -f 3`
	  for mcFile in $mcFiles
	    do
	    mcName=`basename $mcFile  | sed 's|\(.*\)-\([0-9]*-[0-9]*\).root|\1 \2|;s|\(.*\)-\(allRange\).root|\1 \2|'`
	    extraName=`dirname $mcFile`
	    extraName=`basename $extraName`
	    mcNames="$mcNames $mcName $extraName"
	    #echo "---> " $mcFile $mcName $extraName

	  done
	done
	mcNames=`echo $mcNames | sed 's| |\n|g;' |  sort -r | uniq | sed '/^[0-9][^T]/d'`
	mcName=`echo $mcNames | sort | uniq | sed 's| |_|g;s|^_||'`
	#	echo $mcName
    fi
}




mkSmearerCatSignal(){

    if [ ! -e "data/smearerCat/smearerCat_`basename $1 .dat`_s1-`basename $configFile .dat`.root" -o  ! -e "data/smearerCat/smearerCat_`basename $1 .dat`_s2-`basename $configFile .dat`.root" -o  ! -e "data/smearerCat/smearerCat_`basename $1 .dat`_s3-`basename $configFile .dat`.root" ];then
	echo "[STATUS] Creating smearerCat for signal: `basename $configFile .dat` `basename $1 .dat`"
 	./bin/ZFitter.exe -f ${configFile} --regionsFile=$1  \
 	    --saveRootMacro  --addBranch=smearerCat_s  || exit 1
	basenameConfig=`basename $1 .dat`
	for file in tmp/smearerCat_${basenameConfig}_s*-`basename $configFile .dat`.root
	  do
	  
	  tag=`echo $file | sed "s|tmp/smearerCat_${basenameConfig}_s\([0-9]\)-.*|s\1|"`
	  
	  mv $file data/smearerCat/ || exit 1
	  echo -e "$tag\tsmearerCat_${basenameConfig}\tdata/smearerCat/`echo $file | sed 's|tmp/||'`" >> $configFile
	done
    fi
    
#     tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
#     for tag in $tags
#       do
#       case ${tag} in
# 	  s*)
# 	      if [  "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
# 		  ./bin/ZFitter.exe -f ${configFile} --regionsFile=$1  \
# 		      --saveRootMacro \
# 		      --addBranch=smearerCat_s  --smearerFit
# 		  break;
# 	      fi
# 	      ;;
#       esac
#     done

#     for tag in $tags
#       do
#       case ${tag} in
# 	  s*)
# 	      if [  "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
# 		  mv tmp/smearerCat_`basename $1 .dat`_${tag}-`basename $configFile .dat`.root data/smearerCat/smearerCat_`basename $1 .dat`_${tag}-`basename $configFile .dat`.root #|| exit 1
# 		  echo -e "$tag\tsmearerCat_${basenameEB}\tdata/smearerCat/smearerCat_`basename $1 .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile
# 	      fi
# 	      ;;
#       esac
#     done
}


mkSmearerCatData(){
    #$1: regionFile
    #$2: outDirData/step...
    #$3: configFile
    #$4: corrEleType
    if [ ! -e "$2/smearerCat_`basename $1 .dat`_d1-`basename $configFile .dat`.root" ];then
	echo "[STATUS] Creating smearerCat for data: `basename $configFile .dat` `basename $1 .dat`"
	./bin/ZFitter.exe -f $3 --regionsFile=$1  \
	    --saveRootMacro  --addBranch=smearerCat_d $4 || exit 1
	mv tmp/smearerCat_`basename $1 .dat`_d*-`basename $configFile .dat`.root $2/ || exit 1
    fi
    cat $3 \
	| sed "/selected/ ! d; /selected/{ s|^\(d[1-9]\)\tselected.*|\1\tsmearerCat_`basename $1 .dat`\t$2/smearerCat_`basename $1 .dat`_\1-`basename $3 .dat`.root|}" | sort | uniq |grep smearerCat |grep '^d'   >> $3.tmp
    cat $3.tmp $3 | sort | uniq -d > $3.tmp2
    for line in `cat $3.tmp2 | sed 's|[ ]+|\t|g' | cut  -f 3`;
      do
      sed -i "\#$line# d" $3.tmp
    done
    cat $3.tmp
    echo 
    echo
    echo
    cat $3.tmp >> $3
    rm $3.tmp $3.tmp2
}
	


checkStepDep(){
    for step in $@
      do
      index=0
      for stepName in ${stepNameList[@]}
	do
	if [ "${step}" != "$stepName" ];then
	    let index=$index+1
	else
	    break
	fi
      done
      if [ ! -r "${outDirTable}/${outFileList[$index]}" ];then
	  echo -e "[ERROR] output file for step $step not found:\n ${outDirTable}/${outFileList[$index]}" >> /dev/stderr
	  exit 1
      fi
      if [ "$index" -ge "${#stepNameList[@]}" ];then
	  echo "[ERROR] step $step not set in output file list" >> /dev/stderr
	  exit 1
      fi
    done
} 




mkSmearEleSignal(){
    #$1: regionFile
    #$2: configFile
    #$3: smearEleFile
    #$4: smearEleType
    #$5: smearCBAlpha
    basenameConfig=`basename $2 .dat`
    
    if [ ! -e "data/other/smearEle/smearEle_$4_s1-${basenameConfig}.root" -o  ! -e "data/other/smearEle/smearEle_$4_s2-${basenameConfig}.root" -o  ! -e "data/other/smearEle/smearEle_$4_s3-${basenameConfig}.root" ];then
	echo "[STATUS] Creating smearEle for signal: ${basenameConfig} `basename $1 .dat`"
 	./bin/ZFitter.exe -f $2 --regionsFile=$1  \
 	    --saveRootMacro   --noPU \
	    --smearEleType=$4 --smearEleFile=$3 --smearingCBAlpha=$5 --smearingCBPower=$6 || exit 1 #> ${baseDir}/treeGen.log || exit 1

	for file in tmp/smearEle_$4_s*-${basenameConfig}.root
	  do
	  
	  tag=`echo $file | sed "s|tmp/smearEle_$4_s\([0-9]\)-.*|s\1|"`
	  
	  mv $file data/other/smearEle/ || exit 1
	  echo -e "$tag\tsmearEle_$4\tdata/other/smearEle/`echo $file | sed 's|tmp/||'`" >> $2
	done
    fi

#     for file in data/other/smearEle/smearEle_$4_s*-${basenameConfig}.root
#       do
      
#       tag=`echo $file | sed "s|data/other/smearEle/smearEle_.*_s\([0-9]\)-.*|s\1|"`
      
#       echo -e "$tag\tsmearEle_$4\tdata/other/`echo $file | sed 's|tmp/||'`" >> $2
#     done
}



######
# Convert RooRealVar .config files into smearEle config files
getSmearEleCfg(){
    #$1= config file
    sed 's|_| |;' $1 | awk -f awk/smearEleCfg.awk
}
