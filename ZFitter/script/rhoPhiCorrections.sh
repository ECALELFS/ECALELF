#!/bin/bash
dir=test/dato/22Jan2012-runDepMCAll_v2/loose/invMass_SC_regrCorrSemiParV5_pho/step9/

 grep "profile for alpha" $dir/*/fitres/*-scaleStep2smearing_9-stdout.log | awk '($6==-1 && $8!=0){n[$5]++; sumRho[$5]+=$6; sum2Rho[$5]+=$6*$6; sumEmean[$5]+=$9; sum2Emean[$5]+=$9*$9};END{for( cat in n){meanRho=sumRho[cat]/n[cat]; meanEmean=sumEmean[cat]/n[cat]; print cat, meanEmean, sqrt(sum2Emean[cat]/n[cat]-meanEmean*meanEmean)}}' |sed 's|alpha_|Emean |' | sort -k 2 > tmp/emean.dat

 cat `dirname $dir`/table/step9*.dat | grep phi | grep -v 'rho_phi' |sed 's|phi_|phi |' | sort -k 2 >tmp/phi.dat
 cat `dirname $dir`/table/step9*.dat | grep rho | grep -v 'rho_phi' |sed 's|rho_|rho |' | sort -k 2  > tmp/rho.dat

cat tmp/emean.dat
cat tmp/phi.dat
cat tmp/rho.dat

 paste tmp/rho.dat tmp/phi.dat tmp/emean.dat | sed 's|=||g;s|+/-||g;s|L([-.0-9 ]*)||g' | awk '($2==$6 && $2==$10){print $2,$3,$4,$7,$8,$11,$12}' | awk -f awk/rhoPhiConversion.awk


#  cat `dirname $dir`/table/outFile-step9*.dat
#cat `dirname $dir`/table/step9*.dat 