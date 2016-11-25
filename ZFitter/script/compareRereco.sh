#!/bin/bash
OUTDIR=comparison

usage(){
    echo "`basename $0`  tablefiles"
    echo "  -e: no error"
    echo "  --outdir"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o he -l help,column:,outdir:,plusMC,noerror -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-e) SYNERROR=y;; 
	--noerror) NOERROR=y;;
	--outdir) OUTDIR=$2; shift;;
	--plusMC) plusMC=y;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done
mkdir -p $OUTDIR
files=""
for file in $@
do
echo $file
sort $file -o $file
files="$files $file"
done

it=0;
array=( 2 3 5 6 8 10 11 15 )
for it in "${array[@]}"
do

# !ds = dollar sign $ (sub later, sed gets confused with escapable chars)
# !bs = backslash sign \ (sub later)
# !us = backslash sign _ (sub later)
case $it in 
    2)
	cName="events"
	;;
    3|4)
	cName="!ds !bsDelta m!ds"
	;;
    5)
	cName="!ds !bsDelta P !ds"
	;;
    6)
	cName="!ds !bssigma!us{CB}!ds"
	;;
    8|9)
	cName="!ds!bsresol !ds"
	;;
    10)
	cName="add. smear."
	;;
    11)
	cName="!ds !bschi^2 !ds."
	;;
    15)
	cName="!ds !bssigma!us{eff} !ds"
	;;
    *)
	exit 1
	;;
esac


tmpFile=$OUTDIR/slide$it.tex
./script/compareColumns.sh -e  --column $it $files
cp tex/compareSlides.tex $tmpFile 
sed -i "/_TABLE_/ r tmp/file.tex" $tmpFile
sed -i '/_TABLE_/ d' $tmpFile
sed -i "s|0poiname|$cName|g " $tmpFile
sed -i "s|!us|_|g " $tmpFile
sed -i "s|!bs|\\\|g " $tmpFile
sed -i "s|!ds|$|g " $tmpFile

done
#cat $tmpFile

cat > $OUTDIR/slide0.tex <<EOF
\begin{comment}

\end{comment}

\makeatletter
\@ifundefined{dataSample}{
  \newcommand{\dataSample}{$dataSample}
  \newcommand{\mcSample}{$mcSample}
  \newcommand{\imgDir}{$dirData/$selection/$invMass_var/img}
  \newcommand{\PeriodDivisionTickzOld}{}
  \newcommand{\PeriodDivisionTickzNew}{}
  \newcommand{\PeriodDivisionTickz}{}
 \newcommand{\period}{$PERIOD}
}{
  %\renewcommand{\dataSample}{$dataSample}
  \renewcommand{\mcSample}{$mcSample}
  \renewcommand{\imgDir}{$dirData/$selection/$invMass_var/img}
  \renewcommand{\period}{$PERIOD}
}
\@ifundefined{invMassVarName}{
  \newcommand{\invMassVarName}{$invMassVarName}
}{
  \renewcommand{\invMassVarName}{$invMassVarName}
}
\@ifundefined{resol}{
  \newcommand{\resol}{\ensuremath{\frac{\sigma_{CB}}{peak_{CB}}}}
}{}

\makeatother

\usebackgroundtemplate{
	\includegraphics[width=\paperwidth,height=\paperheight]{logos/Blasenkammer_by_HenryGale_white}%
}

\section{\invMassVarName}
\frame{\centering\scalebox{2}{\textbf{\textsf{\rotatebox{35}{Rereco comparison tables}}}}}

\usebackgroundtemplate{
	\includegraphics[width=\paperwidth,height=\paperheight]{}%
}



EOF

#cat tex/template.tex $OUTDIR/slide{3,8}.tex $OUTDIR/slide{5,6,10,11,15}.tex tex/template_end.tex > $OUTDIR/comparison.tex
cat tex/template.tex $OUTDIR/slide{8,15,3,11,2,}.tex tex/template_end.tex > $OUTDIR/comparison.tex

pdflatex -interaction nonstopmode -file-line-error $OUTDIR/comparison.tex
