eos_path=/eos/project/c/cms-ecal-calibration
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
    ## create MC name
    # $1 = configFile
    if [ -z "${mcName}" ];then
	mcCount=`grep -v '#' $1 | grep  'selected' | grep '^s' | cut -f 3 |wc -l` #counts how many MC you have
	mcTags=`grep -v '#' $1  | grep  'selected' | grep '^s' | cut -f 1 | sort | uniq` #this is s1, s2, ecc...
	for mcTag in ${mcTags}
	  do
	  mcFiles=`grep -v '#' $1  | grep  'selected' | grep "^${mcTags}" | cut -f 3`
	  for mcFile in $mcFiles
	    do
	    mcName=`basename $mcFile  | sed 's|\(.*\)-\([0-9]*-[0-9]*\).root|\1 \2|;s|\(.*\)-\(allRange\).root|\1 \2|'`
	    extraName=`dirname $mcFile`
	    extraName=`basename $extraName`
		 #only add the LTbinned names once
		 if [[ "$mcName" == *"DYJets_madgraph-LT_"* ]]
		 then
			 if [[ -z "$first_LT" ]]
			 then
				 mcName=$(echo $mcName | sed 's/_[[:digit:]]\+To[[:digit:]]\+//')
				 first_LT=true
			 else
				 continue
			 fi
		 fi
	    mcNames="$mcNames $mcName $extraName"
	    #echo "---> " $mcFile $mcName $extraName

	  done
	done
	mcNames=`echo $mcNames | sed 's| |\n|g;' |  sort -r | uniq | sed '/^[0-9][^T]/d'`
	mcName=`echo $mcNames | sort | uniq | sed 's| |_|g;s|^_||'`
	#	echo $mcName
    fi
}




#mkSmearerCatSignal(){
#   basenameRegion=`basename $1 .dat`
#   if [ ! -e "${eos_path}/data/smearerCat/smearerCat_`basename $1 .dat`_s1-`basename $configFile .dat`.root" ] || [ "$3" = "-f" ];then
#		if [ "$3" = "-f" ]; then
#			echo "[INFO] Force categorization for MC"
#		fi
#		echo "[STATUS] Creating smearerCat for signal: `basename $configFile .dat` `basename $1 .dat`"
#		echo "./bin/ZFitter.exe -f ${configFile} --regionsFile=$1 --saveRootMacro  --addBranch=smearerCat_s"
# 		./bin/ZFitter.exe -f ${configFile} --regionsFile=$1  \
# 	    	--saveRootMacro  --addBranch=smearerCat_s || exit 1 
#		for tag in `grep "^s" ${configFile} | grep selected | awk -F" " ' { print $1 } '`
#		do
#	    	mv tmp/smearerCat_${basenameRegion}_${tag}-`basename $configFile .dat`.root ${eos_path}/data/smearerCat/ || exit 1
#		done
#   fi
#
#   #Once cat root files are created, just write them in the validation file and be sure they are unique
#   for tag in `grep "^s" ${configFile} | grep selected | awk -F" " ' { print $1 } '`
#   do
#		is_already_written=$(cat $2 |grep smearerCat_${basenameRegion}_${tag}|wc -l)
#		if [ "${is_already_written}" = "1" ]; then
#	    	echo "[CHECK NEEDED] The categorization root file was already written for tag ${tag}. Be sure of what you are doing"
#		elif [ "${is_already_written}" = "0" ]; then  
#	    	echo "[INFO] You are writing the categorization file in the validation file for tag ${tag}"
#	    	echo -e "${tag}\tsmearerCat_${basenameRegion}\t${eos_path}/data/smearerCat/smearerCat_${basenameRegion}_${tag}-$(basename $configFile .dat).root" >> $2
#		else
#	    	echo "[ERROR] you have written multiple times the same categorization file! There must be a mistake"
#		fi
#   done
#}

mkSmearerCatX() {
	dORs=$1	
	regionFile=$2
   basenameRegion=`basename $2 .dat`
	configFile=$4
	basenameConfig=`basename $4 .dat`
	outFolder=$3
	outFileForm=smearerCat_${basenameRegion}_DDD-${basenameConfig}.root
	if [ "$6" ]; then
		corrEleType=$5 #optional
		force=$6
	elif [ "$5" = "-f" ]; then
		force=$5
	else
		corrEleType=$5
	fi

	mkdir -p ${outFolder}
	file1=${outFolder}/${outFileForm/DDD/${dORs}1}
	echo "[STATUS] checking for $file1"
   if [ ! -e "$file1" ] || [ "$force" = "-f" ]; then
		echo "[STATUS] Creating smearerCat for $dORs : ${basenameConfig} ${basenameRegion}"

		echo "[DEBUG] ./bin/ZFitter.exe -f $configFile --regionsFile=$regionFile  \
	    	--saveRootMacro  --addBranch=smearerCat_$dORs $corrEleType"
		./bin/ZFitter.exe -f $configFile --regionsFile=$regionFile  \
	    	--saveRootMacro  --addBranch=smearerCat_$dORs $corrEleType || exit 1 
		mv tmp/${outFileForm/DDD/${dORs}*} ${outFolder}/ || exit 1
   fi

   #TO-DO -> be sure that the writing is unique to avoid stupid crashes
   #Once cat root files are created, just write them in the validation file and be sure they are unique
   for tag in `grep "^$dORs" $configFile | grep selected | awk -F" " ' { print $1 } '`
   do
		is_already_written=$(cat $configFile |grep smearerCat_${basenameRegion}_${tag}-|wc -l)
		if [ "${is_already_written}" = "1" ]; then
	    	echo "[CHECK NEEDED] The categorization root file was already written for tag ${tag}. Be sure of what you are doing"
		elif [ "${is_already_written}" = "0" ]; then  
	    	echo "[INFO] You are writing the categorization file in the validation file for tag ${tag}"
	    	echo -e "${tag}\tsmearerCat_${basenameRegion}\t${outFolder}/${outFileForm/DDD/${tag}}" >> $configFile
		else
	    	echo "[ERROR] you have written multiple times the same categorization file! There must be a mistake"
		fi
   done
}

mkSmearerCatSignal() {
	mkSmearerCatX s "$@"
}
mkSmearerCatData() {
	mkSmearerCatX d "$@"
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
