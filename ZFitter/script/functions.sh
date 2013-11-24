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
