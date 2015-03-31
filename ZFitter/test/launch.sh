#!/bin/bash
#
#data/validation/rereco/AlphaStudies2012/13Jul_Alpha*_v*.dat 
#data/validation/rereco/Cal_Dic2012/Cal_Dic2012_ICcombTCphiSym_v4.dat
#data/validation/rereco/Cal_Dic2012/Cal_Dic2012_ICcombTCcomb_v5_offGT_bis.dat
#data/validation/rereco/Cal_Dic2012/Cal_Dic2012_Alphacomb_v*.dat
#data/validation/monitoring_2012_53X.dat
#data/validation/moriond2013.dat
#data/validation/rereco/ungrouped/Winter13_noLas.dat
#data/validation/rereco/ungrouped/Moriond2013_noLas.dat
#data/validation/rereco/ungrouped/Winter13.dat
#data/validation/rereco/ungrouped/Winter13_noLas.dat
#data/validation/rereco/ungrouped/Moriond2013.dat
for file in data/validation/rereco/ungrouped/Winter13_noLas.dat
  do
  for invMass in  invMass_SC_regrCorr_ele
    do
    rereco=Winter13
#   rereco=Moriond13

    ./script/monitoring_validation.sh -f $file  --invMass_var=$invMass --rereco $rereco --stability --runRangesFile=data/runRanges/runRangePlot.txt
#    ./script/monitoring_validation.sh -f $file  --invMass_var=$invMass --period=RUN2012D  --validation --stability --slide --rereco $rereco 
#    ./script/monitoring_validation.sh -f $file  --invMass_var=$invMass --period=RUN2012C  --validation --stability --slide --rereco $rereco 
#    ./script/monitoring_validation.sh -f $file  --invMass_var=$invMass --period=RUN2012AB --validation --stability --slide --rereco $rereco 
##  ./script/monitoring_validation.sh -f $file  --invMass_var=$invMass --validation --period=RUN2012B
##  ./script/monitoring_validation.sh -f $file  --invMass_var=$invMass --validation --period=RUN2012A
  done
done


