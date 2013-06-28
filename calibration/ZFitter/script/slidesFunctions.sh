#!/bin/bash


validationIntroSlide(){
    echo "[STATUS] Creating validation intro slide: ${dirSlides}/validation-${invMass_var}-${selection}-intro_slide.tex"
    
cat > ${dirSlides}/validation-${invMass_var}-${selection}-intro_slide.tex <<EOF
\begin{comment}

\end{comment}

\makeatletter
\@ifundefined{dataSample}{
  \newcommand{\dataSample}{dataSample}
  \newcommand{\mcSample}{mcSample}
  \newcommand{\imgDir}{$dirData/$selection/$invMass_var/img}
  \newcommand{\PeriodDivisionTickzOld}{}
  \newcommand{\PeriodDivisionTickzNew}{}
  \newcommand{\PeriodDivisionTickz}{}
 \newcommand{\period}{$PERIOD}
}{
  %\renewcommand{\dataSample}{dataSample}
  \renewcommand{\mcSample}{mcSample}
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

\section{\invMassVarName}
\frame{\centering\scalebox{2}{\textbf{\textsf{\rotatebox{35}{\invMassVarName}}}}}


EOF


}


validationTableSlides(){
    echo "[STATUS] Creating validation table slides: ${dirSlides}/validation-${invMass_var}-${selection}-table_slide.tex"
    tmpFile=${dirSlides}/validation-${invMass_var}-${selection}-table_slide.tex 
    cat tex/tableSlides.tex > $tmpFile

    grep -v "#" $dirData/${selection}/${invMass_var}/table/$PERIOD/monitoring_summary-${invMass_var}-${selection}.tex | cut -d '&' -f 1-2,8-10 | awk -F '[&]' -f awk/format.awk | sed -f sed/tex.sed | sed 's|\(.*eta.*R9\)|%\1|;/^EE[ ]*&/ i \\\hline' > tmp/file.tex
    sed -i "/_TABLESIGMANEW_/ r tmp/file.tex" $tmpFile
    sed -i '/_TABLESIGMANEW_/ d' $tmpFile

    grep -v "#" $dirData/${selection}/${invMass_var}/table/$PERIOD/monitoring_summary-${invMass_var}-${selection}.tex | cut -d '&' -f 1-2,6-7 | awk -F '[&]' -f awk/format.awk | sed -f sed/tex.sed | sed 's|\(.*eta.*R9\)|%\1|;/^EE[ ]*&/ i \\\hline' > tmp/file.tex
    sed -i "/_TABLESIGMA_/ r tmp/file.tex" $tmpFile
    sed -i '/_TABLESIGMA_/ d' $tmpFile

    grep -v "#" $dirData/${selection}/${invMass_var}/table/$PERIOD/monitoring_summary-${invMass_var}-${selection}.tex | cut -d '&' -f 1-2,3-5 | awk -F '[&]' -f awk/format.awk | sed -f sed/tex.sed | sed 's|\(.*eta.*R9\)|%\1|;/^EE[ ]*&/ i \\\hline' > tmp/file.tex
    sed -i "/_TABLE_/ r tmp/file.tex" $tmpFile
    sed -i '/_TABLE_/ d' $tmpFile
    sed -i "s|RERECOTAG|$rereco|" $tmpFile
    sed -i "s|PERIOD|$PERIOD|" $tmpFile
    rm tmp/file.tex
}

validationFitSlides(){
    echo "[STATUS] Creating validation fit slides: ${dirSlides}/validation-${invMass_var}-${selection}-fit_slide.tex"
    cat tex/fitSlides.tex > ${dirSlides}/validation-${invMass_var}-${selection}-fit_slide.tex 
}

validationStabilitySlides(){
    tmpFile=${dirSlides}/validation-${invMass_var}-${selection}-stability_slide.tex 
    echo "[STATUS] Creating validation stability slides: ${tmpFile}"
    cat tex/stabilitySlides.tex > $tmpFile
    sed -f sed/tex.sed $dirData/${selection}/${invMass_var}/table/$PERIOD/stability_sum_table.tex > tmp/file.tex
    sed -i "/_TABLE_/ r tmp/file.tex" $tmpFile
    sed -i '/_STABILITY_TABLE_/ d' $tmpFile
    
}


step3stabilitySlides(){
    tmpFile=${dirSlides}/step3stability-${invMass_var}-${selection}-${commonCut}.tex 
    echo "[STATUS] Creating validation stability slides: ${tmpFile}"
    cat > $tmpFile <<EOF
    \makeatletter
    \@ifundefined{imgDirStep}{
      \newcommand{\imgDirStep}{\imgDir/../step3/img}
    }{
      \renewcommand{\imgDirStep}{\imgDir/../step3/img}
    }
    \makeatother
EOF

    cat tex/stabilitySlides.tex | sed 's|imgDir|imgDirStep|g; 291,350 d' >> $tmpFile
#    sed -f sed/tex.sed $dirData/${selection}/${invMass_var}/table/$PERIOD/stability_sum_table.tex > tmp/file.tex
#    sed -i "/_TABLE_/ r tmp/file.tex" $tmpFile
#    sed -i '/_STABILITY_TABLE_/ d' $tmpFile
}    

step3Slides(){
    cat > tmp/step3-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
\subsection{Smearing}
  %% ------------------------------------------------------------
  \color{titlecolor}
  \begin{frame}
    \frametitle{Additional smearings with fit method}
    \framesubtitle{${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'`}
\larger
\alert{    Additional smearings measured with the fit method using the ${invMassVarName} and `echo ${selection}|sed 's|_|\\\\_|'` selection.}
\begin{center}
    \begin{tabular}{|c|c|}
      \hline
      ECAL Region  	& $\sigma_E/E$ (\%)	  \\\\
    			& ${invMassVarName}	  \\\\
%                        & no $R_9$ reweight	  \\\\
      \hline\hline


EOF

 cat $step3File  | cut -d '&' -f 1,10 |awk -F '&' -f awk/format.awk  | sed -f sed/tex.sed | grep 'E[BE]' | grep R9 | grep -v '\\&' | sort >> tmp/step3-${invMass_var}-${selection}-${commonCut}-slide.tex

    cat >> tmp/step3-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF

      \hline
    \end{tabular}
\end{center}

  \end{frame}
EOF
}


step2Slides(){
    cat > tmp/step2-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF


  %==============================
  \subsection{Step 2}

 
 %% ------------------------------------------------------------
  \subsubsection{Corrections}
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 2: \$c_2\$}
    \framesubtitle{${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'`}

    After the application of the \$c_1\$ energy scales, corrections
    based on \$R_9\$ categorization in high/low \$\eta\$ regions  
    are derived (\$c_2\$) 


      \begin{tabular}{|>{}l|c|c|} \hline
	ECAL Region   & \$\Delta\$P (\%) \\\\
	 &   \\\\
	\hline
	\hline

EOF
    grep -v '#'  $step2File |  cut -d '&' -f 1,5 |  awk -F '&' -f awk/format.awk | sed -f sed/tex.sed |  grep  R9>> tmp/step2-${invMass_var}-${selection}-${commonCut}-slide.tex
cat >> tmp/step2-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
\hline
      \end{tabular}

\bl
The \$c_2\$ corrections are derived combining both diagonal and off
diagonal categories.
  \end{frame}


EOF

cat >> tmp/step2-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF

  %% ------------------------------------------------------------
  \subsubsection{Closure test}
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 2: closure test}
    \framesubtitle{${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'`}

    `echo ${selection}|sed 's|_|\\\\_|'` selection, corrections derived using ${invMassVarName}

    \vspace{-5pt}
    \begin{center}
      \begin{tabular}{|>{}l|c|c|} 
	\hline
	ECAL Region of    &  \multicolumn{1}{c|}{\$\Delta\$P (\%)} \\\\
	the two electrons &  \multicolumn{1}{c|}{}       \\\\
	                  &  fit method \\\\
	\hline
	\hline

EOF
grep -v '#' $step3File |  cut -d '&' -f 1,5 |  awk -F '&' -f awk/format.awk | sed -f sed/tex.sed >> tmp/step2-${invMass_var}-${selection}-${commonCut}-slide.tex

cat >> tmp/step2-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
	\hline 
      \end{tabular}
    \end{center}



  \end{frame}


EOF

}

step1Slides(){

    cat > tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF

  %% ------------------------------------------------------------
  %============================================================
  %==============================
  \subsection{Step 1}
  %----------------------------------------
  %% ------------------------------------------------------------
  \subsubsection{Corrections}
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 1: \$c_1\$}
    \framesubtitle{EB \$\abs{\eta} < 1 \$}

    Corrections derived dividing into high/low eta regions and run range.

    Invariant mass evaluated using ${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'` selection.

    \smaller\bl
      \begin{tabular}{|l|c|c|c|>{\bfseries \color{red}}c|} 
	\hline
	ECAL Region  & run range & \$\Delta m_{data}\$ & \$\Delta m_{MC}\$  & \$\Delta\$P (\%) \\\\
	 & & (GeV) & (GeV) & \\\\
	\hline
	\hline
EOF


    grep EB-absEta_0_1 $step1File |  grep -v '#' | cut -d '&' -f 1,3-5 | sed -f sed/tex.sed | awk -F '&' -f awk/format.awk | sed 's|\[|\& [|' >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
 \hline
       \end{tabular}
   \end{frame}

EOF

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF

  %% ------------------------------------------------------------
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 1: \$c_1\$}
    \framesubtitle{EB \$\abs{\eta} > 1 \$}

    Corrections derived dividing into high/low eta regions and run range.

    Invariant mass evaluated using ${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'` selection.

    \smaller\bl
      \begin{tabular}{|l|c|c|c|>{\bfseries \color{red}}c|} 
	\hline
	ECAL Region  & run range & \$\Delta m_{data}\$ & \$\Delta m_{MC}\$  & \$\Delta\$P (\%) \\\\
	 & & (GeV) & (GeV) & \\\\
	\hline
	\hline
EOF


    grep EB-absEta_1_1.4442 $step1File |  grep -v '#' | cut -d '&' -f 1,3-5 | sed -f sed/tex.sed | awk -F '&' -f awk/format.awk | sed 's|\[|\& [|' >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
 \hline
       \end{tabular}
   \end{frame}

EOF

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF


  %% ------------------------------------------------------------
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 1: \$c_1\$}
    \framesubtitle{EE \$\abs{\eta} <2\$}

    Corrections derived dividing into high/low eta regions and run range.

    Invariant mass evaluated using ${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'` selection.

    \smaller\bl
      \begin{tabular}{|l|c|c|c|>{\bfseries \color{red}}c|} 
	\hline
	ECAL Region  & run range & \$\Delta m_{data}\$ & \$\Delta m_{MC}\$  & \$\Delta\$P (\%) \\\\
	 & & (GeV) & (GeV) & \\\\
	\hline
	\hline
EOF


    grep EE-absEta_1.566_2 $step1File |  grep -v '#' | cut -d '&' -f 1,3-5 | sed -f sed/tex.sed | awk -F '&' -f awk/format.awk | sed 's|\[|\& [|' >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
 \hline
       \end{tabular}
   \end{frame}

EOF

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF


  %% ------------------------------------------------------------
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 1: \$c_1\$}
    \framesubtitle{EE \$\abs{\eta} >2\$}

    Corrections derived dividing into high/low eta regions and run range.

    Invariant mass evaluated using ${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'` selection.

    \smaller\bl
      \begin{tabular}{|l|c|c|c|>{\bfseries \color{red}}c|} 
	\hline
	ECAL Region  & run range & \$\Delta m_{data}\$ & \$\Delta m_{MC}\$  & \$\Delta\$P (\%) \\\\
	 & & (GeV) & (GeV) & \\\\
	\hline
	\hline
EOF


    grep EE-absEta_2_2.5 $step1File |  grep -v '#' | cut -d '&' -f 1,3-5 | sed -f sed/tex.sed | awk -F '&' -f awk/format.awk | sed 's|\[|\& [|' >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
 \hline
       \end{tabular}
   \end{frame}

EOF

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF

  %% ------------------------------------------------------------
  \subsubsection{Closure test}
  \color{titlecolor}
  \begin{frame}
    \frametitle{Step 1: closure test}
    \framesubtitle{${invMassVarName}, `echo ${selection}|sed 's|_|\\\\_|'`}

      \$\$ \Delta P = \frac{\Delta m_{data} - \Delta m_{MC}}{m_{Z}}\$\$

    After the application of the \$c_1\$ energy scales:

    \begin{center}
      \begin{tabular}{|l|*{2}{c|}|>{\bf \color{red}}c|} \hline
	ECAL Region of & \$\Delta m_{data}\$ & \$\Delta m_{MC}\$  & \$\Delta\$P (\%) \\\\
	the two electrons & (GeV) & (GeV) & \\\\
	\hline
	\hline
EOF

    grep -v '#' $step2File |  cut -d '&' -f 1,3-5 | sed -f sed/tex.sed | awk -F '&' -f awk/format.awk | sed 's|\[|\& [|' | grep -v R9>> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex

    cat >> tmp/step1-${invMass_var}-${selection}-${commonCut}-slide.tex <<EOF
 	\hline
       \end{tabular}
     \end{center}

   \end{frame}

EOF

}



