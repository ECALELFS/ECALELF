#!/bin/bash
titleOne="Data"
titleTwo=""
color=false
column=3
usage(){
    echo "`basename $0` -t tableFile -x xVar -y yVar --outDirImgData dir"
    echo " xVar: runNumber | absEta"
    echo " yVar: peak | scaledWidth"
    echo "----- Optional"
    echo " --noPlot: does not make the stability plot"
    echo " --xMin"
    echo " --xMax"
    echo " -l legend"
    echo " --color: make coloured plot (def=$color)"
    echo " --column arg (def=$column)"
    echo " --allRegions"
    echo " -g arg: select regions matching arg, to be used with --allRegions"
    echo " --norm: normalized the y axis to the first point"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:x:y:l:g: -l help,tableFile:,xVar:,yVar:,outDirImgData:,noPlot,xMin:,xMax:,tableFile2:,titleFile1:,titleFile2:,color,column:,allRegions,norm -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

index=0
while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-t) TABLEFILES="${TABLEFILES} $2"; shift;;
	-l) LEGENDS[${index}]="$2";  echo $index; echo ${LEGENDS[$index]}; let index=$index+1; shift;;
	-x|--xVar) xVar=$2; shift;;
	-y|--yVar) yVar=$2; shift;;
	--outDirImgData) outDirImgData=$2; shift;;
	--noPlot) NOPLOT=y;;
	--xMin) xMin=$2; shift;;
	--xMax) xMax=$2; shift;;
	--titleFile1) titleOne="$2"; echo $titleOne; shift;;
	--titleFile2) titleTwo="$2"; shift;;
	--color) color=true;;
	--xMean) XMEAN=true;;
	--column) column=$2; shift;;
	--allRegions) MULTIREGION=y;;
	-g) REGIONGREP=$2; shift;;
	--norm) NORM=y;;
	#--titleFile1) titleOne=$2; shift;;
	#--titleFile2) titleTwo=$2; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


if [ -z "${TABLEFILE}" -a -z "${TABLEFILES}" ];then
    echo "[ERROR] Table file not specified: mandatory paramater" >> /dev/stderr
    exit 1
fi


if [ -n "${TABLEFILETWO}" -a ! -r "${TABLEFILETWO}" ];then
    echo "[ERROR] Second table file ${TABLEFILETWO} not found or not readable" >> /dev/stderr
    exit 1
fi

if [ -z "${xVar}" -o -z "${yVar}" ]; then 
    echo "[ERROR] xVar or yVar not specified: mandatory paramater" >> /dev/stderr
    exit 1
fi

if [ -z "${outDirImgData}" -a -z "${NOPLOT}" ];then
    echo "[ERROR] outDirImgData not specified: mandatory paramater" >> /dev/stderr
    exit 1
fi

if [ ! -e "tmp/" ];then mkdir tmp/; fi

tmpFile=tmp/tmpFile.dat
columns=$COLUMNDATA,$COLUMNMC,$COLUMNDATAMC
#./script/tex2txt.sh $TABLEFILE > $tmpFile

case $yVar in
    peak|deltaM)
	columns=3-5
	yVarName="#Delta m [GeV/c^{2}]"
	yMin=-1
	yMax=1
	if [ -n "${MULTIREGION}" ];then
	    yMin=10;
	    yMax=-10;
	fi
	;;
    sigmaCB)
	columns=6-7
	yVarName="#sigma_{CB} [GeV/c^{2}]"
	;;
    sigmaCBoverPeakCB|scaledWidth)
	columns=8-10
	yVarName="#sigma_{CB}/peak_{CB} [%]"
	case $xVar in 
	    unixTime)
		yVarName="#sigma_{CB} / M_{Z} (%)"
		;;
	esac
	yMin=1
	yMax=4
	;;
    chi2)
	columns=11-13
	yVarName="#sigma_{CB}/peak_{CB} [%]"
#	case $xVar in 
#	    unixTime)
#		yVarName="#sigma_{CB} / M_{Z} (%)"
#		;;
#	esac
	yMin=0
	yMax=10
	;;
    selEff)
	columns=13
	yVarName="nEvents * pb"
	yMin=1
	yMax=0
	;;
    *)
	echo "[ERROR] yVar not defined" >> /dev/stderr
	exit 1
	;;
esac

case $xVar in
    unixTime)
	xVarName="date (day/month)"
	;;
    runNumber)
	xVarName=runNumber
	;;
    absEta)
	xVarName="|#eta|"
	;;
    absEtaSingleEle)
        xVarName="|#eta of one electron|"
        ;;
    nPV)
	xVarName="nVtx"
	;;
   invMassRelSigma*)
        xVarName="#sigma_{M}/M"
        ;;
    ADC)
	xVarName="scale shift [%]"
	;;
    nHitsSCEle)
	xVarName="num Hits SC"
	;;
    Et)
	xVarName="Et (GeV)"
#	yMin=0.994
#	yMax=1.008
	yMin=0.999
	yMax=1.002

	;;
esac

if [ ! -e "tmp/stability/dat/" ]; then 
    mkdir -p tmp/stability/dat
else
    rm tmp/stability -Rf
    mkdir -p tmp/stability/dat
fi

cat > tmp/stability_macro.C <<EOF

{
  //gROOT->ProcessLine(".L src/setTDRStyle.C+");
  gROOT->ProcessLine(".L macro/stability.C+");

  //setTDRStyle();
  //tdrStyle->SetOptTitle(0);
  //gROOT->SetStyle("tdrStyle");  
  TCanvas *c;

std::vector<TString> filenameList;
std::vector<TString> labelList;
  //============================== 
EOF

index=0
for TABLEFILE in $TABLEFILES
  do
  if [ ! -r "${TABLEFILE}" ];then
      echo "[ERROR] Table file ${TABLEFILE} not found or not readable" >> /dev/stderr
      exit 1
  fi
  echo $TABLEFILE
  tmpFile=tmp/stability/tmpFile-${index}.tex
  tmpFileDat=tmp/stability/dat/tmpFile-${index}.dat
  runListFile=tmp/stability/runList-${index}.dat
  runListSed=tmp/stability/run2time-${index}.sed
  echo $tmpFile
  echo $tmpFileDat
  case $xVar in
      Et)
	  grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | grep scale | sed 's|scale_||;s|=|\& 0 \& -- \& -- \&|;s|+/-|\\pm|;s|L(.*)| \\\\|;s|//.*||' |  sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" | cut -d '&' -f 1,2,$columns > $tmpFile
	  ;;
    unixTime)
	grep -v '#' $TABLEFILE | grep -v '^%' | grep runNumber | sed "s|[-]*runNumber_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\trunNumber\t\1\t\2|;s|^\trunNumber|noname\trunNumber|;s|^-||" |cut -d '&' -f 1,2,$columns  > $tmpFile
	cut -d '&' -f 1 $tmpFile | sed 's|.*runNumber\t\([0-9]*\)\t\([0-9]*\)|\1-\2|' |sort | uniq > ${runListFile}
	for line in `cat ${runListFile}`; do grep $line data/runRanges/*.dat | sed -r 's|[ ]+|\t|g;s|[\t]+|\t|g' | cut -f 1,3 | cut -d ':' -f 2|  awk '(NF>1){print $0}' |sort | uniq | sed 's/\([0-9]*\)-\([0-9]*\)\t\([0-9]*\)-\([0-9]*\)/s|runNumber\t\1\t\2|unixTime\t\3\t\4|/'; done > ${runListSed}
	sed -i -f ${runListSed} $tmpFile
	;;
    *)
        grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > $tmpFile
	;;
esac
  NF=`head -1 $tmpFile | awk -F '&' '{print NF}'`
  case $NF in
      5)
	  ;;
      4)
	  sed -i 's|\\\\$| \& -- \\\\|' $tmpFile
	  ;;
      3)
	  sed -i 's|\\\\$| \& -- \& -- \\\\|' $tmpFile
	  ;;
  esac

  ./script/tex2txt.sh $tmpFile |  sed 's| $||;s|--|0\t0|g' > $tmpFileDat # the double 0\t0 is num+-err
  cp $tmpFileDat tmp/tmpFile_tmp.dat  

# remove the values for the x axis out of xMin <= x <= xMax range
  if [ -n "$xMin" -a -n "$xMax" ];then
      awk "(\$1!=\"#\" && \$3>=$xMin && \$4<=$xMax){print \$0}" tmp/tmpFile_tmp.dat > $tmpFileDat
  elif [ -n "$xMin" ];then
      awk "(\$1!=\"#\" && \$3>=$xMin){print \$0}" tmp/tmpFile_tmp.dat > $tmpFileDat
  elif [ -n "$xMax" ];then
      awk "(\$1!=\"#\" && \$4<=$xMax){print \$0}" tmp/tmpFile_tmp.dat > $tmpFileDat
  fi

  cat >> tmp/stability_macro.C<<EOF
  //------------------------------ 
filenameList.push_back("${tmpFileDat}");
labelList.push_back("${LEGENDS[$index]}");
//labelList.push_back("${region}");
EOF
  let index=${index}+1

done


if [ -n "${REGIONGREP}" ];then
    regions=`cat tmp/stability/dat/*.dat | cut -f 1 | sort | uniq |grep ${REGIONGREP}`
    REGIONGREP="-${REGIONGREP}"
else
    regions=`cat tmp/stability/dat/*.dat | cut -f 1 | sort | uniq`
fi

if [ -n "${MULTIREGION}" ];then
    cat >> tmp/stability_macro.C<<EOF
  //------------------------------ 
	filenameList.clear();
labelList.clear();
EOF
    index=0
    for region in $regions
      do
      for tmpFileDat in tmp/stability/dat/*.dat
	do
	tmpFileDatNew=tmp/stability/`basename $tmpFileDat .dat`-${region}.dat
	if [ -z "${NORM}" ];then
	    grep -P "${region}\t" $tmpFileDat > ${tmpFileDatNew}	
	else
	    grep -P "${region}\t" $tmpFileDat |awk '(NR==1){a=$6};(NF!=0){print $1,$2,$3,$4,$5,$6/a,$7,$8,$9,$10,$11}'  > ${tmpFileDatNew}
	fi
	
	cat >> tmp/stability_macro.C<<EOF
  filenameList.push_back("${tmpFileDatNew}");
EOF
	if [ -n "${LEGENDS[0]}" ];then
	    cat >> tmp/stability_macro.C<<EOF
  //labelList.push_back("${region}");
  labelList.push_back("${LEGENDS[$index]}");
EOF
	else
	    cat >> tmp/stability_macro.C<<EOF
labelList.push_back("${region}");
EOF
	fi
	let index=$index+1
      done
    done
#else
# cat >> tmp/stability_macro.C<<EOF
#   //------------------------------ 
# 	//filenameList.clear();
# labelList.clear();
# EOF
#     index=0
#     for region in $regions
#       do
# 	    cat >> tmp/stability_macro.C<<EOF
# labelList.push_back("${region}");
# EOF
# 	    let index=$index+1
#     done
fi

if [ -n "${MULTIREGION}" ];then
      cat >> tmp/stability_macro.C<<EOF
  //------------------------------ 
  c = var_Stability(filenameList, labelList, "",$yMin,$yMax, $color, $column, "$xVarName", "$yVarName");
  c->SaveAs("${outDirImgData}/${yVar}_vs_${xVar}${REGIONGREP}.png");
  c->SaveAs("${outDirImgData}/${yVar}_vs_${xVar}${REGIONGREP}.pdf");
  delete c;

EOF

  else 
    for region in $regions
      do
      cat >> tmp/stability_macro.C<<EOF
  //------------------------------ 
  c = var_Stability(filenameList, labelList, "${region}",$yMin,$yMax, $color, $column, "$xVarName", "$yVarName");
  c->SaveAs("${outDirImgData}/${yVar}_vs_${xVar}-${region}.png");
  c->SaveAs("${outDirImgData}/${yVar}_vs_${xVar}-${region}.pdf");
  delete c;

EOF
    done
fi
  
cat >> tmp/stability_macro.C<<EOF

}
EOF


if [ -z "$NOPLOT" ];then
    root -l -b -q tmp/stability_macro.C
fi

exit 0

exit 0    

echo  >  tmp/stability_sum_table-$yVar.tex
  
  echo -n -e "$region\t&\t" >> tmp/stability_sum_table-$yVar.tex
  grep -P "$region\t" tmp/tmpFile.dat | awk '{n+=1;sum[6]+=$6; sum2[6]+=$6*$6; sum[7]+=$7;};END{printf("%.2f & %.2f & %.2f \\\\\n", sum[6]/n, sqrt(sum2[6]/n-(sum[6]/n)*(sum[6]/n)), sum[7]/n)}' >> tmp/stability_sum_table-$yVar.tex


if [ -n "${TABLEFILETWO}" ];then
    tableTwo=tmp/tmpFile2.dat
fi

done

if [ -z "$NOPLOT" ];then
    root -l -b -q tmp/stability_macro.C
fi

exit 0
case $xVar in
    unixTime)
	grep -v '%' $TABLEFILE | grep -v '#' | cut -d '&' -f 1 | sed 's|.*runNumber_\([0-9]*\)_\([0-9]*\)|\1-\2|' |sort | uniq > tmp/runList.txt
        if [ -n "${TABLEFILETWO}" ];then 
	    grep -v '%' $TABLEFILETWO | grep -v '#' | cut -d '&' -f 1 | sed 's|.*runNumber_\([0-9]*\)_\([0-9]*\)|\1-\2|' |sort | uniq >> tmp/runList.txt
	    cat tmp/runList.txt | sort | uniq > tmp/runList_tmp.txt
	    mv tmp/runList_tmp.txt tmp/runList.txt
	fi
	for line in `cat tmp/runList.txt`; do grep $line data/runRanges/*.dat | sed -r 's|[ ]+|\t|g;s|[\t]+|\t|g' | cut -f 1,3 | cut -d ':' -f 2|  awk '(NF>1){print $0}' |sort | uniq | sed 's/\([0-9]*\)-\([0-9]*\)\t\([0-9]*\)-\([0-9]*\)/s|runNumber_\1_\2|unixTime_\3_\4|/'; done > sed/run2time.sed
        grep -v '#' $TABLEFILE | grep -v '^%' | sed -f sed/run2time.sed | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;" |cut -d '&' -f 1,2,$columns | sed -r 's|[ \t]+|\t|g;s|[\t]+|\t|g' > tmp/tmpFile.tex
        if [ -n "${TABLEFILETWO}" ];then 
            grep -v '#' $TABLEFILETWO | grep -v '^%' | sed -f sed/run2time.sed | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;" |cut -d '&' -f 1,2,$columns | sed -r 's|[ \t]+|\t|g;s|[\t]+|\t|g' > tmp/tmpFile2.tex
        fi
	;;
    absEta*)
        grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile.tex
        if [ -n "${TABLEFILETWO}" ];then 
            grep -v '#' $TABLEFILETWO | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile2.tex
        fi
        ;;
    nPV|invMassRelSigma*)
        grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile.tex
        if [ -n "${TABLEFILETWO}" ];then 
            grep -v '#' $TABLEFILETWO | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile2.tex
        fi
        ;;
    ADC)
        grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^ ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile.tex
        if [ -n "${TABLEFILETWO}" ];then 
            grep -v '#' $TABLEFILETWO | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile2.tex
        fi
        ;;
    runNumber)
        grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile.tex
        if [ -n "${TABLEFILETWO}" ];then 
            grep -v '#' $TABLEFILETWO | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile2.tex
        fi
        ;;
    *)
        grep -v '#' $TABLEFILE | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile.tex
        if [ -n "${TABLEFILETWO}" ];then 
            grep -v '#' $TABLEFILETWO | grep -v '^%' | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||" |cut -d '&' -f 1,2,$columns  > tmp/tmpFile2.tex
        fi
	;;

esac

#take the categories from the table
#category=$1-$2_$3_$4
# cat tmp/tmpFile.tex | awk '(NF!=0){printf("%s-%s_%s_%s\n" ,$1,$2,$3,$4)}' | sed "s|noname-||;s|$|-$commonCut|" > tmp/categoryList.txt
#take the xVar variable
#./script/GetMean.sh -x $xVar -f data/regions/test.dat --commonCut=$commonCut
#for each category take the mean of the variable distribution
#add it as column in the tmpFile.dat
#

./script/tex2txt.sh tmp/tmpFile.tex |  sed 's| $||;s|--|0\t0|' > tmp/tmpFile.dat
cp tmp/tmpFile.dat tmp/tmpFile_tmp.dat
# remove the values for the x axis out of xMin <= x <= xMax range
if [ -n "$xMin" -a -n "$xMax" ];then
    awk "(\$1!=\"#\" && \$3>=$xMin && \$4<=$xMax){print \$0}" tmp/tmpFile_tmp.dat > tmp/tmpFile.dat
elif [ -n "$xMin" ];then
    awk "(\$1!=\"#\" && \$3>=$xMin){print \$0}" tmp/tmpFile_tmp.dat > tmp/tmpFile.dat
elif [ -n "$xMax" ];then
    awk "(\$1!=\"#\" && \$4<=$xMax){print \$0}" tmp/tmpFile_tmp.dat > tmp/tmpFile.dat
fi

if [ -n "${TABLEFILETWO}" ];then 
    ./script/tex2txt.sh tmp/tmpFile2.tex |  sed 's| $||;s|--|0\t0|' > tmp/tmpFile2.dat
    cp tmp/tmpFile2.dat tmp/tmpFile2_tmp.dat
    # remove the values for the x axis out of xMin <= x <= xMax range
    if [ -n "$xMin" -a -n "$xMax" ];then
	awk "(\$1!=\"#\" && \$3>=$xMin && \$4<=$xMax){print \$0}" tmp/tmpFile2_tmp.dat > tmp/tmpFile2.dat
    elif [ -n "$xMin" ];then
	awk "(\$1!=\"#\" && \$3>=$xMin){print \$0}" tmp/tmpFile2_tmp.dat > tmp/tmpFile2.dat
    elif [ -n "$xMax" ];then
	awk "(\$1!=\"#\" && \$4<=$xMax){print \$0}" tmp/tmpFile2_tmp.dat > tmp/tmpFile2.dat
    fi
fi

regions=`cat tmp/tmpFile.dat | cut -f 1 | sort | uniq`
    
cat > tmp/stability_macro.C <<EOF

{
  gROOT->ProcessLine(".L src/setTDRStyle.C+");
  gROOT->ProcessLine(".L macro/stability.C+");

  setTDRStyle();
  tdrStyle->SetOptTitle(0);
   gROOT->SetStyle("tdrStyle");  
  TCanvas *c;

  //============================== PEAK STABILITY 
EOF

echo  >  tmp/stability_sum_table-$yVar.tex
for region in $regions
  do
  
  echo -n -e "$region\t&\t" >> tmp/stability_sum_table-$yVar.tex
  grep -P "$region\t" tmp/tmpFile.dat | awk '{n+=1;sum[6]+=$6; sum2[6]+=$6*$6; sum[7]+=$7;};END{printf("%.2f & %.2f & %.2f \\\\\n", sum[6]/n, sqrt(sum2[6]/n-(sum[6]/n)*(sum[6]/n)), sum[7]/n)}' >> tmp/stability_sum_table-$yVar.tex


if [ -n "${TABLEFILETWO}" ];then
    tableTwo=tmp/tmpFile2.dat
fi

  cat >> tmp/stability_macro.C<<EOF
  //------------------------------ 
  c = var_Stability("tmp/tmpFile.dat", "${region}",$yMin,$yMax, $color, $column, "$xVarName", "$yVarName", "$tableTwo", "$titleOne", "$titleTwo");
  c->SaveAs("${outDirImgData}/${yVar}_vs_${xVar}-${region}.png");
  c->SaveAs("${outDirImgData}/${yVar}_vs_${xVar}-${region}.pdf");
  delete c;

EOF
done

cat >> tmp/stability_macro.C<<EOF

}
EOF


